#include "bfcompiler.hpp"

#include <gtest/gtest.h>

#include <iterator>
#include <string>
#include <vector>

namespace {

std::vector<inst_t> compile_program(std::string const& program) {
  return compile(program);
}

}  // namespace

TEST(BFCompiler, ProducesBasicInstructions) {
  auto const bytecode = compile_program(">+<-.");
  ASSERT_EQ(bytecode.size(), 5u);

  EXPECT_EQ(bytecode[0].opcode, inst_t::op_code_t::mpadd);
  EXPECT_EQ(bytecode[0].operand, 1);

  EXPECT_EQ(bytecode[1].opcode, inst_t::op_code_t::add);
  EXPECT_EQ(bytecode[1].operand, 1);

  EXPECT_EQ(bytecode[2].opcode, inst_t::op_code_t::mpadd);
  EXPECT_EQ(bytecode[2].operand, -1);

  EXPECT_EQ(bytecode[3].opcode, inst_t::op_code_t::add);
  EXPECT_EQ(bytecode[3].operand, -1);

  EXPECT_EQ(bytecode[4].opcode, inst_t::op_code_t::out);
  EXPECT_EQ(bytecode[4].operand, 0);
}

TEST(BFCompiler, GeneratesMatchingJumpTargets) {
  auto const bytecode = compile_program("[>+<-]");
  ASSERT_EQ(bytecode.size(), 6u);

  EXPECT_EQ(bytecode[0].opcode, inst_t::op_code_t::jmpz);
  EXPECT_EQ(bytecode[0].operand, 5);

  EXPECT_EQ(bytecode[1].opcode, inst_t::op_code_t::mpadd);
  EXPECT_EQ(bytecode[1].operand, 1);

  EXPECT_EQ(bytecode[2].opcode, inst_t::op_code_t::add);
  EXPECT_EQ(bytecode[2].operand, 1);

  EXPECT_EQ(bytecode[3].opcode, inst_t::op_code_t::mpadd);
  EXPECT_EQ(bytecode[3].operand, -1);

  EXPECT_EQ(bytecode[4].opcode, inst_t::op_code_t::add);
  EXPECT_EQ(bytecode[4].operand, -1);

  EXPECT_EQ(bytecode[5].opcode, inst_t::op_code_t::jmpnz);
  EXPECT_EQ(bytecode[5].operand, 0);
}

TEST(BFCompiler, IgnoresNonBrainfuckCharacters) {
  auto const bytecode = compile_program(">a+b>?!");
  ASSERT_EQ(bytecode.size(), 3u);

  EXPECT_EQ(bytecode[0].opcode, inst_t::op_code_t::mpadd);
  EXPECT_EQ(bytecode[0].operand, 1);

  EXPECT_EQ(bytecode[1].opcode, inst_t::op_code_t::add);
  EXPECT_EQ(bytecode[1].operand, 1);

  EXPECT_EQ(bytecode[2].opcode, inst_t::op_code_t::mpadd);
  EXPECT_EQ(bytecode[2].operand, 1);
}

TEST(BFCompiler, OptimizesZeroingLoopToSet) {
  auto const bytecode = compile_program("[-]");
  ASSERT_EQ(bytecode.size(), 1u);

  EXPECT_EQ(bytecode[0].opcode, inst_t::op_code_t::set);
  EXPECT_EQ(bytecode[0].operand, 0);
}
