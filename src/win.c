/**/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"
#include "app.h"
#include "cfg.h"
#include "tab.h"

struct _LxtermcWin {
	GtkWidget		parent_instance;

	// subclass instance variabled
	LxtermcApp		*app;		// back ref to main application instance
	gchar			*id;		// win id string
//	cmdargs_t		*cmdargs;	// ownership stealed from app
	gchar			*title;		// window title
	gchar			*cmd_tabs;	// comma separated list of tab titles
	lxtccfg_t		*cfg;		// prefs for this window instance
	GdkDisplay		*display;
	GtkCssProvider		*provider;

	// window components
	GtkNotebook		*book;		// window child
	GPtrArray		*tabs;		// LxtermcTab pointers
	LxtermcTab		*visible_tab;
	int			rows;		// vte pty number of rows
	int			cols;		// and cols
};

G_DEFINE_TYPE(LxtermcWin, lxtermc_win, GTK_TYPE_APPLICATION_WINDOW)

void
lxtermc_win_set_cmd_tabs(LxtermcWin *w, gchar *cmd_tabs)
{
	gchar *fn = "lxtermc_win_set_cmd_tabs()";
	g_print("%s", fn);
	if (!cmd_tabs) g_print(" - cmd_tabs null pointer");
	if (!w) g_print(" - win null pointer");
	else w->cmd_tabs = cmd_tabs;
	g_print("\n");
}

void
lxtermc_win_set_cfg(LxtermcWin *w, lxtccfg_t *cfg)
{
	gchar *fn = "lxtermc_win_set_cfg()";
	g_print("%s - to struct at: %p", fn, (void *)cfg);
	if (!w) g_print(" - win null pointer");
	else w->cfg = cfg;
	g_print("\n");
}

void
lxtermc_win_set_title(LxtermcWin *w, gchar *title)
{
	gchar *fn = "lxtermc_win_set_title()";
	g_print("%s - at: %p - to '%s'", fn, (void *)w, title);
	if (!w) g_print(" - win null pointer");
	else w->title = title;
	g_print("\n");
}

/* GDestroyNotify signature, *win is of type (LxtermcWin *) */
void
lxtermc_win_free(void *ptr)
{
	gchar *fn = "lxtermc_win_free()";
	LxtermcWin *w = LXTERMC_WIN(ptr);
	g_print("%s - '%s' - at: %p\n", fn, w->id, ptr);
	g_free(w->id);
//	lxtermc_cmdargs_free(w->cmdargs);
	lxtccfg_free(w->cfg);
	g_ptr_array_free(w->tabs, TRUE); // calls lxtermc_tab_free()
	g_print("%s - end\n", fn);
}

/* GFunc signature - for passing to g_ptr_array_forech() */
void lxtermc_win_close(gpointer ptr, gpointer data)
{
	gchar *fn = "lxtermc_win_close()";
	LxtermcWin *w = LXTERMC_WIN(ptr);
	g_print("%s - '%s ', lxwin at: %p - data at: %p\n",
		fn, w->id, (void *)w, (void *)data);
	g_ptr_array_foreach(w->tabs, lxtermc_tab_close, NULL);
	g_print("%s - end, passing task to gtk_window_close()\n", fn);
	gtk_window_close(GTK_WINDOW(w));
}

/* GTK_WINDOW:close-request signal handler
 * default handler is called afterwards (if return value is FALSE) */
gboolean
lxtermc_win_close_request(GtkWindow *gwin, LxtermcWin *w)
{
	gchar *fn = "lxtcwin_close_request()";
	g_print("%s - '%s' - gwin at: %p - lxwin at: %p\n",
		fn, w->id, (void *)gwin, (void *)w);

// Before doing this, make sure terminals are in closeable state, HOW ????
// TODO: use gtk... close tabs etc...
	g_print("%s - end, passing task to GtkWindow\n", fn);
	return FALSE; // let GtkWindow handle the rest ...
}

static void
lxtermc_win_new_tab(LxtermcWin *w, const gchar *label)
{
	gchar *fn = "lxtermc_win_new_tab()";
	gchar *tabstr = (label) ? g_strdup(label) : g_strdup_printf("Tab %u", w->tabs->len+1);
	g_print("%s - at:%p - label: '%s' -> creating '%s' - tabs at: %p\n",
		fn, (void *)w, label, tabstr, (void *)w->tabs);
	LxtermcTab *t = lxtermc_tab_new(w, tabstr);
	lxtermc_tab_construct(t);
	g_ptr_array_add(w->tabs, t);
	gtk_notebook_append_page(w->book, GTK_WIDGET(t), lxtermc_tab_get_label(t));
	g_free(tabstr);
	g_print("%s - end\n", fn);
}

