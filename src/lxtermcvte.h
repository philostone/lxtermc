/**/

#ifndef _lxtermcvte_h_
#define _lxtermcvte_h_

#include <glib.h>
#include <gtk/gtk.h>

//#include "lxtermc.h"
#include "lxtermccfg.h"

G_BEGIN_DECLS

#define LXTERMC_TYPE_VTE (lxtermc_vte_get_type())

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
G_DECLARE_FINAL_TYPE(LxtermcVte,    lxtermc_vte,    LXTERMC, VTE,      GtkBox)

LxtermcVte *lxtermc_vte_new(const gchar *label, lxtermccfg_t *cfg);
void lxtermc_vte_construct(LxtermcVte *vte);

G_END_DECLS

#endif /* _lxtermcvte_h_ */