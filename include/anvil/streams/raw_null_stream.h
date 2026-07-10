#pragma once
#include "anvil/streams/raw_ostream.h"

namespace anvil {

class RawNullStream : public RawOstream {
protected:
    void
    WriteImpl (const char *ptr, size_t size) override {
        (void) ptr;
        (void) size;
    }

public:
    RawNullStream () : RawOstream (0) {}

    RawNullStream (const RawNullStream &) = delete;
    RawNullStream (RawNullStream &&)      = default;
    RawNullStream &
    operator= (const RawNullStream &) = delete;
    RawNullStream &
    operator= (RawNullStream &&) = default;

    ~RawNullStream () override {
        FlushBuffer ();
    }
};

}
