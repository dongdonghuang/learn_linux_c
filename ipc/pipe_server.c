#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_FILE "MYFIFO"

int main()
{
	FILE *fp;
	char readbuff[80];

	if((fp=fopen(FIFO_FILE, "r")) == NULL)
	{
		umask(0);
		mknod(FIFO_FILE, S_IFIFO|0666, 0);
	}
	else
		fclose(fp);

	while(1)
	{
		if((fp=fopen(FIFO_FILE, "r")) == NULL)
		{
			printf("open fifo failed.\n");
			exit(1);
		}

		if(fgets(readbuff, 80, fp) != NULL)
		{
			printf("Received string:%s\n", readbuff);
			fclose(fp);
		}
		else
		{
			if(ferror(fp))
			{
				printf("read fifo failed.\n");
				exit(1);
			}
		}
	}
	return 0;
}