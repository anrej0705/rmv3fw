#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "GPIO.h"
#include "TIM17.h"
#include "TIM16.h"
#include "TIM15.h"

#include "global_vars.h"

bool pa_service_menu_button_lock = 0;
bool pb_main_switch_lock = 0;

void setup_gpio(void)
{	
	//Теперь самый сок - порты блять
	
	//Дрочим всё до чего дотянемся - ну тут на самом деле немного
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitTypeDef m_gpio;
	GPIO_StructInit(&m_gpio);
	
	//Эти параметры будут везде
	m_gpio.GPIO_Speed = GPIO_Speed_2MHz;
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Кнопки
	m_gpio.GPIO_Mode = GPIO_Mode_IPD;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &m_gpio);
	
	//GPIOC
	m_gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOC, &m_gpio);
	
	//GPIOD
	m_gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &m_gpio);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настройка пинов которые будут обычными пинами
	m_gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &m_gpio);
	
	//GPIOC
	m_gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &m_gpio);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настройка АНАЛоговых входов
	m_gpio.GPIO_Mode = GPIO_Mode_AIN;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &m_gpio);
	
	//GPIOC
	m_gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &m_gpio);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настройка блатных чертей
	m_gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &m_gpio);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

inline void check_buttons()
{
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Проверяем нажатие кнопки сервисного меню
	if((GPIOA->IDR & PA_SERVICE_MENU_BUTTON) == 1 && !pa_service_menu_button_lock)
	{
		pa_service_menu_button_lock = 1;
		
		GPIOA->ODR |= PA_FEED_COIL_DIRECTION;
		GPIOA->ODR &= ~PA_TAKE_COIL_DIRECTION;
		GPIOB->ODR |= PB_TTM_DIRECTION;
		
		//Пока что переключаем направление вращения двигателей
	}
	else if((GPIOA->IDR & PA_SERVICE_MENU_BUTTON) == 0 && pa_service_menu_button_lock)
	{
		pa_service_menu_button_lock = 0;
		
		GPIOA->ODR &= ~PA_FEED_COIL_DIRECTION;
		GPIOA->ODR |= PA_TAKE_COIL_DIRECTION;
		GPIOB->ODR &= ~PB_TTM_DIRECTION;
		
		//Пока что переключаем направление вращения двигателей
	}
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

inline void check_switchers()
{
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if((GPIOD->IDR & PD_MOTOR_MAIN_SWITCH) >> 2 == 1 && pb_main_switch_lock)
	{	//Лог.1 - выкл
		pb_main_switch_lock = 0;
		
		ttm_engine_enable = TTM_ENGINE_DISABLE;
		coils_engine_enable = COILS_ENGINE_DISABLE;
		
		//Выключаем пропеллеры чтобы не шумели, всё равно охлаждать нечего
		engine_cooler_enable = ENGINE_COOLER_DISABLE;
		
		//Вырубаем таймеры
		stop_ttm();
		stop_feed_coil();
		stop_take_coil();
	}
	else if ((GPIOD->IDR & PD_MOTOR_MAIN_SWITCH) >> 2 == 0 && !pb_main_switch_lock)
	{	//Лог.0 - вкл
		pb_main_switch_lock = 1;
		
		ttm_engine_enable = TTM_ENGINE_ENABLE;
		coils_engine_enable = COILS_ENGINE_ENABLE;
		
		//Врубаем пропеллеры, а то всё перегреется к хуям)))
		engine_cooler_enable = ENGINE_COOLER_ENABLE;
		
		//Врубаем таймеры
		start_ttm();
		
		if(!feed_coil_lock)
		{
			start_feed_coil();
		}
		if(!take_coil_lock)
		{
			start_take_coil();
		}
	}
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
