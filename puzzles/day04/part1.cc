#include <iostream>
#include <string>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

namespace {

struct Range {
  explicit Range(absl::string_view desc) {
    std::vector<absl::string_view> parts = absl::StrSplit(desc, '-');
    CHECK_EQ(2, parts.size());
    CHECK(absl::SimpleAtoi(parts.front(), &low));
    CHECK(absl::SimpleAtoi(parts.back(), &high));
    CHECK_GE(high, low);
  }

  bool FullyContains(const Range& other) const {
    return low <= other.low && high >= other.high;
  }

  int low = 0;
  int high = 0;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  int full_contain_count = 0;
  for (absl::string_view line : lines) {
    std::vector<absl::string_view> parts = absl::StrSplit(line, ',');
    CHECK_EQ(2, parts.size());
    Range r1(parts.front()), r2(parts.back());
    if (r1.FullyContains(r2) || r2.FullyContains(r1)) {
      ++full_contain_count;
    }
  }
  std::cout << full_contain_count << "\n";
  return 0;
}
