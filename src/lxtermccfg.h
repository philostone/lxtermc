/* config */

#ifndef _LXTERMCCFG_H_
#define _LXTERMCCFG_H_

#include <glib.h>
#include <gdk/gdk.h>

typedef struct _lxtermccfg_t {
	GKeyFile *keyfile;
	gchar *config;
//	const char *preset_color;
	GdkRGBA palette[16];
	gboolean geometry_change;

	// in keyfile
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
} lxtermccfg_t;

typedef struct _colorpreset {
	const char *name;
	const char *bg_color;
	const char *fg_color;
	const char *palette[16];
} colorpreset_t;

extern colorpreset_t color_presets[];

#endif /* _LXTERMCCFG_H_ */
