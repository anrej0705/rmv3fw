#include "speed_ctrl.h"
#include "global_vars.h"
#include "speed_lut.h"
#include "TIM16.h"
#include "TIM17.h"

//Работа этой функции рассказана в файле "предварительно рассчитанная таблица значений.txt"
//current_speed нужно передавать как ссылку чтобы скорость менялась
uint16_t calc_segment(uint16_t target_val, uint16_t *current_speed, bool engine_select)
{
	if(target_val >= 2000 && engine_select)										//Защита от выхода за пределы таблицы
	{
		return feed_coil_acceleration_lut[*current_speed];
	}
	else if(target_val >= 2000 && !engine_select)							//Защита от выхода за пределы таблицы
	{
		return coil_acceleration_lut[*current_speed];
	}
	
	//Выбрана подающая бобина
	if(engine_select)
	{
		if(*current_speed > FEED_COIL_START_THRESHOLD && !feed_coil_lock)
		{
			feed_coil_lock = 1;
			stop_feed_coil();
		}
		else if(*current_speed < FEED_COIL_START_THRESHOLD && feed_coil_lock)
		{
			feed_coil_lock = 0;
			start_feed_coil();
		}
	}
	else
	{
		if(*current_speed < TAKE_COIL_STOP_THRESHOLD && !take_coil_lock)
		{
			take_coil_lock = 1;
			stop_take_coil();
		}
		else if(*current_speed > TAKE_COIL_STOP_THRESHOLD && take_coil_lock)
		{
			take_coil_lock = 0;
			start_take_coil();
		}
	}
	
	//Если current_speed и target_speed равны то обновляем значения считая что функция закончила плавный переход
	if(*current_speed == target_val && engine_select)
	{
		*current_speed = target_val;														//Задаём длину отрезка
		return feed_coil_acceleration_lut[*current_speed];			//Возвращаем текущую скорость
	}
	if(*current_speed == target_val && !engine_select)
	{
		*current_speed = target_val;														//Задаём длину отрезка
		return coil_acceleration_lut[*current_speed];						//Возвращаем текущую скорость
	}
	
	//Если отрезок идёт право от 0, то прибавляем но если влево то отнимаем
	if(*current_speed <= target_val && (*current_speed + 5) < COIL_ACCELERATION_LUT_SIZE)
	{
		*current_speed += 5;
		if(*current_speed + 5 >= target_val)										//Если очередное прибавление в цикле превысит значение конца отрезка то считаем что достигли конца отрезка
		{
			*current_speed = target_val;
		}
	}
	else if(*current_speed >= target_val && (*current_speed - 5) > 0)
	{
		*current_speed -= 5;
		if(*current_speed - 5 <= target_val)										//Если очередное прибавление в цикле превысит значение конца отрезка то считаем что достигли конца отрезка
		{
			*current_speed = target_val;
		}
	}
	
	if(engine_select)
	{
		degub_selected_feed_coil_pwm = feed_coil_acceleration_lut[*current_speed/* - 400*/];
		return feed_coil_acceleration_lut[*current_speed/* - 400*/];
	}
	else
	{
		degub_selected_take_coil_pwm = coil_acceleration_lut[*current_speed];
		return coil_acceleration_lut[*current_speed];
	}
}

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
