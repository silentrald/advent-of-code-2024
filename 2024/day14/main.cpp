#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
const int32_t WIDTH = 11;
const int32_t LENGTH = 7;
#else
const char *const FILENAME = "input.txt";
const int32_t WIDTH = 101;
const int32_t LENGTH = 103;
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

struct Robot {
  vec pos;
  vec speed;
};

[[nodiscard]] std::vector<Robot> get_robots() noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  std::vector<Robot> robots{};
  Robot r{};

  while (std::getline(file, line)) {
    sscanf(line.c_str(), "p=%d,%d v=%d,%d", &r.pos.x, &r.pos.y, &r.speed.x,
           &r.speed.y);
    robots.push_back(r);
  }

  return std::move(robots);
}

void print_robots_position(const std::vector<Robot> &robots) noexcept {
  for (const auto &r : robots) {
    printf("Robot (%d, %d)\n", r.pos.x, r.pos.y);
  }
}

void print_robots_grid(const std::vector<Robot> &robots) noexcept {
  int8_t grid[LENGTH][WIDTH];
  for (int32_t r = 0; r < LENGTH; ++r) {
    std::memset(grid[r], 0, WIDTH);
  }

  for (const auto &r : robots) {
    ++grid[r.pos.y][r.pos.x];
  }

  for (int32_t r = 0; r < LENGTH; ++r) {
    for (int32_t c = 0; c < WIDTH; ++c) {
      // printf("%d", grid[r][c]);
      switch (grid[r][c]) {
      case 0:
        printf(".");
        break;

      case 1:
        printf("O");
        break;

      default:
        printf("X");
      }
    }
    printf("\n");
  }
}

[[nodiscard]] std::vector<Robot>
robot_simulation(std::vector<Robot> robots, // Copy the vector
                 int32_t seconds) noexcept {
  for (auto &r : robots) {
    r.pos += r.speed * seconds;
    r.pos.x %= WIDTH;
    if (r.pos.x < 0) {
      r.pos.x += WIDTH;
    }

    r.pos.y %= LENGTH;
    if (r.pos.y < 0) {
      r.pos.y += LENGTH;
    }
  }

  return std::move(robots);
}

[[nodiscard]] int32_t
get_safety_factor(const std::vector<Robot> &robots) noexcept {
  int32_t q1 = 0;
  int32_t q2 = 0;
  int32_t q3 = 0;
  int32_t q4 = 0;
  vec pos{};

  // Quadrant I
  for (const auto &r : robots) {
    if (r.pos.x == WIDTH / 2 || r.pos.y == LENGTH / 2) {
      continue;
    }

    if (r.pos.x < WIDTH / 2) {
      if (r.pos.y < LENGTH / 2) {
        ++q1;
      } else {
        ++q4;
      }
    } else {
      if (r.pos.y < LENGTH / 2) {
        ++q2;
      } else {
        ++q3;
      }
    }
  }

  return q1 * q2 * q3 * q4;
}

[[nodiscard]] bool is_easter_egg(const std::vector<Robot> robots) noexcept {
  int8_t grid[LENGTH][WIDTH];
  for (int32_t r = 0; r < LENGTH; ++r) {
    std::memset(grid[r], 0, WIDTH);
  }

  for (const auto &r : robots) {
    ++grid[r.pos.y][r.pos.x];
    if (grid[r.pos.y][r.pos.x] >= 2) {
      return false;
    }
  }

  return true;
}

[[nodiscard]] int32_t find_easter_egg(std::vector<Robot> robots) noexcept {
  for (int32_t s = 1;; ++s) {
    for (auto &r : robots) {
      r.pos += r.speed;

      r.pos.x %= WIDTH;
      if (r.pos.x < 0) {
        r.pos.x += WIDTH;
      }

      r.pos.y %= LENGTH;
      if (r.pos.y < 0) {
        r.pos.y += LENGTH;
      }
    }

    if (is_easter_egg(robots)) {
      print_robots_grid(robots);
      return s;
    }
  }
}

int32_t main() {
  auto robots = get_robots();

  // 229421808
  printf("Part 1: %d\n", get_safety_factor(robot_simulation(robots, 100)));
  printf("Part 2: %d\n", find_easter_egg(robots));

  return 0;
}
