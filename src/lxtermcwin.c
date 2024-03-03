/**/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermcapp.h"
#include "lxtermcwin.h"
#include "lxtermccfg.h"
#include "lxtermcvte.h"
#include "lxtermc.h"

struct _LxtermcWin {
	GtkApplicationWindow parent_instance;
	// subclass instance variables
	gchar *label;
	cmdargs_t *cmdargs;
	lxtermccfg_t *cfg;
	GtkWidget *box;			// vertical: menu + notebook
	GtkWidget *menu;
	GtkWidget *notebook;
	GtkWidget *vte;
};

G_DEFINE_TYPE(LxtermcWin, lxtermc_win, GTK_TYPE_APPLICATION_WINDOW)

void
lxtermc_win_set_cmdargs(LxtermcWin *win, cmdargs_t *cargs)
{
	char *fn ="lxtermc_win_set_cmdargs()";
	g_print("%s - check!\n", fn);
	if (win->cmdargs) {
		g_print("%s - freeing former set of args...\n", fn);
		lxtermc_clear_cmdargs(&(win->cmdargs));
	}
	win->cmdargs = cargs;
}

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

gboolean
lxtermc_win_close(GtkWindow *win)
{
	gchar *fn = "lxtermc_win_close()";
	g_print("%s - win at: %p\n", fn, (void *)win);
	gtk_window_close(GTK_WINDOW(win));
	return FALSE; // let gtk_window handle the close request
}

void
lxtermc_win_destroy(LxtermcWin *win)
{
	gchar *fn = "lxtermc_win_destroy()";
	g_print("%s - win at: %p\n", fn, (void *)win);
	gtk_window_destroy(GTK_WINDOW(win));
}

static void lxtermc_win_dispose(GObject *obj)
{
	gchar *fn = "lxtermc_win_dispose()";
	LxtermcWin *win = LXTERMC_WIN(obj);
	g_print("%s - '%s' - at: %p\n", fn, win->label, (void *)obj);
	G_OBJECT_CLASS(lxtermc_win_parent_class)->dispose(obj);
}

static void lxtermc_win_finalize(GObject *obj)
{
	gchar *fn = "lxtermc_win_finalize()";
	LxtermcWin *win = LXTERMC_WIN(obj);
	g_print("%s - '%s' - at: %p\n", fn, win->label, (void *)obj);
	g_free(win->label);
	win->label = NULL;

	// maybe allocated
	if (win->cmdargs) lxtermc_clear_cmdargs(&(win->cmdargs));
	else g_print("%s - no cmdargs to free, why ???\n", fn);
	G_OBJECT_CLASS(lxtermc_win_parent_class)->finalize(obj);
}

static void
lxtermc_win_class_init(LxtermcWinClass *class)
{
	g_print("lxtermc_win_class_init() - class at: %p\n", (void *)class);
	// virtual function overrides
	// property and signal definitions
//	GObjectClass *obj_class = G_OBJECT_CLASS(class);
//	GTK_WINDOW_CLASS(class)->destroy = lxtermc_win_destroy;
	GTK_WINDOW_CLASS(class)->close_request = lxtermc_win_close;
	G_OBJECT_CLASS(class)->dispose = lxtermc_win_dispose;
	G_OBJECT_CLASS(class)->finalize = lxtermc_win_finalize;
}

static void
lxtermc_win_init(LxtermcWin *win)
{
	g_print("lxtermc_win_init() - win at: %p\n", (void *)win);
	// initializations
}

LxtermcWin *
lxtermc_win_new(LxtermcApp *app, const gchar *label)
{
	g_print("lxtermc_win_new() - '%s' - app at: %p\n", label, (void *)app);
	LxtermcWin *win = g_object_new(LXTERMC_TYPE_WIN, "application", app, NULL);
//	LxtermcWin *win = gtk_application_window_new(GTK_APPLICATION(app));
	win->label = g_strdup(label);
	return win;
}
