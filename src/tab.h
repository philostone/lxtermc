/**/

#ifndef _LXTERMC_TAB_H_
#define _LXTERMC_TAB_H_

G_BEGIN_DECLS

#include "lxtermc.h"

/* one terminal tab */

typedef struct _lxtctab {
//	gchar		*title;		// tab label text
	lxtcwin_t	*win;		// back ref to win struct
	GtkWidget	*tab;
	GtkWidget	*scrollwin;
	GtkWidget	*vte;
} lxtctab_t;

// win.h requires lxtctab_t
//#include "win.h"		// for lxtcwin_t (win.h require lxtctab_t)

lxtctab_t *lxtctab_new(lxtcwin_t *win, gchar *title);
void lxtctab_close(lxtctab_t *tab);
//void lxtctab_clear(lxtctab_t **tab);

G_END_DECLS

#endif /* _LXTERMC_TAB_H_ */