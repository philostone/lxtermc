/**/

#include "lxtermcwin.h"

struct _LxtermcWin {
	GtkApplicationWindow parent_instance;
	// subclass instance variables
};

typedef struct _LxtermcWinPrivate LxtermcWinPrivate;

struct _LxtermcWinPrivate {
	char *tobeused;
};

G_DEFINE_TYPE(LxtermcWin, lxtermc_win, GTK_TYPE_APPLICATION_WINDOW)
/*
static void
lxtermc_win_destroy(GtkWindow *gtkwin)
{
	g_print("lxtermc_win_destroy() - nothing yet!\n");
	fprintf(stderr, "lxtermc_win_destroy() - nothing yet!\n");
	GTK_WINDOW_CLASS(lxtermc_win_parent_class)->destroy(gtkwin);
}
*/
/*
static void
lxtermc_window_finalize(GObject *gobj)
{
	g_print("lxtermc_window_dispose() - nothing yet!\n");
	fprintf(stderr, "lxtermc_window_dispose() - nothing yet!\n");
	G_OBJECT_CLASS(lxtermc_window_parent_class)->finalize(gobj);
}
*/
static void
lxtermc_win_class_init(LxtermcWinClass *class)
{
	g_print("lxtermc_win_class_init()\n");
	fprintf(stderr, "lxtermc_win_class_init()\n");
	// virtual function overrides
	// property and signal definitions
//	GObjectClass *obj_class = G_OBJECT_CLASS(class);
//	GTK_WINDOW_CLASS(class)->destroy = lxtermc_window_destroy;
//	GTK_APPLICATION_WINDOW_CLASS(class)->finalize = lxtermc_window_finalize;
}

static void
lxtermc_win_init(LxtermcWin *win)
{
	g_print("lxtermc_win_init()\n");
	fprintf(stderr, "lxtermc_win_init()\n");
	// initializations
}

LxtermcWin *
lxtermc_win_new(GtkApplication *app)
{
	g_print("lxtermc_win_new()\n");
	// g_object_new(LXTERMC_TYPE_WINDOW, NULL);
	return LXTERMC_WINDOW(gtk_application_window_new(app));
//	return g_object_new(LXTERMC_WINDOW_TYPE, )
}
