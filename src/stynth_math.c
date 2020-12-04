
#include <stdint.h>
#include "stynth_math.h"


const float sin_table[SIN_TABLE_SIZE] = {0.f};


float sin_lut_nn(float x) {
  float in = x * 0.159151943092f; // 1.f / (2.f * pi)
  uint32_t n = (int32_t)in;
  in = in - (float)n;

  uint32_t index = SIN_TABLE_SIZE * in;

  return sin_table[index + 1];
}

