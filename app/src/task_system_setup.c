#include <stdio.h>
#include "task_system_setup.h"
#include "task_system_interface.h"
#include "task_display_interface.h"

void task_system_setup_statechart(task_system_dta_t *p_task_system_dta)
{
    static uint32_t menu_param = 0;
    static uint8_t  val_modo   = 0;
    static uint8_t  val_potencia = 0;
    static uint8_t  val_tiempo = 10;

    char lcd_buffer[17];

    if (true == any_event_task_system())
    {
        p_task_system_dta->flag = true;
        p_task_system_dta->event = get_event_task_system();
    }

    switch (p_task_system_dta->state)
    {
        case ST_SYS_IDLE:
            p_task_system_dta->state = ST_SYS_MAIN;
            /* La primera vez que entra, siempre dibuja esto */
            put_event_task_display(0, 0, "ASPIRADORA SETUP");
            put_event_task_display(0, 1, "ENTER TO NAVIG. ");
            break;

        case ST_SYS_MAIN:
            if (true == p_task_system_dta->flag)
            {
                if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MENU_1;
                    put_event_task_display(0, 0, "MENU 1: PARAM   ");
                    put_event_task_display(0, 1, "> MODO          ");
                    menu_param = 0;
                }
                /* MODIFICACIÓN: Agregamos el botón SET como opción de salida junto al ESCAPE */
                else if (EV_SYS_ESCAPE == p_task_system_dta->event || EV_SYS_SETUP_BTN == p_task_system_dta->event)
                {
                    put_event_task_display(0, 0, "MODO NORMAL     ");
                    put_event_task_display(0, 1, "ENTER: EMPEZAR  ");

                    /* RESET ON EXIT: Aseguramos que arranque bien la próxima */
                    p_task_system_dta->state = ST_SYS_IDLE;
                    task_system_set_mode(NORMAL);
                }
            }
            break;

        case ST_SYS_MENU_1:
            if (true == p_task_system_dta->flag)
            {
                /* MODIFICACIÓN: Aborto desde el Menú 1 con SET */
                if (EV_SYS_SETUP_BTN == p_task_system_dta->event)
                {
                    put_event_task_display(0, 0, "MODO NORMAL     ");
                    put_event_task_display(0, 1, "ENTER: EMPEZAR  ");
                    p_task_system_dta->state = ST_SYS_IDLE;
                    task_system_set_mode(NORMAL);
                }
                else if (EV_SYS_NEXT == p_task_system_dta->event)
                {
                    menu_param = (menu_param + 1) % 3;
                    if (0 == menu_param) put_event_task_display(0, 1, "> MODO          ");
                    else if (1 == menu_param) put_event_task_display(0, 1, "> POTENCIA      ");
                    else put_event_task_display(0, 1, "> TIEMPO        ");
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MENU_2;
                    put_event_task_display(0, 0, "MENU 2: EDITAR  ");

                    if (0 == menu_param) snprintf(lcd_buffer, sizeof(lcd_buffer), "VAL: %s", (val_modo == 0) ? "MANUAL" : "AUTO");
                    else if (1 == menu_param) snprintf(lcd_buffer, sizeof(lcd_buffer), "VAL: %s", (val_potencia == 0) ? "BAJA" : (val_potencia == 1) ? "MEDIA" : "ALTA");
                    else snprintf(lcd_buffer, sizeof(lcd_buffer), "VAL: %d MIN", val_tiempo);

                    put_event_task_display(0, 1, lcd_buffer);
                }
                else if (EV_SYS_ESCAPE == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MAIN;
                    put_event_task_display(0, 0, "ASPIRADORA SETUP");
                    put_event_task_display(0, 1, "ENTER TO NAVIG. ");
                }
            }
            break;

        case ST_SYS_MENU_2:
            if (true == p_task_system_dta->flag)
            {
                /* MODIFICACIÓN: Aborto desde el Menú 2 con SET */
                if (EV_SYS_SETUP_BTN == p_task_system_dta->event)
                {
                    put_event_task_display(0, 0, "MODO NORMAL     ");
                    put_event_task_display(0, 1, "ENTER: EMPEZAR  ");
                    p_task_system_dta->state = ST_SYS_IDLE;
                    task_system_set_mode(NORMAL);
                }
                else if (EV_SYS_NEXT == p_task_system_dta->event)
                {
                    if (0 == menu_param){
                        val_modo = !val_modo;
                        snprintf(lcd_buffer, sizeof(lcd_buffer), "VAL: %s", (val_modo == 0) ? "MANUAL" : "AUTO");
                    }
                    else if (1 == menu_param){
                        val_potencia = (val_potencia >= 2) ? 0 : val_potencia + 1;
                        snprintf(lcd_buffer, sizeof(lcd_buffer), "VAL: %s", (val_potencia == 0) ? "BAJA" : (val_potencia == 1) ? "MEDIA" : "ALTA");
                    }
                    else{
                        val_tiempo = (val_tiempo >= 60) ? 10 : val_tiempo + 5;
                        snprintf(lcd_buffer, sizeof(lcd_buffer), "VAL: %d MIN", val_tiempo);
                    }
                    put_event_task_display(0, 1, lcd_buffer);
                }
                else if (EV_SYS_ENTER == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MENU_1;
                    put_event_task_display(0, 0, "MENU 1: PARAM   ");
                    if (0 == menu_param) put_event_task_display(0, 1, "> MODO (SAVED)  ");
                    else if (1 == menu_param) put_event_task_display(0, 1, "> POT  (SAVED)  ");
                    else put_event_task_display(0, 1, "> TIME (SAVED)  ");
                }
                else if (EV_SYS_ESCAPE == p_task_system_dta->event)
                {
                    p_task_system_dta->state = ST_SYS_MENU_1;
                    put_event_task_display(0, 0, "MENU 1: PARAM   ");
                    if (0 == menu_param) put_event_task_display(0, 1, "> MODO          ");
                    else if (1 == menu_param) put_event_task_display(0, 1, "> POTENCIA      ");
                    else put_event_task_display(0, 1, "> TIEMPO        ");
                }
            }
            break;

        default: p_task_system_dta->state = ST_SYS_IDLE; break;
    }
    p_task_system_dta->flag = false;
}
