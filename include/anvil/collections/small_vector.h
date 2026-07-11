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
    SmallVector (const SmallVector &other) {
        if (other._len <= N) {
            _data = (T *) (_inlineBuf);
            _cap  = N;
        } else {
            _data = static_cast<T *> (std::malloc (other._len * sizeof (T)));
            _cap  = other._len;
        }

        for (size_t i = 0; i < other._len; ++i) {
            ::new (&_data[_len++]) T (other._data[i]);
        }
    }

    SmallVector &
    operator= (const SmallVector &other) {
        if (this == &other) {
            return *this;
        }

        for (size_t i = 0; i < _len; ++i) {
            _data[i].~T ();
        }
        _len = 0;

        if (_cap < other._len) {
            if (_cap != N) {
                std::free (_data);
            }
            _data = static_cast<T *> (std::malloc (other._len * sizeof (T)));
            _cap  = other._len;
        }

        for (size_t i = 0; i < other._len; ++i) {
            ::new (&_data[_len++]) T (other._data[i]);
        }

        return *this;
    }

    SmallVector &
    operator= (SmallVector &&) = default;

    SmallVector () : _data ((T *) (_inlineBuf)) {}

    SmallVector (std::initializer_list<T> init) {
        if (init.size () <= N) {
            _data = (T *) (_inlineBuf);
            _cap  = N;
        } else {
            _data = static_cast<T *> (std::malloc (init.size () * sizeof (T)));
            _cap  = init.size ();
        }

        for (const auto &elem : init) {
            ::new (&_data[_len++]) T (elem);
        }
    }

    SmallVector (SmallVector &&other) noexcept : _len (other._len), _cap (other._cap) {
        if (other._cap == N) {
            _data = (T *) _inlineBuf;
            for (size_t i = 0; i < _len; ++i) {
                ::new (&_data[i]) T (std::move (other._data[i]));
                other._data[i].~T ();
            }
        } else {
            _data       = other._data;
            other._data = (T *) other._inlineBuf;
        }
    }

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

    T &
    operator[] (size_t index) {
        return At (index);
    }

    const T &
    operator[] (size_t index) const {
        return At (index);
    }

    size_t
    Size () const {
        return _len;
    }

    size_t
    Capacity () const {
        return _cap;
    }

    bool
    Contains (const T &el) const {
        for (size_t i = 0; i < _len; ++i) {
            if (_data[i] == el) {
                return true;
            }
        }
        return false;
    }

    void
    Remove (const T &el) {
        size_t i = 0;
        for (; i < _len; ++i) {
            if (_data[i] == el) {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    _data[i].~T ();
                }
                break;
            }
        }
        for (; i < _len - 1; ++i) {
            _data[i] = std::move (_data[i + 1]);
        }
    }
};

}
