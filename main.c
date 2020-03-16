#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>

#include "mythread.h"

//Each thread executes this function
extern void function_thread(int sec);

int main(int argc, char *argv[])
{
  int j,k,l,m,a,b,f,p;

  mythread_setpriority(LOW_PRIORITY);
  if((f = mythread_create(function_thread,LOW_PRIORITY, 5)) == -1){
      printf("thread failed to initialize\n");
      exit(-1);
  }

  if((f = mythread_create(function_thread,HIGH_PRIORITY, 5)) == -1){
      printf("thread failed to initialize\n");
      exit(-1);
  }
  read_disk();
      
     
  for (a=0; a<10; ++a) {
    for (b=0; b<30000000; ++b);
  }	
    
  mythread_exit();	
  
  printf("This program should never come here\n");
  
  return 0;
} /****** End main() ******/