void
lxtermc_win_close_tab(LxtermcWin *w, LxtermcTab *t)
{
	gchar *fn = "lxtcwin_close_tab()";
	const gchar *str = lxtermc_tab_get_title(t);
	g_print("%s - '%s' at: %p - win: '%s' at: %p\n",
		fn, str, (void *)t, w->id, (void *)w);
	int p = gtk_notebook_page_num(w->book, GTK_WIDGET(t));
	int n = gtk_notebook_get_n_pages(w->book);
	g_print("%s - remove page index: %i among %i pages\n", fn, p, n);
	if (n < 1 || p < 0) {
		g_print("%s - n: %i, p: %i - nothing to close - end\n", fn, n, p);
		return;
	}
	if (!g_ptr_array_remove(w->tabs, t)) { // calls lxtermc_tab_free()
		g_print("%s - '%s' - tab pointer not found...\n", fn, str);
	}
	if (n < 2) {
		g_print("%s - n: %i, closing the window - end\n", fn, n);
		gtk_window_close(GTK_WINDOW(w));
		return;
	}
	gtk_notebook_remove_page(w->book, p);
	g_print("%s - '%s' at: %p, removed from win '%s' at: %p\n",
		fn, str, (void *)t, w->id, (void *)w);

	g_print("%s - end\n", fn);
}

/* GTK_WINDOW - notify::default-[width|heigh] handler */
static void
lxtermc_win_size(LxtermcWin *w)
{
	gchar *fn = "lxtermc_win_size()";
	if (!w->book) { g_print("%s - notebook not filled yet\n", fn); return; }
	int p = gtk_notebook_get_current_page(w->book);
	if (p < 0) { g_print("%s - no pages in notebook\n", fn); return; }
	if (!w->tabs || w->tabs->len < 1) { g_print("%s - no tabs in window\n", fn); return; }
	if ((guint)p > w->tabs->len) { g_print("%s - more pages than tabs\n", fn); return; }
	LxtermcTab *t = LXTERMC_TAB(g_ptr_array_index(w->tabs, p));

	int rows = lxtermc_tab_get_rows(t);
	int cols = lxtermc_tab_get_cols(t);
	if (w->rows != rows || w->cols != cols) {
		g_print("%s - new pty size, %i x %i - end\n", fn, rows, cols);
		w->rows = rows;
		w->cols = cols;
	} else {
		g_print("%s - end\n", fn);
	}
}

/* GTK_WINDOW - notify::is-active handler */
static void
lxtermc_win_active(LxtermcWin *w)
{
	gchar *fn = "lxtermc_win_active()";
	gboolean active = gtk_window_is_active(GTK_WINDOW(w));
	g_print("%s - '%s' is active: %s\n", fn, w->id, ((active) ? "true" : "false"));
}

/* GTK_NOTEBOOK:switch-page signal handler
 * default handler is called afterwards */
static void
lxtermc_win_tab_switched(GtkNotebook *book, GtkWidget *wid, guint num, LxtermcWin *w)
{
	gchar *fn = "lxtermc_win_tab_switched()";
//	if (page < 1) { g_print("%s - to nothing\n", fn); return; }
//	if (gtk_notebook_get_n_pages(nb) < 1) { g_print("%s - to nothing\n", fn); return; }

	g_print("%s - wid: %p, num: %i, win: %p, book: %p\n",
		fn, (void *)wid, num, (void *)w, (void *)book);

	LxtermcTab *t = LXTERMC_TAB(g_ptr_array_index(w->tabs, num));
	if ((void *)t != (void *)wid) {
		g_print("%s - notebook child mismatch, why? lxtermctab: %p, wid: %p\n",
			fn, (void *)t, (void *)wid);
		return;
	}

	w->visible_tab = t;
	const gchar *str = lxtermc_tab_get_title(t);
	g_print("%s - switched to tab '%s'\n", fn, str);

	// this does not work, probably because of default handler called afterwards...
//	if (!gtk_widget_grab_focus(GTK_WIDGET(tab->vte))) {
//		g_print("%s - vte failed to grab focus\n", fn);
//	}

	g_print("%s - end\n", fn);
}

