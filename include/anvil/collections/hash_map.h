#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <utility>

namespace anvil {

inline uint32_t
// NOLINTNEXTLINE(readability-identifier-naming)
Murmur3_32 (const void *key, size_t len, uint32_t seed = 0) noexcept {
    const auto  *data    = (const uint8_t *) (key);
    const size_t NBLOCKS = len / 4;
    uint32_t     h1      = seed;

    constexpr uint32_t C1 = 0xCC9E2D51;
    constexpr uint32_t C2 = 0x1B873593;

    for (size_t i = 0; i < NBLOCKS; ++i) {
        uint32_t k = 0;
        std::memcpy (&k, data + (i * 4), sizeof (uint32_t));

        k *= C1;
        k = (k << 15) | (k >> 17);
        k *= C2;

        h1 ^= k;
        h1 = (h1 << 13) | (h1 >> 19);
        h1 = (h1 * 5) + 0xE6546B64;
    }

    const uint8_t *tail = data + (NBLOCKS * 4);
    uint32_t       k1   = 0;

    switch (len & 3) {
    case 3:
        k1 ^= static_cast<uint32_t> (tail[2]) << 16;
        [[fallthrough]];
    case 2:
        k1 ^= static_cast<uint32_t> (tail[1]) << 8;
        [[fallthrough]];
    case 1:
        k1 ^= tail[0];
        k1 *= C1;
        k1 = (k1 << 15) | (k1 >> 17);
        k1 *= C2;
        h1 ^= k1;
    default:
        break;
    }

    h1 ^= static_cast<uint32_t> (len);

    h1 ^= h1 >> 16;
    h1 *= 0x85EBCA6B;
    h1 ^= h1 >> 13;
    h1 *= 0xC2B2AE35;
    h1 ^= h1 >> 16;

    return h1;
}

template <typename T>
inline uint32_t
HashType (const T &value, uint32_t seed = 0) noexcept {
    return Murmur3_32 (&value, sizeof (T), seed);
}

template <typename K, typename V>
class HashMap {
    enum class BucketState : uint8_t { Empty, Occupied, Deleted };

    struct Bucket {
        alignas (K) uint8_t KeyBytes[sizeof (K)]{};
        alignas (V) uint8_t ValueBytes[sizeof (V)]{};
        BucketState State = BucketState::Empty;
    };

    Bucket *_buckets       = nullptr;
    size_t  _size          = 0;
    size_t  _capacity      = 0;
    float   _maxLoadFactor = 0.7F;

    size_t
    findLookupSlot (const K &key) const noexcept {
        if (_capacity == 0) {
            return 0;
        }

        size_t idx = HashType (key) & (_capacity - 1);
        for (size_t i = 0; i < _capacity; ++i) {
            if (_buckets[idx].State == BucketState::Empty) {
                return _capacity;
            }
            if (_buckets[idx].State == BucketState::Occupied
                && *(const K *) (_buckets[idx].KeyBytes) == key) {
                return idx;
            }
            idx = (idx + 1) & (_capacity - 1);
        }
        return _capacity;
    }

    size_t
    findInsertionSlot (const K &key) const noexcept {
        size_t idx          = HashType (key) & (_capacity - 1);
        size_t firstDeleted = _capacity;

        for (size_t i = 0; i < _capacity; ++i) {
            if (_buckets[idx].State == BucketState::Empty) {
                return firstDeleted != _capacity ? firstDeleted : idx;
            }
            if (_buckets[idx].State == BucketState::Deleted) {
                if (firstDeleted == _capacity) {
                    firstDeleted = idx;
                }
            } else if (_buckets[idx].State == BucketState::Occupied
                       && *(const K *) (_buckets[idx].KeyBytes) == key) {
                return idx;
            }
            idx = (idx + 1) & (_capacity - 1);
        }
        return firstDeleted;
    }

    void
    rehash (size_t newCapacity) {
        Bucket *oldBuckets  = _buckets;
        size_t  oldCapacity = _capacity;

        _capacity = newCapacity;
        _buckets  = new Bucket[_capacity]();
        _size     = 0;

        for (size_t i = 0; i < oldCapacity; ++i) {
            if (oldBuckets[i].State == BucketState::Occupied) {
                auto &oldKey   = *(K *) (oldBuckets[i].KeyBytes);
                auto &oldValue = *(V *) (oldBuckets[i].ValueBytes);

                size_t idx = findInsertionSlot (oldKey);
                new (_buckets[idx].KeyBytes) K (std::move (oldKey));
                new (_buckets[idx].ValueBytes) V (std::move (oldValue));
                _buckets[idx].State = BucketState::Occupied;
                ++_size;

                if constexpr (!std::is_trivially_destructible_v<K>) {
                    oldKey.~K ();
                }
                if constexpr (!std::is_trivially_destructible_v<V>) {
                    oldValue.~V ();
                }
            }
        }
        delete[] oldBuckets;
    }

