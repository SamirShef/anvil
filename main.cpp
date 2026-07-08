#include "anvil/streams/raw_ostream.h"

int
main () {
    anvil::Outs () << "Hello from anvil's ostream!\n";
    anvil::Outs () << INT64_MIN << '\n';
    return 0;
}
