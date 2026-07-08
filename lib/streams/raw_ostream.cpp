#include "anvil/streams/raw_ostream.h"
#include "anvil/streams/raw_fd_ostream.h"
#include "anvil/streams/raw_null_stream.h"

namespace anvil {

RawOstream &
Outs () {
    static RawFdOstream outs (1);
    return outs;
}

RawOstream &
Errs () {
    static RawFdOstream errs (2);
    return errs;
}

RawOstream &
Nulls () {
    static RawNullStream null;
    return null;
}

}
