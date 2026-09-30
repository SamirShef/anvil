#include "anvil/target/triple.h"
#include "anvil/collections/small_vector.h"

namespace anvil {

void
Triple::parse (std::string_view triple) {
    if (triple.empty ()) {
        return;
    }

    SmallVector<std::string_view, 4> tokens;
    size_t                           start = 0;
    while (true) {
        size_t pos = triple.find ('-', start);
        if (pos == std::string_view::npos) {
            tokens.PushBack (triple.substr (start));
            break;
        }
        tokens.PushBack (triple.substr (start, pos - start));
        start = pos + 1;
    }

    if (tokens.Size () == 0) {
        return;
    }

    _arch = parseArch (tokens[0]);
    switch (tokens.Size ()) {
    case 2: {
        _vendor = Vendor::UnknownVendor;
        _os     = parseOS (tokens[1]);
        _env    = Environment::Unknown;
    }
    case 3: {
        auto maybeOS = parseOS (tokens[1]);
        if (maybeOS != OS::Unknown) {

            _vendor = Vendor::UnknownVendor;
            _os     = maybeOS;
            _env    = parseEnv (tokens[2]);
        } else {
            _vendor = parseVendor (tokens[1]);
            _os     = parseOS (tokens[2]);
            _env    = Environment::Unknown;
        }
    }
    case 4: {
        _vendor = parseVendor (tokens[1]);
        _os     = parseOS (tokens[2]);
        _env    = parseEnv (tokens[3]);
    }
    default:
        break;
    }
}

Arch
Triple::parseArch (std::string_view s) {
    if (s == "x86_64") {
        return Arch::X86_64;
    }
    if (s == "x86" || s == "i386") {
        return Arch::X86;
    }
    if (s == "aarch64") {
        return Arch::AArch64;
    }
    if (s == "riscv64") {
        return Arch::RiscV64;
    }
    return Arch::Unknown;
}

OS
Triple::parseOS (std::string_view s) {
    if (s == "linux") {
        return OS::Linux;
    }
    if (s == "windows") {
        return OS::Windows;
    }
    if (s == "darwin") {
        return OS::Darwin;
    }
    if (s == "freebsd") {
        return OS::FreeBSD;
    }
    if (s == "uefi") {
        return OS::UEFI;
    }
    return OS::Unknown;
}

Vendor
Triple::parseVendor (std::string_view s) {
    if (s == "pc") {
        return Vendor::PC;
    }
    if (s == "apple") {
        return Vendor::Apple;
    }
    if (s == "amd") {
        return Vendor::AMD;
    }
    if (s == "unknown") {
        return Vendor::UnknownVendor;
    }
    return Vendor::Unknown;
}

Environment
Triple::parseEnv (std::string_view s) {
    if (s == "gnu") {
        return Environment::GNU;
    }
    if (s == "msvc") {
        return Environment::MSVC;
    }
    if (s == "musl") {
        return Environment::Musl;
    }
    if (s == "android") {
        return Environment::Android;
    }
    return Environment::Unknown;
}

}
