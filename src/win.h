/**/

#ifndef _LXTERMC_WIN_H_
#define _LXTERMC_WIN_H_

G_BEGIN_DECLS

#include "lxtermc.h"

/* top level terminal window */

typedef struct _lxtcwin {
	LxtermcApp	*app;		// main application instance
	gchar		*id;
	cmdargs_t 	*cmdargs;	// ownership stealed from app
	lxtccfg_t	*cfg;		// preferences for this window

	// window components
	GtkWidget *win;		// main window
	GtkWidget *notebook;	// window child
	GPtrArray *tabs;	// lxtctab_t pointers
} lxtcwin_t;

lxtcwin_t *lxtcwin_new(LxtermcApp *app, const gchar *id);
void lxtcwin_free_at(lxtcwin_t **win);

gboolean lxtcwin_close(GtkWindow *gwin, lxtcwin_t *lxwin);
void lxtcwin_close_tab(lxtcwin_t *win, lxtctab_t *tab);

G_END_DECLS

#endif /* _LXTERMC_WIN_H_ */