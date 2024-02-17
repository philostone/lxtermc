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
};

//typedef struct _LxtermcWindowPrivate LxtermcWindowPrivate;
/*
typedef struct _LxtermcAppPrivate {
	char *tobeused;
} LxtermcAppPrivate;
*/

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
lxtermc_app_activate(GApplication *app, gpointer data)
{
	gchar *fn = "lxtermc_app_activate()";
	g_print("%s - '%s' - app at: %p - data at: %p\n",
		fn, ((LxtermcApp *)app)->label, (void *)app, (void *)data);

	LxtermcWin *win = lxtermc_win_new(LXTERMC_APP(app));
//	GtkWidget *window = gtk_application_window_new(app);
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
	g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(win->destroy), win);

	gtk_box_append(GTK_BOX(box), hello_button);
	gtk_box_append(GTK_BOX(box), locale_button);
	gtk_box_append(GTK_BOX(box), close_button);
	gtk_window_present(GTK_WINDOW(win));

	g_print("%s - end\n", fn);
}

/* return true (1) if option is matched, set *opt to option arg (if not NULL)
 *  */
/*
static int
lxtermc_option(int argc, char **argv, int *at, const char *so, const char *lo, char **opt)
{
	char *fn = "lxtermc_option()";

	// buffer to hold the longest option (including --) plus = and \0
	char lx[22] = { "" };
	size_t ll = (lo) ? strlen(lo)+1 : 0;
	if (ll > 20) {
		fprintf(stderr,
			"%s - too long option (>20 chars) encountered, aborting!\n"
			"-> %s\n", fn, lo);
		return FALSE;
	}
	if (ll) {
		strcpy(lx, lo);
		strcat(lx, "=");
		if (strncmp(argv[*at], lx, ll) == 0) {
			if (opt) *opt = argv[*at]+ll;
			printf("%s: %s=%s\n", fn, lo, (opt) ? *opt : "NULL");
			return TRUE;
		}
		if (strcmp(argv[*at], lo) != 0) lo = NULL;
	}
	if (!lo && strcmp(argv[*at], so) != 0) so = NULL;
	if (!lo && !so) return FALSE;
	(*at)++;
	if (opt) *opt = (*at < argc) ? argv[*at] : NULL;
	g_print("%s: %s%s%s\n", fn,
		(so) ? so : lo,
		(*opt) ? ", " : "",
		(*opt) ? *opt : ""
	);
	return TRUE;
}
*/
/*
static int
lxtermc_args(int argc, char **argv, cmdargs_t *cmdargs)
{
	g_print("lxtermc_args() - start\n");
	int at = 0;
	cmdargs->cmd = argv[0];
	while (++at < argc) {
		if (lxtermc_option(argc, argv, &at, "-c", "--config", &cmdargs->cfg)) continue;
		if (lxtermc_option(argc, argv, &at, "-l", "--locale", &cmdargs->locale)) continue;

		g_print("\nlxtermc unknown option: %s\n", argv[at]);
		g_print("\n%s\n", lxtermc_usage);
		g_print("lxtermc_args() - end FALSE\n");
		return FALSE;
	}
	g_print("lxtermc_args() - end TRUE\n");
	return TRUE;
}
*/

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
	g_print("lxtermc_app_shutdown() - '%s' - app at: %p\n",
		((LxtermcApp *)app)->label, (void *)app);
	g_free(((LxtermcApp *)app)->label);
	G_APPLICATION_CLASS(lxtermc_app_parent_class)->shutdown(app);
}

static int
lxtermc_app_cmdline(GApplication *app, GApplicationCommandLine *cmdline)
{
	char *fn = "lxtermc_app_cmdline()";
	gint argc;
	gchar **argv = g_application_command_line_get_arguments(cmdline, &argc);
	g_print("%s - '%s' - app at: %p - cmdline at: %p\n",
		fn, ((LxtermcApp *)app)->label, (void *)app, (void *)cmdline);
	for (int i = 0; i < argc; i++)
		g_print("%s - arg #%i: %s\n", fn, i, argv[i]);
	g_strfreev(argv);

	// this implementation needs an explicit activation signal...
	g_application_activate(app);
	g_print("%s - end\n", fn);
	return TRUE;
}
/*
static gboolean
lxtermc_app_cmdline(GApplication *app, gchar ***args, gint *status)
{
	char *fn = "lxtermc_app_cmdline()";
	g_print("%s - app at: %p - args at: %p - status: %i\n",
		fn, (void *)app, (void *)args, *status);

	*status = 0;
	gchar **argv = *args;
	if (!argv[0]) return FALSE;

	gint i = 0;
	while (argv[i]) {
		g_print("%s - argument: %s\n", fn, argv[i]);
		// if handled:
		// g_free(argv[i]);
		// for (int j = i; argv[j]; j++) argv[j] = argv[j+1];
		i++;
	}

	g_print("%s - end\n", fn);
	return FALSE;
}
*/
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
//	G_APPLICATION_CLASS(class)->local_command_line = lxtermc_app_cmdline;
}

static void
lxtermc_app_init(LxtermcApp *app)
{
	// no use showing label, it is not yet set...
//	g_print("lxtermc_app_init() - '%s' - app at: %p\n", app->label, (void *)app);
	g_print("lxtermc_app_init() - app at: %p\n", (void *)app);
//	g_signal_connect(app, "activate", G_CALLBACK(lxtermc_app_activate), NULL);
	// initializations
}

LxtermcApp *
lxtermc_app_new(const gchar *label)
{
	g_print("lxtermc_app_new() - '%s'\n", label);
	// g_object_new(LXTERMC_TYPE_WINDOW, NULL);
	//return LXTERMC_WINDOW(gtk_application_window_new(app));
//	return g_object_new(LXTERMC_WINDOW_TYPE, )
	LxtermcApp *app = g_object_new(LXTERMC_TYPE_APP,
		"application-id", LXTERMC_APP_ID,
		"flags", G_APPLICATION_HANDLES_COMMAND_LINE,
		NULL);
	app->label = g_strdup(label);
	return app;
}
