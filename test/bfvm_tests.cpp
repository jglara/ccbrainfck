#include "bfcompiler.hpp"
#include "bfvm.hpp"

#include <gtest/gtest.h>

#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace {

std::string run_vm(std::string_view program, std::string_view input = {}) {
  std::istringstream in{std::string{input}};
  std::ostringstream out;

  std::vector<inst_t> bytecode;
  compile(program, std::back_inserter(bytecode));

  BrainFckVM vm{in, out};
  vm.run(bytecode);
  return out.str();
}

} // namespace

TEST(BrainFckVM, EmitsIncrementedByte) {
  auto const output = run_vm("+.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(1));
}

TEST(BrainFckVM, EchoesInputByte) {
  auto const output = run_vm(",.", "Z");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], 'Z');
}

TEST(BrainFckVM, SetsCellToZeroOnEOF) {
  auto const output = run_vm(",.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(0));
}

TEST(BrainFckVM, SkipsLoopBodyWhenCellZero) {
  auto const output = run_vm("[+]+.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(1));
}

TEST(BrainFckVM, ExecutesLoopUntilCellZero) {
  auto const output = run_vm("++[.-]");
  std::string expected;
  expected.push_back(static_cast<char>(2));
  expected.push_back(static_cast<char>(1));
  EXPECT_EQ(output, expected);
}

TEST(BrainFckVM, MovesToNextCell) {
  auto const output = run_vm(">+.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(1));
}

TEST(BrainFckVM, WrapsLeftFromTapeOrigin) {
  auto const output = run_vm("<+.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(1));
}
