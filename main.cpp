#include "anvil/collections/small_vector.h"
#include "anvil/streams/raw_ostream.h"

int
main () {
    anvil::Outs () << "Hello from anvil's ostream!\n";
    anvil::SmallVector<int, 0> vec1;
    for (int i = 0; i < 5; ++i) {
        vec1.PushBack (i);
    }

    for (int i = 0; i < 5; ++i) {
        anvil::Outs () << vec1.At (i) << '\n';
    }

    anvil::SmallVector<int, 10> vec2;
    for (int i = 0; i < 5; ++i) {
        vec2.PushBack (i);
    }

    for (int i = 0; i < 5; ++i) {
        anvil::Outs () << vec2.At (i) << '\n';
    }

    anvil::SmallVector<int, 10> vec3;
    for (int i = 0; i < 20; ++i) {
        vec3.PushBack (i);
    }

    for (int i = 0; i < 20; ++i) {
        anvil::Outs () << vec3.At (i) << '\n';
    }
    return 0;
}
