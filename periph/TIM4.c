#include "TIM4.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "presets.h"
#include "stdbool.h"
#include "GPIO.h"
#include "global_vars.h"

//Есть такая тема - в контроллере баг, когда настраиваешь таймер при запуске он сразу же даёт 
//прерывание которое никто не ждал. Чтобы такой лабуды не было нужно первый вызов прерывания скипать
bool tim4_irg_bugfix = 0;

void setup_led_screen_update(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//Настраиваем таймер
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//TIM5 - управление светодиодами и экраном BA63
	m_tim.TIM_Prescaler = TIM4_PSC;
	m_tim.TIM_Period = TIM4_ARR;
	TIM_TimeBaseInit(TIM4, &m_tim);
	
	//Врубаем прерывания
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM4_IRQn);
}

inline void start_led_screen_update(void)
{
	TIM4->CR1 |= TIM_CR1_CEN;
}

inline void stop_led_screen_update(void)
{
	TIM4->CR1 &= ~TIM_CR1_CEN;
}

void TIM4_IRQHandler()
{
	//Сбрасываем флаг прерывания а то охуевание будет
	TIM4->SR &= ~TIM_SR_UIF;
	
	//Зануляем счётчик, готовим к новой партии импульсов
	TIM4->CNT = 0;
	
	if(!tim4_irg_bugfix)
	{	//Фиксим ложное срабатывание прерывания
		tim4_irg_bugfix = 1;
		return;
	}
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Зажигаем или гасим светодиод смены кадра
	green_led_frame_change == 0 ? (GPIOB->ODR |= PB_FRAME_CHANGE_LED) : (GPIOB->ODR &= ~PB_FRAME_CHANGE_LED);
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
