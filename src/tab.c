/**/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"		// all lxtermc components are included here

void
lxtctab_free_at(lxtctab_t **tab)
{
	char *fn = "lxtctab_free_at()";
	g_print("%s  '%s' - start!\n", fn, gtk_label_get_text(GTK_LABEL((*tab)->tab)));
//	g_object_unref((*tab)->tab);
//	g_object_unref((*tab)->scrollwin);
//	g_object_unref((*tab)->vte);
	g_free(*tab);
	*tab = NULL;
}

/*
void
lxtctab_destroy(GtkWidget *gwid, lxtctab_t *tab)
{
	char *fn = "lxtctab_destroy()";
	g_print("%s - '%s' - gwid at: %p - lxtab at: %p\n",
		fn, gtk_label_get_text(GTK_LABEL(tab->tab)), (void *)gwid, (void *)tab);
	lxtctab_clear(&tab);
}
*/

/* GFunc */
void lxtctab_detach(gpointer tab_ptr, gpointer data)
{
	gchar *fn = "lxtctab_detach()";
	lxtctab_t *tab = (lxtctab_t *)tab_ptr;
	g_print("%s - '%s' - at: %p - data at:%p\n",
		fn, gtk_label_get_text(GTK_LABEL(tab->tab)), (void *)tab, (void *)data);
	tab->win = NULL;
}

void lxtctab_close(lxtctab_t *tab)
{
	gchar *fn = "lxtctab_close()";
	g_print("%s - '%s' - at: %p\n",
		fn, gtk_label_get_text(GTK_LABEL(tab->tab)), (void *)tab);
	if (tab->win) lxtcwin_close_tab(tab->win, tab);
	lxtctab_free_at(&tab);
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
lxtctab_spawn_async_callback(VteTerminal *vte, GPid pid, GError *error, void *tab)
{
	char *fn = "lxtctab_spawn_async_callback()";
//	g_print("%s - vte at: %p - pid: %i - error: %p, tab: %p\n",
//		fn, (void *)vte, pid, (void *)error, tab);
	g_print("%s - start!\n", fn);
	g_print("%s - vte  at: %p\n", fn, (void *)vte);
	g_print("%s - tab  at: %p\n", fn, (void *)tab);
	g_print("%s - pty pid: %i\n", fn, pid);
	((lxtctab_t *)tab)->pty = vte_terminal_get_pty(VTE_TERMINAL(((lxtctab_t *)tab)->vte));
	g_print("%s - pty  at: %p\n", fn, (void *)((lxtctab_t *)tab)->pty);
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
	char *fn = "lxtctab_new()";
	g_print("%s - '%s' - start!\n", fn, title);

	lxtctab_t *tab = g_new0(lxtctab_t, 1);
	tab->win = win;
	tab->tab = gtk_label_new(title);
	tab->scrollwin = gtk_scrolled_window_new();
	tab->vte = GTK_WIDGET(lxtermc_vte_new(tab));

//	g_signal_connect(GTK_WIDGET(tab->scrollwin),
//		"destroy", G_CALLBACK(lxtctab_destroy), tab);

//	g_signal_connect(GTK_WIDGET(tab->vte), "show", G_CALLBACK(lxtctab_show), tab);
//	g_signal_connect(GTK_WIDGET(tab->vte), "hide", G_CALLBACK(lxtctab_hide), tab);

	// set up terminal properties
	vte_terminal_set_size(VTE_TERMINAL(tab->vte), 20, 20);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(tab->vte), 100);

	// construct main widget
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(tab->scrollwin),
		GTK_WIDGET(tab->vte));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(tab->scrollwin),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

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

	// makes all tabs exit if eof is typed, why?
//	vte_terminal_feed(VTE_TERMINAL(tab->vte), "lxtermc\n", 8);

	return tab;
}
