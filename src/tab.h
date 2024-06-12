
#ifndef _LXTERMC_TAB_H_
#define _LXTERMC_TAB_H_

#include "lxtermc.h"
#include "win.h"

G_BEGIN_DECLS

/* one terminal tab */

#define LXTERMC_TYPE_TAB (lxtermc_tab_get_type())
G_DECLARE_FINAL_TYPE(LxtermcTab, lxtermc_tab, LXTERMC, TAB, GtkWidget)

GtkWidget *lxtermc_tab_get_label(LxtermcTab *t);
const gchar *lxtermc_tab_get_title(LxtermcTab *t);

int lxtermc_tab_get_rows(LxtermcTab *t);
int lxtermc_tab_get_cols(LxtermcTab *t);

/* GDestroyNotify signature, *ptr is of (LxtermcTab *) */
void lxtermc_tab_free(void *ptr);

/* GFunc signature */
void lxtermc_tab_close(gpointer tab, gpointer data);

void lxtermc_tab_construct(LxtermcTab *t);

LxtermcTab *lxtermc_tab_new(LxtermcWin *w, gchar *title);

//GtkWidget *lxtermc_tab_new(LxtermcWin *win, gchar *title);
/*
typedef struct _lxtctab {
	lxtcwin_t		*win;		// back ref to win struct
	GtkScrolledWindow	*scrollwin;	// child of GtkNotebook
	GtkLabel		*label;
	VteTerminal		*vte;
	VtePty			*pty;
} lxtctab_t;
*/

//lxtctab_t *lxtctab_new(lxtcwin_t *win, gchar *title);
//void lxtctab_close(lxtctab_t *tab);

/* GDestroyNotify signature, *tab is of (lxtctab_t *) */
//void lxtctab_free(void *tab);

/* GFunc signature */
//void lxtctab_close(gpointer tab, gpointer data);

G_END_DECLS

#endif /* _LXTERMC_TAB_H_ */