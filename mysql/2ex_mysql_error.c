#include <stdio.h>
#include <stdio.h>
#include <mysql/mysql.h>

int main()
{
	MYSQL my_connection;
	mysql_init(&my_connection);

	if(mysql_real_connect(&my_connection, "localhost", "test", "test",
		"testdb", 0, NULL, 0))
	{
		printf("Connection Succeed.\n");
		mysql_close(&my_connection);
	}
	else
	{
		fprintf(stderr, "Connection failed.\n");
		if(mysql_errno(&my_connection))
		{
			fprintf(stderr, "Connection error:%d %s\n",
				mysql_errno(&my_connection), mysql_error(&my_connection));
			return -1;
		}
	}


	return 0;
}