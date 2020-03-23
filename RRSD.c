#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include "my_io.h"
#include "mythread.h"
#include "interrupt.h"
#include "queue.h"

TCB* scheduler();
void activator();
void timer_interrupt(int sig);
void disk_interrupt(int sig);

int apanyo=0;

/* Array of state thread control blocks: the process allows a maximum of N threads */
static TCB t_state[N]; 

struct queue *q_low;
struct queue *q_high;    //Colas para la planificación RR sin prioridades
struct queue *q_disk;

/* Current running thread */
static TCB* running;
static int current = 0;

/* Variable indicating if the library is initialized (init == 1) or not (init == 0) */
static int init=0;

/* Thread control block for the idle thread */
static TCB idle;

static void idle_function()
{
  while(1);
}

void function_thread(int sec)
{
  //time_t end = time(NULL) + sec;
  while(running->remaining_ticks){}
  mythread_exit();
}

/* Initialize the thread library */
void init_mythreadlib() 
{
  int i;

  /* Create context for the idle thread */
  if(getcontext(&idle.run_env) == -1)
  {
    perror("*** ERROR: getcontext in init_thread_lib");
    exit(-1);
  }

  idle.state = IDLE;
  idle.priority = SYSTEM;
  idle.function = idle_function;
  idle.run_env.uc_stack.ss_sp = (void *)(malloc(STACKSIZE));
  idle.tid = -1;

  if(idle.run_env.uc_stack.ss_sp == NULL)
  {
    printf("*** ERROR: thread failed to get stack space\n");
    exit(-1);
  }

  idle.run_env.uc_stack.ss_size = STACKSIZE;
  idle.run_env.uc_stack.ss_flags = 0;
  idle.rodaja = QUANTUM_TICKS;
  idle.ticks = 0;
  makecontext(&idle.run_env, idle_function, 1); 

  t_state[0].state = INIT;
  t_state[0].priority = LOW_PRIORITY;
  t_state[0].rodaja = QUANTUM_TICKS;
  t_state[0].ticks = 0;

  if(getcontext(&t_state[0].run_env) == -1)
  {
    perror("*** ERROR: getcontext in init_thread_lib");
    exit(5);
  }	

  for(i=1; i<N; i++) t_state[i].state = FREE;

  t_state[0].tid = 0;
  running = &t_state[0];
 
  //Se inicializa la cola
  q_low = queue_new();
  q_high = queue_new();
  q_disk = queue_new();

  /* Initialize disk and clock interrupts */
  init_disk_interrupt();
  init_interrupt();
}

/* Create and intialize a new thread with body fun_addr and one integer argument */ 
int mythread_create (void (*fun_addr)(),int priority,int seconds)
{
  int i;
  
  if (!init) {
    init_mythreadlib();
    init=1;
  }

  for (i=0; i<N; i++) if (t_state[i].state == FREE) break;
  if (i == N) return(-1);
  if(getcontext(&t_state[i].run_env) == -1)
  {
    perror("*** ERROR: getcontext in my_thread_create");
    exit(-1);
  }

  t_state[i].state = INIT;
  t_state[i].priority = priority;
  t_state[i].function = fun_addr;
  t_state[i].execution_total_ticks = seconds_to_ticks(seconds);
  t_state[i].remaining_ticks = t_state[i].execution_total_ticks;
  t_state[i].rodaja = QUANTUM_TICKS; //parece ser que 20ticks son 100 ms
  t_state[i].ticks=0;
  t_state[i].run_env.uc_stack.ss_sp = (void *)(malloc(STACKSIZE));
  
  if(t_state[i].run_env.uc_stack.ss_sp == NULL)
  {
    printf("*** ERROR: thread failed to get stack space\n");
    exit(-1);
  }

  t_state[i].tid = i;
  t_state[i].run_env.uc_stack.ss_size = STACKSIZE;
  t_state[i].run_env.uc_stack.ss_flags = 0;
  makecontext(&t_state[i].run_env, fun_addr,2,seconds);

  if(t_state[i].priority == LOW_PRIORITY) {
    disable_interrupt();
    disable_disk_interrupt();
    enqueue(q_low, &t_state[i]);
    enable_disk_interrupt();
    enable_interrupt();
  }
  else if(t_state[i].priority == HIGH_PRIORITY) {
    if(running->priority == LOW_PRIORITY){
      disable_interrupt();
      disable_disk_interrupt();
      running->rodaja=QUANTUM_TICKS;
      enqueue(q_low, running);
      sorted_enqueue(q_high, &t_state[i], t_state[i].remaining_ticks);
      enable_disk_interrupt();
      enable_interrupt();
      TCB* next = scheduler(); 
      activator(next);
      return i;
    }
    if(t_state[i].remaining_ticks < running->remaining_ticks){ //expulsar al proceso si se mete uno que dura menos
      disable_interrupt();
      disable_disk_interrupt();
      sorted_enqueue(q_high, running, running->remaining_ticks);
      enable_disk_interrupt();
      enable_interrupt();
      activator(&t_state[i]);
    }else{// sino todo sigue su cauce
      disable_interrupt();
      disable_disk_interrupt();
      sorted_enqueue(q_high, &t_state[i], t_state[i].remaining_ticks);
      enable_disk_interrupt();
      enable_interrupt();
    }
  }
  return i;
} 

