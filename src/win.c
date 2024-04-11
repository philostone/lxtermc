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

/* GFunc */
void lxtcwin_close(gpointer lxtcwin_ptr, gpointer data)
{
	gchar *fn = "lxtcwin_close()";
	lxtcwin_t *lxwin = (lxtcwin_t *)lxtcwin_ptr;
	g_print("%s - lxwin at: %p - data at: %p\n", fn, lxwin->id, (void *)data);
	g_print("%s - end, passing task to gtk_window_close()\n", fn);
	gtk_window_close(GTK_WINDOW(lxwin->win));	
}

gboolean
lxtcwin_close_request(GtkWindow *gwin, lxtcwin_t *lxwin)
{
	gchar *fn = "lxtcwin_close_request()";
	g_print("%s - '%s' - gwin at: %p - lxwin at: %p\n",
		fn, lxwin->id, (void *)gwin, (void *)lxwin);

// Before doing this, make sure terminals are in closeable state, HOW ????
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
		GTK_WIDGET(tab->scrollwin), tab->tab);

	g_free(tabstr);
	g_print("%s - end\n", fn);
}

void
lxtcwin_close_tab(lxtcwin_t *win, lxtctab_t *tab)
{
	gchar *fn = "lxtcwin_close_tab()";
	const gchar *str = gtk_label_get_text(GTK_LABEL(tab->tab)); 
	g_print("%s - '%s' at: %p - win: '%s' at: %p\n",
		fn, str, (void *)tab, win->id, (void *)win);
	if (!g_ptr_array_remove(win->tabs, tab)) {
		g_print("%s - '%s' - tab pointer not found...\n", fn, str);
	}
	g_print("%s - '%s' at: %p, removed from win '%s' at: %p\n",
		fn, str, (void *)tab, win->id, (void *)win);





	int i = 0;
	if ((i = gtk_notebook_get_n_pages(GTK_NOTEBOOK(win->notebook)) < 2)) {
		gtk_window_close(GTK_WINDOW(win->win));
		return;
	}
	if ((i = gtk_notebook_page_num(GTK_NOTEBOOK(win->notebook), GTK_WIDGET(tab->scrollwin)) < 0)) {
		g_print("%s - tab not found in notebook\n", fn);
	} else {
		gtk_notebook_remove_page(GTK_NOTEBOOK(win->notebook), i);
	}
	g_print("%s - end\n", fn);
}

static void lxtcwin_size(GtkWindow *gwin, lxtcwin_t *win)
{
	gchar *fn = "lxtcwin_size()";
	g_print("%s\n", fn);
//	g_print("%s - '%s' at: %p, gwin at: %p\n",
//		fn, win->id, (void *)win, (void *)gwin);
	if (!win->tabs || win->tabs->len < 1) return;
	int r, c;
	GError *err = NULL;

// TODO, need to get active tab

	lxtctab_t *tab = (lxtctab_t *)g_ptr_array_index(win->tabs, 0);
	VtePty *pty = VTE_PTY(tab->pty);
	if (!vte_pty_get_size(pty, &r, &c, &err) || err) {
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

static void
lxtcwin_width(GtkWindow *gwin, gpointer unknown, lxtcwin_t *win)
{
	gchar *fn = "lxtcwin_width()";
	g_print("%s\n", fn);
//	g_print("%s - gwin: %p - unknown: %p - lxtcwin: %p\n",
//		fn, (void *)gwin, (void *)unknown, (void *)win);
	lxtcwin_size(gwin, win);
	g_print("%s - end\n", fn);
}

static void
lxtcwin_height(GtkWindow *gwin, gpointer unknown, lxtcwin_t *win)
{
	gchar *fn = "lxtcwin_height()";
	g_print("%s\n", fn);
//	g_print("%s - gwin: %p - unknown: %p - lxtcwin: %p\n",
//		fn, (void *)gwin, (void *)unknown, (void *)win);
	lxtcwin_size(gwin, win);
	g_print("%s - end\n", fn);
}

static void
lxtcwin_tab_switched(GtkNotebook *nb, GtkWidget *wid, guint num, lxtcwin_t *win)
{
	gchar *fn = "lxtcwin_tab_switched()";
	int page = gtk_notebook_get_current_page(nb);
	lxtctab_t *tab = (lxtctab_t *)g_ptr_array_index(win->tabs, page);





//	const gchar *str = gtk_label_get_text(GTK_LABEL(tab->tab)); 
	g_print("%s - nb at: %p, wid at: %p, num: %u (%u), win at: %p\n",
		fn, (void *)nb, (void *)wid, num, win->tabs->len, (void *)win);
//	g_print("%s - new tab '%s', nb at: %p, wid at: %p, num: %u, win at: %p\n",
//		fn, str, (void *)nb, (void *)wid, num, (void *)win);

// TODO: track visible_tab
	g_print("%s - end\n", fn);
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
	g_signal_connect(GTK_WINDOW(win->win),
		"notify::default-width", G_CALLBACK(lxtcwin_width), win);
	g_signal_connect(GTK_WINDOW(win->win),
		"notify::default-height", G_CALLBACK(lxtcwin_height), win);

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

	gtk_window_set_child(GTK_WINDOW(win->win), GTK_WIDGET(win->notebook));

	int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(win->notebook));
	win->visible_tab = g_ptr_array_index(win->tabs, page);
	const gchar *str = gtk_label_get_text(GTK_LABEL(win->visible_tab->tab)); 

	g_print("%s - current page is '%s' : %i\n", fn, str, page);
	return win;
}
