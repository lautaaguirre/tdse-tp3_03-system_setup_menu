#include "task_sensor_button.h"
#include "task_system_interface.h" // Para put_event_task_system

#define DEL_BTN_MIN 0ul

void task_sensor_button_statechart(const task_sensor_btn_cfg_t *p_cfg, task_sensor_btn_dta_t *p_dta)
{
    /* Lectura cruda de GPIO */
    if (p_cfg->pressed == HAL_GPIO_ReadPin(p_cfg->gpio_port, p_cfg->pin)) {
        p_dta->event = EV_BTN_DOWN;
    } else {
        p_dta->event = EV_BTN_UP;
    }

    /* Tu Máquina de Estados Original */
    switch (p_dta->state)
    {
        case ST_BTN_UP:
            if (EV_BTN_DOWN == p_dta->event) {
                p_dta->tick = p_cfg->tick_max;
                p_dta->state = ST_BTN_FALLING;
            }
            break;

        case ST_BTN_FALLING:
            p_dta->tick--;
            if (DEL_BTN_MIN == p_dta->tick) {
                if (EV_BTN_DOWN == p_dta->event) {
                    put_event_task_system(p_cfg->signal_down); // Inyección al Sistema
                    p_dta->state = ST_BTN_DOWN;
                } else {
                    p_dta->state = ST_BTN_UP;
                }
            }
            break;

        case ST_BTN_DOWN:
            if (EV_BTN_UP == p_dta->event) {
                p_dta->state = ST_BTN_RISING;
                p_dta->tick = p_cfg->tick_max;
            }
            break;

        case ST_BTN_RISING:
            p_dta->tick--;
            if (DEL_BTN_MIN == p_dta->tick) {
                if (EV_BTN_UP == p_dta->event) {
                    put_event_task_system(p_cfg->signal_up);
                    p_dta->state = ST_BTN_UP;
                } else {
                    p_dta->state = ST_BTN_DOWN;
                }
            }
            break;

        default:
            p_dta->tick  = DEL_BTN_MIN;
            p_dta->state = ST_BTN_UP;
            p_dta->event = EV_BTN_UP;
            break;
    }
}
