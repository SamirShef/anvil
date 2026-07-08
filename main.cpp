#include "anvil/collections/small_vector.h"
#include "anvil/streams/raw_ostream.h"

int
main () {
    anvil::Outs () << "Hello from anvil's ostream!\n";
    anvil::SmallVector<int, 0> vec;
    for (int i = 0; i < 5; ++i) {
        vec.PushBack (i);
    }

    for (int i = 0; i < 5; ++i) {
        anvil::Outs () << vec.At (i) << '\n';
    }
    return 0;
}
