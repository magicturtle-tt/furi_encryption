#include <stdint.h>

#define LTC_TEA_OK 0
#define LTC_TEA_INVALID_KEYSIZE 3

typedef struct {
  uint32_t k[4];
} ltc_tea_key;

static uint32_t load32be(const uint8_t* p) {
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) |
         (uint32_t)p[3];
}

static void store32be(uint8_t* p, uint32_t v) {
  p[0] = (uint8_t)(v >> 24);
  p[1] = (uint8_t)(v >> 16);
  p[2] = (uint8_t)(v >> 8);
  p[3] = (uint8_t)v;
}

int ltc_tea_setup(const uint8_t* key, int keylen, ltc_tea_key* skey) {
  if (key == 0 || skey == 0) {
    return LTC_TEA_INVALID_KEYSIZE;
  }
  if (keylen != 16) {
    return LTC_TEA_INVALID_KEYSIZE;
  }

  skey->k[0] = load32be(key + 0);
  skey->k[1] = load32be(key + 4);
  skey->k[2] = load32be(key + 8);
  skey->k[3] = load32be(key + 12);
  return LTC_TEA_OK;
}

int ltc_tea_ecb_encrypt(const uint8_t* pt, uint8_t* ct, const ltc_tea_key* skey) {
  if (pt == 0 || ct == 0 || skey == 0) {
    return LTC_TEA_INVALID_KEYSIZE;
  }

  uint32_t y = load32be(pt);
  uint32_t z = load32be(pt + 4);
  uint32_t sum = 0;
  const uint32_t delta = 0x9E3779B9UL;

  for (int r = 0; r < 32; ++r) {
    sum += delta;
    y += ((z << 4) + skey->k[0]) ^ (z + sum) ^ ((z >> 5) + skey->k[1]);
    z += ((y << 4) + skey->k[2]) ^ (y + sum) ^ ((y >> 5) + skey->k[3]);
  }

  store32be(ct, y);
  store32be(ct + 4, z);
  return LTC_TEA_OK;
}

int ltc_tea_ecb_decrypt(const uint8_t* ct, uint8_t* pt, const ltc_tea_key* skey) {
  if (pt == 0 || ct == 0 || skey == 0) {
    return LTC_TEA_INVALID_KEYSIZE;
  }

  uint32_t v0 = load32be(ct);
  uint32_t v1 = load32be(ct + 4);
  uint32_t sum = 0xC6EF3720UL;
  const uint32_t delta = 0x9E3779B9UL;

  for (int r = 0; r < 32; ++r) {
    v1 -= ((v0 << 4) + skey->k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + skey->k[3]);
    v0 -= ((v1 << 4) + skey->k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + skey->k[1]);
    sum -= delta;
  }

  store32be(pt, v0);
  store32be(pt + 4, v1);
  return LTC_TEA_OK;
}
