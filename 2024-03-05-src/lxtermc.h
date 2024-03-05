
#ifndef _lxtermc_h_
#define _lxtermc_h_

#include <gtk/gtk.h>
#include <glib.h>

#define LXTERMC_APP_ID				"com.github.philostone.lxtermc"
#define LXTERMC_NAME				"lxtermc"
#define LXTERMC_DEFAULT_LOCALE			"sv_SE.utf8"

#define LXTERMC_DEFAULT_DISALLOW_BOLD		FALSE
#define LXTERMC_DEFAULT_BOLD_BRIGHT		FALSE
#define LXTERMC_DEFAULT_CURSOR_BLINK		FALSE
#define LXTERMC_DEFAULT_CURSOR_UNDERLINE	FALSE
#define LXTERMC_DEFAULT_AUDIBLE_BELL		FALSE
#define LXTERMC_DEFAULT_VISUAL_BELL		FALSE
#define LXTERMC_DEFAULT_TAB_POS			"top"
#define LXTERMC_DEFAULT_SCROLLBACK		1000
#define LXTERMC_DEFAULT_COLS			80
#define LXTERMC_DEFAULT_ROWS			25
#define LXTERMC_DEFAULT_HIDE_SCROLLBAR		FALSE
#define LXTERMC_DEFAULT_HIDE_MENUBAR		FALSE
#define LXTERMC_DEFAULT_HIDE_CLOSE_BUTTON	FALSE
#define LXTERMC_DEFAULT_HIDE_POINTER		FALSE
#define LXTERMC_DEFAULT_DISABLE_F10		FALSE
#define LXTERMC_DEFAULT_DISABLE_ALT		FALSE
#define LXTERMC_DEFAULT_DISABLE_CONFIRM		FALSE

#define LXTERMC_FALLBACK_SHELL			"/bin/sh"

// provided by meson.build
//#define LXTERMC_VERSION "0.0.1"
//#define LXTERMC_DATA_DIR

// TODO: change this to rows and cols
#define LXTERMC_DEFAULT_WIDTH 400
#define LXTERMC_DEFAULT_HEIGHT 200

typedef struct _cmdargs {
//	char *cmd;		// -> argv[0]
	char *exec;		// copy of data from -e, --command
	char *cfg;		// copy of -c argument
	gboolean cfg_ro;	// treat provided config as read only
	char *title;		// copy of -t argument
	char *locale;		// copy of locale id, to use instead of user locale
//	GtkWindow *win;		// -> to the top level window that the arguments regard
} cmdargs_t;

extern gchar lxtermc_usage[];
int lxtermc_args(int argc, char **argv, cmdargs_t *cargs);
void lxtermc_clear_cmdargs(cmdargs_t **cargs);

#endif /* _lxtermc_h_ */
