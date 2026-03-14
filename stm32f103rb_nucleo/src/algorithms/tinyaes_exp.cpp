#include <Arduino.h>
#include <AES.h>

#include "exp_common.h"

void run_tinyaes_experiments(void) {
  const uint8_t payload_bytes = 16;
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

  uint8_t out_block[16] = {0};
  AESTiny128 aes;
  aes.setKey(key, sizeof(key));

  unsigned long time_trials = 0UL;
  for (int i = 0; i < 1000; ++i) {
    const unsigned long start = micros();
    aes.encryptBlock(out_block, in_block);
    time_trials += (micros() - start);
  }

  const unsigned long avg_time = time_trials / 1000UL;
  print_experiment_row(F("tinyaes_ecb_128"), sizeof(key), payload_bytes, avg_time,
                       key[0], out_block[0], F("rweather/Crypto:AESTiny128"));
}
