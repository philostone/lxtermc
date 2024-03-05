/**/

#ifndef _lxtermcapp_h_
#define _lxtermcapp_h_

#include <glib.h>
#include <gtk/gtk.h>

#include "lxtermc.h"

G_BEGIN_DECLS

#define LXTERMC_TYPE_APP (lxtermc_app_get_type())

/* macro below defined in gobject/gtype.h - expands to the following:
 *	GType lxtermc_app_get_type(void);
 *	G_GNUC_BEGIN_IGNORE_DEPRECATIONS --> _Pragma push disable:1478
 *	typedef struct _LxtermcApp LxtermcApp;
 *	typedef struct { GtkApplicationClass parent_class; } LxtermcAppClass;
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
G_DECLARE_FINAL_TYPE(LxtermcApp,    lxtermc_app,    LXTERMC, APP,      GtkApplication)

LxtermcApp *lxtermc_app_new(const gchar *label);
cmdargs_t *lxtermc_app_steal_cmdargs(LxtermcApp *app);

G_END_DECLS

#endif /* _lxtermcwindow_h_ */