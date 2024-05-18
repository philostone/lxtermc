/**/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"		// all lxtermc components are included here

struct _LxtermcTab {
	GtkWidget parent_instance;
	
	lxtcwin_t		*win;		// back ref to win struct
	GtkScrolledWindow	*scrollwin;	// child of GtkNotebook
	GtkLabel		*label;
	VteTerminal		*vte;
	VtePty			*pty;
} lxtctab_t;




/* GDestroyNotify signature, *tab is of type (lxtctab_t *) */
void
lxtctab_free(void *tab)
{
	gchar *fn = "lxtctab_free()";
	lxtctab_t *t = (lxtctab_t *)tab;
	const gchar *str = gtk_label_get_text(t->label);
	g_print("%s  '%s' - start!\n", fn, str);
	g_free(t);
	g_print("%s - end\n", fn);
}

/* GFunc signature - for passing to g_ptr_array_foreach() */
void lxtctab_close(gpointer tab, gpointer data)
{
	gchar *fn = "lxtctab_close()";
	lxtctab_t *t = (lxtctab_t *)tab;
	lxtcwin_t *w = t->win;
	const gchar *str = gtk_label_get_text(t->label);
	if (!w) { g_print("%s - '%s' already closed - end\n", fn, str); return; }
	g_print("%s - '%s' - at: %p - data at:%p\n", fn, str, (void *)t, (void *)data);
	t->win = NULL;
	lxtcwin_close_tab(w, t);
	g_print("%s - end\n", fn);
}

static void
lxtctab_child_exited(VteTerminal *vte, int status, gpointer data)
{
	gchar *fn ="lxtctab_child_exited()";
	lxtctab_t *tab = (lxtctab_t *)data;
	const gchar *str = gtk_label_get_text(tab->label);
	if (!tab->win) { g_print("%s - '%s' already closed - end\n", fn, str); return; }
	g_print("%s - '%s', vte at: %p, status: %i\n", fn, str, (void *)vte, status);
	lxtctab_close(tab, NULL);
	g_print("%s - '%s' end\n", fn, str);
}

static void
lxtctab_selection_changed(VteTerminal *vte, gpointer data)
{
	gchar *fn ="lxtctab_selection_changed()";
	lxtctab_t *tab = (lxtctab_t *)data;
	const gchar *str = gtk_label_get_text(tab->label);
	g_print("%s - '%s', vte at: %p\n", fn, str, (void *)vte);
	g_print("%s - '%s' end\n", fn, str);
}

static const gchar *
lxtctab_preferred_shell()
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
lxtctab_spawn_async_callback(VteTerminal *vte, GPid pid, GError *error, void *tab_data)
{
	char *fn = "lxtctab_spawn_async_callback()";
	lxtctab_t *tab = (lxtctab_t *)tab_data;
	const gchar *str = gtk_label_get_text(tab->label); 
	g_print("%s - vte at: %p - pid: %i - error: %p, tab_data: %p\n",
		fn, (void *)vte, pid, (void *)error, (void *)tab_data);
	g_print("%s - start!\n", fn);
	g_print("%s - vte  at: %p\n", fn, (void *)vte);
	g_print("%s - tab  at: %p - '%s'\n", fn, (void *)tab, str);
	g_print("%s - pty pid: %i\n", fn, pid);
	tab->pty = vte_terminal_get_pty(tab->vte);
	g_print("%s - pty  at: %p\n", fn, (void *)tab->pty);
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

lxtctab_t *
lxtctab_new(lxtcwin_t *win, gchar *title)
{
	gchar *fn = "lxtctab_new()";
	g_print("%s - '%s' - start!\n", fn, title);

	lxtctab_t *tab = g_new0(lxtctab_t, 1);
	tab->win = win;
	tab->scrollwin = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
	tab->label = GTK_LABEL(gtk_label_new(title));
	tab->vte = VTE_TERMINAL(vte_terminal_new());

//	g_signal_connect(GTK_WIDGET(tab->vte), "show", G_CALLBACK(lxtctab_show), tab);
//	g_signal_connect(GTK_WIDGET(tab->vte), "hide", G_CALLBACK(lxtctab_hide), tab);

	// set up terminal properties
	vte_terminal_set_size(tab->vte, 20, 20);
	vte_terminal_set_scrollback_lines(tab->vte, 100);

	g_signal_connect(tab->vte, "child-exited",
		G_CALLBACK(lxtctab_child_exited), tab);
	g_signal_connect(tab->vte, "selection-changed",
		G_CALLBACK(lxtctab_selection_changed), tab);

// eof signal comes after child-exited, making it useless ???
//	g_signal_connect(tab->vte, "eof", G_CALLBACK(lxtctab_eof), tab);

	// construct main widget
	gtk_scrolled_window_set_child(tab->scrollwin, GTK_WIDGET(tab->vte));
	gtk_scrolled_window_set_policy(tab->scrollwin, GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	gchar **exec = g_malloc(3*sizeof(gchar *));
	exec[0] = g_strdup(lxtctab_preferred_shell());
	exec[1] = g_path_get_basename(exec[0]);		// TODO: user provided path
	exec[2] = NULL;

	vte_terminal_spawn_async(
		VTE_TERMINAL(tab->vte),		// terminal
		VTE_PTY_DEFAULT,		// flags
		g_get_current_dir(),		// working directory
		exec,				// child argv
		NULL,				// envv
		G_SPAWN_SEARCH_PATH | G_SPAWN_FILE_AND_ARGV_ZERO,
		NULL,				// child setup func (virtually useless)
		NULL,				// child setup data
		NULL,				// child setup data destroy
		-1,				// default timeout
		NULL,				// cancellable
		lxtctab_spawn_async_callback,	// spawn callback
		tab);				// callback user data
	g_strfreev(exec);

	g_print("%s - '%s'\n", fn, title);
	g_print("%s - lxtctab_t at: %p\n", fn, (void *)tab);
	g_print("%s - lxtcwin_t at: %p\n", fn, (void *)tab->win);
	g_print("%s - scrollwin at: %p\n", fn, (void *)tab->scrollwin);
	g_print("%s -       vte at: %p\n", fn, (void *)tab->vte);
	g_print("%s - end\n", fn);

	// makes all tabs exit if eof is typed, why?
//	vte_terminal_feed(VTE_TERMINAL(tab->vte), "lxtermc\n", 8);

	return tab;
}
