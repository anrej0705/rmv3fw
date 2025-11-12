#ifndef TIM16_H_
#define TIM16_H_

#include "stdint.h"

void setup_take_coil(void);
void start_take_coil(void);
void stop_take_coil(void);
void set_speed_take_coil(uint16_t speed);

#endif
