#ifndef _STYNTH_H_
#define _STYNTH_H

#include <stdint.h>

#include "stynth_math.h"

#define SAMPLES (64)
#define SAMPLERATE (48000.f)
#define OP_COUNT (6)


static const float invSamplerate = 1.f / SAMPLERATE;

typedef struct sine {
  float phase;
  float freq;
} Sine;

typedef struct Operator {
  float freq;
  float accu;
  float value;
} Operator;

typedef struct Stynth {
  Operator ops[OP_COUNT];
} Stynth;

void Init(Stynth *stynth);
void InitOperator(Operator *op);

void genSine(Sine *sinGen, uint16_t *buffer);
void genSamplerate(uint16_t *buffer);

#endif

