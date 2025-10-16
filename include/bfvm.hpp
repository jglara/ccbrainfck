#pragma once
#include "bytecode.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <limits>
#include <ostream>
#include <ranges>

namespace rng = std::ranges;

class BrainFckVM {
 public:

  explicit BrainFckVM(std::istream& in, std::ostream& out)
    : memory_{}, pc_{0}, mp_{0}, is_(in), os_(out) {}

  void reset() {
    memory_.fill(0);
    pc_ = 0;
    mp_ = 0;
  }

  void run(rng::random_access_range auto program) {
    reset();
    auto const program_size = rng::size(program);
    while (pc_ < program_size) {
      inst_t const inst = program[pc_];
      switch (inst.opcode) {
        case inst_t::op_code_t::mpadd:
          mp_ = wrap_pointer(mp_, inst.operand);
          break;
        case inst_t::op_code_t::add:
          memory_[mp_] += static_cast<std::uint8_t>(inst.operand);
          break;
        case inst_t::op_code_t::jmpz:
          if (memory_[mp_] == 0) {
            pc_ = static_cast<std::size_t>(inst.operand);
          }
          break;
        case inst_t::op_code_t::jmpnz:
          if (memory_[mp_] != 0) {
            pc_ = static_cast<std::size_t>(inst.operand);
          }
          break;
        case inst_t::op_code_t::out:
          os_.put(static_cast<char>(memory_[mp_]));
          break;          
        case inst_t::op_code_t::in:
          auto const value = is_.get();
          if (value == std::istream::traits_type::eof()) {
            memory_[mp_] = 0;
          } else {
            memory_[mp_] = static_cast<std::uint8_t>(value);
          }
          break;
      }
      ++pc_;

    }      

  }

 private:
  static constexpr std::size_t memory_size = 30000;
  std::array<std::uint8_t, memory_size> memory_{};
  std::size_t pc_{0}; // program counter
  std::size_t mp_{0}; // memory pointer

  static std::size_t wrap_pointer(std::size_t current, std::int32_t delta) {
    static_assert(memory_size <= static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()),
                  "Tape size exceeds ptrdiff_t range");
    auto const size = static_cast<std::ptrdiff_t>(memory_size);
    auto next = static_cast<std::ptrdiff_t>(current) + static_cast<std::ptrdiff_t>(delta);
    next %= size;
    if (next < 0) {
      next += size;
    }
    return static_cast<std::size_t>(next);
  }
  
  std::istream& is_;
  std::ostream& os_;

};