void
lxtermc_win_construct(LxtermcWin *w)
{
	gchar *fn = "lxtermc_win_construct()";
	if (!w) g_print("%s - win null pointer\n", fn);
	g_print("%s - at: %p - start\n", fn, (void *)w);

	// GtkNotebook initialization
	w->book = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_notebook_set_group_name(w->book, LXTERMC_NAME);
	g_signal_connect(w->book, "switch-page",
		G_CALLBACK(lxtermc_win_tab_switched), w);
	gtk_window_set_child(GTK_WINDOW(w), GTK_WIDGET(w->book));

// ToDo: check if already constructed...

	if (w->cmd_tabs) {
		char *tabs = g_strdup(w->cmd_tabs);
		char *at = tabs;
		char *next = NULL;
		do {
			if ((next = strchr(at, ','))) *next = '\0';
			lxtermc_win_new_tab(w, at);
			if (next) at = next+1;
		} while (next && *at);
		g_free(tabs);
	} else {
		lxtermc_win_new_tab(w, NULL);
	}

	// final initializations
	int p = gtk_notebook_get_current_page(w->book);
	w->visible_tab = g_ptr_array_index(w->tabs, p);
	const gchar *str = lxtermc_tab_get_title(w->visible_tab);
	gtk_window_set_title(GTK_WINDOW(w),
		((w->title) ? w->title: LXTERMC_NAME));

// should be done by tab...
//	if (!gtk_widget_grab_focus(GTK_WIDGET(w->visible_tab->vte))) {
//		g_print("%s - vte failed to grab focus\n", fn);
//	}
	g_print("%s - current page is '%s' : %i\n", fn, str, p);
	g_print("%s - end\n", fn);
}

static void
lxtermc_win_dispose(GObject *obj)
{
	gchar *fn = "lxtermc_win_dispose()";
	g_print("%s - at: %p - start & end, handing over to parent...\n", fn, (void *)obj);
	G_OBJECT_CLASS(lxtermc_win_parent_class)->dispose(obj);
}

static void
lxtermc_win_finalize(GObject *obj)
{
	gchar *fn = "lxtermc_win_finalize()";
	g_print("%s - at: %p - start & end, handing over to parent...\n", fn, (void *)obj);
	G_OBJECT_CLASS(lxtermc_win_parent_class)->finalize(obj);
}

static void
lxtermc_win_class_init(LxtermcWinClass *c)
{
	gchar *fn = "lxtermc_win_class_init()";
	g_print("%s - at: %p - start\n", fn, (void *)c);

	// virtual function overrides
	G_OBJECT_CLASS(c)->dispose = lxtermc_win_dispose;
	G_OBJECT_CLASS(c)->finalize = lxtermc_win_finalize;

	// property and signal definitions
	g_print("%s - at: %p - end\n", fn, (void *)c);
}

static void
lxtermc_win_init(LxtermcWin *w)
{
	gchar *fn = "lxtermc_win_init()";
	g_print("%s - at: %p - start\n", fn, (void *)w);

	// initializations
	w->display = gdk_display_get_default();
	w->provider = gtk_css_provider_new();
	w->rows = 0;
	w->cols = 0;

	// GptArray of (LxtermcTab *) initialization
	w->tabs = g_ptr_array_new_with_free_func(lxtermc_tab_free);

	// GtkWindow initialization
	gtk_window_set_default_size(GTK_WINDOW(w),
		LXTERMC_DEFAULT_WIDTH, LXTERMC_DEFAULT_HEIGHT);
	g_signal_connect(GTK_WINDOW(w), "close-request",
		G_CALLBACK(lxtermc_win_close_request), w);
	g_signal_connect_swapped(GTK_WINDOW(w), "notify::default-width",
		G_CALLBACK(lxtermc_win_size), w);
	g_signal_connect_swapped(GTK_WINDOW(w), "notify::default-height",
		G_CALLBACK(lxtermc_win_size), w);
	g_signal_connect_swapped(GTK_WINDOW(w), "notify::is-active",
		G_CALLBACK(lxtermc_win_active), w);
	g_print("%s - at: %p - end\n", fn, (void *)w);
}

LxtermcWin *
lxtermc_win_new(LxtermcApp *a, const gchar *id)
{
	gchar *fn = "lxtermc_win_new()";
	g_print("%s - app at: %p, id: %s\n", fn, (void *)a, id);
	LxtermcWin *w = g_object_new(LXTERMC_TYPE_WIN,
		"application", GTK_APPLICATION(a),
		NULL);
	w->app = a;
	w->id = g_strdup(id);
	g_print("%s - at: %p - end\n", fn, (void *)w);
	return w;
}
