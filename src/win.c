/**/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"		// all components are included here

void
lxtcwin_free_at(lxtcwin_t **win)
{
	gchar *fn = "lxtcwin_clear()";
	g_print("%s - '%s' - at: %p\n", fn, (*win)->id, (void *)*win);
	g_free((*win)->id);
	lxtermc_free_cmdargs_at(&(*win)->cmdargs);
	lxtccfg_free_at(&(*win)->cfg);
	*win = NULL;
	g_print("%s - end\n", fn);
}

/* GFunc - for passing to g_ptr_array_forech() */
void lxtcwin_close(gpointer lxtcwin_ptr, gpointer data)
{
	gchar *fn = "lxtcwin_close()";
	lxtcwin_t *lxwin = (lxtcwin_t *)lxtcwin_ptr;
	g_print("%s - lxwin at: %p - data at: %p\n", fn, lxwin->id, (void *)data);
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
	g_ptr_array_foreach(lxwin->tabs, lxtctab_detach, NULL);
	lxtcwin_free_at(&lxwin);
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

//	g_print("%s - adding tab at: %p to array at: %p\n", fn, (void *)tab, (void *)win->tabs);
	g_ptr_array_add(win->tabs, tab);

//	g_print("%s - adding scrollwin at: %p to notebook at: %p with label at: %p\n",
//		fn, (void *)tab->scrollwin, (void *)win->notebook, (void *)tab->tab);

//	g_print("%s - calling gtk_notebook_append_page()\n", fn);
	gtk_notebook_append_page(GTK_NOTEBOOK(win->notebook),
		GTK_WIDGET(tab->scrollwin), tab->label);

	g_free(tabstr);
	g_print("%s - end\n", fn);
}

void
lxtcwin_close_tab(lxtcwin_t *win, lxtctab_t *tab)
{
	gchar *fn = "lxtcwin_close_tab()";
	const gchar *str = gtk_label_get_text(GTK_LABEL(tab->label));
	g_print("%s - '%s' at: %p - win: '%s' at: %p\n",
		fn, str, (void *)tab, win->id, (void *)win);

	int p = gtk_notebook_page_num(GTK_NOTEBOOK(win->notebook), tab->scrollwin);
	int n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(win->notebook));
	g_print("%s - remove page: %i among %i pages\n", fn, p, n);

	if (n < 1 || p < 0) {
		g_print("%s - n: %i, p: %i - nothing to close - end\n", fn, n, p);
		return;
	}
	if (n < 2) {
		g_print("%s - n: %i, closing the window - end\n", fn, n);
		gtk_window_close(GTK_WINDOW(win->win));
		return;
	}

	if (!g_ptr_array_remove(win->tabs, tab)) {
		g_print("%s - '%s' - tab pointer not found...\n", fn, str);
	}
	g_print("%s - '%s' at: %p, removed from win '%s' at: %p\n",
		fn, str, (void *)tab, win->id, (void *)win);

	gtk_notebook_remove_page(GTK_NOTEBOOK(win->notebook), p);
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
	int p = gtk_notebook_get_current_page(GTK_NOTEBOOK(win->notebook));
	if (p < 0) { g_print("%s - no pages in window\n", fn); return; }
	if (!win->tabs || win->tabs->len < 1) { g_print("%s - no tabs in window\n", fn); return; }
	if ((guint)p > win->tabs->len) { g_print("%s - more pages than tabs\n", fn); return; }
	int r, c;
	GError *err = NULL;
	lxtctab_t *tab = (lxtctab_t *)g_ptr_array_index(win->tabs, p);
	if (!vte_pty_get_size(tab->pty, &r, &c, &err) || err) {
		g_print("%s - error when getting pty size\n", fn);
		g_error_free(err);
		return;
	}
	if (win->rows != r || win->cols != c) {
		g_print("%s - new pty size, %i x %i\n", fn, r, c);
		win->rows = r;
		win->cols = c;
	}
	g_print("%s - end\n", fn);
}

