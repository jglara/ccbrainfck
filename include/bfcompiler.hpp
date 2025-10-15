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
  static constexpr std::string_view valid{"<>+-,.[]"};

  std::vector<char> filtered_program;
  filtered_program.reserve(rng::size(program));
  for (auto const instruction : program) {
    if (valid.find(instruction) != std::string_view::npos) {
      filtered_program.push_back(instruction);
    }
  }

  auto const jumps = build_jump_table(filtered_program);

  for (std::size_t pc = 0; pc < filtered_program.size(); ++pc) {
    auto const input = filtered_program[pc];
    switch (input) {
      case '>':
        *compiled++ = inst_t{inst_t::op_code_t::mpadd, 1};
        break;
      case '<':
        *compiled++ = inst_t{inst_t::op_code_t::mpadd, -1};
        break;
      case '+':
        *compiled++ = inst_t{inst_t::op_code_t::add, 1};
        break;
      case '-':
        *compiled++ = inst_t{inst_t::op_code_t::add, -1};
        break;
      case '.':
        *compiled++ = inst_t{inst_t::op_code_t::out, 0};
        break;
      case ',':
        *compiled++ = inst_t{inst_t::op_code_t::in, 0};
        break;
      case '[':
        *compiled++ = inst_t{inst_t::op_code_t::jmpz, static_cast<std::int32_t>(jumps[pc])};
        break;
      case ']':
        *compiled++ = inst_t{inst_t::op_code_t::jmpnz, static_cast<std::int32_t>(jumps[pc])};
        break;
      default:
        break;
    }
  }
}

