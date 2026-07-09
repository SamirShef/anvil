#include "anvil/streams/raw_ostream.h"
#include "anvil/support/arena.h"

int
main () {
    anvil::Outs () << "Hello from anvil's ostream!\n";
    anvil::ArenaAllocator arena{};
    size_t                size = 10;
    auto                 *arr  = arena.Alloc<int> (size);
    for (int i = 0; i < size; ++i) {
        arr[i] = i;
    }
    for (size_t i = 0; i < size; ++i) {
        if (i != 0) {
            anvil::Outs () << ", ";
        }
        anvil::Outs () << arr[i];
    }
    anvil::Outs () << '\n';
    return 0;
}
