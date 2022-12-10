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

  int64_t SignalStrength() const { return cycle_counter_ * registers_.r; }

  void AdvanceCycles(int cycles) {
    while (cycles > 0) {
      if (remaining_instr_latency_ > 0) {
        const int advance = std::min(cycles, remaining_instr_latency_);
        cycles -= advance;
        remaining_instr_latency_ -= advance;
        cycle_counter_ += advance;
        continue;
      }
      if (pc_ >= 0) {
        program_[pc_]->Apply(registers_);
      }
      ++pc_;
      if (pc_ < program_.size()) {
        remaining_instr_latency_ = program_[pc_]->Latency();
      }
    }
  }

 private:
  Registers registers_;
  int64_t cycle_counter_ = 0;
  std::vector<std::unique_ptr<Instruction>> program_;
  int64_t pc_ = -1;
  int remaining_instr_latency_ = 0;
};

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(2, argc);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);
  Cpu cpu(ParseProgram(lines));

  int64_t total_strength = 0;

  cpu.AdvanceCycles(20);
  total_strength += cpu.SignalStrength();
  for (int i = 0; i < 5; ++i) {
    cpu.AdvanceCycles(40);
    total_strength += cpu.SignalStrength();
  }
  std::cout << total_strength << "\n";
  return 0;
}
