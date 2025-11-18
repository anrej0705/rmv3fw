#ifndef SPEED_LUT_H_
#define SPEED_LUT_H_

#include "stdint.h"

#define COIL_ACCELERATION_LUT_SIZE			2000

extern const uint16_t coil_acceleration_lut[COIL_ACCELERATION_LUT_SIZE];
extern const uint16_t feed_coil_acceleration_lut[COIL_ACCELERATION_LUT_SIZE];

#endif
