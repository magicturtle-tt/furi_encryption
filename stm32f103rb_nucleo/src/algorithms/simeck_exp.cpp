#include <Arduino.h>

extern "C" {
#include "../include/Simeck/c/simeck64.h"
}

#include "exp_common.h"

namespace {

static inline uint32_t rotl32(uint32_t x, uint8_t r) {
  return (x << r) | (x >> (32U - r));
}

static inline uint32_t simeck_f(uint32_t x) {
  return (x & rotl32(x, 5)) ^ rotl32(x, 1);
}

static void simeck64_128_expand_keys(const uint32_t master_key[4], uint32_t round_keys[44]) {
  uint32_t keys[4] = {master_key[0], master_key[1], master_key[2], master_key[3]};
  uint32_t constant = 0xFFFFFFFCUL;
  uint64_t sequence = 0x938BCA3083FULL;

  for (uint8_t i = 0; i < 44; ++i) {
    round_keys[i] = keys[0];

    constant = (constant & 0xFFFFFFFCUL) | static_cast<uint32_t>(sequence & 1ULL);
    sequence >>= 1;

    uint32_t tmp = keys[1];
    keys[1] = simeck_f(keys[1]) ^ keys[0] ^ constant;
    keys[0] = tmp;

    tmp = keys[1];
    keys[1] = keys[2];
    keys[2] = keys[3];
    keys[3] = tmp;
  }
}

static void simeck64_128_decrypt(const uint32_t master_key[4],
                                 const uint32_t ciphertext[2],
                                 uint32_t plaintext[2]) {
  uint32_t round_keys[44];
  simeck64_128_expand_keys(master_key, round_keys);

  uint32_t left = ciphertext[1];
  uint32_t right = ciphertext[0];
  for (int i = 43; i >= 0; --i) {
    const uint32_t prev_left = right;
    const uint32_t prev_right = simeck_f(prev_left) ^ left ^ round_keys[i];
    left = prev_left;
    right = prev_right;
  }

  plaintext[0] = right;
  plaintext[1] = left;
}

static bool simeck_self_test() {
  const uint32_t key[4] = {0x03020100, 0x0b0a0908, 0x13121110, 0x1b1a1918};
  const uint32_t plain[2] = {0x20646e75, 0x656b696c};
  const uint32_t expected[2] = {0x5f7ab7ed, 0x45ce6902};

  uint32_t block[2] = {plain[0], plain[1]};
  simeck_64_128(key, block, block);
  if (block[0] != expected[0] || block[1] != expected[1]) {
    return false;
  }

  simeck64_128_decrypt(key, block, block);
  return (block[0] == plain[0]) && (block[1] == plain[1]);
}

}  // namespace

void run_simeck_experiments(void) {
  const uint32_t key[4] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c};
  const uint32_t in_block[2] = {0x13121110, 0x17161514};
  uint32_t out_block[2] = {in_block[0], in_block[1]};

  if (!simeck_self_test()) {
    print_unsupported_algo(F("simeck64_128"), 16, 8, static_cast<uint8_t>(key[0]),
                           static_cast<uint8_t>(in_block[0]), F("self-test failed"));
    return;
  }

  unsigned long time_trials = 0UL;
  for (int i = 0; i < 200; ++i) {
    out_block[0] = in_block[0];
    out_block[1] = in_block[1];
    const unsigned long start = micros();
    simeck_64_128(key, out_block, out_block);
    simeck64_128_decrypt(key, out_block, out_block);
    time_trials += (micros() - start);
  }

  print_experiment_row(F("simeck64_128"), 16, 8, time_trials / 200UL, static_cast<uint8_t>(key[0]),
                       static_cast<uint8_t>(out_block[0]),
                       F("bozhu/Simeck"));
}
