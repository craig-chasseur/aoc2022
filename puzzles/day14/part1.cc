#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "util/grid2.h"
#include "util/io.h"

using aoc::grid2::Point;
using aoc::grid2::Vec;

namespace {

class Sandmap {
 public:
  explicit Sandmap(const std::vector<std::string>& lines) {
    for (absl::string_view line : lines) {
      ParseLine(line);
    }
    max_y_ = aoc::grid2::MaxDimensions(occupied_points_).y;
  }

  int64_t CountGrains() {
    int64_t count = 0;
    while (DropGrain()) {
      ++count;
    }
    return count;
  }

 private:
  void ParseLine(absl::string_view line) {
    std::vector<absl::string_view> coords = absl::StrSplit(line, " -> ");
    for (auto coords_it = coords.begin() + 1; coords_it != coords.end();
         ++coords_it) {
      const Point start = Point::ParseCoordPair(*(coords_it - 1));
      const Point end = Point::ParseCoordPair(*coords_it);
      const Vec step = (end - start).UnitStep();
      for (Point point = start; point != end; point += step) {
        occupied_points_.emplace(point);
      }
      occupied_points_.emplace(end);
    }
  }

  bool DropGrain() {
    static constexpr Vec kMoves[] = {
        {.dx = 0, .dy = 1}, {.dx = -1, .dy = 1}, {.dx = 1, .dy = 1}};
    Point grain{.x = 500, .y = 0};
    for (;;) {
      if (grain.y > max_y_) return false;
      bool moved = false;
      for (const Vec move : kMoves) {
        if (!occupied_points_.contains(grain + move)) {
          grain += move;
          moved = true;
          break;
        }
      }
      if (!moved) {
        occupied_points_.emplace(grain);
        return true;
      }
    }
  }

  absl::flat_hash_set<Point> occupied_points_;
  int64_t max_y_ = 0;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::cout << Sandmap(aoc::ReadLinesFromFile(argv[1])).CountGrains() << "\n";
  return 0;
}
