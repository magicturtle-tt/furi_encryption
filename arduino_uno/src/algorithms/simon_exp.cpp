#include <Arduino.h>
#include <string.h>

extern "C" {
#include "simon.h"
}

#include "exp_common.h"

namespace {

bool simon_self_test() {
  const uint8_t tv_key[16] = {0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
                              0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1A, 0x1B};
  const uint8_t tv_pt[8] = {0x75, 0x6E, 0x64, 0x20, 0x6C, 0x69, 0x6B, 0x65};
  const uint8_t tv_ct[8] = {0x7A, 0xA0, 0xDF, 0xB9, 0x20, 0xFC, 0xC8, 0x44};

  uint8_t round_keys[SIMON_ROUNDS_128 * 4] = {0};
  uint8_t block[8] = {0};
  simon_64_128_key_schedule(tv_key, round_keys);

  memcpy(block, tv_pt, sizeof(block));
  simon_64_128_encrypt(block, round_keys);
  if (memcmp(block, tv_ct, sizeof(block)) != 0) {
    return false;
  }

  simon_64_128_decrypt(block, round_keys);
  return memcmp(block, tv_pt, sizeof(block)) == 0;
}

}  // namespace

void run_simon_experiments(void) {
  const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  const uint8_t in_block[8] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

  if (!simon_self_test()) {
    print_unsupported_algo(F("simon64_128"), 16, 8, key[0], in_block[0],
                           F("self-test failed"));
    return;
  }

  uint8_t round_keys[SIMON_ROUNDS_128 * 4] = {0};
  uint8_t out_block[8] = {0};
  simon_64_128_key_schedule(key, round_keys);

  unsigned long time_trials = 0UL;
  for (int i = 0; i < 200; ++i) {
    memcpy(out_block, in_block, sizeof(out_block));
    const unsigned long start = micros();
    simon_64_128_encrypt(out_block, round_keys);
    simon_64_128_decrypt(out_block, round_keys);
    time_trials += (micros() - start);
  }

  print_experiment_row(F("simon64_128"), 16, 8, time_trials / 200UL, key[0], out_block[0],
                       F("block-ciphers/simon"));
}
