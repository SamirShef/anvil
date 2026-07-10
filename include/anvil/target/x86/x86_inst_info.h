#pragma once
#include <cstdint>

namespace anvil::x86 {

enum OpCode : uint8_t { MOV64ri = 0, MOV64rr, ADD64rr, SUB64rr, RET };

}
