
#define LXTERMC_APP_ID "com.github.philostone.lxtermc"

#define LXTERMC_DEFAULT_LOCALE "sv_SE.utf8"

typedef struct _cmdargs {
	char *cmd;		// -> argv[0]
	gchar *exec;		// copy of -e, --command
	char *cfg;		// -> -c, --config data
	char *locale;		// use locale instead of user locale
} cmdargs_t;
