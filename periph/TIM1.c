#include "TIM1.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "presets.h"
#include "global_vars.h"

#include "GPIO.h"
#include "stdbool.h"

//Есть такая тема - в контроллере баг, когда настраиваешь таймер при запуске он сразу же даёт 
//прерывание которое никто не ждал. Чтобы такой лабуды не было нужно первый вызов прерывания скипать
bool tim1_irg_bugfix = 0;

void setup_ttm_controller()
{
	//Да начнётся ёбка
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//Настраиваем таймер
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//Тырим из main.c
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	m_tim.TIM_Prescaler = 0;									//Без делителя, считаем всем импульсы
	m_tim.TIM_Period = TTM_PULSES_THRESHOLD;
	TIM_TimeBaseInit(TIM1, &m_tim);
	
	//Здесь настраиваем таймер чтобы он тактировался снаружи
	TIM_ETRClockMode2Config(TIM1, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x00);
	
	//Врубаем прерывания
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
}

inline void start_ttm_controller()
{
	TIM1->CR1 |= TIM_CR1_CEN;
}

inline void stop_ttm_controller()
{
	TIM1->CR1 &= ~TIM_CR1_CEN;
}

void TIM1_UP_TIM16_IRQHandler()
{
	//Сбрасываем флаг прерывания а то охуевание будет
	TIM1->SR &= ~TIM_SR_UIF;
	
	//Зануляем счётчик, готовим к новой партии импульсов
	TIM1->CNT = 0;
	
	if(!tim1_irg_bugfix)
	{	//Фиксим ложное срабатывание прерывания
		tim1_irg_bugfix = 1;
		return;
	}
	
	green_led_frame_change = !green_led_frame_change;
	
	if(green_led_frame_change)
	{
		//GPIOB->ODR |= PB_FRAME_CHANGE_LED;
		GPIOB->ODR |= PB_TTM_ENABLE;
	}
	else
	{
		//GPIOB->ODR &= ~PB_FRAME_CHANGE_LED;
		GPIOB->ODR &= ~PB_TTM_ENABLE;
	}
}