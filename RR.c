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


/* Array of state thread control blocks: the process allows a maximum of N threads */
static TCB t_state[N]; 

struct queue *q;    //Cola para la planificación RR sin prioridades

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
    while(running->remaining_ticks)
    {
      //do something
    }
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
  idle.ticks = QUANTUM_TICKS;
  makecontext(&idle.run_env, idle_function, 1); 

  t_state[0].state = INIT;
  t_state[0].priority = LOW_PRIORITY;
  t_state[0].ticks = QUANTUM_TICKS;

  if(getcontext(&t_state[0].run_env) == -1)
  {
    perror("*** ERROR: getcontext in init_thread_lib");
    exit(5);
  }	

  for(i=1; i<N; i++)
  {
    t_state[i].state = FREE;
  }

  t_state[0].tid = 0;
  running = &t_state[0];
    //Se inicializa la cola
    q = queue_new();

  /* Initialize disk and clock interrupts */
  init_disk_interrupt();
  init_interrupt();
}


/* Create and intialize a new thread with body fun_addr and one integer argument */ 
int mythread_create (void (*fun_addr)(),int priority,int seconds)
{
  int i;
  
  if (!init) { init_mythreadlib(); init=1;}

  for (i=0; i<N; i++)
    if (t_state[i].state == FREE) break;

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
  t_state[i].rodaja = 20; //parece ser que 20ticks son 100 ms
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

    //Se añade el nuevo thread a la cola
    disable_interrupt();
    enqueue(q, &t_state[i]);
    enable_interrupt();
  
  return i;
} 
/****** End my_thread_create() ******/


/* Read disk syscall */
int read_disk()
{
   return 1;
}

/* Disk interrupt  */
void disk_interrupt(int sig)
{

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


/* Sets the priority of the calling thread */
void mythread_setpriority(int priority) 
{
  int tid = mythread_gettid();	
  t_state[tid].priority = priority;
  if(priority ==  HIGH_PRIORITY){
    t_state[tid].remaining_ticks = 195;
  }
}

/* Returns the priority of the calling thread */
int mythread_getpriority(int priority) 
{
  int tid = mythread_gettid();	
  return t_state[tid].priority;
}


/* Get the current thread id.  */
int mythread_gettid(){
  if (!init) { init_mythreadlib(); init=1;}
  return current;
}


/* SJF para alta prioridad, RR para baja*/

TCB* scheduler()
{
//   int i;
//   for(i=0; i<N; i++)
//   {
//     if (t_state[i].state == INIT) 
//     {
//       current = i;
// 	    return &t_state[i];
//     }
//   }
//   printf("mythread_free: No thread in the system\nExiting...\n");	
//   exit(1);
    
    if(queue_empty(q) == 1) {
        //Si la cola está vacía significa que no quedan threads por planificar
        printf("mythread_free: No thread in the system\nExiting...\n");	
        exit(1);
    }

    //Se desencola el siguiente thread de la cola y se devuelve como resultado
    disable_interrupt();
    TCB* next = dequeue(q);
    enable_interrupt();

    return next;
}


/* Timer interrupt */
void timer_interrupt(int sig)
{
    running->ticks++;   //Se reduce un tick por cada timer_interrupt
    running->rodaja--;
    running->remaining_ticks--;
    if (running->rodaja == 0){
        running->rodaja = 20;
        disable_interrupt();  //Se protege de posibles interrupciones
        enqueue(q, running);
        enable_interrupt();
        TCB* next = scheduler();    //Se obtiene el siguiente proceso
        activator(next);
    }
} 

/* Activator */
void activator(TCB* next)
{
    //Si el siguiente thread es el mismo que el actual no se hace nada
    if(running == next) return;
    if(running->state == FREE) {
        //Thread terminado, se hace setcontext al siguiente
        printf("*** THREAD %d TERMINATED: SET CONTEXT OF %d\n", running->tid,  next->tid);
        
        running = next;
        current = next->tid;
        
        setcontext(&(next->run_env));
        printf("mythread_free: After setcontext, should never get here!!...\n");
    } else if(running != next) {
        //Se debe hacer un swapcontext al siguiente proceso para no perder el actual
        TCB* aux;
        aux= running;
        running = next;
        current = next->tid;
        printf("*** SWAPCONTEXT FROM %d TO %d\n", aux->tid, next->tid);
        swapcontext(&(aux->run_env), &(next->run_env));
    }
}
