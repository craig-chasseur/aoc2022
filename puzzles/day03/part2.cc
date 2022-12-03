#include <bit>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

namespace {

uint64_t ItemBits(const absl::string_view line) {
  uint64_t bits = 0;
  for (const char c : line) {
    if (c >= 'a' && c <= 'z') {
      bits |= uint64_t{1} << (c - 'a');
    } else {
      CHECK(c >= 'A' && c <= 'Z');
      bits |= uint64_t{1} << (c - 'A' + 26);
    }
  }
  return bits;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);

  int total_priority = 0;
  for (auto group_begin_it = lines.begin(); group_begin_it != lines.end();
       group_begin_it += 3) {
    uint64_t group_bit = ItemBits(*group_begin_it) &
                         ItemBits(*(group_begin_it + 1)) &
                         ItemBits(*(group_begin_it + 2));
    CHECK_EQ(1, std::popcount(group_bit));
    total_priority += std::countr_zero(group_bit) + 1;
  }
  std::cout << total_priority << "\n";
  return 0;
}
