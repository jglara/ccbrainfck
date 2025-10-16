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
std::vector<inst_t> optimize_bytecodes(std::vector<inst_t> const& bytecodes);

} // namespace bfcompiler_internal

std::vector<inst_t> compile(rng::input_range auto const& program) {

  auto compile_program = bfcompiler_internal::make_compile_program_view(program);

  std::vector<inst_t> bytecodes;
  rng::copy(compile_program, std::back_inserter(bytecodes)); // for lack of rng::to()

  auto bytecodes_opt = bfcompiler_internal::optimize_bytecodes(bytecodes);    
  bfcompiler_internal::resolve_jumps(bytecodes_opt);


  return bytecodes_opt;
}
