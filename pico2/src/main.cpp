#include "pico/stdlib.h"
#include <cstdio>

void run_tinyaes_experiments(void);
void run_lea_experiments(void);
void run_tea_experiments(void);
void run_rc5_experiments(void);
void run_twofish_experiments(void);
void run_simeck_experiments(void);
void run_simon_experiments(void);
void run_speck_experiments(void);
void run_hight_experiments(void);
void run_seed_experiments(void);

int main() {
    stdio_init_all();
    const int LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    sleep_ms(1200);

    while (true) {
        gpio_put(LED_PIN, 1);
        printf("algo,key_bytes,block_bytes,time_us,key0,block0,note\n");
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

        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(1250);
    }
}
