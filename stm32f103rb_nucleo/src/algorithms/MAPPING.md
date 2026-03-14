# Algorithm Source Mapping (Uno)

This project targets Arduino Uno (`ATmega328P`), so Crypto++ is not used directly.

Current mapping plan:

- `HIGHT` -> `openluopworld/block-ciphers` (C implementation)
- `LEA` -> `openluopworld/block-ciphers` (C implementation)
- `SIMON` -> `openluopworld/block-ciphers` (C implementation)
- `SPECK` -> `openluopworld/block-ciphers` (C implementation)
- `SIMECK` -> `bozhu/Simeck` (C implementation)
- `RC5` -> `LibTomCrypt` (C implementation)
- `SEED` -> `LibTomCrypt` (C implementation)
- `Twofish` -> `LibTomCrypt` (C implementation)
- `TEA` -> standalone TEA C implementation (to be added)
- `TinyAES` -> `rweather/Crypto` (`AESTiny128`) [integrated now]

Status in code:

- `run_tinyaes_experiments()` performs a real Uno benchmark.
- Other `run_*_experiments()` functions compile and emit a CSV row with an `unsupported` note that names the mapped source.
