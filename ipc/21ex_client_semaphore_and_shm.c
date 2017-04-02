#include "myshm.h"

int main()
{
	char *shm;
	int shmid;
	int producer,consumer,i;

	if((consumer = semget(ftok("consumer", 0), 1, IPC_CREAT|0666)) == -1)
	{
		printf("semget failed.\n");
		exit(1);
	}
	init_a_semaphore(consumer, 0, 1);

	if((producer = semget(ftok("producer", 0), 1, IPC_CREAT|0666)) == -1)
	{
		printf("semget failed.\n");
		exit(1);
	}
	init_a_semaphore(producer, 0, 1);

	if((shmid = shmget(ftok("shared", 0), SHMSZ, 0666|IPC_CREAT)) == -1)
	{
		printf("shmget failed.\n");
	}
	if((shm = shmat(shmid, (unsigned char *)0, 0)) == -1)
	{
		printf("shmget failed.\n");
		exit(1);
	}

	for(i=0; ;i++)
	{
		semaphore_V(consumer);
		semaphore_P(producer);
		printf("Data receive:%s\n",shm);

		sleep(1);
		if(strncmp(shm, "end", 3) == 0)
			break;
	}

	semctl(producer, 0, IPC_RMID, 0);
	semctl(consumer, 0, IPC_RMID, 0);

	return 0;
}