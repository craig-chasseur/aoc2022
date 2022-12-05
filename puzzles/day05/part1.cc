#include <iostream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "re2/re2.h"
#include "util/io.h"

namespace {

class Dock {
 public:
  explicit Dock(const std::vector<std::string>& rep) {
    std::vector<absl::string_view> stack_desc = absl::StrSplit(rep.back(), ' ', absl::SkipEmpty());
    stacks_.resize(stack_desc.size());
    for (auto row_it = rep.rbegin() + 1; row_it != rep.rend(); ++row_it) {
      for (int stack_idx = 0; stack_idx < stacks_.size(); ++stack_idx) {
        const char crate = (*row_it)[4 * stack_idx + 1];
        if (crate != ' ') {
          stacks_[stack_idx].push(crate);
        }
      }
    }
  }

  void ApplyMove(absl::string_view move) {
    static re2::LazyRE2 kMovePattern = {R"re(move (\d+) from (\d+) to (\d+))re"};
    int num_crates, src, dest;
    CHECK(RE2::FullMatch(move, *kMovePattern, &num_crates, &src, &dest));
    src = src - 1;
    dest = dest - 1;
    for (int i = 0; i < num_crates; ++i) {
      CHECK(!stacks_[src].empty());
      const char crate = stacks_[src].top();
      stacks_[src].pop();
      stacks_[dest].push(crate);
    }
  }

  std::string Tops() const {
    std::string tops;
    for (const std::stack<char>& stack : stacks_) {
      CHECK(!stack.empty());
      tops.push_back(stack.top());
    }
    return tops;
  }

 private:
  std::vector<std::stack<char>> stacks_;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  std::vector<std::vector<std::string>> parts = aoc::SplitByEmptyStrings(std::move(lines));
  CHECK_EQ(2, parts.size());

  Dock dock(parts.front());
  for (const absl::string_view move : parts.back()) {
    dock.ApplyMove(move);
  }
  std::cout << dock.Tops() << "\n";
  return 0;
}
