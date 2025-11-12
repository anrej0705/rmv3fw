#ifndef TIM15_H_
#define TIM15_H_

#include "stdint.h"

void setup_ttm(void);
void start_ttm(void);
void stop_ttm(void);
void set_speed_ttm(uint16_t speed);

#endif
