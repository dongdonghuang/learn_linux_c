#include "servicethread.h"

int listen_fd,conn_fd;
struct sockaddr_in server,cli_addr;
int ret,buffer_index,i,cli_len;
unsigned long ip_addr;
int flag = 1;
pthread_t listentid,servicetid;

char socket_print[512] = {0};

static GtkWidget *app;
static GtkWidget *frame, *vbox, *box2, *table;
static GtkWidget *serverwindow, *vscrollbar;

//listen thread
void *listen_thread(void *p)
{
	char msg[512];
	while(1)
	{
		//accept require TCP connect from remote
		cli_len = sizeof(cli_addr);

		sprintf(socket_print, "I have ready for accept!\n");

		conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
		if(conn_fd < 0)
			continue; 


		ip_addr = ntohl(cli_addr.sin_addr.s_addr);
		//check the same connect
		check_connection(ip_addr);
		//distribute buffer of thread
		buffer_index = get_free_buff();
		if(buffer_index < 0)
		{
			sprintf(msg, "do not have free buffer of thread.\n");
			add_info(msg);
			close(conn_fd);
			continue;
		}

		//write message of serve thread nedd
		pthread_mutex_lock(&buff_mutex);
		thread_buff[buffer_index].buff_index = buffer_index;
		thread_buff[buffer_index].ip_addr = ip_addr;
		thread_buff[buffer_index].conn_fd = conn_fd;
		thread_buff[buffer_index].buff_status = BUFF_OCCUPIED;
		pthread_mutex_unlock(&buff_mutex);

		//create the new serve thread
		ret = pthread_create(&servicetid, NULL, service_thread, &thread_buff[buffer_index]);
		if(ret == -1)
		{
			sprintf(msg, "create thread wrong!\n");
			add_info(msg);
			close(conn_fd);
			//release buffer of thread
			pthread_mutex_lock(&buff_mutex);
			thread_buff[buffer_index].tid = 0;
			thread_buff[buffer_index].buff_status = BUFF_FREED;
			pthread_mutex_unlock(&buff_mutex);
		}

	}
}

int isserveropened = FALSE;

void startserver(GtkWidget *widget, gpointer data);
void stopserver(GtkWidget *widget, gpointer data);
void inquireone();
void inquireall();
void displaycontens(GtkWidget *widget, gpointer data);
void about(GtkWidget *widget, gpointer data);

//create menu of server operation
GnomeUIInfo server_operation_menu[]={
	GNOMEUIINFO_ITEM_NONE("Open server", "Open server", startserver),
	GNOMEUIINFO_ITEM_NONE("Close server", "Close server", stopserver),
	GNOMEUIINFO_ITEM_NONE("Exit", "Exit program", gtk_main_quit),
	GNOMEUIINFO_END
};

//create menu of flight inquire
GnomeUIInfo inquire_menu[]={
	GNOMEUIINFO_ITEM_NONE("Inquire special flight", "Inquire ticket of information for a special flight", inquireone),
	GNOMEUIINFO_ITEM_NONE("Inquire all flight", "Inquire all information for all flight", inquireall),
	GNOMEUIINFO_END
};

//create menu of help menu
GnomeUIInfo help_menu[]={
	GNOMEUIINFO_ITEM_NONE("Display contents", "Display help contents", displaycontens),
	GNOMEUIINFO_ITEM_NONE("About", "About statement of th program", about),
	GNOMEUIINFO_END
};

//create top menu
GnomeUIInfo menubar[]={
	GNOMEUIINFO_SUBTREE("Server operate(_S)", server_operation_menu),
	GNOMEUIINFO_SUBTREE("Inquire ticket(_Q)", inquire_menu),
	GNOMEUIINFO_SUBTREE("Help(_H)", help_menu),
	GNOMEUIINFO_END
};

