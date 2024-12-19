#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
#endif

struct Input {
  std::unordered_set<std::string> patterns{};
  std::vector<std::string> towels{};
};

Input get_input() noexcept {
  Input input{};

  std::ifstream file{FILENAME};
  std::string line{};

  // First line
  std::getline(file, line);
  int64_t start = 0;
  int64_t end = 0;
  while (true) {
    end = line.find(",", start);
    if (end == -1) {
      input.patterns.insert(line.substr(start));
      break;
    }

    input.patterns.insert(line.substr(start, end - start));
    start = end + 2;
  }

  // \n
  std::getline(file, line);

  // Remaning
  while (std::getline(file, line)) {
    input.towels.push_back(std::move(line));
  }

  return std::move(input);
}

[[nodiscard]] bool
valid_towel(int64_t index, const std::string &towel,
            const std::unordered_set<std::string> patterns,
            const int64_t max_pattern,
            std::unordered_map<std::string, bool> &memo) noexcept {
  auto it = memo.find(towel.substr(index));
  if (it != memo.end()) {
    return it->second;
  }

  if (index == towel.size()) {
    return true;
  }

  for (int64_t size = 1; size <= max_pattern && index + size <= towel.size();
       ++size) {
    if (patterns.find(towel.substr(index, size)) == patterns.end()) {
      continue;
    }

    if (valid_towel(index + size, towel, patterns, max_pattern, memo)) {
      memo.insert({towel.substr(index), true});
      return true;
    }
  }

  memo.insert({towel.substr(index), false});
  return false;
}

void part1(const std::unordered_set<std::string> &patterns,
           const std::vector<std::string> &towels) noexcept {
  int64_t count = 0;
  int64_t max_pattern = 1;
  std::unordered_map<std::string, bool> memo{};

  for (const auto &p : patterns) {
    max_pattern = std::max(max_pattern, (int64_t)p.size());
  }

  for (const auto &t : towels) {
    count += valid_towel(0, t, patterns, max_pattern, memo);
  }

  printf("Part 1: %ld\n", count);
}

// Dynamic programming, used recursion but can be iterative to save stack calls
[[nodiscard]] int64_t
count_towel(int64_t index, const std::string &towel,
            const std::unordered_set<std::string> patterns,
            const int64_t max_pattern,
            std::unordered_map<std::string, int64_t> &memo) noexcept {
  auto it = memo.find(towel.substr(index));
  if (it != memo.end()) {
    return it->second;
  }

  if (index == towel.size()) {
    return 1;
  }

  int64_t total = 0;
  for (int64_t size = 1; size <= max_pattern && index + size <= towel.size();
       ++size) {
    if (patterns.find(towel.substr(index, size)) == patterns.end()) {
      continue;
    }

    int64_t count =
        count_towel(index + size, towel, patterns, max_pattern, memo);
    memo.insert({towel.substr(index + size), count});
    total += count;
  }

  return total;
}

void part2(const std::unordered_set<std::string> &patterns,
           const std::vector<std::string> &towels) noexcept {
  int64_t count = 0;
  int64_t max_pattern = 1;
  std::unordered_map<std::string, int64_t> memo{};

  for (const auto &p : patterns) {
    max_pattern = std::max(max_pattern, (int64_t)p.size());
  }

  for (const auto &t : towels) {
    count += count_towel(0, t, patterns, max_pattern, memo);
  }

  printf("Part 2: %ld\n", count);
}

int32_t main() noexcept {
  auto input = get_input();

  part1(input.patterns, input.towels);
  part2(input.patterns, input.towels);

  return 0;
}
