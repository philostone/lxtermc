/**/

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <locale.h>

#include "lxtermcapp.h"
#include "lxtermcwin.h"
#include "lxtermc.h"

struct _LxtermcApp {
	GtkApplication parent_instance;
	// subclass instance variables
	gchar *label;
	cmdargs_t *cmdargs;
};

G_DEFINE_TYPE(LxtermcApp, lxtermc_app, GTK_TYPE_APPLICATION)

static void
print_hello(GtkWidget *w, gpointer data)
{
	g_print(_("Hello!"));
	g_print(" -> w at: %p - data at: %p\n", (void *)w, (void *)data);
}

static void
display_locale(GtkWidget *w, gpointer data)
{
	char *fn = "display_locale()";
	g_print("%s - w at: %p - data at: %p\n", fn, (void *)w, (void *)data);

	g_print("%s - current msg locale    : %s\n", fn, setlocale(LC_MESSAGES, NULL));
	g_print("%s - current base dir      : %s\n", fn, bindtextdomain(GETTEXT_PACKAGE, NULL));
	g_print("%s - current codeset       : %s\n", fn, bind_textdomain_codeset(GETTEXT_PACKAGE, NULL));
	g_print("%s - current text domain   : %s\n", fn, textdomain(NULL));
	g_print("%s - gettext('Hello!')     : %s\n", fn, gettext("Hello!"));
	g_print("%s - gettext('Locale')     : %s\n", fn, gettext("Locale"));
	g_print("%s - gettext('Welcome!')   : %s\n", fn, gettext("Welcome!"));
	g_print("%s - gettext('Hello gtk4') : %s\n", fn, gettext("Hello gtk4"));
	g_print("%s - gettext('Exit')       : %s\n", fn, gettext("Exit"));
}

static void
lxtermc_app_open(GApplication *app, GFile **files, int nfiles, const char *hint)
{
	gchar *fn = "lxtermc_app_open()";
	g_print("%s - '%s' - app at: %p\n", fn, ((LxtermcApp *)app)->label, (void *)app);
	g_print("%s - '%s' - %i files as: %p\n", fn, ((LxtermcApp *)app)->label,
		nfiles, (void *)files);
	g_print("%s - '%s' - hint: %s\n", fn, ((LxtermcApp *)app)->label, hint);
}

static void
lxtermc_app_activate(GApplication *app)
{
	gchar *fn = "lxtermc_app_activate()";
	g_print("%s - '%s' - app at: %p\n",
		fn, ((LxtermcApp *)app)->label, (void *)app);

	LxtermcWin *win = lxtermc_win_new(LXTERMC_APP(app));
	gtk_window_set_title(GTK_WINDOW(win), _("Welcome!"));
	gtk_window_set_default_size(GTK_WINDOW(win), 300, 200);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

	gtk_window_set_child(GTK_WINDOW(win), box);

	GtkWidget *hello_button = gtk_button_new_with_label(_("Hello gtk4"));
	g_signal_connect(hello_button, "clicked", G_CALLBACK(print_hello), NULL);

	GtkWidget *locale_button = gtk_button_new_with_label(_("Locale"));
	g_signal_connect(locale_button, "clicked", G_CALLBACK(display_locale), NULL);

	GtkWidget *close_button = gtk_button_new_with_label(_("Exit"));
	g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(lxtermc_win_destroy), win);

	gtk_box_append(GTK_BOX(box), hello_button);
	gtk_box_append(GTK_BOX(box), locale_button);
	gtk_box_append(GTK_BOX(box), close_button);
	gtk_window_present(GTK_WINDOW(win));

	g_print("%s - end\n", fn);
}

/*
static int
lxtermc_cmdline(GApplication *app, GApplicationCommandLine *cmdline, cmdargs_t *cmdargs)
{
	char *fn = "lxtermc_cmdline()";
	g_print("%s - start - app: %p\n", fn, (void *)app);
	gint argc;
	gchar **argv = g_application_command_line_get_arguments(cmdline, &argc);

	g_print("%s - %i arguments\n", fn, argc);
	for (int i = 0; i < argc; i++) {
		g_print("%s - argument %2d: %s\n", fn, i, argv[i]);
	}
	if (!lxtermc_args(argc, argv, cmdargs)) return EXIT_FAILURE;

	setlocale(LC_ALL, "");
	g_print("%s - setting locale to %s\n", fn, setlocale(LC_MESSAGES, cmdargs->locale));
	bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

//	display_locale(NULL, NULL);

	g_strfreev(argv);

	g_application_activate(G_APPLICATION(app));

	g_print("%s - end\n", fn);
	return TRUE;
}
*/

