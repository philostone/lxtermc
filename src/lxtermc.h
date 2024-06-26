
#ifndef _LXTERMC_H_
#define _LXTERMC_H_

#include <glib.h>
#include <gtk/gtk.h>
#include <vte/vte.h>

G_BEGIN_DECLS

// required to resolv circular header dependencies
typedef struct _LxtermcTab LxtermcTab;
typedef struct _LxtermcWin LxtermcWin;
typedef struct _LxtermcApp LxtermcApp;

//typedef struct _cmdargs cmdargs_t;
//typedef struct _cfg lxtccfg_t;
//typedef struct _lxtcwin lxtcwin_t;
//typedef struct _lxtctab lxtctab_t;

// include all component headers here
//#include "cfg.h"
//#include "app.h"
//#include "win.h"
//#include "tab.h"

#define LXTERMC_NAME				"lxtermc"
#define LXTERMC_BASE				"/com/github/philostone/" LXTERMC_NAME "/"
#define LXTERMC_APP_ID				"com.github.philostone." LXTERMC_NAME
#define LXTERMC_DEFAULT_LOCALE			"sv_SE.utf8"

#define LXTERMC_DEFAULT_ALLOW_HYPERLINK		TRUE
#define LXTERMC_DEFAULT_AUDIBLE_BELL		FALSE
#define LXTERMC_DEFAULT_ERASE_BINDING		VTE_ERASE_AUTO
#define LXTERMC_DEFAULT_BOLD_BRIGHT		FALSE
#define LXTERMC_DEFAULT_CELL_HEIGHT_SCALE	1.0
#define LXTERMC_DEFAULT_CELL_WIDTH_SCALE	1.0
#define LXTERMC_DEFAULT_CJK_AMBIGUOUS_WIDTH	1	// 1 narrow, 2 wide
#define LXTERMC_DEFAULT_CLEAR_BACKGROUND	TRUE

// color_background
// color_bold
// color_cursor			<- background, if NULL reversed bg/fg
// color_cursor_foreground	<- if NULL, reversed bg/fg
// color_foreground
// color_highlight		<- backgound
// color_highlight_foreground	<- if both NULL, highlight is reversed bg/fg

#define LXTERMC_DEFAULT_CURSOR_BLINK		VTE_CURSOR_BLINK_SYSTEM
#define LXTERMC_DEFAULT_CURSOR_SHAPE		VTE_CURSOR_SHAPE_BLOCK
#define LXTERMC_DEFAULT_DELETE_BINDING		VTE_ERASE_AUTO
#define LXTERMC_DEFAULT_ENABLE_BIDI		FALSE
#define LXTERMC_DEFAULT_ENABLE_FALLBACK_SCROLL	FALSE
#define LXTERMC_DEFAULT_ENABLE_SHAPING		FALSE
#define LXTERMC_DEFAULT_ENABLE_SIXEL		FALSE

// font
// font_options
// font_scale

#define LXTERMC_DEFAULT_INPUT_ENABLED		TRUE
#define LXTERMC_DEFAULT_MOUSE_AUTOHIDE		FALSE

// pty

#define LXTERMC_DEFAULT_SCROLL_ON_INPUT		TRUE
#define LXTERMC_DEFAULT_SCROLL_ON_OUTPUT	FALSE
#define LXTERMC_DEFAULT_SCROLL_SPEED		1	// one line
#define LXTERMC_DEFAULT_SCROLL_IS_PIXELS	FALSE
#define LXTERMC_DEFAULT_SCROLLBACK		1000
#define LXTERMC_DEFAULT_COLS			80
#define LXTERMC_DEFAULT_ROWS			25
#define LXTERMC_DEFAULT_TEXT_BLINK		VTE_TEXT_BLINK_FOCUSED

// word_char_exceptions


#define LXTERMC_DEFAULT_VISUAL_BELL		FALSE
#define LXTERMC_DEFAULT_TAB_POS			"top"
#define LXTERMC_DEFAULT_HIDE_SCROLLBAR		FALSE
#define LXTERMC_DEFAULT_HIDE_MENUBAR		FALSE
#define LXTERMC_DEFAULT_HIDE_CLOSE_BUTTON	FALSE
#define LXTERMC_DEFAULT_HIDE_POINTER		FALSE
#define LXTERMC_DEFAULT_DISABLE_F10		FALSE
#define LXTERMC_DEFAULT_DISABLE_ALT		FALSE
#define LXTERMC_DEFAULT_DISABLE_CONFIRM		FALSE

#define LXTERMC_FALLBACK_SHELL			"/bin/sh"

// provided by meson.build
//#define LXTERMC_VERSION
//#define LXTERMC_DATA_DIR

// TODO: change this to rows and cols
#define LXTERMC_DEFAULT_WIDTH 400
#define LXTERMC_DEFAULT_HEIGHT 200

typedef struct _cmdargs {
	char *exec;		// copy of data from -e, --command
	char *cfg;		// copy of -c argument
	gboolean cfg_ro;	// treat provided config as read only
	char *title;		// copy of -t argument
	char *tabs;		// comma separated list of tab titles
	char *locale;		// copy of locale id, to use instead of user locale
} cmdargs_t;

extern gchar lxtermc_usage[];
int lxtermc_args(int argc, char **argv, cmdargs_t *cargs);
void lxtermc_free_str_at(char **ptr);
void lxtermc_cmdargs_free(cmdargs_t *cargs);

G_END_DECLS

#endif /* _LXTERMC_H_ */
