#include "global_vars.h"
#include "stm32f10x.h"
#include "presets.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Флаги светодиодов
bool green_led_frame_change = 0;													//Светодиод, который зажигается при работе привода ЛПМ
bool green_led_sublight_en = 0;														//Лампочка показывающая что подсветка работает
bool yellow_led_feed_coil = 0;														//Светодиод показывающий работу раздающей бобины
bool yellow_led_take_coil = 0;														//То же самое, но с принимающей бобины
bool red_led_alarm = 0;																		//Лампочка тревоги, которая зажигается если возникают какие-то проблемы в работе сканера
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ttm_engine_enable = 1;																//Флаг активации двигателя привода ЛПМ
bool coils_engine_enable = 1;															//Флаг активации двигателей бобин
bool film_direction = FILM_DIRECTION_DEFAULT;							//Направление протяжки
//bool engine_cooler_enable = ENGINE_COOLER_DISABLE;				//Флаг вкл/выкл пропеллеров(тех самых гитлеровских)
bool ttm_engine_pwm_en = 0;																//Вкл/выкл генерацию ШИМ для двигателя привода ЛПМ
bool feed_coil_engine_pwm_en = 0;													//Вкл/выкл генерацию ШИМ для двигателя подающей бобины
bool take_coil_engine_pwm_en = 0;													//Вкл/выкл генерацию ШИМ для двигателя принимающей бобины
bool engine_override = 1;																	//Срабатывает когда пользователь выключил двигатели переключателем
bool pause_request = 0;																		//Запрос на паузу
bool led_enabled = 1;																			//Вкл/выкл подсветки кадра - защита красителей плёнки. Включается автоматически при выходе из паузы
//bool led_enabled_lock = 1;																//Флаг защиты от повторного срабатывания
bool service_lane_lock = 0;																//Флаг отключающий первый слева вертикальный ряд кнопок по время процесса сканирования
bool engines_enabled = 0;																	//Предотвращает преждевременный запуск двигателей, например на этапе настройки подсветки
bool camera_shot = 0;																			//Сигнал на затвор камеры
bool camera_busy = 0;																			//Флажок "камера занята"
//bool scan_enabled = 0;																		//Флаг запущенного сканирования
bool high_speed = 0;																			//Флажок переключения низкой и высокой скорости
bool take_coil_freeze = 1;																//Флажок отключения двигателя принимающей бобины
bool take_coil_freeze_lock = 1;														//Защита от повторного срабатывания
bool drop_detect = 0;																			//Бит срабатывания датчика выпадения плёни
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t feed_coil_tension_sensor = UINT16_MAX >> 4;
uint16_t take_coil_tension_sensor = UINT16_MAX >> 4;
uint16_t cpu_temp_sensor = 0;
uint16_t adc_vref = 0;
uint16_t tim1_pulses_cnt;
uint16_t callback_sensor = 0;
uint16_t led_calibration = 0;
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
uint16_t adc_buffer[6];
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Определим структуру здесь так как мы словом extern в заголовке пообещали её описать где-то там в другом месте
struct ba63_fifo_buffer ba63_fifo = {.remain = 223, .size = 224, .tx_en = 0, .stop = 0};	//Размер указываем

inline bool cmp_remain(uint8_t desired_bytes)
{
	if(desired_bytes >= ba63_fifo.remain)
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
uint16_t ttm_current_speed = DRIVER_DEFAULT_PWM;
uint16_t ttm_target_speed = DRIVER_DEFAULT_PWM;
int8_t ttm_speed_dv = 1;
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
//Управление режимами кнопок и отображения
uint8_t ui_code = 0;
uint8_t cached_ui_code = 0;
uint8_t key_code = 0;
char debug_key_code_char[3] = { 0x00, 0x00, 0x00 };
bool key_lock = 0;
bool key_proced = 0;
bool key_start_lock = 0;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Значения по каждой компоненте цвета RGBY
uint8_t level_red = 0;
uint8_t level_green = 0;
uint8_t level_blue = 0;
uint8_t level_yellow = 0;
uint16_t level_tmp = 0;
uint8_t dig_num = 0;
uint8_t dig_tmp = 0;
char char_level_red[4] = {0};
char char_level_green[4] = {0};
char char_level_blue[4] = {0};
char char_level_yellow[4] = {0};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Счётчик кадров
uint16_t frames_counter = 0;
char char_frames_counter[6] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t def = 0;
uint16_t xt_shutter_preset[14] = { 
	681,		//Выдержка 1/1
	398,		//Выдержка 1/2
	275,		//Выдержка 1/4
	193,		//Выдержка 1/8
	121,		//Выдержка 1/15
	98,			//Выдержка 1/30
	86,			//Выдержка 1/60
	79,			//Выдержка 1/125
	77,			//Выдержка 1/250
	73,			//Выдержка 1/500
	72,			//Выдержка 1/1000
	70,			//Выдержка 1/2000
	67,			//Выдержка 1/4000
	65			//Выдержка 1/8000
};
uint8_t xt_shutter_mode = 0;
uint16_t xt_shutter_delay = 0;
