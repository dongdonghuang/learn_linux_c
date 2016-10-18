#include <gtk/gtk.h>
#include <stdio.h>

void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

void callback_ok(GtkWidget *widget, gpointer data)
{
	printf("Asure press button!\n");
	gtk_main_quit();
}

void callback_cancel(GtkWidget *widget, gpointer data)
{
	printf("Cancel press button!\n");
	gtk_main_quit();
}

int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *vbox, *hbox1, *hbox2, *hbox3;
	GtkWidget *label1, *label2;
	GtkWidget *entry1, *entry2;
	GtkWidget *sep;
	GtkWidget *button_ok, *button_cancel;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(window), "destroy",
		GTK_SIGNAL_FUNC(destroy), NULL);

	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
	gtk_widget_show(hbox1);

	label1 = gtk_label_new("Name:");
	gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);
	gtk_widget_show(label1);

	entry1 = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox1), entry1, TRUE, TRUE, 0);
	gtk_widget_show(entry1);

	hbox2 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
	gtk_widget_show(hbox2);

	label2 = gtk_label_new("Phone Number:");
	gtk_box_pack_start(GTK_BOX(hbox2), label2, FALSE, FALSE, 0);
	gtk_widget_show(label2);

	entry2 = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox2), entry2, TRUE, TRUE, 0);
	gtk_widget_show(entry2);

	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);
	gtk_widget_show(sep);	

	hbox3 = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 0);
	gtk_widget_show(hbox3);

	button_ok = gtk_button_new_with_label("Asure");
	gtk_box_pack_start(GTK_BOX(hbox3), button_ok, TRUE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button_ok), "clicked",
		GTK_SIGNAL_FUNC(callback_ok), entry1);
	gtk_widget_show(button_ok);

	button_cancel = gtk_button_new_with_label("Cancel");
	gtk_box_pack_start(GTK_BOX(hbox3), button_cancel, TRUE, TRUE, 0);
	gtk_signal_connect(GTK_OBJECT(button_cancel), "clicked",
		GTK_SIGNAL_FUNC(callback_cancel), NULL);
	gtk_widget_show(button_cancel);

	gtk_widget_show_all(window);
	gtk_main();
	return 0;

}