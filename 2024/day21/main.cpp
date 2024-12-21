#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
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

[[nodiscard]] std::vector<std::string> get_codes() noexcept {
  std::ifstream file{FILENAME};
  std::string line{};
  std::vector<std::string> codes{};

  while (std::getline(file, line)) {
    codes.push_back(line);
  }

  return std::move(codes);
}

[[nodiscard]] vec get_numpad_button_position(char c) noexcept {
  static const vec NUMPAD_BUTTONS[] = {
      // 0
      {1, 3},
      // 1, 2, 3
      {0, 2},
      {1, 2},
      {2, 2},
      // 4, 5, 6
      {0, 1},
      {1, 1},
      {2, 1},
      // 7, 8, 9
      {0, 0},
      {1, 0},
      {2, 0},
      // A
      {2, 3},
  };

  if (c == 'A') {
    return NUMPAD_BUTTONS[10];
  }

  if (c >= '0' && c <= '9') {
    return NUMPAD_BUTTONS[c - '0'];
  }

  return {-1, -1};
}

[[nodiscard]] std::string get_numpad_submovement(char src, char dst) noexcept {
  if (src == dst) {
    return "A";
  }

  std::string out{};
  const vec diff =
      get_numpad_button_position(dst) - get_numpad_button_position(src);
  const bool is_bot = src == '0' || src == 'A';
  const bool is_left = src == '1' || src == '4' || src == '7';

  // Ordering is important ๐·°(⋟﹏⋞)°·๐

  // The bottom row buttons cannot prioritize going left
  //   because of the blank/going outside rule
  if (!is_bot) {
    for (int32_t i = 0; i > diff.x; --i) {
      out.push_back('<');
    }
  }

  // Same reasoning above but with going down
  if (!is_left) {
    for (int32_t i = 0; i < diff.y; ++i) {
      out.push_back('v');
    }
  }

  for (int32_t i = 0; i > diff.y; --i) {
    out.push_back('^');
  }

  for (int32_t i = 0; i < diff.x; ++i) {
    out.push_back('>');
  }

  if (is_bot) {
    for (int32_t i = 0; i > diff.x; --i) {
      out.push_back('<');
    }
  }

  if (is_left) {
    for (int32_t i = 0; i < diff.y; ++i) {
      out.push_back('v');
    }
  }

  out.push_back('A');

  return std::move(out);
}

[[nodiscard]] std::string
get_numpad_movements(const std::string &code) noexcept {
  std::string moves{};
  char src = 'A';

  for (const auto &dst : code) {
    moves.append(get_numpad_submovement(src, dst));
    src = dst;
  }

  return std::move(moves);
}

[[nodiscard]] vec get_remote_button_position(char c) noexcept {
  switch (c) {
  case 'A':
    return {2, 0};

  case '^':
    return {1, 0};

  case '<':
    return {0, 1};

  case 'v':
    return {1, 1};

  case '>':
    return {2, 1};

  default:
    return {-1, -1};
  }
}

[[nodiscard]] std::string get_remote_submovement(char src, char dst) noexcept {
  if (src == dst) {
    return "A";
  }

  std::string out{};
  const vec diff =
      get_remote_button_position(dst) - get_remote_button_position(src);

  // Ordering is very very important (╥﹏╥)

  // The left button destination cannot prioritize going left
  //   because of the blank/going outside rule, needs to go down then left
  if (dst != '<') {
    for (int32_t i = 0; i > diff.x; --i) {
      out.push_back('<');
    }
  }

  for (int32_t i = 0; i < diff.y; ++i) {
    out.push_back('v');
  }

  // Up here is invalid (blank space), so move rightward then move up
  if (src != '<') {
    for (int32_t i = 0; i > diff.y; --i) {
      out.push_back('^');
    }
  }

  for (int32_t i = 0; i < diff.x; ++i) {
    out.push_back('>');
  }

  if (src == '<') {
    for (int32_t i = 0; i > diff.y; --i) {
      out.push_back('^');
    }
  }

  if (dst == '<') {
    for (int32_t i = 0; i > diff.x; --i) {
      out.push_back('<');
    }
  }

  out.push_back('A');

  return std::move(out);
}

[[nodiscard]] inline std::string get_key(int32_t level,
                                         std::string move) noexcept {
  return std::to_string(level) + ":" + move;
}

std::unordered_map<std::string, int64_t> memo{};
[[nodiscard]] int64_t get_remote_movements(const int32_t level,
                                           const std::string &move) noexcept {
  if (level == 0) {
    return move.size();
  }

  auto it = memo.find(get_key(level, move));
  if (it != memo.end()) {
    return it->second;
  }

  int64_t total = 0LL;
  char src = 'A';

  for (const auto &dst : move) {
    total += get_remote_movements(level - 1, get_remote_submovement(src, dst));
    src = dst;
  }

  memo.insert({get_key(level, move), total});
  return total;
}

[[nodiscard]] int32_t code_to_int(const std::string &code) noexcept {
  return (code[0] - '0') * 100 + (code[1] - '0') * 10 + code[2] - '0';
}

[[nodiscard]] int64_t get_complexity(const std::vector<std::string> &codes,
                                     const int32_t remotes) noexcept {
  int64_t complexity = 0LL;
  int64_t tmp;

  for (const auto &code : codes) {
    tmp = get_remote_movements(remotes, get_numpad_movements(code));
    printf("%ld * %d\n", tmp, code_to_int(code));
    complexity += tmp * code_to_int(code);
  }

  return complexity;
}

int32_t main() noexcept {
  auto codes = get_codes();

  // 203734
  printf("Part 1: %ld\n", get_complexity(codes, 2));

  // 246810588779586
  printf("Part 2: %ld\n", get_complexity(codes, 25));

  return 0;
}
