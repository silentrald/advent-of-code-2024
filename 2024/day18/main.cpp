#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <queue>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
const int32_t SIZE = 7;
const int32_t P1_BYTES = 12;
#else
const char *const FILENAME = "input.txt";
const int32_t SIZE = 71;
const int32_t P1_BYTES = 1024;
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

const vec RIGHT{1, 0};
const vec LEFT{-1, 0};
const vec UP{0, -1};
const vec DOWN{0, 1};

struct Move {
  int32_t cost;
  vec pos;

  void print() const noexcept {
    printf("[%d] (%d,%d)\n", this->cost, this->pos.x, this->pos.y);
  }
};

struct MoveCostCompare {
  bool operator()(Move m1, Move m2) noexcept { return m1.cost > m2.cost; }
};

[[nodiscard]] std::vector<vec> get_falling_bytes() noexcept {
  std::vector<vec> bytes{};
  vec pos{};
  char g; // for ,

  std::ifstream file{FILENAME};
  while (file >> pos.x >> g >> pos.y) {
    bytes.push_back(pos);
  }

  return std::move(bytes);
}

void create_maze(const std::vector<vec> &bytes, const int32_t num,
                 char maze[SIZE][SIZE]) noexcept {
  // Clear the maze
  for (int32_t y = 0; y < SIZE; ++y) {
    for (int32_t x = 0; x < SIZE; ++x) {
      maze[y][x] = '.';
    }
  }

  for (int32_t i = 0; i < num; ++i) {
    maze[bytes[i].y][bytes[i].x] = '#';
  }
}

void print_maze(const char maze[SIZE][SIZE]) noexcept {
  for (int32_t i = 0; i < SIZE; ++i) {
    printf("%.*s\n", SIZE, maze[i]);
  }
}

[[nodiscard]] bool in_bounds(vec pos) noexcept {
  return pos.x >= 0 && pos.x < SIZE && pos.y >= 0 && pos.y < SIZE;
}

[[nodiscard]] int32_t a_star(const char maze[SIZE][SIZE]) noexcept {
  bool visited[SIZE][SIZE];
  for (int32_t i = 0; i < SIZE; ++i) {
    std::memset(visited[i], false, SIZE);
  }

  Move move{};
  vec pos{};
  std::priority_queue<Move, std::vector<Move>, MoveCostCompare> queue{};
  queue.push({.cost = 0, .pos = {0, 0}});

  while (!queue.empty()) {
    move = queue.top();
    queue.pop();

    if (visited[move.pos.y][move.pos.x]) {
      continue;
    }

    if (move.pos.x == SIZE - 1 && move.pos.y == SIZE - 1) {
      return move.cost;
    }

    visited[move.pos.y][move.pos.x] = true;

    pos = move.pos + RIGHT;
    if (in_bounds(pos) && maze[pos.y][pos.x] != '#') {
      queue.push({move.cost + 1, pos});
    }

    pos = move.pos + DOWN;
    if (in_bounds(pos) && maze[pos.y][pos.x] != '#') {
      queue.push({move.cost + 1, pos});
    }

    pos = move.pos + LEFT;
    if (in_bounds(pos) && maze[pos.y][pos.x] != '#') {
      queue.push({move.cost + 1, pos});
    }

    pos = move.pos + UP;
    if (in_bounds(pos) && maze[pos.y][pos.x] != '#') {
      queue.push({move.cost + 1, pos});
    }
  }

  return -1;
}

void part1(const std::vector<vec> &bytes) noexcept {
  char maze[SIZE][SIZE];
  create_maze(bytes, P1_BYTES, maze);
  // print_maze(maze);

  // 288
  printf("Part 1: %d\n", a_star(maze));
}

void part2(const std::vector<vec> &bytes) noexcept {
  char maze[SIZE][SIZE];

  vec b{-1, -1};
  for (int32_t i = P1_BYTES + 1; i <= bytes.size(); ++i) {
    // printf("Testing %d (%d,%d)\n", i, b.x, b.y);
    create_maze(bytes, i, maze);
    if (a_star(maze) == -1) {
      b = bytes[i - 1];
      break;
    }
  }

  printf("Part 2: %d,%d\n", b.x, b.y);
}

int32_t main() noexcept {
  auto bytes = get_falling_bytes();

  part1(bytes);
  part2(bytes);

  return 0;
}
