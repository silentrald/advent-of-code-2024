#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

#if 0
#if 0
const char *const FILENAME = "sample.txt";
const int32_t SIZE = 4;
#else
const char *const FILENAME = "sample-2.txt";
const int32_t SIZE = 6;
#endif
#else
const char *const FILENAME = "input.txt";
const int32_t SIZE = 140;
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

  bool operator!=(vec rhs) const noexcept {
    return this->x != rhs.x || this->y != rhs.y;
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

vec dirs[] = {
    // UP
    {.x = 0, .y = -1},
    // DOWN
    {.x = 0, .y = 1},
    // LEFT
    {.x = -1, .y = 0},
    // RIGHT
    {.x = 1, .y = 0},
};

void get_garden(char grid[SIZE][SIZE]) noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  for (int32_t i = 0; i < SIZE; ++i) {
    std::getline(file, line);
    std::strncpy(grid[i], line.c_str(), SIZE);
  }
}

void print_garden(const char grid[SIZE][SIZE]) noexcept {
  for (int32_t r = 0; r < SIZE; ++r) {
    printf("%.*s\n", SIZE, grid[r]);
  }
}

[[nodiscard]] bool in_bounds(vec pos) noexcept {
  return pos.x >= 0 && pos.x < SIZE && pos.y >= 0 && pos.y < SIZE;
}

[[nodiscard]] int32_t
flood_scan_garden(vec pos, bool region[SIZE][SIZE],
                  const char garden[SIZE][SIZE]) noexcept {
  int32_t count = 1;
  char c = garden[pos.y][pos.x];
  vec new_pos{};

  region[pos.y][pos.x] = true;

  std::vector<vec> stack{};
  stack.push_back(pos);
  while (!stack.empty()) {
    pos = stack.back();
    stack.pop_back();

    for (int32_t i = 0; i < 4; ++i) {
      new_pos = pos + dirs[i];
      if (in_bounds(new_pos) && garden[new_pos.y][new_pos.x] == c &&
          !region[new_pos.y][new_pos.x]) {
        region[new_pos.y][new_pos.x] = true;
        ++count;
        stack.push_back(new_pos);
      }
    }
  }

  return count;
}

[[nodiscard]] int32_t get_perimeter(const bool region[SIZE][SIZE]) noexcept {
  int32_t perimeter = 0;

  vec pos{};
  vec new_pos{};
  for (pos.y = 0; pos.y < SIZE; ++pos.y) {
    for (pos.x = 0; pos.x < SIZE; ++pos.x) {
      if (!region[pos.y][pos.x]) {
        continue;
      }

      for (int32_t i = 0; i < 4; ++i) {
        new_pos = pos + dirs[i];
        if (!in_bounds(new_pos) || !region[new_pos.y][new_pos.x]) {
          ++perimeter;
        }
      }
    }
  }

  return perimeter;
}

void print_region(const bool b[SIZE][SIZE]) noexcept {
  for (int32_t r = 0; r < SIZE; ++r) {
    for (int32_t c = 0; c < SIZE; ++c) {
      printf("%d", b[r][c]);
    }
    printf("\n");
  }
}

void part1(const char garden[SIZE][SIZE]) noexcept {
  int32_t visited_count = 0;
  int32_t area = 0;
  int32_t perimeter = 0;
  int32_t cost = 0;
  bool visited[SIZE][SIZE];
  bool region[SIZE][SIZE];

  for (int32_t r = 0; r < SIZE; ++r) {
    std::memset(visited[r], false, SIZE);
    std::memset(region[r], false, SIZE);
  }

  vec pos{};
  while (visited_count < SIZE * SIZE) {
    for (pos.y = 0; pos.y < SIZE; ++pos.y) {
      for (pos.x = 0; pos.x < SIZE; ++pos.x) {
        if (visited[pos.y][pos.x]) {
          continue;
        }

        area = flood_scan_garden(pos, region, garden);
        visited_count += area;
        perimeter = get_perimeter(region);
        cost += area * perimeter;

        for (int32_t r = 0; r < SIZE; ++r) {
          for (int32_t c = 0; c < SIZE; ++c) {
            if (region[r][c]) {
              region[r][c] = false;
              visited[r][c] = true;
            }
          }
        }
      }
    }
  }

  // 1371306
  printf("Part 1: %d\n", cost);
}

[[nodiscard]] vec turn_right(vec direction) noexcept {
  return {.x = direction.y * -1, .y = direction.x};
}

[[nodiscard]] vec turn_left(vec direction) noexcept {
  return {.x = direction.y, .y = direction.x * -1};
}

