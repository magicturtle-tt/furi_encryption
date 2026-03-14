#include <Arduino.h>

#include "algorithms/experiments.h"

const uint8_t LED_PIN = LED_BUILTIN;

// UNO has very limited flash/RAM. Enable only what you need per build.
#ifndef ENABLE_TINYAES_EXP
#define ENABLE_TINYAES_EXP 1
#endif
#ifndef ENABLE_TEA_EXP
#define ENABLE_TEA_EXP 1
#endif
#ifndef ENABLE_RC5_EXP
#define ENABLE_RC5_EXP 0
#endif
#ifndef ENABLE_TWOFISH_EXP
#define ENABLE_TWOFISH_EXP 0
#endif
#ifndef ENABLE_SIMECK_EXP
#define ENABLE_SIMECK_EXP 1
#endif
#ifndef ENABLE_SIMON_EXP
#define ENABLE_SIMON_EXP 1
#endif
#ifndef ENABLE_SPECK_EXP
#define ENABLE_SPECK_EXP 1
#endif
#ifndef ENABLE_HIGHT_EXP
#define ENABLE_HIGHT_EXP 0
#endif
#ifndef ENABLE_LEA_EXP
#define ENABLE_LEA_EXP 0
#endif
#ifndef ENABLE_SEED_EXP
#define ENABLE_SEED_EXP 0
#endif

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  delay(300);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);

  Serial.println(F("algo,key_bytes,block_bytes,time_us,key0,block0,note"));
#if ENABLE_TINYAES_EXP
  run_tinyaes_experiments();
#endif
#if ENABLE_LEA_EXP
  run_lea_experiments();
#endif
#if ENABLE_TEA_EXP
  run_tea_experiments();
#endif
#if ENABLE_RC5_EXP
  run_rc5_experiments();
#endif
#if ENABLE_TWOFISH_EXP
  run_twofish_experiments();
#endif
#if ENABLE_SIMECK_EXP
  run_simeck_experiments();
#endif
#if ENABLE_SIMON_EXP
  run_simon_experiments();
#endif
#if ENABLE_SPECK_EXP
  run_speck_experiments();
#endif
#if ENABLE_HIGHT_EXP
  run_hight_experiments();
#endif
#if ENABLE_SEED_EXP
  run_seed_experiments();
#endif

  digitalWrite(LED_PIN, LOW);
  delay(1500);
}
