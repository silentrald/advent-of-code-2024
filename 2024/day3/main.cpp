#include <cstdint>
#include <fstream>

void part1() noexcept {
  // Cleaned with grep and sed
  // grep -oh "mul(\([0-9]*\),\([0-9]*\))" * > day3-cleaned.txt
  // %s/mul(\([0-9]*\),\([0-9]*\))/\1 \2
  std::ifstream file{"cleaned.txt"};

  int32_t i1, i2, accumulator = 0;
  while (file >> i1 >> i2) {
    accumulator += i1 * i2;
  }

  printf("Part 1: %d\n", accumulator);
}

void part2() noexcept {
  // Cleaned with grep and sed
  // grep -oh "mul(\([0-9]*\),\([0-9]*\))\|don't()\|do()" * > day3-cleaned-2.txt
  // manual cleaning for do() / don't()
  // %s/mul(\([0-9]*\),\([0-9]*\))/\1 \2
  std::ifstream file("cleaned-2.txt");

  int32_t i1, i2, accumulator = 0;
  while (file >> i1 >> i2) {
    accumulator += i1 * i2;
  }

  printf("Part 2: %d\n", accumulator);
}

int main() {
  part1();
  part2();
  return 0;
}
