#ifndef __TICKET_H
#define __TICKET_H

#include "global.h"

//total numbers of flight
#define FLIGHT_NUM 10

//a simple introduction of flight ticket
typedef struct ticket_struct_t{
	int flight_ID;
	int ticket_num;
	int ticket_price;
	pthread_mutex_t ticket_mutex;
} ticket_struct;

ticket_struct ticket_list[FLIGHT_NUM];

//init the ticket_list
void init_ticket_list()
{
	int i;
	for(i=0; i<FLIGHT_NUM; i++)
	{
		ticket_list[i].flight_ID = i+1;
		ticket_list[i].ticket_num = 100;
		ticket_list[i].ticket_price = 300*(i+1);
	}
}

#endif