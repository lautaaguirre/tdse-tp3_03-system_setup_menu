/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 */

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"

#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"
#include "task_display_attribute.h"
#include "task_display_interface.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"

/********************** macros and definitions *******************************/
#define DEL_SYS_MIN			0ul
#define DEL_SYS_MED			250ul
#define DEL_SYS_MAX			500ul

/* Modes to excite Task System */
typedef enum task_system_mode {NORMAL, SETUP, MODE_QTY} task_system_mode_t;

#define SYSTEM_DTA_QTY	MODE_QTY

/********************** internal data declaration ****************************/
task_system_dta_t task_system_dta_list[SYSTEM_DTA_QTY];

/********************** internal functions declaration ***********************/
void task_system_normal_statechart(void);
void task_system_setup_statechart(void);

void task_system_set_mode(task_system_mode_t);

/********************** internal data definition *****************************/
const char *p_task_system 		= "Task System (System Statechart)";
const char *p_task_system_ 		= "Non-Blocking Code";
const char *p_task_system__ 	= "(Update by Time Code, period = 1mS)";

/********************** external data declaration ****************************/
task_system_mode_t g_task_system_mode;

/********************** external functions definition ************************/
void task_system_init(void *parameters)
{
	uint32_t index;
	task_system_dta_t 	*p_task_system_dta;
	task_system_st_t	state;
	task_system_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_system_init), HAL_GetTick());
	LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system_);
	LOGGER_INFO("   %s is a %s", GET_NAME(task_system), p_task_system__);

	init_event_task_system();

	task_system_set_mode(NORMAL);

	for (index = 0; SYSTEM_DTA_QTY > index; index++)
	{
		/* Update Task System Data Pointer */
		p_task_system_dta = &task_system_dta_list[index];

		/* Init & Print out: Task execution FSM */
		state = ST_SYS_IDLE;
		p_task_system_dta->state = state;

		event = EV_SYS_IDLE;
		p_task_system_dta->event = event;

		b_event = false;
		p_task_system_dta->flag = b_event;

		LOGGER_INFO(" ");
		LOGGER_INFO("   %s = %lu   %s = %lu   %s = %s",
					GET_NAME(state), (uint32_t)state,
					GET_NAME(event), (uint32_t)event,
					GET_NAME(b_event), (b_event ? "true" : "false"));
	}

	put_event_task_display(0, 0, "task_system_mode");
	put_event_task_display(0, 1, " NORMAL         ");

	task_system_set_mode(NORMAL);
}

void task_system_update(void *parameters)
{
	/* Run Task Statechart */
	switch (g_task_system_mode)
	{
		case NORMAL:

			task_system_normal_statechart();

			break;

		case SETUP:

			task_system_setup_statechart();

			break;

		default:

			task_system_set_mode(NORMAL);

			break;
		}
}

