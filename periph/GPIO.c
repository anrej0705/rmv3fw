#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "GPIO.h"
#include "TIM17.h"
#include "TIM16.h"
#include "TIM15.h"

#include "global_vars.h"

bool pa_service_menu_button_lock = 0;
bool pb_main_switch_lock = 0;

void setup_gpio(void)
{	
	//Теперь самый сок - порты блять
	
	//Дрочим всё до чего дотянемся - ну тут на самом деле немного
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitTypeDef m_gpio;
	GPIO_StructInit(&m_gpio);
	
	//Эти параметры будут везде
	m_gpio.GPIO_Speed = GPIO_Speed_2MHz;
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Кнопки
	m_gpio.GPIO_Mode = GPIO_Mode_IPD;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &m_gpio);
	
	//GPIOC
	m_gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOC, &m_gpio);
	
	//GPIOD
	m_gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &m_gpio);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настройка пинов которые будут обычными пинами
	m_gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &m_gpio);
	
	//GPIOC
	m_gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &m_gpio);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настройка АНАЛоговых входов
	m_gpio.GPIO_Mode = GPIO_Mode_AIN;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &m_gpio);
	
	//GPIOC
	m_gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &m_gpio);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Настройка блатных чертей
	m_gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	
	//GPIOA
	m_gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &m_gpio);
	
	//GPIOB
	m_gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &m_gpio);
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

inline void check_buttons()
{
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Проверяем нажатие кнопки сервисного меню
	if((GPIOA->IDR & PA_SERVICE_MENU_BUTTON) == 1 && !pa_service_menu_button_lock)
	{
		pa_service_menu_button_lock = 1;
		
		GPIOA->ODR |= PA_FEED_COIL_DIRECTION;
		GPIOA->ODR &= ~PA_TAKE_COIL_DIRECTION;
		GPIOB->ODR |= PB_TTM_DIRECTION;
		
		//Пока что переключаем направление вращения двигателей
	}
	else if((GPIOA->IDR & PA_SERVICE_MENU_BUTTON) == 0 && pa_service_menu_button_lock)
	{
		pa_service_menu_button_lock = 0;
		
		GPIOA->ODR &= ~PA_FEED_COIL_DIRECTION;
		GPIOA->ODR |= PA_TAKE_COIL_DIRECTION;
		GPIOB->ODR &= ~PB_TTM_DIRECTION;
		
		//Пока что переключаем направление вращения двигателей
	}
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	switch(ui_code)
	{
		case 0:
		{
			//Не используется
			//Проверка клавиатуры
			break;
		}
		case 1:
		{
			//Шаблон настройки цвета
			break;
		}
		case 2:
		{
			//Шаблон счётчика кадров
			break;
		}
		case 3:
		{
			//Очистка от указателей на меняемый цвет
			break;
		}
		case 31:
		{
			//Настройка красного
			break;
		}
		case 32:
		{
			//Настройка зелёного
			break;
		}
		case 33:
		{
			//Настройка синего
			break;
		}
		case 34:
		{
			//Готовность
			break;
		}
		case 35:
		{
			//Сканирование с ожиданием
			break;
		}
		case 36:
		{
			//Протяжка плёнки вперёд
			break;
		}
		/*case 37:
		{
			//Протяжка плёнки назад
			break;
		}*/
		case 38:
		{
			//Сигнал затвора камеры
			break;
		}
		case 39:
		{
			//Пауза
			break;
		}
		case 40:
		{
			//Настройка красного из паузы
			break;
		}
		case 41:
		{
			//Настройка зелёного из паузы
			break;
		}
		case 42:
		{
			//Настройка синего из паузы
			break;
		}
		case 43:
		{
			//Завершено
			break;
		}
		case 44:
		{
			//Управление с компа
			break;
		}
		case 50:
		{
			//Сервисное меню 1 страница
			break;
		}
		case 51:
		{
			//Сервисное меню 2 страница
			break;
		}
		case 52:
		{
			//Сервисное меню 3 страница
			break;
		}
		case 53:
		{
			//Сервисное меню 4 страница
			break;
		}
		case 54:
		{
			//Сервисное меню 5 страница
			break;
		}
		case 55:
		{
			//Сервисное меню 6 страница
			break;
		}
		case 100:
		{
			//Перегрелся двигатель лпм
			break;
		}
		case 101:
		{
			//Перегрелся двигатель раздающей бобины
			break;
		}
		case 102:
		{
			//Перегрелся двигатель принимающей бобины
			break;
		}
		case 103:
		{
			//Сработал таходатчик лпм
			break;
		}
		case 104:
		{
			//Сработал таходатчик раздающей бобины
			break;
		}
		case 105:
		{
			//Сработал таходатчик принимающей бобины
			break;
		}
		case 106:
		{
			//Больше 1 срабатывания разных датчиков
			break;
		}
	}
}

