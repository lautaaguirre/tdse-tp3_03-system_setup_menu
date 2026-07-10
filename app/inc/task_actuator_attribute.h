/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 */

#ifndef TASK_ACTUATOR_ATTRIBUTE_H_
#define TASK_ACTUATOR_ATTRIBUTE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* Eventos que inyecta el System */
typedef enum task_actuator_ev {
    EV_ACT_IDLE,
    EV_ACT_MOTORS_FORWARD,
    EV_ACT_MOTORS_REVERSE,
    EV_ACT_MOTORS_SPIN,
    EV_ACT_MOTORS_STOP
} task_actuator_ev_t;

/* --- NUEVO: Estados físicos explícitos --- */
typedef enum task_actuator_st {
    ST_ACT_STOPPED,
    ST_ACT_MOVING_FWD,
    ST_ACT_MOVING_REV,
    ST_ACT_SPINNING
} task_actuator_st_t;

/* Identificadores de los 4 Canales PWM */
typedef enum task_actuator_id {
    ID_MOT_L_FWD,
    ID_MOT_L_REV,
    ID_MOT_R_FWD,
    ID_MOT_R_REV
} task_actuator_id_t;

/* Configuración Constante (Flash) */
typedef struct {
    task_actuator_id_t  identifier;
    TIM_HandleTypeDef* htim;
    uint32_t            channel;
    uint32_t            pwm_fwd;
    uint32_t            pwm_rev;
    uint32_t            pwm_spin;
    uint32_t            pwm_stop;
} task_actuator_cfg_t;

/* Datos Dinámicos (RAM) */
typedef struct {
    uint32_t            tick;
    task_actuator_st_t  state;
    task_actuator_ev_t  event;
    bool                flag;
} task_actuator_dta_t;

/* Agregamos el extern para que la interfaz pueda usar esta variable */
extern task_actuator_dta_t task_actuator_dta_list[];

#ifdef __cplusplus
}
#endif

#endif /* TASK_ACTUATOR_ATTRIBUTE_H_ */
