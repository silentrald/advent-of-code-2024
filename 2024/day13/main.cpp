#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
#endif

struct vec {
  int64_t x = 0;
  int64_t y = 0;

  void print() const noexcept { printf("(%ld, %ld)", this->x, this->y); }

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

struct Machine {
  vec a{};
  vec b{};
  vec prize{};

  void print() const noexcept {
    printf("A: (%ld,%ld) ; B: (%ld,%ld) ; P: (%ld,%ld)\n", this->a.x, this->a.y,
           this->b.x, this->b.y, this->prize.x, this->prize.y);
  }
};

void print_machines(const std::vector<Machine> &machines) noexcept {
  for (const auto &m : machines) {
    m.print();
  }
}

[[nodiscard]] std::vector<Machine> get_machines() noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  std::vector<Machine> machines{};
  Machine m{};
  while (std::getline(file, line)) {
    sscanf(line.c_str(), "Button A: X+%ld, Y+%ld\n", &m.a.x, &m.a.y);
    std::getline(file, line);
    sscanf(line.c_str(), "Button B: X+%ld, Y+%ld\n", &m.b.x, &m.b.y);
    std::getline(file, line);
    sscanf(line.c_str(), "Prize: X=%ld, Y=%ld\n", &m.prize.x, &m.prize.y);

    // White space
    std::getline(file, line);

    machines.push_back(m);
  }

  return std::move(machines);
}

// Checked out tutorial for kramer's rule
[[nodiscard]] int64_t
solve_machines(const std::vector<Machine> &machines) noexcept {
  int64_t tokens = 0;

  for (const auto &m : machines) {
    int64_t det = (m.a.x * m.b.y - m.a.y * m.b.x);
    int64_t a = (m.prize.x * m.b.y - m.prize.y * m.b.x) / det;
    int64_t b = (m.a.x * m.prize.y - m.a.y * m.prize.x) / det;
    if (m.a.x * a + m.b.x * b == m.prize.x &&
        m.a.y * a + m.b.y * b == m.prize.y) {
      tokens += a * 3 + b;
    }
  }

  return tokens;
}

int32_t main() noexcept {
  auto machines = get_machines();
  // print_machines(machines);
  // part1(machines);

  // 31065
  printf("Part 1: %ld\n", solve_machines(machines));

  for (auto &m : machines) {
    m.prize += vec{10000000000000, 10000000000000};
  }
  // 93866170395343
  printf("Part 2: %ld\n", solve_machines(machines));

  return 0;
}
