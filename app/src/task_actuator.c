/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 */

/********************** inclusions *******************************************/
#include "main.h"
#include "logger.h"
#include "task_actuator_attribute.h"
#include "task_actuator_interface.h"

/********************** macros and definitions *******************************/
#define ACTUATOR_CFG_QTY    (sizeof(task_actuator_cfg_list)/sizeof(task_actuator_cfg_t))
#define ACTUATOR_DTA_QTY    ACTUATOR_CFG_QTY

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

/********************** internal data declaration ****************************/
const task_actuator_cfg_t task_actuator_cfg_list[] = {
    {ID_MOT_L_FWD,   &htim3,  TIM_CHANNEL_1, 800,   0,  500,    0},
    {ID_MOT_L_REV,   &htim3,  TIM_CHANNEL_2,   0, 500,    0,    0},
    {ID_MOT_R_FWD,   &htim4,  TIM_CHANNEL_1, 800,   0,    0,    0},
    {ID_MOT_R_REV,   &htim4,  TIM_CHANNEL_2,   0, 500,  500,    0}
};

task_actuator_dta_t task_actuator_dta_list[ACTUATOR_DTA_QTY];

/********************** internal functions declaration ***********************/
void task_actuator_statechart(uint32_t index);

/********************** internal data definition *****************************/
const char *p_task_actuator        = "Task Actuator (Classic Statechart)";
const char *p_task_actuator_       = "Non-Blocking Code";
const char *p_task_actuator__      = "(Update by Time Code, period = 1mS)";

/********************** external functions definition ************************/
void task_actuator_init(void *parameters)
{
    uint32_t index;

    LOGGER_INFO(" ");
    LOGGER_INFO("  %s is running - Tick [mS] = %lu", GET_NAME(task_actuator_init), HAL_GetTick());

    for (index = 0; ACTUATOR_DTA_QTY > index; index++)
    {
        task_actuator_dta_list[index].state = ST_ACT_STOPPED;
        task_actuator_dta_list[index].event = EV_ACT_IDLE;
        task_actuator_dta_list[index].flag  = false;
        task_actuator_dta_list[index].tick  = 0;

        HAL_TIM_PWM_Start(task_actuator_cfg_list[index].htim, task_actuator_cfg_list[index].channel);
        __HAL_TIM_SET_COMPARE(task_actuator_cfg_list[index].htim, task_actuator_cfg_list[index].channel, 0);
    }
}

void put_event_task_actuator(task_actuator_ev_t event)
{
    uint32_t index;
    for (index = 0; ACTUATOR_DTA_QTY > index; index++)
    {
        task_actuator_dta_list[index].event = event;
        task_actuator_dta_list[index].flag  = true;
    }
}

void task_actuator_update(void *parameters)
{
    uint32_t index;
    for (index = 0; ACTUATOR_DTA_QTY > index; index++)
    {
        task_actuator_statechart(index);
    }
}

void task_actuator_statechart(uint32_t index)
{
    const task_actuator_cfg_t *p_cfg = &task_actuator_cfg_list[index];
    task_actuator_dta_t       *p_dta = &task_actuator_dta_list[index];

    switch (p_dta->state)
    {
        case ST_ACT_STOPPED:
            if (true == p_dta->flag)
            {
                if (EV_ACT_MOTORS_FORWARD == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_fwd);
                    p_dta->state = ST_ACT_MOVING_FWD;
                }
                else if (EV_ACT_MOTORS_REVERSE == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_rev);
                    p_dta->state = ST_ACT_MOVING_REV;
                }
                else if (EV_ACT_MOTORS_SPIN == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_spin);
                    p_dta->state = ST_ACT_SPINNING;
                }
            }
            break;

        case ST_ACT_MOVING_FWD:
            if (true == p_dta->flag)
            {
                if (EV_ACT_MOTORS_STOP == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_stop);
                    p_dta->state = ST_ACT_STOPPED;
                }
                else if (EV_ACT_MOTORS_REVERSE == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_rev);
                    p_dta->state = ST_ACT_MOVING_REV;
                }
                else if (EV_ACT_MOTORS_SPIN == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_spin);
                    p_dta->state = ST_ACT_SPINNING;
                }
            }
            break;

        case ST_ACT_MOVING_REV:
            if (true == p_dta->flag)
            {
                if (EV_ACT_MOTORS_STOP == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_stop);
                    p_dta->state = ST_ACT_STOPPED;
                }
                else if (EV_ACT_MOTORS_FORWARD == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_fwd);
                    p_dta->state = ST_ACT_MOVING_FWD;
                }
                else if (EV_ACT_MOTORS_SPIN == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_spin);
                    p_dta->state = ST_ACT_SPINNING;
                }
            }
            break;

        case ST_ACT_SPINNING:
            if (true == p_dta->flag)
            {
                if (EV_ACT_MOTORS_STOP == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_stop);
                    p_dta->state = ST_ACT_STOPPED;
                }
                else if (EV_ACT_MOTORS_FORWARD == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_fwd);
                    p_dta->state = ST_ACT_MOVING_FWD;
                }
                else if (EV_ACT_MOTORS_REVERSE == p_dta->event) {
                    __HAL_TIM_SET_COMPARE(p_cfg->htim, p_cfg->channel, p_cfg->pwm_rev);
                    p_dta->state = ST_ACT_MOVING_REV;
                }
            }
            break;

        default:
            p_dta->state = ST_ACT_STOPPED;
            break;
    }

    /* Limpieza estricta de la bandera al final del ciclo */
    p_dta->flag = false;
}
