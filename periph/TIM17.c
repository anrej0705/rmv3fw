#include "TIM17.h"
#include "stm32f10x_tim.h"
#include "presets.h"

//Настройка таймера
void setup_feed_coil(void)
{
	//Тактируем из шины APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
	
	//Настраиваем таймер
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//Кручу верчу настроить таймер хочу
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	m_tim.TIM_Prescaler = TIM17_PSC;
	m_tim.TIM_Period = TIM17_PWM_MAX;
	TIM_TimeBaseInit(TIM17, &m_tim);
	
	//Настраивае ШИМ где надо
	TIM_OCInitTypeDef m_pwm;
	TIM_OCStructInit(&m_pwm);
	
	//Эти настройки будут везде
	m_pwm.TIM_OCMode = TIM_OCMode_PWM1;
	m_pwm.TIM_OutputState = TIM_OutputState_Enable;
	m_pwm.TIM_Pulse = DRIVER_DEFAULT_PWM;	//50%
	
	TIM_OC1Init(TIM17, &m_pwm);
}

inline void start_feed_coil(void)
{	//ВРУБАЕМ ТАЙМЕР И ШЫМ ЕБАНЫЙ
	TIM17->CR1 |= TIM_CR1_CEN;
	TIM17->BDTR |= TIM_BDTR_MOE;
}

inline void stop_feed_coil(void)
{	//ВЫРУБАЕМ И ТАЙМЕР И ЕГО ШИМ
	TIM17->CR1 &= ~TIM_CR1_CEN;
	TIM17->BDTR &= ~TIM_BDTR_MOE;
}

inline void set_speed_feed_coil(uint16_t speed)
{	//Больше - меньше
	TIM17->ARR = speed;
	TIM17->CCR1 = speed/2;	//50% ШИМ
}