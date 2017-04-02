#include <stdio.h>
#include <signal.h>

void sigget(int signum);

int main()
{
	signal(SIGINT, &sigget);
	for(;;);


	return 0;
}

void sigget(int signum)
{
	printf("Get signum SIGINT!!!%d\n",signum);
}