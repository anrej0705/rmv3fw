#ifndef TIM17_H_
#define TIM17_H_

#include "stdint.h"

void setup_feed_coil(void);
void start_feed_coil(void);
void stop_feed_coil(void);
void set_speed_feed_coil(uint16_t speed);

#endif
