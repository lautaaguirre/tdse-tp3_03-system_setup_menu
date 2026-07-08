/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * (Licencia estándar FIUBA...)
 */

#ifndef TASK_SYSTEM_INTERFACE_H_
#define TASK_SYSTEM_INTERFACE_H_

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
/* Funciones principales de la tarea */
extern void task_system_init(void *parameters);
extern void task_system_update(void *parameters);

/* Función para cambiar el modo de la aspiradora */
extern void task_system_set_mode(task_system_mode_t task_system_mode);

/* Interfaces FIFO para manejo de eventos de la capa System */
extern void init_event_task_system(void);
extern void put_event_task_system(task_system_ev_t event);
extern task_system_ev_t get_event_task_system(void);
extern bool any_event_task_system(void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_INTERFACE_H_ */

/********************** end of file ******************************************/
