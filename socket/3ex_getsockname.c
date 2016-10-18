#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

int main()
{
	int sockfd;
	socklen_t len;
	struct sockaddr_in addr,raddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("create socket failed.\n");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(0);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("bind socket failed.\n");
		return -2;
	}

	len = sizeof(raddr);

	if(getsockname(sockfd, (struct sockaddr *)&raddr, &len) < 0)
	{
		printf("getsockname failed.\n");
		return -3;
	}

	printf("bound name=%d,port = %d\n",
		ntohl(raddr.sin_addr.s_addr), ntohs(raddr.sin_port));

	return 0;
}