// Try to flood scan the hole within the specific region
void flood_scan_region(vec pos, bool inside_region[SIZE][SIZE],
                       const bool region[SIZE][SIZE]) noexcept {
  vec new_pos{};
  inside_region[pos.y][pos.x] = true;

  std::vector<vec> stack{};
  stack.push_back(pos);
  while (!stack.empty()) {
    pos = stack.back();
    stack.pop_back();

    for (int32_t i = 0; i < 4; ++i) {
      new_pos = pos + dirs[i];
      if (in_bounds(new_pos) && !region[new_pos.y][new_pos.x] &&
          !inside_region[new_pos.y][new_pos.x]) {
        inside_region[new_pos.y][new_pos.x] = true;
        stack.push_back(new_pos);
      }
    }
  }
}

[[nodiscard]] int32_t get_sides(vec pos, const bool region[SIZE][SIZE],
                                bool is_hole = false) noexcept {
  int32_t sides = 0;
  vec direction{1, 0};
  vec new_pos{};
  std::unordered_set<vec, VecHash, VecEquals> outline{};

  --pos.y;
  vec end = pos;
  do {
    outline.insert(pos);

    new_pos = pos + turn_right(direction);
    if ((!in_bounds(pos + direction) && !in_bounds(new_pos)) ||
        !region[new_pos.y][new_pos.x]) {
      direction = turn_right(direction);
      pos += direction;
      ++sides;
      continue;
    }

    new_pos = pos + direction;
    if (in_bounds(new_pos) && region[new_pos.y][new_pos.x]) {
      direction = turn_left(direction);
      ++sides;
      continue;
    }

    pos += direction;
  } while (pos != end);

  if (is_hole) {
    return sides;
  }

  // Search for holes
  std::vector<vec> stack{};
  bool visited[SIZE][SIZE];
  bool inside_region[SIZE][SIZE];
  for (int32_t r = 0; r < SIZE; ++r) {
    std::memset(visited[r], false, SIZE);
    std::memset(inside_region[r], false, SIZE);
  }

  stack.push_back(end + vec{0, 1});
  while (!stack.empty()) {
    pos = stack.back();
    stack.pop_back();

    if (visited[pos.y][pos.x]) {
      continue;
    }
    visited[pos.y][pos.x] = true;

    for (int32_t i = 0; i < 4; ++i) {
      new_pos = pos + dirs[i];

      if (outline.find(new_pos) != outline.end() ||
          visited[new_pos.y][new_pos.x]) {
        continue;
      }

      if (region[new_pos.y][new_pos.x]) {
        stack.push_back(new_pos);
        continue;
      }

      // Hole found in the region

      flood_scan_region(new_pos, inside_region, region);
      for (new_pos += vec{0, -1};
           in_bounds(new_pos) && inside_region[new_pos.y][new_pos.x];)
        new_pos += vec{0, -1};
      sides += get_sides(new_pos + vec{0, 1}, inside_region, true);

      for (int32_t r = 0; r < SIZE; ++r) {
        for (int32_t c = 0; c < SIZE; ++c) {
          if (inside_region[r][c]) {
            inside_region[r][c] = false;
            visited[r][c] = true;
          }
        }
      }
    }
  }

  return sides;
}

void part2(const char garden[SIZE][SIZE]) noexcept {
  int32_t visited_count = 0;
  int32_t area = 0;
  int32_t sides = 0;
  int32_t cost = 0;
  bool visited[SIZE][SIZE];
  bool region[SIZE][SIZE];

  for (int32_t r = 0; r < SIZE; ++r) {
    std::memset(visited[r], false, SIZE);
    std::memset(region[r], false, SIZE);
  }

  vec pos{};
  while (visited_count < SIZE * SIZE) {
    for (pos.y = 0; pos.y < SIZE; ++pos.y) {
      for (pos.x = 0; pos.x < SIZE; ++pos.x) {
        if (visited[pos.y][pos.x]) {
          continue;
        }

        area = flood_scan_garden(pos, region, garden);
        visited_count += area;
        sides = get_sides(pos, region);
        printf("[%c] %3d * %3d = %3d\n", garden[pos.y][pos.x], area, sides,
               area * sides);
        cost += area * sides;

        for (int32_t r = 0; r < SIZE; ++r) {
          for (int32_t c = 0; c < SIZE; ++c) {
            if (region[r][c]) {
              visited[r][c] = true;
            }
            region[r][c] = false;
          }
        }
      }
    }
  }

  // 777048 X
  // 805880
  printf("Part 2: %d\n", cost);
}

int32_t main() noexcept {
  char garden[SIZE][SIZE];
  get_garden(garden);
  // print_garden(garden);

  part1(garden);
  part2(garden);

  return 0;
}
