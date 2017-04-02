#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h> //for inet_ntoa()

#define LISTENQ 5
#define MAXLINE 512

int main()
{
	int listenfd,connfd;
	socklen_t len;
	struct sockaddr_in servaddr,cliaddr;
	char buff[MAXLINE];
	time_t ticks;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		printf("Socket created failed.\n");
		return -1;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6666);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("Bind failed.\n");
		return -1;
	}

	printf("listening...\n");
	listen(listenfd, LISTENQ);

	while(1)
	{
		char *p;
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
		//p = inet_ntoa(cliaddr.sin_addr);
		printf("connect from %s,port %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

		ticks = time(NULL);
		
		sprintf(buff, "%s\r\n",ctime(&ticks));
		write(connfd, buff, strlen(buff));
		close(connfd);
	}

	return 0;
}
