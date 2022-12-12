#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <variant>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/strings/strip.h"
#include "util/io.h"

namespace {

class MonkeyGang;

class Monkey {
 public:
  explicit Monkey(const std::vector<std::string>& lines) {
    CHECK_EQ(6, lines.size());

    absl::string_view monkey_line = lines[0];
    CHECK(absl::ConsumePrefix(&monkey_line, "Monkey "));
    CHECK(absl::ConsumeSuffix(&monkey_line, ":"));
    CHECK(absl::SimpleAtoi(monkey_line, &index_));

    absl::string_view starting_items = lines[1];
    CHECK(absl::ConsumePrefix(&starting_items, "  Starting items: "));
    for (const absl::string_view itemstr :
         absl::StrSplit(starting_items, ", ")) {
      int item = 0;
      CHECK(absl::SimpleAtoi(itemstr, &item));
      items_.push_back(item);
    }

    absl::string_view operation = lines[2];
    CHECK(absl::ConsumePrefix(&operation, "  Operation: new = old "));
    switch (operation.front()) {
      case '+':
        operation_ = Op::kAdd;
        break;
      case '*':
        operation_ = Op::kMul;
        break;
      default:
        CHECK(false) << "Bad operation: " << operation.front();
    }
    absl::string_view argstr = operation.substr(2);
    if (argstr == "old") {
      arg_ = Old{};
    } else {
      int arg = 0;
      CHECK(absl::SimpleAtoi(argstr, &arg));
      arg_ = arg;
    }

    absl::string_view test = lines[3];
    CHECK(absl::ConsumePrefix(&test, "  Test: divisible by "));
    CHECK(absl::SimpleAtoi(test, &test_divisor_));

    absl::string_view true_line = lines[4];
    CHECK(absl::ConsumePrefix(&true_line, "    If true: throw to monkey "));
    CHECK(absl::SimpleAtoi(true_line, &true_target_));
    CHECK_NE(index_, true_target_);

    absl::string_view false_line = lines[5];
    CHECK(absl::ConsumePrefix(&false_line, "    If false: throw to monkey "));
    CHECK(absl::SimpleAtoi(false_line, &false_target_));
    CHECK_NE(index_, false_target_);
  }

  void Receive(int item) {
    items_.push_back(item);
  }

  void TakeTurn(MonkeyGang& gang);

  int InspectionCount() const {
    return inspection_count_;
  }

 private:
  enum class Op {
    kAdd,
    kMul,
  };

  struct Old {};

  int index_ = 0;

  std::vector<int> items_;

  Op operation_;
  std::variant<int, Old> arg_ = 0;

  int test_divisor_ = 0;
  int true_target_ = 0;
  int false_target_ = 0;

  int inspection_count_ = 0;
};

class MonkeyGang {
 public:
  explicit MonkeyGang(std::vector<std::string> lines) {
    std::vector<std::vector<std::string>> groups =
        aoc::SplitByEmptyStrings(std::move(lines));
    for (const std::vector<std::string>& group : groups) {
      monkeys_.emplace_back(group);
    }
  }

  void ThrowTo(int monkey, int item) {
    CHECK_GE(monkey, 0);
    CHECK_LT(monkey, monkeys_.size());
    monkeys_[monkey].Receive(item);
  }

  void PlayRound() {
    for (Monkey& monkey : monkeys_) {
      monkey.TakeTurn(*this);
    }
  }

  int MonkeyBusiness() const {
    std::priority_queue<int, std::vector<int>, std::greater<int>> top2;
    for (const Monkey& monkey : monkeys_) {
      top2.push(monkey.InspectionCount());
      if (top2.size() > 2) top2.pop();
    }
    CHECK_EQ(2, top2.size());
    int business = top2.top();
    top2.pop();
    return business * top2.top();
  }

 private:
  std::vector<Monkey> monkeys_;
};

void Monkey::TakeTurn(MonkeyGang& gang) {
  for (int item : items_) {
    ++inspection_count_;
    switch (operation_) {
      case Op::kAdd:
        if (std::holds_alternative<int>(arg_)) {
          item += std::get<int>(arg_);
        } else {
          CHECK(std::holds_alternative<Old>(arg_));
          item *= 2;
        }
        break;
      case Op::kMul:
        if (std::holds_alternative<int>(arg_)) {
          item *= std::get<int>(arg_);
        } else {
          CHECK(std::holds_alternative<Old>(arg_));
          item = item * item;
        }
        break;
    }
    item /= 3;

    if (item % test_divisor_ == 0) {
      gang.ThrowTo(true_target_, item);
    } else {
      gang.ThrowTo(false_target_, item);
    }
  }
  items_.clear();
}

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  MonkeyGang gang(aoc::ReadLinesFromFile(argv[1]));
  for (int round = 0; round < 20; ++round) {
    gang.PlayRound();
  }
  std::cout << gang.MonkeyBusiness() << "\n";
  return 0;
}
