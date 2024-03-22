/**/

#ifndef _lxtermctab_h_
#define _lxtermctab_h_

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
void lxtctab_clear(lxtctab_t *tab);

G_END_DECLS

#endif /* _lxtermctab_h_ */