static void
lxtermc_app_shutdown(GApplication *app)
{
	LxtermcApp *lxapp = LXTERMC_APP(app);
	g_print("lxtermc_app_shutdown() - '%s' - app at: %p\n",
		lxapp->label, (void *)app);
//		((LxtermcApp *)app)->label, (void *)app);
//	g_free(((LxtermcApp *)app)->label);
	g_free(lxapp->label);
	lxapp->label = NULL;

//	g_free(LXTERMC_APP(app)->cmdargs);
	g_free(lxapp->cmdargs);
	lxapp->cmdargs = NULL;
	G_APPLICATION_CLASS(lxtermc_app_parent_class)->shutdown(app);
}

int
lxtermc_app_cmdline(GApplication *app, GApplicationCommandLine *cmdline)
{
	char *fn = "lxtermc_app_cmdline()";
	gint argc;
	gchar **argv = g_application_command_line_get_arguments(cmdline, &argc);
	g_print("%s - '%s' - app at: %p - cmdline at: %p\n",
		fn, LXTERMC_APP(app)->label, (void *)app, (void *)cmdline);
	for (int i = 0; i < argc; i++)
		g_print("%s - arg #%i: %s\n", fn, i, argv[i]);

	if (lxtermc_args(argc, argv, LXTERMC_APP(app)->cmdargs) != TRUE) {
		g_print("%s - lxtermc_args() returned with error\n", fn);
		return FALSE;
	}

	setlocale(LC_ALL, "");
	g_print("%s - '%s' - setting locale to %s\n", fn, LXTERMC_APP(app)->label,
		setlocale(LC_MESSAGES, LXTERMC_APP(app)->cmdargs->locale));
	bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	g_strfreev(argv);

	// this implementation needs an explicit activation signal...
	g_application_activate(G_APPLICATION(app));
	g_print("%s - end\n", fn);
	return TRUE;
}

static void
lxtermc_app_dispose(GObject *obj)
{
	gchar *fn ="lxtermc_app_dispose()";
	g_print("%s - '%s' - at: %p\n", fn, LXTERMC_APP(obj)->label, (void *)obj);
	G_OBJECT_CLASS(lxtermc_app_parent_class)->dispose(obj);
}

static void
lxtermc_app_finalize(GObject *obj)
{
	gchar *fn ="lxtermc_app_finalize()";
	g_print("%s - '%s' - at: %p\n", fn, LXTERMC_APP(obj)->label, (void *)obj);
	G_OBJECT_CLASS(lxtermc_app_parent_class)->finalize(obj);
}

static void
lxtermc_app_class_init(LxtermcAppClass *class)
{
	g_print("lxtermc_app_class_init() - class at: %p\n", (void *)class);
	// virtual function overrides
	// property and signal definitions
	G_APPLICATION_CLASS(class)->open = lxtermc_app_open;
	G_APPLICATION_CLASS(class)->command_line = lxtermc_app_cmdline;
	G_APPLICATION_CLASS(class)->activate = lxtermc_app_activate;
	G_APPLICATION_CLASS(class)->shutdown = lxtermc_app_shutdown;
	G_OBJECT_CLASS(class)->dispose = lxtermc_app_dispose;
	G_OBJECT_CLASS(class)->finalize = lxtermc_app_finalize;
}

static void
lxtermc_app_init(LxtermcApp *app)
{
	// no use showing label, it is not yet set...
	// this function is called before g_object_new() returns ...
//	g_print("lxtermc_app_init() - '%s' - app at: %p\n", app->label, (void *)app);
	g_print("lxtermc_app_init() - app at: %p\n", (void *)app);
	// initializations
	app->cmdargs = g_new0(cmdargs_t, 1);
	app->cmdargs->locale = LXTERMC_DEFAULT_LOCALE;
}

LxtermcApp *
lxtermc_app_new(const gchar *label)
{
	g_print("lxtermc_app_new() - '%s'\n", label);
	LxtermcApp *app = g_object_new(LXTERMC_TYPE_APP,
		"application-id", LXTERMC_APP_ID,
		"flags", G_APPLICATION_HANDLES_COMMAND_LINE,
		NULL);
	app->label = g_strdup(label);
	return app;
}
