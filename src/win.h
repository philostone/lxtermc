/**/

#ifndef _LXTERMC_WIN_H_
#define _LXTERMC_WIN_H_

#include "lxtermc.h"
#include "win.h"
#include "cfg.h"
#include "tab.h"

G_BEGIN_DECLS

/* top level terminal window */

#define LXTERMC_TYPE_WIN (lxtermc_win_get_type())
G_DECLARE_FINAL_TYPE(LxtermcWin, lxtermc_win, LXTERMC, WIN, GtkApplicationWindow)

/* GDestroyNotify signature, *win is is of type (LxtermcWin *) */
void lxtermc_win_free(void *ptr);

/* GFunc signature */
void lxtermc_win_close(gpointer ptr, gpointer data);

gboolean lxtermc_win_close_request(GtkWindow *gwin, LxtermcWin *win);

void lxtermc_win_close_tab(LxtermcWin *w, LxtermcTab *t);

LxtermcWin *lxtermc_win_new(LxtermcApp *a, const gchar *id);

void lxtermc_win_set_cfg(LxtermcWin *w, lxtccfg_t *cfg);
void lxtermc_win_set_cmd_tabs(LxtermcWin *w, gchar *cmd_tabs);
void lxtermc_win_set_title(LxtermcWin *w, gchar *title);
void lxtermc_win_construct(LxtermcWin *w);

/*
typedef struct _lxtcwin {
	LxtermcApp		*app;		// main application instance
	gchar			*id;		// win id string
	cmdargs_t 		*cmdargs;	// ownership stealed from app
	lxtccfg_t		*cfg;		// preferences for this window
	GdkDisplay		*display;
	GtkCssProvider		*provider;

	// window components
	GtkApplicationWindow	*win;		// top level window
	GtkNotebook		*notebook;	// window child (GtkNotebook)
//	GPtrArray		*tabs;		// lxtctab_t pointers
	GPtrArray		*tabs;		// LxtermcTab pointers
	lxtctab_t		*visible_tab;
	int rows;				// vte pty number of rows
	int cols;				// vte pty number of cols
} lxtcwin_t;
*/

//lxtcwin_t *lxtcwin_new(LxtermcApp *app, const gchar *id);
//gboolean lxtcwin_close_request(GtkWindow *gwin, lxtcwin_t *lxwin);
//void lxtcwin_close_tab(lxtcwin_t *win, lxtctab_t *tab);
//void lxtcwin_close_tab(lxtcwin_t *win, LxtermcTab *tab);

/* GDestroyNotify signature, *win is is of type (lxtcwin_t *) */
//void lxtcwin_free(void *win);

/* GFunc signature */
//void lxtcwin_close(gpointer win, gpointer data);

G_END_DECLS

#endif /* _LXTERMC_WIN_H_ */