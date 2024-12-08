
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <uchar.h>
#include <unordered_map>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
#endif

[[nodiscard]] std::unordered_map<int32_t, std::vector<int32_t>>
parse_map(std::ifstream &file) {
  std::unordered_map<int32_t, std::vector<int32_t>> map{};

  std::string line{};
  int32_t i1, i2;
  char8_t c;

  while (std::getline(file, line)) {
    if (line == "") {
      break;
    }

    std::stringstream ss(line);
    ss >> i1 >> c >> i2;

    auto it = map.find(i1);
    if (it != map.end()) {
      it->second.push_back(i2);
      continue;
    }

    std::vector<int32_t> vec{};
    vec.push_back(i2);
    map.insert({i1, std::move(vec)});
  }

  return std::move(map);
}

int32_t main() {
  std::ifstream file{FILENAME};
  auto map = parse_map(file);

  std::string line{};
  std::vector<int32_t> current{};
  int32_t i;
  char8_t c;
  bool valid;
  int32_t p1 = 0;
  int32_t p2 = 0;

  while (std::getline(file, line)) {
    current.clear();
    std::stringstream ss(line);

    do {
      ss >> i;
      current.push_back(i);
    } while (ss >> c);

    valid = true;
    for (i = 0; valid && i < current.size(); ++i) {
      auto it = map.find(current[i]);
      if (it == map.end()) {
        continue;
      }

      for (int32_t j = 0; j < i; ++j) {
        if (std::find(it->second.begin(), it->second.end(), current[j]) !=
            it->second.end()) {
          valid = false;
          break;
        }
      }
    }

    // printf("[%d] %s\n", valid, line.c_str());
    if (valid) {
      // printf("%d + %d", accumulator, current[current.size() / 2]);
      p1 += current[current.size() / 2];
      continue;
    }

    for (i = 0; i < current.size(); ++i) {
      auto it = map.find(current[i]);
      if (it == map.end()) {
        continue;
      }

      for (int32_t j = 0; j < i; ++j) {
        if (std::find(it->second.begin(), it->second.end(), current[j]) !=
            it->second.end()) {
          std::swap(current[i], current[j]);
          i = j - 1;
          break;
        }
      }
    }

    p2 += current[current.size() / 2];
  }

  printf("Part 1: %d\n", p1);
  printf("Part 2: %d\n", p2);

  return 0;
}
