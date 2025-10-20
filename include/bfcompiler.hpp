#pragma once
#include "bytecode.hpp"
#include "ccbf.hpp"
#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>
#include <iostream>
namespace rng = std::ranges;
namespace vws = std::ranges::views;


namespace bfcompiler_internal {

  auto make_compile_program_view(rng::input_range auto const& program) {
  return vws::zip_transform(
             [](auto const&, auto input) {
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
             vws::iota(std::size_t{0}), program) |
         vws::filter([](auto const& i) { return i.opcode != inst_t::op_code_t::nop; });
}

void resolve_jumps(std::vector<inst_t>& bytecodes);
std::vector<inst_t> optimize_bytecodes_opt1(std::vector<inst_t> const& bytecodes);
std::vector<inst_t> optimize_bytecodes_opt2(std::vector<inst_t> const& bytecodes);  

} // namespace bfcompiler_internal

std::vector<inst_t> compile(rng::input_range auto const& program, size_t optims=2) {

  auto compile_program = bfcompiler_internal::make_compile_program_view(program);

  std::vector<inst_t> bytecodes;
  rng::copy(compile_program, std::back_inserter(bytecodes)); // for lack of rng::to()
  std::cout << "Compiled program: " << rng::size(bytecodes) << " op codes\n";

  if (optims>0) {
    bytecodes = bfcompiler_internal::optimize_bytecodes_opt1(bytecodes);
    std::cout << "Optimization 1: " << rng::size(bytecodes) << " op codes\n";
  }
  if (optims>1) {
    bytecodes = bfcompiler_internal::optimize_bytecodes_opt2(bytecodes);
    std::cout << "Optimization 2: " << rng::size(bytecodes) << " op codes\n";
  }
    
  bfcompiler_internal::resolve_jumps(bytecodes);


  return bytecodes;
}
