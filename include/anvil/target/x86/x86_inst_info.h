#pragma once
#include <cstdint>

namespace anvil::x86 {

enum OpCode : uint8_t {
    MOV64ri = 0,
    MOV64rr,
    MOV64rm,
    MOV64mr,
    ADD64rr,
    SUB64rr,
    SUB64ri,
    RET,
    PUSH64r,
    POP64r,
};

}
