#include "xor.h"

#include <iostream>
#include <iomanip>  // std::hex, std::setw, std::setfill
#include <sstream>

void xor_cipher(unsigned char* data, std::size_t length, std::uint8_t key) {
    for (std::size_t i = 0; i < length; ++i) {
        data[i] ^= key;
    }
}
//
// void print_hex_formatted(const unsigned char* data, std::size_t len) {
//
//     len -= 1;
//
//     const std::size_t line_bytes = 16;
//
//     std::printf("unsigned char buf[] =\n");
//
//     // 按行打印每 16 字节
//     for (std::size_t i = 0; i < len; i += line_bytes) {
//         std::printf("    \"");
//         std::size_t end = (i + line_bytes < len) ? (i + line_bytes) : len;
//         for (std::size_t j = i; j < end; ++j) {
//             std::printf("\\x%02x", data[j]);
//         }
//         std::printf("\"");
//
//         if (end < len) std::printf("\n");
//         else           std::printf(";\n");
//     }
// }