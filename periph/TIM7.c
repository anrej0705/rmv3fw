#include "TIM7.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "presets.h"
#include "GPIO.h"

void setup_key_poller(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	
	//Настраиваем таймер
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	m_tim.TIM_Prescaler = TIM7_PSC;
	m_tim.TIM_Period = TIM7_ARR;
	TIM_TimeBaseInit(TIM7, &m_tim);
	
	//Врубаем прерывания
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM7_IRQn);
}

inline void start_key_poller(void)
{
	TIM7->CR1 |= TIM_CR1_CEN;
}

inline void stop_key_poller(void)
{
	TIM7->CR1 &= ~TIM_CR1_CEN;
}

void TIM7_IRQHandler(void)
{
	//Сбрасываем флаг прерывания а то охуевание будет
	TIM7->SR &= ~TIM_SR_UIF;
	
	check_buttons();
	check_switchers();
}
