#include "TIM16.h"
#include "stm32f10x_tim.h"
#include "presets.h"
#include "GPIO.h"

#include "global_vars.h"

//Настройка таймера
void setup_take_coil(void)
{
	//Врубаем тактирование
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	
	//Настраиваем таймер
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//Кручу верчу настроить таймер хочу
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	m_tim.TIM_Prescaler = TIM16_PSC;
	m_tim.TIM_Period = TIM16_PWM_MAX;
	TIM_TimeBaseInit(TIM16, &m_tim);
	
	//Настраивае ШИМ где надо
	TIM_OCInitTypeDef m_pwm;
	TIM_OCStructInit(&m_pwm);
	
	//Эти настройки будут везде
	m_pwm.TIM_OCMode = TIM_OCMode_PWM1;
	m_pwm.TIM_OutputState = TIM_OutputState_Enable;
	m_pwm.TIM_Pulse = DRIVER_DEFAULT_PWM;	//50%
	
	TIM_OC1Init(TIM16, &m_pwm);
	
	//Настраиваем направления кручения хуй
	GPIOA->ODR |= PA_TAKE_COIL_DIRECTION;
}

inline void start_take_coil(void)
{	//ВРУБАЕМ ТАЙМЕР И ШЫМ ЕБАНЫЙ
	TIM16->CR1 |= TIM_CR1_CEN;
	TIM16->BDTR |= TIM_BDTR_MOE;
	
	//Врубаем лампочку
	yellow_led_take_coil = 1;
}

inline void stop_take_coil(void)
{	//ВЫРУБАЕМ И ТАЙМЕР И ЕГО ШИМ
	TIM16->CR1 &= ~TIM_CR1_CEN;
	TIM16->BDTR &= ~TIM_BDTR_MOE;
	
	//Зануляем счётчик
	//TIM16->CNT = 0;
	
	//Вырубаем лампочку
	yellow_led_take_coil = 0;
}

inline void set_speed_take_coil(uint16_t speed)
{	//Больше - меньше
	uint16_t tmp = TIM16->CNT;
	
	//Проверяем если задаётся одна и та же скорость то не надо ломать работу таймера
	if(TIM16->ARR == speed)
	{
		return;
	}
	
	TIM16->ARR = speed;
	TIM16->CCR1 = speed/2;	//50% ШИМ
	
	debug_take_coil_arr = speed;
	debug_take_coil_ccr1 = speed/2;
	
	if(TIM16->CNT > speed)
	{
		TIM16->CNT = speed - 1;
	}
}

inline uint16_t get_speed_take_coil()
{
	return TIM16->ARR;
}
