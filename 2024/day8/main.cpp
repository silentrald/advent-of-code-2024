
#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
const uint32_t SIZE = 12U;
#else
const char *const FILENAME = "input.txt";
const uint32_t SIZE = 50U;
#endif

struct vec {
  int32_t x = 0;
  int32_t y = 0;

  void print() const noexcept { printf("(%d, %d)\n", this->x, this->y); }

  vec operator+(vec rhs) const noexcept {
    return {.x = this->x + rhs.x, .y = this->y + rhs.y};
  }

  vec &operator+=(vec rhs) noexcept {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
  }

  vec operator-(vec rhs) const noexcept {
    return {.x = this->x - rhs.x, .y = this->y - rhs.y};
  }

  vec &operator-=(vec rhs) noexcept {
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
  }

  bool operator==(vec rhs) const noexcept {
    return this->x == rhs.x && this->y == rhs.y;
  }

  int64_t operator<(vec rhs) const noexcept {
    if (this->x - rhs.x) {
      return this->x - rhs.x;
    }
    return this->y - rhs.y;
  }
};

struct VecHash {
  std::size_t operator()(vec v) const noexcept {
    return ((std::size_t)v.x << 32) || v.y;
  }
};

struct VecEquals {
  bool operator()(vec v1, vec v2) const noexcept {
    return v1.x == v2.x && v1.y == v2.y;
  }
};

void parse(char grid[SIZE][SIZE]) noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  for (int32_t i = 0; std::getline(file, line); ++i) {
    std::strncpy(grid[i], line.c_str(), SIZE);
  }

  file.close();
}

void print_grid(const char grid[SIZE][SIZE]) noexcept {
  for (int32_t r = 0; r < SIZE; ++r) {
    printf("%.*s\n", SIZE, grid[r]);
  }
}

[[nodiscard]] std::unordered_map<char, std::vector<vec>>
get_antennas(const char grid[SIZE][SIZE]) noexcept {
  std::unordered_map<char, std::vector<vec>> antennas{};

  char c;
  for (int32_t y = 0; y < SIZE; ++y) {
    for (int32_t x = 0; x < SIZE; ++x) {
      c = grid[y][x];
      if (c == '.') {
        continue;
      }

      auto it = antennas.find(c);
      if (it != antennas.end()) {
        it->second.push_back(vec{.x = x, .y = y});
        continue;
      }

      std::vector<vec> positions{};
      positions.push_back(vec{.x = x, .y = y});
      antennas.insert({c, std::move(positions)});
    }
  }

  return std::move(antennas);
}

[[nodiscard]] bool in_bounds(vec v) noexcept {
  return v.x >= 0 && v.x < SIZE && v.y >= 0 && v.y < SIZE;
}

void part1(const char grid[SIZE][SIZE]) noexcept {
  auto antennas = get_antennas(grid);

  std::unordered_set<vec, VecHash, VecEquals> antinodes{};
  vec node{};
  vec change{};
  for (const auto &same : antennas) {
    for (int32_t i = 0; i < same.second.size(); ++i) {
      for (int32_t j = i + 1; j < same.second.size(); ++j) {
        change = same.second[i] - same.second[j];

        node = same.second[i] + change;
        if (in_bounds(node)) {
          antinodes.insert(node);
        }

        node = same.second[j] - change;
        if (in_bounds(node)) {
          antinodes.insert(node);
        }
      }
    }
  }

  printf("Part 1: %ld\n", antinodes.size());
}

void part2(const char grid[SIZE][SIZE]) noexcept {
  auto antennas = get_antennas(grid);

  std::unordered_set<vec, VecHash, VecEquals> antinodes{};
  vec change{};
  for (const auto &same : antennas) {
    for (const auto &node : same.second) {
      antinodes.insert(node);
    }

    for (int32_t i = 0; i < same.second.size(); ++i) {
      for (int32_t j = i + 1; j < same.second.size(); ++j) {
        change = same.second[i] - same.second[j];
        for (vec node = same.second[i] + change; in_bounds(node);
             node += change) {
          antinodes.insert(node);
        }

        for (vec node = same.second[j] - change; in_bounds(node);
             node -= change) {
          antinodes.insert(node);
        }
      }
    }
  }

  printf("Part 2: %ld\n", antinodes.size());
}

int32_t main() noexcept {
  char grid[SIZE][SIZE];
  parse(grid);

  // print_grid(grid);

  part1(grid);
  part2(grid);

  return 0;
}
