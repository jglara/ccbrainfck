#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace rng = std::ranges;

class BFMachine {
public:  
  static constexpr std::size_t memory_size = 30000;

  std::array<std::uint8_t, memory_size> memory_{};
  std::istream& is_;
  std::ostream& os_;

  explicit BFMachine(std::istream& in, std::ostream& out) : is_(in), os_(out) {}
};


void run(BFMachine bfm, rng::random_access_range auto const& program) {
    auto const program_size = rng::size(program);
    std::vector<std::size_t> jump(program_size, program_size);

    std::vector<std::size_t> loop_stack;
    loop_stack.reserve(program_size);

    for (std::size_t i = 0; i < program_size; ++i) {
      auto const inst = program[i];
      if (inst == '[') {
        loop_stack.push_back(i);
      } else if (inst == ']') {
        if (loop_stack.empty()) {
          throw std::runtime_error("Unmatched closing bracket in Brainfuck program");
        }
        auto const match = loop_stack.back();
        loop_stack.pop_back();
        jump[match] = i;
        jump[i] = match;
      }
    }

    if (!loop_stack.empty()) {
      throw std::runtime_error("Unmatched opening bracket in Brainfuck program");
    }

    std::size_t pc{0};
    std::size_t mp{0};

    while (pc < program_size) {
      auto const inst = program[pc];
      switch (inst) {
      case '>':
        mp = (mp + 1) % bfm.memory_size;
        break;
      case '<':
        mp = (mp == 0) ? bfm.memory_size - 1 : mp - 1;
        break;
      case '+':
        ++bfm.memory_[mp];
        break;
      case '-':
        --bfm.memory_[mp];
        break;
      case '.':
        bfm.os_.put(static_cast<char>(bfm.memory_[mp]));
        break;
      case ',': {
        auto const value = bfm.is_.get();
        if (value == std::istream::traits_type::eof()) {
          bfm.memory_[mp] = 0;
        } else {
          bfm.memory_[mp] = static_cast<std::uint8_t>(value);
        }
        break;
      }
      case '[':
        if (bfm.memory_[mp] == 0) {
          pc = jump[pc];
        }
        break;
      case ']':
        if (bfm.memory_[mp] != 0) {
          pc = jump[pc];
        }
        break;
      default:
        break;
      }
      ++pc;
    }
}
