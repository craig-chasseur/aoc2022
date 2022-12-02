#include <iostream>
#include <string>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/string_view.h"
#include "util/io.h"

namespace {

int ScoreRound(const absl::string_view line) {
  const int opp_move_idx = line.front() - 'A';

  const int goal_state = line.back() - 'X';
  int my_move_idx = 0;
  switch (goal_state) {
    case 0:
      my_move_idx = (opp_move_idx - 1 + 3) % 3;
      break;
    case 1:
      my_move_idx = opp_move_idx;
      break;
    case 2:
      my_move_idx = (opp_move_idx + 1) % 3;
      break;
  }

  const int move_score = my_move_idx + 1;
  if (my_move_idx == opp_move_idx) return 3 + move_score;
  if (((opp_move_idx + 1) % 3) == my_move_idx) return 6 + move_score;
  return move_score;
}

}  // namespace

int main(int argc, char** argv) {
  CHECK_EQ(argc, 2);
  std::vector<std::string> lines = aoc::ReadLinesFromFile(argv[1]);

  int score = 0;
  for (absl::string_view line : lines) {
    score += ScoreRound(line);
  }
  std::cout << score << "\n";

  return 0;
}
