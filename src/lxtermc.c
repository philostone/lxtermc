
#include <gio/gio.h>
//#include <glib/gi18n.h>
#include <gtk/gtk.h>
//#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>		// for unix
#include <glib-unix.h>		// signals handling
//#include <X11/Xlib.h>

#include "lxtermc.h"		// all components are included here

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
	"  --config-ro <fname>        load <fname> as config, but do not alter it\n"
	"  -t, -T, --title <title>    set <title> as terminal window title\n"
	"  --tabs <n1>[,<n2>[...]]    start tabs with names, multiple statements are added\n"
	"  --working-directory <dir>  set <dir> as working directory\n"
	"  --locale <id>              use <id> as locale instead of user's (must be installed)\n"
	"  --no-remote                do not accept or send remote commands\n"
	"  -v, --version              show version information\n"
	"  -h, --help                 show this help information\n"
};

/* test argument at argv[*at] against the num_optids long list of option strings and,
 * if opt_arg is not NULL, copy option argument to *opt_arg, either from within current argument
 * as the data folowing a =, or from next argument,
 * return TRUE (1) if option is matched, FALSE (0) otherwise (*at is left unchanged),
 * increase *at if a separate option argument is consumed and set *at to -1 if expected
 * option data is not found or if unexpected option data is found (i.e. an unexpected =)
*/
static int
lxtermc_is_opt(int argc, char **argv, int *at, char **opt_arg, int num_optids, ...)
{
	char *fn = "lxtermc_is_opt()";
	char *opt = NULL;
	va_list ap;
	va_start(ap, num_optids);
	for (int i = 0; i < num_optids; i++) {
		opt = va_arg(ap, char *);
		int l = strlen(opt);

		// argument does NOT match option
		if (strncmp(opt, argv[*at], l) != 0) {
			opt = NULL;
			continue;
		}

		// argument IS matched, WITH trailing '='
		if (*(argv[*at]+l) == '=') {
			g_print("%s: %s", fn, opt);
			if (!opt_arg) {
				g_print(", unexpected option argument!");
				*at = -1;
				break;
			}
			if (*opt_arg) {
				g_free(*opt_arg);
				g_print(", earlier option argument had to be freed, why?");
			}
			*opt_arg = g_strdup(argv[*at]+l+1);
			g_print("=%s\n", *opt_arg);
			break;
		}

		// not a match, after all ...
		if (*(argv[*at]+l) != '\0') {
			opt = NULL;
			continue;
		}

		// option IS matched, as it is, no option argument expected
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
		g_print(": %s\n", *opt_arg);
		break;
	}
	va_end(ap);
	return (opt) ? 1 : 0;
}

