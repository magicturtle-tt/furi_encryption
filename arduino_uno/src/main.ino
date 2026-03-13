#include <Arduino.h>

#include "algorithms/experiments.h"

const uint8_t LED_PIN = LED_BUILTIN;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  delay(300);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);

  Serial.println(F("algo,key_bytes,block_bytes,time_us,key0,block0,note"));
  run_tinyaes_experiments();
  run_lea_experiments();
  run_tea_experiments();
  run_rc5_experiments();
  run_twofish_experiments();
  run_simeck_experiments();
  run_simon_experiments();
  run_speck_experiments();
  run_hight_experiments();
  run_seed_experiments();

  digitalWrite(LED_PIN, LOW);
  delay(1500);
}
