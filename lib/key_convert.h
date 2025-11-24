#ifndef KEY_CONVERT_H_
#define KEY_CONVERT_H_

#include "stdint.h"

uint8_t convert_key(uint8_t key_code);
uint16_t set_zero(uint8_t input, uint16_t grade);
uint8_t convert_level(uint8_t prev_level, uint8_t key_code);

#endif
