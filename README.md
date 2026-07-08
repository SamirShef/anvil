# anvil

`anvil` is a lightweight, self-contained code generation and optimization backend designed for compilers and programming language infrastructures.

## Overview

The purpose of `anvil` is to provide a clean, fast, and completely dependency-free low-level code generation
framework. It architecture-independently processes intermediate representation (IR), applies
target-independent and machine-specific optimizations, and emits native assembly or object files for
specified platforms.

By intentionally avoiding heavy monolithic frameworks and relying entirely on lightweight, custom-tailored
data structures, `anvil` serves as a robust, minimalist foundational block for building compiler backends.

## Key Goals

- **Zero External Dependencies:** Built entirely with modern standard C++ and custom high-performance
utilities (including dedicated arena allocators, tailored hash tables, and small vectors).
- **Clean Extensibility:** Uses a rigid `TargetMachine` abstraction layer, decoupling core IR optimization
logic from specific hardware architectures.
- **Strict Efficiency:** Optimized for fast compilation passes, minimal memory overhead, and mechanical
sympathy with modern modern hardware.

## Licensing

This project is dual-licensed under the MIT License and the Apache License (Version 2.0), at your option.
You may choose to use the software under the terms of either license.

- For details regarding the MIT license, see the [LICENSE-MIT](LICENSE-MIT) file.
- For details regarding the Apache 2.0 license, see the [LICENSE-APACHE](LICENSE-APACHE) file.
