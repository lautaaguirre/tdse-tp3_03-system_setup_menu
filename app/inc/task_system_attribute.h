/*
 * Copyright (c) 2026 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * (Licencia omitida por brevedad, pero mantenla en tu archivo original)
 */

#ifndef TASK_SYSTEM_ATTRIBUTE_H_
#define TASK_SYSTEM_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Events to excite Task System */
typedef enum task_system_ev {
    EV_SYS_IDLE,
    EV_SYS_BTN_A,
    EV_SYS_ENTER,
    EV_SYS_NEXT,
    EV_SYS_ESCAPE
} task_system_ev_t;

/* State of Task System */
typedef enum task_system_st {
    ST_SYS_IDLE,
    ST_SYS_ACTIVE,   /* <--- FALTABA ESTE ESTADO PARA EL MODO NORMAL */
    ST_SYS_MAIN,     /* Nivel raíz del setup */
    ST_SYS_MENU_1,   /* Seleccionar Motor */
    ST_SYS_MENU_2,   /* Seleccionar Parámetro */
    ST_SYS_MENU_3    /* Editar Valor */
} task_system_st_t;

typedef struct
{
	uint32_t			tick;
	task_system_st_t	state;
	task_system_ev_t	event;
	bool				flag;
} task_system_dta_t;

/********************** external data declaration ****************************/
extern task_system_dta_t task_system_dta_list[];

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
