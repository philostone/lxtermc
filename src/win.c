/**/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"
#include "win.h"
#include "app.h"
#include "cfg.h"
#include "vte.h"

void
lxtcwin_clear(lxtcwin_t **win)
{
	g_print("lxtcwin_clear() - '%s' - at: %p\n", (*win)->id, (void *)*win);
	g_free((*win)->id);
	lxtermc_clear_cmdargs(&(*win)->cmdargs);
	lxtccfg_clear(&(*win)->cfg);
	*win = NULL;
}

gboolean
lxtcwin_close(GtkWindow *gwin, lxtcwin_t *lxwin)
{
	char *fn = "lxtcwin_close()";
	g_print("%s - '%s' - gwin at: %p - lxwin at: %p\n", fn, lxwin->id, (void *)gwin, (void *)lxwin);
	lxtcwin_clear(&lxwin);
	return FALSE; // let GtkWindow handle the rest ...
}

lxtcwin_t *
lxtcwin_new(LxtermcApp *app, const gchar *id)
{
	char *fn = "lxtcwin_new()";
	g_print("%s - '%s' - app at: %p\n", fn, id, (void *)app);

	lxtcwin_t *lxtcw = g_new0(lxtcwin_t, 1);
	lxtcw->app = app;
	lxtcw->id = g_strdup(id);
	lxtcw->cmdargs = lxtcapp_steal_cmdargs(app);
	lxtcw->cfg = lxtccfg_load(lxtcw->cmdargs->cfg);
	lxtcw->win = gtk_application_window_new(GTK_APPLICATION(app));
	g_print("%s - new lxtcwin_t struct at: %p - gtkappwin at: %p\n",
		fn, (void *)lxtcw, (void *)lxtcw->win);

	g_signal_connect(GTK_WINDOW(lxtcw->win),
		"close-request", G_CALLBACK(lxtcwin_close), lxtcw);

	gtk_window_set_title(GTK_WINDOW(lxtcw->win),
		((lxtcw->cmdargs->title) ? lxtcw->cmdargs->title: LXTERMC_NAME));
	gtk_window_set_default_size(GTK_WINDOW(lxtcw->win),
		LXTERMC_DEFAULT_WIDTH, LXTERMC_DEFAULT_HEIGHT);

	lxtcw->notebook = gtk_notebook_new();
	lxtcw->tabs = g_ptr_array_new();

	// TODO: from cmdargs load multiple tabs ...
	lxtcvte_t *lxtcv = lxtcvte_new(NULL);
	g_ptr_array_add(lxtcw->tabs, lxtcv);
	gtk_notebook_append_page(GTK_NOTEBOOK(lxtcw->notebook),
		GTK_WIDGET(lxtcv->scrollwin), NULL);

	gtk_window_set_child(GTK_WINDOW(lxtcw->win), GTK_WIDGET(lxtcw->notebook));

//	gtk_notebook_append_page(GTK_WIDGET(lxtcw->notebook), GTK_WIDGET(...), NULL);
	return lxtcw;
}

/*
void
lxtermc_win_construct(LxtermcWin *win)
{
	char *fn = "lxtermc_win_construct()";
	g_print("%s - '%s' - at: %p\n", fn, win->label, (void *)win);
//	GtkBuilder *builder = gtk_builder_new_from_string(lxtermc_ui, strlen(lxtermc_ui)-1);

	// set title and everything else according to cargs and preferences
	win->cfg = lxtermc_load_cfg(win->cmdargs->cfg);
	g_print("%s - '%s' - setting new window title to '%s'\n", fn, win->label,
		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));

	gtk_window_set_title(GTK_WINDOW(win),
		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));
	gtk_window_set_default_size(GTK_WINDOW(win),
		LXTERMC_DEFAULT_WIDTH, LXTERMC_DEFAULT_HEIGHT);

	// populate window
	win->vte = GTK_WIDGET(lxtermc_vte_new(win->label, win->cfg));
	lxtermc_vte_construct(LXTERMC_VTE(win->vte));

//	gtk_notebook_set_group_name(GTK_NOTEBOOK(win->notebook), LXTERMC_APP_ID);

	gtk_window_set_child(GTK_WINDOW(win), win->vte);
	gtk_widget_set_visible(win->vte, TRUE);

//	gtk_range_set_adjustment(GTK_RANGE(win->vtescroll),
//		gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(win->vte)));
}
*/
