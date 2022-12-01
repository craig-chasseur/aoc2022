#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

int main(int argc, char** argv) {
  CHECK_EQ(argc, 2);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> groups = aoc::SplitByEmptyStrings(
      std::move(lines));
  std::vector<std::vector<int>> calorie_groups;
  for (const auto& group : groups) {
    calorie_groups.emplace_back(aoc::ParseIntegers(group));
  }

  int max_calories = 0;
  for (const auto& group : calorie_groups) {
    max_calories = std::max(max_calories,
                            std::accumulate(group.begin(), group.end(), 0));
  }
  std::cout << max_calories << "\n";
  return 0;
}
