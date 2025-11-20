#include "global_vars.h"
#include "stm32f10x.h"
#include "presets.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool green_led_frame_change = 0;
bool ttm_engine_enable = TTM_ENGINE_DISABLE;							//Флаг активации двигателя привода ЛПМ
bool coils_engine_enable = COILS_ENGINE_DISABLE;					//Флаг активации двигателей бобин
bool film_direction = FILM_DIRECTION_DEFAULT;							//Направление протяжки
bool engine_cooler_enable = ENGINE_COOLER_DISABLE;				//Флаг вкл/выкл пропеллеров(тех самых гитлеровских)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t feed_coil_tension_sensor = UINT16_MAX >> 4;
uint16_t take_coil_tension_sensor = UINT16_MAX >> 4;
uint16_t cpu_temp_sensor = 0;
uint16_t adc_vref = 0;
uint16_t tim1_pulses_cnt;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Кешируется чтобы лишний раз не занимать процовое время на передачу по rs232 - usart
uint16_t cached_feed_coil_tension_sensor = 0;
uint16_t cached_take_coil_tension_sensor = 0;
uint16_t cached_cpu_temp_sensor = 0;
uint16_t cached_adc_vref = 0;
uint16_t cached_tim1_pulses_cnt;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//struct screen_buf;
extern struct 
{
	char first[21];						//Верхняя строка экрана
	char second[21];					//Нижняя строка экрана
	bool tx_fin;							//Флаг завершения педерачи
	uint8_t first_startpos;		//Позиция начала передачи на первой строке
	uint8_t second_startpos;	//Позиция начала педерачи на второй строке
	uint8_t first_endpos;			//Позиция остановки на первой строке
	uint8_t second_endpos;		//Позиция остановки на второй строке
}screen_buf;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t adc_buffer[4];
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Определим структуру здесь так как мы словом extern в заголовке пообещали её описать где-то там в другом месте
struct ba63_fifo_buffer ba63_fifo = {.remain = 223, .size = 224, .tx_en = 0, .stop = 0};	//Размер указываем

inline bool cmp_remain(uint8_t desired_bytes)
{
	if(desired_bytes > ba63_fifo.remain)
	{
		return 0;
	}
	return 1;
}

inline bool fifo_is_empty()
{
	if(ba63_fifo.stop == 0)
	{
		ba63_fifo.tx_en = 0;		//Вот такой корявый способ показать что всё
		return 1;								//Если буфер пустой то возвращаем 1 а иначе 0 - в буфере чото есть
	}
	return 0;
}

inline void fifo_push(char symbol)
{
	ba63_fifo.buffer[ba63_fifo.stop] = symbol;
	++ba63_fifo.stop;
}

inline void fifo_insert(char* queue, uint8_t size)
{
	if(size >= ba63_fifo.size)
	{	//Если пришло какое-то ебанутое число то даже не будем пробовать
		return;
	}
	for(uint8_t idx = 0; idx < size; ++idx)
	{	//Копируем в буфер инфу
		ba63_fifo.buffer[idx + ba63_fifo.stop] = queue[idx];
		//++ba63_fifo.stop;
		GPIOC->ODR = queue[idx];
	}
	//Обновляем позицию остановки
	ba63_fifo.stop += size;
}

inline char fifo_read()
{
	char tmp = ba63_fifo.buffer[ba63_fifo.pos];							//Читаем
	ba63_fifo.buffer[ba63_fifo.pos] = 0;										//Затираем
	if(ba63_fifo.pos == ba63_fifo.stop)
	{
		ba63_fifo.pos = 0;																		//Мы дошли до конца очереди
		ba63_fifo.stop = 0;																		//буфер пуст, поэтому указатели нужно занулить
		return tmp;
	}
	++ba63_fifo.pos;																				//Двигаем указатель
	return tmp;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Координаты отрезка плавного изменения скорости двигателя
uint16_t feed_coil_current_speed = DRIVER_DEFAULT_PWM;
uint16_t take_coil_current_speed = DRIVER_DEFAULT_PWM;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Массив с подбранными значениями из ацп для сглаживания графика скорости подающей бобины
uint16_t feed_coil_samples_map[COIL_AA_SAMPLES] = {0};
uint16_t take_coil_samples_map[COIL_AA_SAMPLES] = {0};
//Указатели на массивы со значениями для сглаживания
uint8_t feed_coil_semaples_map_ptr;
uint8_t take_coil_semaples_map_ptr;
//Флаги блокировок - двигатели остановлены?
bool feed_coil_lock = 0;
bool take_coil_lock = 0;
//Дебажные фишки - показывает выбранное значение периода ШИМ
uint16_t degub_selected_feed_coil_pwm = DRIVER_DEFAULT_PWM;
uint16_t degub_selected_take_coil_pwm = DRIVER_DEFAULT_PWM;
uint16_t debug_feed_coil_arr = 0;
uint16_t debug_take_coil_arr = 0;
uint16_t debug_feed_coil_ccr1 = 0;
uint16_t debug_take_coil_ccr1 = 0;
//Приросты скоростей двигателей
int16_t feed_coil_dv = 1;
int16_t feed_coil_slowdown_dv = 1;
int16_t take_coil_dv = 1;
int16_t take_coil_slowdown_dv = 1;
uint8_t feed_coil_dv_lut_ptr = 0;
uint8_t feed_coil_slowdown_dv_lut_ptr = 0;
uint8_t take_coil_dv_lut_ptr = 0;
uint8_t take_coil_slowdown_dv_lut_ptr = 0;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
