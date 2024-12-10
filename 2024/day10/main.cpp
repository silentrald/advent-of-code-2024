#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_set>

#if 0
const char *const FILENAME = "sample.txt";
const int32_t SIZE = 8;
#else
const char *const FILENAME = "input.txt";
const int32_t SIZE = 50;
#endif

struct vec {
  int32_t x = 0;
  int32_t y = 0;

  void print() const noexcept { printf("(%d, %d)", this->x, this->y); }

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

const vec directions[] = {
    // RIGHT
    {.x = 1, .y = 0},
    // LEFT
    {.x = -1, .y = 0},
    // DOWN
    {.x = 0, .y = 1},
    // UP
    {.x = 0, .y = -1}};

void get_grid(char grid[SIZE][SIZE]) noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  for (int32_t i = 0; i < SIZE; ++i) {
    std::getline(file, line);
    std::strncpy(grid[i], line.c_str(), SIZE);
  }
}

[[nodiscard]] bool in_bounds(vec position) noexcept {
  return position.x >= 0 && position.x < SIZE && position.y >= 0 &&
         position.y < SIZE;
}

void get_trailhead(std::unordered_set<vec, VecHash, VecEquals> &visited,
                   vec position, const char grid[SIZE][SIZE],
                   char height) noexcept {
  for (int32_t i = 0; i < 4; ++i) {
    auto new_position = position + directions[i];
    if (!in_bounds(new_position) ||
        grid[new_position.y][new_position.x] != height) {
      continue;
    }

    if (height != '9') {
      get_trailhead(visited, new_position, grid, height + 1);
      continue;
    }

    visited.insert(new_position);
  }
}

void part1(const char grid[SIZE][SIZE]) noexcept {
  int32_t count = count;
  std::unordered_set<vec, VecHash, VecEquals> visited{};

  vec pos{};
  for (pos.y = 0; pos.y < SIZE; ++pos.y) {
    for (pos.x = 0; pos.x < SIZE; ++pos.x) {
      if (grid[pos.y][pos.x] != '0') {
        continue;
      }

      visited.clear();
      get_trailhead(visited, pos, grid, '1');
      count += visited.size();
    }
  }

  printf("Part 1: %d\n", count);
}

[[nodiscard]] int32_t get_unique_trailhead(vec position,
                                           const char grid[SIZE][SIZE],
                                           char height) noexcept {
  int32_t count = 0;
  for (int32_t i = 0; i < 4; ++i) {
    auto new_position = position + directions[i];
    if (!in_bounds(new_position) ||
        grid[new_position.y][new_position.x] != height) {
      continue;
    }

    if (height != '9') {
      count += get_unique_trailhead(new_position, grid, height + 1);
      continue;
    }

    ++count;
  }
  return count;
}

void part2(const char grid[SIZE][SIZE]) noexcept {
  int32_t count = count;

  vec pos{};
  for (pos.y = 0; pos.y < SIZE; ++pos.y) {
    for (pos.x = 0; pos.x < SIZE; ++pos.x) {
      if (grid[pos.y][pos.x] != '0') {
        continue;
      }

      count += get_unique_trailhead(pos, grid, '1');
    }
  }

  printf("Part 2: %d\n", count);
}

int32_t main() noexcept {
  char grid[SIZE][SIZE];
  get_grid(grid);

  part1(grid);
  part2(grid);

  return 0;
}
