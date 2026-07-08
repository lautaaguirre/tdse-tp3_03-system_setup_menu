/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * (Licencia estándar FIUBA...)
 */

#ifndef TASK_SYSTEM_NORMAL_H_
#define TASK_SYSTEM_NORMAL_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "task_system_attribute.h"

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
/* Prototipo de la submáquina inyectando la dependencia de datos */
extern void task_system_normal_statechart(task_system_dta_t *p_task_system_dta);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_NORMAL_H_ */

/********************** end of file ******************************************/
