/* config */

#include <glib.h>

#include "lxtermc.h"
#include "lxtermccfg.h"

// TODO: update to rgba
colorset_t color_sets[] = {
	{
		.name = "lxtermc",	// TODO: now VGA, define new... 
		.bg_color = "#000000",
		.fg_color = "#aaaaaa",
		.palette = {
			"#000000", "#aa0000", "#00aa00", "#aa5500",
			"#0000aa", "#aa00aa", "#00aaaa", "#aaaaaa",
			"#555555", "#ff5555", "#55ff55", "#ffff55",
			"#5555ff", "#ff55ff", "#55ffff", "#ffffff"
		}
	}, {
		.name = "VGA",
		.bg_color = "#000000",
		.fg_color = "#aaaaaa",
		.palette = {
			"#000000", "#aa0000", "#00aa00", "#aa5500",
			"#0000aa", "#aa00aa", "#00aaaa", "#aaaaaa",
			"#555555", "#ff5555", "#55ff55", "#ffff55",
			"#5555ff", "#ff55ff", "#55ffff", "#ffffff"
		}
	}, {
		.name = "xterm",
		.bg_color = "#000000",
		.fg_color = "#e5e5e5",
		.palette = {
			"#000000", "#cd0000", "#00cd00", "#cdcd00",
			"#0000ee", "#cd00cd", "#00cdcd", "#e5e5e5",
			"#7f7f7f", "#ff0000", "#00ff00", "#ffff00",
			"#5c5cff", "#ff00ff", "#00ffff", "#ffffff"
		}
	}, {
		.name = "Tango",
		.bg_color = "#000000",
		.fg_color = "#d3d7cf",
		.palette = {
			"#000000", "#cd0000", "#4e9a06", "#c4a000",
			"#3465a4", "#75507b", "#06989a", "#d3d7cf",
			"#555753", "#ef2929", "#8ae234", "#fce94f",
			"#729fcf", "#ad7fa8", "#34e2e2", "#eeeeec"
		}
	}, {
		.name = "Solarized Dark",
		.bg_color = "#002b36",
		.fg_color = "#839496",
		.palette = {
			"#073642", "#dc322f", "#859900", "#b58900",
			"#268bd2", "#d33682", "#2aa198", "#eee8d5",
			"#002b36", "#cb4b16", "#586e75", "#657b83",
			"#839496", "#6c71c4", "#93a1a1", "#fdf6e3"
		}
	}, {
		.name = "Solarized Light",
		.bg_color = "#fdf6e3",
		.fg_color = "#657b83",
		.palette = {
			"#073642", "#dc322f", "#859900", "#b58900",
			"#268bd2", "#d33682", "#2aa198", "#eee8d5",
			"#002b36", "#cb4b16", "#586e75", "#657b83",
			"#839496", "#6c71c4", "#93a1a1", "#fdf6e3"
		}
	}, {
		.name = "Custom"
	}
};
#define NUM_COLOR_SETS 7

static void
free_and_unset_cfg(lxtermccfg_t **cfg)
{
	g_key_file_free((*cfg)->keyfile);
//	g_free((*cfg)->config);
	g_free((*cfg)->tab_pos);
	g_free(*cfg);
}

/*
lxtermccfg_t *
lxtermccfg_copy(lxtermccfg_t *cfg)
{
	g_return_val_if_fail(cfg != NULL, NULL);
	lxtermccfg_t *copy = g_new0(lxtermccfg_t, 1);
	memcpy(copy, cfg, sizeof(*copy));
	copy->config = g_strdup(cfg->config);

	// do deep copy ...
}
*/
/* save cfg to cfg-> */
/*
void
lxtermc_save_cfg(lxtermccfg_t *cfg)
{
	// from GdkRGBA to hex
	// g_strdup_printf("#%02x%02x%02x", (unsigned int)rgba.red*255, ...);
}
*/

// assume parsing of color_sets (from above) works
static void
set_rgba(GKeyFile *kf, gchar *key, GdkRGBA *rgba, const char *rgba_str)
{
	gchar *str = NULL;
	if ((str = g_key_file_get_string(kf, MAIN_GROUP, key, NULL))
		&& !gdk_rgba_parse(rgba, str)) gdk_rgba_parse(rgba, rgba_str);
	g_free(str);
}

static void
set_boolean(GKeyFile *kf, gchar *key, gboolean *ptr, gboolean default_value)
{
	GError *err = NULL;
	*ptr = g_key_file_get_boolean(kf, MAIN_GROUP, key, &err);
	if (err) {
		*ptr = default_value;
		g_error_free(err);
	}
}