int read_disk()
{
  int ret = data_in_page_cache();
    
  if (ret != 0) {
    int tid = mythread_gettid();
    t_state[tid].state = WAITING;
    enqueue(q_disk, &t_state[tid]);
    printf("*** THREAD %d READ FROM DISK\n", current);

    activator(scheduler());
  }
  return 1;
}

void disk_interrupt(int sig)
{
  if(queue_empty(q_disk) != 1) {
    TCB* tdisk = dequeue(q_disk);
    tdisk->state=INIT;

    if(tdisk->priority == LOW_PRIORITY) {
      disable_interrupt();
      disable_disk_interrupt();
      enqueue(q_low, tdisk);
      enable_disk_interrupt();
      enable_interrupt();
    } else if(tdisk->priority == HIGH_PRIORITY) {
      disable_interrupt();
      disable_disk_interrupt();
      enqueue(q_high, tdisk);
      enable_disk_interrupt();
      enable_interrupt();
    }        
    printf("*** THREAD %d READY\n", tdisk->tid);
  }
}

/* Free terminated thread and exits */
void mythread_exit() {
  int tid = mythread_gettid();	

  printf("*** THREAD %d FINISHED\n", tid);	
  t_state[tid].state = FREE;
  free(t_state[tid].run_env.uc_stack.ss_sp); 

  TCB* next = scheduler();
  activator(next);
}

void mythread_timeout(int tid) {
    printf("*** THREAD %d EJECTED\n", tid);
    t_state[tid].state = FREE;
    free(t_state[tid].run_env.uc_stack.ss_sp);

    TCB* next = scheduler();
    activator(next);
}

void mythread_setpriority(int priority) 
{
  int tid = mythread_gettid();	
  t_state[tid].priority = priority;
  if(priority ==  HIGH_PRIORITY) t_state[tid].remaining_ticks = 195;
}

int mythread_getpriority(int priority) 
{
  int tid = mythread_gettid();	
  return t_state[tid].priority;
}

int mythread_gettid(){
  if (!init) {
    init_mythreadlib();
    init=1;
  }
  return current;
}

/* SJF para alta prioridad, RR para baja*/
TCB* scheduler()
{
  if(queue_empty(q_low) == 1 && queue_empty(q_high) == 1) {
    /* No threads waiting */
    if(running->state != FREE) printf("*** THREAD %d FINISHED\n", current);
    if(queue_empty(q_disk) != 1) {
      return &idle;
    }
    printf("FINISH\n");
    exit(1);
  }

  if(queue_empty(q_high) != 1) {
    /* High priority threads waiting to be executed */
    disable_interrupt();
    disable_disk_interrupt();
    TCB* nextH = dequeue(q_high);
    enable_disk_interrupt();
    enable_interrupt();
    return nextH;
  } else {
    /* No high priority threads waiting, execute low priority ones */
    disable_interrupt();
    disable_disk_interrupt();
    TCB* next = dequeue(q_low);
    enable_disk_interrupt();
    enable_interrupt();
    return next;
  }  
}

void timer_interrupt(int sig)
{
  if(QUANTUM_TICKS > running->remaining_ticks) running->rodaja=running->remaining_ticks;
  if(running->ticks > running->execution_total_ticks) mythread_timeout(running->tid);
  if(running->priority== HIGH_PRIORITY){
    running->ticks++;
    running->remaining_ticks--;    
  }
  if(running->priority == LOW_PRIORITY){
    running->ticks++;   //Se reduce un tick por cada timer_interrupt
    running->rodaja--;
    running->remaining_ticks--;
    if (running->rodaja == 0){   //Se comprueba si ha terminado y si la prioridad es baja
      running->rodaja = QUANTUM_TICKS;
      disable_interrupt();  //Se protege de posibles interrupciones
      disable_disk_interrupt();
      enqueue(q_low, running);
      enable_disk_interrupt();
      enable_interrupt();
      TCB* next = scheduler();    //Se obtiene el siguiente proceso
      activator(next);
    }
  }  
} 

void activator(TCB* next)
{
  TCB *procesoActual = running;
  current = next->tid;
  running = next;
  if(procesoActual == next) return;
  if(running->state == IDLE) {
    printf("*** THREAD READY: SET CONTEXT TO %d\n", next->tid);
    setcontext(&(next->run_env));
  }
  if (procesoActual->state == FREE){ /*Si el proceso en marcha termina imprimimos por pantalla y ponemos el contexto del nuevo */
    printf("*** THREAD %d TERMINATED: SETCONTEXT OF %d\n", procesoActual->tid, next->tid); 
    //El scheduler ya devuelve el proceso de prioridad que toque, a si que solo lo ponemos a ejecutar
    setcontext(&(next->run_env));/* se pone el contexto del nuevo*/
  } else {
    if (procesoActual->priority == LOW_PRIORITY && next->priority == HIGH_PRIORITY){ /*si el thread actual es de baja y el siogoente es de alta imprimimos un mensaje, y si son los dos baja, uno distinto*/
      printf("*** THREAD %d PREEMTED: SETCONTEXT OF %d\n", procesoActual->tid, next->tid);
    }
    if(procesoActual->priority == LOW_PRIORITY && next->priority == LOW_PRIORITY) {
      printf("*** SWAPCONTEXT FROM %d TO %d\n", procesoActual->tid, next->tid);
    }
    if (swapcontext(&(procesoActual->run_env), &(next->run_env)) == -1){
      printf("mythread_free: After setcontext, should never get here!!...");
    }
  }
}
