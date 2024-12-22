#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <unordered_map>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#elif 0
const char *const FILENAME = "sample2.txt";
#else
const char *const FILENAME = "input.txt";
#endif

[[nodiscard]] std::vector<int64_t> get_numbers() noexcept {
  std::ifstream file{FILENAME};
  int64_t i;

  std::vector<int64_t> numbers{};
  while (file >> i) {
    numbers.push_back(i);
  }

  return std::move(numbers);
}

int64_t hash(int64_t num) noexcept {
  num = (num ^ (num << 6)) & 0x00FF'FFFF;
  num = (num ^ (num >> 5)) & 0x00FF'FFFF;
  num = (num ^ (num << 11)) & 0x00FF'FFFF;
  return num;
}

void part1(const std::vector<int64_t> &numbers) noexcept {
  int64_t secret = 0;
  int64_t sum = 0;

  for (const auto &num : numbers) {
    secret = num;
    for (int32_t i = 0; i < 2000; ++i) {
      secret = hash(secret);
    }

    // printf("%ld: %ld\n", num, secret);
    sum += secret;
  }

  printf("Part 1: %ld\n", sum);
}

void part2(const std::vector<int64_t> &numbers) noexcept {
  int64_t secret = 0;
  int64_t new_secret = 0;

  // diff_hash should represent the 4 changes, 8 bit signed for each
  // eg.  1 2  3 4 => 0x01 | 0x02 | 0x03 | 0x04
  //     -2 1 -1 3 => 0xfe | 0x01 | 0xff | 0x03
  uint32_t diff_hash = 0;

  // Keep a running total for all changes, then find the max total later
  std::unordered_map<uint32_t, int32_t> changes_memo{};
  // Get the first occurence of each pattern for the current number
  std::unordered_map<uint32_t, int32_t> current_memo{};

  for (const auto &num : numbers) {
    secret = num;
    current_memo.clear();

    // Prepare the diff_hash before inserting to the current_memo
    for (int32_t _i = 0; _i < 3; ++_i) {
      new_secret = hash(secret);
      diff_hash =
          (diff_hash << 8) | (((new_secret % 10L) - (secret % 10L)) & 0xFF);
      secret = new_secret;
    }

    for (int32_t _i = 3; _i < 2000; ++_i) {
      new_secret = hash(secret);
      diff_hash =
          (diff_hash << 8) | (((new_secret % 10L) - (secret % 10L)) & 0xFF);
      secret = new_secret;

      auto it = current_memo.find(diff_hash);
      if (it == current_memo.end()) {
        current_memo.insert({diff_hash, secret % 10L});
      }
    }

    for (const auto &pair : current_memo) {
      auto it = changes_memo.find(pair.first);
      if (it == changes_memo.end()) {
        changes_memo.insert({pair.first, pair.second});
      } else {
        it->second += pair.second;
      }
    }
  }

  int32_t max = INT32_MIN;
  for (const auto &pair : changes_memo) {
    max = std::max(max, pair.second);
  }

  printf("Part 2: %d\n", max);
}

int32_t main() noexcept {
  auto numbers = get_numbers();

  part1(numbers);
  part2(numbers);

  return 0;
}
