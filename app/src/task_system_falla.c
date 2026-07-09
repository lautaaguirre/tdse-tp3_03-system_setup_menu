#include "task_system_falla.h"
#include "task_system_interface.h"
#include "task_actuator_interface.h"
#include "task_display_interface.h"
#include "task_actuator_attribute.h"

void task_system_falla_statechart(task_system_dta_t *p_task_system_dta)
{
    if (true == any_event_task_system())
    {
        p_task_system_dta->flag = true;
        p_task_system_dta->event = get_event_task_system();
    }

    switch (p_task_system_dta->state)
    {
        case ST_SYS_IDLE:
            /* Entrada inicial al modo Falla */
            p_task_system_dta->state = ST_SYS_FALLA_MAIN;

            /* Frenado incondicional de los motores por seguridad física */
            put_event_task_actuator(EV_ACT_MOTORS_STOP);

            /* Alarma Visual al usuario */
            put_event_task_display(0, 0, "ERR: SOBRECARGA ");
            put_event_task_display(0, 1, "ENTER -> REANUD ");
            break;

        case ST_SYS_FALLA_MAIN:
            /* El sistema está enclavado (LATCHED). Solo salimos por intervención humana. */
            if (true == p_task_system_dta->flag)
            {
                if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    /* El usuario reconoce (ACK) el error. Volvemos al estado inicial del sistema */
                    put_event_task_display(0, 0, "MODO NORMAL     ");
                    put_event_task_display(0, 1, "ENTER: EMPEZAR  ");

                    /* Limpiamos nuestro propio estado para la próxima vez que falle */
                    p_task_system_dta->state = ST_SYS_IDLE;

                    /* Transferimos control de vuelta a Normal (en reposo) */
                    task_system_set_mode(NORMAL);
                }
            }
            break;

        default:
            p_task_system_dta->state = ST_SYS_IDLE;
            break;
    }

    /* Limpieza Incondicional de eventos residuales (pueden llegar rebotes espurios) */
    p_task_system_dta->flag = false;
}
