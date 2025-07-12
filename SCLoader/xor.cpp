#include "xor.h"

#include <iostream>
#include <iomanip>  // std::hex, std::setw, std::setfill
#include <sstream>

void xor_cipher(unsigned char *data, std::size_t length, std::uint8_t key) {
    for (std::size_t i = 0; i < length; ++i) {
        data[i] ^= key;
    }
}
