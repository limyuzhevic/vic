# NLM Build Guide

# Table of Contents

1. [Introduction](#introduction)
2. [Quick Start](#quick-start)
3. [Build Configurations](#build-configurations)
   - [Debug](#debug)
   - [Release](#release)
   - [RelWithDebInfo](#relwithdebinfo)
   - [MinSizeRel](#minsizerel)
4. [Platform-Specific Builds](#platform-specific-builds)
   - [Linux](#linux)
   - [macOS](#macos)
   - [Windows](#windows)
5. [Build Optimization Tips](#build-optimization-tips)
6. [Build Artifacts Documentation](#build-artifacts-documentation)
   - [Executables](#executables)
   - [Libraries](#libraries)
7. [Performance Build Options](#performance-build-options)
   - [SIMD Builds](#simd-builds)
   - [OpenMP Configuration](#openmp-configuration)
   - [Threading Options](#threading-options)
   - [Memory Optimization](#memory-optimization)
8. [Example Scripts](#example-scripts)
   - [build.sh (Linux/macOS)](#buildsh-linuxmacos)
   - [CMake Configuration Examples](#cmake-configuration-examples)
   - [Installation Examples](#installation-examples)
9. [Troubleshooting](#troubleshooting)
   - [Common Errors](#common-errors)
   - [FAQ](#faq)

<a name="introduction"></a>
# Introduction

This document provides comprehensive instructions for building the NLM (Neural Learning Machine) project from source. NLM is an experimental artificial developmental brain implementation written in C++20.

<a name="quick-start"></a>
# Quick Start

## Basic Build Commands

```bash
# Create and navigate to build directory
mkdir build
cd build

# Configure with Release build (recommended for production)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Or build specific targets
make nlm                    # Main executable
make nlm_phase3_demo        # Phase 3 demo
make nlm_phase4_demo        # Phase 4 demo
```

## Run After Build

```bash
# Phase 6: Final Integration Demo (default)
./nlm

# Phase 3: Basic neural computation experiments
./nlm_phase3_demo

# Phase 4: Complex cognitive experiments
./nlm_phase4_demo
```

<a name="build-configurations"></a>
# Build Configurations

<a name="debug"></a>
## Debug

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

**Characteristics**: Includes debug symbols, no optimizations, full assertions.

<a name="release"></a>
## Release

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**Characteristics**: Maximum optimizations, no debug symbols, fastest execution.

<a name="relwithdebinfo"></a>
## RelWithDebInfo

```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j$(nproc)
```

**Characteristics**: Optimizations with debug symbols for production debugging.

<a name="minsizerel"></a>
## MinSizeRel

```bash
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make -j$(nproc)
```

**Characteristics**: Optimizations focused on minimal binary size.

<a name="platform-specific-builds"></a>
# Platform-Specific Builds

<a name="linux"></a>
## Linux

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential cmake git pkg-config libomp-dev libeigen3-dev

mkdir nlm-build
cd nlm-build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DBUILD_SHARED_LIBS=OFF \
         -DENABLE_OPENMP=ON \
         -DENABLE_AVX2=ON
cmake --build . --config Release -- -j$(nproc)
```

<a name="macos"></a>
## macOS

### With Homebrew

```bash
brew install cmake gcc ninja pkg-config

mkdir nlm-build
cd nlm-build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -G "Ninja" \
         -DBUILD_SHARED_LIBS=OFF
ninja
```

<a name="windows"></a>
## Windows

### Visual Studio

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

<a name="build-optimization-tips"></a>
# Build Optimization Tips

## Parallel Building

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -- -j$(nproc)
```

## Incremental Builds

```bash
make clean-nlm
cmake --build . --target nlm --clean-first
make nlm
```

<a name="build-artifacts-documentation"></a>
# Build Artifacts Documentation

<a name="executables"></a>
## Executables

### `nlm`
**Description**: Main executable for Phase 6: Final Integration
**Usage**: `./nlm` (demonstrates integration of all brain systems)

**Key Features**:
- Integration verification of all brain systems
- Memory system tests
- Neuromodulation system tests
- Checkpoint save/load functionality
- Replay and consolidation tests

**Command Line Options**:
```bash
./nlm                  # Standard Phase 6 demo
./nlm --phase2         # Also run Phase 2 experiments
./nlm --verbose        # Verbose output
./nlm --checkpoint=file  # Use specific checkpoint file
```

### `nlm_phase3_demo`
**Description**: Phase 3 demonstration executable
**Purpose**: World interaction loop with reward prediction error

### `nlm_phase4_demo`
**Description**: Phase 4 demonstration executable
**Purpose**: Complex cognitive functions including prediction

<a name="libraries"></a>
## Libraries

### `libnlm_core.a`
**Description**: Core brain components static library
**Components**: All neural, memory, cognition, and neuromodulation systems

### `libnlm_agent.a`
**Description**: Agent system static library

### `libnlm_world.a`
**Description**: World system static library

<a name="performance-build-options"></a>
# Performance Build Options

<a name="simd-builds"></a>
## SIMD Builds

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_AVX2=ON \
         -DENABLE_SSE4_2=ON
cmake --build . --config Release
```

<a name="openmp-configuration"></a>
## OpenMP Configuration

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_OPENMP=ON
cmake --build . --config Release
```

<a name="threading-options"></a>
## Threading Options

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -- -j$(nproc)
```

<a name="memory-optimization"></a>
## Memory Optimization

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_MEMORY_POOLS=ON
cmake --build . --config Release
```

<a name="example-scripts"></a>
# Example Scripts

<a name="buildsh-linuxmacos"></a>
## build.sh (Linux/macOS)

```bash
#!/bin/bash

# NLM Build Script for Linux/macOS
set -euo pipefail

BUILD_TYPE="Release"
BUILD_DIR="build"
ENABLE_OPENMP=true
ENABLE_AVX2=true
PARALLEL_JOBS=$(nproc)

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --no-openmp)
            ENABLE_OPENMP=false
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DENABLE_OPENMP="$ENABLE_OPENMP" \
    -DENABLE_AVX2="$ENABLE_AVX2"

cmake --build . --config "$BUILD_TYPE" -- -j$PARALLEL_JOBS
```

<a name="troubleshooting"></a>
# Troubleshooting

<a name="common-errors"></a>
## Common Errors

### "CMake cannot find Python"

**Solution**:
```bash
pip install pybind11 scikit-build-core
cmake .. -DPython_EXECUTABLE=$(which python3)
```

### "Compiler does not support C++20"

**Solution**:
```bash
# Upgrade compiler or use Docker
cmake .. -DCMAKE_CXX_COMPILER=/usr/bin/g++-11
```

### "OpenMP not found"

**Solution**:
```bash
sudo apt install libomp-dev
cmake .. -DENABLE_OPENMP=OFF
```

### "SIMD instructions not supported"

**Solution**:
```bash
cmake .. -DENABLE_AVX2=OFF -DENABLE_SSE4_2=OFF
```

<a name="faq"></a>
## FAQ

**Q: What build type should I use for development?**
**A**: Use `Debug` for development and debugging.

**Q: Why does my build take so long?**
**A**: Use `-j$(nproc)` for parallel builds.

**Q: How do I run unit tests?**
**A**: Run `./nlm_test` after building.
