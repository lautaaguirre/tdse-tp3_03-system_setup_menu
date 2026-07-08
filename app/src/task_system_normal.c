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
                    put_event_task_actuator(EV_ACT_MOTORS_FORWARD, 0);
                }
                else if (EV_SYS_ESCAPE == p_task_system_dta->event)
                {
                    task_system_set_mode(SETUP);
                }
            }
            break;

        case ST_SYS_NORMAL_CLEANING:
            if (true == p_task_system_dta->flag)
            {
                // CONDICIÓN DE ABORTO CRÍTICO (Prioridad 1)
                if (EV_SYS_FAULT_STALL == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_IDLE; // Reseteamos nuestra máquina interna
                    task_system_set_mode(FALLA);            // Expulsamos el control al enrutador
                }
                // CONDICIÓN REACTIVA NORMAL (Prioridad 2)
                else if (EV_SYS_SENSOR_OBSTACLE == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_NORMAL_AVOIDING;
                    fase_evasion = PHASE_STOP;
                    p_task_system_dta->tick = 200;

                    put_event_task_display(0, 0, "OBSTACULO!      ");
                    put_event_task_display(0, 1, "EVADIENDO...    ");
                    put_event_task_actuator(EV_ACT_MOTORS_STOP, 0);
                }
                else if (EV_SYS_ESCAPE == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_IDLE;
                    put_event_task_actuator(EV_ACT_MOTORS_STOP, 0);
                    put_event_task_display(0, 0, "MODO NORMAL     ");
                    put_event_task_display(0, 1, "ENTER: EMPEZAR  ");
                }
            }
            break;

        case ST_SYS_NORMAL_AVOIDING:
            if (true == p_task_system_dta->flag) {
                // Aún evadiendo, si hay sobreconsumo en la marcha atrás o giro, ABORTAMOS.
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
                        put_event_task_actuator(EV_ACT_MOTORS_REVERSE, 0);
                        break;

                    case PHASE_REVERSE:
                        fase_evasion = PHASE_SPIN;
                        p_task_system_dta->tick = 800;
                        put_event_task_actuator(EV_ACT_MOTORS_SPIN, 0);
                        break;

                    case PHASE_SPIN:
                        p_task_system_dta->state = ST_SYS_NORMAL_CLEANING;
                        put_event_task_display(0, 0, "LIMPIANDO...    ");
                        put_event_task_display(0, 1, "                ");
                        put_event_task_actuator(EV_ACT_MOTORS_FORWARD, 0);
                        break;
                }
            }
            break;

        default:
            p_task_system_dta->state = ST_SYS_IDLE;
            break;
    }

    p_task_system_dta->flag = false;
}
