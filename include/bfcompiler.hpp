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

  auto filter_program_view = program | vws::filter([](auto const &c) { return valid.find(c) != std::string_view::npos; });
  std::vector<char> filtered_program{filter_program_view.begin(), filter_program_view.end()};
  
  auto const jumps = build_jump_table(filtered_program);

  auto compile_program =  vws::zip_transform([&jumps](const auto &pc, const auto &input) {
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
      return inst_t{inst_t::op_code_t::jmpz, static_cast<std::int32_t>(jumps[pc])};
      
    case ']':
      return inst_t{inst_t::op_code_t::jmpnz, static_cast<std::int32_t>(jumps[pc])};
    }
    return inst_t{inst_t::op_code_t::nop, 0}; // should not happen
  }, vws::iota(0), filtered_program);

  rng::copy(compile_program, compiled);
}

