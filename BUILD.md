# NLM Build Troubleshooting Guide

This document provides comprehensive troubleshooting guidance for the Neural Learning Machine (NLM) build system. It covers common issues, solutions, and best practices for building NLM.

## Table of Contents

1. [Quick Reference](#quick-reference)
2. [Common Issues](#common-issues)
3. [Dependency Issues](#dependency-issues)
4. [Compiler Issues](#compiler-issues)
5. [CMake Issues](#cmake-issues)
6. [Build System Issues](#build-system-issues)
7. [Platform-Specific Issues](#platform-specific-issues)
8. [Performance Issues](#performance-issues)
9. [Installation Issues](#installation-issues)
10. [Advanced Troubleshooting](#advanced-troubleshooting)
11. [Build Debugging](#build-debugging)
12. [Environment Setup](#environment-setup)
13. [Community Support](#community-support)

## Quick Reference

| Symptom | Possible Cause | Solution |
|---------|----------------|----------|
| "CMake not found" | CMake not installed | Install CMake 3.16+ |
| "Compiler not supported" | Old compiler | Upgrade to GCC 9+/Clang 10+/MSVC 19.2+ |
| "Package not found" | Missing dependencies | Use package manager or install manually |
| "Build fails" | Configuration error | Check CMake configuration |
| "Tests fail" | Code issues | Debug and fix code |
| "Installation fails" | Permission issues | Use sudo or install to user directory |

## Common Issues

### 1. "CMake not found"

**Description:** CMake is not installed or not in PATH.

**Solution:**

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
# Download from https://cmake.org/download/
# Or use Chocolatey: choco install cmake
```

**Verification:**

```bash
cmake --version
# Should output: cmake version 3.16.0 or higher
```

### 2. "Compiler not supported"

**Description:** The compiler doesn't meet NLM's minimum requirements.

**Solution:**

```bash
# Check compiler version
g++ --version
clang++ --version
```

**Upgrade compiler:**

```bash
# Ubuntu/Debian
sudo apt-get install g++

# macOS
brew install gcc

# Windows
# Install Visual Studio 2019 or later
```

### 3. "Unable to find nlohmann_json"

**Description:** nlohmann/json library not found.

**Solution:**

```bash
# Using Conan (preferred)
curl -L https://raw.githubusercontent.com/conan-io/conan/master/conanfile.py -o conanfile.py
python3 conanfile.py install

# Using vcpkg
./vcpkg integrate install
./vcpkg install nlohmann-json

# Manual installation
sudo apt-get install libnlohmann-json-dev
```

### 4. "Unable to find yaml-cpp"

**Description:** yaml-cpp library not found.

**Solution:**

```bash
# Using Conan
conan install yaml-cpp/0.8.0

# Using vcpkg
./vcpkg install yaml-cpp

# Manual installation
sudo apt-get install libyaml-cpp-dev
```

### 5. "Build fails with 'command not found'"

**Description:** Build system command not found.

**Solution:**

```bash
# Install build tools
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Windows
# Install Visual Studio Build Tools
```

## Dependency Issues

### Conan Dependency Management

#### Conan Installation

```bash
# Install Conan
curl -L https://raw.githubusercontent.com/conan-io/conan/master/conanfile.py -o conanfile.py
python3 conanfile.py install

# Or use pip
pip3 install conan
```

#### Common Conan Issues

**Issue:** "Conan command not found"

**Solution:**

```bash
# Add Conan to PATH
cat >> ~/.bashrc << EOF
export PATH="$HOME/.local/bin:$PATH"
EOF
source ~/.bashrc

# Or install via pip
pip3 install conan
```

**Issue:** "Recipe not found"

**Solution:**

```bash
# Create conanfile.py
conan new nlm/0.1.0 -t

# Or use existing conanfile.py from NLM
```

#### vcpkg Dependency Management

**vcpkg Installation**

```bash
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap
./bootstrap-vcpkg.sh

# Integrate
./vcpkg integrate install
```

**vcpkg Configuration**

```bash
# Install dependencies
./vcpkg install nlohmann-json yaml-cpp pybind11

# Configure CMake
cmake .. -G "Ninja" -A x64 -DVCPKG_ROOT=$(pwd)/vcpkg
```

### Python Dependency Issues

#### Python Installation Issues

**Issue:** "Python not found"

**Solution:**

```bash
# Ubuntu/Debian
sudo apt-get install python3 python3-pip

# macOS
brew install python3

# Windows
# Download from https://www.python.org/downloads/
```

**Issue:** "pybind11 not found"

**Solution:**

```bash
# Install pybind11 via pip
pip3 install pybind11[global]

# Or install via CMake (preferred)
./build.py --components all --python-bindings
```

## Compiler Issues

### Compiler Version Issues

#### Minimum Requirements

| Compiler | Minimum Version | Test Command |
|----------|----------------|--------------|
| GCC | 9.0 | g++ --version |
| Clang | 10.0 | clang++ --version |
| MSVC | 19.2 | cl /? |

#### Compiler-Specific Issues

**GCC Issues**

**Issue:** "ISO C++ forbids converting 'long' signed to 'bool'"

**Solution:**

```bash
# Add compiler flags
cmake .. -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wno-implicit-fallthrough"
```

**Issue:** "‘std::variant’ is not supported by this compiler"

**Solution:**

```bash
# Upgrade compiler
# Or use different build type
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Clang Issues**

**Issue:** "error: unknown warning option '-Wno-implicit-fallthrough'"

**Solution:**

```bash
# Use different warning flags
cmake .. -DCMAKE_CXX_FLAGS="-Wall -Wextra"
```

**MSVC Issues**

**Issue:** "__cplusplus not set to expected value"

**Solution:**

```bash
# Use specific compiler flags
cmake .. -DCMAKE_CXX_FLAGS="/std:c++20 /permissive-"
```

### Parallel Build Issues

**Issue:** "Parallel build fails with segmentation fault"

**Solution:**

```bash
# Reduce parallel jobs
cmake --build . -- -j2

# Or disable parallel build
cmake --build .
```

**Issue:** "Build time increases with more cores"

**Solution:**

```bash
# Use fewer cores
cmake --build . -- -j$(nproc)

# Use cache to speed up subsequent builds
cmake --build . --build-type Release
```

## CMake Issues

### CMake Configuration Issues

**Issue:** "CMakeCache.txt not found"

**Solution:**

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake ..
```

**Issue:** "Cannot find package nlohmann_json"

**Solution:**

```bash
# Add library path
cmake .. -Dnlohmann_json_DIR=/path/to/nlohmann_json

# Or use FetchContent
cmake .. -DNLM_ENABLE_FETCHCONTENT=ON
```

**Issue:** "CMAKE_BUILD_TYPE not set"

**Solution:**

```bash
# Set build type
cmake .. -DCMAKE_BUILD_TYPE=Release

# Or use build presets
cmake --build . --preset=nlm-release
```

### CMake Generator Issues

**Issue:** "Ninja build tool not found"

**Solution:**

```bash
# Install Ninja
# Ubuntu/Debian
sudo apt-get install ninja-build

# macOS
brew install ninja

# Windows
# Download from https://github.com/ninja-build/ninja/releases
```

**Issue:** "Visual Studio version not found"

**Solution:**

```bash
# Specify generator
cmake .. -G "Visual Studio 16 2019" -A x64
```

### CPack Configuration Issues

**Issue:** "CPack not found"

**Solution:**

```bash
# Install CPack
# Ubuntu/Debian
sudo apt-get install cpack

# macOS
brew install cpack

# Windows
# CPack is included with CMake
```

**Issue:** "Package creation failed"

**Solution:**

```bash
# Check CPack configuration
cat CPackConfig.cmake

# Set package version
cmake .. -DPACKAGE_VERSION=0.1.0
```

## Build System Issues

### Python Build Script Issues

**Issue:** "python3 build.py not found"

**Solution:**

```bash
# Check if build.py exists
ls -la build.py

# Make executable
chmod +x build.py
```

**Issue:** "Invalid arguments"

**Solution:**

```bash
# Check help
python3 build.py --help

# Use correct arguments
python3 build.py --build-type Release --install
```

### Shell Script Issues

**Issue:** "Permission denied"

**Solution:**

```bash
# Make executable
chmod +x install.sh

# Run with sudo if needed
sudo ./install.sh
```

**Issue:** "Verbose mode not working"

**Solution:**

```bash
# Check script for verbose flag
./install.sh --verbose
```

## Platform-Specific Issues

### Linux Issues

**Issue:** "Unable to find libstdc++"

**Solution:**

```bash
# Install development packages
sudo apt-get install g++ libstdc++-dev
```

**Issue:** "Unable to find libpthread"

**Solution:**

```bash
# Install pthread library
sudo apt-get install libc6-dev
```

### macOS Issues

**Issue:** "Xcode command line tools not found"

**Solution:**

```bash
# Install Xcode command line tools
xcode-select --install
```

**Issue:** "Homebrew not found"

**Solution:**

```bash
# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Windows Issues

**Issue:** "Visual Studio not found"

**Solution:**

```bash
# Install Visual Studio
# Or use WSL
wsl --install
```

**Issue:** "CMake not found in PATH"

**Solution:**

```bash
# Add CMake to PATH
cat >> %USERPROFILE%\AppData\Local\Programs\CMake\bin\cmake.bat
```

## Performance Issues

### Slow Build Times

**Issue:** "Build takes too long"

**Solution:**

```bash
# Use incremental builds
cmake --build . --build-type Release

# Use cache
cmake --build . --cache

# Use parallel build
cmake --build . -- -j$(nproc)
```

**Issue:** "Compilation takes too long"

**Solution:**

```bash
# Reduce optimization level
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Use faster compiler flags
cmake .. -DCMAKE_CXX_FLAGS="-O2 -march=native"
```

### Memory Issues

**Issue:** "Out of memory during build"

**Solution:**

```bash
# Use less memory
cmake --build . -- -j2

# Build one component at a time
cmake --build . --target nlm_core
```

**Issue:** "Swap usage too high"

**Solution:**

```bash
# Increase memory
cmake .. -DCMAKE_BUILD_TYPE=Release

# Use faster SSD
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Installation Issues

### Permission Issues

**Issue:** "Permission denied"

**Solution:**

```bash
# Use sudo for system installation
sudo cmake --install .

# Or install to user directory
cmake --install . --prefix ~/.local
```

**Issue:** "Directory not found"

**Solution:**

```bash
# Create directory
mkdir -p /usr/local/include/nlm

# Install with correct prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
```

### Configuration Issues

**Issue:** "Library not found"

**Solution:**

```bash
# Check installation
ldconfig -p | grep nlm

# Reinstall
rm -rf /usr/local/lib/*nlm*
cmake .. --install
```

### Python Installation Issues

**Issue:** "Python module not found"

**Solution:**

```bash
# Check Python path
python3 -c "import sys; print(sys.path)"

# Install with correct Python
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
```

## Advanced Troubleshooting

### Debug Build

**Issue:** "Cannot reproduce crash"

**Solution:**

```bash
# Create debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Add debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0"
```

**Issue:** "Hard to reproduce bug"

**Solution:**

```bash
# Use deterministic build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Run with specific options
ctest --output-on-failure -R test_name
```

### Profile Build

**Issue:** "Performance bottleneck"

**Solution:**

```bash
# Enable profiling
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_PROFILING=ON

# Profile with GDB

cd build
gdb ./nlm

# In GDB
b main
run
```

**Issue:** "Memory leak"

**Solution:**

```bash
# Enable memory detection
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_ASAN=ON

# Run with valgrind

cd build
valgrind --tool=memcheck ./nlm_test
```

### Cross-Compilation

**Issue:** "Cross-compilation not working"

**Solution:**

```bash
# Set toolchain file
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake

# Set architecture
cmake .. -DCMAKE_SYSTEM_PROCESSOR=armv7
```

## Build Debugging

### Enable Debug Output

```bash
# Enable verbose output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON

# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Run with debug output
cmake --build . --verbose
```

### Use ccache

**ccache Installation**

```bash
# Ubuntu/Debian
sudo apt-get install ccache

# macOS
brew install ccache

# Windows
choco install ccache
```

**ccache Configuration**

```bash
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

### Cache Build Artifacts

**CMake Cache**

```bash
# Create build directory
cd build

# Configure
cmake ..

# Build
cmake --build .

# Clean build
rm -rf build
```

**Compiler Cache**

```bash
# Clear compiler cache
rm -rf ~/.cache/ccache

# Clear build directory
rm -rf build
```

## Environment Setup

### Typical Development Environment

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-pip \
    clang-format \
    clang-tidy \
    ccache

# macOS
xcode-select --install
brew install cmake ninja python3 git clang-format clang-tidy ccache

# Windows (WSL)
# Run Ubuntu on Windows
```

### Development Tools

**Essential Tools**

- **CMake:** 3.16+
- **Compiler:** GCC 9+/Clang 10+/MSVC 19.2+
- **Build System:** Ninja or Make
- **Version Control:** Git
- **IDE:** Visual Studio Code, CLion, or similar

**Optional Tools**

- **Static Analysis:** clang-tidy, cppcheck
- **Formatting:** clang-format
- **Debugging:** GDB, LLDB
- **Profiling:** Valgrind, perf
- **Package Management:** Conan, vcpkg

**Recommended Tools**

- **Build System:** Python build script
- **Documentation:** Doxygen, Sphinx
- **Testing:** ctest, Google Test
- **CI/CD:** GitHub Actions, GitLab CI

## Community Support

### Online Resources

#### Documentation

- **Official Documentation:** https://docs.nlm.org
- **Build Documentation:** https://docs.nlm.org/build
- **API Documentation:** https://docs.nlm.org/api
- **User Guide:** https://docs.nlm.org/user

#### Community Forums

- **GitHub Discussions:** https://github.com/nlm/nlm/discussions
- **Reddit:** https://www.reddit.com/r/NLM/
- **Discord:** https://discord.nlm.org
- **Stack Overflow:** https://stackoverflow.com/questions/tagged/nlm

#### Support Channels

| Channel | Purpose | Link |
|---------|---------|------|
| GitHub Issues | Bug reports, feature requests | https://github.com/nlm/nlm/issues |
| Discord | General discussion, help | https://discord.nlm.org |
| Community Forum | Discussion, tutorials | https://community.nlm.org |
| Email | Business inquiries | team@nlm.org |

### Getting Help

#### Step 1: Reproduce the Issue

1. **Check version:** Identify your NLM version
2. **Reproduce steps:** Follow exact steps to reproduce
3. **Gather information:** Collect error messages and system info
4. **Test simple case:** Try with minimal configuration

#### Step 2: Search for Solutions

1. **GitHub Issues:** Search existing issues
2. **Documentation:** Check documentation
3. **Forum:** Search community forums
4. **Stack Overflow:** Search Stack Overflow

#### Step 3: Create Support Request

1. **Use GitHub Issues:** Create new issue
2. **Provide details:** Include all relevant information
3. **Include logs:** Attach log files if available
4. **Be patient:** Wait for community response

### Contributing to Documentation

#### Fix Documentation

1. **Fork repository:** Fork the NLM repository
2. **Fix issue:** Fix the documentation issue
3. **Create pull request:** Submit changes as pull request
4. **Review:** Have changes reviewed and merged

#### Improve Documentation

1. **Identify needs:** Identify areas for improvement
2. **Write documentation:** Write comprehensive documentation
3. **Test documentation:** Test documentation with examples
4. **Submit changes:** Submit as pull request

## Conclusion

Troubleshooting build issues is a common part of development. By following these guidelines and using the available tools and resources, you should be able to resolve most build issues quickly and efficiently.

Remember to:

1. **Check documentation:** Always check documentation first
2. **Reproduce issue:** Reproduce the issue before seeking help
3. **Search solutions:** Search for existing solutions
4. **Provide details:** Provide complete information when seeking help
5. **Be patient:** Be patient when waiting for community response

For the most up-to-date troubleshooting information, check the NLM documentation and community forums.

---

*Last updated: $(date -u +%Y-%m-%d)*
