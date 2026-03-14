#include <Arduino.h>
#include <stdint.h>

#include "exp_common.h"

static inline uint32_t rotl32(uint32_t x, uint32_t r) {
  r &= 31U;
  return (x << r) | (x >> (32U - r));
}

static inline uint32_t rotr32(uint32_t x, uint32_t r) {
  r &= 31U;
  return (x >> r) | (x << (32U - r));
}

static inline uint32_t load32le(const uint8_t* p) {
  return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

static inline void store32le(uint8_t* p, uint32_t v) {
  p[0] = static_cast<uint8_t>(v);
  p[1] = static_cast<uint8_t>(v >> 8);
  p[2] = static_cast<uint8_t>(v >> 16);
  p[3] = static_cast<uint8_t>(v >> 24);
}

static void rc5_32_12_16_key_schedule(const uint8_t key[16], uint32_t S[26]) {
  const uint32_t P = 0xB7E15163UL;
  const uint32_t Q = 0x9E3779B9UL;
  uint32_t L[4] = {0, 0, 0, 0};

  for (int i = 15; i >= 0; --i) {
    L[i / 4] = (L[i / 4] << 8) + key[i];
  }

  S[0] = P;
  for (int i = 1; i < 26; ++i) {
    S[i] = S[i - 1] + Q;
  }

  uint32_t A = 0, B = 0;
  int i = 0, j = 0;
  for (int k = 0; k < 78; ++k) {
    A = S[i] = rotl32(S[i] + A + B, 3);
    B = L[j] = rotl32(L[j] + A + B, A + B);
    i = (i + 1) % 26;
    j = (j + 1) % 4;
  }
}

static void rc5_encrypt_block(const uint32_t S[26], const uint8_t in[8], uint8_t out[8]) {
  uint32_t A = load32le(in) + S[0];
  uint32_t B = load32le(in + 4) + S[1];
  for (int i = 1; i <= 12; ++i) {
    A = rotl32(A ^ B, B) + S[2 * i];
    B = rotl32(B ^ A, A) + S[2 * i + 1];
  }
  store32le(out, A);
  store32le(out + 4, B);
}

static void rc5_decrypt_block(const uint32_t S[26], const uint8_t in[8], uint8_t out[8]) {
  uint32_t A = load32le(in);
  uint32_t B = load32le(in + 4);
  for (int i = 12; i >= 1; --i) {
    B = rotr32(B - S[2 * i + 1], A) ^ A;
    A = rotr32(A - S[2 * i], B) ^ B;
  }
  B -= S[1];
  A -= S[0];
  store32le(out, A);
  store32le(out + 4, B);
}

void run_rc5_experiments(void) {
  const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  const uint8_t in_block[8] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  uint32_t S[26] = {0};
  rc5_32_12_16_key_schedule(key, S);

  uint8_t out_block[8] = {0};
  unsigned long time_trials = 0UL;
  for (int i = 0; i < 200; ++i) {
    const unsigned long start = micros();
    rc5_encrypt_block(S, in_block, out_block);
    rc5_decrypt_block(S, out_block, out_block);
    time_trials += (micros() - start);
  }

  print_experiment_row(F("rc5_ref"), 16, 8, time_trials / 200UL, key[0], out_block[0],
                       F("RC5-32/12/16 reference C"));
}
