/**/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"		// all components are included here

/* GDestroyNotify signature, *win is of type (lxtcwin_t *) */
void
lxtcwin_free(void *win)
{
	gchar *fn = "lxtcwin_free()";
	lxtcwin_t *w = (lxtcwin_t *)win;
	g_print("%s - '%s' - at: %p\n", fn, w->id, (void *)win);
	g_free(w->id);
	lxtermc_cmdargs_free(w->cmdargs);
	lxtccfg_free(w->cfg);
	g_ptr_array_free(w->tabs, TRUE); // calls lxtctab_free()
	g_print("%s - end\n", fn);
}

/* GFunc signature - for passing to g_ptr_array_forech() */
void lxtcwin_close(gpointer lxtcwin_ptr, gpointer data)
{
	gchar *fn = "lxtcwin_close()";
	lxtcwin_t *lxwin = (lxtcwin_t *)lxtcwin_ptr;
	g_print("%s - '%s ', lxwin at: %p - data at: %p\n",
		fn, lxwin->id, (void *)lxwin, (void *)data);
	g_ptr_array_foreach(lxwin->tabs, lxtctab_close, NULL);
	g_print("%s - end, passing task to gtk_window_close()\n", fn);
	gtk_window_close(GTK_WINDOW(lxwin->win));
}

/*
 * GTK_WINDOW:close-request signal handler
 * default handler is called afterwards (if return value is FALSE)
 */
gboolean
lxtcwin_close_request(GtkWindow *gwin, lxtcwin_t *lxwin)
{
	gchar *fn = "lxtcwin_close_request()";
	g_print("%s - '%s' - gwin at: %p - lxwin at: %p\n",
		fn, lxwin->id, (void *)gwin, (void *)lxwin);

// Before doing this, make sure terminals are in closeable state, HOW ????
// TODO: use gtk... close tabs etc...
	g_print("%s - end, passing task to GtkWindow\n", fn);
	return FALSE; // let GtkWindow handle the rest ...
}

static void
lxtcwin_new_tab(lxtcwin_t *win, const gchar *label)
{
	gchar *fn = "lxtcwin_new_tab()";
	gchar *tabstr = (label) ? g_strdup(label) : g_strdup_printf("Tab %u", win->tabs->len+1);
	g_print("%s - '%s -> creating '%s' - tabs at: %p\n",
		fn, label, tabstr, (void *)win->tabs);
	lxtctab_t *tab = lxtctab_new(win, tabstr);
	g_ptr_array_add(win->tabs, tab);
	gtk_notebook_append_page(win->notebook,
		GTK_WIDGET(tab->scrollwin), GTK_WIDGET(tab->label));
	g_free(tabstr);
	g_print("%s - end\n", fn);
}

void
lxtcwin_close_tab(lxtcwin_t *win, lxtctab_t *tab)
{
	gchar *fn = "lxtcwin_close_tab()";
	const gchar *str = gtk_label_get_text(tab->label);
	g_print("%s - '%s' at: %p - win: '%s' at: %p\n",
		fn, str, (void *)tab, win->id, (void *)win);
	int p = gtk_notebook_page_num(win->notebook, GTK_WIDGET(tab->scrollwin));
	int n = gtk_notebook_get_n_pages(win->notebook);
	g_print("%s - remove page index: %i among %i pages\n", fn, p, n);
	if (n < 1 || p < 0) {
		g_print("%s - n: %i, p: %i - nothing to close - end\n", fn, n, p);
		return;
	}
	if (!g_ptr_array_remove(win->tabs, tab)) { // calls lxtctab_free()
		g_print("%s - '%s' - tab pointer not found...\n", fn, str);
	}
	if (n < 2) {
		g_print("%s - n: %i, closing the window - end\n", fn, n);
		gtk_window_close(GTK_WINDOW(win->win));
		return;
	}
	gtk_notebook_remove_page(win->notebook, p);
	g_print("%s - '%s' at: %p, removed from win '%s' at: %p\n",
		fn, str, (void *)tab, win->id, (void *)win);

	g_print("%s - end\n", fn);
}
/*
 * GTK_WINDOW - notify::default-[width|heigh] handler
 */
static void
lxtcwin_size(lxtcwin_t *win)
{
	gchar *fn = "lxtcwin_size()";
	if (!win->notebook) { g_print("%s - notebook not filled yet\n", fn); return; }
	int p = gtk_notebook_get_current_page(win->notebook);
	if (p < 0) { g_print("%s - no pages in notebook\n", fn); return; }
	if (!win->tabs || win->tabs->len < 1) { g_print("%s - no tabs in window\n", fn); return; }
	if ((guint)p > win->tabs->len) { g_print("%s - more pages than tabs\n", fn); return; }
	lxtctab_t *tab = (lxtctab_t *)g_ptr_array_index(win->tabs, p);

	int rows = vte_terminal_get_row_count(tab->vte);
	int cols = vte_terminal_get_column_count(tab->vte);
	if (win->rows != rows || win->cols != cols) {
		g_print("%s - new pty size, %i x %i - end\n", fn, rows, cols);
		win->rows = rows;
		win->cols = cols;
	} else {
		g_print("%s - end\n", fn);
	}
}

