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

#endif