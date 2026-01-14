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

#include "GPIO.h"

//Расчёты
#include "speed_ctrl.h"

#include "speed_lut.h"
#include "stdbool.h"

uint16_t psc = 0;

//Есть такая тема - в контроллере баг, когда настраиваешь таймер при запуске он сразу же даёт 
//прерывание которое никто не ждал. Чтобы такой лабуды не было нужно первый вызов прерывания скипать
bool tim3_irq_bugfix = 0;
	
//Счётчик пропущенных циклов и флаг пропуска циклов
bool skip_cycle = 0;
uint16_t cycles_nop = 0;

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
	
	switch(ui_code)
	{
		case 36:
		{
			if(!camera_busy)
			{
				//Симулируем датчик обратной связи
				skip_cycle = 0;
				cycles_nop = 0;
				ui_code = 38;
			}
			//ui_code = 37;
			
			break;
		}
		case 37:
		{
			if(drop_detect)
			{
				//Завершаем сканирование
				ui_code = 43;
			}
			else if(!ttm_engine_pwm_en)
			{
				//ui_code = 255;
				
				//Запускаем протяжку на 1 кадр вперёд
				ttm_current_speed = TTM_START_SPEED;
				ttm_engine_pwm_en = 1;
				start_ttm();
			}
			
			break;
		}
		case 38:
		{
			//Подаём сигнал на затвор
			camera_shot = 1;
			
			if(cycles_nop == 0)
			{
				skip_cycle = 0;
			}
			if(cycles_nop >= 40)
			{
				//Симулируем сигнал на затвор
				skip_cycle = 0;
				cycles_nop = 0;
				
				//+1 к отфотканным кадрам
				//++frames_counter;
				
				ui_code = 48;
			}
			
			break;
		}
		case 39:
		{
			//Сбрасываем счетчик для защитного интервала
			def = 0;
			
			break;
		}
		case 43:
		{
			//Снимаем сигнал на затвор камеры
			camera_shot = 0;
			//Гасим свет
			led_enabled = 0;
			//Выкл двигателей
			ttm_engine_enable = TTM_ENGINE_ENABLE;
			coils_engine_enable = COILS_ENGINE_ENABLE;
			
			break;
		}
		case 45:
		{
			if(cycles_nop == 0)
			{
				skip_cycle = 0;
			}
			
			//Выжидаем 150 мс чтобы на экране значение обновились
			//Костыль, чтобы кнопка паузы успела опроситься хотя бы 1 раз
			if(cycles_nop >= 150)
			{
				//Ждём остановки двигателя подающей бобины
				if(!yellow_led_feed_coil)
				{
					ui_code = 36;
				}
			}
			
			break;
		}
		case 47:
		{
			engines_enabled = 1;
			
			if(pause_request)
			{
				//Настройка из режима паузы
				ui_code = 39;
			}
			else if(!yellow_led_feed_coil && !yellow_led_take_coil)
			{
					ui_code = 35;
			}
			
			break;
		}
		case 48:
		{
			//Снимаем сигнал на затвор
			camera_shot = 0;
			
			//Логика поменялась. Теперь флаг означает что надо подождать пока отработает затвор
			//время задержки устанавливает пользователь
			if(camera_busy)
			{
				//Симулируем сигнал на затвор
				skip_cycle = 0;
				cycles_nop = 0;
				
				ui_code = 81;
			}
			
			break;
		}
		case 81:
		{
			if(cycles_nop >= xt_shutter_delay)
			{
				//Симулируем сигнал на затвор
				skip_cycle = 0;
				cycles_nop = 0;
				
				//+1 к отфотканным кадрам
				++frames_counter;
				
				ui_code = 37;
				
			}
			
			break;
		}
		default:
		{
			//Ничего не делаем в случае ошибочного попадания
			break;
		}
	}
	
	if(!skip_cycle)
	{
		++cycles_nop;
	}
	else
	{
		cycles_nop = 0;
	}
	
	//Смотрим битики
	ttm_engine_enable == TTM_ENGINE_DISABLE ? (GPIOB->ODR |= PB_TTM_ENABLE) : (GPIOB->ODR &= ~PB_TTM_ENABLE);
	coils_engine_enable == COILS_ENGINE_DISABLE ? (GPIOB->ODR |= PB_COILS_ENABLE) : (GPIOB->ODR &= ~PB_COILS_ENABLE);
	//engine_cooler_enable == ENGINE_COOLER_DISABLE ? (GPIOB->ODR |= PB_FAN_ENABLE) : (GPIOB->ODR &= ~PB_FAN_ENABLE);
	
	camera_shot == 1 ? (GPIOC->ODR |= PC_CAMERA_SHOT) : (GPIOC->ODR &= ~PC_CAMERA_SHOT);
	
	ttm_engine_pwm_en == 1 ? (start_ttm()) : (stop_ttm());
	feed_coil_engine_pwm_en == COILS_ENGINE_DISABLE ? (stop_feed_coil()) : (start_feed_coil());
	take_coil_engine_pwm_en == ENGINE_COOLER_DISABLE ? (stop_take_coil()) : (start_take_coil());
	
	//Опрос датчика обратной связи камеры
	if(callback_sensor > CALLBACK_THRESHOLD_EN && !camera_busy)
	{
		camera_busy = 1;
	}
	else if(callback_sensor < CALLBACK_THRESHOLD_DIS && camera_busy)
	{
		camera_busy = 0;
	}
	
	insert_sample(feed_coil_samples_map, &feed_coil_semaples_map_ptr, feed_coil_tension_sensor);
	insert_sample(take_coil_samples_map, &take_coil_semaples_map_ptr, take_coil_tension_sensor);
	
	if(!film_direction)
	{
		GPIOA->ODR &= ~PA_FEED_COIL_DIRECTION;
		GPIOA->ODR |= PA_TAKE_COIL_DIRECTION;
		GPIOB->ODR |= PB_TTM_DIRECTION;
		
		//Пока что переключаем направление вращения двигателей
	}
	else
	{
		GPIOA->ODR |= PA_FEED_COIL_DIRECTION;
		GPIOA->ODR &= ~PA_TAKE_COIL_DIRECTION;
		GPIOB->ODR &= ~PB_TTM_DIRECTION;
		
		//Пока что переключаем направление вращения двигателей
	}

	set_speed_feed_coil(calc_segment(get_sample(feed_coil_samples_map), &feed_coil_current_speed, feed_coil_tension_sensor, FEED_COIL));
	
	if(!take_coil_freeze)
	{
		//Фикс - проверка флага до расчёта скорости так как расчёт скорости отключает двигатель
		//если достигнут порог остановки
		if(!take_coil_freeze_lock)
		{
			take_coil_engine_pwm_en = 1;
			take_coil_freeze_lock = 1;
		}
		set_speed_take_coil(calc_segment(get_sample(take_coil_samples_map), &take_coil_current_speed, take_coil_tension_sensor, TAKE_COIL));
	}
	else if(take_coil_freeze_lock)
	{
		take_coil_engine_pwm_en = 0;
		take_coil_freeze_lock = 0;
		//drop_detect = 0;
	}
	
	set_speed_ttm(get_new_speed());
}
