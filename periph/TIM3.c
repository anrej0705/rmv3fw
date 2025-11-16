#include "TIM3.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "presets.h"
#include "stdbool.h"
#include "global_vars.h"
//Управление скоростью
#include "TIM15.h"
#include "TIM16.h"
#include "TIM17.h"

#include "speed_lut.h"
#include "stdbool.h"

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
	
	insert_sample(feed_coil_samples_map, &feed_coil_semaples_map_ptr, feed_coil_tension_sensor);
	insert_sample(feed_coil_samples_map, &take_coil_semaples_map_ptr, take_coil_tension_sensor);
	
	//set_speed_feed_coil(calc_segment(get_sample(feed_coil_samples_map)));
	set_speed_feed_coil(calc_segment(get_sample(feed_coil_samples_map), &feed_coil_current_speed, &feed_coil_target_speed, FEED_COIL));
	//set_speed_feed_coil(calc_segment(get_sample(take_coil_samples_map), &take_coil_current_speed, &take_coil_target_speed, TAKE_COIL));
	
	//set_speed_feed_coil(calc_segment(feed_coil_tension_sensor));
	//set_speed_take_coil(calc_segment(take_coil_tension_sensor));
}

//Работа этой функции рассказана в файле "предварительно рассчитанная таблица значений.txt"
uint16_t calc_segment(uint16_t target_val, uint16_t *current_speed, uint16_t *target_speed, bool engine_select)
{
	if(target_val >= 2000)													//Защита от выхода за пределы таблицы
	{
		return coil_acceleration_lut[feed_coil_current_speed];
	}
	
	//Выбрана подающая бобина
	if(engine_select)
	{
		if(feed_coil_current_speed < TAKE_COIL_START_THRESHOLD && !feed_coil_lock)
		{
			feed_coil_lock = 1;
			stop_feed_coil();
		}
		else if(feed_coil_current_speed > FEED_COIL_START_THRESHOLD && feed_coil_lock)
		{
			feed_coil_lock = 0;
			start_feed_coil();
		}
	}
	else
	{
		if(take_coil_current_speed < TAKE_COIL_STOP_THRESHOLD && !take_coil_lock)
		{
			take_coil_lock = 1;
			stop_take_coil();
		}
		else if(take_coil_current_speed > FEED_COIL_STOP_THRESHOLD && take_coil_lock)
		{
			take_coil_lock = 0;
			start_take_coil();
		}
	}
	
	//Если current_speed и target_speed равны то обновляем значения считая что функция закончила плавный переход
	if(feed_coil_current_speed == feed_coil_target_speed)
	{
		feed_coil_target_speed = target_val;										//Задаём длину отрезка
		return coil_acceleration_lut[feed_coil_current_speed];	//Возвращаем текущую скорость
	}
	
	//Если отрезок идёт право от 0, то прибавляем но если влево то отнимаем
	if(feed_coil_current_speed < feed_coil_target_speed && (feed_coil_current_speed + 10) < COIL_ACCELERATION_LUT_SIZE)
	{
		feed_coil_current_speed += 10;
		if(feed_coil_current_speed + 10 >= feed_coil_target_speed)				//Если очередное прибавление в цикле превысит значение конца отрезка то считаем что достигли конца отрезка
		{
			feed_coil_current_speed = feed_coil_target_speed;
		}
	}
	else if(feed_coil_current_speed > feed_coil_target_speed && (feed_coil_current_speed - 10) > 0)
	{
		feed_coil_current_speed -= 10;
		if(feed_coil_current_speed - 10 <= feed_coil_target_speed)				//Если очередное прибавление в цикле превысит значение конца отрезка то считаем что достигли конца отрезка
		{
			feed_coil_current_speed = feed_coil_target_speed;
		}
	}
	
	return coil_acceleration_lut[feed_coil_current_speed];
}

/*
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
	
	insert_sample(feed_coil_samples_map, &feed_coil_semaples_map_ptr, feed_coil_tension_sensor);
	insert_sample(feed_coil_samples_map, &take_coil_semaples_map_ptr, take_coil_tension_sensor);
	
	set_speed_feed_coil(calc_segment(get_sample(feed_coil_samples_map), &feed_coil_current_speed, &feed_coil_target_speed, FEED_COIL));
	set_speed_feed_coil(calc_segment(get_sample(take_coil_samples_map), &take_coil_current_speed, &take_coil_target_speed, TAKE_COIL));
}

//Работа этой функции рассказана в файле "предварительно рассчитанная таблица значений.txt"
uint16_t calc_segment(uint16_t target_val, uint16_t *current_speed, uint16_t *target_speed, bool engine_select)
{
	if(target_val >= 2000)													//Защита от выхода за пределы таблицы
	{
		return coil_acceleration_lut[*current_speed];
	}
	
	//Выбрана подающая бобина
	if(engine_select)
	{
		//Проверка и остановка или запуск двигателя подающей бобины
		if(*current_speed < TAKE_COIL_START_THRESHOLD && !feed_coil_lock)
		{
			feed_coil_lock = 1;
			stop_feed_coil();
		}
		else if(*current_speed > FEED_COIL_START_THRESHOLD && feed_coil_lock)
		{
			feed_coil_lock = 0;
			start_feed_coil();
		}
	}
	else
	{
		//Проверка и остановка или запуск двигателя принимающей бобины
		if(*current_speed < TAKE_COIL_STOP_THRESHOLD && !take_coil_lock)
		{
			take_coil_lock = 1;
			stop_take_coil();
		}
		else if(*current_speed > FEED_COIL_STOP_THRESHOLD && take_coil_lock)
		{
			take_coil_lock = 0;
			start_take_coil();
		}
	}
	
	//Если current_speed и target_speed равны то обновляем значения считая что функция закончила плавный переход
	if(current_speed == target_speed)
	{
		*target_speed = target_val;										//Задаём длину отрезка
		return coil_acceleration_lut[*current_speed];	//Возвращаем текущую скорость
	}
	
	//Если отрезок идёт право от 0, то прибавляем но если влево то отнимаем
	if(*current_speed < *target_speed && (*current_speed + 10) < COIL_ACCELERATION_LUT_SIZE)
	{
		*current_speed += 10;
		if(*current_speed + 10 >= *target_speed)				//Если очередное прибавление в цикле превысит значение конца отрезка то считаем что достигли конца отрезка
		{
			*current_speed = *target_speed;
		}
	}
	else if(*current_speed > *target_speed && (current_speed - 10) > 0)
	{
		*current_speed -= 10;
		if(*current_speed - 10 <= *target_speed)				//Если очередное прибавление в цикле превысит значение конца отрезка то считаем что достигли конца отрезка
		{
			*current_speed = *target_speed;
		}
	}
	
	return coil_acceleration_lut[*current_speed];
}
*/

//Добавляем в карту инфу из ацп
inline void insert_sample(uint16_t *samples_map, uint8_t *samples_ptr, uint16_t new_sample)
{
	if(*samples_ptr >= COIL_AA_SAMPLES)
	{
		*samples_ptr = 0;
	}
	
	samples_map[*samples_ptr] = new_sample;
	
	++*samples_ptr;
}

//Читаем сглаженное значение
inline uint16_t get_sample(uint16_t *samples_map)
{
	uint32_t tmp = 0;
	for(uint8_t idx = 0; idx < COIL_AA_SAMPLES; ++idx)
	{
		tmp += samples_map[idx];
	}
	
	return (uint16_t)(tmp / COIL_AA_SAMPLES);
}
