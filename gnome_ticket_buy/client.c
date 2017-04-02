#include "global.h"

int socket_fd;
struct sockaddr_in server;
int ret, i;
int flag = 1;

static GtkWidget *app;
static GtkWidget *frame, *vbox, *box2, *table;
static GtkWidget *clientwindow, *vscroball;

int isconnected = FALSE;

void connectserver(GtkWidget *widget, gpointer data);
void disconnect(GtkWidget *widget, gpointer data);
void buyticket(GtkWidget *widget, gpointer data);
void inquireone();
void inquireall();
void displaycontents(GtkWidget *widget, gpointer data);
void about(GtkWidget *widget, gpointer data);

GnomeUIInfo client_operation_menu[]={
	GNOMEUIINFO_ITEM_NONE("Connect server", "Build connect with remote server", connectserver),
	GNOMEUIINFO_ITEM_NONE("Close connect","Close connect with remote server", disconnect),
	GNOMEUIINFO_ITEM_NONE("Buy ticket", "Buy ticket", buyticket),
	GNOMEUIINFO_ITEM_NONE("Exit", "Exit program", gtk_main_quit),
	GNOMEUIINFO_END
};

GnomeUIInfo inquire_menu[]={
	GNOMEUIINFO_ITEM_NONE("Inquire special flight", "Inquire a certain flight", inquireone),
	GNOMEUIINFO_ITEM_NONE("Inquire all flight", "Inquire all information for every flight", inquireall),
	GNOMEUIINFO_END
};

GnomeUIInfo help_menu[]={
	GNOMEUIINFO_ITEM_NONE("Display contents", "Display contents for help", displaycontents),
	GNOMEUIINFO_ITEM_NONE("About", "About explanation of the program", about),
	GNOMEUIINFO_END
};

GnomeUIInfo menubar[]={
	GNOMEUIINFO_SUBTREE("Client operation(_S)", client_operation_menu),
	GNOMEUIINFO_SUBTREE("Inquire ticket(_Q)", inquire_menu),
	GNOMEUIINFO_SUBTREE("help(_H)", help_menu),
	GNOMEUIINFO_END
};

