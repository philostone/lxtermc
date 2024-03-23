/**/

#ifndef _LXTERMC_VTE_H_
#define _LXTERMX_VTE_H_

G_BEGIN_DECLS

#include "lxtermc.h"

#define LXTERMC_TYPE_VTE (lxtermc_vte_get_type())
G_DECLARE_FINAL_TYPE(LxtermcVte, lxtermc_vte, LXTERMC, VTE, VteTerminal)
LxtermcVte *lxtermc_vte_new(lxtctab_t *tab);

G_END_DECLS

#endif /* _LXTERMC_VTE_H_ */
