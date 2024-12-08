#include <cstdlib>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>

const char *const FILENAME = "input.txt";

void part1() noexcept {
  std::ifstream file(FILENAME);
  int32_t i1, i2;
  std::priority_queue<int32_t> list1{};
  std::priority_queue<int32_t> list2{};
  int32_t distance = 0;

  while (file >> i1 >> i2) {
    list1.push(i1);
    list2.push(i2);
  }
  file.close();

  while (!list1.empty()) {
    distance += std::abs(list1.top() - list2.top());
    list1.pop();
    list2.pop();
  }

  printf("Part 1: %d\n", distance); // 2430334
}

void part2() noexcept {
  std::ifstream file(FILENAME);
  int32_t i1, i2;
  std::vector<int32_t> left_list{};
  std::unordered_map<int32_t, int32_t> right_memo{};

  while (file >> i1 >> i2) {
    left_list.push_back(i1);

    auto it = right_memo.find(i2);
    if (it == right_memo.end()) {
      right_memo.insert({i2, 1});
    } else {
      ++it->second;
    }
  }
  file.close();

  int32_t accumulator = 0;
  for (auto i : left_list) {
    auto it = right_memo.find(i);
    if (it != right_memo.end()) {
      accumulator += i * it->second;
    }
  }

  printf("Part 2: %d\n", accumulator); // 28786472
}

int32_t main() noexcept {
  part1();
  part2();
  return 0;
}
