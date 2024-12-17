

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#elif 0
const char *const FILENAME = "sample2.txt";
#else
const char *const FILENAME = "input.txt";
#endif

enum Opcode : int64_t {
  adv = 0,
  bxl = 1,
  bst = 2,
  jnz = 3,
  bxc = 4,
  out = 5,
  bdv = 6,
  cdv = 7,
};

struct Registers {
  int64_t a;
  int64_t b;
  int64_t c;

  void print() const noexcept {
    printf("Registers => A: %ld ; B: %ld ; C: %ld\n", this->a, this->b,
           this->c);
  }
};

struct Input {
  Registers registers{};
  std::vector<int32_t> instructions{};
};

Input get_input() noexcept {
  Input input{};
  std::ifstream file{FILENAME};
  std::string line{};
  int32_t i;
  char g;

  // Registers
  std::getline(file, line);
  sscanf(line.c_str(), "Register A: %ld", &input.registers.a);
  std::getline(file, line);
  sscanf(line.c_str(), "Register B: %ld", &input.registers.b);
  std::getline(file, line);
  sscanf(line.c_str(), "Register C: %ld", &input.registers.c);

  // \n
  std::getline(file, line);

  // Instructions
  std::getline(file, line);

  std::stringstream ss{line.substr(std::strlen("Program: "))};
  while (ss >> i) {
    input.instructions.push_back(i);
    // , (comma)
    ss >> g;
  }

  return std::move(input);
}

[[nodiscard]] int64_t combo(const Registers &registers,
                            int64_t operand) noexcept {
  switch (operand) {
  case 0:
  case 1:
  case 2:
  case 3:
    return operand;

  case 4:
    return registers.a;

  case 5:
    return registers.b;

  case 6:
    return registers.c;

  case 7:
    // Reserved
    break;
  }

  std::abort();
}

[[nodiscard]] int64_t perform(int32_t instruction_pointer, int32_t opcode,
                              int64_t operand, Registers &registers,
                              int64_t &output) noexcept {
  switch (opcode) {
  case adv:
    registers.a /= 1 << combo(registers, operand);
    break;

  case bxl:
    registers.b ^= operand;
    break;

  case bst:
    // mod 8
    registers.b = combo(registers, operand) & 0b111;
    break;

  case jnz:
    if (registers.a) {
      // Jump to the literal operand
      return operand;
    }
    break;

  case bxc:
    registers.b ^= registers.c;
    break;

  case out:
    output = combo(registers, operand) & 0b111;
    break;

  case bdv:
    registers.b = registers.a / (1 << combo(registers, operand));
    break;

  case cdv:
    registers.c = registers.a / (1 << combo(registers, operand));
    break;

  default:
    std::abort();
  }

  return instruction_pointer + 2;
}

const char *get_opcode_name(int64_t opcode) noexcept {
  switch (opcode) {
  case adv:
    return "adv";

  case bxl:
    return "bxl";

  case bst:
    return "bst";

  case jnz:
    return "jnz";

  case bxc:
    return "bxc";

  case out:
    return "out";

  case bdv:
    return "bdv";

  case cdv:
    return "cdv";
  }
  std::abort();
}

void step_info(int64_t ip, const Registers &registers,
               const std::vector<int32_t> &instructions) noexcept {
  registers.print();
  printf("IP: %ld ; Opcode: %s (%d) ; Operand: %d\n\n", ip,
         get_opcode_name(instructions[ip]), instructions[ip],
         instructions[ip + 1]);
}

[[nodiscard]] std::vector<int64_t>
run_instructions(Registers registers,
                 const std::vector<int32_t> &instructions) noexcept {
  std::vector<int64_t> outputs{};

  int64_t output = -1;
  for (int64_t ip = 0; ip < instructions.size();) {
    // step_info(ip, registers, instructions);
    ip = perform(ip, instructions[ip], instructions[ip + 1], registers, output);
    if (output > -1) {
      outputs.push_back(output);
      output = -1;
    }
  }

  // registers.print();
  // printf("\n");

  return std::move(outputs);
}

