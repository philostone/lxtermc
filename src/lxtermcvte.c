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
#include "lxtermc.h"

struct _LxtermcVte {
	GtkApplicationWindow parent_instance;
	// subclass instance variables
	gchar *label;
	lxtermccfg_t *cfg;		// pointer to parent window cfg
/*
	GtkWidget *box;			// vertical: menu + notebook
	GtkWidget *menu;
	GtkWidget *notebook;

	GtkWidget *vtebox;		// horizontal: vte + scrollbar
	GtkWidget *vte;
	GtkWidget *vtescroll;
*/
};

G_DEFINE_TYPE(LxtermcVte, lxtermc_vte, GTK_TYPE_WIDGET)

static const gchar *
lxtermc_preferred_shell()
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
lxtermc_spawn_async_callback(VteTerminal *vte, int pid, GError *error, void *data)
{
	char *fn = "lxtermc_spawn_async_callback()";
	g_print("%s - check! - pid: %i - error: %p, data: %p\n",
		fn, pid, (void *)error, data);
}

void
lxtermc_vte_construct(LxtermcVte *vte)
{
	char *fn = "lxtermc_vte_construct()";
	g_print("%s - '%s' - at: %p\n", fn, vte->label, (void *)vte);
//	GtkBuilder *builder = gtk_builder_new_from_string(lxtermc_ui, strlen(lxtermc_ui)-1);

	// set title and everything else according to cargs and preferences
//	win->cfg = lxtermc_load_cfg(win->cmdargs->cfg);
//	g_print("%s - '%s' - setting new window title to '%s'\n", fn, win->label,
//		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));

//	gtk_window_set_title(GTK_WINDOW(win),
//		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));
//	gtk_window_set_default_size(GTK_WINDOW(win),
//		LXTERMC_DEFAULT_WIDTH, LXTERMC_DEFAULT_HEIGHT);

	// populate window
/*
	win->notebook = gtk_notebook_new();
	win->vtebox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	win->vtescroll = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, NULL);
	win->vte = vte_terminal_new();

	gtk_notebook_set_group_name(GTK_NOTEBOOK(win->notebook), LXTERMC_APP_ID);

	gtk_box_append(GTK_BOX(win->vtebox), win->vte);
	gtk_box_append(GTK_BOX(win->vtebox), win->vtescroll);
	gtk_window_set_child(GTK_WINDOW(win), win->vtebox);

	gtk_widget_set_visible(win->vtescroll, TRUE);
	gtk_widget_set_visible(win->vte, TRUE);
	gtk_widget_set_visible(win->vtebox, TRUE);

	gchar **exec = g_malloc(3*sizeof(gchar *));
	exec[0] = g_strdup(lxtermc_preferred_shell());
	exec[1] = g_path_get_basename(exec[0]);		// TODO: user provided path
	exec[2] = NULL;

	vte_terminal_spawn_async(
		VTE_TERMINAL(vte),	// terminal
		VTE_PTY_DEFAULT,	// flagse
		g_get_current_dir(),	// working directory
		exec,			// child argv
		NULL,			// envv
		G_SPAWN_SEARCH_PATH | G_SPAWN_FILE_AND_ARGV_ZERO,
		NULL,			// child setup func (virtually useless)
		NULL,			// child setup data
		NULL,			// child setup data destroy
		-1,			// default timeout
		NULL,			// cancellable
		lxtermc_spawn_async_callback,	// spawn callback
		NULL);			// callback user data
	g_strfreev(exec);
*/
}

static void lxtermc_vte_dispose(GObject *obj)
{
	gchar *fn = "lxtermc_vte_dispose()";
	LxtermcVte *vte = LXTERMC_WIN(obj);
	g_print("%s - '%s' - at: %p\n", fn, vte->label, (void *)vte);
	G_OBJECT_CLASS(lxtermc_win_parent_class)->dispose(obj);
}

static void lxtermc_vte_finalize(GObject *obj)
{
	gchar *fn = "lxtermc_vte_finalize()";
	LxtermcVte *vte = LXTERMC_WIN(obj);
	g_print("%s - '%s' - at: %p\n", fn, vte->label, (void *)vte);
	g_free(vte->label);
	vte->label = NULL;
	G_OBJECT_CLASS(lxtermc_vte_parent_class)->finalize(obj);
}

static void
lxtermc_vte_class_init(LxtermcVteClass *class)
{
	g_print("lxtermc_vte_class_init() - class at: %p\n", (void *)class);
	// virtual function overrides
	// property and signal definitions
	G_OBJECT_CLASS(class)->dispose = lxtermc_vte_dispose;
	G_OBJECT_CLASS(class)->finalize = lxtermc_vte_finalize;
}

static void
lxtermc_vte_init(LxtermcVte *vte)
{
	g_print("lxtermc_vte_init() - vte at: %p\n", (void *)vte);
	// initializations
}

LxtermcVte *
lxtermc_vte_new(const gchar *label, lxtermccfg_t *cfg)
{
	g_print("lxtermc_vte_new() - '%s'\n", label);
	LxtermcVte *vte = g_object_new(LXTERMC_TYPE_VTE, "application", NULL);
	vte->label = g_strdup(label);
	vte->cfg = cfg;
	return vte;
}
