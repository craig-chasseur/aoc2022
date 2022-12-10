#include <cstddef>
#include <iostream>
#include <string>
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
      aoc::grid2::Point previous_head = head_;
      switch (dir) {
        case 'U':
          head_ += aoc::grid2::Vecs::kUp1;
          break;
        case 'D':
          head_ += aoc::grid2::Vecs::kDown1;
          break;
        case 'L':
          head_ += aoc::grid2::Vecs::kLeft1;
          break;
        case 'R':
          head_ += aoc::grid2::Vecs::kRight1;
          break;
        default:
          CHECK(false) << "Unknown direction: " << dir;
      }
      const aoc::grid2::Vec diff = tail_ - head_;
      if (diff.dx < -1 || diff.dx > 1 || diff.dy < -1 || diff.dy > 1) {
        tail_ = previous_head;
        tail_visited_.insert(tail_);
      }
    }
  }

  size_t TotalTailVisited() const { return tail_visited_.size(); }

 private:
  aoc::grid2::Point head_;
  aoc::grid2::Point tail_;

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
