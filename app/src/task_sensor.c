/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 * (Licencia estándar FIUBA...)
 */

/********************** inclusions *******************************************/
#include "main.h"
#include "logger.h"


/* Inclusión de las interfaces de nuestras submáquinas */
#include "task_sensor_button.h"
#include "task_sensor_adc.h"
#include "task_system_attribute.h"

/********************** macros and definitions *******************************/
/* Hardware para botones según CubeMX */
#define BTN_A_PORT   GPIOA
#define BTN_A_PIN    GPIO_PIN_0
#define BTN_ENT_PORT GPIOA
#define BTN_ENT_PIN  GPIO_PIN_1
#define BTN_NEX_PORT GPIOA
#define BTN_NEX_PIN  GPIO_PIN_4
#define BTN_ESC_PORT GPIOB
#define BTN_ESC_PIN  GPIO_PIN_0

#define BTN_PRESSED  GPIO_PIN_RESET
#define DEL_BTN_MAX  50ul  /* 50 milisegundos de antirrebote */

/********************** internal data definition *****************************/
/* 1. CONFIGURACIÓN E INSTANCIACIÓN DE BOTONES */
#define SENSOR_BTN_QTY 4
const task_sensor_btn_cfg_t sensor_btn_cfg_list[SENSOR_BTN_QTY] = {
    {ID_BTN_A,   BTN_A_PORT,   BTN_A_PIN,   BTN_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_BTN_A},
    {ID_BTN_ENT, BTN_ENT_PORT, BTN_ENT_PIN, BTN_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_ENTER},
    {ID_BTN_NEX, BTN_NEX_PORT, BTN_NEX_PIN, BTN_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_NEXT},
    {ID_BTN_ESC, BTN_ESC_PORT, BTN_ESC_PIN, BTN_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_ESCAPE}
};
static task_sensor_btn_dta_t sensor_btn_dta_list[SENSOR_BTN_QTY];

/* 2. CONFIGURACIÓN E INSTANCIACIÓN DE ADCs */
extern ADC_HandleTypeDef hadc1; /* Exportado desde main.c generado por CubeMX */

#define SENSOR_ADC_QTY 1
const task_sensor_adc_cfg_t sensor_adc_cfg_list[SENSOR_ADC_QTY] = {
    {&hadc1, 2048, 100, EV_SYS_FAULT_STALL} /* Dispara falla si el potenciómetro supera la mitad */
};
static task_sensor_adc_dta_t sensor_adc_dta_list[SENSOR_ADC_QTY];

/********************** external functions definition ************************/
void task_sensor_init(void *parameters)
{
    uint32_t i;
    LOGGER_INFO("  task_sensor is running - Tick [mS] = %lu", HAL_GetTick());

    /* Inicializar estado dinámico (DTA) de Botones */
    for (i = 0; i < SENSOR_BTN_QTY; i++) {
        sensor_btn_dta_list[i].state = ST_BTN_UP;
        sensor_btn_dta_list[i].event = EV_BTN_UP;
        sensor_btn_dta_list[i].tick  = 0;
    }

    /* Inicializar estado dinámico (DTA) de ADCs */
    for (i = 0; i < SENSOR_ADC_QTY; i++) {
        sensor_adc_dta_list[i].state = ST_ADC_IDLE;
        sensor_adc_dta_list[i].tick  = 0;
        sensor_adc_dta_list[i].flag_it_ready = false;
        sensor_adc_dta_list[i].raw_value = 0;
    }
}

void task_sensor_update(void *parameters)
{
    uint32_t i;

    /* * ¡AQUÍ ESTÁ LA MAGIA MODULAR!
     * task_sensor no tiene switches. Solo delega la ejecución de la
     * máquina de estados a la función correspondiente de cada módulo.
     */

    /* Barrido (Polling) de todos los botones para el antirrebote */
    for (i = 0; i < SENSOR_BTN_QTY; i++) {
        task_sensor_button_statechart(&sensor_btn_cfg_list[i], &sensor_btn_dta_list[i]);
    }

    /* Actualización asíncrona de los canales ADC */
    for (i = 0; i < SENSOR_ADC_QTY; i++) {
        task_sensor_adc_statechart(&sensor_adc_cfg_list[i], &sensor_adc_dta_list[i]);
    }
}

/********************** end of file ******************************************/
