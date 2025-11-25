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
uint8_t ptr = 0;

uint8_t ui_code_arr[31] = { 0, 1, 2, 3, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 50, 51, 52, 53, 54, 55, 100, 101, 102, 103, 104, 105, 106 };

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
			if(cycles_nop == 0)
			{
				skip_cycle = 1;
			}
			if(cycles_nop >= 50)
			{
				//Симулируем датчик обратной связи
				skip_cycle = 0;
				cycles_nop = 0;
				ui_code = 37;
			}
		}
		case 37:
		{
			if(cycles_nop == 0)
			{
				skip_cycle = 1;
			}
			if(cycles_nop >= 50)
			{
				//Симулируем протяжку плёнки
				skip_cycle = 0;
				cycles_nop = 0;
				ui_code = 45;
			}
		}
		case 38:
		{
			if(cycles_nop == 0)
			{
				skip_cycle = 1;
			}
			if(cycles_nop >= 50)
			{
				//Симулируем сигнал на затвор
				skip_cycle = 0;
				cycles_nop = 0;
				
				//+1 к отфотканным кадрам
				++frames_counter;
				
				ui_code = 36;
			}
		}
		case 45:
		{
			if(cycles_nop == 0)
			{
				skip_cycle = 1;
			}
			if(cycles_nop >= 50)
			{
				//Симулируем работу двигателей
				skip_cycle = 0;
				cycles_nop = 0;
				ui_code = 38;
			}
		}
		case 47:
		{
			if(pause_request)
			{
				//Настройка из режима паузы
				ui_code = 39;
			}
			else
			{
				if(cycles_nop == 0)
				{
					skip_cycle = 1;
				}
				if(cycles_nop >= 50)
				{
					//Условно, типа ждём механику
					skip_cycle = 0;
					cycles_nop = 0;
					ui_code = 35;
				}
			}
			break;
		}
	}
	
	if(skip_cycle)
	{
		++cycles_nop;
	}
	else
	{
		cycles_nop = 0;
	}
	
	//if(!ttm_engine_pwm_en)
	//{
	//	++psc;
	//}
	
	/*if(psc == 160)
	{
		TIM2->CCR2 = 384;
	}
	if(psc == 210)
	{
		TIM2->CCR2 = 0;
	}
	
	if(psc == 260)
	{
		TIM2->CCR3 = 384;
	}
	if(psc == 310)
	{
		TIM2->CCR3 = 0;
	}
	
	if(psc == 360)
	{
		TIM2->CCR4 = 384;
	}
	if(psc == 410)
	{
		TIM2->CCR4 = 0;
	}
	
	if(psc == 480)
	{
		ttm_engine_pwm_en = 1;
		green_led_frame_change = 0;
		psc = 0;
		TIM2->CCR2 = 0;
		TIM2->CCR3 = 0;
		TIM2->CCR4 = 0;
	}*/
	
	/*if(psc == 180)
	{
		ttm_engine_pwm_en = 1;
		green_led_frame_change = 0;
		psc = 0;
	}*/
	
	if(psc == 100)
	{
		++ptr;
		if(ptr >= 31)
		{
			ptr = 0;
		}
		
		//ui_code = ui_code_arr[ptr];
		
		psc = 0;
	}
	
	//Смотрим битики
	ttm_engine_enable == TTM_ENGINE_DISABLE ? (GPIOB->ODR |= PB_TTM_ENABLE) : (GPIOB->ODR &= ~PB_TTM_ENABLE);
	coils_engine_enable == COILS_ENGINE_DISABLE ? (GPIOB->ODR |= PB_COILS_ENABLE) : (GPIOB->ODR &= ~PB_COILS_ENABLE);
	engine_cooler_enable == ENGINE_COOLER_DISABLE ? (GPIOB->ODR |= PB_FAN_ENABLE) : (GPIOB->ODR &= ~PB_FAN_ENABLE);
	
	ttm_engine_pwm_en == TTM_ENGINE_DISABLE ? (stop_ttm()) : (start_ttm());
	feed_coil_engine_pwm_en == COILS_ENGINE_DISABLE ? (stop_feed_coil()) : (start_feed_coil());
	take_coil_engine_pwm_en == ENGINE_COOLER_DISABLE ? (stop_take_coil()) : (start_take_coil());
	
	//GPIOB->ODR |= PB_COILS_ENABLE;
	//GPIOB->ODR |= PB_TTM_ENABLE;
	//GPIOB->ODR |= PB_FAN_ENABLE;
	
	insert_sample(feed_coil_samples_map, &feed_coil_semaples_map_ptr, feed_coil_tension_sensor);
	insert_sample(take_coil_samples_map, &take_coil_semaples_map_ptr, take_coil_tension_sensor);
	
	set_speed_feed_coil(calc_segment(get_sample(feed_coil_samples_map), &feed_coil_current_speed, feed_coil_tension_sensor, FEED_COIL));
	set_speed_take_coil(calc_segment(get_sample(take_coil_samples_map), &take_coil_current_speed, take_coil_tension_sensor, TAKE_COIL));
	set_speed_ttm(get_new_speed());
}
