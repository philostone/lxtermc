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

struct _LxtermcWin {
	GtkApplicationWindow parent_instance;
	// subclass instance variables
	gchar *label;
	cmdargs_t *cmdargs;
	lxtermccfg_t *cfg;
	GtkWidget *box;			// vertical: menu + notebook
	GtkWidget *menu;
	GtkWidget *notebook;

	GtkWidget *vtebox;		// horizontal: vte + scrollbar
	GtkWidget *vte;
	GtkWidget *vtescroll;
};

G_DEFINE_TYPE(LxtermcWin, lxtermc_win, GTK_TYPE_APPLICATION_WINDOW)

void
lxtermc_win_set_cmdargs(LxtermcWin *win, cmdargs_t *cargs)
{
	char *fn ="lxtermc_win_set_cmdargs()";
	if (win->cmdargs) {
		g_print("%s - freeing former set of args...\n", fn);
		lxtermc_clear_cmdargs(&(win->cmdargs));
	}
	win->cmdargs = cargs;
}

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
	win->notebook = gtk_notebook_new();
	win->vtebox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	win->vtescroll = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, NULL);
	win->vte = vte_terminal_new();

	GtkWidget *test_vte = lxtermc_vte_new();

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
		VTE_TERMINAL(win->vte),	// terminal
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
/*
static void
lxtermc_win_activate(GApplication *app)
{
	gchar *fn = "lxtermc_win_activate()";
	LxtermcWin *lxwin = LXTERMC_WIN(app);
	g_print("%s - '%s' - at: %p\n", fn, lxwin->label, (void *)app);
*/
/*
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

	gtk_window_set_child(GTK_WINDOW(win), box);

	GtkWidget *hello_button = gtk_button_new_with_label(_("Hello gtk4"));
	g_signal_connect(hello_button, "clicked", G_CALLBACK(print_hello), NULL);

	GtkWidget *locale_button = gtk_button_new_with_label(_("Locale"));
	g_signal_connect(locale_button, "clicked", G_CALLBACK(display_locale), NULL);

	GtkWidget *close_button = gtk_button_new_with_label(_("Exit"));
	g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(lxtermc_win_destroy), win);

	gtk_box_append(GTK_BOX(box), hello_button);
	gtk_box_append(GTK_BOX(box), locale_button);
	gtk_box_append(GTK_BOX(box), close_button);
	gtk_window_present(GTK_WINDOW(win));
*/
/*
	g_print("%s - end\n", fn);
}
*/

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
//	G_APPLICATION_CLASS(class)->activate = lxtermc_win_activate;
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
	win->label = g_strdup(label);
	return win;
}
