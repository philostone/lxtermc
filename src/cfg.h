/* config */

#ifndef _LXTERMC_CFG_H_
#define _LXTERMC_CFG_H_

G_BEGIN_DECLS

#include "lxtermc.h"		// for LXTERMC_NAME

/* config file groups */
#define MAIN_GROUP		LXTERMC_NAME
#define PALETTE_PREFIX		"palette_"

#define PRESET_COLOR		"preset_color"
#define BG_COLOR		"bg_color"
#define FG_COLOR		"fg_color"

/* config file main group keys */
#define ALLOW_HYPERLINK		"allow_hyperlink"
#define AUDIBLE_BELL		"audible_bell"
#define ERASE_BINDING		"erase_binding"
#define BOLD_BRIGHT		"bold_bright"
#define CELL_HEIGHT_SCALE	"height_scale"
#define CELL_WIDTH_SCALE	"width_scale"
#define CJK_AMBIGUOUS_WIDTH	"cjk_ambiguous_width"
#define CLEAR_BACKGROUND	"clear_background"

#define CURSOR_BLINK		"cursor_blink"
#define CURSOR_SHAPE		"cursor_shape"
#define DELETE_BINDING		"delete_binding"
#define ENABLE_BIDI		"enable_bidi"
#define ENABLE_FALLBACK_SCROLL	"enable_fallback_scroll"
#define ENABLE_SHAPING		"enable_shaping"
#define ENABLE_SIXEL		"enable_sixel"

#define INPUT_ENABLED		"input_enabled"
#define MOUSE_AUTOHIDE		"mouse_autohide"

#define SCROLL_ON_INPUT		"scroll_on_input"
#define SCROLL_ON_OUTPUT	"scroll_on_output"
#define SCROLL_SPEED		"scroll_speed"
#define SCROLL_IS_PIXELS	"scroll_is_pixels"
#define SCROLLBACK		"scrollback"
#define COLS			"cols"
#define ROWS			"rows"
#define TEXT_BLINK		"text_blink"


#define TAB_POS			"tab_pos"
#define HIDE_SCROLLBAR		"hide_scrollbar"
#define HIDE_MENUBAR		"hide_menubar"
#define HIDE_CLOSE_BUTTON	"hide_close_button"
#define DISABLE_F10		"disable_f10"
#define DISABLE_ALT		"disable_alt"
#define DISABLE_CONFIRM		"disable_confirm"

/* config default values in lxtermc.h */

typedef struct _cfg {
	GKeyFile *keyfile;
//	gchar *config;
	const char *preset_color;
	GdkRGBA palette[16];
	GdkRGBA bg_color;
	GdkRGBA fg_color;
	gboolean geometry_change;

	// in keyfile
	gboolean allow_hyperlink;
	gboolean audible_bell;
	gint erase_binding;
	gboolean bold_bright;
	double cell_width_scale;
	double cell_height_scale;
	gint cjk_ambiguous_width;
	gboolean clear_bg;

	gint cursor_blink;
	gint cursor_shape;
	gint delete_binding;
	gboolean enable_bidi;
	gboolean enable_fallback_scroll;
	gboolean enable_shaping;
	gboolean enable_sixel;

	gboolean input_enabled;
	gboolean mouse_autohide;

	gboolean scroll_on_input;
	gboolean scroll_on_output;
	gint scroll_speed;
	gboolean scroll_is_pixels;
	gint scrollback;
	gint cols;
	gint rows;
	gint text_blink;



	char *tab_pos;
//	char *word_delims;
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
void lxtccfg_free_at(lxtccfg_t **cfg);

G_END_DECLS

#endif /* _LXTERMC_CFG_H_ */
