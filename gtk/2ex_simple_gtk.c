#include <gtk/gtk.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	GtkWidget *window1;
	gtk_init(&argc, &argv);
	window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(window1), "delete_event",
		GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

	gtk_widget_show(window1);
	gtk_main();



	return 0;
}