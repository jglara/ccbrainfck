#include "ccbf.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>

namespace rng = std::ranges;

int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::string program;
    do {
      program.clear();
      BFMachine bf{std::cin, std::cout};    
      std::cout << "\nCCBF> ";
      std::cin >> program;
      bf.run(program);

    } while (!program.empty());
  } else {
    std::ifstream ifs{argv[1], std::ios::in};
    BFMachine bf{std::cin, std::cout};
    if (!ifs.is_open()) {
      std::cerr << "Failed to open file: " << argv[1] << '\n';
      return 1;
    }

    auto input = rng::subrange(std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{});
    std::string program{input.begin(), input.end()};
    bf.run(program);
  }

  return 0;
}
