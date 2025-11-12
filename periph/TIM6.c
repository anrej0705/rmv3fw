#include "TIM6.h"
#include "stm32f10x_tim.h"
#include "presets.h"

void setup_delay_ms()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//TIM6 - генерация миллисекундной задержки
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	m_tim.TIM_Prescaler = TIM6_PSC;
	TIM_TimeBaseInit(TIM6, &m_tim);
	
	//Врубаем
	TIM6->CR1 |= TIM_CR1_CEN;
}

//Миллисекундная задержка - спамить не стоит так как виснет весь контроллер
void delay_ms(uint16_t ms)
{
	TIM6->CNT = 0;																												//Пишем 0 в счётчик
	TIM_Cmd(TIM6, ENABLE);																								//Врубаем таймер
	while(ms > TIM6->CNT);																								//Курим бамбук
	TIM_Cmd(TIM6, DISABLE);																								//Вырубаем таймер и сваливаем
}
