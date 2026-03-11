#include <cstdint>
#include <cstdio>

#include "pico/time.h"
#include "cryptopp/rc5.h"

void run_rc5_experiments(void) {
    const unsigned key_bytes = CryptoPP::RC5::DEFAULT_KEYLENGTH;
    const unsigned block_bytes = CryptoPP::RC5::BLOCKSIZE;

    const CryptoPP::byte key[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F
    };
    const CryptoPP::byte in_block[8] = {
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
    };
    CryptoPP::byte out_block[8] = {0};

    CryptoPP::RC5::Encryption enc;
    CryptoPP::RC5::Decryption dec;
    enc.SetKey(key, key_bytes);
    dec.SetKey(key, key_bytes);
    enc.ProcessBlock(in_block, out_block);

    absolute_time_t start = get_absolute_time();
    dec.ProcessBlock(out_block, out_block);
    int64_t elapsed_us = absolute_time_diff_us(start, get_absolute_time());

    printf("rc5_cryptopp,%u,%u,%lld,%02x,%02x,%s\n",
           key_bytes, block_bytes, static_cast<long long>(elapsed_us), key[0], out_block[0], "Crypto++ RC5 encrypt");
}
