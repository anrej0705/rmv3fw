#include "TIM4.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "presets.h"
#include "stdbool.h"
#include "GPIO.h"
#include "global_vars.h"
#include "stdio.h"
#include "string.h"
#include "locale_ru.h"
#include "BA63.h"

struct 
{
	char first[21];
	char second[21];
}screen_buf;

//uint8_t i1 = 0;

//Есть такая тема - в контроллере баг, когда настраиваешь таймер при запуске он сразу же даёт 
//прерывание которое никто не ждал. Чтобы такой лабуды не было нужно первый вызов прерывания скипать
bool tim4_irq_bugfix = 0;

void setup_led_screen_update()
{
	//Вырубаем прерывания
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
	NVIC_DisableIRQ(TIM4_IRQn);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//Настраиваем таймер
	TIM_TimeBaseInitTypeDef m_tim;
	TIM_TimeBaseStructInit(&m_tim);
	
	//TIM4 - управление светодиодами и экраном BA63
	m_tim.TIM_ClockDivision = 0;
	m_tim.TIM_CounterMode = TIM_CounterMode_Up;
	m_tim.TIM_RepetitionCounter = 0;
	m_tim.TIM_Prescaler = TIM4_PSC;
	m_tim.TIM_Period = TIM4_ARR;
	TIM_TimeBaseInit(TIM4, &m_tim);
	
	TIM4->CNT = 0;
	
	TIM4->EGR |= TIM_EGR_UG;
	TIM_ClearITPendingBit(TIM4, TIM4_IRQn);
	
	//Врубаем прерывания
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM4_IRQn);
	
	//Чистим буфер экрана
	strncpy(screen_buf.first, string_empty, 21);
	strncpy(screen_buf.second, string_empty, 21);
}

inline void start_led_screen_update()
{
	TIM4->CR1 |= TIM_CR1_CEN;
}

inline void stop_led_screen_update()
{
	TIM4->CR1 &= ~TIM_CR1_CEN;
}

void TIM4_IRQHandler()
{
	//Сбрасываем флаг прерывания а то охуевание будет
	TIM4->SR &= ~TIM_SR_UIF;
	
	//Зануляем счётчик, готовим к новой партии импульсов
	TIM4->CNT = 0;
	
	if(!tim4_irq_bugfix)
	{	//Фиксим ложное срабатывание прерывания
		tim4_irq_bugfix = 1;
		return;
	}
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Зажигаем или гасим светодиод смены кадра
	green_led_frame_change == 0 ? (GPIOB->ODR &= ~PB_FRAME_CHANGE_LED) : (GPIOB->ODR |= PB_FRAME_CHANGE_LED);
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	/*if(feed_coil_tension_sensor != cached_feed_coil_tension_sensor || take_coil_tension_sensor != cached_take_coil_tension_sensor)
	{
		update_screen();
	}*/
	update_screen();
}

void update_screen(void)
{
	char sensor_val[5];
	
	//Обновляем строку сенсоров
	//sprintf(sensor_val, "%04d", feed_coil_tension_sensor);
	//strncpy(&screen_buf.first[0], ru_debug_adc, 4);
	//strncpy(&screen_buf.first[5], sensor_val, 4);
	
	//sprintf(sensor_val, "%04d", take_coil_tension_sensor);
	//strncpy(&screen_buf.first[11], ru_debug_adc, 4);
	//strncpy(&screen_buf.first[16], sensor_val, 5);
	
	//Температура и референсное напряжение
	//sprintf(sensor_val, "%04d", cpu_temp_sensor);
	//strncpy(&screen_buf.second[0], ru_debug_adc, 4);
	//strncpy(&screen_buf.second[5], sensor_val, 4);
	
	tim1_pulses_cnt = TIM1->CNT;
	
	//sprintf(sensor_val, "%04d", tim1_pulses_cnt);
	//strncpy(&screen_buf.second[11], ru_debug_adc, 4);
	//strncpy(&screen_buf.second[16], sensor_val, 5);
	
	//Обновляем кешированные значения чтобы не было повторного вызова
	cached_feed_coil_tension_sensor = feed_coil_tension_sensor;
	cached_take_coil_tension_sensor = take_coil_tension_sensor;
	cached_cpu_temp_sensor = cpu_temp_sensor;
	cached_adc_vref = adc_vref;
	cached_tim1_pulses_cnt = tim1_pulses_cnt;
	
	sprintf(sensor_val, "%04d", feed_coil_current_speed);
	strncpy(&screen_buf.first[0], "X0:", 2);
	strncpy(&screen_buf.first[3], sensor_val, 4);
	
	sprintf(sensor_val, "%04d", feed_coil_target_speed);
	strncpy(&screen_buf.first[10], "X1:", 2);
	strncpy(&screen_buf.first[13], sensor_val, 4);
	
	sprintf(sensor_val, "%04d", feed_coil_tension_sensor);
	strncpy(&screen_buf.second[3], sensor_val, 4);
	
	//Обновляем экран
	BA63_SetPos(0, 0);
	BA63_SendString(screen_buf.first, sizeof(screen_buf.first));
	BA63_SetPos(0, 1);
	BA63_SendString(screen_buf.second, sizeof(screen_buf.second));
	
	//++i1;
}
