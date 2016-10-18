#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 4
#define OVER (-1)
/*
typedef union
{
  char __size[__SIZEOF_SEM_T];
  long int __align;
} sem_t;
*/

struct producers
{
	int buffer[BUFFER_SIZE];
	int readpos,writepos;
	sem_t sem_read;
	sem_t sem_write;
};

void init(struct producers *b)
{
	sem_init(&b->sem_write, 0, BUFFER_SIZE-1);
	sem_init(&b->sem_read, 0, 0);
	b->readpos = 0;
	b->writepos = 0;
}

void put(struct producers *b, int data)
{
	sem_wait(&b->sem_write);
	b->buffer[b->writepos] = data;
	b->writepos++;
	if(b->writepos >= BUFFER_SIZE)
		b->writepos = 0;
	sem_post(&b->sem_read);
}

int get(struct producers *b)
{
	int data;
	sem_wait(&b->sem_read);
	data = b->buffer[b->readpos];
	b->readpos++;

	if(b->readpos >= BUFFER_SIZE)
		b->readpos = 0;
	sem_post(&b->sem_write);
	return data;
}

struct producers buffer;

void *producer(void *data)
{
	int n;
	for(n=0; n<10; n++)
	{
		printf("Producer:%d-->\n",n);
		put(&buffer, n);
	}
	put(&buffer, OVER);
	return NULL;
}

void *consumer(void *data)
{
	int d;
	while(1)
	{
		d = get(&buffer);
		if(d == OVER)
			break;
		printf("Consumer:-->%d\n",d);
	}
	return NULL;
}

int main()
{
	pthread_t tha,thb;
	void *retval;
	init(&buffer);
	pthread_create(&tha, NULL, producer, 0);
	pthread_create(&thb, NULL, consumer, 0);
	pthread_join(tha, &retval);
	pthread_join(thb, &retval);
	return 0;
}