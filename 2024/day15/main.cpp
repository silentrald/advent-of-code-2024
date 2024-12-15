#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdint.h>
#include <string>
#include <utility>

#if 0
const char *const FILENAME = "sample.txt";
const int32_t SIZE = 8;
#elif 0
const char *const FILENAME = "sample-3.txt";
const int32_t SIZE = 7;
#elif 0
const char *const FILENAME = "sample-2.txt";
const int32_t SIZE = 10;
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

  vec operator*(int32_t magnitude) noexcept {
    return {.x = this->x * magnitude, .y = this->y * magnitude};
  }

  vec &operator*=(int32_t magnitude) noexcept {
    this->x *= magnitude;
    this->y *= magnitude;
    return *this;
  }

  vec operator/(int32_t magnitude) noexcept {
    return {.x = this->x / magnitude, .y = this->y / magnitude};
  }

  vec &operator/=(int32_t magnitude) noexcept {
    this->x /= magnitude;
    this->y /= magnitude;
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

[[nodiscard]] std::string read_file(char grid[SIZE][SIZE]) noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  std::string movement{};

  for (int32_t i = 0; i < SIZE; ++i) {
    std::getline(file, line);
    std::strncpy(grid[i], line.c_str(), SIZE);
  }

  // \n
  std::getline(file, line);

  while (std::getline(file, line)) {
    movement.append(line);
  }

  return std::move(movement);
}

void print_grid(const char grid[SIZE][SIZE]) noexcept {
  for (int32_t r = 0; r < SIZE; ++r) {
    printf("%.*s\n", SIZE, grid[r]);
  }
}

void move_robot(char grid[SIZE][SIZE], const std::string &movement) noexcept {
  vec pos{-1, -1};
  for (int32_t y = 0; y < SIZE && pos == vec{-1, -1}; ++y) {
    for (int32_t x = 0; x < SIZE; ++x) {
      if (grid[y][x] == '@') {
        pos.x = x;
        pos.y = y;
        break;
      }
    }
  }

  vec dir{};
  for (const auto &m : movement) {
    switch (m) {
    case '>':
      dir = {1, 0};
      break;

    case '<':
      dir = {-1, 0};
      break;

    case '^':
      dir = {0, -1};
      break;

    case 'v':
      dir = {0, 1};
      break;
    }

    // Scan O until a wall
    for (vec new_pos = pos + dir;; new_pos += dir) {
      if (grid[new_pos.y][new_pos.x] == '#') {
        break;
      }

      if (grid[new_pos.y][new_pos.x] == '.') {
        for (vec tmp = new_pos - dir; new_pos != pos;
             tmp -= dir, new_pos -= dir) {
          std::swap(grid[tmp.y][tmp.x], grid[new_pos.y][new_pos.x]);
        }
        pos += dir;
        break;
      }
    }
  }
}

[[nodiscard]] int32_t compute_gps(const char grid[SIZE][SIZE]) noexcept {
  int32_t gps = 0;

  vec pos{};
  for (pos.y = 1; pos.y < SIZE - 1; ++pos.y) {
    for (pos.x = 1; pos.x < SIZE - 1; ++pos.x) {
      gps += (grid[pos.y][pos.x] == 'O') * (100 * pos.y + pos.x);
    }
  }

  return gps;
}

[[nodiscard]] std::string read_file_double(char grid[SIZE][SIZE * 2]) noexcept {
  std::ifstream file{FILENAME};
  std::string line{};
  std::string new_line{};

  std::string movement{};

  for (int32_t i = 0; i < SIZE; ++i) {
    std::getline(file, line);

    new_line.clear();
    for (const auto &c : line) {
      switch (c) {
      case '#':
        new_line.append("##");
        break;

      case 'O':
        new_line.append("[]");
        break;

      case '.':
        new_line.append("..");
        break;

      case '@':
        new_line.append("@.");
        break;
      }
    }
    std::strncpy(grid[i], new_line.c_str(), SIZE * 2);
  }

  // \n
  std::getline(file, line);

  while (std::getline(file, line)) {
    movement.append(line);
  }

  return std::move(movement);
}

void print_grid_double(const char grid[SIZE][SIZE * 2]) noexcept {
  for (int32_t r = 0; r < SIZE; ++r) {
    printf("%.*s\n", SIZE * 2, grid[r]);
  }
}

