#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <queue>
#include <string>

// NOTE: Can used map to track which saved costs to double check with the sample

#if 0
const char *const FILENAME = "sample.txt";
const int32_t SIZE = 15;
const bool TEST = true;
#else
const char *const FILENAME = "input.txt";
const int32_t SIZE = 141;
const bool TEST = false;
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

void get_maze(char maze[SIZE][SIZE]) noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  for (int32_t i = 0; std::getline(file, line); ++i) {
    std::strncpy(maze[i], line.c_str(), SIZE);
  }
}

[[nodiscard]] vec get_position(char c, const char maze[SIZE][SIZE]) noexcept {
  vec pos{};
  for (pos.y = 0; pos.y < SIZE; ++pos.y) {
    for (pos.x = 0; pos.x < SIZE; ++pos.x) {
      if (maze[pos.y][pos.x] == c) {
        return pos;
      }
    }
  }

  return {-1, -1};
}

[[nodiscard]] bool in_bounds(vec pos) noexcept {
  return pos.x >= 0 && pos.x < SIZE && pos.y >= 0 && pos.y < SIZE;
}

[[nodiscard]] bool a_star_costs(int32_t costs[SIZE][SIZE],
                                const char maze[SIZE][SIZE]) noexcept {
  for (int32_t y = 0; y < SIZE; ++y) {
    for (int32_t x = 0; x < SIZE; ++x) {
      costs[y][x] = INT32_MAX;
    }
  }

  Move move{};
  vec pos{};
  vec end_pos = get_position('E', maze);
  std::priority_queue<Move, std::vector<Move>, MoveCostCompare> queue{};
  queue.push({.cost = 0, .pos = get_position('S', maze)});

  while (!queue.empty()) {
    move = queue.top();
    queue.pop();

    if (costs[move.pos.y][move.pos.x] != INT32_MAX) {
      continue;
    }

    costs[move.pos.y][move.pos.x] = move.cost;
    if (move.pos == end_pos) {
      return true;
    }

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

  return false;
}

inline void update_costs(int32_t &min_cost, int32_t &max_cost, vec pos,
                         const char maze[SIZE][SIZE],
                         const int32_t costs[SIZE][SIZE]) noexcept {
  if (maze[pos.y][pos.x] == '#') {
    return;
  }

  min_cost = std::min(min_cost, costs[pos.y][pos.x]);
  max_cost = std::max(max_cost, costs[pos.y][pos.x]);
}

[[nodiscard]] int32_t
cheat_maze_2_seconds(const int32_t minimum_saved, const char maze[SIZE][SIZE],
                     const int32_t costs[SIZE][SIZE]) noexcept {
  int32_t count = 0;
  int32_t min_cost = 0;
  int32_t max_cost = 0;
  int32_t saved = 0;
  vec pos{};

  // Remove the edges
  for (pos.y = 1; pos.y < SIZE - 1; ++pos.y) {
    for (pos.x = 1; pos.x < SIZE - 1; ++pos.x) {
      if (maze[pos.y][pos.x] != '#') {
        continue;
      }

      min_cost = INT32_MAX;
      max_cost = INT32_MIN;

      update_costs(min_cost, max_cost, pos + RIGHT, maze, costs);
      update_costs(min_cost, max_cost, pos + LEFT, maze, costs);
      update_costs(min_cost, max_cost, pos + DOWN, maze, costs);
      update_costs(min_cost, max_cost, pos + UP, maze, costs);

      if (
          // No path
          min_cost == INT32_MAX
          // Only 1 path opened
          || min_cost == max_cost) {
        continue;
      }

      saved = max_cost - min_cost - 2;
      if (saved >= minimum_saved) {
        // NOTE: Can change to std::map
        ++count;
      }
    }
  }

  return count;
}

[[nodiscard]] int32_t
cheat_maze_seconds(const int32_t minimum_saved, const int32_t cheat,
                   const char maze[SIZE][SIZE],
                   const int32_t costs[SIZE][SIZE]) noexcept {
  int32_t count = 0;
  int32_t min_cost = 0;
  int32_t new_cost = 0;
  int32_t saved = 0;
  int32_t y_magnitude = 0;
  int32_t x_end = 0;
  vec pos{};
  vec offset{};
  vec new_pos{};

  const vec edge_case_offsets[] = {{0, -20}, {-20, 0}, {20, 0}, {0, 20}};

  for (pos.y = 1; pos.y < SIZE - 1; ++pos.y) {
    for (pos.x = 1; pos.x < SIZE - 1; ++pos.x) {
      if (maze[pos.y][pos.x] == '#') {
        continue;
      }

      min_cost = costs[pos.y][pos.x];

      // Find any costs that is lower than the min_cost + move
      // Diamond pattern
      for (offset.y = -cheat; offset.y <= cheat; ++offset.y) {
        y_magnitude = std::abs(offset.y);
        x_end = cheat - y_magnitude;

        for (offset.x = -x_end; offset.x <= x_end; ++offset.x) {
          new_pos = pos + offset;
          if (!in_bounds(new_pos) || maze[new_pos.y][new_pos.x] == '#' ||
              min_cost >= costs[new_pos.y][new_pos.x]) {
            continue;
          }

          saved = costs[new_pos.y][new_pos.x] -
                  (min_cost + y_magnitude + std::abs(offset.x));
          if (saved >= minimum_saved) {
            // NOTE: Can change to std::map
            ++count;
          }
        }
      }
    }
  }

  return count;
}

int32_t main() noexcept {
  char maze[SIZE][SIZE];
  int32_t costs[SIZE][SIZE];

  get_maze(maze);
  if (!a_star_costs(costs, maze)) {
    printf("Could not create costs grid\n");
    return -1;
  }

  printf("Part 1: %d\n", cheat_maze_2_seconds(TEST ? 1 : 100, maze, costs));
  printf("Part 2: %d\n", cheat_maze_seconds(TEST ? 50 : 100, 20, maze, costs));

  return 0;
}
