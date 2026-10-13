# NLM - Building Guide

**Date:** 2026-10-13
**Audience:** Developers and contributors
**Level:** Building guide

---

## Table of Contents

1. [Quick Start Building](#quick-start-building)
2. [Standard Build Methods](#standard-build-methods)
3. [Build Configuration Options](#build-configuration-options)
4. [Build Outputs](#build-outputs)
5. [Build Verification](#build-verification)
6. [Build Troubleshooting](#build-troubleshooting)

---

## Quick Start Building

### Quick Build Commands

#### Quick Build (Release)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Quick Build (Debug)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Build with Python Bindings
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=ON
cmake --build . -j$(nproc)
```

#### Build with Advanced API
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_ADVANCED_API=ON
make -j$(nproc)
```

## Standard Build Methods

### Method 1: Standard CMake Build

#### Create Build Directory
```bash
cd /workspace/app
mkdir build
cd build
```

#### Configure Build
```bash
cmake .. [options]
```

#### Build
```bash
make -j$(nproc)
```

### Method 2: Cross-Platform Build

#### For Different Platforms
```bash
# Linux
cd /workspace/app
mkdir build_linux
cd build_linux
cmake .. -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# macOS
cd /workspace/app
mkdir build_macos
cd build_macos
cmake .. -DCMAKE_SYSTEM_NAME=Darwin -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Windows (MinGW)
cd /workspace/app
mkdir build_windows
cd build_windows
cmake .. -G "MinGW Makefiles" -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Method 3: Docker Build

#### Build Docker Image
```bash
cd /workspace/app
cat > Dockerfile << 'EOF'
FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev

COPY requirements.txt .
RUN pip3 install -r requirements.txt

COPY . /nlm
WORKDIR /nlm

RUN mkdir build && cd build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j$(nproc)

RUN pip3 install -e .

ENTRYPOINT ["/nlm/build/nlm"]
