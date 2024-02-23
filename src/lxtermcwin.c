/**/

#include <glib/gi18n.h>
#include <locale.h>

#include "lxtermcapp.h"
#include "lxtermcwin.h"
#include "lxtermc.h"

struct _LxtermcWin {
	GtkApplicationWindow parent_instance;
	// subclass instance variables
	gchar *label;
	cmdargs_t *cmdargs;
};

G_DEFINE_TYPE(LxtermcWin, lxtermc_win, GTK_TYPE_APPLICATION_WINDOW)
/*
char lxtermc_ui[] =
'<?xml version="1.0" encoding="UTF-8"?>
<interface>
  <object id="window" class="GtkWindow">
    <property name="title">Grid</property>
    <child>
      <object id="grid" class="GtkGrid">
        <child>
          <object id="button1" class="GtkButton">
            <property name="label">Button 1</property>
            <layout>
              <property name="column">0</property>
              <property name="row">0</property>
            </layout>
          </object>
        </child>
        <child>
          <object id="button2" class="GtkButton">
            <property name="label">Button 2</property>
            <layout>
              <property name="column">1</property>
              <property name="row">0</property>
            </layout>
          </object>
        </child>
        <child>
          <object id="quit" class="GtkButton">
            <property name="label">Quit</property>
            <layout>
              <property name="column">0</property>
              <property name="row">1</property>
              <property name="column-span">2</property>
            </layout>
          </object>
        </child>
      </object>
    </child>
  </object>
</interface>';
*/
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

void
lxtermc_win_construct(LxtermcWin *win)
{
	char *fn = "lxtermc_win_construct()";
	g_print("%s - '%s' - at: %p\n", fn, win->label, (void *)win);
//	GtkBuilder *builder = gtk_builder_new_from_string(lxtermc_ui, strlen(lxtermc_ui)-1);

	// set title and everything else according to cargs and preferences
	// populate window
	
	g_print("%s - '%s' - setting new window title to %s\n", fn, win->label,
		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));

	gtk_window_set_title(GTK_WINDOW(win),
		((win->cmdargs->title) ? win->cmdargs->title: LXTERMC_NAME));
	gtk_window_set_default_size(GTK_WINDOW(win),
		LXTERMC_DEFAULT_WIDTH, LXTERMC_DEFAULT_HEIGHT);

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
