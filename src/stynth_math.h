#ifndef _STYNTH_MATH_H_
#define _STYNTH_MATH_H_

#include <stdint.h>

// #define PI (3.141592654f)
#define TWOPI (6.283185307f)
#define SIN_TABLE_SIZE (127)

float sin_lut_nn(float x);

static inline uint16_t scaleFixedDac(float x)
{
 return (uint16_t) ((x + 1.f) * 2047.f);
};

#endif
