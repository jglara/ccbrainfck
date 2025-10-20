#pragma once
#include "bytecode.hpp"
#include "ccbf.hpp"
#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>
#include <iostream>
#include <string_view>
namespace rng = std::ranges;
namespace vws = std::ranges::views;


namespace bfcompiler_internal {

  // Translate raw Brainfuck characters into a bytecode view.
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

// Populate jump targets by pairing brackets.
void resolve_jumps(std::vector<inst_t>& bytecodes);

// Collapse runs of pointer/memory arithmetic into single instructions.
std::vector<inst_t> optimize_bytecodes_opt1(std::vector<inst_t> const& bytecodes);

// Replace canonical zeroing loops like [-] with set instructions.
std::vector<inst_t> optimize_bytecodes_opt2(std::vector<inst_t> const& bytecodes);


// Dump bytecode instructions with indentation reflecting loop nesting.
inline void print_bytecodes(std::vector<inst_t> const& bytecodes, std::ostream& os = std::cout) {
  auto opcode_name = [](inst_t::op_code_t opcode) -> std::string_view {
    switch (opcode) {
      case inst_t::op_code_t::nop:
        return "nop";
      case inst_t::op_code_t::mpadd:
        return "mpadd";
      case inst_t::op_code_t::add:
        return "add";
      case inst_t::op_code_t::jmpz:
        return "jmpz";
      case inst_t::op_code_t::jmpnz:
        return "jmpnz";
      case inst_t::op_code_t::in:
        return "in";
      case inst_t::op_code_t::out:
        return "out";
      case inst_t::op_code_t::set:
        return "set";
    }
    return "unknown";
  };

  int indent = 0;
  for (std::size_t idx = 0; idx < bytecodes.size(); ++idx) {
    auto const& inst = bytecodes[idx];

    if (inst.opcode == inst_t::op_code_t::jmpnz) {
      indent = std::max(indent - 1, 0);
    }

    for (int i = 0; i < indent; ++i) {
      os << "  ";
    }

    os << '[' << idx << "] " << opcode_name(inst.opcode) << ' ' << inst.operand << '\n';

    if (inst.opcode == inst_t::op_code_t::jmpz) {
      ++indent;
    }
  }
}

} // namespace bfcompiler_internal

// Compile a Brainfuck program into optimized bytecode.
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
  //bfcompiler_internal::print_bytecodes(bytecodes);
  bfcompiler_internal::resolve_jumps(bytecodes);


  return bytecodes;
}
