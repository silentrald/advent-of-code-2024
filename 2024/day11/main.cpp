#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <unordered_map>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
#endif

[[nodiscard]] std::vector<int64_t> get_stones() noexcept {
  std::ifstream file{FILENAME};
  int64_t i;

  std::vector<int64_t> stones{};
  while (file >> i) {
    stones.push_back(i);
  }

  return std::move(stones);
}

[[nodiscard]] int32_t count_digits(int64_t i) noexcept {
  int64_t count = 0;
  for (; i > 0; ++count, i /= 10)
    ;
  return count;
}

inline void upsert(std::unordered_map<int64_t, int64_t> *map, int64_t key,
                   int64_t value) noexcept {
  auto it = map->find(key);
  if (it == map->end()) {
    map->insert({key, value});
  } else {
    it->second += value;
  }
}

[[nodiscard]] int64_t calculate_blinking(const std::vector<int64_t> &stones,
                                         const int64_t blinking) noexcept {
  std::unordered_map<int64_t, int64_t> stone_map1{};
  std::unordered_map<int64_t, int64_t> stone_map2{};
  std::unordered_map<int64_t, int64_t> *map_source = nullptr;
  std::unordered_map<int64_t, int64_t> *map_destination = nullptr;
  int64_t count = 0;
  int64_t digits = 0;

  for (const auto &s : stones) {
    upsert(&stone_map1, s, 1);
  }

  for (int32_t _b = 0; _b < blinking; ++_b) {
    // printf("Blinking %d\n", _b);

    if (_b & 1) {
      map_source = &stone_map2;
      map_destination = &stone_map1;
    } else {
      map_source = &stone_map1;
      map_destination = &stone_map2;
    }
    map_destination->clear();

    // print_map(map_source);

    for (const auto &pair : *map_source) {
      // 0 => 1
      if (pair.first == 0) {
        upsert(map_destination, 1, pair.second);
        continue;
      }

      // Split if even
      digits = count_digits(pair.first);
      if (!(digits & 1)) {
        digits = std::pow(10, digits / 2);
        upsert(map_destination, pair.first % digits, pair.second);
        upsert(map_destination, pair.first / digits, pair.second);
        continue;
      }

      upsert(map_destination, pair.first * 2024, pair.second);
    }
  }

  for (const auto &pair : *map_destination) {
    count += pair.second;
  }
  return count;
}

int32_t main() noexcept {
  auto stones = get_stones();

  // 189092
  printf("Part 1: %ld\n", calculate_blinking(stones, 25));
  // 224869647102559
  printf("Part 2: %ld\n", calculate_blinking(stones, 75));
  return 0;
}
