
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <stdio.h>

#define APPLICATION_ID "com.github.philostone.lxtermc-meson"

static void
print_hello(GtkWidget *w, gpointer data)
{
	g_print(_("Hello!\n"));
}

static void
activate(GtkApplication *app, gpointer data)
{
	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), _("Welcome!"));
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

	gtk_window_set_child(GTK_WINDOW(window), box);

	GtkWidget *hello_button = gtk_button_new_with_label(_("Hello gtk4"));
	g_signal_connect(hello_button, "clicked", G_CALLBACK(print_hello), NULL);

	GtkWidget *close_button = gtk_button_new_with_label(_("Exit"));
	g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_window_destroy), window);

	gtk_box_append(GTK_BOX(box), hello_button);
	gtk_box_append(GTK_BOX(box), close_button);
	gtk_window_present(GTK_WINDOW(window));
}

int
main(int argc, char **argv)
{
//#ifdef ENABLE_NLS
	fprintf(stderr, "Setting locale (NLS is enabled)\n");
	setlocale(LC_ALL, "");
	fprintf(stderr, "setting locale to %s\n", setlocale(LC_MESSAGES, "sv_SE.utf8"));
	bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
//#endif
	print_hello(NULL, NULL);
	fprintf(stderr, "current text domain   : %s\n", textdomain(NULL));
	fprintf(stderr, "current codeset       : %s\n", bind_textdomain_codeset(GETTEXT_PACKAGE, NULL));
	fprintf(stderr, "current base dir      : %s\n", bindtextdomain(GETTEXT_PACKAGE, NULL));
	fprintf(stderr, "current msg locale    : %s\n", setlocale(LC_MESSAGES, NULL));
	fprintf(stderr, "gettext('Hello!\n')   : %s\n", gettext("Hello!\n"));
	fprintf(stderr, "gettext('Welcome!')   : %s\n", gettext("Welcome!"));
	fprintf(stderr, "gettext('Hello gtk4') : %s\n", gettext("Hello gtk4"));
	fprintf(stderr, "gettext('Exit')       : %s\n", gettext("Exit"));

	// automatic resources:
	// load GtkBuilder resource from gtk/menus.ui
	gtk_disable_setlocale();
	GtkApplication *app = gtk_application_new(APPLICATION_ID, G_APPLICATION_SEND_ENVIRONMENT);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	print_hello(NULL, NULL);

	return status;
}
