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

/*GnomeUIInfo menubar[]={
	GNOMEUIINFO_MENU_FILE_TREE(file_menu),
	GNOMEUIINFO_END
};
*/
  GnomeUIInfo menubar[]={
	{GNOME_APP_UI_SUBTREE, "Top menu", NULL, file_menu, NULL, NULL, 0, NULL, 0, 0, NULL},
	{ GNOME_APP_UI_ENDOFINFO, ((void *)0), ((void *)0), ((void *)0), ((void *)0), ((void *)0), (GnomeUIPixmapType) 0, ((void *)0), 0, (GdkModifierType) 0, ((void *)0) }

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
	GtkWidget *sep;
	GtkWidget *w;

	//gnome_init("exemple", "0.1", argc, argv);
	gnome_program_init("gnome1", "0.1", LIBGNOMEUI_MODULE, argc, argv, GNOME_PARAM_NONE);
	app = gnome_app_new("gnome1", "Exemple of toolbar and menu");
	gtk_window_set_default_size(GTK_WINDOW(app), 300, 200);

	gtk_signal_connect(GTK_OBJECT(app), "delete_event",
		GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

	w = gnome_appbar_new(FALSE, TRUE, GNOME_PREFERENCES_USER);
	gnome_app_set_statusbar(GNOME_APP(app), w);

	gnome_app_install_menu_hints(GNOME_APP(app), menubar);


	//app = gnome_app_new("exemple", "Exemple of toolbar and menu");



	gnome_app_create_menus(GNOME_APP(app), menubar);


	//gnome_app_create_toolbar(GNOME_APP(app), toolbar);

	gtk_widget_show_all(app);
	gtk_main();

	return 0;
}

//gcc 7ex_simple_menu_and_tool_gnome.c `pkg-config --cflags --libs libgnomeui-2.0`