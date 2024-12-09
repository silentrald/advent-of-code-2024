
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

const char *const FILENAME = "input.txt";

bool validate_array(const std::vector<int32_t> &diffs) noexcept {
  int32_t curr;
  int32_t prev = diffs[0];
  int32_t safe = prev >= -3 && prev <= 3 && prev != 0;
  if (!safe) {
    return false;
  }

  for (int32_t i = 1; i < diffs.size(); ++i) {
    curr = diffs[i];
    if (curr < -3 || curr > 3 || curr == 0 ||
        ((prev & 0x8000'0000) ^ (curr & 0x8000'0000))) {
      return false;
    }
    prev = curr;
  }

  return true;
}

void print_diffs(const std::vector<int32_t> &diffs) noexcept {
  for (const auto d : diffs) {
    printf("%d ", d);
  }
}

void part1() noexcept {
  std::ifstream file{FILENAME};

  std::string line{};
  std::vector<int32_t> diffs{};
  int32_t prev, curr;
  int32_t count = 0;

  while (std::getline(file, line)) {
    diffs.clear();
    std::istringstream iss(line);

    iss >> prev;
    while (iss >> curr) {
      diffs.push_back(curr - prev);
      prev = curr;
    }

    count += validate_array(diffs);
  }

  file.close();
  printf("Part 1: %d\n", count); // 306
}

void part2() noexcept {
  std::ifstream file{FILENAME};

  std::string line{};
  int32_t prev, curr, sum, index;
  bool safe, dampened;
  std::vector<int32_t> diffs{};
  int32_t count = 0;

  while (std::getline(file, line)) {
    diffs.clear();
    std::istringstream iss(line);

    iss >> prev;
    while (iss >> curr) {
      diffs.push_back(curr - prev);
      prev = curr;
    }

    if (validate_array(diffs)) {
      ++count;
      continue;
    }

    curr = diffs.back();
    diffs.pop_back();

    if (validate_array(diffs)) {
      ++count;
      continue;
    }

    // Brute force :)
    safe = false;
    for (index = diffs.size() - 1; index >= 0; --index) {
      sum = diffs[index] + curr;
      curr = diffs[index];
      diffs[index] = sum;
      if (validate_array(diffs)) {
        safe = true;
        break;
      }
      diffs[index] -= curr;
    }

    if (index == -1) {
      safe = validate_array(diffs);
    }

    count += safe;
  }

  file.close();
  printf("Part 2: %d\n", count); // 366
}

int32_t main() noexcept {
  part1();
  part2();
  return 0;
}
