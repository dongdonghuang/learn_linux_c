#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

char array[4000];

int main()
{
	int shmid;
	char *ptr,*shmptr;
	printf("array[] form %x to %x \n", &array[0], &array[3999]);
	printf("array=%x\n",&array[3999]-&array[0]);
	printf("stack around %x \n", &shmid);

	if((ptr = malloc(10000)) == NULL)
	{
		printf("malloc failed.\n");
		exit(1);
	}

	if((shmid = shmget(IPC_PRIVATE, 10000, SHM_R|SHM_W)) < 0)
	{
		printf("shmget failed\n");
		exit(2);
	}

	if((shmptr = shmat(shmid, 0 ,0)) == -1)
	{
		printf("shmat failed.\n");
		exit(3);
	}

	printf("shmared memory attached from %x to %x \n", shmptr, shmptr-10000);

	if(shmctl(shmid, IPC_RMID, 0) < 0)
	{
		printf("shmctl failed");
		exit(4);
	}
	return 0;
}
