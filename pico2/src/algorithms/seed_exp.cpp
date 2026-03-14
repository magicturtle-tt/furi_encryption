#include <cstdint>
#include <cstdio>

#include "pico/time.h"
#include "cryptopp/seed.h"

void run_seed_experiments(void) {
    const unsigned key_bytes = CryptoPP::SEED::DEFAULT_KEYLENGTH;
    const unsigned block_bytes = CryptoPP::SEED::BLOCKSIZE;
    const CryptoPP::byte key[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F
    };
    const CryptoPP::byte in_block[16] = {
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B,
        0x1C, 0x1D, 0x1E, 0x1F
    };
    CryptoPP::byte out_block[16] = {0};
    CryptoPP::SEED::Encryption enc;
    CryptoPP::SEED::Decryption dec;
    enc.SetKey(key, key_bytes);
    dec.SetKey(key, key_bytes);
    absolute_time_t start = get_absolute_time();
    int64_t trial = 0;
    int64_t time_trials = 0;
    for(int i = 0; i < 1000; i++){
        enc.ProcessBlock(in_block, out_block);
        start = get_absolute_time();
        dec.ProcessBlock(out_block);
        trial = absolute_time_diff_us(start, get_absolute_time());
        time_trials += trial;
    }
    int64_t avg_time = time_trials / 1000;

    printf("algo,key_bytes,block_bytes,time_us,key0,block0,note\n");
    printf("seed_cryptopp,%u,%u,%lld,%02x,%02x,%s\n",
           key_bytes,
           block_bytes,
           static_cast<long long>(avg_time),
           key[0],
           out_block[0],
           "Crypto++:SEED encrypt");
}
