#include <bit>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "absl/log/check.h"
#include "util/io.h"

namespace {

uint64_t CharBit(char c) {
  return uint64_t{1} << (c - 'a');
}

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  CHECK_EQ(1, lines.size());
  std::string buffer = std::move(lines.front());
  for (int pos = 3; pos < buffer.size(); ++pos) {
    const uint64_t mask = CharBit(buffer[pos - 3]) | CharBit(buffer[pos - 2]) | CharBit(buffer[pos - 1]) | CharBit(buffer[pos]);
    if (std::popcount(mask) == 4) {
      std::cout << (pos + 1) << "\n";
      return 0;
    }
  }
  return 1;
}
