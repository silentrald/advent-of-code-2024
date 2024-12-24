#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if 0
const char *const FILENAME = "sample.txt";
#elif 0
const char *const FILENAME = "sample2.txt";
#else
const char *const FILENAME = "input.txt";
#endif

enum class Operator : int32_t { AND, XOR, OR };

struct Wire {
  char name[4] = "";

  Wire() noexcept = default;

  void copy(const std::string &name) noexcept {
    std::strncpy(this->name, name.c_str(), 4);
    this->name[3] = '\0';
  }

  [[nodiscard]] bool operator==(const Wire &rhs) const noexcept {
    return std::strncmp(this->name, rhs.name, 4) == 0;
  }

  [[nodiscard]] bool operator==(const std::string &rhs) const noexcept {
    return std::strncmp(this->name, rhs.c_str(), 4) == 0;
  }

  [[nodiscard]] bool operator==(const char *rhs) const noexcept {
    return std::strncmp(this->name, rhs, 4) == 0;
  }
};

struct Gate {
  Wire in1{};
  Wire in2{};
  Operator op{};

  Gate() noexcept = default;

  [[nodiscard]] const char *get_operator_name() const noexcept {
    switch (this->op) {
    case Operator::AND:
      return "AND";

    case Operator::XOR:
      return "XOR";

    case Operator::OR:
      return "OR";

    default:
      std::abort();
    }
  }

  [[nodiscard]] bool evaluate(bool v1, bool v2) noexcept {
    switch (this->op) {
    case Operator::AND:
      return v1 & v2;

    case Operator::XOR:
      return v1 ^ v2;

    case Operator::OR:
      return v1 | v2;

    default:
      std::abort();
    }
  }
};

struct Input {
  std::unordered_map<std::string, bool> initial_value{};
  std::unordered_set<std::string> wires{};
  std::unordered_map<std::string, Gate> gates{};

  void print() const noexcept {
    for (const auto &w : this->initial_value) {
      printf("%s: %d\n", w.first.c_str(), w.second);
    }
    printf("\n");
    for (const auto &p : this->gates) {
      printf("%s %s %s -> %s\n", p.second.in1.name,
             p.second.get_operator_name(), p.second.in2.name, p.first.c_str());
    }
  }
};

[[nodiscard]] Input get_input() noexcept {
  std::ifstream file{FILENAME};
  std::string line{};

  Input input{};
  Gate gate{};
  Wire op{};
  Wire out{};

  while (true) {
    std::getline(file, line);
    if (line == "")
      break;
    input.initial_value[line.substr(0, 3)] = line[5] == '1';
  }

  while (std::getline(file, line)) {
    sscanf(line.c_str(), "%s %s %s -> %s", gate.in1.name, op.name,
           gate.in2.name, out.name);

    if (op == "AND") {
      gate.op = Operator::AND;
    } else if (op == "XOR") {
      gate.op = Operator::XOR;
    } else {
      gate.op = Operator::OR;
    }

    input.gates[out.name] = gate;
    input.wires.insert(out.name);
    input.wires.insert(gate.in1.name);
    input.wires.insert(gate.in2.name);
  }

  return std::move(input);
}

void part1(std::unordered_map<std::string, bool> values,
           const std::unordered_set<std::string> &wires,
           const std::unordered_map<std::string, Gate> &gates) noexcept {
  std::vector<std::string> stack{};

  for (const auto &w : wires) {
    if (values.find(w) != values.end()) {
      continue;
    }

    stack.push_back(w);
    while (!stack.empty()) {
      auto out = stack.back();
      // printf("Checking %s\n", out.c_str());

      auto gate = gates.at(out);
      auto v1 = values.find(gate.in1.name);
      auto v2 = values.find(gate.in2.name);
      if (v1 != values.end() && v2 != values.end()) {
        // printf("Writing %s\n", out.c_str());
        values[out] = gate.evaluate(v1->second, v2->second);
        stack.pop_back();
        continue;
      }

      if (v1 == values.end()) {
        stack.push_back(gate.in1.name);
      }

      if (v2 == values.end()) {
        stack.push_back(gate.in2.name);
      }
    }
  }

  uint64_t num = 0;
  std::string wire{};
  for (uint64_t i = 0;; ++i) {
    if (i < 10) {
      wire = "z0" + std::to_string(i);
    } else {
      wire = "z" + std::to_string(i);
    }

    auto it = values.find(wire);
    if (it == values.end()) {
      break;
    }

    num |= (uint64_t)it->second << i;
  }

  printf("Part 1: %ld\n", num);
}

int32_t main() noexcept {
  auto input = get_input();
  // input.print();

  part1(input.initial_value, input.wires, input.gates);

  return 0;
}
