#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <string_view>
#include <unistd.h>

namespace anvil {

class RawOstream {
protected:
    char *_bufStart = nullptr;
    char *_bufPtr   = nullptr;
    char *_bufEnd   = nullptr;

    virtual void
    WriteImpl (const char *ptr, size_t size) = 0;

    void
    FlushBuffer () {
        if (_bufPtr != _bufStart) {
            WriteImpl (_bufStart, _bufPtr - _bufStart);
            _bufPtr = _bufStart;
        }
    }

public:
    RawOstream (RawOstream &&) = default;
    RawOstream &
    operator= (RawOstream &&) = default;

    explicit RawOstream (size_t bufSize) {
        if (bufSize > 0) {
            _bufStart = new char[bufSize];
            _bufPtr   = _bufStart;
            _bufEnd   = _bufStart + bufSize;
        }
    }

    virtual ~RawOstream () {
        FlushBuffer ();
        delete[] _bufStart;
    }

    RawOstream (const RawOstream &) = delete;
    RawOstream &
    operator= (const RawOstream &) = delete;

    void
    Write (const char *ptr, size_t size) {
        size_t freeSpace = _bufEnd - _bufPtr;

        if (size > freeSpace) {
            FlushBuffer ();
            if (size > static_cast<size_t> (_bufEnd - _bufStart)) {
                WriteImpl (ptr, size);
                return;
            }
        }

        std::memcpy (_bufPtr, ptr, size);
        _bufPtr += size;
    }

    void
    Flush () {
        FlushBuffer ();
    }

    RawOstream &
    operator<< (std::string_view str) {
        Write (str.data (), str.size ());
        return *this;
    }

    RawOstream &
    operator<< (const char *str) {
        Write (str, std::strlen (str));
        return *this;
    }

    RawOstream &
    operator<< (uint64_t num) {
        char  localBuf[21];
        char *p = localBuf + 20;
        *p      = '\0';

        if (num == 0) {
            *--p = '0';
        } else {
            while (num > 0) {
                *--p = '0' + (num % 10); // NOLINT(bugprone-narrowing-conversions,
                                         // cppcoreguidelines-narrowing-conversions)
                num /= 10;
            }
        }
        return *this << p;
    }
};

extern RawOstream &
Outs ();

extern RawOstream &
Errs ();

extern RawOstream &
Nulls ();

}
