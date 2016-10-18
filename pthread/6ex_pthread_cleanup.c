#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define NUM_THREADS 5

void *search(void *);
void print_it(void *);

pthread_t threads[NUM_THREADS];
pthread_mutex_t lock;

int tries;
int started;

int main(int argc, char *argv[])
{
	int i,pid;

	pid = getpid();
	printf("Searching for the number = %d...\n",pid);

	pthread_mutex_init(&lock, NULL);
	for(started=0; started<NUM_THREADS; started++)
		pthread_create(&threads[started], NULL, search, (void *)pid);

	for(i=0; i<NUM_THREADS; i++)
		pthread_join(threads[i], NULL);

	printf("It took %d to find the number.\n",tries);
	return 0;
}

void print_it(void *arg)
{
	int *try = (int *)arg;
	pthread_t tid;

	tid = pthread_self();
	printf("Thread %lx was cancled on its %d try.\n",tid, *try);
}

void *search(void *arg)
{
	int num = (int) arg;
	int i,j,ntries;
	pthread_t tid;
	
	tid = pthread_self();
	while(pthread_mutex_trylock(&lock) == EBUSY)
		pthread_testcancel();

	srand((int)tid);
	i = rand()&0xFFFFFF;
	pthread_mutex_unlock(&lock);

	ntries = 0;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	while(started < NUM_THREADS)
		sched_yield();
	pthread_cleanup_push(print_it, (void *)&ntries);

	while(1)
	{
		i = (i+1)&0xffffff;
		ntries++;
		if(num == i)
		{
			while(pthread_mutex_trylock(&lock) == EBUSY)
				pthread_testcancel();
			tries = ntries;
			printf("Thread %lx found the number!\n",tid);

			for(j=0; j<NUM_THREADS; j++)
				if(threads[j] != tid)
					pthread_cancel(threads[j]);
			break;
		}
		if(ntries%100 == 0)
			pthread_testcancel();
	}
	pthread_cleanup_pop(0);
	return ((void *)0);

}
