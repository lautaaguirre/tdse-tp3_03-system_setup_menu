/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 */

#ifndef TASK_SENSOR_ADC_ATTRIBUTE_H_
#define TASK_SENSOR_ADC_ATTRIBUTE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#include "task_system_attribute.h"

/* States of Task Sensor ADC */
typedef enum {
    ST_ADC_IDLE,
    ST_ADC_WAIT_IT,
    ST_ADC_PROCESS
} task_sensor_adc_st_t;

typedef struct {
    ADC_HandleTypeDef * hadc;
    uint32_t            threshold;     // Ej: 2048 p/ sobreconsumo
    uint32_t            sample_period; // Ej: 100ms
    task_system_ev_t    signal_alarm;  // Evento a inyectar al System
} task_sensor_adc_cfg_t;

typedef struct {
    uint32_t             tick;
    task_sensor_adc_st_t state;
    bool                 flag_it_ready; // Bandera q levanta el callback HW
    uint32_t             raw_value;     // Valor crudo capturado
} task_sensor_adc_dta_t;

#ifdef __cplusplus
}
#endif

#endif /* TASK_SENSOR_ADC_ATTRIBUTE_H_ */
