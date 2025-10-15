#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <iterator>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace rng = std::ranges;

static std::vector<std::size_t> build_jump_table(rng::input_range auto const& program) {
  auto const program_size = rng::size(program);
  std::vector<std::size_t> jumps(program_size, program_size);
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
      jumps[match] = i;
      jumps[i] = match;
    }
  }
  
  if (!loop_stack.empty()) {
    throw std::runtime_error("Unmatched opening bracket in Brainfuck program");
  }
  
  return jumps;
}


class BFMachine {
 public:
  static constexpr std::size_t memory_size = 30000;

  explicit BFMachine(std::istream& in, std::ostream& out)
      : memory_{}, is_(in), os_(out) {}

  void reset() {
    memory_.fill(0);
  }

  void run(rng::random_access_range auto const& program) {
    reset();

    auto const program_size = rng::size(program);
    auto const jumps = build_jump_table(program);

    std::size_t pc{0};
    std::size_t mp{0};

    while (pc < program_size) {
      auto const inst = program[pc];
      switch (inst) {
      case '>':
        mp = (mp + 1) % memory_size;
        break;
      case '<':
        mp = (mp == 0) ? memory_size - 1 : mp - 1;
        break;
      case '+':
        ++memory_[mp];
        break;
      case '-':
        --memory_[mp];
        break;
      case '.':
        os_.put(static_cast<char>(memory_[mp]));
        break;
      case ',': {
        auto const value = is_.get();
        if (value == std::istream::traits_type::eof()) {
          memory_[mp] = 0;
        } else {
          memory_[mp] = static_cast<std::uint8_t>(value);
        }
        break;
      }
      case '[':
        if (memory_[mp] == 0) {
          pc = jumps[pc];
        }
        break;
      case ']':
        if (memory_[mp] != 0) {
          pc = jumps[pc];
        }
        break;
      default:
        break;
      }
      ++pc;
    }
  }

 private:

  std::array<std::uint8_t, memory_size> memory_;
  std::istream& is_;
  std::ostream& os_;
};
