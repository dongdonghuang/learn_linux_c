#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	int shmid;
	char c;
	char *shmptr,*s;

	if((shmid = shmget(1234, 256, IPC_CREAT|0666)) < 0)
	{
		printf("shmget failed.\n");
		exit(1);
	}

	if(*(shmptr = shmat(shmid, 0, 0)) == -1)
	{
		shmctl(shmid, IPC_RMID, shmptr);
		printf("shmat failed.\n");
		exit(2);
	}

	s = shmptr;
	for(c='a'; c<='z'; c++)
		*s++ = c;
	*s = '\0';
	while(*shmptr != '*')
		sleep(1);

	shmctl(shmid, IPC_RMID, shmptr);
	return 0;
}