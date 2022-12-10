#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/numbers.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

namespace {

struct Registers {
  int64_t r = 1;
};

class Instruction {
 public:
  virtual ~Instruction() = default;
  virtual int Latency() const = 0;
  virtual void Apply(Registers& registers) const = 0;
};

class Noop final : public Instruction {
 public:
  Noop() = default;
  ~Noop() override = default;

  int Latency() const override { return 1; }
  void Apply(Registers& registers) const override {}
};

class Addx final : public Instruction {
 public:
  explicit Addx(int64_t immediate) : immediate_(immediate) {}
  ~Addx() override = default;

  int Latency() const override { return 2; }

  void Apply(Registers& registers) const override { registers.r += immediate_; }

 private:
  int64_t immediate_ = 0;
};

std::unique_ptr<Instruction> Parse(absl::string_view asm_line) {
  if (asm_line.substr(0, 4) == "noop") {
    return std::make_unique<Noop>();
  }
  CHECK_EQ(asm_line.substr(0, 4), "addx");
  int64_t immediate = 0;
  CHECK(absl::SimpleAtoi(asm_line.substr(5), &immediate));
  return std::make_unique<Addx>(immediate);
}

std::vector<std::unique_ptr<Instruction>> ParseProgram(
    const std::vector<std::string>& asm_lines) {
  std::vector<std::unique_ptr<Instruction>> program;
  program.reserve(asm_lines.size());
  for (const absl::string_view asm_line : asm_lines) {
    program.emplace_back(Parse(asm_line));
  }
  return program;
}

class Cpu {
 public:
  explicit Cpu(std::vector<std::unique_ptr<Instruction>> program)
      : program_(std::move(program)) {}

  std::string Draw() {
    remaining_instr_latency_ = program_[pc_]->Latency();
    std::string screen;
    for (;;) {
      for (int col = 0; col < 40; ++col) {
        if (col == registers_.r - 1 || col == registers_.r || col == registers_.r + 1){
          screen.push_back('#');
        } else {
          screen.push_back(' ');
        }
        if (--remaining_instr_latency_ == 0) {
          program_[pc_]->Apply(registers_);
          if (++pc_ == program_.size()) return screen;
          remaining_instr_latency_ = program_[pc_]->Latency();
        }
      }
      screen.push_back('\n');
    }
  }

 private:
  Registers registers_;
  int64_t cycle_counter_ = 0;
  std::vector<std::unique_ptr<Instruction>> program_;
  int64_t pc_ = 0;
  int remaining_instr_latency_ = 0;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  Cpu cpu(ParseProgram(lines));
  std::cout << cpu.Draw() << "\n";
  return 0;
}
