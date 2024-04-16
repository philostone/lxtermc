/**/

#ifndef _LXTERMC_TAB_H_
#define _LXTERMC_TAB_H_

#include "lxtermc.h"

G_BEGIN_DECLS

/* one terminal tab */

typedef struct _lxtctab {
	lxtcwin_t		*win;		// back ref to win struct
	GtkScrolledWindow	*scrollwin;	// child of GtkNotebook
	GtkLabel		*label;
	VteTerminal		*vte;
	VtePty			*pty;
} lxtctab_t;

lxtctab_t *lxtctab_new(lxtcwin_t *win, gchar *title);
//void lxtctab_close(lxtctab_t *tab);

/* GDestroyNotify signature, *tab is of (lxtctab_t *) */
void lxtctab_free(void *tab);

/* GFunc signature */
void lxtctab_close(gpointer tab, gpointer data);

G_END_DECLS

#endif /* _LXTERMC_TAB_H_ */