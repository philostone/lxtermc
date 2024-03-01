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
	cmdargs_t *cmdargs;		// temporary ownership, transferred to lxtermwin instance
//	GPtrArray *lxwins;		// array of pointers to lxtermcwin instances
};

G_DEFINE_TYPE(LxtermcApp, lxtermc_app, GTK_TYPE_APPLICATION)
/*
static void
lxtermc_app_open(GApplication *app, GFile **files, int nfiles, const char *hint)
{
	gchar *fn = "lxtermc_app_open()";
	g_print("%s - '%s' - app at: %p\n", fn, ((LxtermcApp *)app)->label, (void *)app);
	g_print("%s - '%s' - %i files as: %p\n", fn, ((LxtermcApp *)app)->label,
		nfiles, (void *)files);
	g_print("%s - '%s' - hint: %s\n", fn, ((LxtermcApp *)app)->label, hint);
}
*/
static void
lxtermc_app_activate(GApplication *app)
{
	gchar *fn = "lxtermc_app_activate()";
	LxtermcApp *lxapp = LXTERMC_APP(app);
	g_print("%s - '%s' - app at: %p\n", fn, lxapp->label, (void *)app);

	GList *winlist = gtk_application_get_windows(GTK_APPLICATION(app));
	guint numwin = g_list_length(winlist);
	gchar *label = g_strdup_printf("= win label #%u =", numwin+1);

	LxtermcWin *win = lxtermc_win_new(lxapp, label);
	g_free(label);
// TODO: is it necessary to keep track of wins???

	// transfer cmdargs struct ownership to the new window
	lxtermc_win_set_cmdargs(win, lxapp->cmdargs);
	lxapp->cmdargs = NULL;

	lxtermc_win_construct(win);
/*
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

	gtk_window_set_child(GTK_WINDOW(win), box);

	GtkWidget *hello_button = gtk_button_new_with_label(_("Hello gtk4"));
	g_signal_connect(hello_button, "clicked", G_CALLBACK(print_hello), NULL);

	GtkWidget *locale_button = gtk_button_new_with_label(_("Locale"));
	g_signal_connect(locale_button, "clicked", G_CALLBACK(display_locale), NULL);

	GtkWidget *close_button = gtk_button_new_with_label(_("Exit"));
	g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(lxtermc_win_close), win);
//	g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(lxtermc_win_destroy), win);

	gtk_box_append(GTK_BOX(box), hello_button);
	gtk_box_append(GTK_BOX(box), locale_button);
	gtk_box_append(GTK_BOX(box), close_button);
*/
	gtk_window_present(GTK_WINDOW(win));

	g_print("%s - end\n", fn);
}

static void
lxtermc_app_shutdown(GApplication *app)
{
	LxtermcApp *lxapp = LXTERMC_APP(app);
	g_print("lxtermc_app_shutdown() - '%s' - app at: %p\n",
		lxapp->label, (void *)app);
	G_APPLICATION_CLASS(lxtermc_app_parent_class)->shutdown(app);
}

