#include <gnome.h>
#include <stdio.h>

void callback(GtkWidget *widget, gpointer data)
{
	g_print("Item Selected!\n");
}

GnomeUIInfo file_menu[]={
	GNOMEUIINFO_ITEM_NONE("New", "This is the menubar info", callback),
	GNOMEUIINFO_MENU_EXIT_ITEM(gtk_main_quit, NULL),
	GNOMEUIINFO_END
};

GnomeUIInfo menubar[]={
	GNOMEUIINFO_MENU_FILE_TREE(file_menu),
	GNOMEUIINFO_END
};

GnomeUIInfo toolbar[]={
	GNOMEUIINFO_ITEM_STOCK("Print", "This is another tooltip",callback,
		GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_ITEM_STOCK("Exit","Exit the application", gtk_main_quit,
		GNOME_STOCK_PIXMAP_EXIT),
	GNOMEUIINFO_END
};

int main(int argc, char *argv[])
{
	GtkWidget *app;
	GtkWidget *button;
	GtkWidget *label;

	gnome_init("exemple", "0.1", argc, argv);
	app = gnome_app_new("exemple", "Exemple of toolbar and menu");

	gtk_signal_connect(GTK_OBJECT(app), "delete_event",
		GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

	gnome_app_create_menus(GNOME_APP(app), menubar);
	gnome_app_create_toolbar(GNOME_APP(app), toolbar);

	gtk_widget_show_all(app);
	gtk_main();

	return 0;
}

//gcc 7ex_simple_menu_and_tool_gnome.c `pkg-config --cflags --libs libgnomeui-2.0`