GnomeUIInfo toolbar[]={
	GNOMEUIINFO_ITEM_STOCK("Open server", "Open server", startserver, GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_ITEM_STOCK("Close server", "Close server", stopserver, GNOME_STOCK_PIXMAP_PREFERENCES),
	GNOMEUIINFO_ITEM_STOCK("Inquire Special flight", "Inquire ticket of information for a special flight", inquireone, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK("Inquire all flight", "Inquire all information for all flight", inquireall, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK("Exit", "Exit program", gtk_main_quit, GNOME_STOCK_PIXMAP_EXIT),
	GNOMEUIINFO_END
};

//func of information display
void display_info(char *msg, GtkWidget *window)
{
	gtk_text_freeze(GTK_TEXT(window));
	gtk_text_insert(GTK_TEXT(window), NULL, &window->style->black, NULL, msg, -1);
	gtk_text_thaw(GTK_TEXT(window));
}

//open server
void startserver(GtkWidget *widget, gpointer data)
{
	char msg[512];
	GtkWidget *isenable;

	//init data struct
	init_thread_buff();
	init_ticket_list();

	if(!isserveropened)
	{
		listen_fd = socket(AF_INET, SOCK_STREAM, 0);
		if(listen_fd < 0)
		{
			sprintf(msg, "create listen socket failer!\n");
			display_info(msg, serverwindow);
			return ;
		}

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = htonl(INADDR_ANY);
		server.sin_port = htons(SERVER_PORT_NO);

		setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&flag, sizeof(int));

		ret = bind(listen_fd, (struct sockaddr *)&server, sizeof(server));
		if(ret < 0)
		{
			sprintf(msg, "bind TCP port :%d wrong!\n",SERVER_PORT_NO);
			display_info(msg, serverwindow);
			close(listen_fd);
			return;
		}

		listen(listen_fd, 5);
		ret = pthread_create(&listentid, NULL, listen_thread,NULL);
		if(ret == -1)
		{
			sprintf(msg, "create listenfd thread wrong!\n");
			display_info(msg, serverwindow);
			close(listen_fd);
			return;
		}

		//if succeed output the note
		sprintf(msg, "Server open succeed!Server of port:%d is read or accept connect!\n",SERVER_PORT_NO);
		display_info(msg, serverwindow);
		isserveropened = TRUE;

		display_info(socket_print, serverwindow);

		isenable = toolbar[0].widget;
		gtk_widget_set_sensitive(isenable, FALSE);
		isenable = server_operation_menu[0].widget;
		gtk_widget_set_sensitive(isenable, FALSE);

		//close server's function of menu and toolbar
		isenable = toolbar[1].widget;
		gtk_widget_set_sensitive(isenable, TRUE);
		isenable = server_operation_menu[1].widget;
		gtk_widget_set_sensitive(isenable, TRUE);
	}
}

//operation of stopping server
void stopserver(GtkWidget *widget, gpointer data)
{
	GtkWidget *isenable;
	char msg[512];
	if(isserveropened)
	{
		pthread_mutex_lock(&buff_mutex);
		for(i=0; i<THREAD_BUFF_NUM; i++)
		{
			if(thread_buff[i].buff_status != BUFF_FREED)
			{
				//exit the server's thread
				pthread_cancel(thread_buff[i].tid);
				pthread_join(thread_buff[i].tid, NULL);

				thread_buff[i].tid = 0;
				thread_buff[i].buff_status = BUFF_FREED;
				close(thread_buff[i].conn_fd);
			}
		}
		pthread_mutex_unlock(&buff_mutex);
		pthread_cancel(listentid);
		pthread_join(listentid, NULL);
		close(listen_fd);
		sprintf(msg, "Server close succeed!\n");
		display_info(msg, serverwindow);
		isserveropened = FALSE;

		//open the function of server's menu and toolbar
		isenable = toolbar[0].widget;
		gtk_widget_set_sensitive(isenable, TRUE);
		isenable = server_operation_menu[0].widget;
		gtk_widget_set_sensitive(isenable, TRUE);

		//close server's menu and toolbar
		isenable = toolbar[1].widget;
		gtk_widget_set_sensitive(isenable, FALSE);
		isenable = server_operation_menu[1].widget;
		gtk_widget_set_sensitive(isenable, FALSE);
	}
}

//inquire special one of flight about "sure","cancel"
int button_inquireone(GnomeDialog *dialog, gint id, gpointer data)
{
	GtkWidget *flight = data;
	GtkWidget *mbox;
	char msg[512];
	int flight_ID;
	//if button of "sure" pressed
	if(id == 0)
	{
		sprintf(msg, gtk_entry_get_text(GTK_ENTRY(flight)));
		flight_ID = atoi(msg);
		if(flight_ID <= 0 || flight_ID > 10)
		{
			mbox = gnome_message_box_new("Flight_ID input wrong!Please input again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
			gtk_widget_show(mbox);
			gtk_window_set_modal
			(GTK_WINDOW(mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
			gtk_entry_set_text(GTK_ENTRY(flight), " ");
			return;
		}

		for(i=0; i<FLIGHT_NUM; i++)
		{
			pthread_mutex_lock(&ticket_list[i].ticket_mutex);
			if(ticket_list[i].flight_ID == flight_ID)
			{
				sprintf(msg, "You inquire the number of flight_ID is:%d,last ticket:%d,ticket price:%d\n",
					ticket_list[i].flight_ID, ticket_list[i].ticket_num, ticket_list[i].ticket_price);
				display_info(msg, serverwindow);
				pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
				break;
			}
			pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
		}
		gnome_dialog_close(dialog);
	}
	else
		gnome_dialog_close(dialog);
}

//inquire special flight of one
void inquireone()
{
	GtkWidget *dialog;
	GtkWidget *label,*flight_entry;
	dialog = gnome_dialog_new(_("Inquire a special information for flight"),
		GNOME_STOCK_BUTTON_OK, GNOME_STOCK_BUTTON_CANCEL, NULL);

	label = gtk_label_new("Please input the flight_ID you inquired(1-10):");
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	flight_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(flight_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), flight_entry, FALSE, FALSE, 0);
	gtk_widget_show(flight_entry);

	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);

	gtk_signal_connect(GTK_OBJECT(dialog), "clicked", GTK_SIGNAL_FUNC(button_inquireone), flight_entry);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_widget_show(dialog);
	gnome_dialog_set_parent(GNOME_DIALOG(dialog), GTK_WINDOW(app));
}

void inquireall()
{
	int i;
	char msg[512];
	for(i=0; i<FLIGHT_NUM; i++)
	{
		sprintf(msg, "Flight_ID:%d,last number of flight:%d,ticket price:%d\n",
			ticket_list[i].flight_ID, ticket_list[i].ticket_num, ticket_list[i].ticket_price);
		display_info(msg, serverwindow);
	}
}

//help dialog box
void dialog_ok(GnomeDialog *dialog, gint id, gpointer data)
{
	gnome_dialog_close(dialog);
}

//help  ---show contents
void displaycontens(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	char msg[4][512]={
					  "Open server:open server,ready for accept connect from client",
					  "Close server:close server",
					  "Inquire special flight:inquire information for a special flight",
					  "Inquire all flight:inquire information for all flight"
	};
	dialog = gnome_dialog_new(_("Help"),_("Asure"), NULL, NULL);
	for(i=0; i<4; i++)
	{
		label = gtk_label_new(msg[i]);
		gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
		gtk_widget_show(label);
	}

	gtk_signal_connect(GTK_OBJECT(dialog), "clicked", GTK_SIGNAL_FUNC(dialog_ok), &dialog);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_widget_show(dialog);
	gnome_dialog_set_parent(GNOME_DIALOG(dialog), GTK_WINDOW(app));
}

//Help---about
void about(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *contentlabel,*versionlabel,*authorlabel,*copyrightlabel;
	dialog = gnome_dialog_new(_("About the program"), _("Asure"), NULL, NULL);
	contentlabel = gtk_label_new(_("Simulate Network Sale Ticket System of Server"));
	copyrightlabel = gtk_label_new(_("Copyright 2016"));
	authorlabel = gtk_label_new(_("hdy"));
	versionlabel = gtk_label_new(_("Version:1.0"));

	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), contentlabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), versionlabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), copyrightlabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), authorlabel, TRUE, TRUE, 0);

	gtk_widget_show(contentlabel);
	gtk_widget_show(versionlabel);
	gtk_widget_show(contentlabel);
	gtk_widget_show(authorlabel);

	gtk_signal_connect(GTK_OBJECT(dialog), "clicked", GTK_SIGNAL_FUNC(dialog_ok), &dialog);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_widget_show(dialog);
	gnome_dialog_set_parent(GNOME_DIALOG(dialog), GTK_WINDOW(app));
}

