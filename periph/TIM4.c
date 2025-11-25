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
	if(ui_code == cached_ui_code && key_proced)
	{	//Убираем бесполезную перерисовку
		return;
	}
	
	switch(ui_code)
	{
		case 0:
		{
			//Не используется
			BA63_ClearVFD();
			
			sprintf(debug_key_code_char, "%02d", key_code);
			strncpy(&screen_buf.first[0], debug_key_code_char, 2);
			
			key_code = 0;
			
			BA63_SetPos(0,0);
			BA63_SendString(screen_buf.first, 21);
			
			cached_ui_code = ui_code;
			
			//Подымаем флаг обработанного нажатия кнопки
			key_proced = 1;
			break;
		}
		case 1:
		{
			//Шаблон настройки цвета
			
			//Рисуем шаблон настроек в буфере
			strncpy(screen_buf.first, ru_color_preset_template, 21);
			//Пишем подсказку
			strncpy(screen_buf.second, ru_color_preset_hint, 21);
			
			//Загружаем текущие значения баланса цвета
			sprintf(char_level_red, "%03d", level_red);
			strncpy(&screen_buf.first[3], char_level_red, 3);
			sprintf(char_level_green, "%03d", level_green);
			strncpy(&screen_buf.first[9], char_level_green, 3);
			sprintf(char_level_blue, "%03d", level_blue);
			strncpy(&screen_buf.first[15], char_level_blue, 3);
			
			//Отправляем на экран
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			
			//Переходим к настройке красного
			ui_code = 31;
			break;
		}
		case 2:
		{
			//Шаблон счётчика кадров
			
			//Рисуем в буфер на второй строчке
			strncpy(screen_buf.second, ru_frames_template, 21);
			
			//Отправляем на экран
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			ui_code = 36;
			
			break;
		}
		case 3:
		{
			//Очистка от указателей на меняемый цвет
			
			//Убираем все квадратные скобки
			screen_buf.first[0] = ' ';
			screen_buf.first[6] = ' ';
			screen_buf.first[12] = ' ';
			screen_buf.first[18] = ' ';
			
			//Отправляем на экран
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			
			cached_ui_code = ui_code;
			
			//Переходим в режим готовности
			ui_code = 46;
			
			break;
		}
		case 31:
		{
			//Настройка красного
			
			//Рисуем в буфере квадратные скобки как-бэ показывая что сейчас изменяется красный цвет
			screen_buf.first[0] = '[';
			screen_buf.first[6] = ']';
			screen_buf.first[12] = ' ';
			screen_buf.first[18] = ' ';
			
			//Рисуем число в квадратных скобочках
			sprintf(char_level_red, "%03d", level_red);
			strncpy(&screen_buf.first[3], char_level_red, 3);
			
			//Отправляем на экран
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			
			cached_ui_code = ui_code;
			
			//Подымаем флаг обработанного нажатия кнопки
			key_proced = 1;
			
			break;
		}
		case 32:
		{
			//Настройка зелёного
			
			//Рисуем в буфере квадратные скобки как-бэ показывая что сейчас изменяется зелёный цвет
			screen_buf.first[0] = ' ';
			screen_buf.first[6] = '[';
			screen_buf.first[12] = ']';
			screen_buf.first[18] = ' ';
			
			//Рисуем число в квадратных скобочках
			sprintf(char_level_green, "%03d", level_green);
			strncpy(&screen_buf.first[9], char_level_green, 3);
			
			//Отправляем на экран
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 33:
		{
			//Настройка синего
			
			//Рисуем в буфере квадратные скобки как-бэ показывая что сейчас изменяется синий цвет
			screen_buf.first[0] = ' ';
			screen_buf.first[6] = ' ';
			screen_buf.first[12] = '[';
			screen_buf.first[18] = ']';
			
			//Рисуем число в квадратных скобочках
			sprintf(char_level_blue, "%03d", level_blue);
			strncpy(&screen_buf.first[15], char_level_blue, 3);
			
			//Отправляем на экран
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 35:
		{
			//Готовность
			
			//Пишем надпись готовности
			strncpy(screen_buf.second, ru_ready, 21);
			
			//Отправляем на экран, на вторую строку
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 36:
		{
			//Сканирование с ожиданием
			
			//Чистим значки в буфере и обновляем экран
			screen_buf.second[10] = ' ';
			screen_buf.second[12] = ' ';
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 37:
		{
			//Протяжка плёнки вперёд
			
			//Ставим значок
			screen_buf.second[14] = ' ';
			screen_buf.second[12] = 0x99;
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 38:
		{
			//Сигнал затвора камеры
			
			//Обновляем счётчик кадров
			sprintf(char_frames_counter, "%05d", frames_counter);
			strncpy(&screen_buf.second[5], char_frames_counter, 5);
			
			//Ставим значок
			screen_buf.second[12] = 0x90;
			screen_buf.second[14] = 0x3C;
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 39:
		{
			//Пауза
			
			//Пишем надпись паузы
			strncpy(screen_buf.second, ru_paused, 21);
			
			//Отправляем на экран, на вторую строку
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 40:
		{
			//Настройка красного из паузы
			
			//Рисуем в буфере квадратные скобки как-бэ показывая что сейчас изменяется красный цвет
			screen_buf.first[0] = '[';
			screen_buf.first[6] = ']';
			screen_buf.first[12] = ' ';
			screen_buf.first[18] = ' ';
			
			//Рисуем число в квадратных скобочках
			sprintf(char_level_red, "%03d", level_red);
			strncpy(&screen_buf.first[3], char_level_red, 3);
			
			//Отправляем на экран
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 41:
		{
			//Настройка зелёного из паузы
			
			//Рисуем в буфере квадратные скобки как-бэ показывая что сейчас изменяется красный цвет
			screen_buf.first[0] = ' ';
			screen_buf.first[6] = '[';
			screen_buf.first[12] = ']';
			screen_buf.first[18] = ' ';
			
			//Рисуем число в квадратных скобочках
			sprintf(char_level_green, "%03d", level_green);
			strncpy(&screen_buf.first[9], char_level_green, 3);
			
			//Отправляем на экран
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 42:
		{
			//Настройка синего из паузы
			
			//Рисуем в буфере квадратные скобки как-бэ показывая что сейчас изменяется красный цвет
			screen_buf.first[0] = ' ';
			screen_buf.first[6] = ' ';
			screen_buf.first[12] = '[';
			screen_buf.first[18] = ']';
			
			//Рисуем число в квадратных скобочках
			sprintf(char_level_blue, "%03d", level_blue);
			strncpy(&screen_buf.first[15], char_level_blue, 3);
			
			//Отправляем на экран
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 43:
		{
			//Завершено
			
			//Пишем надпись паузы
			strncpy(screen_buf.second, ru_paused, 21);
			
			//Отправляем на экран, на вторую строку
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 44:
		{
			//Управление с компа
			
			//Пишем надпись, сообщающую о том что эта хрень с компа рулится
			strncpy(screen_buf.second, ru_remote_ctrl, 21);
			
			//Отправляем на экран, на вторую строку
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 45:
		{
			//Ожидание остановки двигателей
			
			//Убираем значок протяжки
			screen_buf.second[12] = '*';
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			break;
		}
		case 46:
		{
			//Сообщение с просьбой включить двигатели
			if(!engine_override)
			{
				strncpy(screen_buf.second, ru_engine_disabled, 21);
				BA63_SetPos(0, 1);
				BA63_SendString(screen_buf.second, 21);
			}
			else
			{
				cached_ui_code = ui_code;
				ui_code = 47;
			}
			break;
		}
		case 47:
		{
			//Сообщение с просьбой подождать - механика готовится
			strncpy(screen_buf.second, ru_wait, 21);
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			//ui_code = 35;
			break;
		}
		case 50:
		{
			//Сервисное меню 1 страница
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.first, ru_service_menu_1, 21);
			strncpy(screen_buf.second, ru_service_menu_2, 21);
			
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 51:
		{
			//Сервисное меню 2 страница
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.first, ru_service_menu_3, 21);
			strncpy(screen_buf.second, ru_service_menu_4, 21);
			
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 52:
		{
			//Сервисное меню 3 страница
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.first, ru_service_menu_5, 21);
			strncpy(screen_buf.second, ru_service_menu_6, 21);
			
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 53:
		{
			//Сервисное меню 4 страница
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.first, ru_service_menu_7, 21);
			strncpy(screen_buf.second, ru_service_menu_8, 21);
			
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 54:
		{
			//Сервисное меню 5 страница
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.first, ru_service_menu_9, 21);
			strncpy(screen_buf.second, ru_service_menu_10, 21);
			
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 55:
		{
			//Сервисное меню 6 страница
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.first, ru_service_menu_11, 21);
			strncpy(screen_buf.second, ru_service_menu_12, 21);
			
			BA63_SetPos(0, 0);
			BA63_SendString(screen_buf.first, 21);
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 100:
		{
			//Перегрелся двигатель лпм
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.second, ru_alarm_ttm_engine_overheat, 21);
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 101:
		{
			//Перегрелся двигатель раздающей бобины
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.second, ru_alarm_feed_engine_overheat, 21);
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 102:
		{
			//Перегрелся двигатель принимающей бобины
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.second, ru_alarm_take_engine_overheat, 21);
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 103:
		{
			//Сработал таходатчик лпм
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.second, ru_alarm_ttm_break, 21);
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 104:
		{
			//Сработал таходатчик раздающей бобины
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.second, ru_alarm_feed_break, 21);
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 105:
		{
			//Сработал таходатчик принимающей бобины
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.second, ru_alarm_take_break, 21);
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
		case 106:
		{
			//Больше 1 срабатывания разных датчиков
			
			//Рисуем пункты меню на экране
			strncpy(screen_buf.second, ru_alarm_general_feilor, 21);
			
			BA63_SetPos(0, 1);
			BA63_SendString(screen_buf.second, 21);
			
			cached_ui_code = ui_code;
			break;
		}
	}
	/*char sensor_val[5];
	
	char char_feed_coil_dv[4];
	char char_take_coil_dv[4];
	
	//Обновляем строку сенсоров
	sprintf(sensor_val, "%04d", feed_coil_tension_sensor);
	//strncpy(&screen_buf.first[0], ru_debug_adc, 4);
	strncpy(&screen_buf.first[0], sensor_val, 4);
	
	sprintf(sensor_val, "%04d", take_coil_tension_sensor);
	//strncpy(&screen_buf.second[0], ru_debug_adc, 4);
	strncpy(&screen_buf.second[0], sensor_val, 4);
	
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
	
	sprintf(sensor_val, "%04d", debug_feed_coil_arr);
	strncpy(&screen_buf.first[5], sensor_val, 4);
	
	sprintf(char_feed_coil_dv, "%03d", feed_coil_dv);
	strncpy(&screen_buf.first[10], char_feed_coil_dv, 3);
	
	sprintf(char_feed_coil_dv, "%03d", feed_coil_slowdown_dv);
	strncpy(&screen_buf.first[14], char_feed_coil_dv, 3);
	
	//sprintf(sensor_val, "%04d", TIM1->CNT);
	//strncpy(&screen_buf.first[13], sensor_val, 4);
	
	//sprintf(sensor_val, "%03d", feed_coil_dv_lut_ptr);
	//strncpy(&screen_buf.first[13], sensor_val, 3);
	
	//sprintf(sensor_val, "%03d", feed_coil_slowdown_dv_lut_ptr);
	//strncpy(&screen_buf.first[17], sensor_val, 3);
	
	sprintf(sensor_val, "%04d", debug_take_coil_arr);
	strncpy(&screen_buf.second[5], sensor_val, 4);
	
	sprintf(char_take_coil_dv, "%03d", take_coil_dv);
	strncpy(&screen_buf.second[10], char_take_coil_dv, 3);
	
	sprintf(char_take_coil_dv, "%03d", take_coil_slowdown_dv);
	strncpy(&screen_buf.second[14], char_take_coil_dv, 3);
	
	//sprintf(sensor_val, "%04d", tim1_pulses_cnt);
	//strncpy(&screen_buf.second[13], sensor_val, 4);
	
	//sprintf(sensor_val, "%03d", take_coil_dv_lut_ptr);
	//strncpy(&screen_buf.second[13], sensor_val, 3);
	
	//sprintf(sensor_val, "%03d", take_coil_slowdown_dv_lut_ptr);
	//strncpy(&screen_buf.second[17], sensor_val, 3);
	
	//Обновляем экран
	BA63_SetPos(0, 0);
	BA63_SendString(screen_buf.first, sizeof(screen_buf.first));
	BA63_SetPos(0, 1);
	BA63_SendString(screen_buf.second, sizeof(screen_buf.second));*/
	
	//++i1;
}
