#pragma once

#include <sys/types.h>
#include <array>
#include <istream>
#include <ranges>
#include <iostream>


namespace rng = std::ranges;

class BFMachine {
  static constexpr u_int16_t MEMORY_SIZE=30000;
  std::array<u_int8_t, MEMORY_SIZE> memory_{};
  std::istream& is_;
  std::ostream& os_;


 public:
  explicit BFMachine(std::istream& in, std::ostream &out) : is_(in), os_(out) {}
  void run(rng::random_access_range auto const& program) {
// Runs a program in the BFMachine

  size_t pc{0}; // program counter
  size_t mp{0}; // memory pointer

  while (pc < std::size(program)) {
    auto inst = program[pc];
    switch (inst) {
    case '>':
      mp = (mp + 1) % BFMachine::MEMORY_SIZE;
      break;
    case '<':
      mp = (mp==0)?BFMachine::MEMORY_SIZE-1:mp-1;
      break;
    case '+':
      ++memory_[mp];
      break;
    case '-':
      --memory_[mp];
      break;
    case '.':
      os_ << memory_[mp];
      break;
    case ',':
      is_ >> memory_[mp];
      break;
    case '[':
      if (memory_[mp] == 0) {
        size_t depth{0};        
        for (++pc;pc<std::size(program); ++pc) {
          if (program[pc] == ']') {
            if (depth == 0) {
              break;
            } else {
              --depth;
            }
          } else if (program[pc] == '[') {
            ++depth;
          }            
        }
      }
      break;
    case ']':
      if (memory_[mp] != 0) {
        size_t depth{0};
        for (--pc;pc<std::size(program); --pc) {
          if (program[pc] == '[') {
            if (depth == 0) {
              break;
            } else {
              --depth;
            }
          } else if (program[pc] == ']') {
            ++depth;
          }            
        }
      }      
      break;
    }
    ++pc;
  }

}    
      
  
};
