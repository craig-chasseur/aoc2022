#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

namespace {

enum class CompareResult { kLess, kEqual, kGreater };

class Packet {
 public:
  explicit Packet(absl::string_view line) {
    if (line.front() == '[') {
      std::vector<Packet> elements;
      CHECK_EQ(line.back(), ']');
      const absl::string_view body = line.substr(1, line.size() - 2);
      int depth = 0;
      int segment_start = 0;
      for (int pos = 0; pos < body.size(); ++pos) {
        switch (body[pos]) {
          case '[':
            ++depth;
            break;
          case ']':
            --depth;
            break;
          case ',':
            if (depth == 0) {
              elements.emplace_back(
                  body.substr(segment_start, pos - segment_start));
              segment_start = pos + 1;
            }
            break;
          default:
            break;
        }
      }
      if (body.size() > 0) {
        elements.emplace_back(body.substr(segment_start));
      }
      rep_ = std::move(elements);
    } else {
      int intval = 0;
      CHECK(absl::SimpleAtoi(line, &intval)) << line.size();
      rep_ = intval;
    }
  }

  std::string PrettyPrint() const {
    if (std::holds_alternative<int>(rep_)) {
      return absl::StrCat(std::get<int>(rep_));
    }
    std::string printed = "[";
    const std::vector<Packet>& children = std::get<std::vector<Packet>>(rep_);
    for (int idx = 0; idx < children.size(); ++idx) {
      absl::StrAppend(&printed, children[idx].PrettyPrint());
      if (idx != children.size() - 1) {
        printed.push_back(',');
      }
    }
    printed.push_back(']');
    return printed;
  }

  bool operator<(const Packet& other) const {
    return CompareImpl(other) == CompareResult::kLess;
  }

  bool operator==(const Packet& other) const {
    return CompareImpl(other) == CompareResult::kEqual;
  }

 private:
  Packet() = default;

  CompareResult CompareImpl(const Packet& other) const {
    if (std::holds_alternative<int>(rep_)) {
      if (std::holds_alternative<int>(other.rep_)) {
        if (std::get<int>(rep_) < std::get<int>(other.rep_)) {
          return CompareResult::kLess;
        } else if (std::get<int>(rep_) == std::get<int>(other.rep_)) {
          return CompareResult::kEqual;
        } else {
          return CompareResult::kGreater;
        }
      }

      Packet listed;
      listed.rep_ = std::vector<Packet>{*this};
      return listed.CompareImpl(other);
    }

    if (std::holds_alternative<int>(other.rep_)) {
      Packet other_listed;
      other_listed.rep_ = std::vector<Packet>{other};
      return CompareImpl(other_listed);
    }

    const std::vector<Packet>& this_children =
        std::get<std::vector<Packet>>(rep_);
    const std::vector<Packet>& other_children =
        std::get<std::vector<Packet>>(other.rep_);
    size_t min_length = std::min(this_children.size(), other_children.size());
    for (size_t idx = 0; idx < min_length; ++idx) {
      switch (this_children[idx].CompareImpl(other_children[idx])) {
        case CompareResult::kLess:
          return CompareResult::kLess;
        case CompareResult::kEqual:
          continue;
        case CompareResult::kGreater:
          return CompareResult::kGreater;
      }
    }
    if (this_children.size() < other_children.size()) {
      return CompareResult::kLess;
    }
    if (this_children.size() == other_children.size()) {
      return CompareResult::kEqual;
    }
    return CompareResult::kGreater;
  }

  std::variant<int, std::vector<Packet>> rep_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  std::vector<Packet> all_packets;
  for (const absl::string_view line : lines) {
    if (!line.empty()) {
      all_packets.emplace_back(line);
    }
  }
  all_packets.emplace_back("[[2]]");
  all_packets.emplace_back("[[6]]");
  std::sort(all_packets.begin(), all_packets.end());

  auto [sep2_begin, sep2_end] = std::equal_range(all_packets.begin(), all_packets.end(), Packet("[[2]]"));
  CHECK_EQ(sep2_end - sep2_begin, 1);

  auto [sep6_begin, sep6_end] = std::equal_range(all_packets.begin(), all_packets.end(), Packet("[[6]]"));
  CHECK_EQ(sep6_end - sep6_begin, 1);

  const int product = ((sep2_begin - all_packets.begin()) + 1) * ((sep6_begin - all_packets.begin()) + 1);
  std::cout << product << "\n";
  return 0;
}
