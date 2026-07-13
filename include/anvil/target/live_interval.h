#pragma once
#include <algorithm>
#include <cstddef>

namespace anvil {

struct LiveInterval {
    size_t Start = ~0ULL;
    size_t End   = 0;

    void
    Define (size_t start) {
        Start = start;
        End   = std::max (End, Start);
    }

    void
    Use (size_t end) {
        if (End == 0) {
            End = end;
        }
        if (Start == ~0ULL || end < Start) {
            Start = end;
        }
    }

    bool
    InterferesWith (const LiveInterval &other) const {
        return Start < other.End && other.Start < End;
    }
};

}