gint info_print()
{
	int i;
	pthread_mutex_lock(&info_mutex);
	for(i=0; i<INFO_NUM; i++)
	{
		if(info[i].status == INFO_OCCUPIED)
		{
			display_info(info[i].msg, serverwindow);
			info[i].status = INFO_FREED;
		}
	}

	pthread_mutex_unlock(&info_mutex);
	return TRUE;
}

int main(int argc, char *argv[])
{
	GtkWidget *isenable;

	init_ticket_list();
	gnome_init("example", "0.1", argc, argv);
	app = gnome_app_new("example", "Simulate Network Sale Ticket System of Server");
	gtk_signal_connect(GTK_OBJECT(app), "delete_event", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

	gnome_app_create_menus(GNOME_APP(app), menubar);
	gnome_app_create_toolbar(GNOME_APP(app), toolbar);

	if(!isserveropened)
	{
		isenable = toolbar[1].widget;
		gtk_widget_set_sensitive(isenable, FALSE);
		isenable = server_operation_menu[1].widget;
		gtk_widget_set_sensitive(isenable, FALSE);
	}

	gtk_window_set_default_size((GtkWindow *)app, 800, 600);
	vbox = gtk_vbox_new(FALSE, 0);
	gnome_app_set_contents(GNOME_APP(app), vbox);

	frame = gtk_frame_new(NULL);
	gtk_frame_set_label(GTK_FRAME(frame), "Server Information");
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);

	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 10);
	gtk_widget_show(vbox);

	box2 = gtk_vbox_new(FALSE, 10);
	gtk_container_set_border_width(GTK_CONTAINER(box2), 10);
	gtk_container_add(GTK_CONTAINER(frame), box2);
	gtk_widget_show(box2);

	table = gtk_table_new(2, 2, FALSE);
	gtk_table_set_row_spacing(GTK_TABLE(table), 0, 2);
	gtk_table_set_col_spacing(GTK_TABLE(table), 0, 2);
	gtk_box_pack_start(GTK_BOX(box2), table, TRUE, TRUE, 0);
	gtk_widget_show(table);

	serverwindow = gtk_text_new(NULL, NULL);
	gtk_text_set_editable(GTK_TEXT(serverwindow), FALSE);
	gtk_text_set_word_wrap(GTK_TEXT(serverwindow), TRUE);
	gtk_table_attach(GTK_TABLE(table), serverwindow, 0, 1, 0, 1,
	 GTK_EXPAND|GTK_SHRINK|GTK_FILL, GTK_EXPAND|GTK_SHRINK|GTK_FILL, 0, 0);
	gtk_widget_show(serverwindow);

	vscrollbar = gtk_vscrollbar_new(GTK_TEXT(serverwindow)->vadj);
	gtk_table_attach(GTK_TABLE(table), vscrollbar, 1, 2, 0, 1, GTK_FILL,
		GTK_EXPAND|GTK_SHRINK|GTK_FILL, 0, 0);
	gtk_widget_show(vscrollbar);
	gtk_widget_realize(serverwindow);

	gtk_widget_show_all(app);
	g_timeout_add(500, info_print, NULL);
	gtk_main();

	return 0;

}
