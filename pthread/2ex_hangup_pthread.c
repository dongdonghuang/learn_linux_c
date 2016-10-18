//#include <stddef.h>
#include <stdio.h>
//#include <unistd.h>
#include <pthread.h>

void print_message(char *ptr);

int main()
{
	pthread_t thread1,thread2;
	int i,j;
	void * retval;
	char *msg1 = "Hello\n";
	char *msg2 = "World\n";

	pthread_create(&thread1, NULL, (void *)(&print_message), (void *)msg1);
	pthread_create(&thread2, NULL, (void *)(&print_message), (void *)msg2);
	pthread_join(thread1, &retval);
	pthread_join(thread2, &retval);
	//sleep(1);
	return 0;
}

void print_message(char * ptr)
{
	int i;
	for(i=0; i<10; i++)
		printf("%s",ptr);
}