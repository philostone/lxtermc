/**/

#ifndef _LXTERMC_WIN_H_
#define _LXTERMC_WIN_H_

#include "lxtermc.h"

G_BEGIN_DECLS

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
	lxtctab_t *visible_tab;
	int rows;		// vte pty number of rows
	int cols;		// vte pty number of cols
} lxtcwin_t;

lxtcwin_t *lxtcwin_new(LxtermcApp *app, const gchar *id);
void lxtcwin_free_at(lxtcwin_t **win);

/* GFunc */
void lxtcwin_close(gpointer win, gpointer data);

gboolean lxtcwin_close_request(GtkWindow *gwin, lxtcwin_t *lxwin);
void lxtcwin_close_tab(lxtcwin_t *win, lxtctab_t *tab);

G_END_DECLS

#endif /* _LXTERMC_WIN_H_ */