int
lxtermc_args(int argc, char **argv, cmdargs_t *cargs)
{
	gchar *fn = "lxtermc_args()";
	g_print("%s - parsing %i arguments (including argv[0] = '%s')\n", fn, argc, argv[0]);

	int at = 0;	// at is set to -1 by is_opt() on (option argument expectation) error
	// cargs->cmd = argv[0] -> not used
	while (++at < argc && at > 0) {
		if (lxtermc_is_opt(argc, argv, &at, &(cargs->exec), 2, "-e", "--command"))
			continue;
		if (lxtermc_is_opt(argc, argv, &at, NULL, 2, "-l", "--login_shell")) {
			g_print("\nnot implemented yet!\n");
			at = 0;
			break;
		}
		if (lxtermc_is_opt(argc, argv, &at, &(cargs->cfg), 2, "-c", "--config"))
			continue;
		if (lxtermc_is_opt(argc, argv, &at, &(cargs->cfg), 2, "--config-ro")) {
			cargs->cfg_ro = true;
			continue;
		}
		if (lxtermc_is_opt(argc, argv, &at, &(cargs->title), 3, "-t", "-T", "--title"))
			continue;
		if (lxtermc_is_opt(argc, argv, &at, &(cargs->tabs), 1, "--tabs"))
			continue;
		if (lxtermc_is_opt(argc, argv, &at, NULL, 1, "--working-directory")) {
			g_print("\nnot implemented yet!\n");
			at = 0;
			break;
		}
		if (lxtermc_is_opt(argc, argv, &at, NULL, 1, "--no-remote")) {
			g_print("\nnot implemented yet!\n");
			at = 0;
			break;
		}
		if (lxtermc_is_opt(argc, argv, &at, &(cargs->locale), 1, "--locale"))
			continue;
		if (lxtermc_is_opt(argc, argv, &at, NULL, 2, "-v", "--version")) {
			g_print("\n"LXTERMC_NAME" - "LXTERMC_VERSION"\n");
			at = 0;
			break;
		}
		if (lxtermc_is_opt(argc, argv, &at, NULL, 2, "-h", "--help")) {
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

void
lxtermc_free_str_at(char **ptr)
{
	gchar *fn = "lxtermc_free_str_at()";
	if (ptr) {
		g_free(*ptr);
		*ptr = NULL;
	} else {
		fprintf(stderr, "%s - NULL pointer, nothing is freed...\n", fn);
	}
}

void
lxtermc_free_cmdargs_at(cmdargs_t **cargs)
{
	char *fn = "lxtermc_clear_cmdargs()";
	g_print("%s - start!\n", fn);
	if (!cargs || !(*cargs)) {
		g_print("%s - NULL pointer, nothing is freed...\n", fn);
		return;
	}

	// *cargs is pointer to cmdargs_t struct
	// (*cargs)->exe is pointer to char
	// &((*cargs)->exe) is pointer to the pointer to the char
	lxtermc_free_str_at(&((*cargs)->exec));
	lxtermc_free_str_at(&((*cargs)->cfg));
	lxtermc_free_str_at(&((*cargs)->title));
	lxtermc_free_str_at(&((*cargs)->tabs));

////	g_ptr_array_foreach((*cargs)->tabs, lxtc_gfunc_free, NULL);
//	g_ptr_array_free((*cargs)->tabs, TRUE);
	lxtermc_free_str_at(&((*cargs)->locale));
	*cargs = NULL;
}

static gint
handle_sighup(gpointer data)
{
	gchar *fn = "handle_sighup()";
	g_print("%s - data at: %p\n", fn, (void *)data);
	g_application_quit(G_APPLICATION(data));
	return G_SOURCE_CONTINUE;
}

static gint
handle_sigint(gpointer data)
{
	gchar *fn = "handle_sigint()";
	g_print("%s - data at: %p\n", fn, (void *)data);
	g_application_quit(G_APPLICATION(data));
	return G_SOURCE_CONTINUE;
}

static gint
handle_sigterm(gpointer data)
{
	gchar *fn = "handle_sighup()";
	g_print("%s - data at: %p\n", fn, (void *)data);
	g_application_quit(G_APPLICATION(data));
	return G_SOURCE_CONTINUE;
}

static gint
handle_sigusr1(gpointer data)
{
	gchar *fn = "handle_sigusr1()";
	g_print("%s - data at: %p\n", fn, (void *)data);
	return G_SOURCE_CONTINUE;
}

static gint
handle_sigusr2(gpointer data)
{
	gchar *fn = "handle_sigusr2()";
	g_print("%s - data at: %p\n", fn, (void *)data);
	return G_SOURCE_CONTINUE;
}

static gint
handle_sigwinch(gpointer data)
{
	gchar *fn = "handle_sigwinch()";
	g_print("%s - data at: %p\n", fn, (void *)data);
	return G_SOURCE_CONTINUE;
}

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

//	LxtermcApp *app = lxtermc_app_new("= main app =");
//	LxtermcApp *app = lxtermc_app_new();
	GtkWidget *app = lxtermc_app_new();
	g_print("%s - app at: %p - starting main application loop ...\n", fn, (void *)app);

	g_unix_signal_add(SIGHUP, handle_sighup, app);
	g_unix_signal_add(SIGINT, handle_sigint, app);
	g_unix_signal_add(SIGTERM, handle_sigterm, app);
	g_unix_signal_add(SIGUSR1, handle_sigusr1, app);
	g_unix_signal_add(SIGUSR2, handle_sigusr2, app);
	g_unix_signal_add(SIGWINCH, handle_sigwinch, app);

	int gtk_status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	g_print("%s - end! - status: %i\n", fn, gtk_status);
	return gtk_status;
}
