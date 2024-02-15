
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "lxtermc.h"
#include "lxtermc-window.h"

static char lxtermc_usage[] = {
	"lxtermc - is an individually configurable terminal emulator\n"
	"\n"
	"usage:\n"
	"\n"
	"  lxtermc [options ...]\n"
	"\n"
	"options: [ -o [<arg>] | --long_opt [<arg>] | --long_opt=<arg> ]\n"
	"  -c, --config <fname>    use file <fname> instead of user or system config\n"
	"  -l, --locale <id>       use <id> as locale instead of user's (must be installed)\n"
};

/* return true (1) if option is matched, set *opt to option arg (if not NULL)
 *  */
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

static void
print_hello(GtkWidget *w, gpointer data)
{
	g_print(_("Hello!"));
	g_print("\n");
}

static void
display_locale(GtkWidget *w, gpointer data)
{
	char *fn = "display_locale()";

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

/*
static void
lxtermc_startup(GtkApplication *app, gpointer data)
{
	char *fn = "lxtermc_startup()";
	g_print("%s - start\n", fn);

	g_print("%s - end\n", fn);
}
*/

//lxtermc_activate(GApplication *app, gpointer data)
static void
lxtermc_activate(GtkApplication *app, gpointer data)
{
	char *fn = "lxtermc_activate()";
	g_print("%s - start - app: %p\n", fn, (void *)app);

	LxtermcWindow *window = lxtermc_window_new(app);
//	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), _("Welcome!"));
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

	gtk_window_set_child(GTK_WINDOW(window), box);

	GtkWidget *hello_button = gtk_button_new_with_label(_("Hello gtk4"));
	g_signal_connect(hello_button, "clicked", G_CALLBACK(print_hello), NULL);

	GtkWidget *locale_button = gtk_button_new_with_label(_("Locale"));
	g_signal_connect(locale_button, "clicked", G_CALLBACK(display_locale), NULL);

	GtkWidget *close_button = gtk_button_new_with_label(_("Exit"));
	g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_window_destroy), window);

	gtk_box_append(GTK_BOX(box), hello_button);
	gtk_box_append(GTK_BOX(box), locale_button);
	gtk_box_append(GTK_BOX(box), close_button);
	gtk_window_present(GTK_WINDOW(window));

	g_print("%s - end\n", fn);
}

static void
lxtermc_shutdown(GtkApplication *app, gpointer data)
{
	char *fn = "lxtermc_shutdown()";
	g_print("%s - start - app: %p\n", fn, (void *)app);
	g_print("%s - end\n", fn);
}

static void
lxtermc_open(GtkApplication *app, gpointer data)
{
	char *fn = "lxtermc_open()";
	g_print("%s - start\n", fn);
	g_print("%s - end\n", fn);
}

int
main(int argc, char **argv)
{
	char *fn ="lxtermc - main()";
	g_print("%s\n", fn);

	gtk_disable_setlocale();

	cmdargs_t cmdargs;
	cmdargs.exec = NULL;
	cmdargs.cfg = NULL;
	cmdargs.locale = LXTERMC_DEFAULT_LOCALE;

	// automatic resources:
	// load GtkBuilder resource from gtk/menus.ui

	GtkApplication *app = gtk_application_new(LXTERMC_APP_ID,
		G_APPLICATION_HANDLES_COMMAND_LINE);

	g_print("%s - app: %p\n", fn, (void *)app);

//	g_signal_connect(app, "startup", G_CALLBACK(lxtermc_startup), NULL);
	g_signal_connect(app, "command-line", G_CALLBACK(lxtermc_cmdline), &cmdargs);
	g_signal_connect(app, "open", G_CALLBACK(lxtermc_open), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(lxtermc_activate), NULL);
	g_signal_connect(app, "shutdown", G_CALLBACK(lxtermc_shutdown), NULL);

	int gtk_status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	g_print("%s - end! - status: %i\n", fn, gtk_status);
	return gtk_status;
}
