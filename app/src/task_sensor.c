#include "main.h"
#include "logger.h"
#include "board.h"

#include "task_sensor_button.h"
#include "task_sensor_adc.h"
#include "task_system_attribute.h"

#define DEL_BTN_MAX  50ul  /* 50 ms de antirrebote */

/* 1. MAPEO FÍSICO DE BOTONES
 * Se corrige la nomenclatura del array para utilizar estrictamente las abstracciones
 * (_PIN, _PORT y _PRESSED) definidas en el board.h, evitando errores de compilación
 * por mezcla de labels crudos de CubeMX con las macros del board.
 */
#define SENSOR_BTN_QTY 4
const task_sensor_btn_cfg_t sensor_btn_cfg_list[SENSOR_BTN_QTY] = {
    {ID_BTN_ENT, BTN_ENT_PORT, BTN_ENT_PIN, BTN_ENT_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_ENTER},
    {ID_BTN_NEX, BTN_NEX_PORT, BTN_NEX_PIN, BTN_NEX_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_NEXT},
    {ID_BTN_ESC, BTN_ESC_PORT, BTN_ESC_PIN, BTN_ESC_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_ESCAPE},
    {ID_BTN_SET, BTN_SET_PORT, BTN_SET_PIN, BTN_SET_PRESSED, DEL_BTN_MAX, EV_SYS_IDLE, EV_SYS_SETUP_BTN}
};
static task_sensor_btn_dta_t sensor_btn_dta_list[SENSOR_BTN_QTY];

/* 2. MAPEO FÍSICO DE SENSORES ANALÓGICOS
 * Usamos Instancias separadas (ADC1 y ADC2) para evitar colisiones de Hardware
 */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

#define SENSOR_ADC_QTY 2
const task_sensor_adc_cfg_t sensor_adc_cfg_list[SENSOR_ADC_QTY] = {
    {&hadc1, 2048, 100, EV_SYS_FAULT_STALL},       // Potenciómetro: Chequea consumo c/ 100ms
    {&hadc2, 1500,  50, EV_SYS_SENSOR_OBSTACLE}    // Sharp IR: Chequea pared c/ 50ms
};
static task_sensor_adc_dta_t sensor_adc_dta_list[SENSOR_ADC_QTY];

void task_sensor_init(void *parameters)
{
    uint32_t i;
    for (i = 0; i < SENSOR_BTN_QTY; i++) {
        sensor_btn_dta_list[i].state = ST_BTN_UP;
        sensor_btn_dta_list[i].event = EV_BTN_UP;
        sensor_btn_dta_list[i].tick  = 0;
    }
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
    for (i = 0; i < SENSOR_BTN_QTY; i++) {
        task_sensor_button_statechart(&sensor_btn_cfg_list[i], &sensor_btn_dta_list[i]);
    }
    for (i = 0; i < SENSOR_ADC_QTY; i++) {
        task_sensor_adc_statechart(&sensor_adc_cfg_list[i], &sensor_adc_dta_list[i]);
    }
}
