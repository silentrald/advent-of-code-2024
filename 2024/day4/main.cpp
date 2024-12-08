
#include <cstring>
#include <fstream>

#if 0
const char *const FILENAME = "sample.txt";
const int SIZE = 10;
#else
const char *const FILENAME = "input.txt";
const int SIZE = 140;
#endif

const char *SEARCH = "XMAS";
const char *SEARCH_REVERSE = "SAMX";
const int SEARCH_LENGTH = std::strlen(SEARCH);

bool check_horizontal(char grid[SIZE][SIZE], int row, int col,
                      const char *word) {
  if (col + SEARCH_LENGTH > SIZE) {
    return false;
  }

  bool valid = std::strncmp(grid[row] + col, word, SEARCH_LENGTH) == 0;
  // if (valid) {
  //   printf("H: (%d,%d) [%4.4s]\n", row, col, grid[row] + col);
  // }
  return valid;
}

bool check_vertical(char grid[SIZE][SIZE], int row, int col, const char *word) {
  if (row + SEARCH_LENGTH > SIZE) {
    return false;
  }

  for (int i = 1; i < SEARCH_LENGTH; ++i) {
    if (grid[row + i][col] != word[i]) {
      return false;
    }
  }
  return true;
}

bool check_forward_diagonal(char grid[SIZE][SIZE], int row, int col,
                            const char *word) {
  if (row + SEARCH_LENGTH > SIZE || col + SEARCH_LENGTH > SIZE) {
    return false;
  }

  for (int i = 1; i < SEARCH_LENGTH; ++i) {
    if (grid[row + i][col + i] != word[i]) {
      return false;
    }
  }

  return true;
}

bool check_backward_diagonal(char grid[SIZE][SIZE], int row, int col,
                             const char *word) {
  if (col < SEARCH_LENGTH - 1 || row + SEARCH_LENGTH > SIZE) {
    return false;
  }

  for (int i = 1; i < SEARCH_LENGTH; ++i) {
    if (grid[row + i][col - i] != word[i]) {
      return false;
    }
  }

  return true;
}

int part1_check(char grid[SIZE][SIZE], int row, int col) {
  char c = grid[row][col];
  if (c != 'X' && c != 'S') {
    return 0;
  }

  int count = 0;
  const char *word = c == 'X' ? SEARCH : SEARCH_REVERSE;
  count += check_horizontal(grid, row, col, word);
  count += check_vertical(grid, row, col, word);
  count += check_forward_diagonal(grid, row, col, word);
  count += check_backward_diagonal(grid, row, col, word);

  return count;
}

int part1_count(char grid[SIZE][SIZE]) {
  int count = 0;
  for (int r = 0; r < SIZE; ++r) {
    for (int c = 0; c < SIZE; ++c) {
      count += part1_check(grid, r, c);
    }
  }
  return count;
}

bool part2_check(char grid[SIZE][SIZE], int row, int col) {
  return grid[row + 1][col + 1] == 'A' &&
         ((grid[row][col] == 'M' && grid[row + 2][col + 2] == 'S') ||
          (grid[row][col] == 'S' && grid[row + 2][col + 2] == 'M')) &&
         ((grid[row][col + 2] == 'M' && grid[row + 2][col] == 'S') ||
          (grid[row][col + 2] == 'S' && grid[row + 2][col] == 'M'));
}

int part2_count(char grid[SIZE][SIZE]) {
  int count = 0;
  for (int r = 0; r < SIZE - 2; ++r) {
    for (int c = 0; c < SIZE - 2; ++c) {
      count += part2_check(grid, r, c);
    }
  }
  return count;
}

int main() {
  char grid[SIZE][SIZE];

  {
    std::ifstream file{FILENAME};
    std::string line{};

    for (int i = 0; std::getline(file, line); ++i) {
      std::strncpy(grid[i], line.c_str(), SIZE);
    }
  }

  printf("Part 1: %d\n", part1_count(grid));
  printf("Part 2: %d\n", part2_count(grid));

  return 0;
}
