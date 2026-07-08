/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 */

#ifndef TASK_SYSTEM_ATTRIBUTE_H_
#define TASK_SYSTEM_ATTRIBUTE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Modos para excitar a la Task System */
typedef enum task_system_mode {
    NORMAL,
    SETUP,
    FALLA,            /* <--- NUEVO: Modo de falla crítica */
    MODE_QTY
} task_system_mode_t;

/* Eventos del Sistema */
typedef enum task_system_ev {
    EV_SYS_IDLE,
    EV_SYS_ENTER,
    EV_SYS_NEXT,
    EV_SYS_ESCAPE,
    EV_SYS_BTN_A,
	EV_SYS_SETUP_BTN,
    EV_SYS_SENSOR_OBSTACLE,
    EV_SYS_FAULT_STALL        /* <--- NUEVO: Sobre-consumo en Motores (ADC Potenciómetro) */
} task_system_ev_t;

/* Estados Globales de las Máquinas de Estado del Sistema */
typedef enum task_system_st {
    ST_SYS_IDLE,
    ST_SYS_ACTIVE,
    ST_SYS_MAIN,
    ST_SYS_MENU_1,
    ST_SYS_MENU_2,
    ST_SYS_NORMAL_CLEANING,
    ST_SYS_NORMAL_AVOIDING,
    ST_SYS_FALLA_MAIN         /* <--- NUEVO: Estado de enclavamiento de la falla */
} task_system_st_t;

typedef struct task_system_dta {
    task_system_ev_t event;
    task_system_st_t state;
    bool             flag;
    uint32_t         tick;
} task_system_dta_t;

#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_ATTRIBUTE_H_ */
