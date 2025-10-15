#pragma once
#include <cstdint>

struct inst_t {
  enum class op_code_t : std::uint8_t {
    nop,
    mpadd,  // add a value to memory pointer
    add,    // add a value to the memory
    jmpz,    // jump to location if [mem] == 0
    jmpnz, // jump to location if [mem] != 0
    in, // input 1 char at [mem]
    out, // output 1 char at [mem]
  };

  op_code_t opcode;
  std::int32_t operand;
  
};
