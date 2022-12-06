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

template <typename Iter>
bool AllUnique(Iter begin, Iter end) {
  uint64_t mask = 0;
  for (; begin != end; ++begin) {
    const uint64_t bit = CharBit(*begin);
    if ((mask & bit) != 0) return false;
    mask |= bit;
  }
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  CHECK_EQ(1, lines.size());
  std::string buffer = std::move(lines.front());
  for (auto it = buffer.begin() + 14; it != buffer.end(); ++it) {
    if (AllUnique(it - 14, it)) {
      std::cout << (it - buffer.begin()) << "\n";
      return 0;
    }
  }
  return 1;
}
