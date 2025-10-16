#pragma once
#include "bytecode.hpp"
#include "ccbf.hpp"
#include <algorithm>
#include <iterator>
#include <ranges>
#include <string_view>
#include <vector>
namespace rng = std::ranges;
namespace vws = std::ranges::views;


void compile(rng::random_access_range auto const& program, std::output_iterator<inst_t> auto compiled) {

  auto compile_program = vws::zip_transform(
                             [](const auto& pc, const auto& input) {
                               switch (input) {
                                 case '>':
                                   return inst_t{inst_t::op_code_t::mpadd, 1};
                                 case '<':
                                   return inst_t{inst_t::op_code_t::mpadd, -1};

                                 case '+':
                                   return inst_t{inst_t::op_code_t::add, 1};

                                 case '-':
                                   return inst_t{inst_t::op_code_t::add, -1};

                                 case '.':
                                   return inst_t{inst_t::op_code_t::out, 0};

                                 case ',':
                                   return inst_t{inst_t::op_code_t::in, 0};

                                 case '[':
                                   return inst_t{inst_t::op_code_t::jmpz, 0};

                                 case ']':
                                   return inst_t{inst_t::op_code_t::jmpnz, 0};
                               }
                               return inst_t{inst_t::op_code_t::nop, 0};
                             },
                             vws::iota(0), program) |
                         vws::filter([](auto const& i) { return i.opcode != inst_t::op_code_t::nop; });

  std::vector<inst_t> bytecodes;
  rng::copy(compile_program, std::back_inserter(bytecodes)); // for lack of rng::to()  

  // resolve jumps
  auto const program_size = bytecodes.size(); 
  std::vector<std::size_t> loop_stack;
  
  for (std::size_t i = 0; i < program_size; ++i) {
    auto const inst = bytecodes[i];
    if (inst.opcode == inst_t::op_code_t::jmpz ) {
      loop_stack.push_back(i);
    } else if (inst.opcode == inst_t::op_code_t::jmpnz) {
      if (loop_stack.empty()) {
        throw std::runtime_error("Unmatched closing bracket in Brainfuck program");
      }
      auto const match = loop_stack.back();
      loop_stack.pop_back();
      bytecodes[match].operand = i;
      bytecodes[i].operand = match;
    }
  }
  
  if (!loop_stack.empty()) {
    throw std::runtime_error("Unmatched opening bracket in Brainfuck program");
  }

  rng::copy(bytecodes, compiled);
}

