#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f10x_gpio.h"
#include "stdbool.h"

#define	PA_SERVICE_MENU_BUTTON			GPIO_Pin_0
#define PA_FEED_COIL_DIRECTION			GPIO_Pin_8
#define PA_TAKE_COIL_DIRECTION			GPIO_Pin_11

#define PB_COIL_ENABLE							GPIO_Pin_5
#define PB_TTM_ENABLE								GPIO_Pin_6
#define PB_TTM_DIRECTION						GPIO_Pin_15

#define PD_MOTOR_MAIN_SWITCH				GPIO_Pin_2

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern bool pa_service_menu_button_lock;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup_gpio(void);

void check_buttons(void);
void check_switchers(void);
//void check_keyboard(void);

#endif
