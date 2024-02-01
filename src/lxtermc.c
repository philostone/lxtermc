
#include <gtk/gtk.h>
#include <stdio.h>

static void
print_hello(GtkWidget *w, gpointer d)
{
	g_print("Hello!\n");
}

static void
activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Welcome!");
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

	gtk_window_set_child(GTK_WINDOW(window), box);

	GtkWidget *hello_button = gtk_button_new_with_label("Hello gtk4");
	g_signal_connect(hello_button, "clicked", G_CALLBACK(print_hello), NULL);

	GtkWidget *close_button = gtk_button_new_with_label("Exit");
	g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_window_destroy), window);

	gtk_box_append(GTK_BOX(box), hello_button);
	gtk_box_append(GTK_BOX(box), close_button);
	gtk_window_present(GTK_WINDOW(window));
}

int
main(int argc, char **argv)
{
	print_hello(NULL, NULL);

	GtkApplication *app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