    void
    clearBuckets () noexcept {
        if constexpr (!std::is_trivially_destructible_v<K>
                      || !std::is_trivially_destructible_v<V>) {
            if (_buckets) {
                for (size_t i = 0; i < _capacity; ++i) {
                    if (_buckets[i].State == BucketState::Occupied) {
                        if constexpr (!std::is_trivially_destructible_v<K>) {
                            ((K *) (_buckets[i].KeyBytes))->~K ();
                        }
                        if constexpr (!std::is_trivially_destructible_v<V>) {
                            ((V *) (_buckets[i].ValueBytes))->~V ();
                        }
                    }
                }
            }
        }
    }

public:
    // NOLINTBEGIN(readability-identifier-naming)
    struct KeyValue {
        const K &first;
        V       &second;
    };

    struct ConstKeyValue {
        const K &first;
        const V &second;
    };

    class Iterator {
        Bucket *_current;
        Bucket *_end;

        void
        advance () noexcept {
            while (_current < _end && _current->State != BucketState::Occupied) {
                ++_current;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = KeyValue;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = KeyValue;

        Iterator (Bucket *start, Bucket *end) noexcept : _current (start), _end (end) {
            advance ();
        }

        KeyValue
        operator* () const noexcept {
            return { *(const K *) (_current->KeyBytes), *(V *) (_current->ValueBytes) };
        }

        Iterator &
        operator++ () noexcept {
            ++_current;
            advance ();
            return *this;
        }

        Iterator
        operator++ (int) noexcept {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool
        operator== (const Iterator &other) const noexcept {
            return _current == other._current;
        }

        bool
        operator!= (const Iterator &other) const noexcept {
            return _current != other._current;
        }
    };

    class ConstIterator {
        const Bucket *_current;
        const Bucket *_end;

        void
        advance () noexcept {
            while (_current < _end && _current->State != BucketState::Occupied) {
                ++_current;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = ConstKeyValue;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = ConstKeyValue;

        ConstIterator (const Bucket *start, const Bucket *end) noexcept
            : _current (start), _end (end) {
            advance ();
        }

        ConstKeyValue
        operator* () const noexcept {
            return { *(const K *) (_current->KeyBytes),
                     *(const V *) (_current->ValueBytes) };
        }

        ConstIterator &
        operator++ () noexcept {
            ++_current;
            advance ();
            return *this;
        }

        ConstIterator
        operator++ (int) noexcept {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool
        operator== (const ConstIterator &other) const noexcept {
            return _current == other._current;
        }

        bool
        operator!= (const ConstIterator &other) const noexcept {
            return _current != other._current;
        }
    };

    Iterator
    begin () noexcept {
        return Iterator (_buckets, _buckets + _capacity);
    }

    Iterator
    end () noexcept {
        return Iterator (_buckets + _capacity, _buckets + _capacity);
    }

    ConstIterator
    begin () const noexcept {
        return ConstIterator (_buckets, _buckets + _capacity);
    }

    ConstIterator
    end () const noexcept {
        return ConstIterator (_buckets + _capacity, _buckets + _capacity);
    }

    ConstIterator
    cbegin () const noexcept {
        return ConstIterator (_buckets, _buckets + _capacity);
    }

    ConstIterator
    cend () const noexcept {
        return ConstIterator (_buckets + _capacity, _buckets + _capacity);
    }

    // NOLINTEND(readability-identifier-naming)

    HashMap () noexcept = default;

    explicit HashMap (size_t initialCapacity) : _capacity (1) {
        while (_capacity < initialCapacity) {
            _capacity <<= 1;
        }
        _buckets = new Bucket[_capacity]();
    }

    ~HashMap () noexcept {
        clearBuckets ();
        delete[] _buckets;
    }

    HashMap (const HashMap &other)
        : _capacity (other._capacity), _maxLoadFactor (other._maxLoadFactor) {
        if (_capacity > 0) {
            _buckets = new Bucket[_capacity]();
            for (size_t i = 0; i < _capacity; ++i) {
                if (other._buckets[i].State == BucketState::Occupied) {
                    new (_buckets[i].KeyBytes)
                        K (*(const K *) (other._buckets[i].KeyBytes));
                    new (_buckets[i].ValueBytes)
                        V (*(const V *) (other._buckets[i].ValueBytes));
                    _buckets[i].State = BucketState::Occupied;
                    ++_size;
                }
            }
        }
    }

    HashMap &
    operator= (const HashMap &other) {
        if (this != &other) {
            clearBuckets ();
            delete[] _buckets;

            _capacity      = other._capacity;
            _size          = 0;
            _maxLoadFactor = other._maxLoadFactor;
            _buckets       = _capacity > 0 ? new Bucket[_capacity]() : nullptr;

            for (size_t i = 0; i < _capacity; ++i) {
                if (other._buckets[i].State == BucketState::Occupied) {
                    new (_buckets[i].KeyBytes)
                        K (*(const K *) (other._buckets[i].KeyBytes));
                    new (_buckets[i].ValueBytes)
                        V (*(const V *) (other._buckets[i].ValueBytes));
                    _buckets[i].State = BucketState::Occupied;
                    ++_size;
                }
            }
        }
        return *this;
    }

    HashMap (HashMap &&other) noexcept
        : _buckets (other._buckets),
          _size (other._size),
          _capacity (other._capacity),
          _maxLoadFactor (other._maxLoadFactor) {
        other._buckets  = nullptr;
        other._size     = 0;
        other._capacity = 0;
    }

    HashMap &
    operator= (HashMap &&other) noexcept {
        if (this != &other) {
            clearBuckets ();
            delete[] _buckets;

            _buckets       = other._buckets;
            _size          = other._size;
            _capacity      = other._capacity;
            _maxLoadFactor = other._maxLoadFactor;

            other._buckets  = nullptr;
            other._size     = 0;
            other._capacity = 0;
        }
        return *this;
    }

    bool
    Insert (const K &key, const V &value) noexcept (
        std::is_nothrow_copy_constructible_v<K>
        && std::is_nothrow_copy_constructible_v<V>) {
        if (_capacity == 0
            || static_cast<float> (_size + 1)
                   > static_cast<float> (_capacity) * _maxLoadFactor) {
            size_t newCap = _capacity == 0 ? 16 : _capacity * 2;
            rehash (newCap);
        }

        size_t  idx    = findInsertionSlot (key);
        Bucket &bucket = _buckets[idx];

        if (bucket.State == BucketState::Occupied) {
            *(V *) (bucket.ValueBytes) = value;
            return false;
        }

        new (bucket.KeyBytes) K (key);
        new (bucket.ValueBytes) V (value);
        bucket.State = BucketState::Occupied;
        ++_size;
        return true;
    }

    V *
    Find (const K &key) noexcept {
        size_t idx = findLookupSlot (key);
        if (idx == _capacity) {
            return nullptr;
        }
        return (V *) (_buckets[idx].ValueBytes);
    }

    const V *
    Find (const K &key) const noexcept {
        size_t idx = findLookupSlot (key);
        if (idx == _capacity) {
            return nullptr;
        }
        return (const V *) (_buckets[idx].ValueBytes);
    }

    bool
    Remove (const K &key) noexcept {
        size_t idx = findLookupSlot (key);
        if (idx == _capacity) {
            return false;
        }

        Bucket &bucket = _buckets[idx];
        if constexpr (!std::is_trivially_destructible_v<K>) {
            ((K *) (bucket.KeyBytes))->~K ();
        }
        if constexpr (!std::is_trivially_destructible_v<V>) {
            ((V *) (bucket.ValueBytes))->~V ();
        }
        bucket.State = BucketState::Deleted;
        --_size;
        return true;
    }

    void
    Clear () noexcept {
        clearBuckets ();
        if (_buckets) {
            for (size_t i = 0; i < _capacity; ++i) {
                _buckets[i].State = BucketState::Empty;
            }
        }
        _size = 0;
    }

    size_t
    Size () const noexcept {
        return _size;
    }

    size_t
    Capacity () const noexcept {
        return _capacity;
    }

    bool
    IsEmpty () const noexcept {
        return _size == 0;
    }
};

}
