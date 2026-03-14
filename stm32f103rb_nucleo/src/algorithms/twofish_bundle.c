#include <stdint.h>

#define LTC_TWOFISH_OK 0
#define LTC_TWOFISH_INVALID_KEYSIZE 3
#define LTC_TWOFISH_INVALID_ROUNDS 4

typedef struct {
  uint32_t K[40];
  uint8_t S[32];
  uint8_t start;
} ltc_twofish_key;

static uint32_t load32le(const uint8_t* p) {
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static void store32le(uint8_t* p, uint32_t v) {
  p[0] = (uint8_t)v;
  p[1] = (uint8_t)(v >> 8);
  p[2] = (uint8_t)(v >> 16);
  p[3] = (uint8_t)(v >> 24);
}

static uint32_t rotl32(uint32_t x, unsigned r) {
  r &= 31U;
  return (x << r) | (x >> ((32U - r) & 31U));
}

static uint32_t rotr32(uint32_t x, unsigned r) {
  r &= 31U;
  return (x >> r) | (x << ((32U - r) & 31U));
}

static const uint8_t kRS[4][8] = {{0x01, 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E},
                                  {0xA4, 0x56, 0x82, 0xF3, 0x1E, 0xC6, 0x68, 0xE5},
                                  {0x02, 0xA1, 0xFC, 0xC1, 0x47, 0xAE, 0x3D, 0x19},
                                  {0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E, 0x03}};

static const uint8_t kQOrd[4][5] = {
    {1, 1, 0, 0, 1},
    {0, 1, 1, 0, 0},
    {0, 0, 0, 1, 1},
    {1, 0, 1, 1, 0},
};

static const uint8_t kQBox[2][4][16] = {
    {{0x8, 0x1, 0x7, 0xD, 0x6, 0xF, 0x3, 0x2, 0x0, 0xB, 0x5, 0x9, 0xE, 0xC, 0xA, 0x4},
     {0xE, 0xC, 0xB, 0x8, 0x1, 0x2, 0x3, 0x5, 0xF, 0x4, 0xA, 0x6, 0x7, 0x0, 0x9, 0xD},
     {0xB, 0xA, 0x5, 0xE, 0x6, 0xD, 0x9, 0x0, 0xC, 0x8, 0xF, 0x3, 0x2, 0x4, 0x7, 0x1},
     {0xD, 0x7, 0xF, 0x4, 0x1, 0x2, 0x6, 0xE, 0x9, 0xB, 0x3, 0x0, 0x8, 0x5, 0xC, 0xA}},
    {{0x2, 0x8, 0xB, 0xD, 0xF, 0x7, 0x6, 0xE, 0x3, 0x1, 0x9, 0x4, 0x0, 0xA, 0xC, 0x5},
     {0x1, 0xE, 0x2, 0xB, 0x4, 0xC, 0x3, 0x7, 0x6, 0xD, 0xA, 0x5, 0xF, 0x9, 0x0, 0x8},
     {0x4, 0xC, 0x7, 0x5, 0x1, 0x6, 0x9, 0xA, 0x0, 0xE, 0xD, 0x8, 0x2, 0xB, 0x3, 0xF},
     {0xB, 0x9, 0x5, 0x1, 0xC, 0x3, 0xD, 0xE, 0x6, 0x4, 0x7, 0xF, 0x2, 0x0, 0x8, 0xA}}};

static uint32_t gf_mult(uint32_t a, uint32_t b, uint32_t p) {
  uint32_t result;
  uint32_t B[2];
  uint32_t P[2];

  P[1] = p;
  B[1] = b;
  result = P[0] = B[0] = 0;

  result ^= B[a & 1U];
  a >>= 1;
  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
  result ^= B[a & 1U];
  a >>= 1;
  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
  result ^= B[a & 1U];
  a >>= 1;
  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
  result ^= B[a & 1U];
  a >>= 1;
  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
  result ^= B[a & 1U];
  a >>= 1;
  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
  result ^= B[a & 1U];
  a >>= 1;
  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
  result ^= B[a & 1U];
  a >>= 1;
  B[1] = P[B[1] >> 7] ^ (B[1] << 1);
  result ^= B[a & 1U];

  return result;
}

static uint8_t twofish_sbox(uint8_t i, uint8_t x) {
  uint8_t a0, b0, a1, b1, a2, b2, a3, b3, a4, b4;

  a0 = (x >> 4) & 15U;
  b0 = x & 15U;
  a1 = a0 ^ b0;
  b1 = (a0 ^ ((uint8_t)(b0 << 3) | (b0 >> 1)) ^ (uint8_t)(a0 << 3)) & 15U;
  a2 = kQBox[i][0][a1];
  b2 = kQBox[i][1][b1];
  a3 = a2 ^ b2;
  b3 = (a2 ^ ((uint8_t)(b2 << 3) | (b2 >> 1)) ^ (uint8_t)(a2 << 3)) & 15U;
  a4 = kQBox[i][2][a3];
  b4 = kQBox[i][3][b3];

  return (uint8_t)((b4 << 4) + a4);
}

static uint32_t mds_column_mult(uint8_t in, int col) {
  const uint32_t x01 = in;
  const uint32_t x5B = gf_mult(in, 0x5B, 0x169);
  const uint32_t xEF = gf_mult(in, 0xEF, 0x169);

  switch (col) {
    case 0:
      return (x01 << 0) | (x5B << 8) | (xEF << 16) | (xEF << 24);
    case 1:
      return (xEF << 0) | (xEF << 8) | (x5B << 16) | (x01 << 24);
    case 2:
      return (x5B << 0) | (xEF << 8) | (x01 << 16) | (xEF << 24);
    default:
      return (x5B << 0) | (x01 << 8) | (xEF << 16) | (x5B << 24);
  }
}

static void mds_mult(const uint8_t* in, uint8_t* out) {
  uint32_t tmp = 0;
  for (int x = 0; x < 4; ++x) {
    tmp ^= mds_column_mult(in[x], x);
  }
  store32le(out, tmp);
}

static void rs_mult(const uint8_t* in, uint8_t* out) {
  for (int x = 0; x < 4; ++x) {
    out[x] = 0;
    for (int y = 0; y < 8; ++y) {
      out[x] ^= (uint8_t)gf_mult(in[y], kRS[x][y], 0x14D);
    }
  }
}

static void h_func(const uint8_t* in, uint8_t* out, const uint8_t* M, int k, int offset) {
  uint8_t y[4];

  for (int x = 0; x < 4; ++x) {
    y[x] = in[x];
  }

  switch (k) {
    case 4:
      y[0] = (uint8_t)(twofish_sbox(1, y[0]) ^ M[4 * (6 + offset) + 0]);
      y[1] = (uint8_t)(twofish_sbox(0, y[1]) ^ M[4 * (6 + offset) + 1]);
      y[2] = (uint8_t)(twofish_sbox(0, y[2]) ^ M[4 * (6 + offset) + 2]);
      y[3] = (uint8_t)(twofish_sbox(1, y[3]) ^ M[4 * (6 + offset) + 3]);
      /* FALLTHROUGH */
    case 3:
      y[0] = (uint8_t)(twofish_sbox(1, y[0]) ^ M[4 * (4 + offset) + 0]);
      y[1] = (uint8_t)(twofish_sbox(1, y[1]) ^ M[4 * (4 + offset) + 1]);
      y[2] = (uint8_t)(twofish_sbox(0, y[2]) ^ M[4 * (4 + offset) + 2]);
      y[3] = (uint8_t)(twofish_sbox(0, y[3]) ^ M[4 * (4 + offset) + 3]);
      /* FALLTHROUGH */
    case 2:
      y[0] = twofish_sbox(0, y[0]);
      y[0] ^= M[4 * (2 + offset) + 0];
      y[0] = twofish_sbox(0, y[0]);
      y[0] ^= M[4 * (0 + offset) + 0];
      y[0] = twofish_sbox(1, y[0]);

      y[1] = twofish_sbox(1, y[1]);
      y[1] ^= M[4 * (2 + offset) + 1];
      y[1] = twofish_sbox(0, y[1]);
      y[1] ^= M[4 * (0 + offset) + 1];
      y[1] = twofish_sbox(0, y[1]);

      y[2] = twofish_sbox(0, y[2]);
      y[2] ^= M[4 * (2 + offset) + 2];
      y[2] = twofish_sbox(1, y[2]);
      y[2] ^= M[4 * (0 + offset) + 2];
      y[2] = twofish_sbox(1, y[2]);

      y[3] = twofish_sbox(1, y[3]);
      y[3] ^= M[4 * (2 + offset) + 3];
      y[3] = twofish_sbox(1, y[3]);
      y[3] ^= M[4 * (0 + offset) + 3];
      y[3] = twofish_sbox(0, y[3]);
      break;
    default:
      break;
  }

  mds_mult(y, out);
}

static uint32_t g_func(uint32_t x, const ltc_twofish_key* key) {
  uint32_t res = 0;

  for (uint8_t y = 0; y < 4; ++y) {
    uint8_t z = key->start;
    uint8_t g = twofish_sbox(kQOrd[y][z++], (uint8_t)((x >> (8U * y)) & 255U));
    uint8_t i = 0;

    while (z != 5) {
      g ^= key->S[4 * i++ + y];
      g = twofish_sbox(kQOrd[y][z++], g);
    }

    res ^= mds_column_mult(g, y);
  }

  return res;
}

static uint32_t g1_func(uint32_t x, const ltc_twofish_key* key) {
  return g_func(rotl32(x, 8), key);
}

int ltc_twofish_setup(const uint8_t* key, int keylen, ltc_twofish_key* skey) {
  int k;
  uint8_t M[32] = {0};
  uint8_t tmp[4], tmp2[4];
  uint32_t A, B;

  if (key == 0 || skey == 0) {
    return LTC_TWOFISH_INVALID_KEYSIZE;
  }
  if (keylen != 16 && keylen != 24 && keylen != 32) {
    return LTC_TWOFISH_INVALID_KEYSIZE;
  }

  k = keylen / 8;
  for (int x = 0; x < keylen; ++x) {
    M[x] = key[x];
  }

  for (int x = 0; x < k; ++x) {
    rs_mult(M + (x * 8), skey->S + (x * 4));
  }

  for (int x = 0; x < 20; ++x) {
    for (int y = 0; y < 4; ++y) {
      tmp[y] = (uint8_t)(x + x);
    }
    h_func(tmp, tmp2, M, k, 0);
    A = load32le(tmp2);

    for (int y = 0; y < 4; ++y) {
      tmp[y] = (uint8_t)(x + x + 1);
    }
    h_func(tmp, tmp2, M, k, 1);
    B = rotl32(load32le(tmp2), 8);

    skey->K[x + x] = A + B;
    skey->K[x + x + 1] = rotl32(B + B + A, 9);
  }

  switch (k) {
    case 4:
      skey->start = 0;
      break;
    case 3:
      skey->start = 1;
      break;
    default:
      skey->start = 2;
      break;
  }

  return LTC_TWOFISH_OK;
}

int ltc_twofish_ecb_encrypt(const uint8_t* pt, uint8_t* ct, const ltc_twofish_key* skey) {
  uint32_t a, b, c, d, ta, tb, tc, td, t1, t2;
  const uint32_t* k;

  if (pt == 0 || ct == 0 || skey == 0) {
    return LTC_TWOFISH_INVALID_KEYSIZE;
  }

  a = load32le(&pt[0]);
  b = load32le(&pt[4]);
  c = load32le(&pt[8]);
  d = load32le(&pt[12]);
  a ^= skey->K[0];
  b ^= skey->K[1];
  c ^= skey->K[2];
  d ^= skey->K[3];

  k = skey->K + 8;
  for (int r = 8; r != 0; --r) {
    t2 = g1_func(b, skey);
    t1 = g_func(a, skey) + t2;
    c = rotr32(c ^ (t1 + k[0]), 1);
    d = rotl32(d, 1) ^ (t2 + t1 + k[1]);

    t2 = g1_func(d, skey);
    t1 = g_func(c, skey) + t2;
    a = rotr32(a ^ (t1 + k[2]), 1);
    b = rotl32(b, 1) ^ (t2 + t1 + k[3]);
    k += 4;
  }

  ta = c ^ skey->K[4];
  tb = d ^ skey->K[5];
  tc = a ^ skey->K[6];
  td = b ^ skey->K[7];

  store32le(&ct[0], ta);
  store32le(&ct[4], tb);
  store32le(&ct[8], tc);
  store32le(&ct[12], td);

  return LTC_TWOFISH_OK;
}

int ltc_twofish_ecb_decrypt(const uint8_t* ct, uint8_t* pt, const ltc_twofish_key* skey) {
  uint32_t a, b, c, d, ta, tb, tc, td, t1, t2;
  const uint32_t* k;

  if (pt == 0 || ct == 0 || skey == 0) {
    return LTC_TWOFISH_INVALID_KEYSIZE;
  }

  ta = load32le(&ct[0]);
  tb = load32le(&ct[4]);
  tc = load32le(&ct[8]);
  td = load32le(&ct[12]);

  a = tc ^ skey->K[6];
  b = td ^ skey->K[7];
  c = ta ^ skey->K[4];
  d = tb ^ skey->K[5];

  k = skey->K + 36;
  for (int r = 8; r != 0; --r) {
    t2 = g1_func(d, skey);
    t1 = g_func(c, skey) + t2;
    a = rotl32(a, 1) ^ (t1 + k[2]);
    b = rotr32(b ^ (t2 + t1 + k[3]), 1);

    t2 = g1_func(b, skey);
    t1 = g_func(a, skey) + t2;
    c = rotl32(c, 1) ^ (t1 + k[0]);
    d = rotr32(d ^ (t2 + t1 + k[1]), 1);
    k -= 4;
  }

  a ^= skey->K[0];
  b ^= skey->K[1];
  c ^= skey->K[2];
  d ^= skey->K[3];

  store32le(&pt[0], a);
  store32le(&pt[4], b);
  store32le(&pt[8], c);
  store32le(&pt[12], d);

  return LTC_TWOFISH_OK;
}
