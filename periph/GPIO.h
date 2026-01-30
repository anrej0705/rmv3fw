#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f10x_gpio.h"
#include "stdbool.h"

#define	PA_SERVICE_MENU_BUTTON			GPIO_Pin_0
#define PA_FEED_COIL_DIRECTION			GPIO_Pin_8
#define PA_TAKE_COIL_DIRECTION			GPIO_Pin_11

#define PB_CALLBACK_DISABLE					GPIO_Pin_2
#define PB_COILS_ENABLE							GPIO_Pin_5
#define PB_TTM_ENABLE								GPIO_Pin_6
#define PB_ALARM_LED								GPIO_Pin_10
#define PB_TAKE_COIL_LED						GPIO_Pin_11
#define PB_FRAME_CHANGE_LED					GPIO_Pin_12
#define PB_FEED_COIL_LED						GPIO_Pin_13
#define PB_TTM_DIRECTION						GPIO_Pin_15

#define PC_CALLBACK_TRIGGER					GPIO_Pin_4
#define PC_DROP_DETECT							GPIO_Pin_10
#define PC_SUBLIGHT_EN_LED					GPIO_Pin_11
#define PC_CAMERA_SHOT							GPIO_Pin_12

#define PD_MOTOR_MAIN_SWITCH				GPIO_Pin_2

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern bool pa_service_menu_button_lock;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup_gpio(void);

void check_buttons(void);
void check_switchers(void);
void check_keyboard(void);

#endif
