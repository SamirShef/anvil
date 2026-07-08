#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <string_view>
#include <unistd.h>

namespace anvil {

static char *
IntToStr (uint64_t num, char *buf) {
    char *p = buf + 20;
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
    return p;
}

static char *
IntToStr (int64_t num, char *buf) {
    char *p = buf + 20;
    *p      = '\0';

    if (num == 0) {
        *--p = '0';
    } else {
        bool isNeg = num < 0;
        if (isNeg) {
            num = -num;
        }
        while (num > 0) {
            *--p = '0' + (num % 10); // NOLINT(bugprone-narrowing-conversions,
                                     // cppcoreguidelines-narrowing-conversions)
            num /= 10;
        }
        if (isNeg) {
            *--p = '-';
        }
    }
    return p;
}

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
    operator<< (unsigned char c) {
        Write ((const char *) &c, 1);
        return *this;
    }

    RawOstream &
    operator<< (signed char c) {
        Write ((const char *) &c, 1);
        return *this;
    }

    RawOstream &
    operator<< (char c) {
        Write ((const char *) &c, 1);
        return *this;
    }

    // NOLINTBEGIN(bugprone-macro-parentheses)
#define integet_op(prefix, bits)                                                         \
    RawOstream &operator<< (prefix##int##bits##_t num) {                                 \
        char localBuf[21];                                                               \
        return *this << IntToStr (static_cast<prefix##int64_t> (num), localBuf);         \
    }

#define uint_op(bits) integet_op (u, bits)
#define int_op(bits) integet_op (, bits)

    uint_op (16);
    uint_op (32);
    uint_op (64);

    int_op (16);
    int_op (32);
    int_op (64);

#undef uint_op
#undef int_op
#undef integet_op

    // NOLINTEND(bugprone-macro-parentheses)
};

extern RawOstream &
Outs ();

extern RawOstream &
Errs ();

extern RawOstream &
Nulls ();

}
