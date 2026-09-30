#include "anvil/streams/raw_ostream.h"
#include "anvil/streams/raw_fd_ostream.h"
#include "anvil/streams/raw_null_stream.h"

namespace anvil {

RawOstream &
Outs () {
#ifdef _WIN32
    static RawFdOstream outs (GetStdHandle (STD_OUTPUT_HANDLE));
#else
    static RawFdOstream outs (1);
#endif
    return outs;
}

RawOstream &
Errs () {
#ifdef _WIN32
    static RawFdOstream errs (GetStdHandle (STD_ERROR_HANDLE));
#else
    static RawFdOstream errs (2);
#endif
    return errs;
}

RawOstream &
Nulls () {
    static RawNullStream null;
    return null;
}

}
