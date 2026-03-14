#include <Arduino.h>
#include <string.h>

#include "exp_common.h"

namespace {

const uint32_t LEA_DELTA[4][36] = {
    {0xc3efe9db, 0x87dfd3b7, 0x0fbfa76f, 0x1f7f4ede, 0x3efe9dbc, 0x7dfd3b78, 0xfbfa76f0, 0xf7f4ede1,
     0xefe9dbc3, 0xdfd3b787, 0xbfa76f0f, 0x7f4ede1f, 0xfe9dbc3e, 0xfd3b787d, 0xfa76f0fb, 0xf4ede1f7,
     0xe9dbc3ef, 0xd3b787df, 0xa76f0fbf, 0x4ede1f7f, 0x9dbc3efe, 0x3b787dfd, 0x76f0fbfa, 0xede1f7f4,
     0xdbc3efe9, 0xb787dfd3, 0x6f0fbfa7, 0xde1f7f4e, 0xbc3efe9d, 0x787dfd3b, 0xf0fbfa76, 0xe1f7f4ed,
     0xc3efe9db, 0x87dfd3b7, 0x0fbfa76f, 0x1f7f4ede},
    {0x44626b02, 0x88c4d604, 0x1189ac09, 0x23135812, 0x4626b024, 0x8c4d6048, 0x189ac091, 0x31358122,
     0x626b0244, 0xc4d60488, 0x89ac0911, 0x13581223, 0x26b02446, 0x4d60488c, 0x9ac09118, 0x35812231,
     0x6b024462, 0xd60488c4, 0xac091189, 0x58122313, 0xb0244626, 0x60488c4d, 0xc091189a, 0x81223135,
     0x0244626b, 0x0488c4d6, 0x091189ac, 0x12231358, 0x244626b0, 0x488c4d60, 0x91189ac0, 0x22313581,
     0x44626b02, 0x88c4d604, 0x1189ac09, 0x23135812},
    {0x79e27c8a, 0xf3c4f914, 0xe789f229, 0xcf13e453, 0x9e27c8a7, 0x3c4f914f, 0x789f229e, 0xf13e453c,
     0xe27c8a79, 0xc4f914f3, 0x89f229e7, 0x13e453cf, 0x27c8a79e, 0x4f914f3c, 0x9f229e78, 0x3e453cf1,
     0x7c8a79e2, 0xf914f3c4, 0xf229e789, 0xe453cf13, 0xc8a79e27, 0x914f3c4f, 0x229e789f, 0x453cf13e,
     0x8a79e27c, 0x14f3c4f9, 0x29e789f2, 0x53cf13e4, 0xa79e27c8, 0x4f3c4f91, 0x9e789f22, 0x3cf13e45,
     0x79e27c8a, 0xf3c4f914, 0xe789f229, 0xcf13e453},
    {0x78df30ec, 0xf1be61d8, 0xe37cc3b1, 0xc6f98763, 0x8df30ec7, 0x1be61d8f, 0x37cc3b1e, 0x6f98763c,
     0xdf30ec78, 0xbe61d8f1, 0x7cc3b1e3, 0xf98763c6, 0xf30ec78d, 0xe61d8f1b, 0xcc3b1e37, 0x98763c6f,
     0x30ec78df, 0x61d8f1be, 0xc3b1e37c, 0x8763c6f9, 0x0ec78df3, 0x1d8f1be6, 0x3b1e37cc, 0x763c6f98,
     0xec78df30, 0xd8f1be61, 0xb1e37cc3, 0x63c6f987, 0xc78df30e, 0x8f1be61d, 0x1e37cc3b, 0x3c6f9876,
     0x78df30ec, 0xf1be61d8, 0xe37cc3b1, 0xc6f98763}
};

static inline uint32_t rotl32(uint32_t x, uint32_t n) {
  n &= 31U;
  return (x << n) | (x >> (32U - n));
}

static inline uint32_t rotr32(uint32_t x, uint32_t n) {
  n &= 31U;
  return (x >> n) | (x << (32U - n));
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

void lea128_set_key(const uint8_t key_bytes[16], uint32_t rk[144]) {
  const uint32_t k0 = load32le(key_bytes + 0);
  const uint32_t k1 = load32le(key_bytes + 4);
  const uint32_t k2 = load32le(key_bytes + 8);
  const uint32_t k3 = load32le(key_bytes + 12);

  rk[  0] = rotl32(k0 + LEA_DELTA[0][ 0], 1);
  rk[  6] = rotl32(rk[  0] + LEA_DELTA[1][ 1], 1);
  rk[ 12] = rotl32(rk[  6] + LEA_DELTA[2][ 2], 1);
  rk[ 18] = rotl32(rk[ 12] + LEA_DELTA[3][ 3], 1);
  rk[ 24] = rotl32(rk[ 18] + LEA_DELTA[0][ 4], 1);
  rk[ 30] = rotl32(rk[ 24] + LEA_DELTA[1][ 5], 1);
  rk[ 36] = rotl32(rk[ 30] + LEA_DELTA[2][ 6], 1);
  rk[ 42] = rotl32(rk[ 36] + LEA_DELTA[3][ 7], 1);
  rk[ 48] = rotl32(rk[ 42] + LEA_DELTA[0][ 8], 1);
  rk[ 54] = rotl32(rk[ 48] + LEA_DELTA[1][ 9], 1);
  rk[ 60] = rotl32(rk[ 54] + LEA_DELTA[2][10], 1);
  rk[ 66] = rotl32(rk[ 60] + LEA_DELTA[3][11], 1);
  rk[ 72] = rotl32(rk[ 66] + LEA_DELTA[0][12], 1);
  rk[ 78] = rotl32(rk[ 72] + LEA_DELTA[1][13], 1);
  rk[ 84] = rotl32(rk[ 78] + LEA_DELTA[2][14], 1);
  rk[ 90] = rotl32(rk[ 84] + LEA_DELTA[3][15], 1);
  rk[ 96] = rotl32(rk[ 90] + LEA_DELTA[0][16], 1);
  rk[102] = rotl32(rk[ 96] + LEA_DELTA[1][17], 1);
  rk[108] = rotl32(rk[102] + LEA_DELTA[2][18], 1);
  rk[114] = rotl32(rk[108] + LEA_DELTA[3][19], 1);
  rk[120] = rotl32(rk[114] + LEA_DELTA[0][20], 1);
  rk[126] = rotl32(rk[120] + LEA_DELTA[1][21], 1);
  rk[132] = rotl32(rk[126] + LEA_DELTA[2][22], 1);
  rk[138] = rotl32(rk[132] + LEA_DELTA[3][23], 1);

  rk[  1] = rk[  3] = rk[  5] = rotl32(k1 + LEA_DELTA[0][ 1], 3);
  rk[  7] = rk[  9] = rk[ 11] = rotl32(rk[  1] + LEA_DELTA[1][ 2], 3);
  rk[ 13] = rk[ 15] = rk[ 17] = rotl32(rk[  7] + LEA_DELTA[2][ 3], 3);
  rk[ 19] = rk[ 21] = rk[ 23] = rotl32(rk[ 13] + LEA_DELTA[3][ 4], 3);
  rk[ 25] = rk[ 27] = rk[ 29] = rotl32(rk[ 19] + LEA_DELTA[0][ 5], 3);
  rk[ 31] = rk[ 33] = rk[ 35] = rotl32(rk[ 25] + LEA_DELTA[1][ 6], 3);
  rk[ 37] = rk[ 39] = rk[ 41] = rotl32(rk[ 31] + LEA_DELTA[2][ 7], 3);
  rk[ 43] = rk[ 45] = rk[ 47] = rotl32(rk[ 37] + LEA_DELTA[3][ 8], 3);
  rk[ 49] = rk[ 51] = rk[ 53] = rotl32(rk[ 43] + LEA_DELTA[0][ 9], 3);
  rk[ 55] = rk[ 57] = rk[ 59] = rotl32(rk[ 49] + LEA_DELTA[1][10], 3);
  rk[ 61] = rk[ 63] = rk[ 65] = rotl32(rk[ 55] + LEA_DELTA[2][11], 3);
  rk[ 67] = rk[ 69] = rk[ 71] = rotl32(rk[ 61] + LEA_DELTA[3][12], 3);
  rk[ 73] = rk[ 75] = rk[ 77] = rotl32(rk[ 67] + LEA_DELTA[0][13], 3);
  rk[ 79] = rk[ 81] = rk[ 83] = rotl32(rk[ 73] + LEA_DELTA[1][14], 3);
  rk[ 85] = rk[ 87] = rk[ 89] = rotl32(rk[ 79] + LEA_DELTA[2][15], 3);
  rk[ 91] = rk[ 93] = rk[ 95] = rotl32(rk[ 85] + LEA_DELTA[3][16], 3);
  rk[ 97] = rk[ 99] = rk[101] = rotl32(rk[ 91] + LEA_DELTA[0][17], 3);
  rk[103] = rk[105] = rk[107] = rotl32(rk[ 97] + LEA_DELTA[1][18], 3);
  rk[109] = rk[111] = rk[113] = rotl32(rk[103] + LEA_DELTA[2][19], 3);
  rk[115] = rk[117] = rk[119] = rotl32(rk[109] + LEA_DELTA[3][20], 3);
  rk[121] = rk[123] = rk[125] = rotl32(rk[115] + LEA_DELTA[0][21], 3);
  rk[127] = rk[129] = rk[131] = rotl32(rk[121] + LEA_DELTA[1][22], 3);
  rk[133] = rk[135] = rk[137] = rotl32(rk[127] + LEA_DELTA[2][23], 3);
  rk[139] = rk[141] = rk[143] = rotl32(rk[133] + LEA_DELTA[3][24], 3);

  rk[  2] = rotl32(k2 + LEA_DELTA[0][ 2], 6);
  rk[  8] = rotl32(rk[  2] + LEA_DELTA[1][ 3], 6);
  rk[ 14] = rotl32(rk[  8] + LEA_DELTA[2][ 4], 6);
  rk[ 20] = rotl32(rk[ 14] + LEA_DELTA[3][ 5], 6);
  rk[ 26] = rotl32(rk[ 20] + LEA_DELTA[0][ 6], 6);
  rk[ 32] = rotl32(rk[ 26] + LEA_DELTA[1][ 7], 6);
  rk[ 38] = rotl32(rk[ 32] + LEA_DELTA[2][ 8], 6);
  rk[ 44] = rotl32(rk[ 38] + LEA_DELTA[3][ 9], 6);
  rk[ 50] = rotl32(rk[ 44] + LEA_DELTA[0][10], 6);
  rk[ 56] = rotl32(rk[ 50] + LEA_DELTA[1][11], 6);
  rk[ 62] = rotl32(rk[ 56] + LEA_DELTA[2][12], 6);
  rk[ 68] = rotl32(rk[ 62] + LEA_DELTA[3][13], 6);
  rk[ 74] = rotl32(rk[ 68] + LEA_DELTA[0][14], 6);
  rk[ 80] = rotl32(rk[ 74] + LEA_DELTA[1][15], 6);
  rk[ 86] = rotl32(rk[ 80] + LEA_DELTA[2][16], 6);
  rk[ 92] = rotl32(rk[ 86] + LEA_DELTA[3][17], 6);
  rk[ 98] = rotl32(rk[ 92] + LEA_DELTA[0][18], 6);
  rk[104] = rotl32(rk[ 98] + LEA_DELTA[1][19], 6);
  rk[110] = rotl32(rk[104] + LEA_DELTA[2][20], 6);
  rk[116] = rotl32(rk[110] + LEA_DELTA[3][21], 6);
  rk[122] = rotl32(rk[116] + LEA_DELTA[0][22], 6);
  rk[128] = rotl32(rk[122] + LEA_DELTA[1][23], 6);
  rk[134] = rotl32(rk[128] + LEA_DELTA[2][24], 6);
  rk[140] = rotl32(rk[134] + LEA_DELTA[3][25], 6);

  rk[  4] = rotl32(k3 + LEA_DELTA[0][ 3], 11);
  rk[ 10] = rotl32(rk[  4] + LEA_DELTA[1][ 4], 11);
  rk[ 16] = rotl32(rk[ 10] + LEA_DELTA[2][ 5], 11);
  rk[ 22] = rotl32(rk[ 16] + LEA_DELTA[3][ 6], 11);
  rk[ 28] = rotl32(rk[ 22] + LEA_DELTA[0][ 7], 11);
  rk[ 34] = rotl32(rk[ 28] + LEA_DELTA[1][ 8], 11);
  rk[ 40] = rotl32(rk[ 34] + LEA_DELTA[2][ 9], 11);
  rk[ 46] = rotl32(rk[ 40] + LEA_DELTA[3][10], 11);
  rk[ 52] = rotl32(rk[ 46] + LEA_DELTA[0][11], 11);
  rk[ 58] = rotl32(rk[ 52] + LEA_DELTA[1][12], 11);
  rk[ 64] = rotl32(rk[ 58] + LEA_DELTA[2][13], 11);
  rk[ 70] = rotl32(rk[ 64] + LEA_DELTA[3][14], 11);
  rk[ 76] = rotl32(rk[ 70] + LEA_DELTA[0][15], 11);
  rk[ 82] = rotl32(rk[ 76] + LEA_DELTA[1][16], 11);
  rk[ 88] = rotl32(rk[ 82] + LEA_DELTA[2][17], 11);
  rk[ 94] = rotl32(rk[ 88] + LEA_DELTA[3][18], 11);
  rk[100] = rotl32(rk[ 94] + LEA_DELTA[0][19], 11);
  rk[106] = rotl32(rk[100] + LEA_DELTA[1][20], 11);
  rk[112] = rotl32(rk[106] + LEA_DELTA[2][21], 11);
  rk[118] = rotl32(rk[112] + LEA_DELTA[3][22], 11);
  rk[124] = rotl32(rk[118] + LEA_DELTA[0][23], 11);
  rk[130] = rotl32(rk[124] + LEA_DELTA[1][24], 11);
  rk[136] = rotl32(rk[130] + LEA_DELTA[2][25], 11);
  rk[142] = rotl32(rk[136] + LEA_DELTA[3][26], 11);
}

void lea128_encrypt_block(const uint32_t rk[144], const uint8_t in[16], uint8_t out[16]) {
  uint32_t x0 = load32le(in + 0);
  uint32_t x1 = load32le(in + 4);
  uint32_t x2 = load32le(in + 8);
  uint32_t x3 = load32le(in + 12);

  for (int r = 0; r < 24; ++r) {
    const uint32_t* k = rk + (r * 6);
    const uint32_t t0 = rotl32((x0 ^ k[0]) + (x1 ^ k[1]), 9);
    const uint32_t t1 = rotr32((x1 ^ k[2]) + (x2 ^ k[3]), 5);
    const uint32_t t2 = rotr32((x2 ^ k[4]) + (x3 ^ k[5]), 3);
    const uint32_t t3 = x0;
    x0 = t0;
    x1 = t1;
    x2 = t2;
    x3 = t3;
  }

  store32le(out + 0, x0);
  store32le(out + 4, x1);
  store32le(out + 8, x2);
  store32le(out + 12, x3);
}

void lea128_decrypt_block(const uint32_t rk[144], const uint8_t in[16], uint8_t out[16]) {
  uint32_t x0 = load32le(in + 0);
  uint32_t x1 = load32le(in + 4);
  uint32_t x2 = load32le(in + 8);
  uint32_t x3 = load32le(in + 12);

  for (int r = 23; r >= 0; --r) {
    const uint32_t* k = rk + (r * 6);
    const uint32_t p0 = x3;
    const uint32_t p1 = (rotr32(x0, 9) - (p0 ^ k[0])) ^ k[1];
    const uint32_t p2 = (rotl32(x1, 5) - (p1 ^ k[2])) ^ k[3];
    const uint32_t p3 = (rotl32(x2, 3) - (p2 ^ k[4])) ^ k[5];
    x0 = p0;
    x1 = p1;
    x2 = p2;
    x3 = p3;
  }

  store32le(out + 0, x0);
  store32le(out + 4, x1);
  store32le(out + 8, x2);
  store32le(out + 12, x3);
}

bool lea128_self_test() {
  const uint8_t tv_key[16] = {
      0x07,0xAB,0x63,0x05,0xB0,0x25,0xD8,0x3F,0x79,0xAD,0xDA,0xA6,0x3A,0xC8,0xAD,0x00};
  const uint8_t tv_pt[16] = {
      0xF2,0x8A,0xE3,0x25,0x6A,0xAD,0x23,0xB4,0x15,0xE0,0x28,0x06,0x3B,0x61,0x0C,0x60};
  const uint8_t tv_ct[16] = {
      0x64,0xD9,0x08,0xFC,0xB7,0xEB,0xFE,0xF9,0x0F,0xD6,0x70,0x10,0x6D,0xE7,0xC7,0xC5};

  uint32_t rk[144];
  uint8_t buf[16];
  lea128_set_key(tv_key, rk);
  lea128_encrypt_block(rk, tv_pt, buf);
  if (memcmp(buf, tv_ct, sizeof(buf)) != 0) {
    return false;
  }
  lea128_decrypt_block(rk, buf, buf);
  return memcmp(buf, tv_pt, sizeof(buf)) == 0;
}

}  // namespace

void run_lea_experiments(void) {
  const uint8_t key[16] = {
      0x00, 0x01, 0x02, 0x03,
      0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0A, 0x0B,
      0x0C, 0x0D, 0x0E, 0x0F};
  const uint8_t in_block[16] = {
      0x10, 0x11, 0x12, 0x13,
      0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1A, 0x1B,
      0x1C, 0x1D, 0x1E, 0x1F};

  if (!lea128_self_test()) {
    print_unsupported_algo(F("lea"), 16, 16, key[0], in_block[0],
                           F("self-test failed"));
    return;
  }

  uint32_t rk[144];
  uint8_t out_block[16] = {0};
  lea128_set_key(key, rk);

  unsigned long time_trials = 0UL;
  for (int i = 0; i < 200; ++i) {
    const unsigned long start = micros();
    lea128_encrypt_block(rk, in_block, out_block);
    lea128_decrypt_block(rk, out_block, out_block);
    time_trials += (micros() - start);
  }

  print_experiment_row(F("lea128_ref"), 16, 16, time_trials / 200UL, key[0], out_block[0],
                       F("LEA-128 standalone"));
}
