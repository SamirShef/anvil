#pragma once
#include <cstdint>
#include <string_view>

namespace anvil {

enum class Arch : uint8_t {
    Unknown,
    X86,
    X86_64, // NOLINT
    AArch64,
    Arm64,
    RiscV32,
    RiscV64,
    Wasm32,
    Wasm64
};

enum class OS : uint8_t { Unknown, Linux, Windows, Darwin, FreeBSD, UEFI };

enum class Vendor : uint8_t { Unknown, PC, Apple, AMD, UnknownVendor };

enum class Environment : uint8_t { Unknown, GNU, MSVC, Musl, Android };

class Triple {
    Arch        _arch   = Arch::Unknown;
    OS          _os     = OS::Unknown;
    Vendor      _vendor = Vendor::Unknown;
    Environment _env    = Environment::Unknown;

public:
    Triple () = default;

    explicit Triple (std::string_view triple) {
        parse (triple);
    }

    Arch
    GetArch () const {
        return _arch;
    }

    OS
    GetOS () const {
        return _os;
    }

    Vendor
    GetVendor () const {
        return _vendor;
    }

    Environment
    GetEnv () const {
        return _env;
    }

private:
    void
    parse (std::string_view triple);

    static Arch
    parseArch (std::string_view s);

    static OS
    parseOS (std::string_view s);

    static Vendor
    parseVendor (std::string_view s);

    static Environment
    parseEnv (std::string_view s);
};

}
