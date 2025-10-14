#include "ccbf.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <string_view>

namespace {

std::string run_program(std::string_view program, std::string_view input = {}) {
  std::istringstream in{std::string{input}};
  std::ostringstream out;
  BFMachine machine{in, out};

  std::string program_buffer{program};
  machine.run(program_buffer);
  return out.str();
}

} // namespace

TEST(BFMachine, OutputsIncrementedByte) {
  auto const output = run_program("+.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(1));
}

TEST(BFMachine, DecrementInstructionRestoresValue) {
  auto const output = run_program("+-.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(0));
}

TEST(BFMachine, MovesDataPointerRightAndLeft) {
  auto const output = run_program(">+.<.");
  std::string expected{static_cast<char>(1), static_cast<char>(0)};
  EXPECT_EQ(output, expected);
}

TEST(BFMachine, DataPointerWrapsLeft) {
  auto const output = run_program("<+.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(1));
}

TEST(BFMachine, ReadsAndEchoesInputByte) {
  auto const output = run_program(",.", "A");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], 'A');
}

TEST(BFMachine, SkipsLoopWhenCellZero) {
  auto const output = run_program("[+++]++.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(2));
}

TEST(BFMachine, ExecutesLoopUntilCellZero) {
  auto const output = run_program("+[->+<]>.");
  ASSERT_EQ(output.size(), 1u);
  EXPECT_EQ(output[0], static_cast<char>(1));
}

