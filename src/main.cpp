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
    std::string input;
    do {
      input.clear();
      std::cout << "\nCCBF> ";
      std::cin >> input;
      std::cout << input;
    } while (!input.empty());
  } else {
    std::ifstream ifs{argv[1], std::ios::in};
    if (!ifs.is_open()) {
      std::cerr << "Failed to open file: " << argv[1] << '\n';
      return 1;
    }

    auto range = rng::subrange(std::istreambuf_iterator<char>{ifs},
                               std::istreambuf_iterator<char>{});
    rng::copy(range, std::ostream_iterator<char>{std::cout});
  }

  return 0;
}
