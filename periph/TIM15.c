#include "TIM15.h"
#include "stm32f10x_tim.h"
#include "presets.h"

//Настройка таймера
void setup_ttm(void)
{
	//Врубаем продрачивание таймера
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	
	//Токмо для таймера 15 ибо заибал сцука)))))))
	//Переназначем ножку генерации ШЫМки так как старая занята генерацией ШИМ подсветки к,з,с,ж
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
	AFIO->MAPR2 |= AFIO_MAPR2_TIM15_REMAP;
	
	//Настраиваем таймер
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//Кручу верчу настроить таймер хочу
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	m_tim.TIM_Prescaler = TIM15_PSC;
	m_tim.TIM_Period = TIM15_PWM_MAX;
	TIM_TimeBaseInit(TIM15, &m_tim);
	
	//Настраивае ШИМ где надо
	TIM_OCInitTypeDef m_pwm;
	TIM_OCStructInit(&m_pwm);
	
	//Эти настройки будут везде
	m_pwm.TIM_OCMode = TIM_OCMode_PWM1;
	m_pwm.TIM_OutputState = TIM_OutputState_Enable;
	m_pwm.TIM_Pulse = DRIVER_DEFAULT_PWM;	//50%
	
	TIM_OC1Init(TIM15, &m_pwm);
}

inline void start_ttm(void)
{	//ВРУБАЕМ ТАЙМЕР И ШЫМ ЕБАНЫЙ
	TIM15->CR1 |= TIM_CR1_CEN;
	TIM15->BDTR |= TIM_BDTR_MOE;
}

inline void stop_ttm(void)
{	//ВЫРУБАЕМ И ТАЙМЕР И ЕГО ШИМ
	TIM15->CR1 &= ~TIM_CR1_CEN;
	TIM15->BDTR &= ~TIM_BDTR_MOE;
}

inline void set_speed_ttm(uint16_t speed)
{	//Больше - меньше
	TIM15->ARR = speed;
	TIM15->CCR1 = speed/2;	//50% ШИМ
}