[[nodiscard]] bool
validate_move_double(vec pos, vec dir,
                     const char grid[SIZE][SIZE * 2]) noexcept {
  switch (grid[pos.y][pos.x]) {
  case '#':
    return false;

  case '.':
    return true;

  case '[':
    return dir.x ? validate_move_double(pos + (dir * 2), dir, grid)
                 : validate_move_double(pos + dir, dir, grid) &&
                       validate_move_double(pos + vec{1, dir.y}, dir, grid);

  case ']':
    return dir.x ? validate_move_double(pos + (dir * 2), dir, grid)
                 : validate_move_double(pos + dir, dir, grid) &&
                       validate_move_double(pos + vec{-1, dir.y}, dir, grid);
    break;
  }

  return false;
}

void update_grid_double(vec pos, vec dir, char grid[SIZE][SIZE * 2]) noexcept {
  if (dir.x) {
    int32_t x;
    for (x = pos.x + dir.x; grid[pos.y][x] != '.'; x += dir.x)
      ;

    if (dir.x > 0) {
      std::memcpy(grid[pos.y] + pos.x + 1, grid[pos.y] + pos.x, x - pos.x);
    } else {
      std::memcpy(grid[pos.y] + x, grid[pos.y] + x + 1, pos.x - x);
    }
    grid[pos.y][pos.x] = '.';
    return;
  }

  int32_t y = pos.y + dir.y;
  char line[SIZE * 2 - 2];
  bool running = true;

  for (int32_t i = 0; i < SIZE * 2 - 2; ++i) {
    line[i] = '.';
  }
  line[pos.x] = '@';
  grid[pos.y][pos.x] = '.';

  while (running) {
    for (int32_t x = 2; x < SIZE * 2 - 2; ++x) {
      if (line[x] != '@' && line[x] != '[' && line[x] != ']') {
        continue;
      }

      switch (grid[y][x]) {
      case '[':
        std::swap(grid[y][x], line[x]);
        std::swap(grid[y][x + 1], line[x + 1]);
        ++x;
        break;

      case ']':
        std::swap(grid[y][x], line[x]);
        std::swap(grid[y][x - 1], line[x - 1]);
        break;

      case '.':
        std::swap(grid[y][x], line[x]);
        break;
      }
    }

    running = false;
    for (int32_t x = 2; x < SIZE * 2 - 2; ++x) {
      if (line[x] != '.') {
        running = true;
        y += dir.y;
        break;
      }
    }
  }
}

void move_robot_double(char grid[SIZE][SIZE * 2],
                       const std::string &movement) noexcept {
  vec pos{-1, -1};
  for (int32_t y = 1; y < SIZE - 1 && pos == vec{-1, -1}; ++y) {
    for (int32_t x = 2; x < SIZE * 2 - 2; ++x) {
      if (grid[y][x] == '@') {
        pos.x = x;
        pos.y = y;
        break;
      }
    }
  }

  vec dir{};
  int32_t i = 0;
  for (const auto &m : movement) {
    switch (m) {
    case '>':
      dir = {1, 0};
      break;

    case '<':
      dir = {-1, 0};
      break;

    case '^':
      dir = {0, -1};
      break;

    case 'v':
      dir = {0, 1};
      break;
    }

    // Scan O until a wall
    if (validate_move_double(pos + dir, dir, grid)) {
      update_grid_double(pos, dir, grid);
      pos += dir;
    }
  }
}

[[nodiscard]] int32_t
compute_gps_double(const char grid[SIZE][SIZE * 2]) noexcept {
  int32_t gps = 0;

  vec pos{};
  for (pos.y = 1; pos.y < SIZE - 1; ++pos.y) {
    for (pos.x = 2; pos.x < SIZE * 2 - 2; ++pos.x) {
      gps += (grid[pos.y][pos.x] == '[') * (100 * pos.y + pos.x);
    }
  }

  return gps;
}

void part1() noexcept {
  char grid[SIZE][SIZE];
  std::string movement = read_file(grid);

  // print_grid(grid);
  move_robot(grid, movement);
  // print_grid(grid);
  // 1412971
  printf("Part 1: %d\n", compute_gps(grid));
}

void part2() noexcept {
  char grid[SIZE][SIZE * 2];
  std::string movement = read_file_double(grid);

  // print_grid_double(grid);
  move_robot_double(grid, movement);
  // print_grid_double(grid);
  // 1429299
  printf("Part 2: %d\n", compute_gps_double(grid));
}

int32_t main() noexcept {
  part1();
  part2();
  return 0;
}
