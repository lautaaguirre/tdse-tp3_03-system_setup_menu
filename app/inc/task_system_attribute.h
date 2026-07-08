/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * (Licencia estándar FIUBA...)
 */

#ifndef TASK_SYSTEM_ATTRIBUTE_H_
#define TASK_SYSTEM_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdint.h>
#include <stdbool.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Modos para excitar a la Task System */
typedef enum task_system_mode {
    NORMAL,
    SETUP,
    MODE_QTY
} task_system_mode_t;

/* Eventos del Sistema (Recibidos desde Sensor u otras tareas) */
typedef enum task_system_ev {
    EV_SYS_IDLE,
    EV_SYS_ENTER,
    EV_SYS_NEXT,
    EV_SYS_ESCAPE,
    EV_SYS_BTN_A,
    EV_SYS_SENSOR_OBSTACLE   /* Evento reactivo de hardware */
} task_system_ev_t;

/* Estados Globales de las Máquinas de Estado del Sistema */
typedef enum task_system_st {
    ST_SYS_IDLE,
    ST_SYS_ACTIVE,
    ST_SYS_MAIN,
    ST_SYS_MENU_1,
    ST_SYS_MENU_2,
    ST_SYS_NORMAL_CLEANING,  /* Estado normal: limpiando */
    ST_SYS_NORMAL_AVOIDING   /* Estado normal: evadiendo obstáculo */
} task_system_st_t;

/* Estructura de Datos de la Tarea */
typedef struct task_system_dta {
    task_system_ev_t event;
    task_system_st_t state;
    bool             flag;
    uint32_t         tick;   /* Temporizador no bloqueante */
} task_system_dta_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
