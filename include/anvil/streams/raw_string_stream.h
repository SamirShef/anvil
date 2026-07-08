#include "anvil/streams/raw_ostream.h"

#include <string>

namespace anvil {

class RawStringStream : public RawOstream {
private:
    std::string &_targetStr;

protected:
    void
    WriteImpl (const char *ptr, size_t size) override {
        _targetStr.append (ptr, size);
    }

public:
    RawStringStream (const RawStringStream &) = delete;
    RawStringStream (RawStringStream &&)      = default;
    RawStringStream &
    operator= (const RawStringStream &) = delete;
    RawStringStream &
    operator= (RawStringStream &&) = delete;

    explicit RawStringStream (std::string &str) : RawOstream (0), _targetStr (str) {}

    ~RawStringStream () override { FlushBuffer (); }
};

}
