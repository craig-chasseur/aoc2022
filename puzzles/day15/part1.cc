#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"
#include "absl/strings/string_view.h"
#include "re2/re2.h"
#include "util/grid2.h"
#include "util/io.h"

namespace {

class Sensor {
 public:
  explicit Sensor(absl::string_view line) {
    static re2::LazyRE2 pattern = {
        R"re(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))re"};
    CHECK(re2::RE2::FullMatch(line, *pattern, &position_.x, &position_.y,
                              &nearest_beacon_.x, &nearest_beacon_.y)) << line;
  }

  std::pair<int64_t, int64_t> RuledOutXRange(const int64_t y) const {
    const aoc::grid2::Vec to_beacon = nearest_beacon_ - position_;
    int64_t x_budget = to_beacon.ManhattanDistance() - std::abs(position_.y - y);
    if (x_budget < 0) return {0, 0};
    return {position_.x - x_budget, position_.x + x_budget + 1};
  }

  aoc::grid2::Point beacon() const { return nearest_beacon_; }

 private:
  aoc::grid2::Point position_;
  aoc::grid2::Point nearest_beacon_;
};

constexpr int64_t kRow = 2000000;

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  std::vector<Sensor> sensors;
  for (absl::string_view line : lines) {
    sensors.emplace_back(line);
  }

  absl::flat_hash_set<int64_t> impossible_x;
  for (const Sensor& sensor : sensors) {
    const auto [min_x, limit_x] = sensor.RuledOutXRange(kRow);
    for (int64_t x = min_x; x < limit_x; ++x) {
      impossible_x.emplace(x);
    }
  }

  for (const Sensor& sensor : sensors) {
    if (sensor.beacon().y == kRow) {
      impossible_x.erase(sensor.beacon().x);
    }
  }

  std::cout << impossible_x.size() << "\n";
  return 0;
}
