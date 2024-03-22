/**/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <sys/types.h>		// this and next for getpwuid
#include <pwd.h>
#include <vte/vte.h>

#include "lxtermc.h"
#include "vte.h"
#include "tab.h"

struct _LxtermcVte {
	VteTerminal parent_instance;

	// subclass instance variables
	lxtctab_t *tab;
};

G_DEFINE_TYPE(LxtermcVte, lxtermc_vte, VTE_TYPE_TERMINAL)

static void
vte_child_exited(VteTerminal *vte, gint status)
{
	char *fn = "vte_child_exited()";
	g_print("%s - at: %p, status: %i\n", fn, (void *)vte, status);
	lxtcwin_close_tab(LXTERMC_VTE(vte->win), LXTERMC_VTE(vte)->tab);
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

static void
lxtermc_vte_class_init(LxtermcVteClass *class)
{
	gchar *fn = "lxtermc_vte_class_init()";
	g_print("%s - class at: %p\n", fn, (void *)class);
	VTE_TERMINAL_CLASS(class)->child_exited = vte_child_exited;
//	VTE_TERMINAL_CLASS(class)->eof = vte_eof;
}

static void
lxtermc_vte_init(LxtermcVte *vte)
{
	gchar *fn = "lxtermc_vte_init()";
	g_print("%s - vte at_ %p\n", fn, (void *)vte);
}

LxtermcVte *
lxtermc_vte_new(lxtctab_t *tab)
{
	char *fn = "lxtermc_vte_new()";
	g_print("%s - start!\n", fn);
	LxtermcVte *vte = g_object_new(LXTERMC_TYPE_VTE, NULL);
	vte->tab = tab;
	return vte;
}
