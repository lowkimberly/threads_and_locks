//  gcc -m32 -O3 -fno-inline slock.c slocktest.c -o slocktest -lpthread

//  The header files we're using
#include <stdio.h>	
#include <stdlib.h>	
#include <sched.h>	
#include <sys/wait.h>	
#include <pthread.h>
#include "rdtsc.h"

//  Our number of threads
#define N_THREADS 8

//  This is our global stuff
int maximum = 100000;
int counter = 0;
pthread_t threadid[N_THREADS];


//  Incrementing the shared global counter
int my_thread(void *data)
{
  int i;
  int timeafter=0;
  int timebefore=rdtsc();
  for(i=0;i<maximum;i++) 
    {
      enter_critical();
      //      counter++;
      leave_critical();
      if (pthread_equal(threadid[0],pthread_self()) !=0)
	{
	  if(((i+1)%100)==0)
	    {
	      timeafter=rdtsc();
	      printf("%d\n",timeafter-timebefore);
	      timebefore=timeafter;
	      timeafter=0;
	    }
	}
    }
  return 0;
}

// Our main function
int main( int argc, char **argv )
{
  int rc=0;
  int loop=0;
  pthread_attr_t pta;

  printf( "%d threads will increment a shared counter %d times\n", N_THREADS, maximum );
  pthread_attr_init(&pta);
  pthread_attr_setdetachstate(&pta, PTHREAD_CREATE_JOINABLE);

  printf("Creating %d threads\n", N_THREADS);
  for (loop=0; loop<N_THREADS; ++loop) {
    rc = pthread_create(&threadid[loop], &pta, (void *(*)(void*))my_thread, NULL);
  }
  printf("Wait for results\n");
  for (loop=0; loop<N_THREADS; ++loop) {
    rc = pthread_join(threadid[loop], NULL);
  }  
  printf("Cleanup and show results\n");
  pthread_attr_destroy(&pta);
  printf( "result: counter=%d\n", counter );
}