inline void check_switchers()
{
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if((GPIOD->IDR & PD_MOTOR_MAIN_SWITCH) >> 2 == 1 && pb_main_switch_lock)
	{	//Лог.1 - выкл
		pb_main_switch_lock = 0;
		
		ttm_engine_enable = TTM_ENGINE_DISABLE;
		coils_engine_enable = COILS_ENGINE_DISABLE;
		
		ttm_engine_pwm_en = 0;
		green_led_frame_change = 0;
		TIM15->CNT = 0;
		ttm_current_speed = DRIVER_DEFAULT_ARR;
		
		//Выключаем пропеллеры чтобы не шумели, всё равно охлаждать нечего
		engine_cooler_enable = ENGINE_COOLER_DISABLE;
	}
	else if ((GPIOD->IDR & PD_MOTOR_MAIN_SWITCH) >> 2 == 0 && !pb_main_switch_lock)
	{	//Лог.0 - вкл
		pb_main_switch_lock = 1;
		
		ttm_engine_enable = TTM_ENGINE_ENABLE;
		coils_engine_enable = COILS_ENGINE_ENABLE;
		
		ttm_engine_pwm_en = 1;
		
		//Врубаем пропеллеры, а то всё перегреется к хуям)))
		engine_cooler_enable = ENGINE_COOLER_ENABLE;
	}
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

inline void check_keyboard()
{
	//Старшие разряды это строки, а младшие это номера кнопок на строке
	
	for(uint8_t row = 0; row < 4; ++row)
	{
		GPIOC->ODR |= 1 << (row + 6);
		
		if(GPIOC->IDR >> 0 & 0x0001)
		{
			//Первая слева
			
			//Проверяем удерживается ли кнопка нажатой? Её могли нажать и не отпускать
			if(!key_lock)
			{
				key_code = 0 | row << 2;
				
				//Скидываем флаг нажатой кнопки и создаём запрос на обработку
				key_proced = 0;
				
				//Кнопка нажата, пока неясно отпустил ли её чел
				key_lock = 1;
			}
			
			GPIOC->ODR &= ~1 << (row + 6);
			return;
		}
		else if(GPIOC->IDR >> 1 & 0x0001)
		{
			//Вторая слева
			
			//Проверяем удерживается ли кнопка нажатой? Её могли нажать и не отпускать
			if(!key_lock)
			{
				key_code = 1 | row << 2;
				
				//Скидываем флаг нажатой кнопки и создаём запрос на обработку
				key_proced = 0;
				
				//Кнопка нажата, пока неясно отпустил ли её чел
				key_lock = 1;
			}
			
			GPIOC->ODR &= ~1 << (row + 6);
			return;
		}
		else if(GPIOC->IDR >> 2 & 0x0001)
		{
			//Вторая справа
			
			//Проверяем удерживается ли кнопка нажатой? Её могли нажать и не отпускать
			if(!key_lock)
			{
				key_code = 2 | row << 2;
				
				//Скидываем флаг нажатой кнопки и создаём запрос на обработку
				key_proced = 0;
				
				//Кнопка нажата, пока неясно отпустил ли её чел
				key_lock = 1;
			}
			
			GPIOC->ODR &= ~1 << (row + 6);
			return;
		}
		else if(GPIOC->IDR >> 3 & 0x0001)
		{
			//Первая справа
			
			//Проверяем удерживается ли кнопка нажатой? Её могли нажать и не отпускать
			if(!key_lock)
			{
				key_code = 3 | row << 2;
				
				//Скидываем флаг нажатой кнопки и создаём запрос на обработку
				key_proced = 0;
				
				//Кнопка нажата, пока неясно отпустил ли её чел
				key_lock = 1;
			}
			
			GPIOC->ODR &= ~1 << (row + 6);
			return;
		}
		
		GPIOC->ODR &= ~1 << (row + 6);
	}
	
	//Ни одна кнопка не была нажата ну или чел отпустил нажатые кнопки, поэтому сбрасываем
	key_lock = 0;
}
