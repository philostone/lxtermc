/**/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"		// all components are included here

struct _LxtermcVte {
	VteTerminal parent_instance;

	// subclass instance variables
	lxtctab_t *tab;		// back reference
};

G_DEFINE_TYPE(LxtermcVte, lxtermc_vte, VTE_TYPE_TERMINAL)

static void
lxtermc_vte_child_exited(VteTerminal *vte, gint status)
{
	gchar *fn = "lxtermc_vte_child_exited()";
//	lxtctab_t *tab = LXTERMC_VTE(vte);
	g_print("%s - at: %p, status: %i\n", fn, (void *)vte, status);

	lxtctab_close(LXTERMC_VTE(vte)->tab);
//	VTE_TERMINAL_CLASS(lxtermc_vte_parent_class)->child_exited(vte, status);
}

/*
static void
vte_eof(VteTerminal *vte)
{
	char *fn = "vte_eof()";
	g_print("%s - at: %p\n", fn, (void *)vte);
//	VTE_TERMINAL_CLASS(lxtermc_vte_parent_class)->eof(vte);
}
*/

static void lxtermc_vte_resize_window(VteTerminal *vte, guint width, guint height)
{
	gchar *fn = "lxtermc_vte_resize_window()";
	g_print("%s - at: %p - to width: %u, height: %u\n", fn, (void *)vte, width, height);
}

static void
lxtermc_vte_class_init(LxtermcVteClass *class)
{
	gchar *fn = "lxtermc_vte_class_init()";
	g_print("%s - class at: %p\n", fn, (void *)class);
	VTE_TERMINAL_CLASS(class)->child_exited = lxtermc_vte_child_exited;
	VTE_TERMINAL_CLASS(class)->resize_window = lxtermc_vte_resize_window;
//	VTE_TERMINAL_CLASS(class)->eof = vte_eof;
}

static void
lxtermc_vte_init(LxtermcVte *vte)
{
	gchar *fn = "lxtermc_vte_init()";
	g_print("%s - vte at: %p\n", fn, (void *)vte);
}

//LxtermcVte *
GtkWidget *
lxtermc_vte_new(lxtctab_t *tab)
{
	char *fn = "lxtermc_vte_new()";
//	LxtermcVte *vte = g_object_new(LXTERMC_TYPE_VTE, NULL);
	GtkWidget *vte = g_object_new(LXTERMC_TYPE_VTE, NULL);
	g_print("%s - new vte at: %p\n", fn, (void *)vte);
	LXTERMC_VTE(vte)->tab = tab;
	return vte;
}
