#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>



#define MAXMSG 512

int main()
{
	int sockfd, n;
	char recvbuff[MAXMSG],sndbuff[MAXMSG];
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		printf("socket created failed.\n");
		return -1;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6666);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	while(fgets(sndbuff, MAXMSG, stdin) != NULL)
	{
		if(sendto(sockfd, sndbuff, sizeof(sndbuff), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		{
			printf("(client)sendto error.\n");
			return -1;
		}
		if(n = recvfrom(sockfd, recvbuff, MAXMSG, 0, NULL, NULL) < 0)
		{
			printf("(client)receiving error.\n");
			return -1;
		}
		recvbuff[n] = 0;
		printf("n=%d Client received message:%s\n", n, recvbuff);
	}
	close(sockfd);


	return 0;
}