#ifndef SPEED_CTRL_H_
#define SPEED_CTRL_H_

#include "stdint.h"
#include "stdbool.h"

//Расчёт отрезка
uint16_t calc_segment(uint16_t target_val, uint16_t *current_speed, bool engine_select);

//Вставка показателя с АЦП в таблицу для сглаживающей фильтрации
void insert_sample(uint16_t *samples_map, uint8_t *samples_ptr, uint16_t new_sample);

//Получение сглаженного значения
uint16_t get_sample(uint16_t *samples_map);

#endif