GnomeUIInfo toolbar[]={
	GNOMEUIINFO_ITEM_STOCK("Connect server", "Build connect with remote server", connectserver, GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_ITEM_STOCK("Close connect", "Close connect with remote server", disconnect, GNOME_STOCK_PIXMAP_CUT),
	GNOMEUIINFO_ITEM_STOCK("Buy ticket", "Buy ticket", buyticket, GNOME_STOCK_PIXMAP_OPEN),
	GNOMEUIINFO_ITEM_STOCK("Inquire special flight", "Inquire a certain flight", inquireone, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK("Inquire all flight", "Inquire all information for every flight", inquireall, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK("Exit", "Exit program", gtk_main_quit, GNOME_STOCK_PIXMAP_EXIT),
	GNOMEUIINFO_END
};

void display_info(char *msg, GtkWidget *window)
{
	gtk_text_freeze(GTK_TEXT(window));
	gtk_text_insert(GTK_TEXT(window), NULL, &window->style->black, NULL, msg, -1);
	gtk_text_thaw(GTK_TEXT(window));
}

void connectserver(GtkWidget *widget, gpointer data)
{
	char msg[512];
	GtkWidget *isenable;
	int i;

	if(!isconnected)
	{
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if(socket_fd < 0)
		{
			sprintf(msg, "Create socket failed!\n");
			display_info(msg, clientwindow);
			return;
		}

		struct timeval time_out;
		time_out.tv_sec = 5;
		time_out.tv_usec = 0;
		setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out));

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr("127.0.0.1");//htonl(INADDR_ANY);
		server.sin_port = htons(SERVER_PORT_NO);

		ret = connect(socket_fd, (struct sockaddr *)&server, sizeof(server));
		if(ret < 0)
		{
			sprintf(msg, "Connect with server port:%d wrong!\n", SERVER_PORT_NO);
			display_info(msg, clientwindow);
			close(socket_fd);
			return;
		}

		sprintf(msg, "Connect with server succeed!\n");
		display_info(msg, clientwindow);
		isconnected = TRUE;

		isenable = toolbar[0].widget;
		gtk_widget_set_sensitive(isenable, FALSE);
		isenable = client_operation_menu[0].widget;
		gtk_widget_set_sensitive(isenable, FALSE);

		for(i=1; i<=4; i++)
		{
			isenable = toolbar[i].widget;
			gtk_widget_set_sensitive(isenable, TRUE);
		}

		for(i=0; i<=2; i++)
		{
			isenable = client_operation_menu[i].widget;
			gtk_widget_set_sensitive(isenable, TRUE);
		}

		isenable = menubar[1].widget;
		gtk_widget_set_sensitive(isenable, TRUE);
	}
}

void disconnect(GtkWidget *widget, gpointer data)
{
	GtkWidget *isenable;
	char msg[512];
	if(isconnected)
	{
		close(socket_fd);
		sprintf(msg, "Close connect succeed!\n");
		display_info(msg, clientwindow);
		isconnected = FALSE;

		isenable = toolbar[0].widget;
		gtk_widget_set_sensitive(isenable, TRUE);
		isenable =client_operation_menu[0].widget;
		gtk_widget_set_sensitive(isenable, TRUE);

		for(i=1; i<=4; i++)
		{
			isenable = toolbar[i].widget;
			gtk_widget_set_sensitive(isenable, FALSE);
		}

		for(i=1; i<=2; i++)
		{
			isenable = client_operation_menu[i].widget;
			gtk_widget_set_sensitive(isenable, FALSE);
		}

		isenable = menubar[1].widget;
		gtk_widget_set_sensitive(isenable, FALSE);
	}
}

struct flight_entry_t{
	GtkWidget *flight_ID;
	GtkWidget *ticket_num;
}st_flight;

int button_buyticket(GnomeDialog *dialog, gint id, gpointer data)
{
	char msg[512];
	char send_buf[512],recv_buf[512];
	GtkWidget *mbox;

	if(id == 0)
	{
		struct flight_entry_t *p = (struct flight_entry_t *)data;
		GtkWidget *flight_ID_entry = p->flight_ID;
		GtkWidget *ticket_num_entry = p->ticket_num;

		char *str = gtk_entry_get_text(GTK_ENTRY(flight_ID_entry));
		int flight_ID = atoi(str);
		if(flight_ID <= 0 || flight_ID >10)
		{
			mbox = gnome_message_box_new("Flight input wrong!Please input again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
			gtk_widget_show(mbox);
			gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
			gtk_entry_set_text(GTK_ENTRY(flight_ID_entry), " ");
			return;
		}

		str = gtk_entry_get_text(GTK_ENTRY(ticket_num_entry));
		int ticket_num = atoi(str);
		if(ticket_num <= 0)
		{
			mbox = gnome_message_box_new("Ticket number input wrong!Please input again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
			gtk_widget_show(mbox);
			gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
			gtk_entry_set_text(GTK_ENTRY(ticket_num_entry), " ");
			return;
		}

		init_message();
		message.msg_type = BUY_TICKET;
		message.flight_ID = flight_ID;
		message.ticket_num = ticket_num;
		memcpy(send_buf, &message, sizeof(message));

		int ret =send(socket_fd, send_buf, sizeof(message), 0);
		if(ret == -1)
		{
			mbox = gnome_message_box_new("Send failed! Please send again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
			gtk_widget_show(mbox);
			gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
			return;
		}

		ret =recv(socket_fd, recv_buf, sizeof(message), 0);
		if(ret == -1)
		{
			mbox = gnome_message_box_new("Accept failed!Please send again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
			gtk_widget_show(mbox);
			gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
			return;
		}

		memcpy(&message, recv_buf, sizeof(message));
		if(message.msg_type == BUY_SUCCEED)
			sprintf(msg, "Buy succeed!Flight number:%d,Ticket Number:%d,Total price:%d\n",
				message.flight_ID, message.ticket_num, message.ticket_total_price);
		else
			sprintf(msg, "Buy failed!Flight number:%d,Surplus Tickets:%d,Need Tickets:%d\n",
				message.flight_ID, message.ticket_num, ticket_num);

		mbox = gnome_message_box_new(msg, GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
		gtk_widget_show(mbox);
		gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
		gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
	}
	else 
		gnome_dialog_close(dialog);
}

void buyticket(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label, *flight_entry;
	dialog = gnome_dialog_new(_("Buy Ticket"), GNOME_STOCK_BUTTON_OK, GNOME_STOCK_BUTTON_CANCEL, NULL);

	label = gtk_label_new("Please input flight number you want buy(1-10):");
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	flight_entry = gtk_entry_new();
	st_flight.flight_ID = flight_entry;
	gtk_entry_set_visibility(GTK_ENTRY(flight_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), flight_entry, FALSE, FALSE, 0);
	gtk_widget_show(flight_entry);

	label = gtk_label_new("Please input Ticket numbers you want buy:");
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	flight_entry = gtk_entry_new();
	st_flight.ticket_num = flight_entry;
	gtk_entry_set_visibility(GTK_ENTRY(flight_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), flight_entry, FALSE, FALSE, 0);
	gtk_widget_show(flight_entry);
	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);//set asure button as default button

	gtk_signal_connect(GTK_OBJECT(dialog), "clicked", GTK_SIGNAL_FUNC(button_buyticket), &st_flight);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_widget_show(dialog);
	gnome_dialog_set_parent(GNOME_DIALOG(dialog), GTK_WINDOW(app));
}

int button_inquireone(GnomeDialog *dialog, gint id, gpointer data)
{
	GtkWidget *flight = data;
	GtkWidget *mbox;
	char msg[512];
	char send_buf[512], recv_buf[512];
	int flight_ID;

	if(id == 0)
	{
		sprintf(msg, gtk_entry_get_text(GTK_ENTRY(flight)));
		flight_ID = atoi(msg);
		if(flight_ID <=0 || flight_ID > 10)
		{
			mbox = gnome_message_box_new("Input flight number wrong!Please input again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
			gtk_widget_show(mbox);
			gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
			gtk_entry_set_text(GTK_ENTRY(flight), " ");
			return;
		}

		init_message();
		message.msg_type = INQUIRE_ONE;
		message.flight_ID = flight_ID;
		memcpy(send_buf, &message, sizeof(message));
		int ret = send(socket_fd, send_buf, sizeof(message), 0);

		if(ret == -1)
		{
			mbox = gnome_message_box_new("Send failed!Please send again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
			gtk_widget_show(mbox);
			gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
			return;
		}

		ret = recv(socket_fd, recv_buf, sizeof(recv_buf), 0);
		if(ret == -1)
		{
			mbox = gnome_message_box_new("Accept failed!Please connect server again!\n",GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
			gtk_widget_show(mbox);
			gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(dialog));
			return;
		}

		memcpy(&message, recv_buf, sizeof(message));
		if(message.msg_type == INQUIRE_SUCCEED)
			sprintf(msg, "Inquire succeed!Flight number:%d,Surplus ticket:%d,Ticket price:%d\n",
				message.flight_ID, message.ticket_num, message.ticket_total_price);
		else
			sprintf(msg, "Inquire failed!Flight number:%d,Surplus ticket:unknown\n", message.flight_ID);
		display_info(msg, clientwindow);
		gnome_dialog_close(dialog);
	}
	else
		gnome_dialog_close(dialog);
}

void inquireone()
{
	GtkWidget *dialog;
	GtkWidget *label, *flight_entry;
	dialog = gnome_dialog_new(_("Inquire a certain flight ticket's information"), GNOME_STOCK_BUTTON_OK, GNOME_STOCK_BUTTON_CANCEL, NULL);

	label = gtk_label_new("Please input flight_ID you want inquire(1-10):");
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	flight_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(flight_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), flight_entry, FALSE, FALSE, 0);
	gtk_widget_show(flight_entry);

	gnome_dialog_set_default(GNOME_DIALOG(dialog), 0);

	gtk_signal_connect(GTK_OBJECT(dialog), "clicked", GTK_SIGNAL_FUNC(button_inquireone), flight_entry);
	gtk_widget_show(dialog);
	gnome_dialog_set_parent(GNOME_DIALOG(dialog), GTK_WINDOW(app));
}

void inquireall()
{
	int i, pos;
	char msg[512];
	char send_buf[512], recv_buf[512];
	GtkWidget *mbox;
	init_message();
	message.msg_type = INQUIRE_ALL;
	memcpy(send_buf, &message, sizeof(message));

	int ret = send(socket_fd, send_buf, sizeof(message), 0);
	if(ret == -1)
	{
		mbox = gnome_message_box_new("Send failed!Please send again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
		gtk_widget_show(mbox);
		gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
		gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(app));
		return;
	}

	ret = recv(socket_fd, recv_buf, sizeof(recv_buf), 0);
	if(ret == -1)
	{
		mbox = gnome_message_box_new("Accept failed!Please send again!", GNOME_MESSAGE_BOX_INFO, GNOME_STOCK_BUTTON_OK, NULL);
		gtk_widget_show(mbox);
		gtk_window_set_modal(GTK_WINDOW(mbox), TRUE);
		gnome_dialog_set_parent(GNOME_DIALOG(mbox), GTK_WINDOW(app));
		return;
	}

	pos = 0;
	sprintf(msg, "All result of inquiring all flight :\n");
	display_info(msg, clientwindow);
	for(i=0; i<ret; i=i+sizeof(message))
	{
		memcpy(&message, recv_buf+pos, sizeof(message));
		if(message.msg_type = INQUIRE_SUCCEED)
			sprintf(msg, "Inquire succeed!Flight number:%d,Surplus ticket:%d,Ticket price:%d\n",
				message.flight_ID, message.ticket_num, message.ticket_total_price);
		else
			sprintf(msg, "Inquire failed!Flight number:%d,Surplus ticket:unknown\n",message.flight_ID);
		display_info(msg, clientwindow);
		pos += sizeof(message);
	}
}

void dialog_ok(GnomeDialog *dialog, gint id, gpointer data)
{
	gnome_dialog_close(dialog);
}

void displaycontents(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	char msg[5][512]={
					  "Connect server:Build connection with remote server",
					  "Close connect:Close connection with remote server",
					  "Buy ticket:Buy ticket",
					  "Inquire a certain flight:inquire information for a certain flight ticket",
					  "Inquire all flight:Inquire all information for all flight"
	};
	dialog = gnome_dialog_new((_("Help")), GNOME_STOCK_BUTTON_OK, NULL, NULL);

	for(i=0; i<5; i++)
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

void about(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *contentlabel, *versionlabel, *authorlabel, *copyrightlabel;
	dialog = gnome_dialog_new(_("About the program"), GNOME_STOCK_BUTTON_OK, NULL, NULL);

	contentlabel = gtk_label_new(_("Simulate Network Sale Ticket System of Client"));
	versionlabel = gtk_label_new(_("Version:1.0"));
	copyrightlabel = gtk_label_new(_("Copyright 2016"));
	authorlabel = gtk_label_new(_("hdy"));

	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), contentlabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), versionlabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), copyrightlabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox), authorlabel, TRUE, TRUE, 0);

	gtk_widget_show(contentlabel);
	gtk_widget_show(versionlabel);
	gtk_widget_show(copyrightlabel);
	gtk_widget_show(authorlabel);

	gtk_signal_connect(GTK_OBJECT(dialog), "clicked", GTK_SIGNAL_FUNC(dialog_ok), &dialog);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_widget_show(dialog);
	gnome_dialog_set_parent(GNOME_DIALOG(dialog), GTK_WINDOW(app));
}


int main(int argc, char *argv[])
{
	GtkWidget *isenable;
	int i;

	gnome_init("example", "0.1", argc, argv);
	app = gnome_app_new("example", "Simulate Network Sale Ticket System of Client");
	gtk_signal_connect(GTK_OBJECT(app), "delete_event", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

	gnome_app_create_menus(GNOME_APP(app), menubar);
	gnome_app_create_toolbar(GNOME_APP(app), toolbar);

	if(!isconnected)
	{
		for(i=1; i<=4; i++)
		{
			isenable = toolbar[i].widget;
			gtk_widget_set_sensitive(isenable, FALSE);
		}

		for(i=1; i<=2; i++)
		{
			isenable = client_operation_menu[i].widget;
			gtk_widget_set_sensitive(isenable, FALSE);
		}

		isenable = menubar[1].widget;
		gtk_widget_set_sensitive(isenable, FALSE);
	}

	gtk_window_set_default_size((GtkWindow *)app, 800, 600);
	vbox = gtk_vbox_new(FALSE, 0);
	gnome_app_set_contents(GNOME_APP(app), vbox);

	frame = gtk_frame_new(NULL);
	gtk_frame_set_label(GTK_FRAME(frame), "Information of Client");
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

	clientwindow = gtk_text_new(NULL, NULL);
	gtk_text_set_editable(GTK_TEXT(clientwindow), FALSE);
	gtk_text_set_word_wrap(GTK_TEXT(clientwindow), TRUE);
	gtk_table_attach(GTK_TABLE(table), clientwindow, 0, 1, 0, 1, GTK_EXPAND|GTK_SHRINK|GTK_FILL, GTK_EXPAND|GTK_SHRINK|GTK_FILL, 0, 0);
	gtk_widget_show(clientwindow);

	vscroball = gtk_vscrollbar_new(GTK_TEXT(clientwindow)->vadj);
	gtk_table_attach(GTK_TABLE(table), vscroball, 1, 2, 0, 1, GTK_FILL, GTK_EXPAND|GTK_SHRINK|GTK_FILL, 0, 0);
	gtk_widget_show(vscroball);
	gtk_widget_realize(clientwindow);

	gtk_widget_show_all(app);
	//g_timeout_add(500, info_print, NULL);

	gtk_main();

	return 0;

}