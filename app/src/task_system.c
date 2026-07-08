#include "main.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "task_system_interface.h"
#include "task_system_normal.h"
#include "task_system_setup.h"
#include "task_system_falla.h"   // <--- Incluimos la nueva cabecera
#include "task_display_interface.h"

#define SYSTEM_DTA_QTY MODE_QTY

static task_system_dta_t task_system_dta_list[SYSTEM_DTA_QTY];
static task_system_mode_t g_task_system_mode;

void task_system_init(void *parameters)
{
    uint32_t index;

    LOGGER_INFO("  task_system is running - Tick [mS] = %lu", HAL_GetTick());

    init_event_task_system();

    for (index = 0; index < SYSTEM_DTA_QTY; index++)
    {
        task_system_dta_list[index].state = ST_SYS_IDLE;
        task_system_dta_list[index].event = EV_SYS_IDLE;
        task_system_dta_list[index].flag  = false;
        task_system_dta_list[index].tick  = 0;
    }

    /* LECTURA DEL DIP SWITCH (HARDWARE BOOTSTRAPPING)
         * Asumimos que pusiste el User Label "DIP_SW" en CubeMX.
         * Si está en 1 (SET), arranca directo en modo SETUP. Si no, a NORMAL. */
    if (HAL_GPIO_ReadPin(DIP_SW_GPIO_Port, DIP_SW_Pin) == GPIO_PIN_SET)
    {
    	task_system_set_mode(SETUP);
    }
    else
    {
    	put_event_task_display(0, 0, "MODO NORMAL     ");
    	put_event_task_display(0, 1, "ENTER: EMPEZAR  ");
    	task_system_set_mode(NORMAL);
    }
}

void task_system_update(void *parameters)
{
    switch (g_task_system_mode)
    {
        case NORMAL:
            task_system_normal_statechart(&task_system_dta_list[NORMAL]);
            break;

        case SETUP:
            task_system_setup_statechart(&task_system_dta_list[SETUP]);
            break;

        case FALLA:
            task_system_falla_statechart(&task_system_dta_list[FALLA]);
            break;

        default:
            task_system_set_mode(NORMAL);
            break;
    }
}

void task_system_set_mode(task_system_mode_t mode)
{
    g_task_system_mode = mode;
}
