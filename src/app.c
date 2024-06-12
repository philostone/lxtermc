/**/

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <locale.h>

//#include "lxtermc.h"		// all components are included here
#include "lxtermc.h"
#include "app.h"
#include "win.h"

//#include "lxtc-resources.h"

struct _LxtermcApp {
	GtkApplication parent_instance;

	// subclass instance variables
	cmdargs_t	*cmdargs;	// temporary ownership, transferred to lxtermwin instance
	GdkDisplay	*display;	// default display (input and output)
	GPtrArray	*wins;		// array of pointers to lxtermcwin instances
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
/*
cmdargs_t *
lxtermc_app_steal_cmdargs(LxtermcApp *a)
{
	gchar *fn = "lxtermc_app_steal_cmdargs()";
	g_print("%s - from app at: %p\n", fn, (void *)a);
	cmdargs_t *c = a->cmdargs;
	a->cmdargs = NULL;
	return c;
}
*/
static void
close_activated(GSimpleAction *act, GVariant *param, gpointer data)
{
	gchar *fn = "close_activated()";
	g_print("%s - '%s' is activated - param: %p - data: %p\n",
		fn, g_action_get_name(G_ACTION(act)), (void *)param, (void *)data);
	gtk_window_close(GTK_WINDOW(data));
	g_print("%s - end\n", fn);
}

static void
prefs_activated(GSimpleAction *act, GVariant *param, gpointer data)
{
	char *fn = "prefs_activated()";
	g_print("%s - '%s' is activated - param: %p - data: %p\n",
		fn, g_action_get_name(G_ACTION(act)), (void *)param, (void *)data);
	g_print("%s - end\n", fn);
}

static void
lxtermc_app_activate(GApplication *app)
{
	G_APPLICATION_CLASS(lxtermc_app_parent_class)->activate(app);

	gchar *fn = "lxtermc_app_activate()";
	LxtermcApp *lxapp = LXTERMC_APP(app);
	guint numwin = lxapp->wins->len;

	g_print("%s - #wins: %i - app at: %p - cmdargs ptr: %p\n",
		fn, numwin, (void *)app, (void *)lxapp->cmdargs);

	// construct win id, create win and let lxtcwin_new() steal ownwership of lxapp->cmdargs
	gchar *id = g_strdup_printf("= win id #%u =", numwin+1);
	LxtermcWin *w = lxtermc_win_new(LXTERMC_APP(app), id);
	g_free(id);

// ToDo: transfer cfg to win... (not in win_init)
//	w->cmdargs = lxtermc_app_steal_cmdargs(w->app);
//	w->cfg = lxtccfg_load(w->cmdargs->cfg);
	lxtermc_win_set_cmd_tabs(w, lxapp->cmdargs->tabs);
	lxtermc_win_set_cfg(w, lxtccfg_load(lxapp->cmdargs->cfg));
	lxtermc_win_set_title(w, lxapp->cmdargs->title);
	lxtermc_win_construct(w);

	// map menu actions to window
	const GActionEntry win_entries[] = {
		{"close", close_activated, NULL, NULL, NULL, {0}}, // last field: gsize padding[3];
		{"prefs", prefs_activated, NULL, NULL, NULL, {0}} // last field: gsize padding[3];
	};
	g_action_map_add_action_entries(G_ACTION_MAP(w),
		win_entries, G_N_ELEMENTS(win_entries), w);

	// store windows - why ???
	g_ptr_array_add(LXTERMC_APP(app)->wins, w);
	gtk_window_present(GTK_WINDOW(w));

	gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(w), TRUE);
	g_print("%s - end\n", fn);
}

static void
lxtermc_app_shutdown(GApplication *app)
{
	gchar *fn = "lxtermc_app_shutdown()";
	g_print("%s - app at: %p\n", fn, (void *)app);

// clean up
//	g_ptr_array_foreach(LXTERMC_APP(app)->wins, lxtcwin_close, NULL);
	G_APPLICATION_CLASS(lxtermc_app_parent_class)->shutdown(app);
	g_print("%s - end\n", fn);
}

