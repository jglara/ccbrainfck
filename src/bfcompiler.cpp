#include "bfcompiler.hpp"

#include <stdexcept>
#include <vector>

namespace bfcompiler_internal {

void resolve_jumps(std::vector<inst_t>& bytecodes) {
  auto const program_size = bytecodes.size();
  std::vector<std::size_t> loop_stack;

  for (std::size_t i = 0; i < program_size; ++i) {
    auto const inst = bytecodes[i];
    if (inst.opcode == inst_t::op_code_t::jmpz) {
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
}

void optimize_bytecodes(std::vector<inst_t>& /*bytecodes*/) {
  // Placeholder for future bytecode optimizations.
}

} // namespace bfcompiler_internal