static int
lxtermc_app_cmdline(GApplication *app, GApplicationCommandLine *cmdline)
{
	char *fn = "lxtermc_app_cmdline()";
	LxtermcApp *lxapp = LXTERMC_APP(app);
	gint argc;
	gchar **argv = g_application_command_line_get_arguments(cmdline, &argc);
	g_print("%s - '%s' - app at: %p - cmdline at: %p\n",
		fn, lxapp->label, (void *)lxapp, (void *)cmdline);
	for (int i = 0; i < argc; i++)
		g_print("%s - arg #%i: %s\n", fn, i, argv[i]);

	if (lxapp->cmdargs) {
		g_print("%s - cmdargs needs freeing - why???\n", fn);
		lxtermc_clear_cmdargs(&(lxapp->cmdargs));
	}
	lxapp->cmdargs = g_new0(cmdargs_t, 1);
	if (lxtermc_args(argc, argv, lxapp->cmdargs) != TRUE) {
		g_print("%s - lxtermc_args() returned with error\n", fn);
		return FALSE;
	}
	g_strfreev(argv);

	if (!lxapp->cmdargs->cfg) {
		gchar **cfg = &(lxapp->cmdargs->cfg);
		g_print("%s - no config provided, try finding user's...\n", fn);
		const gchar *usercfgdir = g_get_user_config_dir();
		*cfg = g_build_filename(usercfgdir, LXTERMC_NAME ".conf", NULL);
		if (!g_file_test(*cfg, G_FILE_TEST_EXISTS)) {
			g_free(*cfg);
			g_print("%s - user config not found, try finding system's...\n", fn);
			*cfg = g_build_filename(LXTERMC_DATA_DIR, LXTERMC_NAME".conf", NULL);
			if (!g_file_test(*cfg, G_FILE_TEST_EXISTS)) {
				g_free(*cfg);
				g_print("%s - no config found, will be using defaults\n", fn);
				*cfg = NULL;
			}
		}
		if (*cfg) g_print("%s - '%s' used as config\n", fn, *cfg);
	}

	if (!lxapp->cmdargs->locale)
		lxapp->cmdargs->locale = g_strdup(LXTERMC_DEFAULT_LOCALE);
	setlocale(LC_ALL, "");
	g_print("%s - '%s' - at: %p - setting locale to %s\n",
		fn, lxapp->label, (void *)lxapp, setlocale(LC_MESSAGES, lxapp->cmdargs->locale));
	bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	// this implementation needs an explicit activation signal...
	// (or open, if that kind of argument was to be accepter, for this impl (so far)
	// the only possible way is through -e --command arguments)
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
	LxtermcApp *lxapp = LXTERMC_APP(obj);
	g_print("%s - '%s' - at: %p\n", fn, lxapp->label, (void *)obj);
	if (lxapp->label) g_free(lxapp->label);
	lxapp->label = NULL;

	// maybe allocated
	if (lxapp->cmdargs) {
		g_print("%s - cmdargs needs freeing - why???\n", fn);
		lxtermc_clear_cmdargs(&(lxapp->cmdargs));
	}
//	g_ptr_array_foreach(lxapp->lxwins, lxtermc_win_destroy, lxapp->);

	G_OBJECT_CLASS(lxtermc_app_parent_class)->finalize(obj);
}

static void
lxtermc_app_startup(GApplication *app)
{
	gchar *fn = "lxtermc_app_startup()";
	LxtermcApp *lxapp = LXTERMC_APP(app);
	g_print("%s - '%s' - at: %p\n", fn, lxapp->label, (void *)app);
	G_APPLICATION_CLASS(lxtermc_app_parent_class)->startup(app);
}

static void
lxtermc_app_class_init(LxtermcAppClass *class)
{
	g_print("lxtermc_app_class_init() - class at: %p\n", (void *)class);
	// virtual function overrides
	// property and signal definitions
	G_APPLICATION_CLASS(class)->startup = lxtermc_app_startup;
//	G_APPLICATION_CLASS(class)->open = lxtermc_app_open;
	G_APPLICATION_CLASS(class)->command_line = lxtermc_app_cmdline;
	G_APPLICATION_CLASS(class)->activate = lxtermc_app_activate;
	G_APPLICATION_CLASS(class)->shutdown = lxtermc_app_shutdown;
	G_OBJECT_CLASS(class)->dispose = lxtermc_app_dispose;
	G_OBJECT_CLASS(class)->finalize = lxtermc_app_finalize;
}

static void
lxtermc_app_init(LxtermcApp *app)
{
	char *fn = "lxtermc_app_init()";
	// no use showing label, it is not yet set...
	// this function is called before g_object_new() returns ...
//	g_print("lxtermc_app_init() - '%s' - app at: %p\n", app->label, (void *)app);
	g_print("%s - app at: %p\n", fn, (void *)app);
	// initializations
//	app->lxwins = g_ptr_array_new();
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