void task_system_normal_statechart(void)
{
	task_system_dta_t *p_task_system_dta;

	/* Update Task System Data Pointer */
	p_task_system_dta = &task_system_dta_list[NORMAL];

	if (true == any_event_task_system())
	{
		p_task_system_dta->flag = true;
		p_task_system_dta->event = get_event_task_system();
	}

	switch (p_task_system_dta->state)
	{
		case ST_SYS_IDLE:

			if ((true == p_task_system_dta->flag) && (EV_SYS_BTN_A == p_task_system_dta->event))
			{
				p_task_system_dta->flag = false;
				put_event_task_actuator(EV_LED_ACTIVE, ID_LED_A);
				p_task_system_dta->state = ST_SYS_ACTIVE;

				put_event_task_display(0, 0, "task_system_mode");
				put_event_task_display(0, 1, " SETUP          ");

				task_system_set_mode(SETUP);
			}

			break;

		case ST_SYS_ACTIVE:

			if ((true == p_task_system_dta->flag) && (EV_SYS_IDLE == p_task_system_dta->event))
			{
				p_task_system_dta->flag = false;
				put_event_task_actuator(EV_LED_IDLE, ID_LED_A);
				p_task_system_dta->state = ST_SYS_IDLE;
			}

			break;

		default:

			p_task_system_dta->tick  = DEL_SYS_MIN;
			p_task_system_dta->state = ST_SYS_IDLE;
			p_task_system_dta->event = EV_SYS_IDLE;
			p_task_system_dta->flag = false;

			break;
	}
}
void task_system_setup_statechart(void)
{
	task_system_dta_t *p_task_system_dta;

	/* Actualizamos el puntero de datos del sistema para el modo SETUP */
	p_task_system_dta = &task_system_dta_list[SETUP];

	/* Variables locales estáticas para almacenar la configuración del menú (Útil para el TF) */
	static uint32_t menu_motor = 1;      // 1: Motor A, 2: Motor B
	static uint32_t menu_param = 0;      // 0: Power, 1: Speed, 2: Spin
	static int32_t  val_power  = 50;     // Rango: 0 a 100 %
	static int32_t  val_speed  = 1500;   // Rango: 500 a 3000 RPM
	static uint32_t val_spin   = 0;      // 0: CW (Horario), 1: CCW (Antihorario)

	char lcd_buffer[17]; // Buffer local para dar formato a los textos del LCD

	/* Captura y desencolado de eventos desde la FIFO circular */
	if (true == any_event_task_system())
	{
		p_task_system_dta->flag = true;
		p_task_system_dta->event = get_event_task_system();
	}

	switch (p_task_system_dta->state)
	{
		case ST_SYS_IDLE:
			/* Estado de reseteo inicial: saltamos directo a la pantalla MAIN del menú */
			p_task_system_dta->state = ST_SYS_MAIN;
			put_event_task_display(0, 0, "SYSTEM SETUP    ");
			put_event_task_display(0, 1, "ENTER TO NAVIGATE");
			break;

		case ST_SYS_MAIN:
			if (true == p_task_system_dta->flag)
			{
				if (EV_SYS_ENTER == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false; // Consumo obligatorio del evento
					p_task_system_dta->state = ST_SYS_MENU_1; // Transición vertical al Nivel 1

					/* Dibujamos el estado inicial de Menu 1 */
					put_event_task_display(0, 0, "MENU 1: SELEC MOT");
					snprintf(lcd_buffer, sizeof(lcd_buffer), "> MOTOR %lu       ", menu_motor);
					put_event_task_display(0, 1, lcd_buffer);
				}
				else if (EV_SYS_ESCAPE == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					/* Salida del menú Setup hacia el modo NORMAL */
					put_event_task_display(0, 0, "task_system_mode");
					put_event_task_display(0, 1, " NORMAL         ");
					task_system_set_mode(NORMAL);
				}
			}
			break;

		case ST_SYS_MENU_1:
			/* Nivel 1: Selección de Motor (Navegación Vertical) */
			if (true == p_task_system_dta->flag)
			{
				if (EV_SYS_NEXT == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					/* Modificación de la variable local: alternamos Motor 1 y 2 */
					menu_motor = (menu_motor == 1) ? 2 : 1;

					snprintf(lcd_buffer, sizeof(lcd_buffer), "> MOTOR %lu       ", menu_motor);
					put_event_task_display(0, 1, lcd_buffer);
				}
				else if (EV_SYS_ENTER == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					p_task_system_dta->state = ST_SYS_MENU_2; // Transición vertical al Nivel 2

					/* Dibujamos el estado inicial de Menu 2 */
					put_event_task_display(0, 0, "MENU 2: PARAMETR");
					put_event_task_display(0, 1, "> POWER         ");
					menu_param = 0; // Por defecto arranca apuntando a Power
				}
				else if (EV_SYS_ESCAPE == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					p_task_system_dta->state = ST_SYS_MAIN; // Regresamos al nodo raíz

					put_event_task_display(0, 0, "SYSTEM SETUP    ");
					put_event_task_display(0, 1, "ENTER TO NAVIGATE");
				}
			}
			break;

		case ST_SYS_MENU_2:
			/* Nivel 2: Selección de Parámetro (Navegación Horizontal) */
			if (true == p_task_system_dta->flag)
			{
				if (EV_SYS_NEXT == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					/* Ciclamos horizontalmente entre los 3 parámetros disponibles */
					menu_param = (menu_param + 1) % 3;

					if (0 == menu_param) put_event_task_display(0, 1, "> POWER         ");
					else if (1 == menu_param) put_event_task_display(0, 1, "> SPEED         ");
					else if (2 == menu_param) put_event_task_display(0, 1, "> SPIN          ");
				}
				else if (EV_SYS_ENTER == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					p_task_system_dta->state = ST_SYS_MENU_3; // Entramos a la edición del parámetro (Nivel 3)

					put_event_task_display(0, 0, "MENU 3: EDIT VAL");
					if (0 == menu_param) {
						snprintf(lcd_buffer, sizeof(lcd_buffer), "POWER: %ld %%    ", val_power);
					} else if (1 == menu_param) {
						snprintf(lcd_buffer, sizeof(lcd_buffer), "SPEED: %ld RPM  ", val_speed);
					} else {
						snprintf(lcd_buffer, sizeof(lcd_buffer), "SPIN: %s         ", (val_spin == 0) ? "CW" : "CCW");
					}
					put_event_task_display(0, 1, lcd_buffer);
				}
				else if (EV_SYS_ESCAPE == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					p_task_system_dta->state = ST_SYS_MENU_1; // Regresamos a la selección de motor

					put_event_task_display(0, 0, "MENU 1: SELEC MOT");
					snprintf(lcd_buffer, sizeof(lcd_buffer), "> MOTOR %lu       ", menu_motor);
					put_event_task_display(0, 1, lcd_buffer);
				}
			}
			break;

		case ST_SYS_MENU_3:
			/* Nivel 3: Modificación y Guardado de Variables */
			if (true == p_task_system_dta->flag)
			{
				if (EV_SYS_NEXT == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					/* Modificamos el valor según el parámetro activo */
					if (0 == menu_param) {
						val_power = (val_power >= 100) ? 0 : val_power + 10;
						snprintf(lcd_buffer, sizeof(lcd_buffer), "POWER: %ld %%    ", val_power);
					} else if (1 == menu_param) {
						val_speed = (val_speed >= 3000) ? 500 : val_speed + 500;
						snprintf(lcd_buffer, sizeof(lcd_buffer), "SPEED: %ld RPM  ", val_speed);
					} else {
						val_spin = !val_spin;
						snprintf(lcd_buffer, sizeof(lcd_buffer), "SPIN: %s         ", (val_spin == 0) ? "CW" : "CCW");
					}
					put_event_task_display(0, 1, lcd_buffer);
				}
				else if (EV_SYS_ENTER == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					/* ACCIÓN DE GUARDADO: Aquí persistirías las variables en la memoria estructural del TF */
					p_task_system_dta->state = ST_SYS_MENU_2; // Retornamos guardando cambios

					put_event_task_display(0, 0, "MENU 2: PARAMETR");
					if (0 == menu_param) put_event_task_display(0, 1, "> POWER (SAVED) ");
					else if (1 == menu_param) put_event_task_display(0, 1, "> SPEED (SAVED) ");
					else put_event_task_display(0, 1, "> SPIN (SAVED)  ");
				}
				else if (EV_SYS_ESCAPE == p_task_system_dta->event)
				{
					p_task_system_dta->flag = false;
					/* Cancelación del cambio: volvemos al nivel 2 sin alterar la lógica */
					p_task_system_dta->state = ST_SYS_MENU_2;

					put_event_task_display(0, 0, "MENU 2: PARAMETR");
					if (0 == menu_param) put_event_task_display(0, 1, "> POWER         ");
					else if (1 == menu_param) put_event_task_display(0, 1, "> SPEED         ");
					else put_event_task_display(0, 1, "> SPIN          ");
				}
			}
			break;

		default:
			/* Control de fallos del puntero de estados */
			p_task_system_dta->state = ST_SYS_IDLE;
			p_task_system_dta->flag = false;
			break;
	}

	/* Bajada incondicional externa de seguridad para limpiar eventos residuales */
	p_task_system_dta->flag = false;
}

void task_system_set_mode(task_system_mode_t task_system_mode)
{
	g_task_system_mode = task_system_mode;
}

/********************** end of file ******************************************/
