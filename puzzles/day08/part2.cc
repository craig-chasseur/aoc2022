#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

namespace {

class Grid {
 public:
  explicit Grid(const std::vector<std::string>& lines) {
    for (const absl::string_view line : lines) {
      grid_.emplace_back();
      for (const char c : line) {
        grid_.back().emplace_back(c - '0');
      }
    }
  }

  int MaxScenicScore() const {
    int max = 0;
    for (size_t row = 0; row < grid_.size(); ++row) {
      for (size_t col = 0; col < grid_[row].size(); ++col) {
        max = std::max(max, ScenicScore(row, col));
      }
    }
    return max;
  }

 private:
  int ScenicScore(const size_t row, const size_t col) const {
    const int8_t height = grid_[row][col];

    int left = 0;
    for (int look_col = col - 1; look_col >= 0; --look_col) {
      ++left;
      if (grid_[row][look_col] >= height) break;
    }

    int right = 0;
    for (int look_col = col + 1; look_col < grid_[row].size(); ++look_col) {
      ++right;
      if (grid_[row][look_col] >= height) break;
    }

    int up = 0;
    for (int look_row = row - 1; look_row >= 0; --look_row) {
      ++up;
      if (grid_[look_row][col] >= height) break;
    }

    int down = 0;
    for (int look_row = row + 1; look_row < grid_.size(); ++look_row) {
      ++down;
      if (grid_[look_row][col] >= height) break;
    }

    return left * right * up * down;
  }

  std::vector<std::vector<int8_t>> grid_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  Grid grid(lines);
  std::cout << grid.MaxScenicScore() << "\n";
  return 0;
}
