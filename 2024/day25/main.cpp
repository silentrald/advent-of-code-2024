#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
#endif

struct Input {
  std::vector<std::array<int32_t, 5>> locks{};
  std::vector<std::array<int32_t, 5>> keys{};

  void print() const noexcept {
    for (const auto &l : this->locks) {
      printf("Lock %d,%d,%d,%d,%d\n", l[0], l[1], l[2], l[3], l[4]);
    }
    for (const auto &k : this->keys) {
      printf(" Key %d,%d,%d,%d,%d\n", k[0], k[1], k[2], k[3], k[4]);
    }

    printf("Locks: %ld ; Keys: %ld\n", this->locks.size(), this->keys.size());
  }
};

[[nodiscard]] Input get_input() noexcept {
  std::ifstream file{FILENAME};
  std::array<std::string, 7> lines{};

  Input input{};
  std::array<int32_t, 5> row{};
  while (std::getline(file, lines[0])) {
    for (int32_t i = 1; i < lines.size(); ++i) {
      std::getline(file, lines[i]);
    }

    for (auto &r : row) {
      r = 0;
    }

    if (lines[0][0] == '#') {
      for (int32_t x = 0; x < row.size(); ++x) {
        for (int32_t y = 1; y < lines.size() - 1; ++y) {
          if (lines[y][x] != '#') {
            break;
          }
          ++row[x];
        }
      }
      input.locks.push_back(row);
    } else {
      for (int32_t x = 0; x < row.size(); ++x) {
        for (int32_t y = lines.size() - 2; y > 0; --y) {
          if (lines[y][x] != '#') {
            break;
          }
          ++row[x];
        }
      }
      input.keys.push_back(row);
    }

    // \n
    std::getline(file, lines[0]);
  }

  return std::move(input);
}

// 250 * 250 calculation > naive is kinda not slow
void part1(const Input &input) noexcept {
  int32_t count = 0;
  int32_t valid = 0;

  for (const auto &lock : input.locks) {
    for (const auto &key : input.keys) {
      valid = 1;
      for (int32_t i = 0; i < lock.size(); ++i) {
        if (lock[i] + key[i] > 5) {
          valid = 0;
          break;
        }
      }
      count += valid;
    }
  }

  printf("Part 1: %d\n", count);
}

int32_t main() noexcept {
  auto input = get_input();
  // input.print();
  part1(input);

  return 0;
}
