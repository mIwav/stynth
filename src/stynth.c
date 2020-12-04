#define ARM_MATH_CM7
#include "arm_math.h"
#include "stynth.h"

void Init(Stynth *stynth) {
  for (uint8_t i = 0; i < OP_COUNT; ++i)
    InitOperator(&stynth->ops[i]);
}

void InitOperator(Operator *op) {
  op->freq = 1000.f;
  op->accu = 0.f;
  op->value = 0.f;
}

void genSine(Sine *sinGen, uint16_t *buffer) {
  float f = sinGen->freq;
  float phaseAcc = sinGen->phase;
  for (uint8_t i = 0; i < SAMPLES; ++i) {
    buffer[i] = scaleFixedDac(arm_sin_f32(phaseAcc));
    phaseAcc += TWOPI * f * invSamplerate;
    if(phaseAcc > TWOPI)
    	phaseAcc -=  TWOPI;
  }
  sinGen->phase = phaseAcc;
}

void genSamplerate(uint16_t *buffer)
{
  uint16_t toggle = 0u;
  for (uint8_t i = 0; i < SAMPLES; ++i) {
    buffer[i] = toggle * 4095u;
    toggle = (toggle == 0u) ? 1u : 0u;
  }
}