/*
 * GTK_NOTEBOOK:switch-page signal handler
 * default handler is called afterwards
 */
static void
lxtcwin_tab_switched(GtkNotebook *nb, GtkWidget *wid, guint num, lxtcwin_t *win)
{
	gchar *fn = "lxtcwin_tab_switched()";
//	int page = gtk_notebook_get_current_page(nb);
	if (gtk_notebook_get_n_pages(nb) < 1) { g_print("%s - to nothing\n", fn); return; }

	g_print("%s - wid: %p, num: %i, win: %p\n",
		fn, (void *)wid, num, (void *)win);

	lxtctab_t *tab = (lxtctab_t *)g_ptr_array_index(win->tabs, num);

	if (tab->scrollwin != wid) {
		g_print("%s - notebook child mismatch, why? scrollwin: %p, wid: %p\n",
			fn, (void *)tab->scrollwin, (void *)wid);
		return;
	}

	const gchar *str = gtk_label_get_text(GTK_LABEL(tab->label));

	g_print("%s - switched to tab '%s'\n", fn, str);

	// this does not work, probably because of default handler called afterwards...
//	if (!gtk_widget_grab_focus(GTK_WIDGET(tab->vte))) {
//		g_print("%s - vte failed to grab focus\n", fn);
//	}

	g_print("%s - end\n", fn);
	// default handler called after this registered handler
}

lxtcwin_t *
lxtcwin_new(LxtermcApp *app, const gchar *id)
{
	gchar *fn = "lxtcwin_new()";
	g_print("%s - '%s' - app at: %p\n", fn, id, (void *)app);

	lxtcwin_t *win = g_new0(lxtcwin_t, 1);
	win->app = app;
	win->id = g_strdup(id);
	win->cmdargs = lxtcapp_steal_cmdargs(app);
	win->cfg = lxtccfg_load(win->cmdargs->cfg);
	win->win = gtk_application_window_new(GTK_APPLICATION(app));
	win->rows = 0;
	win->cols = 0;
	g_print("%s - new lxtcwin_t struct at: %p - gtkappwin at: %p\n",
		fn, (void *)win, (void *)win->win);

	g_signal_connect(GTK_WINDOW(win->win),
			"close-request", G_CALLBACK(lxtcwin_close_request), win);

	gtk_window_set_title(GTK_WINDOW(win->win),
		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));
	gtk_window_set_default_size(GTK_WINDOW(win->win),
		LXTERMC_DEFAULT_WIDTH, LXTERMC_DEFAULT_HEIGHT);

	win->notebook = gtk_notebook_new();
	win->tabs = g_ptr_array_new();
	g_print("%s - tabs at: %p\n", fn, (void *)win->tabs);

	g_signal_connect(GTK_NOTEBOOK(win->notebook),
		"switch-page", G_CALLBACK(lxtcwin_tab_switched), win);

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

//	gtk_window_set_child(GTK_WINDOW(win->win), GTK_WIDGET(win->notebook));
//	gtk_window_set_child(win->win, win->notebook);
	gtk_window_set_child(GTK_WINDOW(win->win), win->notebook);
	g_signal_connect_swapped(GTK_WINDOW(win->win),
		"notify::default-width", G_CALLBACK(lxtcwin_size), win);
	g_signal_connect_swapped(GTK_WINDOW(win->win),
		"notify::default-height", G_CALLBACK(lxtcwin_size), win);

	int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(win->notebook));
	win->visible_tab = g_ptr_array_index(win->tabs, page);
	const gchar *str = gtk_label_get_text(GTK_LABEL(win->visible_tab->label));
	if (!gtk_widget_grab_focus(GTK_WIDGET(win->visible_tab->vte))) {
		g_print("%s - vte failed to grab focus\n", fn);
	}
	g_print("%s - current page is '%s' : %i\n", fn, str, page);
	return win;
}
