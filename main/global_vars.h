#ifndef GLOBAL_VARS_H_
#define GLOBAL_VARS_H_

#include "stdbool.h"
#include "stdint.h"

#include "presets.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern bool green_led_frame_change;
extern bool ttm_engine_enable;							//Флаг активации двигателя привода ЛПМ
extern bool coils_engine_enable;						//Флаг активации двигателей бобин
extern bool film_direction;									//Направление протяжки
extern bool engine_cooler_enable;						//Флаг вкл/выкл пропеллеров(тех самых гитлеровских)
extern bool ttm_engine_pwm_en;							//Вкл/выкл генерацию ШИМ для двигателя привода ЛПМ
extern bool feed_coil_engine_pwm_en;				//Вкл/выкл генерацию ШИМ для двигателя подающей бобины
extern bool take_coil_engine_pwm_en;				//Вкл/выкл генерацию ШИМ для двигателя принимающей бобины
extern bool engine_override;								//Ручное отключение двигателей через переключатель на пенели
extern bool pause_request;									//Запрос на паузу
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern uint16_t feed_coil_tension_sensor;
extern uint16_t take_coil_tension_sensor;
extern uint16_t cpu_temp_sensor;
extern uint16_t adc_vref;
extern uint16_t tim1_pulses_cnt;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Кешируется чтобы лишний раз не занимать процовое время на передачу по rs232 - usart
extern uint16_t cached_feed_coil_tension_sensor;
extern uint16_t cached_take_coil_tension_sensor;
extern uint16_t cached_cpu_temp_sensor;
extern uint16_t cached_adc_vref;
extern uint16_t cached_tim1_pulses_cnt;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Адрес буфера АЦП
extern uint16_t adc_buffer[4];
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Типа FIFO, в stm32f4 тем не менее FIFO есть но он хуйня))
struct ba63_fifo_buffer
{
	volatile char buffer[224];
	volatile char pos;
	volatile char remain;
	volatile uint8_t stop;
	volatile uint8_t size;
	volatile bool tx_en;
};	//Определяем структуру, можно взять любое долбанутое название так как оно светиться нигде не будет

extern struct ba63_fifo_buffer ba63_fifo;		//А вот здесь уже будет экземпляр через который будет идти доступ из различных файлов, его надо по-понятнее назвать
//В .c не забываем определить

bool cmp_remain(uint8_t desired_bytes);			//Возвращает 0 если желаемое количество байт не поместится, 1 если поместится
bool fifo_is_empty();												//1 если пустой
void fifo_insert(char* queue, uint8_t size);//Вставка массива и запуск педерачи если она уже не ведётся
char fifo_read();														//Смотрим что насрано в буфер и возвращаем 1(один(one)) ківш із смердючим гівном
void fifo_push(char symbol);								//Вставка символа в очередь
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Координаты отрезка плавного изменения скорости двигателя
//  ---|----------------------|------
//    x0                     x1
//
//  ---|<<<<<<<<<<<<<<<<<<<<<<|------    x0 < x1
//  
//  ---|>>>>>>>>>>>>>>>>>>>>>>|------    x0 > x1
//
extern uint16_t feed_coil_current_speed;									//X0 подающей бобины, X1 забирается с датчика
extern uint16_t take_coil_current_speed;									//X0 принимающей бобины, X1 забирается с датчика
extern uint16_t ttm_current_speed;												//Здесь по другому. Просто скорость двигателя
extern uint16_t ttm_target_speed;													//Скорость, которой нужно достичь, прирост одинаковый
extern int8_t ttm_speed_dv;																//Прирост скорости
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Массив с подбранными значениями из ацп для сглаживания графика скорости подающей бобины
extern uint16_t feed_coil_samples_map[COIL_AA_SAMPLES];
extern uint16_t take_coil_samples_map[COIL_AA_SAMPLES];
//Указатели на массивы со значениями для сглаживания
extern uint8_t feed_coil_semaples_map_ptr;
extern uint8_t take_coil_semaples_map_ptr;
//Флаги блокировок - двигатели остановлены?
extern bool feed_coil_lock;
extern bool take_coil_lock;
//Дебажные фишки - показывает выбранное значение периода ШИМ
extern uint16_t degub_selected_feed_coil_pwm;
extern uint16_t degub_selected_take_coil_pwm;
extern uint16_t debug_feed_coil_arr;
extern uint16_t debug_take_coil_arr;
extern uint16_t debug_feed_coil_ccr1;
extern uint16_t debug_take_coil_ccr1;
//Приросты скоростей двигателей
extern int16_t feed_coil_dv;
extern int16_t feed_coil_slowdown_dv;
extern int16_t take_coil_dv;
extern int16_t take_coil_slowdown_dv;
extern uint8_t feed_coil_dv_lut_ptr;
extern uint8_t feed_coil_slowdown_dv_lut_ptr;
extern uint8_t take_coil_dv_lut_ptr;
extern uint8_t take_coil_slowdown_dv_lut_ptr;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Управление режимами кнопок и отображения
extern uint8_t ui_code;												//Переключает функции кнопок и меняет отображаемую инфу в зависимости от цифры
extern uint8_t cached_ui_code;								//Предотвращает загрузку буфера бесполезными перерисовками. Сбрасывать при любом обновлении
extern uint8_t key_code;											//Номер нажатой кнопки
extern char debug_key_code_char[3];						//
extern bool key_lock;													//Защита от множественных срабатываний
extern bool key_proced;												//Кнопка обработана
extern bool key_start_lock;										//Защита от множественных срабатываний
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Значения по каждой компоненте цвета RGBY
extern uint8_t level_red;
extern uint8_t level_green;
extern uint8_t level_blue;
extern uint8_t level_yellow;
extern uint16_t level_tmp;										//Для упрощения проверки на переполнение
extern uint8_t dig_num;												//Разряд - 0: 1,2,5 1: 10,20,50 2: 100,200,500
extern uint8_t dig_tmp;												//Может прилететь 255, поэтому сначала надо проверить а это лучше проводить здесь
extern char char_level_red[4];
extern char char_level_green[4];
extern char char_level_blue[4];
extern char char_level_yellow[4];
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Счётчик кадров
extern uint16_t frames_counter;								//Счётчик кадров
extern char char_frames_counter[6];						//Счётчик кадров для кожанных ублюдков
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif