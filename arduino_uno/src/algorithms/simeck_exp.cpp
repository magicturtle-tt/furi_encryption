#include <Arduino.h>

extern "C" {
#include "../include/Simeck/c/simeck64.h"
}

#include "exp_common.h"

void run_simeck_experiments(void) {
  const uint32_t key[4] = {0x03020100, 0x0b0a0908, 0x13121110, 0x1b1a1918};
  const uint32_t in_block[2] = {0x20646e75, 0x656b696c};
  uint32_t out_block[2] = {0, 0};

  unsigned long time_trials = 0UL;
  for (int i = 0; i < 200; ++i) {
    out_block[0] = in_block[0];
    out_block[1] = in_block[1];
    const unsigned long start = micros();
    simeck_64_128(key, out_block, out_block);
    simeck_64_128(key, out_block, out_block);
    time_trials += (micros() - start);
  }

  const uint8_t* out_bytes = reinterpret_cast<const uint8_t*>(out_block);
  print_experiment_row(F("simeck64_128"), 16, 8, time_trials / 200UL, 0x00, out_bytes[0],
                       F("bozhu/Simeck"));
}
