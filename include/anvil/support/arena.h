#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdlib>

namespace anvil {

class ArenaAllocator {
    struct Chunk {
        Chunk *Next;
        size_t Offset;
        size_t Cap;
    };

    Chunk                  *_cur{};
    static constexpr size_t DEFAULT_CHUNK_SIZE
        = static_cast<const size_t> (64 * 1024 * 1024);

public:
    ArenaAllocator (const ArenaAllocator &) = delete;
    ArenaAllocator (ArenaAllocator &&)      = default;
    ArenaAllocator &
    operator= (const ArenaAllocator &) = delete;
    ArenaAllocator &
    operator= (ArenaAllocator &&) = default;
    ArenaAllocator ()             = default;

    ~ArenaAllocator () {
        reset ();
    }

    template <typename T>
    T *
    Alloc () {
        return Alloc<T> (1);
    }

    template <typename T>
    T *
    Alloc (size_t count) {
        size_t size = sizeof (T) * count;
        if (size == 0) {
            return nullptr;
        }

        size_t alignment = alignof (T);

        if (_cur == nullptr) {
            allocChunk (size + alignment);
        }

        size_t alignedOffset = alignUp (_cur->Offset, alignment);

        if (alignedOffset + size > _cur->Cap) {
            allocChunk (size + alignment);
            alignedOffset = alignUp (_cur->Offset, alignment);
        }

        auto *raw    = (char *) _cur + alignedOffset;
        _cur->Offset = alignedOffset + size;

        return (T *) raw;
    }

private:
    static size_t
    alignUp (size_t size, size_t align) {
        return (size + align - 1) & ~(align - 1);
    }

    Chunk *
    allocChunk (size_t size) {
        size_t headerSize    = sizeof (Chunk);
        size_t requiredSpace = size + headerSize;
        size_t sizeToAlloc   = std::max (requiredSpace, DEFAULT_CHUNK_SIZE);
        void  *mem           = malloc (sizeToAlloc);
        auto  *chunk         = (Chunk *) mem;
        chunk->Cap           = sizeToAlloc;
        chunk->Offset        = headerSize;
        chunk->Next          = _cur;
        _cur                 = chunk;
        return chunk;
    }

    void
    reset () {
        auto *cur = _cur;
        while (cur != nullptr) {
            auto *next = cur->Next;
            free (cur);
            cur = next;
        }
    }
};

}
