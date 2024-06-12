/**/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

//#include "lxtermc.h"		// all lxtermc components are included here
#include "lxtermc.h"
#include "win.h"

struct _LxtermcTab {
	GtkWidget parent_instance;
	
//	lxtcwin_t		*win;		// back ref to win struct
	LxtermcWin		*win;		// back ref to win struct
	GtkScrolledWindow	*scrollwin;	// child of GtkNotebook
	GtkLabel		*label;
	VteTerminal		*vte;
	VtePty			*pty;
};

G_DEFINE_TYPE(LxtermcTab, lxtermc_tab, GTK_TYPE_WIDGET)

GtkWidget *
lxtermc_tab_get_label(LxtermcTab *t)
{
	return GTK_WIDGET(t->label);
}

const gchar *
lxtermc_tab_get_title(LxtermcTab *t)
{
	return gtk_label_get_text(t->label);
}

int
lxtermc_tab_get_rows(LxtermcTab *t)
{
	return vte_terminal_get_row_count(t->vte);
}

int
lxtermc_tab_get_cols(LxtermcTab *t)
{
	return vte_terminal_get_column_count(t->vte);
}

/* GDestroyNotify signature, *tab is of type (LxtermcTab *) */
void
lxtermc_tab_free(void *ptr)
{
	gchar *fn = "lxtermc_tab_free()";
	LxtermcTab *t = LXTERMC_TAB(ptr);
	g_print("%s - '%s' - start!\n", fn, lxtermc_tab_get_title(t));
	g_free(ptr);
	g_print("%s - end\n", fn);
}

/* GFunc signature - for passing to g_ptr_array_foreach() */
void
lxtermc_tab_close(gpointer tab, gpointer data)
{
	gchar *fn = "lxtermc_tab_close()";
	LxtermcTab *t = LXTERMC_TAB(tab);
	LxtermcWin *w = t->win;
	const gchar *str = gtk_label_get_text(t->label);
	if (!w) { g_print("%s - '%s' already closed - end\n", fn, str); return; }
	g_print("%s - '%s' - at: %p - data at:%p\n", fn, str, (void *)t, (void *)data);
	t->win = NULL;
	lxtermc_win_close_tab(w, t);
	g_print("%s - end\n", fn);
}

static void
lxtermc_tab_child_exited(VteTerminal *vte, int status, gpointer data)
{
	gchar *fn ="lxtermc_tab_child_exited()";
	LxtermcTab *t = LXTERMC_TAB(data);
	const gchar *str = gtk_label_get_text(t->label);
	if (!t->win) { g_print("%s - '%s' already closed - end\n", fn, str); return; }
	g_print("%s - '%s', vte at: %p, status: %i\n", fn, str, (void *)vte, status);
	lxtermc_tab_close(t, NULL);
	g_print("%s - '%s' end\n", fn, str);
}

static void
lxtermc_tab_selection_changed(VteTerminal *vte, gpointer data)
{
	gchar *fn ="lxtermc_tab_selection_changed()";
	LxtermcTab *t = LXTERMC_TAB(data);
	const gchar *str = gtk_label_get_text(t->label);
	g_print("%s - '%s', vte at: %p\n", fn, str, (void *)vte);
	g_print("%s - '%s' end\n", fn, str);
}

static const gchar *
lxtermc_tab_preferred_shell()
{
	const gchar *fallback = LXTERMC_FALLBACK_SHELL;
	const gchar *shell = g_getenv("SHELL");
	if (geteuid() == getuid() && getegid() == getgid()
		&& shell != NULL && !access(shell, X_OK)) return shell;

	struct passwd *pw = getpwuid(getuid());
	if (pw && pw->pw_shell && !access(pw->pw_shell, X_OK)) return pw->pw_shell;

	if (access(fallback, X_OK)) return fallback;

	return NULL;
}

static void
lxtermc_tab_spawn_async_callback(VteTerminal *vte, GPid pid, GError *error, void *t_data)
{
	char *fn = "lxtermc_tab_spawn_async_callback()";
	LxtermcTab *t = LXTERMC_TAB(t_data);
	const gchar *str = gtk_label_get_text(t->label); 
	g_print("%s - vte at: %p - pid: %i - error: %p, tab_data: %p\n",
		fn, (void *)vte, pid, (void *)error, (void *)t_data);
	g_print("%s - start!\n", fn);
	g_print("%s - vte  at: %p\n", fn, (void *)vte);
	g_print("%s - tab  at: %p - '%s'\n", fn, (void *)t, str);
	g_print("%s - pty pid: %i\n", fn, pid);
	t->pty = vte_terminal_get_pty(t->vte);
	g_print("%s - pty  at: %p\n", fn, (void *)t->pty);
	g_print("%s - end\n", fn);
}
/*
static void
lxtctab_show(GtkWidget *wid, gpointer *unknown, gpointer *tab)
{
	gchar *fn = "lxtctab_show()";
	g_print("%s - widget at: %p - unknown at: %p - tab at: %p\n",
		fn, (void *)wid, (void *)unknown, (void *)tab);
}

static void
lxtctab_hide(GtkWidget *wid, gpointer *unknown, gpointer *tab)
{
	gchar *fn = "lxtctab_show()";
	g_print("%s - widget at: %p - unknown at: %p - tab at: %p\n",
		fn, (void *)wid, (void *)unknown, (void *)tab);
}
*/

