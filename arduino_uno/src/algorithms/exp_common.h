#pragma once

#include <Arduino.h>

static inline void print_experiment_row(const __FlashStringHelper* algo,
                                        unsigned key_bytes,
                                        unsigned block_bytes,
                                        unsigned long time_us,
                                        uint8_t key0,
                                        uint8_t block0,
                                        const __FlashStringHelper* note) {
  Serial.print(algo);
  Serial.print(',');
  Serial.print(key_bytes);
  Serial.print(',');
  Serial.print(block_bytes);
  Serial.print(',');
  Serial.print(time_us);
  Serial.print(',');
  Serial.print(key0, HEX);
  Serial.print(',');
  Serial.print(block0, HEX);
  Serial.print(',');
  Serial.println(note);
}

static inline void print_unsupported_algo(const __FlashStringHelper* algo,
                                          unsigned key_bytes,
                                          unsigned block_bytes,
                                          uint8_t key0,
                                          uint8_t block0,
                                          const __FlashStringHelper* note) {
  print_experiment_row(algo, key_bytes, block_bytes, 0UL, key0, block0, note);
}
