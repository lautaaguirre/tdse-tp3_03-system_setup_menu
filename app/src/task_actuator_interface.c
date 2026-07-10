/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 */

/********************** inclusions *******************************************/
#include "main.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"
#include "app.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"

/* Declaramos externamente el arreglo para que el Linker sepa que la memoria RAM
 * fue reservada en task_actuator.c */
extern task_actuator_dta_t task_actuator_dta_list[];

/********************** external functions definition ************************/

void put_event_task_actuator(task_actuator_ev_t event)
{
    /* PATRÓN BROADCAST (Senior Design):
     * Recibimos un único evento global desde el cerebro (Ej: FORWARD, STOP).
     * Lo replicamos incondicionalmente a los 4 FSMs de nuestros motores.
     * La tabla constante de la Flash se encargará de traducir este evento
     * en la energía correcta para cada canal PWM.
     */

    // 1. Rueda Izquierda Adelante
    task_actuator_dta_list[ID_MOT_L_FWD].event = event;
    task_actuator_dta_list[ID_MOT_L_FWD].flag  = true;

    // 2. Rueda Izquierda Atrás
    task_actuator_dta_list[ID_MOT_L_REV].event = event;
    task_actuator_dta_list[ID_MOT_L_REV].flag  = true;

    // 3. Rueda Derecha Adelante
    task_actuator_dta_list[ID_MOT_R_FWD].event = event;
    task_actuator_dta_list[ID_MOT_R_FWD].flag  = true;

    // 4. Rueda Derecha Atrás
    task_actuator_dta_list[ID_MOT_R_REV].event = event;
    task_actuator_dta_list[ID_MOT_R_REV].flag  = true;
}

/********************** end of file ******************************************/
