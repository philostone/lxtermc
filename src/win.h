/**/

#ifndef _lxtermcwin_h_
#define _lxtermcwin_h_

#include <glib.h>
#include <gtk/gtk.h>

#include "lxtermc.h"
#include "app.h"
#include "cfg.h"

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
	GPtrArray *tabs;	// lxtcvte_t pointers
} lxtcwin_t;

lxtcwin_t *lxtcwin_new(LxtermcApp *app, const gchar *id);
void lxtcwin_clear(lxtcwin_t **win);

gboolean lxtcwin_close(GtkWindow *gwin, lxtcwin_t *lxwin);

//#define LXTERMC_TYPE_WIN (lxtermc_win_get_type())

/* macro defined in gobject/gtype.h - expands to the following:
 *	GType module_obj_name##_get_type(void);
 *	G_GNUC_BEGIN_IGNORE_DEPRECATIONS --> _Pragma push disable:1478
 *	typedef struct _##ModuleObjName ModuleObjName;
 *	typedef struct { ParentName##Class parent_class; } ModuleObjName##Class;
 *
 *	_GLIB_DEFINE_AUTOPTR_CHAINUP(ModuleObjName, ParentName)
 *	G_DEFINE_AUTOPTR_CLEANUP_FUNC (ModuleObjName##Class, g_type_class_unref)
 *
 *	G_GNUC_UNUSED static inline ModuleObjName * MODULE##_##OBJ_NAME (gpointer ptr) {
 *		return G_TYPE_CHECK_INSTANCE_CAST(ptr, module_obj_name##_get_type(), ModuleObjName); }
 *	G_GNUC_UNUSED static inline gboolean MODULE##_IS_##OBJ_NAME (gpointer ptr) {
 *		return G_TYPE_CHECK_INSTANCE_TYPE(ptr, module_obj_name##_get_type()); }
 *	G_GNUC_END_IGNORE_DEPRECATIONS -> _Pragma pop
 *
 * whare params are: ModuleObjName  module_obj_name MODULE   OBJ_NAME  ParentName */
//G_DECLARE_FINAL_TYPE(LxtermcWin,    lxtermc_win,    LXTERMC, WIN,      GtkApplicationWindow)

//LxtermcWin *lxtermc_win_new(LxtermcApp *app, const gchar *label);
//void lxtermc_win_set_cmdargs(LxtermcWin *win, cmdargs_t *cargs);
//void lxtermc_win_construct(LxtermcWin *win);

//void lxtermc_win_destroy(LxtermcWin *win);
//gboolean lxtermc_win_close(GtkWindow *win);

G_END_DECLS

#endif /* _lxtermcwin_h_ */