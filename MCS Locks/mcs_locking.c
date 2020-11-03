/*
 * example of a Mutex Lock using the x86 btr instruciton
 * Compile: gcc -m32 -O3 -fno-inline critical-section.c -o critical-section -lpthread
 */
#include <stdio.h>	
#include <stdlib.h>	
#include <sched.h>	
#include <sys/wait.h>	
#include <pthread.h>
#include "./rdtsc.h"

#define            N_THREADS        8
#define            CPU_SPEED        2800000000.0

int	mutex = 1;

typedef struct record {
	volatile struct record* next;
	volatile unsigned char locked;
} qnode;

typedef qnode *Lock;

Lock *lock;
unsigned int maximum = 100000;	// default number of iterations
int	counter = 0;		// the shared counter is global

extern void aquire_lock(Lock* lock, qnode* I);
extern void release_lock(Lock* lock, qnode* I);

int my_thread( void *data )
{
	unsigned int i, temp;
	unsigned long long times[maximum/100];
	unsigned long long startTime = 0;
	unsigned long long finishTime = 0;
	qnode* I, *loop;
	I = (qnode *)malloc(sizeof(qnode));

	startTime = rdtsc();

	for (i = 0; i <= maximum; i++) 
	{
		aquire_lock(lock, I);
		temp = counter;
		temp += 1;
		counter = temp;
		if(i % 10 == 0 && i>0)
		{
			finishTime = rdtsc();
			//loop = I;
			//printf("%d->", counter);
			//while(loop->next)
			//{
			//	printf("->%p_(%d)", loop->next, loop->next->locked);
			//	loop = loop->next;
			//}
			//printf("->(nil)\n\n");
			//printf("Count %d on Thread %p\n", i, pthread_self());
			times[(i/100)-1] = finishTime - startTime;
			startTime = rdtsc();
		}
		release_lock(lock, I);
	}

	for (i = 0; i < maximum/100; i++) 
	{
		printf("%p,%d,%lld,%lf\n", pthread_self(), i, times[i], ((double) times[i]/CPU_SPEED));
	}

	free(I);
	return	0;
}

int main( int argc, char **argv )
{
	pthread_t             threadid[N_THREADS];
	int                   rc=0;
	int                   loop=0;
	pthread_attr_t        pta;

	//allocated memory for pointer to lock
	lock = (Lock *)malloc(sizeof(Lock));

//	printf( "%d threads will increment a shared counter %d times\n", N_THREADS, maximum );

	pthread_attr_init(&pta);
	pthread_attr_setdetachstate(&pta, PTHREAD_CREATE_JOINABLE);

//	printf("Creating %d threads\n", N_THREADS);
	for (loop=0; loop<N_THREADS; ++loop) {
	rc = pthread_create(&threadid[loop], &pta, (void *(*)(void*))my_thread, NULL);
	}

//	printf("Wait for results\n");
	for (loop=0; loop<N_THREADS; ++loop) {
	rc = pthread_join(threadid[loop], NULL);
	}  

//	printf("Cleanup and show results\n");
	pthread_attr_destroy(&pta);

	//--------------------------------------------
	// display the resulting value of the counter
	//--------------------------------------------
	printf( "result: counter=%d\n", counter );

	free(lock);

	return 0;
}
