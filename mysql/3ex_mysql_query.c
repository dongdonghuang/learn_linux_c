#include <stdio.h>
#include <mysql/mysql.h>

int main()
{
	MYSQL my_connection;
	int res;
	mysql_init(&my_connection);

	if(mysql_real_connect(&my_connection, "localhost", "test", "test",
		"testdb", 0, NULL, 0))
	{
		printf("Connection Succeed.\n");
		res = mysql_query(&my_connection, "insert into children(fname,age)values('Ann',3)");
		if(!res)
		{
			printf("Insert %lu row.\n",
				(unsigned long)mysql_affected_rows(&my_connection));
		}
		else
		{
			fprintf(stderr, "Insert error %d %s\n",mysql_errno(&my_connection),
				mysql_error(&my_connection));
			return -1;
		}

		mysql_close(&my_connection);
		printf("Connection closed.\n");
	}
	else
	{
		fprintf(stderr,"Connection failed.\n");
		if(mysql_errno(&my_connection))
		{
			fprintf(stderr, "Connection error:%d %s\n",mysql_errno(&my_connection),
				mysql_error(&my_connection));
			return -2;
		}
	}


	return 0;
}