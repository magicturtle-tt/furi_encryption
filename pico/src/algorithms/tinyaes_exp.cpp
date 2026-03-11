// TinyAES experiment example called from main.cpp.
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "pico/time.h"
extern "C" {
#include "aes.h"
}

void run_tinyaes_experiments(void) {
    const uint8_t payload_bytes = 16;
    const uint8_t key[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F
    };
    const uint8_t in_block[16] = {
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B,
        0x1C, 0x1D, 0x1E, 0x1F
    };
    uint8_t buffer[16];
    for (size_t i = 0; i < payload_bytes; ++i) {
        buffer[i] = in_block[i];
    }

    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);

    absolute_time_t start = get_absolute_time();
    for (size_t i = 0; i < payload_bytes; i += 16) {
        AES_ECB_encrypt(&ctx, &buffer[i]);
    }
    int64_t elapsed_us = absolute_time_diff_us(start, get_absolute_time());

    printf("algo,payload_bytes,time_us,first_byte\n");
    printf("tinyaes_ecb_128,%u,%lld,%02x\n",
           (unsigned)payload_bytes,
           (long long)elapsed_us,
           buffer[0]);
}
