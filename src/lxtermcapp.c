/**/

#include <gtk/gtk.h>

#include "lxtermcapp.h"
#include "lxtermc.h"

struct _LxtermcApp {
	GtkApplication parent_instance;
	// subclass instance variables
};

typedef struct _LxtermcWindowPrivate LxtermcWindowPrivate;

struct _LxtermcAppPrivate {
	char *tobeused;
};

G_DEFINE_TYPE(LxtermcApp, lxtermc_app, GTK_TYPE_APPLICATION)

/*
static void
lxtermc_window_destroy(GtkWindow *gtkwin)
{
	g_print("lxtermc_window_destroy() - nothing yet!\n");
	fprintf(stderr, "lxtermc_window_destroy() - nothing yet!\n");
	GTK_WINDOW_CLASS(lxtermc_window_parent_class)->destroy(gtkwin);
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
lxtermc_app_activate(GApplication *app)
{
	g_print("lxtermc_app_activate() - app: %p\n", (void *)app);
}

static gboolean
lxtermc_app_cmdline(GApplication *app, gchar ***args, gint *status)
{
	g_print("lxtermc_app_cmdline() - do nothing yet...\n");
	g_print(" -> app: %p - args: %p - status: %i\n", (void *)app, (void *)args, *status);
	return FALSE;
}

static void
lxtermc_app_class_init(LxtermcAppClass *class)
{
	g_print("lxtermc_app_class_init()\n");
	fprintf(stderr, "lxtermc_app_class_init()\n");
	// virtual function overrides
	// property and signal definitions
//	GObjectClass *obj_class = G_OBJECT_CLASS(class);
//	GTK_WINDOW_CLASS(class)->destroy = lxtermc_window_destroy;
//	GTK_APPLICATION_WINDOW_CLASS(class)->finalize = lxtermc_window_finalize;
	G_APPLICATION_CLASS(class)->activate = lxtermc_app_activate;
	G_APPLICATION_CLASS(class)->local_command_line = lxtermc_app_cmdline;
}

static void
lxtermc_app_init(LxtermcApp *app)
{
	g_print("lxtermc_app_init() - app: %p\n", (void *)app);
	fprintf(stderr, "lxtermc_app_init()\n");
	// initializations
}

LxtermcApp *
lxtermc_app_new(void)
{
	g_print("lxtermc_app_new()\n");
	// g_object_new(LXTERMC_TYPE_WINDOW, NULL);
	//return LXTERMC_WINDOW(gtk_application_window_new(app));
//	return g_object_new(LXTERMC_WINDOW_TYPE, )
	return g_object_new(LXTERMC_TYPE_APP,
		"application-id", LXTERMC_APP_ID,
		"flags", G_APPLICATION_HANDLES_COMMAND_LINE,
		NULL);
}
