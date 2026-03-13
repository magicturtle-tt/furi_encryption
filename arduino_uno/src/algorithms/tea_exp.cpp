#include <Arduino.h>
#include <stdint.h>

#include "exp_common.h"

static inline uint32_t load32be(const uint8_t* p) {
  return (static_cast<uint32_t>(p[0]) << 24) | (static_cast<uint32_t>(p[1]) << 16) |
         (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
}

static inline void store32be(uint8_t* p, uint32_t v) {
  p[0] = static_cast<uint8_t>(v >> 24);
  p[1] = static_cast<uint8_t>(v >> 16);
  p[2] = static_cast<uint8_t>(v >> 8);
  p[3] = static_cast<uint8_t>(v);
}

static void tea_encrypt_block(const uint8_t key[16], const uint8_t in[8], uint8_t out[8]) {
  uint32_t v0 = load32be(in);
  uint32_t v1 = load32be(in + 4);
  const uint32_t k0 = load32be(key);
  const uint32_t k1 = load32be(key + 4);
  const uint32_t k2 = load32be(key + 8);
  const uint32_t k3 = load32be(key + 12);

  uint32_t sum = 0;
  const uint32_t delta = 0x9E3779B9UL;
  for (int i = 0; i < 32; ++i) {
    sum += delta;
    v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
    v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
  }

  store32be(out, v0);
  store32be(out + 4, v1);
}

static void tea_decrypt_block(const uint8_t key[16], const uint8_t in[8], uint8_t out[8]) {
  uint32_t v0 = load32be(in);
  uint32_t v1 = load32be(in + 4);
  const uint32_t k0 = load32be(key);
  const uint32_t k1 = load32be(key + 4);
  const uint32_t k2 = load32be(key + 8);
  const uint32_t k3 = load32be(key + 12);

  uint32_t sum = 0xC6EF3720UL;
  const uint32_t delta = 0x9E3779B9UL;
  for (int i = 0; i < 32; ++i) {
    v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
    v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
    sum -= delta;
  }

  store32be(out, v0);
  store32be(out + 4, v1);
}

void run_tea_experiments(void) {
  const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  const uint8_t in_block[8] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  uint8_t out_block[8] = {0};

  unsigned long time_trials = 0UL;
  for (int i = 0; i < 200; ++i) {
    const unsigned long start = micros();
    tea_encrypt_block(key, in_block, out_block);
    tea_decrypt_block(key, out_block, out_block);
    time_trials += (micros() - start);
  }

  print_experiment_row(F("tea_ref"), 16, 8, time_trials / 200UL, key[0], out_block[0],
                       F("TEA reference C"));
}
