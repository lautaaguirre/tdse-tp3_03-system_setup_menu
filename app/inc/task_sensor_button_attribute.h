/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 */

#ifndef TASK_SENSOR_BUTTON_ATTRIBUTE_H_
#define TASK_SENSOR_BUTTON_ATTRIBUTE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "task_system_attribute.h"

/* Events to excite Task Sensor Button */
typedef enum task_sensor_ev {
    EV_BTN_UP,
    EV_BTN_DOWN
} task_sensor_ev_t;

/* States of Task Sensor Button */
typedef enum task_sensor_st {
    ST_BTN_UP,
    ST_BTN_FALLING,
    ST_BTN_DOWN,
    ST_BTN_RISING
} task_sensor_st_t;

/* Identifier of Task Sensor Button */
typedef enum task_sensor_id {
    ID_BTN_A,
    ID_BTN_ENT,
    ID_BTN_NEX,
    ID_BTN_ESC,
	ID_BTN_SET
} task_sensor_id_t;

typedef struct {
    task_sensor_id_t  identifier;
    GPIO_TypeDef * gpio_port;
    uint16_t          pin;
    GPIO_PinState     pressed;
    uint32_t          tick_max;
    task_system_ev_t  signal_up;
    task_system_ev_t  signal_down;
} task_sensor_btn_cfg_t;

typedef struct {
    uint32_t          tick;
    task_sensor_st_t  state;
    task_sensor_ev_t  event;
} task_sensor_btn_dta_t;

#ifdef __cplusplus
}
#endif

#endif /* TASK_SENSOR_BUTTON_ATTRIBUTE_H_ */
