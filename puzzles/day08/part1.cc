#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"
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

  size_t CountVisible() const {
    absl::flat_hash_set<std::pair<int16_t, int16_t>> visible;
    for (int16_t rownum = 0; rownum < grid_.size(); ++rownum) {
      const std::vector<int8_t>& row = grid_[rownum];
      int8_t max_height = -1;
      for (int16_t colnum = 0; colnum < row.size(); ++colnum) {
        if (row[colnum] > max_height) {
          max_height = row[colnum];
          visible.emplace(rownum, colnum);
          if (max_height == 9) break;
        }
      }
      max_height = -1;
      for (int16_t colnum = row.size() - 1; colnum >= 0; --colnum) {
        if (row[colnum] > max_height) {
          max_height = row[colnum];
          visible.emplace(rownum, colnum);
          if (max_height == 9) break;
        }
      }
    }

    for (int16_t colnum = 0; colnum < grid_.front().size(); ++colnum) {
      int8_t max_height = -1;
      for (int16_t rownum = 0; rownum < grid_.size(); ++rownum) {
        if (grid_[rownum][colnum] > max_height) {
          max_height = grid_[rownum][colnum];
          visible.emplace(rownum, colnum);
          if (max_height == 9) break;
        }
      }
      max_height = -1;
      for (int16_t rownum = grid_.size() - 1; rownum >= 0; --rownum) {
        if (grid_[rownum][colnum] > max_height) {
          max_height = grid_[rownum][colnum];
          visible.emplace(rownum, colnum);
          if (max_height == 9) break;
        }
      }
    }

    return visible.size();
  }

 private:
  std::vector<std::vector<int8_t>> grid_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  Grid grid(lines);
  std::cout << grid.CountVisible() << "\n";
  return 0;
}
