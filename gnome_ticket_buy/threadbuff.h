#ifndef __THREAD_BUFF_H
#define __THREAD_BUFF_H

#include "global.h"

//definite the status for buffer of pthread
#define BUFF_OCCUPIED 1
#define BUFF_FREED 0

//struct for buffer of pthread
typedef struct thread_buff_struct_t{
	//index for buffer of pthread
	int buff_index;
	//id of pthread
	int tid;
	//save the ip of client
	unsigned long ip_addr;
	//connect socketfd of pthread using
	int conn_fd;
	//status for buffer of pthread
	int buff_status;
}thread_buff_struct;

thread_buff_struct thread_buff[THREAD_BUFF_NUM];

pthread_mutex_t buff_mutex;

//init buffer of pthread
void init_thread_buff()
{
	int index;
	for(index=0; index<THREAD_BUFF_NUM; index++)
	{
		thread_buff[index].tid = -1;
		thread_buff[index].buff_status = BUFF_FREED;
	}
}

//distribute a free buffer of pthread,if not,then return -1
int get_free_buff()
{
	int i,ret;
	//becareful of opration to mutex
	pthread_mutex_lock(&buff_mutex);
	for(i=0; i<THREAD_BUFF_NUM; i++)
	{
		if(thread_buff[i].buff_status == BUFF_FREED)
		{
			ret = i;
			pthread_mutex_unlock(&buff_mutex);
			break;
		}
	}

	if(i == THREAD_BUFF_NUM)
	{
		ret = -1;
		pthread_mutex_unlock(&buff_mutex);
	}
	return ret;
}

//release buffer of pthread,it also need protect of mutex for visiting buff_status
void free_buff(int index)
{
	pthread_mutex_lock(&buff_mutex);
	if(thread_buff[index].buff_status == BUFF_OCCUPIED)
		thread_buff[index].buff_status = BUFF_FREED;
	pthread_mutex_unlock(&buff_mutex);
}

//check the same connect in buffer pthread,bucause pthread of client
//may end,but it also connect again,so server should end the serving pthread
//and renew a serving pthread,distributing a new buffer of pthread
void check_connection(unsigned long ip_addr)
{
	int i,j;
	struct in_addr in;
	char msg[512];

	pthread_mutex_lock(&buff_mutex);
	for(i=0; i<THREAD_BUFF_NUM; i++)
	{
		//find the same connection
		if((thread_buff[i].buff_status != BUFF_FREED) 
			&& thread_buff[i].ip_addr ==ip_addr)
		{
			in.s_addr = htonl(ip_addr);
			sprintf(msg,"same connection:%s,old connect will be closed!\n",inet_ntoa(in));
			add_info(msg);
			pthread_cancel(thread_buff[i].tid);

			pthread_join(thread_buff[i].tid, NULL);
			//when pthread exit ,don't release it buffer,main pthread will release
			thread_buff[i].tid = 0;
			thread_buff[i].buff_status = BUFF_FREED;
			close(thread_buff[i].conn_fd);
		}
	}
	pthread_mutex_unlock(&buff_mutex);
}



#endif