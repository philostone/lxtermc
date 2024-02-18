
#include <gio/gio.h>
//#include <glib/gi18n.h>
#include <gtk/gtk.h>
//#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "lxtermc.h"
#include "lxtermcapp.h"
#include "lxtermcwin.h"

gchar lxtermc_usage[] = {
	"lxtermc - is a per window individually configurable terminal emulator\n"
	"\n"
	"usage:\n"
	"\n"
	"  lxtermc [options ...]\n"
	"\n"
	"options: [ -o [<arg>] | --long_opt [<arg>] | --long_opt=<arg> ]\n"
	"  -c, --config <fname>    use file <fname> instead of user or system config\n"
	"  -l, --locale <id>       use <id> as locale instead of user's (must be installed)\n"
};

/* return true if option is matched, set *opt to option arg (if not NULL)
 *  */
static int
lxtermc_option(int argc, char **argv, int *at, const gchar *so, const gchar *lo, gchar **opt)
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

int
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

/*
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
*/

int
main(int argc, char **argv)
{
	char *fn ="lxtermc - main()";
	g_print("%s\n", fn);
	g_print("%s - TRUE is %i\n", fn, TRUE);

	gtk_disable_setlocale();

	// automatic resources:
	// load GtkBuilder resource from gtk/menus.ui

//	GtkApplication *app = gtk_application_new(LXTERMC_APP_ID,
//		G_APPLICATION_HANDLES_COMMAND_LINE);

	LxtermcApp *app = lxtermc_app_new("= main app =");

	g_print("%s - app at: %p\n", fn, (void *)app);

	int gtk_status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	g_print("%s - end! - status: %i\n", fn, gtk_status);
	return gtk_status;
}
