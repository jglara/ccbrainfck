#include "bfcompiler.hpp"
#include <iterator>
#include <ranges>
#include <numeric>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace vws = std::ranges::views;
namespace rng = std::ranges;

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
        std::cerr << "Unmatched closing bracket at instruction " << i << '\n';
        throw std::runtime_error("Unmatched closing bracket in Brainfuck program");
      }
      auto const match = loop_stack.back();
      loop_stack.pop_back();
      bytecodes[match].operand = i;
      bytecodes[i].operand = match;
    }
  }

  if (!loop_stack.empty()) {
    for (auto const unmatched_index : loop_stack) {
      std::cerr << "Unmatched opening bracket at instruction " << unmatched_index << '\n';
    }
    throw std::runtime_error("Unmatched opening bracket in Brainfuck program");
  }
}


////// First optimization
// collapse consecutive adding operations
std::vector<inst_t> optimize_bytecodes_opt1(std::vector<inst_t> const& bytecodes) {

  static auto constexpr collapsable = [](inst_t const &i) { return (i.opcode == inst_t::op_code_t::mpadd) or (i.opcode == inst_t::op_code_t::add); };

  auto opt_view = bytecodes |
             vws::chunk_by([](auto const& i, auto const& j) { return (i.opcode == j.opcode) and (collapsable(i)); }) |
             vws::transform([](auto chunk) {
               return *rng::fold_left_first(
                 chunk, [](auto const& acum, auto const& i) { return inst_t{acum.opcode, acum.operand + i.operand}; });
                 });
  std::vector<inst_t> bytecodes_opt;
  rng::copy(opt_view, std::back_inserter(bytecodes_opt));  // for lack of rng::to()

  return bytecodes_opt;
  
}


/// Helper function to return loop depths in the program
std::vector<size_t> loop_depths(std::vector<inst_t> const &prg) {

  
  size_t depth{0};
  auto depth_inc = [&depth](auto const& i) {
    auto ret = depth;
    if (i.opcode == inst_t::op_code_t::jmpz) {
      ++depth;
      ret = depth;
    } else if (i.opcode == inst_t::op_code_t::jmpnz) {
      --depth;
    }
    return ret;
  };

  std::vector<size_t> ret;

  rng::copy( prg | vws::transform(depth_inc), std::back_inserter(ret));

  return ret;

}

  
//// Second optimization
// look for optimizable loops like [-] -> mem[mp] = 0
std::vector<inst_t> optimize_bytecodes_opt2(std::vector<inst_t> const & bytecodes) {

  auto constexpr chunk_by_depth = [](auto const& i_d, auto const& j_d) {
    auto [_i1, d1] = i_d;
    auto [_i2, d2] = j_d;

    return d1 == d2;
  };

  auto constexpr reduce_loop = [](auto const&& loop) {
    if (rng::distance(loop) == 3
        and loop[0].opcode == inst_t::op_code_t::jmpz
        and loop[1].opcode == inst_t::op_code_t::add and loop[1].operand == -1
        and loop[2].opcode == inst_t::op_code_t::jmpnz
      ) {
      std::vector<inst_t> loop_opt;
      loop_opt.emplace_back(inst_t{inst_t::op_code_t::set, 0});
      return loop_opt;
    } else {
      std::vector<inst_t> loop_copy(loop.begin(), loop.end());
      return loop_copy;
    }
  };
  
  auto opt_view = vws::join(vws::zip(bytecodes, loop_depths(bytecodes))
                        | vws::chunk_by(chunk_by_depth)
                        | vws::transform([](auto const &&chunk) { return chunk
                              | vws::transform([] (auto const &i_d) { auto [i,_d] = i_d; return i;}); })
                        | vws::transform(reduce_loop)
    );

  std::vector<inst_t> bytecodes_opt;
  rng::copy(opt_view, std::back_inserter(bytecodes_opt));  // for lack of rng::to()

  return bytecodes_opt;
  
}  

} // namespace bfcompiler_internal
