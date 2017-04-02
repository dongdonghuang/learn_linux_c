#ifndef __GLOBALAPI_H
#define __GLOBALAPI_H

//it very important ,because gtktext.h include 
// #ifdefinne GTK_ENABLE_BROKEN
// #endif
//if don't include the macro of GTK_ENABLE_BROKEN
//it won't include <gtktext.h>
#define GTK_ENABLE_BROKEN


#include <sys/types.h>//base of data
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <gnome.h>





//port of server using
#define SERVER_PORT_NO 8889

//definition of message between client and server
#define INITIAL_VALUE 65535

//definiton of message code client using
#define DISCONNECT 0
#define BUY_TICKET 1
#define INQUIRE_ONE 2
#define INQUIRE_ALL 3

//definition of message code serve using
#define BUY_SUCCEED 255
#define BUY_FAILED 256
#define INQUIRE_SUCCEED 257
#define UNKNOWN_CODE 258

//definition message struct between serve and client
struct stMessage{
	//class of message like DISCONNECT,BUY_TICKET,INQUIRE_ONE or INQUIRE_ALL
	unsigned int msg_type;
	//number of fly
	unsigned int flight_ID;
	//number of ticket
	unsigned int ticket_num;
	//price of ticket
	unsigned int ticket_total_price;
}message;

void init_message()
{
	message.msg_type = INITIAL_VALUE;
	message.flight_ID = 0;
	message.ticket_num = 0;
	message.ticket_total_price = 0;
}

//maximun of pthread buffer number of server
#define THREAD_BUFF_NUM 128

//note of message intput
#define INFO_NUM 10
#define INFO_OCCUPIED 1
#define INFO_FREED 0

struct info_t{
	//INFO_OCCUPIED or INFO_FREED
	int status;
	//contents of message
	char msg[512];
}info[INFO_NUM];

pthread_mutex_t info_mutex;

//init buffer of output message
void init_info()
{
	int i;
	for(i = INFO_NUM; i>0; i--)
		info[i-1].status = INFO_FREED;
	sprintf(info[i-1].msg, " ");
}

//distritute a free buffer of output,if it hasn't free buffer,then return -1
int get_free_info()
{
	int i,ret;
	pthread_mutex_lock(&info_mutex);
	for(i=0; i<INFO_NUM; i++)
	{
		if(info[i].status == INFO_FREED)
		{
			ret = i;
			pthread_mutex_unlock(&info_mutex);
			break;
		}
	}

	if(i == INFO_NUM)
	{
		ret = -1;
		pthread_mutex_unlock(&info_mutex);
	}
	return ret;
}

//release buffer of message,it also need protect of mutex for visit info_status
void free_info(int index)
{
	int i;
	pthread_mutex_lock(&info_mutex);
	if(info[index].status == INFO_OCCUPIED)
		info[index].status = INFO_FREED;
	pthread_mutex_unlock(&info_mutex);
}

void add_info(char *src)
{
	int i;
	while((i = get_free_info()) == -1)
	{
		sleep(1);
	}

	//add message
	pthread_mutex_lock(&info_mutex);
	info[i].status = INFO_OCCUPIED;
	strcpy(info[i].msg, src);
	pthread_mutex_unlock(&info_mutex);

}


#endif