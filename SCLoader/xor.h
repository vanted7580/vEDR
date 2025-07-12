#pragma once
#include <cstdint>

void xor_cipher(unsigned char* data, std::size_t length, std::uint8_t key);
//void print_hex_formatted(const unsigned char* data, std::size_t len);