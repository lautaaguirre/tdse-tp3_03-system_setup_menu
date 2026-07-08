#include "task_sensor_adc.h"
#include "task_system_interface.h"

/* Banderas compartidas con la interrupción (Hardware) */
static volatile bool     g_adc_it_flag = false;
static volatile uint32_t g_adc_raw_val = 0;

/* Callback disparado desde app_it.c cuando el HW termina de medir */
void task_sensor_adc_it_callback(uint32_t raw_value)
{
    g_adc_raw_val = raw_value;
    g_adc_it_flag = true;
}

void task_sensor_adc_statechart(const task_sensor_adc_cfg_t *p_cfg, task_sensor_adc_dta_t *p_dta)
{
    switch (p_dta->state)
    {
        case ST_ADC_IDLE:
            /* Temporizador basado en Ticks p/ no saturar la CPU con el ADC */
            if (p_dta->tick > 0) {
                p_dta->tick--;
            }

            if (0 == p_dta->tick)
            {
                /* Gatillar HW y cambiar de estado p/ esperar asincrónicamente */
                HAL_ADC_Start_IT(p_cfg->hadc);
                p_dta->state = ST_ADC_WAIT_IT;
            }
            break;

        case ST_ADC_WAIT_IT:
            /* Solo avanzamos si el hardware nos avisó que terminó */
            if (true == g_adc_it_flag)
            {
                g_adc_it_flag = false;
                p_dta->raw_value = g_adc_raw_val; // Guardado seguro en memoria local
                p_dta->state = ST_ADC_PROCESS;
            }
            break;

        case ST_ADC_PROCESS:
            /* Evaluar la lógica del sensor */
            if (p_dta->raw_value > p_cfg->threshold)
            {
                put_event_task_system(p_cfg->signal_alarm); // Dispara FALLA
            }

            /* Reiniciar el loop */
            p_dta->tick = p_cfg->sample_period;
            p_dta->state = ST_ADC_IDLE;
            break;

        default:
            p_dta->state = ST_ADC_IDLE;
            break;
    }
}
