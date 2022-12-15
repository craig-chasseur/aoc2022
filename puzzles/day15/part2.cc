#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "absl/container/inlined_vector.h"
#include "absl/log/check.h"
#include "absl/strings/string_view.h"
#include "re2/re2.h"
#include "util/grid2.h"
#include "util/io.h"

namespace {

struct Interval {
  int64_t min = 0;
  int64_t max = 0;

  absl::InlinedVector<Interval, 2> Remove(const Interval& other) const {
    if (other.max <= min) return {*this};
    if (other.min >= max) return {*this};

    if (other.min <= min) {
      if (other.max >= max) {
        return {};
      }
      Interval modified(*this);
      modified.min = other.max;
      return {modified};
    }

    if (other.max <= max) {
      Interval first = *this;
      first.max = other.min;
      Interval second = *this;
      second.min = other.max;
      return {first, second};
    }

    Interval modified(*this);
    modified.max = other.min;
    return {modified};
  }

  bool empty() const { return max <= min; }
};

class Sensor {
 public:
  explicit Sensor(absl::string_view line) {
    static re2::LazyRE2 pattern = {
        R"re(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))re"};
    CHECK(re2::RE2::FullMatch(line, *pattern, &position_.x, &position_.y,
                              &nearest_beacon_.x, &nearest_beacon_.y));
    distance_ = (nearest_beacon_ - position_).ManhattanDistance();
  }

  Interval RuledOutXRange(const int64_t y) const {
    int64_t x_budget = distance_ - std::abs(position_.y - y);
    if (x_budget < 0) return {};
    return {.min = position_.x - x_budget, .max = position_.x + x_budget + 1};
  }

 private:
  aoc::grid2::Point position_;
  aoc::grid2::Point nearest_beacon_;
  int64_t distance_ = 0;
};

constexpr int64_t kMaxCoord = 4000000;

std::optional<aoc::grid2::Point> TrySolveAtY(
    const int64_t y, const std::vector<Sensor>& sensors) {
  std::vector<Interval> open{{.min = 0, .max = kMaxCoord}};
  for (const Sensor& sensor : sensors) {
    Interval ruled_out = sensor.RuledOutXRange(y);
    if (ruled_out.empty()) continue;
    std::vector<Interval> next_open;
    for (const Interval& interval : open) {
      auto remaining = interval.Remove(ruled_out);
      next_open.insert(next_open.end(), remaining.begin(), remaining.end());
    }
    if (next_open.empty()) return std::nullopt;
    open = std::move(next_open);
  }
  CHECK_EQ(open.size(), 1);
  CHECK_EQ(open.front().max, open.front().min + 1);
  return aoc::grid2::Point{.x = open.front().min, .y = y};
}

int64_t Frequency(const aoc::grid2::Point point) {
  return point.x * 4000000 + point.y;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  std::vector<Sensor> sensors;
  for (absl::string_view line : lines) {
    sensors.emplace_back(line);
  }

  for (int64_t y = 0; y < kMaxCoord; ++y) {
    std::optional<aoc::grid2::Point> maybe_signal = TrySolveAtY(y, sensors);
    if (maybe_signal.has_value()) {
      std::cout << *maybe_signal << " => " << Frequency(*maybe_signal) << "\n";
      return 0;
    }
  }

  return 1;
}
