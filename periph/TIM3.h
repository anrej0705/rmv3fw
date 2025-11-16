#ifndef TIM3_H_
#define TIM3_H_

#include "stdint.h"
#include "presets.h"
#include "stdbool.h"

void setup_sensor_poll(void);
void start_sensor_poll(void);
void stop_sensor_poll(void);
void TIM3_IRQHandler(void);
//uint16_t calc_segment(uint16_t target_val);
uint16_t calc_segment(uint16_t target_val, uint16_t *current_speed, bool engine_select);

void insert_sample(uint16_t *samples_map, uint8_t *samples_ptr, uint16_t new_sample);
uint16_t get_sample(uint16_t *samples_map);

#endif