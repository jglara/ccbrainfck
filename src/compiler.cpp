#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include "bytecode.hpp"
#include "bfcompiler.hpp"
#include "bfvm.hpp"

namespace rng = std::ranges;

int main(int argc, char* argv[]) {

  if (argc != 2) {
    std::cout << "Usage " << argv[0] << "<file>\n";
  } else {
    std::ifstream ifs{argv[1], std::ios::in};
    if (!ifs.is_open()) {
      std::cerr << "Failed to open file: " << argv[1] << '\n';
      return 1;
    }

    auto const input =
        rng::subrange(std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{});

    auto bytecodes = compile(input);
        
    BrainFckVM vm{std::cin, std::cout};
    vm.run(bytecodes);
    
  }
}
