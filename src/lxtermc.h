
#define LXTERMC_APP_ID "com.github.philostone.lxtermc"
#define LXTERMC_NAME "lxtermc"

// provided by meson.build
//#define LXTERMC_VERSION "0.0.1"

#define LXTERMC_DEFAULT_LOCALE "sv_SE.utf8"

// TODO: change this to rows and cols
#define LXTERMC_DEFAULT_WIDTH 300
#define LXTERMC_DEFAULT_HEIGHT 200

typedef struct _cmdargs {
//	char *cmd;		// -> argv[0]
	char *exec;		// copy of data from -e, --command
	char *cfg;		// -> -c argument
	char *title;		// -> -t argument
	char *locale;		// use locale instead of user locale
	GtkWindow *win;		// -> to the top level window that the arguments regard
} cmdargs_t;

extern gchar lxtermc_usage[];
int lxtermc_args(int argc, char **argv, cmdargs_t *cmdargs);
