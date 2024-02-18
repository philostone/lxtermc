
#define LXTERMC_APP_ID "com.github.philostone.lxtermc"

#define LXTERMC_DEFAULT_LOCALE "sv_SE.utf8"

typedef struct _cmdargs {
	gchar *cmd;		// -> argv[0]
	gchar *exec;		// copy of data from -e, --command
	gchar *cfg;		// -> -c, --config data-url
	gchar *locale;		// use locale instead of user locale
	GtkWindow *win;		// -> to the top level window arguments regard
} cmdargs_t;

extern gchar lxtermc_usage[];
int lxtermc_args(int argc, char **argv, cmdargs_t *cmdargs);
