#include "bfcompiler.hpp"
#include <ranges>

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

std::vector<inst_t> optimize_bytecodes(std::vector<inst_t> const & bytecodes) {
  static auto constexpr collapsable = [](inst_t const &i) { return (i.opcode == inst_t::op_code_t::mpadd) or (i.opcode == inst_t::op_code_t::add); };

  auto opt = bytecodes |
             vws::chunk_by([](auto const& i, auto const& j) { return (i.opcode == j.opcode) and (collapsable(i)); }) |
             vws::transform([](auto chunk) {
               return *rng::fold_left_first(chunk, [](auto const &acum, auto const& i) {
                 return inst_t{acum.opcode, acum.operand + i.operand};
               });
    });
  std::vector<inst_t> bytecodes_opt;
  rng::copy(opt, std::back_inserter(bytecodes_opt)); // for lack of rng::to()  

  return bytecodes_opt;
  
}

} // namespace bfcompiler_internal