/*
 * GTK_NOTEBOOK:switch-page signal handler
 * default handler is called afterwards
 */
static void
lxtcwin_tab_switched(GtkNotebook *nb, GtkWidget *wid, guint num, lxtcwin_t *win)
{
	gchar *fn = "lxtcwin_tab_switched()";
	if (gtk_notebook_get_n_pages(nb) < 1) { g_print("%s - to nothing\n", fn); return; }

	g_print("%s - wid: %p, num: %i, win: %p\n",
		fn, (void *)wid, num, (void *)win);

	lxtctab_t *tab = (lxtctab_t *)g_ptr_array_index(win->tabs, num);

	if ((void *)tab->scrollwin != (void *)wid) {
		g_print("%s - notebook child mismatch, why? scrollwin: %p, wid: %p\n",
			fn, (void *)tab->scrollwin, (void *)wid);
		return;
	}

	const gchar *str = gtk_label_get_text(tab->label);

	g_print("%s - switched to tab '%s'\n", fn, str);

	// this does not work, probably because of default handler called afterwards...
//	if (!gtk_widget_grab_focus(GTK_WIDGET(tab->vte))) {
//		g_print("%s - vte failed to grab focus\n", fn);
//	}

	g_print("%s - end\n", fn);
}

lxtcwin_t *
lxtcwin_new(LxtermcApp *app, const gchar *id)
{
	gchar *fn = "lxtcwin_new()";
	g_print("%s - '%s' - app at: %p\n", fn, id, (void *)app);

	/* lxtcwin_t initializations */
	lxtcwin_t *win = g_new0(lxtcwin_t, 1);
	win->app = app;
	win->id = g_strdup(id);
	win->cmdargs = lxtcapp_steal_cmdargs(app);
	win->cfg = lxtccfg_load(win->cmdargs->cfg);
	win->win = GTK_APPLICATION_WINDOW(gtk_application_window_new(GTK_APPLICATION(app)));
	win->display = gdk_display_get_default();
	win->provider = gtk_css_provider_new();
	win->rows = 0;
	win->cols = 0;
	g_print("%s - new lxtcwin_t struct at: %p - gtkappwin at: %p\n",
		fn, (void *)win, (void *)win->win);

	/* GtkNotebook initialization */
	win->notebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_notebook_set_group_name(win->notebook, LXTERMC_NAME);
	g_signal_connect(win->notebook, "switch-page", G_CALLBACK(lxtcwin_tab_switched), win);

	/* GptArray of (lxtctab_t *) initialization */
	win->tabs = g_ptr_array_new_with_free_func(lxtctab_free);
	g_print("%s - tabs at: %p\n", fn, (void *)win->tabs);
	if (win->cmdargs->tabs) {
		char *tabs = g_strdup(win->cmdargs->tabs);
		char *at = tabs;
		char *next = NULL;
		do {
			if ((next = strchr(at, ','))) *next = '\0';
			lxtcwin_new_tab(win, at);
			if (next) at = next+1;
		} while (next && *at);
		g_free(tabs);
	} else {
		lxtcwin_new_tab(win, NULL);
	}

	/* GtkWindow initialization */
	gtk_window_set_child(GTK_WINDOW(win->win), GTK_WIDGET(win->notebook));
	gtk_window_set_title(GTK_WINDOW(win->win),
		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));
	gtk_window_set_default_size(GTK_WINDOW(win->win),
		LXTERMC_DEFAULT_WIDTH, LXTERMC_DEFAULT_HEIGHT);
	g_signal_connect(GTK_WINDOW(win->win), "close-request",
		G_CALLBACK(lxtcwin_close_request), win);
	g_signal_connect_swapped(GTK_WINDOW(win->win), "notify::default-width",
		G_CALLBACK(lxtcwin_size), win);
	g_signal_connect_swapped(GTK_WINDOW(win->win), "notify::default-height",
		G_CALLBACK(lxtcwin_size), win);

	/* final initializations */
	int page = gtk_notebook_get_current_page(win->notebook);
	win->visible_tab = g_ptr_array_index(win->tabs, page);
	const gchar *str = gtk_label_get_text(win->visible_tab->label);
	if (!gtk_widget_grab_focus(GTK_WIDGET(win->visible_tab->vte))) {
		g_print("%s - vte failed to grab focus\n", fn);
	}
	g_print("%s - current page is '%s' : %i\n", fn, str, page);
	return win;
}
