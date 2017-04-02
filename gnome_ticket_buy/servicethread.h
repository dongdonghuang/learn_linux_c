#ifndef __SERVICE_THREAD_H
#define __SERVICE_THREAD_H

#include "global.h"
#include "ticket.h"
#include "threadbuff.h"

//serve thread deal with error

static void thread_err(char *s, int index)
{
	int i;
	char msg[512];
	//get free output message buffer,if not,delay sometimes and go on
	sprintf(msg, "thread %d fatal error:,%s\n",(unsigned short)pthread_self(), s);
	add_info(msg);
	//info_print(strmsg, serverwindow);
	//release buffer of thread using
	free_buff(index);
	pthread_exit(NULL);
}

//*service_thread:thread function of serve thread
void *service_thread(void *p)
{
	int conn_fd;
	int buff_index;
	char send_buf[1024],recv_buf[512];
	int ret, i,cnt;
	uint16_t nbyte;
	struct sockaddr_in peer_name;
	int peer_name_len;
	unsigned int required_ticket_num;
	int pos;
	thread_buff_struct *pstruct;
	char msg[512];

	//get the number of thread using buffer of thread
	pstruct = (thread_buff_struct *)p;
	buff_index = pstruct->buff_index;
	pstruct->tid = pthread_self();

	//get sockfd of communication use from buffer of thread
	conn_fd =pstruct->conn_fd;

	//print the address of remote localhost
	peer_name_len = sizeof(peer_name);
	ret = getpeername(conn_fd, (struct sockaddr *)&peer_name, &peer_name_len);
	if(ret == -1)
	thread_err("wrong of get address of remote localhost", buff_index); 
	
	sprintf(msg, "new connection-->thread_ID:%d,connect_ID:%d,thread buffer index:%d,remote address:%s,port:%d\n",
	 (unsigned short)pstruct->tid,conn_fd, buff_index, inet_ntoa(peer_name.sin_addr), ntohs(peer_name.sin_port));
	add_info(msg);

	while(1)
	{
		//get record from internet
		ret = recv(conn_fd, recv_buf, sizeof(message), 0);
		//wrong of get
		if(ret == -1)
		{
			sprintf(msg, "thread:%d is connecting:%d recieve wrong.connect will close soon.\n",
				(unsigned short)pstruct->tid, conn_fd);
			add_info(msg);
			thread_err(msg, buff_index);
		}

		//ret==0 note client connect was closed
		if(ret == 0)
		{
			sprintf(msg,"thread:%d connection (ID:%d) client was closed.servr connect will close soon.\n",
				(unsigned short)pstruct->tid, conn_fd);
			add_info(msg);
			close(conn_fd);
			free_buff(buff_index);
			pthread_exit(NULL);
		}

		//if ret was other number,it daclare receive the message from client
		init_message();
		memcpy(&message, recv_buf, sizeof(message));
		switch(message.msg_type)
		{
			case DISCONNECT:
			sprintf(msg, "thread:%d connect(ID:%d) client was closed.server connect will close soon.\n",
				(unsigned short)pstruct->tid, conn_fd);
			add_info(msg);
			close(conn_fd);
			free_buff(buff_index);
			pthread_exit(NULL);
			break;

			case BUY_TICKET:
			for(i=0; i<FLIGHT_NUM; i++)
			{
				pthread_mutex_lock(&ticket_list[i].ticket_mutex);
				if(ticket_list[i].flight_ID == message.flight_ID)
				{
					if(ticket_list[i].ticket_num >= message.ticket_num)
					{
						message.msg_type = BUY_SUCCEED;
						message.ticket_total_price = message.ticket_num*ticket_list[i].ticket_price;
						ticket_list[i].ticket_num -= message.ticket_num;
						pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
						sprintf(msg, "Buy succeed!Flight ID:%d,ticket numbers:%d,total price:%d\n",
							message.flight_ID, message.ticket_num, message.ticket_total_price);
						add_info(msg);
						memcpy(send_buf, &message, sizeof(message));
						ret = send(conn_fd, send_buf, sizeof(message), 0);
						if(ret < 0)
						{
							thread_err("send message data wrong\n", buff_index);
						}
						break;
					}
					else
					{
						message.msg_type = BUY_FAILED;
						required_ticket_num = message.ticket_num;
						message.ticket_num = ticket_list[i].ticket_num;
						pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
						sprintf(msg, "Buy failure! Flight ID:%d,surplus ticket numbers:%d,require ticket number:%d\n",
							message.flight_ID, message.ticket_num, required_ticket_num);
						add_info(msg);
						memcpy(send_buf, &message, sizeof(message));
						ret = send(conn_fd, send_buf, sizeof(message), 0);
						if(ret < 0)
						{
							thread_err("send message data wrong\n", buff_index);
						}
						break;
					}
				}
				pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
			}
			break;

			case INQUIRE_ONE:
			for(i=0; i<FLIGHT_NUM; i++)
			{
				pthread_mutex_lock(&ticket_list[i].ticket_mutex);
				if(ticket_list[i].flight_ID == message.flight_ID)
				{
					message.msg_type = INQUIRE_SUCCEED;
					message.ticket_num = ticket_list[i].ticket_num;
					message.ticket_total_price = ticket_list[i].ticket_price;
					pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
					sprintf(msg, "Inquire Succeed!Flight ID:%d, Ticket number:%d,Ticket Price:%d\n",
						message.flight_ID, message.ticket_num, message.ticket_total_price);
					add_info(msg);
					memcpy(send_buf, &message, sizeof(message));
					ret = send(conn_fd, send_buf, sizeof(message), 0);
					if(ret < 0)
					{
						thread_err("send message data wrong\n", buff_index);
					}
					break;
				}
				pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
			}
			break;

			case INQUIRE_ALL:
			pos = 0;
			for(i=0; i<FLIGHT_NUM; i++)
			{
				pthread_mutex_lock(&ticket_list[i].ticket_mutex);
				message.msg_type = INQUIRE_SUCCEED;
				message.flight_ID = ticket_list[i].flight_ID;
				message.ticket_num = ticket_list[i].ticket_num;
				message.ticket_total_price = ticket_list[i].ticket_price;
				pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
				if(i == 0)
				{
					sprintf(msg, "Inquire all flight succeed!\n");
					add_info(msg);
				}
				sprintf(msg,"Flight ID:%d, Ticket Number:%d, Ticket Price:%d\n",
					message.flight_ID, message.ticket_num, message.ticket_total_price);
				add_info(msg);
				memcpy(send_buf+pos, &message, sizeof(message));
				pos += sizeof(message);
			}
			ret = send(conn_fd, send_buf, pos, 0);
			if(ret < 0)
			{
				thread_err("send message data wrong\n", buff_index);
			}
			break;

			default :
			message.msg_type = UNKNOWN_CODE;
			memcpy(send_buf, &message, sizeof(message));
			ret = send(conn_fd, send_buf, sizeof(message), 0);
			if(ret < 0)
			{
				thread_err("send mesage wrong\n", buff_index);
			}

		}
	}
}




#endif