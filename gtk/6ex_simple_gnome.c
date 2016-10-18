#include <gnome.h>
#include <stdio.h>

void hello(GtkWidget *widget, gpointer data)
{
	g_print("Hello World!\n");
}

gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
}

int main(int argc, char *argv[])
{
	GtkWidget *app;
	GtkWidget *mybutton;
	gnome_init(" ", "0.1", argc, argv);
	mybutton = gtk_button_new_with_label("Click me");
	app = gnome_app_new("Hello World", "Hello App");
	gnome_app_set_contents(GNOME_APP(app), mybutton);
	gtk_signal_connect(GTK_OBJECT(app), "delete_event",
		GTK_SIGNAL_FUNC(delete_event), NULL);
	gtk_signal_connect(GTK_OBJECT(mybutton), "clicked",
		GTK_SIGNAL_FUNC(hello), NULL);

	gtk_widget_show_all(app);
	gtk_main();

	return 0;
}