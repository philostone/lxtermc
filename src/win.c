/**/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"		// all components are included here
//#include "win.h"

void
lxtcwin_free_at(lxtcwin_t **win)
{
	g_print("lxtcwin_clear() - '%s' - at: %p\n", (*win)->id, (void *)*win);
	g_free((*win)->id);
	lxtermc_free_cmdargs_at(&(*win)->cmdargs);
	lxtccfg_free_at(&(*win)->cfg);
	*win = NULL;
}

/* GFunc */
void lxtcwin_close(gpointer lxtcwin_ptr, gpointer data)
{
	char *fn = "lxtcwin_close()";
	lxtcwin_t *lxwin = (lxtcwin_t *)lxtcwin_ptr;
	g_print("%s - lxwin at: %p - data at: %p\n", fn, lxwin->id, (void *)data);
	gtk_window_close(GTK_WINDOW(lxwin->win));	
}

gboolean
lxtcwin_close_request(GtkWindow *gwin, lxtcwin_t *lxwin)
{
	char *fn = "lxtcwin_close_request()";
	g_print("%s - '%s' - gwin at: %p - lxwin at: %p\n", fn, lxwin->id, (void *)gwin, (void *)lxwin);

// Before doing this, make sure terminals are in closeable state, HOW ????
	g_ptr_array_foreach(lxwin->tabs, lxtctab_detach, NULL);
	lxtcwin_free_at(&lxwin);
	return FALSE; // let GtkWindow handle the rest ...
}

static void
new_tab(lxtcwin_t *win, const gchar *label)
{
	gchar *fn = "new_tab()";
	gchar *tabstr = (label) ? g_strdup(label) : g_strdup_printf("Tab %u", win->tabs->len+1);
	g_print("%s - creating '%s'\n", fn, tabstr);
	lxtctab_t *tab = lxtctab_new(win, tabstr);
	g_ptr_array_add(win->tabs, tab);
	gtk_notebook_append_page(GTK_NOTEBOOK(win->notebook),
		GTK_WIDGET(tab->scrollwin), tab->tab);
	g_free(tabstr);
}

void
lxtcwin_close_tab(lxtcwin_t *win, lxtctab_t *tab)
{
	gchar *fn = "lxtcwin_close_tab()";
	g_print("%s - win: '%s', tab: '%s'\n", fn, win->id, gtk_label_get_text(GTK_LABEL(tab->tab)));
	if (!g_ptr_array_remove(win->tabs, tab)) {
		g_print("%s - tab pointer not found...\n", fn);
	}
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
}

static gboolean
lxtcwin_property_changed(GtkWidget *w, GdkEventProperty event, gpointer lxtcwin)
{
	gchar *fn = "lxtcwin_property_changed()";
	g_print("%s - ...\n", fn);
}

lxtcwin_t *
lxtcwin_new(LxtermcApp *app, const gchar *id)
{
	char *fn = "lxtcwin_new()";
	g_print("%s - '%s' - app at: %p\n", fn, id, (void *)app);

	lxtcwin_t *win = g_new0(lxtcwin_t, 1);
	win->app = app;
	win->id = g_strdup(id);
	win->cmdargs = lxtcapp_steal_cmdargs(app);
	win->cfg = lxtccfg_load(win->cmdargs->cfg);
	win->win = gtk_application_window_new(GTK_APPLICATION(app));
	g_print("%s - new lxtcwin_t struct at: %p - gtkappwin at: %p\n",
		fn, (void *)win, (void *)win->win);

	g_signal_connect(GTK_WINDOW(win->win),
		"close-request", G_CALLBACK(lxtcwin_close_request), win);
	g_signal_connect(GTK_WIDGET(win->win),
		"property-notify-event", G_CALLBACK(lxtcwin_property_changed), win);
	gtk_window_set_title(GTK_WINDOW(win->win),
		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));
	gtk_window_set_default_size(GTK_WINDOW(win->win),
		LXTERMC_DEFAULT_WIDTH, LXTERMC_DEFAULT_HEIGHT);

	win->notebook = gtk_notebook_new();
	win->tabs = g_ptr_array_new();

	if (win->cmdargs->tabs) {
		char *tabs = g_strdup(win->cmdargs->tabs);
		char *at = tabs;
		char *next = NULL;
		do {
			if ((next = strchr(at, ','))) *next = '\0';
			new_tab(win, at);
			if (next) at = next+1;
		} while (next && *at);
		g_free(tabs);
	} else {
		new_tab(win, NULL);
	}

	gtk_window_set_child(GTK_WINDOW(win->win), GTK_WIDGET(win->notebook));
	return win;
}
