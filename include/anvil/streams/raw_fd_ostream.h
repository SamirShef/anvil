#pragma once
#include "anvil/streams/raw_ostream.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace anvil {

class RawFdOstream : public RawOstream {
private:
#ifdef _WIN32
    HANDLE _handle = INVALID_HANDLE_VALUE;
#else
    int _fd = -1;
#endif
    bool _shouldClose = false;

protected:
    void
    WriteImpl (const char *ptr, size_t size) override {
#ifdef _WIN32
        DWORD bytesWritten;
        WriteFile (_handle, ptr, static_cast<DWORD> (size), &bytesWritten, nullptr);
#else
        size_t totalWritten = 0;
        while (totalWritten < size) {
            ssize_t written = ::write (_fd, ptr + totalWritten, size - totalWritten);
            if (written < 0) {
                break;
            }
            totalWritten += written;
        }
#endif
    }

public:
    RawFdOstream (const RawFdOstream &) = delete;
    RawFdOstream (RawFdOstream &&)      = default;
    RawFdOstream &
    operator= (const RawFdOstream &) = delete;
    RawFdOstream &
    operator= (RawFdOstream &&) = default;

    explicit RawFdOstream (
#ifdef _WIN32
        HANDLE standardHandle
#else
        int standardFd
#endif
        )
        : RawOstream (4096) {
#ifdef _WIN32
        _handle = standardHandle;
#else
        _fd = standardFd; // NOLINT(cppcoreguidelines-prefer-member-initializer)
#endif
    }

    ~RawFdOstream () override {
        FlushBuffer ();
#ifdef _WIN32
        if (_shouldClose && _handle != INVALID_HANDLE_VALUE) {
            CloseHandle (_handle);
        }
#else
        if (_shouldClose && _fd != -1) {
            ::close (_fd);
        }
#endif
    }
};

}
