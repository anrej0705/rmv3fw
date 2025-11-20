#ifndef SPEED_LUT_H_
#define SPEED_LUT_H_

#include "stdint.h"

#define COIL_ACCELERATION_LUT_SIZE			20 * 100

extern const uint16_t coil_acceleration_lut[COIL_ACCELERATION_LUT_SIZE];
extern const uint16_t feed_coil_acceleration_lut[COIL_ACCELERATION_LUT_SIZE];
extern const int16_t dv[COIL_ACCELERATION_LUT_SIZE/100];
extern const int16_t slowdown_dv[COIL_ACCELERATION_LUT_SIZE/100];

#endif
