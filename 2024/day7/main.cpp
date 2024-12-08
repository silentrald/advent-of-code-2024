#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
#endif

enum Operator : uint32_t {
  ADDITION = 0b00,
  MULTIPLICATION = 0b01,
  CONCATENATION = 0b10,
};

// With overflow handling
uint64_t concatenate(int64_t i1, int64_t i2) {
  uint8_t d1 = 0;
  uint8_t d2 = 0;
  for (uint64_t tmp = i1; tmp; tmp /= 10, ++d1)
    ;
  for (uint64_t tmp = i2; tmp; tmp /= 10, ++d2)
    ;

  // 18,446,744,073,709,551,615
  if (d1 + d2 >= 20) {
    return UINT_MAX;
  }

  return i1 * std::pow(10, d2) + i2;
}

void part1() noexcept {
  std::ifstream file{FILENAME};
  std::string line{};
  uint64_t accumulator = 0;
  uint64_t expected;
  uint64_t actual;
  uint64_t i;
  uint64_t end;
  char colon;
  std::vector<uint64_t> numbers{};

  while (std::getline(file, line)) {
    numbers.clear();
    std::stringstream ss{line};
    ss >> expected >> colon;

    while (ss >> i) {
      numbers.push_back(i);
    }

    // Bit manipulation
    end = 0x1 << numbers.size();
    for (int32_t bits = 0; bits <= end; ++bits) {
      actual = numbers[0];
      for (int32_t b = bits, i = 1; i < numbers.size(); b >>= 1, ++i) {
        if (b & 1) {
          actual *= numbers[i];
        } else {
          actual += numbers[i];
        }

        if (actual > expected) {
          actual = -1;
          break;
        }
      }

      if (actual == expected) {
        accumulator += expected;
        break;
      }
    }
  }

  file.close();

  // 8401132154762
  printf("Part 1: %lu\n", accumulator);
}

bool recurse_part2(uint64_t actual, uint64_t expected,
                   const std::vector<uint64_t> &numbers, int32_t index,
                   int32_t op) noexcept {
  switch (op) {
  case Operator::ADDITION:
    actual += numbers[index];
    break;

  case Operator::MULTIPLICATION:
    actual *= numbers[index];
    break;

  case Operator::CONCATENATION:
    actual = concatenate(actual, numbers[index]);
    break;
  }

  if (actual > expected) {
    // Early fall thru
    return false;
  }

  ++index;
  if (index == numbers.size()) { // Base case
    return actual == expected;
  }

  for (op = Operator::ADDITION; op <= Operator::CONCATENATION; ++op) {
    if (recurse_part2(actual, expected, numbers, index, op)) {
      return true;
    }
  }
  return false;
}

void part2() noexcept {
  std::ifstream file{FILENAME};
  std::string line{};
  uint64_t accumulator = 0;
  uint64_t expected;
  uint64_t i;
  char colon;
  std::vector<uint64_t> numbers{};

  while (std::getline(file, line)) {
    numbers.clear();

    std::stringstream ss{line};
    ss >> expected >> colon;

    while (ss >> i) {
      numbers.push_back(i);
    }

    for (int32_t op = Operator::ADDITION; op <= Operator::CONCATENATION; ++op) {
      if (recurse_part2(numbers[0], expected, numbers, 1, op)) {
        accumulator += expected;
        break;
      }
    }
  }

  file.close();

  // 95297119227552
  printf("Part 2: %lu\n", accumulator);
}

int32_t main() noexcept {
  part1();
  part2();
  return 0;
}
