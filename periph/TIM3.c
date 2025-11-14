#include "TIM3.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "presets.h"
#include "stdbool.h"
#include "global_vars.h"

uint8_t i = 0;

//Есть такая тема - в контроллере баг, когда настраиваешь таймер при запуске он сразу же даёт 
//прерывание которое никто не ждал. Чтобы такой лабуды не было нужно первый вызов прерывания скипать
bool tim3_irq_bugfix = 0;

void setup_sensor_poll()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//Настраиваем таймер
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//TIM3 - опрос датчиков натяжения
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	m_tim.TIM_Prescaler = TIM3_PSC;
	m_tim.TIM_Period = TIM3_ARR;
	TIM_TimeBaseInit(TIM3, &m_tim);
	
	TIM3->CNT = 0;
	
	TIM3->EGR |= TIM_EGR_UG;
	TIM_ClearITPendingBit(TIM3, TIM4_IRQn);
	
	//Врубаем прерывания
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM3_IRQn);
}
inline void start_sensor_poll()
{
	TIM3->CR1 |= TIM_CR1_CEN;
}
inline void stop_sensor_poll()
{
	TIM3->CR1 &= ~TIM_CR1_CEN;
}
void TIM3_IRQHandler()
{
	//Сбрасываем флаг прерывания а то охуевание будет
	TIM3->SR &= ~TIM_SR_UIF;
	
	//Зануляем счётчик, готовим к новой партии импульсов
	TIM3->CNT = 0;
	
	if(!tim3_irq_bugfix)
	{	//Фиксим ложное срабатывание прерывания
		tim3_irq_bugfix = 1;
		return;
	}
	
	/*ADC1->CR2 |= ADC_CR2_SWSTART;
	//while(!(ADC1->SR & ADC_SR_EOC))
	//{
	//	//NOP
	//}
	//Ждём
	for(int i = 0; i < 256; ++i)
	{
		//NOP
	}*/
	
	//feed_coil_tension_sensor = ADC1->DR;
	//take_coil_tension_sensor = ADC1->JOFR1;
	feed_coil_tension_sensor = adc_buffer[0];
	take_coil_tension_sensor = adc_buffer[1];
	cpu_temp_sensor = adc_buffer[2];
	//adc_vref = adc_buffer[3];
	
	++i;
}
