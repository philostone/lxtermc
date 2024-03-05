/* config */

#ifndef _LXTERMC_CFG_H_
#define _LXTERMC_CFG_H_

#include <glib.h>
#include <gdk/gdk.h>

#include "lxtermc.h"

/* config file groups */
#define MAIN_GROUP		LXTERMC_NAME
#define PALETTE_PREFIX		"palette_"

/* config file main group keys */
#define PRESET_COLOR		"preset_color"
#define BG_COLOR		"bg_color"
#define FG_COLOR		"fg_color"
#define DISALLOW_BOLD		"disallow_bold"
#define BOLD_BRIGHT		"bold_bright"
#define CURSOR_BLINK		"cursor_blink"
#define CURSOR_UNDERLINE	"cursor_underline"
#define AUDIBLE_BELL		"audible_bell"
#define VISUAL_BELL		"visual_bell"
#define TAB_POS			"tab_pos"
#define SCROLLBACK		"scrollback"
#define COLS			"cols"
#define ROWS			"rows"
#define HIDE_SCROLLBAR		"hide_scrollbar"
#define HIDE_MENUBAR		"hide_menubar"
#define HIDE_CLOSE_BUTTON	"hide_close_button"
#define HIDE_POINTER		"hide_pointer"
#define DISABLE_F10		"disable_f10"
#define DISABLE_ALT		"disable_alt"
#define DISABLE_CONFIRM		"disable_confirm"

/* config default values in lxtermc.h */

typedef struct _cfg_t {
	GKeyFile *keyfile;
//	gchar *config;
	GdkRGBA palette[16];
	gboolean geometry_change;

	// in keyfile
	const char *preset_color;
	GdkRGBA bg_color;
	GdkRGBA fg_color;
	gboolean disallow_bold;
	gboolean bold_bright;
	gboolean cursor_blink;
	gboolean cursor_underline;
	gboolean audible_bell;
	gboolean visual_bell;
	char *tab_pos;
//	char *word_delims;
	gint scrollback;
	gint cols;
	gint rows;
	gboolean hide_scrollbar;
	gboolean hide_menubar;
	gboolean hide_close_button;
	gboolean hide_pointer;
	gboolean disable_f10;
	gboolean disable_alt;
	gboolean disable_confirm;
} lxtccfg_t;

typedef struct _colorset {
	const char *name;
	const char *bg_color;
	const char *fg_color;
	const char *palette[16];
} colorset_t;

extern colorset_t color_presets[];

lxtccfg_t *lxtccfg_load(char *fname);
void lxtccfg_clear(lxtccfg_t **cfg);

#endif /* _LXTERMC_CFG_H_ */