static void
lxtermc_app_startup(GApplication *app)
{
	gchar *fn = "lxtermc_app_startup()";
	g_print("%s - start - calling parent startup...\n", fn);
	G_APPLICATION_CLASS(lxtermc_app_parent_class)->startup(app);

	g_print("%s - at: %p\n", fn, (void *)app);

	GtkBuilder *b = gtk_builder_new_from_resource("/com/github/philostone/lxtermc/menu.ui");
	GMenuModel *m = G_MENU_MODEL(gtk_builder_get_object(b, "menubar"));

	gtk_application_set_menubar(GTK_APPLICATION(app), m);
	g_object_unref(m);

/*	const GActionEntry app_entries[] = {
		{"prefs", prefs_activated, NULL, NULL, NULL, {0}} // last field: gsize padding[3];
	};
	g_action_map_add_action_entries(G_ACTION_MAP(app),
		app_entries, G_N_ELEMENTS(app_entries), app);
*/
	g_print("%s - end\n", fn);
}

static int
lxtermc_app_cmdline(GApplication *app, GApplicationCommandLine *cmdline)
{
	gchar *fn = "lxtermc_app_cmdline()";
	g_print("%s - gapp at: %p - cmdline at: %p\n",
		fn, (void *)app, (void *)cmdline);

	LxtermcApp *lxapp = LXTERMC_APP(app);
	gint argc;
	gchar **argv = g_application_command_line_get_arguments(cmdline, &argc);
	for (int i = 0; i < argc; i++)
		g_print("%s - arg #%i (%i): %s\n", fn, i, argc-1, argv[i]);

	if (lxapp->cmdargs) {
		g_print("%s - cmdargs needs freeing - why???\n", fn);
		lxtermc_cmdargs_free(lxapp->cmdargs);
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
	g_print("%s - app at: %p - setting locale to %s\n",
		fn, (void *)lxapp, setlocale(LC_MESSAGES, lxapp->cmdargs->locale));
	bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	// this implementation needs an explicit activation signal...
	// (or open, if that kind of argument was to be accepted, for this impl (so far)
	// the only possible way to open a file is through -e --command arguments)
	g_application_activate(G_APPLICATION(app));
	g_print("%s - end\n", fn);
	return TRUE;
}

static void
lxtermc_app_dispose(GObject *obj)
{
	gchar *fn ="lxtermc_app_dispose()";
	g_print("%s - at: %p\n", fn, (void *)obj);
	g_print("%s - end, handing over to parent...\n", fn);
	G_OBJECT_CLASS(lxtermc_app_parent_class)->dispose(obj);
}

static void
lxtermc_app_finalize(GObject *obj)
{
	gchar *fn ="lxtermc_app_finalize()";
	LxtermcApp *a = LXTERMC_APP(obj);
	g_print("%s - at: %p\n", fn, (void *)obj);

	// maybe allocated
	if (a->cmdargs) {
		g_print("%s - cmdargs needs freeing - why???\n", fn);
		lxtermc_cmdargs_free(a->cmdargs);
	}
	g_print("%s - freeing lxwins array of %i ptrs\n", fn, a->wins->len);
	g_ptr_array_free(a->wins, TRUE);	// calls lxtcwin_free()

	g_print("%s - end, handing over to parent...\n", fn);
	G_OBJECT_CLASS(lxtermc_app_parent_class)->finalize(obj);
}

static void
lxtermc_app_class_init(LxtermcAppClass *c)
{
	gchar *fn = "lxtermc_app_class_init()";
	g_print("%s - class at: %p\n", fn, (void *)c);

	// virtual function overrides
	G_APPLICATION_CLASS(c)->startup = lxtermc_app_startup;
//	G_APPLICATION_CLASS(c)->open = lxtermc_app_open;
	G_APPLICATION_CLASS(c)->command_line = lxtermc_app_cmdline;
	G_APPLICATION_CLASS(c)->activate = lxtermc_app_activate;
	G_APPLICATION_CLASS(c)->shutdown = lxtermc_app_shutdown;
	G_OBJECT_CLASS(c)->dispose = lxtermc_app_dispose;
	G_OBJECT_CLASS(c)->finalize = lxtermc_app_finalize;

	// property and signal definitions
	g_print("%s - end\n", fn);
}

static void
lxtermc_app_init(LxtermcApp *a)
{
	gchar *fn = "lxtermc_app_init()";
	g_print("%s - app at: %p\n", fn, (void *)a);

	// initializations
	a->display = gdk_display_get_default();
	a->wins = g_ptr_array_new_with_free_func(lxtermc_win_free);
	g_print("%s - end\n", fn);
}

LxtermcApp *
lxtermc_app_new()
{
	gchar *fn = "lxtermc_app_new()";
	g_print("%s\n", fn);
	LxtermcApp *a = g_object_new(LXTERMC_TYPE_APP,
		"application-id", LXTERMC_APP_ID,
		"flags", G_APPLICATION_HANDLES_COMMAND_LINE,
		NULL);
	g_print("%s - end\n", fn);
	return a;
}
