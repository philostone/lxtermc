
#include <gio/gio.h>
//#include <glib/gi18n.h>
#include <gtk/gtk.h>
//#include <locale.h>
#include <stdarg.h>
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
	"options: [ -o [<arg>] | -o=<arg> | --long_opt [<arg>] | --long_opt=<arg> ]\n"
	"  -e, --command <string>     execute <string> in terminal\n"
	"  -l, --login-shell          execute login shell\n"
	"  -c, --config <fname>       use file <fname> instead of user or system config\n"
	"  -t, -T, --title <title>    set <title> as terminal window title\n"
	"  --tabs <n1>[,<n2>[...]]    start tabs with names\n"
	"  --working-directory <dir>  set <dir> as working directory\n"
	"  --locale <id>              use <id> as locale instead of user's (must be installed)\n"
	"  --no-remote                do not accept or send remote commands\n"
	"  -v, --version              show version information\n"
	"  -h, --help                 show this help information\n"
};

/* return FALSE (0) if option is unmatched, TRUE (1) if matched, *at is set to -1 on error */
/*
static int
lxtermc_option(int argc, char **argv, int *at, const gchar *so, const gchar *lo, gchar **opt)
{
	gchar *fn = "lxtermc_option()";
	size_t ll = (lo) ? strlen(lo) : 0;
	gchar *arg = NULL;
	if (ll && strncmp(lo, argv[*at], ll) != 0) lo = NULL;
	if (!lo && so && strcmp(argv[*at], so) != 0) so = NULL;
	if (!so && !lo) return 0;

	g_print("%s: %s", fn, ((so) ? so : lo));

	if (lo && *(argv[*at]+ll) == '=') arg = argv[*at]+ll+1;
	if (opt) {
		g_print("%s", ((arg) ? "=" : ", "));
		if (!arg && ++(*at) >= argc) {
			g_print("expected option argument is missing!\n");
			*at = -1;
			return 1;
		} else  {
			arg = argv[*at];
		}
		*opt = arg;
	}
	g_print("%s\n", ((opt) ? *opt : ""));
	return 1;
}
*/

static int
lxtermc_option(int argc, char **argv, int *at, char **opt_arg, int num_opts, ...)
{
	char *fn = "lxtermc_option()";
	char *opt = NULL;
	va_list ap;
	va_start(ap, num_opts);
	for (int i = 0; i < num_opts; i++) {
		opt = va_arg(ap, char *);
		int l = strlen(opt);
		if (strncmp(opt, argv[*at], l) != 0) {
			opt = NULL;
			continue;
		}

		// option IS matched, up until optional '='
		if (*(argv[*at]+l) == '=') {
			g_print("%s: %s", fn, opt);
			if (!opt_arg) {
				g_print(", unexpected option argument!");
				*at = -1;
				break;
			}
			if (*opt_arg) g_free(*opt_arg);
			*opt_arg = g_strdup(argv[*at]+l+1);
			g_print("=%s\n", *opt_arg);
			break;
		}

		// not a match, after all ...
		if (*(argv[*at]+l) != '\0') {
			opt = NULL;
			continue;
		}

		// option IS matched, as it is
		g_print("%s: %s", fn, opt);
		if (!opt_arg) {
			g_print("\n");
			break;
		}

		// option requires separate argument
		(*at)++;
		if (*at >= argc) {
			g_print(", expected option argument is missing!\n");
			*at = -1;
			break;
		}
		if (*opt_arg) g_free(*opt_arg);
		*opt_arg = g_strdup(argv[*at]);
		g_print(", %s\n", *opt_arg);
		break;
	}
	va_end(ap);
	return (opt) ? 1 : 0;
}

int
lxtermc_args(int argc, char **argv, cmdargs_t *cmdargs)
{
	gchar *fn = "lxtermc_args()";
	g_print("%s - start\n", fn);
	int at = 0;
//	cmdargs->cmd = argv[0];
	while (++at < argc && at > 0) {
		if (lxtermc_option(argc, argv, &at, &(cmdargs->exec), 2, "-e", "--command"))
			continue;
		if (lxtermc_option(argc, argv, &at, NULL, 2, "-l", "--login_shell")) {
			g_print("\nnot implemented yet!\n");
			at = 0;
			break;
		}
		if (lxtermc_option(argc, argv, &at, &(cmdargs->cfg), 2, "-c", "--config"))
			continue;
		if (lxtermc_option(argc, argv, &at, &(cmdargs->title), 3, "-t", "-T", "--title"))
			continue;
		if (lxtermc_option(argc, argv, &at, NULL, 1, "--tabs")) {
			g_print("\nnot implemented yet!\n");
			at = 0;
			break;
		}
		if (lxtermc_option(argc, argv, &at, NULL, 1, "--working-directory")) {
			g_print("\nnot implemented yet!\n");
			at = 0;
			break;
		}
		if (lxtermc_option(argc, argv, &at, NULL, 1, "--no-remote")) {
			g_print("\nnot implemented yet!\n");
			at = 0;
			break;
		}
		if (lxtermc_option(argc, argv, &at, &(cmdargs->locale), 1, "--locale"))
			continue;
		if (lxtermc_option(argc, argv, &at, NULL, 2, "-v", "--version")) {
			g_print("\n"LXTERMC_NAME" - "LXTERMC_VERSION"\n");
			at = 0;
			break;
		}
		if (lxtermc_option(argc, argv, &at, NULL, 2, "-h", "--help")) {
			g_print("\n%s\n", lxtermc_usage);
			at = 0;
			break;
		}
		g_print("\n"LXTERMC_NAME" unknown option: %s\n", argv[at]);
		g_print("\n%s\n", lxtermc_usage);
		break;
	}
	g_print("lxtermc_args() - end %s\n", ((at > 0) ? "TRUE" : "FALSE"));
	return (at > 0) ? TRUE : FALSE;
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
	g_print("%s - TRUE is %i\n", fn, TRUE);

	gtk_disable_setlocale();

	// automatic resources (GtkApplication):
	// load GtkBuilder resource from gtk/menus.ui

//	GtkApplication *app = gtk_application_new(LXTERMC_APP_ID,
//		G_APPLICATION_HANDLES_COMMAND_LINE);

	LxtermcApp *app = lxtermc_app_new("= main app =");
	g_print("%s - app at: %p - starting main application loop ...\n", fn, (void *)app);

	int gtk_status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	g_print("%s - end! - status: %i\n", fn, gtk_status);
	return gtk_status;
}
