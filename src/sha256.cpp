#include "include/sha256.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <endian.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

sha256::sha256(const char *filepath)
{
  if (!std::filesystem::is_regular_file(filepath)) {
    std::cerr << "Error: " << filepath << " is not a regular file" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  file = std::ifstream(filepath, std::ifstream::ate | std::ifstream::binary);
  if (!file) {
    std::cerr << "Error: can not open file -> " << filepath << std::endl;
    std::exit(EXIT_FAILURE);
  }

  filesize = file.tellg();
  file.seekg(0, std::ifstream::beg);

  zero_bytes = 64 - (filesize + 1 + 8) % 64;

  total_blocks = (filesize + 1 + zero_bytes + 8) / 64;
  block_to_read = 1;
}

auto sha256::read_block() -> std::array<std::uint8_t, 64>
{
  auto block = std::array<std::uint8_t, 64>{0};

  if (block_to_read < total_blocks - 1) {
    file.read((char *)block.data(), 64);

    if (!file) {
      std::cerr << "Error: can not read complete file" << std::endl;
      std::exit(EXIT_FAILURE);
    }
  } else if (block_to_read == total_blocks - 1) {
    // Second last block
    file.read((char *)block.data(), 64);

    if (zero_bytes > 55)
      block[64 - (zero_bytes - 56) - 1] = 0x80;

  } else {
    // Last block
    size_t bitlen_be = htobe64(filesize * 8);
    memcpy(block.data() + 56, &bitlen_be, 8);

    if (zero_bytes == 55) block[0] = 0x80;
    else if (zero_bytes < 55) {
      file.read((char *)block.data(), 64);
      block[64 - (zero_bytes + 8) - 1] = 0x80;
    }
    file.close();
  }

  ++block_to_read;
  return block;
}

void sha256::update_md(std::array<std::uint8_t, 64> block)
{
  std::array<std::uint32_t, 64> w{0};
  memcpy(w.data(), block.data(), 64);
  std::transform(w.begin(), w.begin() + 64, w.begin(),
                 [](std::uint32_t n) { return htobe32(n); });

  for (int t = 16; t < 64; ++t) {
    auto s0 = rightrotate(w[t - 15], 7) ^ rightrotate(w[t - 15], 18) ^
              (w[t - 15] >> 3);
    auto s1 = rightrotate(w[t - 2], 17) ^ rightrotate(w[t - 2], 19) ^
              (w[t - 2] >> 10);

    w[t] = (w[t - 16] + s0 + w[t - 7] + s1);
  }

  auto [ a, b, c, d, e, f, g, h ] = md;

  for (int t = 0; t < 64; ++t) {
    auto S0 = rightrotate(a, 2) ^ rightrotate(a, 13) ^ rightrotate(a, 22);
    auto maj = (a & b) ^ (a & c) ^ (b & c);
    auto t2 = S0 + maj;

    auto S1 = rightrotate(e, 6) ^ rightrotate(e, 11) ^ rightrotate(e, 25);
    auto ch = (e & f) ^ ((~e) & g);
    auto t1 = h + S1 + ch + K[t] + w[t];

    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }

  md[0] += a;
  md[1] += b;
  md[2] += c;
  md[3] += d;
  md[4] += e;
  md[5] += f;
  md[6] += g;
  md[7] += h;
}

inline auto sha256::digest() -> std::array<std::uint32_t, 8>
{
  while (block_to_read <= total_blocks) update_md(read_block());
  return md;
}

inline auto sha256::digest_str() -> std::string
{
  while (block_to_read <= total_blocks) update_md(read_block());

  std::stringstream ss;
  ss << std::hex;
  for (auto n: md) ss << std::setw(8) << std::setfill('0') << n;
  return ss.str();
}

inline auto rightrotate(std::uint32_t a, int n) -> std::uint32_t
{
  return ( (a >> n) | (a << (32 - n)));
}
