#include <Arduino.h>
#include <string.h>

extern "C" {
typedef struct {
  uint32_t K[40];
  uint8_t S[32];
  uint8_t start;
} ltc_twofish_key;

int ltc_twofish_setup(const uint8_t* key, int keylen, ltc_twofish_key* skey);
int ltc_twofish_ecb_encrypt(const uint8_t* pt, uint8_t* ct, const ltc_twofish_key* skey);
int ltc_twofish_ecb_decrypt(const uint8_t* ct, uint8_t* pt, const ltc_twofish_key* skey);
}

#include "exp_common.h"

namespace {

const int kTwofishOk = 0;

bool twofish_self_test() {
  const uint8_t tv_key[16] = {0x9F, 0x58, 0x9F, 0x5C, 0xF6, 0x12, 0x2C, 0x32,
                              0xB6, 0xBF, 0xEC, 0x2F, 0x2A, 0xE8, 0xC3, 0x5A};
  const uint8_t tv_pt[16] = {0xD4, 0x91, 0xDB, 0x16, 0xE7, 0xB1, 0xC3, 0x9E,
                             0x86, 0xCB, 0x08, 0x6B, 0x78, 0x9F, 0x54, 0x19};
  const uint8_t tv_ct[16] = {0x01, 0x9F, 0x98, 0x09, 0xDE, 0x17, 0x11, 0x85,
                             0x8F, 0xAA, 0xC3, 0xA3, 0xBA, 0x20, 0xFB, 0xC3};

  ltc_twofish_key skey;
  uint8_t buf[16] = {0};

  if (ltc_twofish_setup(tv_key, static_cast<int>(sizeof(tv_key)), &skey) != kTwofishOk) {
    return false;
  }
  if (ltc_twofish_ecb_encrypt(tv_pt, buf, &skey) != kTwofishOk) {
    return false;
  }
  if (memcmp(buf, tv_ct, sizeof(buf)) != 0) {
    return false;
  }
  if (ltc_twofish_ecb_decrypt(buf, buf, &skey) != kTwofishOk) {
    return false;
  }

  return memcmp(buf, tv_pt, sizeof(buf)) == 0;
}

}  // namespace

void run_twofish_experiments(void) {
  const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  const uint8_t in_block[16] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

  if (!twofish_self_test()) {
    print_unsupported_algo(F("twofish"), 16, 16, key[0], in_block[0],
                           F("libtomcrypt-derived self-test failed"));
    return;
  }

  ltc_twofish_key skey;
  if (ltc_twofish_setup(key, static_cast<int>(sizeof(key)), &skey) != kTwofishOk) {
    print_unsupported_algo(F("twofish"), 16, 16, key[0], in_block[0],
                           F("libtomcrypt-derived setup failed"));
    return;
  }

  uint8_t out_block[16] = {0};
  unsigned long time_trials = 0UL;
  for (int i = 0; i < 50; ++i) {
    const unsigned long start = micros();
    ltc_twofish_ecb_encrypt(in_block, out_block, &skey);
    ltc_twofish_ecb_decrypt(out_block, out_block, &skey);
    time_trials += (micros() - start);
  }

  print_experiment_row(F("twofish"), 16, 16, time_trials / 50UL, key[0], out_block[0],
                       F("libtomcrypt twofish.c (small/no-table)"));
}
