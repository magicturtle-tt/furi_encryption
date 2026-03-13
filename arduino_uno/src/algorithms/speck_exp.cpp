#include <Arduino.h>
#include <string.h>

extern "C" {
#include "speck.h"
}

#include "exp_common.h"

void run_speck_experiments(void) {
  const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  const uint8_t in_block[8] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

  uint8_t round_keys[SPECK_ROUNDS_128 * 4] = {0};
  uint8_t out_block[8] = {0};
  speck_64_128_key_schedule(key, round_keys);

  unsigned long time_trials = 0UL;
  for (int i = 0; i < 200; ++i) {
    memcpy(out_block, in_block, sizeof(out_block));
    const unsigned long start = micros();
    speck_64_128_encrypt(out_block, round_keys);
    speck_64_128_decrypt(out_block, round_keys);
    time_trials += (micros() - start);
  }

  print_experiment_row(F("speck64_128"), 16, 8, time_trials / 200UL, key[0], out_block[0],
                       F("block-ciphers/speck"));
}
