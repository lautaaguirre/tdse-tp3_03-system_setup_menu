#include "task_system_normal.h"
#include "task_system_interface.h"
#include "task_actuator_interface.h"
#include "task_display_interface.h"
#include "task_actuator_attribute.h"

typedef enum {
    PHASE_STOP,
    PHASE_REVERSE,
    PHASE_SPIN
} avoid_phase_t;

void task_system_normal_statechart(task_system_dta_t *p_task_system_dta)
{
    static avoid_phase_t fase_evasion = PHASE_STOP;

    if (p_task_system_dta->tick > 0) {
            p_task_system_dta->tick--;
    }

    if (true == any_event_task_system())
    {
        p_task_system_dta->flag = true;
        p_task_system_dta->event = get_event_task_system();
    }

    switch (p_task_system_dta->state)
    {
        case ST_SYS_IDLE:
            if (true == p_task_system_dta->flag)
            {
                if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_NORMAL_CLEANING;
                    put_event_task_display(0, 0, "LIMPIANDO...    ");
                    put_event_task_display(0, 1, "                ");
                    put_event_task_actuator(EV_ACT_MOTORS_FORWARD);
                }
                /* SALTO MANUAL HACIA EL SETUP MEDIANTE EL BOTÓN 4 */
                else if (EV_SYS_SETUP_BTN == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_IDLE; // Reset p/ la prox vez
                    task_system_set_mode(SETUP);
                }
            }
            break;

        case ST_SYS_NORMAL_CLEANING:
            if (true == p_task_system_dta->flag)
            {
                if (EV_SYS_FAULT_STALL == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_IDLE;
                    task_system_set_mode(FALLA);
                }
                else if (EV_SYS_SENSOR_OBSTACLE == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_NORMAL_AVOIDING;
                    fase_evasion = PHASE_STOP;
                    p_task_system_dta->tick = 200;

                    put_event_task_display(0, 0, "OBSTACULO!      ");
                    put_event_task_display(0, 1, "EVADIENDO...    ");
                    put_event_task_actuator(EV_ACT_MOTORS_STOP);
                }
                else if (EV_SYS_ESCAPE == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_IDLE;
                    put_event_task_actuator(EV_ACT_MOTORS_STOP);
                    put_event_task_display(0, 0, "MODO NORMAL     ");
                    put_event_task_display(0, 1, "ENTER: EMPEZAR  ");
                }
            }
            break;

        case ST_SYS_NORMAL_AVOIDING:
            if (true == p_task_system_dta->flag) {
                if (EV_SYS_FAULT_STALL == p_task_system_dta->event) {
                    p_task_system_dta->state = ST_SYS_IDLE;
                    task_system_set_mode(FALLA);
                }
                p_task_system_dta->flag = false;
            }

            if (0 == p_task_system_dta->tick)
            {
                switch(fase_evasion)
                {
                    case PHASE_STOP:
                        fase_evasion = PHASE_REVERSE;
                        p_task_system_dta->tick = 500;
                        put_event_task_actuator(EV_ACT_MOTORS_REVERSE);
                        break;
                    case PHASE_REVERSE:
                        fase_evasion = PHASE_SPIN;
                        p_task_system_dta->tick = 800;
                        put_event_task_actuator(EV_ACT_MOTORS_SPIN);
                        break;
                    case PHASE_SPIN:
                        p_task_system_dta->state = ST_SYS_NORMAL_CLEANING;
                        put_event_task_display(0, 0, "LIMPIANDO...    ");
                        put_event_task_display(0, 1, "                ");
                        put_event_task_actuator(EV_ACT_MOTORS_FORWARD);
                        break;
                }
            }
            break;

        default: p_task_system_dta->state = ST_SYS_IDLE; break;
    }
    p_task_system_dta->flag = false;
}
