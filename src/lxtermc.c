
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "lxtermc.h"

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
//	printf("lxtermc_option() - start\n");

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
	printf("%s: %s%s%s\n", fn,
		(so) ? so : lo,
		(*opt) ? ", " : "",
		(*opt) ? *opt : ""
	);
	return TRUE;
}

static int
lxtermc_args(int argc, char **argv, cmdargs_t *cmdargs)
{
	printf("lxtermc_args() - start\n");
	int at = 0;
	cmdargs->cmd = argv[0];
	while (++at < argc) {
		if (lxtermc_option(argc, argv, &at, "-c", "--config", &cmdargs->cfg)) continue;
		if (lxtermc_option(argc, argv, &at, "-l", "--locale", &cmdargs->locale)) continue;

		printf("\nlxtermc unknown option: %s\n", argv[at]);
		printf("\n%s\n", lxtermc_usage);
		printf("lxtermc_args() - end FALSE\n");
		return FALSE;
	}
	printf("lxtermc_args() - end TRUE\n");
	return TRUE;
}

static void
print_hello(GtkWidget *w, gpointer data)
{
	g_print(_("Hello!\n"));
}

static void
lxtermc_activate(GApplication *app, gpointer data)
//activate(GtkApplication *app, gpointer data)
{
	char *fn = "lxtermc_activate()";
	printf("%s - start\n", fn);
//	GtkWidget *window = gtk_application_window_new(app);
	GtkWidget *window = gtk_window_new();
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

	printf("%s - end\n", fn);
}

static int
lxtermc_cmdline(GApplication *app, GApplicationCommandLine *cmdline)
{
	char *fn = "lxtermc_cmdline()";
	printf("%s - start\n", fn);
	gint argc;
	gchar **argv = g_application_command_line_get_arguments(cmdline, &argc);

	for (int i = 0; i < argc; i++) {
		printf("%s - argument %2d: %s\n", fn, i, argv[i]);
	}

	cmdargs_t cmdargs;
	cmdargs.exec = NULL;
	cmdargs.cfg = NULL;
	cmdargs.locale = LXTERMC_DEFAULT_LOCALE;
	if (!lxtermc_args(argc, argv, &cmdargs)) return EXIT_FAILURE;

	setlocale(LC_ALL, "");
	printf("%s - setting locale to %s\n", fn, setlocale(LC_MESSAGES, cmdargs.locale));
	bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

//	print_hello(NULL, NULL);	// test gettext translation

	printf("%s - current msg locale    : %s\n", fn, setlocale(LC_MESSAGES, NULL));
	printf("%s - current base dir      : %s\n", fn, bindtextdomain(GETTEXT_PACKAGE, NULL));
	printf("%s - current codeset       : %s\n", fn, bind_textdomain_codeset(GETTEXT_PACKAGE, NULL));
	printf("%s - current text domain   : %s\n", fn, textdomain(NULL));
	printf("%s - gettext('Hello!\n')   : %s\n", fn, gettext("Hello!\n"));
	printf("%s - gettext('Welcome!')   : %s\n", fn, gettext("Welcome!"));
	printf("%s - gettext('Hello gtk4') : %s\n", fn, gettext("Hello gtk4"));
	printf("%s - gettext('Exit')       : %s\n", fn, gettext("Exit"));

	g_strfreev(argv);
	printf("%s - end\n", fn);
	return 0;
}

int
main(int argc, char **argv)
{
	printf("lxtermc - main()\n");
/*
	cmdargs_t cmdargs;
	cmdargs.exec = NULL;
	cmdargs.cfg = NULL;
	cmdargs.locale = LXTERMC_DEFAULT_LOCALE;
	if (!lxtermc_args(argc, argv, &cmdargs)) return EXIT_FAILURE;

	printf("Setting locale\n");
	setlocale(LC_ALL, "");
	printf("setting locale to %s\n", setlocale(LC_MESSAGES, cmdargs.locale));
	bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	print_hello(NULL, NULL);	// test gettext translation

	printf("current msg locale    : %s\n", setlocale(LC_MESSAGES, NULL));
	printf("current base dir      : %s\n", bindtextdomain(GETTEXT_PACKAGE, NULL));
	printf("current codeset       : %s\n", bind_textdomain_codeset(GETTEXT_PACKAGE, NULL));
	printf("current text domain   : %s\n", textdomain(NULL));
	printf("gettext('Hello!\n')   : %s\n", gettext("Hello!\n"));
	printf("gettext('Welcome!')   : %s\n", gettext("Welcome!"));
	printf("gettext('Hello gtk4') : %s\n", gettext("Hello gtk4"));
	printf("gettext('Exit')       : %s\n", gettext("Exit"));
*/

	// automatic resources:
	// load GtkBuilder resource from gtk/menus.ui

	gtk_disable_setlocale();

//	GtkApplication *app = gtk_application_new(LXTERMC_APP_ID, G_APPLICATION_DEFAULT_FLAGS);
//		G_APPLICATION_HANDLES_COMMAND_LINE);

	GApplication *app = g_application_new(LXTERMC_APP_ID, G_APPLICATION_HANDLES_COMMAND_LINE);
	g_signal_connect(app, "command-line", G_CALLBACK(lxtermc_cmdline), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(lxtermc_activate), NULL);

//	int gtk_status = g_application_run(G_APPLICATION(app), 0, NULL);
	int gtk_status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	printf("lxtermc - main() - end! - status: %i\n", gtk_status);
	return gtk_status;
}
