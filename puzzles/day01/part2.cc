#include <algorithm>
#include <functional>
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
  std::vector<std::vector<std::string>> groups =
      aoc::SplitByEmptyStrings(std::move(lines));
  std::vector<std::vector<int>> calorie_groups;
  for (const auto& group : groups) {
    calorie_groups.emplace_back(aoc::ParseIntegers(group));
  }

  std::vector<int> top_calories_heap;
  for (const auto& group : calorie_groups) {
    top_calories_heap.push_back(std::accumulate(group.begin(), group.end(), 0));
    std::push_heap(top_calories_heap.begin(), top_calories_heap.end(),
                   std::greater<int>());
    if (top_calories_heap.size() > 3) {
      std::pop_heap(top_calories_heap.begin(), top_calories_heap.end(),
                    std::greater<int>());
      top_calories_heap.pop_back();
    }
  }
  std::cout << std::accumulate(top_calories_heap.begin(),
                               top_calories_heap.end(), 0)
            << "\n";
  return 0;
}
