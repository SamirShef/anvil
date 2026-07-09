#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>

namespace anvil {

template <typename T, size_t N>
class SmallVector {
    alignas (T) char _inlineBuf[(N == 0 ? 1 : N) * sizeof (T)]{};
    T     *_data = nullptr;
    size_t _len  = 0;
    size_t _cap  = N;

    void
    grow () {
        size_t newCap = std::max (_cap * 2, _cap + 1);

        auto *newBuf = static_cast<T *> (std::malloc (newCap * sizeof (T)));

        for (size_t i = 0; i < _len; ++i) {
            ::new (&newBuf[i]) T (std::move (_data[i]));
            _data[i].~T ();
        }

        if (_cap != N) {
            std::free (_data);
        }

        _data = newBuf;
        _cap  = newCap;
    }

public:
    SmallVector (const SmallVector &) = delete;
    SmallVector (SmallVector &&)      = delete;
    SmallVector &
    operator= (const SmallVector &) = delete;
    SmallVector &
    operator= (SmallVector &&) = delete;
    SmallVector () : _data ((T *) (_inlineBuf)) {}

    ~SmallVector () {
        for (size_t i = 0; i < _len; ++i) {
            _data[i].~T ();
        }
        if (_cap != N) {
            std::free (_data);
        }
    }

    void
    PushBack (const T &elem) {
        if (_len >= _cap) {
            grow ();
        }
        ::new (&_data[_len++]) T (elem);
    }

    void
    PushBack (T &&elem) {
        if (_len >= _cap) {
            grow ();
        }
        ::new (&_data[_len++]) T (std::move (elem));
    }

    T &
    At (size_t index) {
        assert (index < _len && "Index out of range");
        return _data[index];
    }

    const T &
    At (size_t index) const {
        assert (index < _len && "Index out of range");
        return _data[index];
    }

    size_t
    Size () const {
        return _len;
    }
    size_t
    Capacity () const {
        return _cap;
    }
};

}
