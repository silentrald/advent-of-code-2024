
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_set>

#if 0
const int32_t SIZE = 10;
const char *const FILENAME = "sample.txt";
#else
const int32_t SIZE = 130;
const char *const FILENAME = "input.txt";
#endif

struct vec {
  int32_t x = 0;
  int32_t y = 0;

  vec operator+(vec rhs) noexcept {
    return {.x = this->x + rhs.x, .y = this->y + rhs.y};
  }

  vec &operator+=(vec rhs) noexcept {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
  }

  vec operator-(vec rhs) noexcept {
    return {.x = this->x - rhs.x, .y = this->y - rhs.y};
  }

  vec &operator-=(vec rhs) noexcept {
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
  }

  bool operator==(vec rhs) noexcept {
    return this->x == rhs.x && this->y == rhs.y;
  }

  int64_t operator<(vec rhs) noexcept {
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

struct VecCompare {
  int64_t operator()(vec v1, vec v2) const noexcept {
    if (v1.x - v2.x) {
      return v1.x - v2.x;
    }
    return v1.y - v2.y;
  }
};

void get_maze(char maze[SIZE][SIZE]) {
  std::ifstream file{FILENAME};
  std::string line;

  for (int32_t i = 0; i < SIZE; ++i) {
    std::getline(file, line);
    std::strncpy(maze[i], line.c_str(), SIZE);
  }

  file.close();
}

void print_maze(char maze[SIZE][SIZE]) noexcept {
  printf("=== MAZE ===\n");
  char ch;
  for (int32_t r = 0; r < SIZE; ++r) {
    for (int32_t c = 0; c < SIZE; ++c) {
      ch = maze[r][c];
      if (ch == '\0') {
        printf(".");
      } else if (ch <= 0b1111) {
        printf("%x", ch);
      } else {
        printf("%c", ch);
      }
    }
    printf("\n");
  }
}

[[nodiscard]] bool is_out_of_bounds(vec position) noexcept {
  return position.x < 0 || position.x >= SIZE || position.y < 0 ||
         position.y >= SIZE;
}

[[nodiscard]] vec guard_turn(vec direction) noexcept {
  return direction.x == 0 ? vec{.x = -direction.y, .y = 0}
                          : vec{.x = 0, .y = direction.x};
}

[[nodiscard]] bool guard_move(char maze[SIZE][SIZE], vec &guard,
                              vec &direction) noexcept {
  guard += direction;
  if (is_out_of_bounds(guard)) {
    return false;
  }

  if (maze[guard.y][guard.x] == '#') {
    guard -= direction;
    direction = guard_turn(direction);
  }

  return true;
}

inline uint8_t get_int_direction(vec dir) {
  return (dir.x) ? (dir.x > 0 ? 0b00'01 : 0b00'10)
                 : (dir.y > 0 ? 0b01'00 : 0b10'00);
}

bool is_loop(char maze[SIZE][SIZE], vec start, vec direction) noexcept {
  vec current{start};

  char copy[SIZE][SIZE];
  for (int32_t r = 0; r < SIZE; ++r) {
    for (int32_t c = 0; c < SIZE; ++c) {
      copy[r][c] = maze[r][c] == '#' ? '#' : '\0';
    }
  }

  uint8_t d;
  while (guard_move(copy, current, direction)) {
    d = get_int_direction(direction);
    if (copy[current.y][current.x] & d) {
      return true;
    }
    copy[current.y][current.x] |= d;
  }

  return false;
}

void part1(char maze[SIZE][SIZE], vec guard, vec direction) noexcept {
  int32_t visited = 0;
  do {
    if (maze[guard.y][guard.x] == '.') {
      maze[guard.y][guard.x] = 'X';
      ++visited;
    }
  } while (guard_move(maze, guard, direction));

  // print_maze(maze);
  printf("Part 1: %d\n", visited);
}

void part2(char maze[SIZE][SIZE], vec guard, vec direction) noexcept {
  std::unordered_set<vec, VecHash, VecEquals> visited{};
  vec forward{};
  int32_t count = 0;

  vec start{guard};

  do {
    forward = guard + direction;
    if (is_out_of_bounds(forward)) {
      break;
    }

    if (start == guard || maze[forward.y][forward.x] == '#' ||
        visited.find(forward) != visited.end()) {
      visited.insert(forward);
      continue;
    }
    visited.insert(forward);

    maze[forward.y][forward.x] = '#';
    count += is_loop(maze, guard, guard_turn(direction));
    maze[forward.y][forward.x] = '.';
  } while (guard_move(maze, guard, direction));

  printf("Part 2: %d\n", count);
}

[[nodiscard]] vec get_guard_position(const char maze[SIZE][SIZE]) noexcept {
  vec guard{.x = -1, .y = -1};

  // Find the guard
  for (int32_t r = 0; r < SIZE; ++r) {
    for (int32_t c = 0; c < SIZE; ++c) {
      if (maze[r][c] == '^') {
        guard.x = c;
        guard.y = r;
      }
    }

    if (guard.x != -1) {
      break;
    }
  }

  return guard;
}

int32_t main() {
  char maze[SIZE][SIZE];

  get_maze(maze);

  // print_maze(maze);

  vec guard = get_guard_position(maze);

  vec direction{.x = 0, .y = -1};
  char g = maze[guard.y][guard.x];
  maze[guard.y][guard.x] = '.';

  // Simulation
  part1(maze, guard, direction);

  // Reset the value
  get_maze(maze);
  direction = {.x = 0, .y = -1};
  maze[guard.y][guard.x] = '.';

  part2(maze, guard, direction);
}