// TODO: rewrite to child init structure...

static void
lxtermc_tab_class_init(LxtermcTabClass *c)
{
	gchar *fn = "lxtermc_tab_class_init()";
	g_print("%s - class at: %p\n", fn, (void *)c);
}

static void
lxtermc_tab_init(LxtermcTab *t)
{
	gchar *fn = "lxtermc_tab_init()";
	g_print("%s - start\n", fn);

	// initializations
	t->scrollwin = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
	t->vte = VTE_TERMINAL(vte_terminal_new());

	// set up terminal properties
	vte_terminal_set_size(t->vte, 20, 20);
	vte_terminal_set_scrollback_lines(t->vte, 100);

	g_signal_connect(t->vte, "child-exited",
		G_CALLBACK(lxtermc_tab_child_exited), t);
	g_signal_connect(t->vte, "selection-changed",
		G_CALLBACK(lxtermc_tab_selection_changed), t);

	// construct main widget
	gtk_scrolled_window_set_child(t->scrollwin, GTK_WIDGET(t->vte));
	gtk_scrolled_window_set_policy(t->scrollwin, GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	gchar **exec = g_malloc(3*sizeof(gchar *));
	exec[0] = g_strdup(lxtermc_tab_preferred_shell());
	exec[1] = g_path_get_basename(exec[0]);		// TODO: user provided path
	exec[2] = NULL;

	vte_terminal_spawn_async(
		VTE_TERMINAL(t->vte),			// terminal
		VTE_PTY_DEFAULT,			// flags
		g_get_current_dir(),			// working directory
		exec,					// child argv
		NULL,					// envv
		G_SPAWN_SEARCH_PATH | G_SPAWN_FILE_AND_ARGV_ZERO,
		NULL,					// child setup func (virtually useless)
		NULL,					// child setup data
		NULL,					// child setup data destroy
		-1,					// default timeout
		NULL,					// cancellable
		lxtermc_tab_spawn_async_callback,	// spawn callback
		t);					// callback user data
	g_strfreev(exec);

//	const gchar *str = lxtermc_tab_get_title(t);
//	g_print("%s - '%s'\n", fn, str);
	g_print("%s - lxtctab_t at: %p\n", fn, (void *)t);
//	g_print("%s - lxtcwin_t at: %p\n", fn, (void *)t->win);
	g_print("%s - scrollwin at: %p\n", fn, (void *)t->scrollwin);
	g_print("%s -       vte at: %p\n", fn, (void *)t->vte);
	g_print("%s - end\n", fn);

	// makes all tabs exit if eof is typed, why?
	//	vte_terminal_feed(VTE_TERMINAL(tab->vte), "lxtermc\n", 8);
}

/*
LxtermcTab *
lxtctab_new(lxtcwin_t *win, gchar *title)
{
	gchar *fn = "lxtctab_new()";
	g_print("%s - '%s' - start!\n", fn, title);
//	lxtctab_t *tab = g_new0(lxtctab_t, 1);
//	LxtermcTab *tab = g_new0(LxtermcTab, 1);
//	g_signal_connect(GTK_WIDGET(tab->vte), "show", G_CALLBACK(lxtctab_show), tab);
//	g_signal_connect(GTK_WIDGET(tab->vte), "hide", G_CALLBACK(lxtctab_hide), tab);
// eof signal comes after child-exited, making it useless ???
//	g_signal_connect(tab->vte, "eof", G_CALLBACK(lxtctab_eof), tab);
	return tab;
}
*/

LxtermcTab *
lxtermc_tab_new(LxtermcWin *w, gchar *title)
{
	gchar *fn = "lxtermc_tab_new()";
	g_print("%s - title: '%s'\n", fn, title);
	LxtermcTab *t = g_object_new(LXTERMC_TYPE_TAB, NULL);
	t->win = w;
	t->label = GTK_LABEL(gtk_label_new(title));
	g_print("%s - end\n", fn);
	return t;
}
