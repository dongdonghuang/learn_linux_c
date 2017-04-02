#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>

//#define BUFSIZ 512

struct my_msg
{
	long int my_msg_type;
	char text[BUFSIZ];
}msgbuf;

int main()
{
	int running = 1;
	int msgid;
	long int msg_to_receive = 0;
	//msgbuf.my_msg_type = 0;
	msgid = msgget((key_t)1234, 0666|IPC_CREAT);

	if(msgid == -1)
	{
		printf("msgget failed.\n");
		exit(1);
	}

	while(running)
	{
		if(msgrcv(msgid, (void *)&msgbuf, BUFSIZ, msg_to_receive, 0) == -1)
		{
			perror("msgrcv failed.");
			exit(1);
		}

		printf("You wrote:%s", msgbuf.text);
		if(strncmp(msgbuf.text, "end", 3) == 0)
		{
			running = 0;
		}
	}

	if(msgctl(msgid, IPC_RMID, 0) == -1)
	{
		printf("msgct(IPC_RMID failed.\n");
		exit(1);
	}
	
	return 0;
}
