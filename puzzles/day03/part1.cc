#include <iostream>
#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

namespace {

int Priority(char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 'a' + 1;
  }
  CHECK(c >= 'A' && c <= 'Z');
  return c - 'A' + 27;
}

}

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  int total_priority = 0;
  for (absl::string_view line : lines) {
    absl::string_view first_half = line.substr(0, line.size() / 2);
    absl::string_view second_half = line.substr(line.size() / 2);

    absl::flat_hash_set<char> first_half_set(first_half.begin(), first_half.end());
    int priority = 0;
    for (const char c : second_half) {
      if (first_half_set.contains(c)) {
        priority = Priority(c);
        break;
      }
    }
    CHECK_NE(priority, 0);
    total_priority += priority;
  }

  std::cout << total_priority << "\n";
  return 0;
}
