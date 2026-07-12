#pragma once
#include <cstddef>

namespace anvil {

struct LiveInterval {
    size_t Start = ~0ULL;
    size_t End   = 0;

    void
    Define (size_t start) {
        if (Start == ~0ULL) {
            Start = start;
        }
    }

    void
    Use (size_t end) {
        if (End == 0) {
            End = end;
        }
    }

    bool
    InterferesWith (const LiveInterval &other) const {
        return Start <= other.End && other.Start <= End;
    }
};

}
