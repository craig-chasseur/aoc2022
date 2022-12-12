#include <cstdint>
#include <string>
#include <vector>

#include "absl/log/check.h"
#include "util/a_star.h"
#include "util/grid2.h"
#include "util/io.h"

using aoc::grid2::Point;
using aoc::grid2::Vec;
using aoc::grid2::Vecs;

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  Point start, goal;

  for (int64_t y = 0; y < lines.size(); ++y) {
    for (int64_t x = 0; x < lines[y].size(); ++x) {
      if (lines[y][x] == 'S') {
        start.x = x;
        start.y = y;
      } else if (lines[y][x] == 'E') {
        goal.x = x;
        goal.y = y;
      }
    }
  }

  auto get_adjacent = [&lines](const Point& point) -> std::vector<Point> {
    auto elevation = [&lines](const Point& point) -> char {
      switch (lines[point.y][point.x]) {
        case 'S':
          return 'a';
          break;
        case 'E':
          return 'z';
          break;
        default:
          return lines[point.y][point.x];
      }
    };
    auto in_bounds = [&lines](const Point& point) -> bool {
      return point.y >= 0 && point.y < lines.size() && point.x >= 0 &&
             point.x < lines[point.y].size();
    };

    std::vector<Point> reachable;
    for (const Vec& cardinal : Vecs::kCardinal) {
      const Point candidate = point + cardinal;
      if (in_bounds(candidate) &&
          elevation(candidate) <= elevation(point) + 1) {
        reachable.emplace_back(candidate);
      }
    }
    return reachable;
  };

  auto edge_cost = [](const Point&, const Point&) { return 1; };

  auto result = aoc::AStar(start, goal, get_adjacent, edge_cost);
  CHECK(result.has_value());
  std::cout << result->cost << "\n";

  return 0;
}
