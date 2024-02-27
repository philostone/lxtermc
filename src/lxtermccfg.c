/* config */

#include <glib.h>

#include "lxtermc.h"
#include "lxtermccfg.h"

colorpreset_t color_sets[] = {
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

static void
free_and_unset_cfg(lxtermccfg_t **cfg)
{
	g_free((*cfg)->keyfile);
	g_free((*cfg)->config);
	g_free((*cfg)->tab_pos);
	g_free(*cfg);
}

static lxtermccfg_t *
create_default_cfg()
{
	lxtermccfg_t *copy = g_new0(lxtermccfg_t, 1);
	copy->keyfile = g_key_file_new();
	copy->config = NULL;
//	copy->preset_color = color_sets[0].name;
	if (!gdk_rgba_parse(&(copy->palette[0]), color_sets[0].palette[0])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[1]), color_sets[0].palette[1])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[2]), color_sets[0].palette[2])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[3]), color_sets[0].palette[3])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[4]), color_sets[0].palette[4])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[5]), color_sets[0].palette[5])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[6]), color_sets[0].palette[6])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[7]), color_sets[0].palette[7])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[8]), color_sets[0].palette[8])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[9]), color_sets[0].palette[9])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[10]), color_sets[0].palette[10])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[11]), color_sets[0].palette[11])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[12]), color_sets[0].palette[12])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[13]), color_sets[0].palette[13])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[14]), color_sets[0].palette[14])) goto FAIL;
	if (!gdk_rgba_parse(&(copy->palette[15]), color_sets[0].palette[15])) goto FAIL;
	copy->geometry_change = FALSE;

	// in keyfile
	if (!gdk_rgba_parse(&(copy->bg_color), color_sets[0].bg_color)) goto FAIL;
	if (!gdk_rgba_parse(&(copy->fg_color), color_sets[0].fg_color)) goto FAIL;
	copy->disallow_bold = FALSE;
	copy->bold_bright = FALSE;
	copy->cursor_blink = FALSE;
	copy->cursor_underline = FALSE;
	copy->audible_bell = FALSE;
	copy->visual_bell = FALSE;
	copy->tab_pos = g_strdup("top");
//	copy->word_delim = g_strdup(###############);
	copy->scrollback = LXTERMC_DEFAULT_SCROLLBACK;
	copy->cols = LXTERMC_DEFAULT_COLS;
	copy->rows = LXTERMC_DEFAULT_ROWS;
	copy->hide_scrollbar = FALSE;
	copy->hide_menubar = FALSE;
	copy->hide_close_button = FALSE;
	copy->hide_pointer = FALSE;
	copy->disable_f10 = FALSE;
	copy->disable_alt = FALSE;
	copy->disable_confirm = FALSE;
	g_file_set_string(copy->keyfile, LXTERMC_NAME, "bg_color", color_sets[0].bg_color);
	g_file_set_string(copy->keyfile, LXTERMC_NAME, "fg_color", color_sets[0].fg_color);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "disallow_bold", copy->disallow_bold);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "bold_bright", copy->bold_bright);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "cursor_blink", copy->cursor_blink);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "cursor_underline", copy->cursor_underline);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "audible_bell", copy->audible_bell);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "visible_bell", copy->visible_bell);
	g_file_set_string(copy->keyfile, LXTERMC_NAME, "tab_pos", copy->tab_pos);
//	g_file_set_string(copy->keyfile, LXTERMC_NAME, "word_delim", copy->word_delim);
	g_file_set_integer(copy->keyfile, LXTERMC_NAME, "scrollback", copy->scrollback);
	g_file_set_integer(copy->keyfile, LXTERMC_NAME, "cols", copy->cols);
	g_file_set_integer(copy->keyfile, LXTERMC_NAME, "rows", copy->rows);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "hide_scrollbar", copy->hide_scrollbar);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "hide_menubar", copy->hide_menubar);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "hide_close_button", copy->hide_close_button);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "hide_pointer", copy->hide_pointer);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "disable_f10", copy->disable_f10);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "disable_alt", copy->disable_alt);
	g_file_set_boolean(copy->keyfile, LXTERMC_NAME, "disable_confirm", copy->disable_confirm);

	return copy;

	FAIL:
	free_and_unset_cfg(&copy);
	return NULL;
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
void
lxtermc_save_cfg(lxtermccfg_t *cfg)
{

}

/* load config from <cfg_fname>, if NULL, or if reading and/or parsing loading from file fails,
 * load default settings instead,
 * return pointer to allocated struct, new struct is owned by caller
 */
lxtermccfg_t *
lxtermc_load_cfg(char *cfg_fname)
{
	if (cfg_fname == NULL || !g_file_test(cfg_fname, G_FILE_TEST_EXISTS))
		return create_default_cfg();

	return NULL;
}