#include <cstdint>
#include <fstream>
#include <pthread.h>
#include <string>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#else
const char *const FILENAME = "input.txt";
#endif

struct Section {
  int32_t id = 0;
  int32_t offset = 0;
  int32_t length = 0;

  bool operator<(const Section &other) const noexcept {
    return this->offset < other.offset;
  }
};

[[nodiscard]] std::string get_disk_map() noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  std::getline(file, line);

  return std::move(line);
}

int64_t summation(int64_t from, int64_t to) noexcept {
  return ((to - 1) * to - (from - 1) * from) / 2;
}

inline int32_t find_last(const std::string &disk_map,
                         int32_t &end_cursor) noexcept {
  int64_t remaining;
  for (remaining = disk_map[end_cursor]; remaining == '0'; end_cursor -= 2) {
    remaining = disk_map[end_cursor];
  }
  return remaining - '0';
}

void part1(const std::string &disk_map) noexcept {
  int32_t start_cursor = 0;
  int32_t end_cursor = disk_map.size() - 1;
  int32_t offset = 0;
  int32_t remaining = find_last(disk_map, end_cursor);
  int64_t accumulator = 0;

  while (start_cursor < end_cursor) {
    if (!(start_cursor & 1)) {
      int64_t size = disk_map[start_cursor] - '0';
      accumulator += summation(offset, offset + size) * (start_cursor / 2);
      offset += size;
      ++start_cursor;
      continue;
    }

    // Consume end
    for (int64_t fill = disk_map[start_cursor] - '0'; fill > 0;) {
      if (fill < remaining) {
        remaining -= fill;
        accumulator += summation(offset, offset + fill) * (end_cursor / 2);
        offset += fill;
        break;
      }

      fill -= remaining;
      accumulator += summation(offset, offset + remaining) * (end_cursor / 2);
      offset += remaining;

      remaining = find_last(disk_map, end_cursor -= 2);
    }
    ++start_cursor;
  }

  if (start_cursor == end_cursor) {
    accumulator += summation(offset, offset + remaining) * (end_cursor / 2);
  }

  // 6262891638328
  printf("Part 1: %ld\n", accumulator);
}

void part2(const std::string &disk_map) noexcept {
  std::vector<Section> sections{};
  std::vector<Section> empty_sections{};
  std::vector<Section> final_sections{};
  Section section{};
  Section empty_section{};

  int32_t offset = 0;
  for (int32_t i = 0; i < disk_map.size(); ++i) {
    section.offset = offset;
    section.length = disk_map[i] - '0';
    if (i & 1) {
      section.id = -1;
      empty_sections.push_back(section);
    } else {
      section.id = i / 2;
      sections.push_back(section);
    }

    offset += disk_map[i] - '0';
  }

  for (int32_t i = sections.size(); i >= 0; --i) {
    section = sections[i];
    for (int32_t j = 0; j < empty_sections.size(); ++j) {
      empty_section = empty_sections[j];
      if (empty_section.offset >= section.offset) {
        break;
      }

      if (empty_section.length > section.length) {
        section.offset = empty_section.offset;

        empty_section.length -= section.length;
        empty_section.offset += section.length;
        empty_sections[j] = empty_section;
        break;
      } else if (empty_section.length == section.length) {
        section.offset = empty_section.offset;
        empty_sections.erase(empty_sections.begin() + j);
        break;
      }
    }

    final_sections.push_back(section);
  }

  sections.clear();
  empty_sections.clear();

  int64_t accumulator = 0;
  for (auto s : final_sections) {
    accumulator += s.id * summation(s.offset, s.offset + s.length);
  }

  // 6287317016845
  printf("Part 2: %ld\n", accumulator);
}

int32_t main() noexcept {
  auto disk_map = get_disk_map();

  part1(disk_map);
  part2(disk_map);

  return 0;
}
