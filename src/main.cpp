#include "ccbf.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>

namespace rng = std::ranges;

int main(int argc, char* argv[]) {
  BFMachine machine{std::cin, std::cout};

  if (argc == 1) {
    std::string program;
    while (true) {
      std::cout << "\nCCBF> ";
      if (!std::getline(std::cin, program)) {
        break;
      }
      if (program.empty()) {
        break;
      }
      machine.run(program);
    }
  } else {
    std::ifstream ifs{argv[1], std::ios::in};
    if (!ifs.is_open()) {
      std::cerr << "Failed to open file: " << argv[1] << '\n';
      return 1;
    }

    auto const input =
        rng::subrange(std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{});
    std::string program{input.begin(), input.end()};
    machine.run(program);
  }

  return 0;
}