void print_outputs(const std::vector<int64_t> &outputs) noexcept {
  if (outputs.empty()) {
    printf("empty\n");
    return;
  }

  printf("%ld", outputs[0]);
  for (int64_t i = 1; i < outputs.size(); ++i) {
    printf(",%ld", outputs[i]);
  }
}

// Specific for my instructions
// Do a backward trial and error approach
void find_register_a(const std::vector<int32_t> &instructions) noexcept {
  int64_t a = 0;
  int64_t a3 = 0;
  int32_t exp_ins = 0;
  bool found = true;

  for (int32_t i = instructions.size() - 1;
       i >= 0 && i < instructions.size();) {

    if (found) {
      found = false;
      a3 = a ? 0L : 1L;
      a <<= 3L;
      // printf("Current %lb ; %d\n", a, instructions[i]);
    } else {
      a >>= 3L;
      a3 = (a & 0b111L) + 1L;
      a &= ~0b111L;
      ++i;
      // printf("Back %lb ; %d -> resume %03lb\n", a, instructions[i], a3);
    }

    exp_ins = instructions[i];

    for (; a3 <= 0b111L; ++a3) {
      // My instructions simplifies to
      // [(A >> (A ^ 3)) ^ A] & 0b111
      //   `& 0b111` is the same as mod 8
      //   A always shifts by 3 bits each loop
      if ((a3 ^ ((a | a3) >> (a3 ^ 0b011L)) & 0b111L) == exp_ins) {
        a |= a3;
        --i;
        found = true;
        break;
      }
    }
  }

  // For verification

  printf("Part 2: %ld\n", a);
  printf("Verification: bits %lb\n", a);

  printf("[%d", instructions[0]);
  for (int32_t i = 1; i < instructions.size(); ++i) {
    printf(",%d", instructions[i]);
  }
  printf("] == [");
  print_outputs(run_instructions({a, 0, 0}, instructions));
  printf("]\n");
}

// Do a backward trial and error approach
void find_register_a_generalized(
    const std::vector<int32_t> &instructions) noexcept {
  int64_t a = 0;
  int64_t a3 = 0;
  int32_t exp_ins = 0;
  bool found = true;

  // Assumes that the instructions ends with jnz instruction
  //   and only contains one out instruction
  std::vector<int32_t> partial_instructions = instructions;

  // Remove the jnz instruction at the end
  partial_instructions.pop_back();
  partial_instructions.pop_back();

  for (int32_t i = instructions.size() - 1;
       i >= 0 && i < instructions.size();) {

    if (found) {
      found = false;
      a3 = a ? 0L : 1L;
      a <<= 3L;
      // printf("Current %lb ; %d\n", a, instructions[i]);
    } else {
      a >>= 3L;
      a3 = (a & 0b111L) + 1L;
      a &= ~0b111L;
      ++i;
      // printf("Back %lb ; %d -> resume %03lb\n", a, instructions[i], a3);
    }

    exp_ins = instructions[i];

    for (; a3 <= 0b111L; ++a3) {
      if (run_instructions({a | a3, 0, 0}, partial_instructions)[0] ==
          exp_ins) {
        a |= a3;
        --i;
        found = true;
        break;
      }
    }
  }

  // For verification

  printf("Part 2: %ld\n", a);
  printf("Verification: bits %lb\n", a);

  printf("[%d", instructions[0]);
  for (int32_t i = 1; i < instructions.size(); ++i) {
    printf(",%d", instructions[i]);
  }
  printf("] == [");
  print_outputs(run_instructions({a, 0, 0}, instructions));
  printf("]\n");
}

int32_t main() {
  auto input = get_input();

  printf("Part 1: ");
  print_outputs(run_instructions(input.registers, input.instructions));
  printf("\n");

  // find_register_a(input.instructions);
  find_register_a_generalized(input.instructions);

  return 0;
}