static void
set_integer(GKeyFile *kf, gchar *key, gint *ptr, gint default_value)
{
	GError *err = NULL;
	*ptr = g_key_file_get_integer(kf, MAIN_GROUP, key, &err);
	if (err) {
		*ptr = default_value;
		g_error_free(err);
	}
}

/* load config from <cfg_fname>, if NULL, or if reading and/or parsing from file fails,
 * use default settings instead,
 * return pointer to allocated struct, new struct is owned by caller
 */
lxtermccfg_t *
lxtermc_load_cfg(char *cfg_fname)
{
	char *fn = "lxtermc_load_cfg()";
	g_print("%s - loading '%s'\n", fn, cfg_fname);
	lxtermccfg_t *copy = g_new0(lxtermccfg_t, 1);
	GKeyFile *kf = copy->keyfile = g_key_file_new();
	GError *error = NULL;
	if (cfg_fname && g_file_test(cfg_fname, G_FILE_TEST_EXISTS)) {
		GKeyFileFlags flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
		if (!g_key_file_load_from_file(kf, cfg_fname, flags, &error)) {
			g_print("%s - failed to load settings from '%s'", fn, cfg_fname);
			g_print("%s - error message: %s\n", fn, error->message);
		}
		g_error_free(error);
	}
	if (cfg_fname && !g_key_file_has_group(kf, MAIN_GROUP)) {
		g_print("%s - config file has no [%s] group\n", fn, MAIN_GROUP);
	}
	gchar *key_str = NULL;
	int set_index = 0;	// default palette
	if (!(key_str = g_key_file_get_string(kf, MAIN_GROUP, PRESET_COLOR, NULL))) {
		copy->preset_color = color_sets[0].name;
	} else {
		for (int i = 0; i < NUM_COLOR_SETS; i++) {
			if (!g_strcmp0(key_str, color_sets[i].name)) {
				copy->preset_color = color_sets[i].name;
				set_index = i;
				break;
			}
		}
		g_free(key_str);
	}

	// load palette
	// TODO:

	copy->geometry_change = FALSE;

	// in keyfile
	set_rgba(kf, BG_COLOR, &copy->bg_color, color_sets[0].bg_color);
	set_rgba(kf, FG_COLOR, &copy->fg_color, color_sets[0].fg_color);
	
	set_boolean(kf, DISALLOW_BOLD, &copy->disallow_bold, LXTERMC_DEFAULT_DISALLOW_BOLD);
	set_boolean(kf, BOLD_BRIGHT, &copy->bold_bright, LXTERMC_DEFAULT_BOLD_BRIGHT);
	set_boolean(kf, CURSOR_BLINK, &copy->cursor_blink, LXTERMC_DEFAULT_CURSOR_BLINK);
	set_boolean(kf, CURSOR_UNDERLINE, &copy->cursor_underline, LXTERMC_DEFAULT_CURSOR_UNDERLINE);
	set_boolean(kf, AUDIBLE_BELL, &copy->audible_bell, LXTERMC_DEFAULT_AUDIBLE_BELL);
	set_boolean(kf, VISUAL_BELL, &copy->visual_bell, LXTERMC_DEFAULT_VISUAL_BELL);

	set_integer(kf, SCROLLBACK, &copy->scrollback, LXTERMC_DEFAULT_SCROLLBACK);
	set_integer(kf, COLS, &copy->cols, LXTERMC_DEFAULT_COLS);
	set_integer(kf, ROWS, &copy->rows, LXTERMC_DEFAULT_ROWS);

	set_boolean(kf, HIDE_SCROLLBAR, &copy->hide_scrollbar, LXTERMC_DEFAULT_HIDE_SCROLLBAR);
	set_boolean(kf, HIDE_MENUBAR, &copy->hide_menubar, LXTERMC_DEFAULT_HIDE_MENUBAR);
	set_boolean(kf, HIDE_CLOSE_BUTTON, &copy->hide_close_button, LXTERMC_DEFAULT_HIDE_CLOSE_BUTTON);
	set_boolean(kf, HIDE_POINTER, &copy->hide_pointer, LXTERMC_DEFAULT_HIDE_POINTER);
	set_boolean(kf, DISABLE_F10, &copy->disable_f10, LXTERMC_DEFAULT_DISABLE_F10);
	set_boolean(kf, DISABLE_ALT, &copy->disable_alt, LXTERMC_DEFAULT_DISABLE_ALT);
	set_boolean(kf, DISABLE_CONFIRM, &copy->disable_confirm, LXTERMC_DEFAULT_DISABLE_CONFIRM);

	return copy;
/*
	FAIL:
	g_free(key_str);
	free_and_unset_cfg(&copy);
	return NULL;
*/
}
