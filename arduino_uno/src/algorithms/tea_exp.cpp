#include <Arduino.h>
#include <string.h>

extern "C" {
typedef struct {
  uint32_t k[4];
} ltc_tea_key;

int ltc_tea_setup(const uint8_t* key, int keylen, ltc_tea_key* skey);
int ltc_tea_ecb_encrypt(const uint8_t* pt, uint8_t* ct, const ltc_tea_key* skey);
int ltc_tea_ecb_decrypt(const uint8_t* ct, uint8_t* pt, const ltc_tea_key* skey);
}

#include "exp_common.h"

namespace {

const int kTeaOk = 0;

bool tea_self_test() {
  const uint8_t tv_key[16] = {0x00};
  const uint8_t tv_pt[8] = {0x00};
  const uint8_t tv_ct[8] = {0x41, 0xEA, 0x3A, 0x0A, 0x94, 0xBA, 0xA9, 0x40};

  ltc_tea_key skey;
  uint8_t buf[8] = {0};

  if (ltc_tea_setup(tv_key, static_cast<int>(sizeof(tv_key)), &skey) != kTeaOk) {
    return false;
  }
  if (ltc_tea_ecb_encrypt(tv_pt, buf, &skey) != kTeaOk) {
    return false;
  }
  if (memcmp(buf, tv_ct, sizeof(buf)) != 0) {
    return false;
  }
  if (ltc_tea_ecb_decrypt(buf, buf, &skey) != kTeaOk) {
    return false;
  }

  return memcmp(buf, tv_pt, sizeof(buf)) == 0;
}

}  // namespace

void run_tea_experiments(void) {
  const uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  const uint8_t in_block[8] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

  if (!tea_self_test()) {
    print_unsupported_algo(F("tea"), 16, 8, key[0], in_block[0], F("libtomcrypt self-test failed"));
    return;
  }

  ltc_tea_key skey;
  if (ltc_tea_setup(key, static_cast<int>(sizeof(key)), &skey) != kTeaOk) {
    print_unsupported_algo(F("tea"), 16, 8, key[0], in_block[0], F("libtomcrypt setup failed"));
    return;
  }

  uint8_t out_block[8] = {0};
  unsigned long time_trials = 0UL;
  for (int i = 0; i < 200; ++i) {
    const unsigned long start = micros();
    ltc_tea_ecb_encrypt(in_block, out_block, &skey);
    ltc_tea_ecb_decrypt(out_block, out_block, &skey);
    time_trials += (micros() - start);
  }

  print_experiment_row(F("tea"), 16, 8, time_trials / 200UL, key[0], out_block[0],
                       F("libtomcrypt tea.c derived"));
}
