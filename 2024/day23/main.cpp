#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
#endif

template <typename T> struct vec {
  T x;
  T y;
};

[[nodiscard]] std::vector<vec<std::string>> get_edges() noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  std::vector<vec<std::string>> edges{};
  vec<std::string> edge{};

  while (std::getline(file, line)) {
    edge.x = line.substr(0, 2);
    edge.y = line.substr(3, 2);
    edges.push_back(std::move(edge));
  }

  return std::move(edges);
}

[[nodiscard]] std::vector<std::string>
get_ids(const std::vector<vec<std::string>> &edges) noexcept {
  std::vector<std::string> ids{};

  for (const auto &e : edges) {
    auto it = std::find(ids.begin(), ids.end(), e.x);
    if (it == ids.end()) {
      ids.push_back(e.x);
    }

    it = std::find(ids.begin(), ids.end(), e.y);
    if (it == ids.end()) {
      ids.push_back(e.y);
    }
  }

  return std::move(ids);
}

template <typename T>
[[nodiscard]] inline int32_t find_value(const std::vector<T> &v,
                                        const T &value) noexcept {
  for (int32_t i = 0; i < v.size(); ++i) {
    if (v[i] == value) {
      return i;
    }
  }
  return -1;
}

[[nodiscard]] std::vector<bool>
convert_matrix(const std::vector<vec<std::string>> &edges,
               const std::vector<std::string> &ids) noexcept {
  std::vector<bool> matrix{};
  matrix.resize(ids.size() * ids.size(), false);

  for (int32_t i = 0; i < ids.size(); ++i) {
    matrix[i + i * ids.size()] = true;
  }

  vec<int32_t> edge{};
  for (const auto &e : edges) {
    edge.x = find_value(ids, e.x);
    edge.y = find_value(ids, e.y);

    // Do a bidirectional representation of the matrix, used in part2
    matrix[edge.x + edge.y * ids.size()] = true;
    matrix[edge.y + edge.x * ids.size()] = true;
  }

  return std::move(matrix);
}

[[nodiscard]] inline bool set_contains(const std::unordered_set<int32_t> &set,
                                       int32_t value) noexcept {
  return set.find(value) != set.end();
}

void part1(const std::vector<bool> &matrix,
           const std::vector<std::string> &ids) noexcept {
  std::unordered_set<int32_t> is_t{};
  for (int32_t i = 0; i < ids.size(); ++i) {
    if (ids[i][0] == 't') {
      is_t.insert(i);
    }
  }

  int32_t count = 0;
  bool contain = false;
  for (int32_t y = 0; y < ids.size(); ++y) {
    for (int32_t x = y + 1; x < ids.size(); ++x) {
      if (!matrix[y * ids.size() + x]) {
        continue;
      }

      contain = set_contains(is_t, y) || set_contains(is_t, x);
      for (int32_t x2 = x + 1; x2 < ids.size(); ++x2) {
        if ((contain || set_contains(is_t, x2)) &&
            matrix[y * ids.size() + x2] && matrix[x * ids.size() + x2]) {
          ++count;
        }
      }
    }
  }

  printf("Part 1: %d\n", count);
}

// Slow but got the answer anyway
void part2(const std::vector<bool> &matrix,
           const std::vector<std::string> &ids) noexcept {
  std::vector<int32_t> stack{};
  std::vector<int32_t> longest{};

  // NOTE: subsets of >= 3 can be memoized to not be recomputed
  //  Maybe bottom up approach should be better for memoization
  for (int32_t y = 1; y < ids.size(); ++y) {
    stack.push_back(y);
    stack.push_back(y);

    while (!stack.empty()) {
      int32_t x = stack.back() + 1;
      stack.pop_back();

      for (; x < ids.size(); ++x) {
        int32_t tmp = x * ids.size();

        bool valid = true;
        for (const auto &s : stack) {
          if (!matrix[tmp + s]) {
            valid = false;
            break;
          }
        }

        if (valid) {
          stack.push_back(x);
          if (stack.size() > longest.size()) {
            longest = stack;
          }
        }
      }
    }
  }

  // Convert the ids to string
  std::set<std::string> selected{};
  for (const auto &i : longest) {
    selected.insert(ids[i]);
  }

  // Join the string
  std::string out = *selected.begin();
  for (auto it = ++selected.begin(); it != selected.end(); ++it) {
    out.push_back(',');
    out.append(*it);
  }

  printf("Part 2: %s\n", out.c_str());
}

int32_t main() noexcept {
  auto edges = get_edges();
  auto ids = get_ids(edges);
  auto matrix = convert_matrix(edges, ids);

  part1(matrix, ids);
  part2(matrix, ids);

  return 0;
}
