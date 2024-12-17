#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
const int32_t SIZE = 15;
#elif 0
const char *const FILENAME = "sample2.txt";
const int32_t SIZE = 17;
#else
const char *const FILENAME = "input.txt";
const int32_t SIZE = 141;
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

const int32_t FORWARD_COST = 1;
const int32_t TURN_COST = 1000;

struct Move {
  int32_t cost;
  vec pos;
  vec dir;

  void print() const noexcept {
    char c;
    if (this->dir == RIGHT) {
      c = '>';
    } else if (this->dir == LEFT) {
      c = '<';
    } else if (this->dir == UP) {
      c = '^';
    } else {
      c = 'v';
    }
    printf("%c (%d,%d)\n", c, this->pos.x, this->pos.y);
  }
};

struct MoveCostCompare {
  std::size_t operator()(Move m1, Move m2) noexcept {
    return m1.cost - m2.cost;
  }
};

void print_maze(const char maze[SIZE][SIZE],
                const int32_t costs[SIZE][SIZE]) noexcept {
  vec pos{};
  printf("=== MAZE ===\n");
  for (pos.y = 0; pos.y < SIZE; ++pos.y) {
    for (pos.x = 0; pos.x < SIZE; ++pos.x) {
      if (costs[pos.y][pos.x] < INT32_MAX) {
        printf("X");
      } else {
        printf("%c", maze[pos.y][pos.x]);
      }
    }
    printf("\n");
  }
}

void print_maze_cost(const int32_t costs[SIZE][SIZE]) noexcept {
  for (int32_t y = 0; y < SIZE; ++y) {
    for (int32_t x = 0; x < SIZE; ++x) {
      if (costs[y][x] == INT32_MAX) {
        printf("XXXXXX|");
      } else {
        printf("%6d|", costs[y][x]);
      }
    }
    printf("\n");
  }
}

void print_maze_seats(
    const char maze[SIZE][SIZE],
    const std::unordered_set<vec, VecHash, VecEquals> &seats) noexcept {
  vec pos{};
  printf("=== MAZE ===\n");
  for (pos.y = 0; pos.y < SIZE; ++pos.y) {
    for (pos.x = 0; pos.x < SIZE; ++pos.x) {
      if (seats.find(pos) != seats.end()) {
        printf("O");
      } else {
        printf("%c", maze[pos.y][pos.x]);
      }
    }
    printf("\n");
  }
}

void get_maze(char maze[SIZE][SIZE]) noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  for (int32_t i = 0; std::getline(file, line); ++i) {
    std::strncpy(maze[i], line.c_str(), SIZE);
  }
}

[[nodiscard]] vec find_character(const char maze[SIZE][SIZE], char c) noexcept {
  for (int32_t y = 0; y < SIZE; ++y) {
    for (int32_t x = 0; x < SIZE; ++x) {
      if (maze[y][x] == c) {
        return vec{x, y};
      }
    }
  }

  return {-1, -1};
}

[[nodiscard]] vec turn_right(vec dir) noexcept { return {dir.y * -1, dir.x}; }

[[nodiscard]] vec turn_left(vec dir) noexcept { return {dir.y, dir.x * -1}; }

void solve(const char maze[SIZE][SIZE]) noexcept {}

int32_t main() noexcept {
  char maze[SIZE][SIZE];
  get_maze(maze);

  // Just a* with a different heuristic cost
  const vec start_pos = find_character(maze, 'S');
  const vec end_pos = find_character(maze, 'E');

  int32_t min_cost = INT32_MAX;
  Move move{
      .cost = 0,
      .pos = start_pos,
      .dir = RIGHT,
  };
  int32_t cost;
  vec pos{};

  int32_t costs[SIZE][SIZE];

  // === Part 1 === //
  {
    vec dir{};
    std::priority_queue<Move, std::vector<Move>, MoveCostCompare> queue{};

    for (pos.y = 0; pos.y < SIZE; ++pos.y) {
      for (pos.x = 0; pos.x < SIZE; ++pos.x) {
        costs[pos.y][pos.x] = INT32_MAX;
      }
    }
    queue.push(move);

    while (!queue.empty()) {
      move = queue.top();
      queue.pop();

      if (maze[move.pos.y][move.pos.x] == 'E') {
        min_cost = std::min(min_cost, move.cost);
        costs[move.pos.y][move.pos.x] = min_cost;
        continue;
      }

      costs[move.pos.y][move.pos.x] =
          std::min(costs[move.pos.y][move.pos.x], move.cost);

      // Move straight
      cost = move.cost + FORWARD_COST;
      pos = move.pos + move.dir;
      if (cost < costs[pos.y][pos.x] && maze[pos.y][pos.x] != '#') {
        queue.push({move.cost + FORWARD_COST, pos, move.dir});
      }

      // Turn and move straight
      cost = move.cost + FORWARD_COST + TURN_COST;
      dir = turn_right(move.dir);
      pos = move.pos + dir;
      if (cost < costs[pos.y][pos.x] && maze[pos.y][pos.x] != '#') {
        queue.push({move.cost + FORWARD_COST + TURN_COST, pos, dir});
      }

      dir = turn_left(move.dir);
      pos = move.pos + dir;
      if (cost < costs[pos.y][pos.x] && maze[pos.y][pos.x] != '#') {
        queue.push({move.cost + FORWARD_COST + TURN_COST, pos, dir});
      }

      // Don't need to turn around
    }

    // 102460
    // print_maze_cost(costs);
    printf("Part 1: %d\n", min_cost);
  }

  // === Part 2 === //
  {
    // Backtracking, reuse the costs array
    std::unordered_set<vec, VecHash, VecEquals> seats{};

    std::vector<Move> backtrack{};
    backtrack.push_back({min_cost, end_pos, DOWN});
    backtrack.push_back({min_cost, end_pos, LEFT});
    backtrack.push_back({min_cost, end_pos, UP});
    backtrack.push_back({min_cost, end_pos, RIGHT});

    while (!backtrack.empty()) {
      move = backtrack.back();
      backtrack.pop_back();

      seats.insert(move.pos);
      if (start_pos == move.pos) {
        continue;
      }

      // Backward
      cost = move.cost - FORWARD_COST;
      pos = move.pos - move.dir;
      if (seats.find(pos) == seats.end() && costs[pos.y][pos.x] == cost) {
        backtrack.push_back({cost, pos, move.dir});
      }

      // Backward Turn
      cost = move.cost - FORWARD_COST - TURN_COST;
      pos = move.pos - move.dir;
      if (seats.find(pos) == seats.end() && costs[pos.y][pos.x] == cost) {
        backtrack.push_back({cost, pos, turn_right(move.dir)});
        backtrack.push_back({cost, pos, turn_left(move.dir)});

        // Double Backwards (Edge Case)
        // ###
        // >^>
        // #^#
        cost = move.cost - FORWARD_COST * 2;
        pos = move.pos - (move.dir * 2);
        if (seats.find(pos) == seats.end() && costs[pos.y][pos.x] == cost) {
          backtrack.push_back({cost, pos, move.dir});
        }
      }
    }

    // print_maze_seats(maze, seats);
    printf("Part 2: %ld\n", seats.size());
  }
  return 0;
}
