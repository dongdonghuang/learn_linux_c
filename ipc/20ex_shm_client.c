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

	if((shmid = shmget(1234, 256, 0666)) < 0)
	{
		printf("shmget failed.\n");
		exit(1);
	}

	if((shmptr = shmat(shmid, 0, 0)) == -1)
	{
		shmctl(shmid, IPC_RMID, shmptr);
		printf("shmgat failed.\n");
		exit(2);
	}

	for(s=shmptr; *s!=NULL; s++)
		putchar(*s);
	printf("\n");

	*shmptr = '*';
	return 0;
}