#include <gtk/gtk.h>
#include <stdio.h>

gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	g_print("delete event occurred!\n");
	return TRUE;
}

void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *hbox;
<<<<<<< HEAD
	GtkWidget *label, *entry;
=======
	GtkWidget *lable, *entry;
>>>>>>> a5ef1da7e0f1b484b8da400bb5dca57b783653e7

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(window), "destroy", 
		GTK_SIGNAL_FUNC(destroy), NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

<<<<<<< HEAD
	hbox = gtk_hbox_new(FALSE, 5);
=======
	hbox = gtk_hbox_new(FALSE, 0);
>>>>>>> a5ef1da7e0f1b484b8da400bb5dca57b783653e7

	gtk_container_add(GTK_CONTAINER(window), hbox);
	gtk_widget_show(hbox);

<<<<<<< HEAD
	label = gtk_label_new("Entry:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);
=======
	lable = gtk_lable_new("Entry:");
	gtk_box_pack_start(GTK_BOX(hbox), lable, FALSE, FALSE, 0);
	gtk_widget_show(lable);
>>>>>>> a5ef1da7e0f1b484b8da400bb5dca57b783653e7

	entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);
	gtk_widget_show(entry);

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}