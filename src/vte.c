/**/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"
#include "app.h"
#include "win.h"
#include "cfg.h"
#include "vte.h"

void
lxtcvte_clear(lxtcvte_t **vte)
{
	char *fn = "lxtcvte_clear()";
	g_print("%s  '%s' - start!\n", fn, (*vte)->title);
	g_free((*vte)->title);
	*vte = NULL;
}

void
lxtcvte_scrollwin_destroy(GtkWidget *gwid, lxtcvte_t *lxvte)
{
	char *fn = "lxtcvte_destroy()";
	g_print("%s - '%s' - gwid at: %p - lxvte at: %p\n",
		fn, lxvte->title, (void *)gwid, (void *)lxvte);
	lxtcvte_clear(&lxvte);
}

static const gchar *
lxtcvte_preferred_shell()
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
lxtcvte_spawn_async_callback(VteTerminal *vte, int pid, GError *error, void *data)
{
	char *fn = "lxtcvte_spawn_async_callback()";
	g_print("%s - vte at: %p - pid: %i - error: %p, data: %p\n",
		fn, (void *)vte, pid, (void *)error, data);
}

lxtcvte_t *
lxtcvte_new(gchar *title)
{
	char *fn = "lxtcvte_new()";
	g_print("%s - '%s' - start!\n", fn, title);

	lxtcvte_t *lxtcv = g_new0(lxtcvte_t, 1);
	lxtcv->tab = gtk_label_new(title);
	lxtcv->scrollwin = gtk_scrolled_window_new();
	lxtcv->vte = vte_terminal_new();
	g_signal_connect(GTK_WIDGET(lxtcv->scrollwin),
		"destroy", G_CALLBACK(lxtcvte_scrollwin_destroy), lxtcv);

	// set up terminal properties
	vte_terminal_set_size(VTE_TERMINAL(lxtcv->vte), 100, 100);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(lxtcv->vte), 1000);

	// pack the main widget
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(lxtcv->scrollwin),
		GTK_WIDGET(lxtcv->vte));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(lxtcv->scrollwin),
		GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);

//	g_print("%s - '%s' - at: %p\n", fn, vte->label, (void *)vte);

//	vte->scroll = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, NULL);
//	vte->vte = vte_terminal_new();
//	gtk_range_set_adjustment(GTK_RANGE(lxvte->scroll),
//		gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(lxvte->vte)));

//	gtk_box_append(GTK_BOX(vte), vte->vte);
//	gtk_box_append(GTK_BOX(vte), vte->scroll);

//	gtk_widget_set_visible(vte->scroll, TRUE);
//	gtk_widget_set_visible(vte->vte, TRUE);
//	gtk_widget_set_visible(GTK_WIDGET(vte), TRUE);

	gchar **exec = g_malloc(3*sizeof(gchar *));
	exec[0] = g_strdup(lxtcvte_preferred_shell());
	exec[1] = g_path_get_basename(exec[0]);		// TODO: user provided path
	exec[2] = NULL;

	vte_terminal_spawn_async(
		VTE_TERMINAL(lxtcv->vte),	// terminal
		VTE_PTY_DEFAULT,		// flagse
		g_get_current_dir(),		// working directory
		exec,				// child argv
		NULL,				// envv
		G_SPAWN_SEARCH_PATH | G_SPAWN_FILE_AND_ARGV_ZERO,
		NULL,				// child setup func (virtually useless)
		NULL,				// child setup data
		NULL,				// child setup data destroy
		-1,				// default timeout
		NULL,				// cancellable
		lxtcvte_spawn_async_callback,	// spawn callback
		NULL);				// callback user data
	g_strfreev(exec);

	return lxtcv;
}




/*
struct _LxtermcVte {
	GtkWidget parent_instance;
	// subclass instance variables
	gchar *label;
	lxtermccfg_t *cfg;		// pointer to parent window cfg

	GtkWidget *vte;
	GtkWidget *scroll;
};

G_DEFINE_TYPE(LxtermcVte, lxtermc_vte, GTK_TYPE_BOX)
*/
/*
static void lxtermc_vte_dispose(GObject *obj)
{
	gchar *fn = "lxtermc_vte_dispose()";
	LxtermcVte *vte = LXTERMC_VTE(obj);
	g_print("%s - '%s' - at: %p\n", fn, vte->label, (void *)vte);
	G_OBJECT_CLASS(lxtermc_vte_parent_class)->dispose(obj);
}
*/
/*
static void lxtermc_vte_finalize(GObject *obj)
{
	gchar *fn = "lxtermc_vte_finalize()";
	LxtermcVte *vte = LXTERMC_VTE(obj);
	g_print("%s - '%s' - at: %p\n", fn, vte->label, (void *)vte);
	g_free(vte->label);
	vte->label = NULL;
	G_OBJECT_CLASS(lxtermc_vte_parent_class)->finalize(obj);
}
*/
/*
static void
lxtermc_vte_class_init(LxtermcVteClass *class)
{
	g_print("lxtermc_vte_class_init() - class at: %p\n", (void *)class);
	// virtual function overrides
	// property and signal definitions
	G_OBJECT_CLASS(class)->dispose = lxtermc_vte_dispose;
	G_OBJECT_CLASS(class)->finalize = lxtermc_vte_finalize;
}
*/
/*
void
lxtermc_vte_construct(LxtermcVte *vte)
{
	char *fn = "lxtermc_vte_construct()";
	g_print("%s - '%s' - at: %p\n", fn, vte->label, (void *)vte);

	vte->scroll = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, NULL);
	vte->vte = vte_terminal_new();
//	gtk_range_set_adjustment(GTK_RANGE(lxvte->scroll),
//		gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(lxvte->vte)));

	gtk_box_append(GTK_BOX(vte), vte->vte);
	gtk_box_append(GTK_BOX(vte), vte->scroll);

	gtk_widget_set_visible(vte->scroll, TRUE);
	gtk_widget_set_visible(vte->vte, TRUE);
	gtk_widget_set_visible(GTK_WIDGET(vte), TRUE);

	gchar **exec = g_malloc(3*sizeof(gchar *));
	exec[0] = g_strdup(lxvte_preferred_shell());
	exec[1] = g_path_get_basename(exec[0]);		// TODO: user provided path
	exec[2] = NULL;

	vte_terminal_spawn_async(
		VTE_TERMINAL(vte->vte),		// terminal
		VTE_PTY_DEFAULT,		// flagse
		g_get_current_dir(),		// working directory
		exec,				// child argv
		NULL,				// envv
		G_SPAWN_SEARCH_PATH | G_SPAWN_FILE_AND_ARGV_ZERO,
		NULL,				// child setup func (virtually useless)
		NULL,				// child setup data
		NULL,				// child setup data destroy
		-1,				// default timeout
		NULL,				// cancellable
		lxvte_spawn_async_callback,	// spawn callback
		NULL);				// callback user data
	g_strfreev(exec);
}
*/
/*
static void
lxtermc_vte_init(LxtermcVte *vte)
{
	char *fn = "lxtermc_vte_init()";
//	g_print("lxtermc_vte_init() - vte at: %p\n", (void *)vte);
	g_print("%s - at: %p\n", fn, (void *)vte);

	// initializations
}
*/
/*
LxtermcVte *
lxtermc_vte_new(const gchar *label, lxtermccfg_t *cfg)
{
	g_print("lxtermc_vte_new() - '%s'\n", label);
	LxtermcVte *vte = g_object_new(LXTERMC_TYPE_VTE, NULL);
	vte->label = g_strdup(label);
	vte->cfg = cfg;
	return vte;
}
*/
