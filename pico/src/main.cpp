#include "pico/stdlib.h"
#include <cstdio>
#include <cstring>

extern "C" {
#include "aes.h"
}

int main() {
    stdio_init_all();
    const int LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    uint8_t key[16] = {0x00};
    uint8_t plaintext[16] = "HelloCubeSat!";
    uint8_t buffer[16];

    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);

    memcpy(buffer, plaintext, 16);
    AES_ECB_encrypt(&ctx, buffer);

    while (true) {
        gpio_put(LED_PIN, 1);
        printf("Encrypted data:\n");
        for(int i = 0; i < 16; i++)
            printf("%02x ", buffer[i]);
        printf("\n");

        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }
}
