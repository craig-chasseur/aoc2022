#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"
#include "absl/strings/numbers.h"
#include "absl/strings/string_view.h"
#include "util/grid2.h"
#include "util/io.h"

namespace {

class Rope {
 public:
  void MoveHead(absl::string_view move) {
    const char dir = move.front();
    int steps = 0;
    CHECK(absl::SimpleAtoi(move.substr(2), &steps));
    for (int step = 0; step < steps; ++step) {
      switch (dir) {
        case 'U':
          knots_.front() += aoc::grid2::Vecs::kUp1;
          break;
        case 'D':
          knots_.front() += aoc::grid2::Vecs::kDown1;
          break;
        case 'L':
          knots_.front() += aoc::grid2::Vecs::kLeft1;
          break;
        case 'R':
          knots_.front() += aoc::grid2::Vecs::kRight1;
          break;
        default:
          CHECK(false) << "Unknown direction: " << dir;
      }
      for (auto knot_it = knots_.begin() + 1; knot_it != knots_.end();
           ++knot_it) {
        aoc::grid2::Vec diff = *(knot_it - 1) - *knot_it;
        const int64_t dist = diff.ManhattanDistance();
        if (dist < 2) break;
        if (diff.Diag45() && dist == 2) break;
        diff.dx = std::clamp(diff.dx, -1l, 1l);
        diff.dy = std::clamp(diff.dy, -1l, 1l);
        *knot_it += diff;
      }
      tail_visited_.insert(knots_.back());
    }
  }

  size_t TotalTailVisited() const { return tail_visited_.size(); }

 private:
  std::array<aoc::grid2::Point, 10> knots_;

  absl::flat_hash_set<aoc::grid2::Point> tail_visited_{
      aoc::grid2::Points::kOrigin};
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  Rope rope;
  for (const absl::string_view move : lines) {
    rope.MoveHead(move);
  }
  std::cout << rope.TotalTailVisited() << "\n";
  return 0;
}
