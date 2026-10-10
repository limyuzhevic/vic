# NLM Troubleshooting Guide

## Introduction

This document provides comprehensive troubleshooting guidance for NLM build and runtime issues. It covers common problems, diagnostic techniques, and solutions for various build scenarios.

## Table of Contents

1. [Common Build Issues](troubleshooting.md#common-build-issues)
2. [Python Bindings Troubleshooting](troubleshooting.md#python-bindings-troubleshooting)
3. [Packaging Issues](troubleshooting.md#packaging-issues)
4. [Cross-Platform Troubleshooting](troubleshooting.md#cross-platform-troubleshooting)
5. [Performance Issues](troubleshooting.md#performance-issues)
6. [Advanced Debugging](troubleshooting.md#advanced-debugging)
7. [Diagnostic Tools](troubleshooting.md#diagnostic-tools)
8. [Getting Help](troubleshooting.md#getting-help)

## Common Build Issues

### CMake Configuration Errors

**Error 1: CMake Minimum Version**

```
CMake Error at CMakeLists.txt:1 (cmake_minimum_required)
The CMake version is X.Y, but this file requires at least version Z.W.
```

**Symptoms:**
- Cannot configure CMake
- Basic CMake commands fail
- Projects not recognized

**Solutions:**

1. **Update CMake:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get update && sudo apt-get install -y cmake
   
   # macOS with Homebrew
   brew install cmake
   
   # Fedora/CentOS
   sudo dnf install cmake
   
   # From source
   wget https://cmake.org/files/v3.19/cmake-3.19.0.tar.gz
   tar -xvf cmake-3.19.0.tar.gz
   cd cmake-3.19.0
   ./bootstrap
   make -j$(nproc)
   sudo make install
   ```

2. **Force CMake version:**
   ```bash
   mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
   ```

**Error 2: Missing C++ Compiler**

```
The following environment variables specifying the C++ compiler are not set:

   CC
   CXX
```

**Symptoms:**
- Cannot compile C++ code
- Build fails at first compilation step
- Missing g++ or clang++

**Solutions:**

1. **Install C++ compiler:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install g++
   
   # macOS with Homebrew
   brew install gcc
   
   # Fedora/CentOS
   sudo dnf groupinstall "Development Tools"
   sudo dnf install gcc-c++
   ```

2. **Set compiler variables:**
   ```bash
   export CC=gcc
   export CXX=g++
   export CC=clang
   export CXX=clang++
   ```

3. **Create compiler symlinks (macOS):**
   ```bash
   ln -s $(which clang++) /usr/local/bin/g++
   ln -s $(which clang) /usr/local/bin/gcc
   ```

**Error 3: Python Development Files Missing**

```
Could NOT FOUND Python Interpreter component: Development
```

**Symptoms:**
- Python bindings fail to configure
- Python headers not found
- Cannot build Python extensions

**Solutions:**

1. **Install Python development packages:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install python3-dev python3-pip python3-setuptools
   
   # macOS with Homebrew
   brew install python
   
   # Fedora/CentOS
   sudo dnf install python3-devel python3-pip
   ```

2. **Check Python installation:**
   ```bash
   python3-config --includes
   python3 -c "import sys; print(sys.prefix)"
   ```

3. **Use virtual environment:**
   ```bash
   python3 -m venv venv
source venv/bin/activate
pip install pybind11
   ```

**Error 4: pybind11 Not Found**

```
Could NOT FOUND Package pybind11 CONFIG REQUIRED
```

**Symptoms:**
- Python bindings cannot be built
- pybind11 headers not found
- Cannot compile Python extensions

**Solutions:**

1. **Install pybind11 system-wide:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install pybind11-dev
   
   # Fedora/CentOS
   sudo dnf install pybind11-devel
   
   # From source
   git clone https://github.com/pybind/pybind11.git
   cd pybind11
   ```

2. **Use FetchContent:**
   ```cmake
   include(FetchContent)
   
   FetchContent_Declare(
       pybind11
       GIT_REPOSITORY https://github.com/pybind/pybind11.git
       GIT_TAG v2.10.0
   )
   
   FetchContent_MakeAvailable(pybind11)
   ```

3. **Environment variable:**
   ```bash
   export pybind11_DIR=/path/to/pybind11/cmake
   ```

**Error 5: Build Type Configuration**

```
Unknown CMake build type "CustomType"
```

**Symptoms:**
- Cannot configure custom build types
- Build fails with unknown type
- Configuration not recognized

**Solutions:**

1. **Use standard build types:**
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
   cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
   ```

2. **Multi-configuration generators:**
   ```bash
   cmake -G "Ninja Multi-Config" ..
   cmake --build . --config Release
   cmake --build . --config Debug
   ```

3. **Define custom build type:**
   ```cmake
   set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;Custom" CACHE STRING "Available build types" FORCE)
   ```

**Error 6: Installation Permissions**

```
install: Permission denied
```

**Symptoms:**
- Cannot install system-wide
- Permission denied errors
- Need sudo access

**Solutions:**

1. **Install with sudo:**
   ```bash
   sudo cmake --build . --target install
   ```

2. **Install locally:**
   ```bash
   cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/local
   cmake --build . --target install
   ```

3. **Create installation directories:**
   ```bash
   sudo mkdir -p /usr/local/lib
   sudo mkdir -p /usr/local/include
   sudo mkdir -p /usr/local/bin
   ```

### Compilation Errors

**Error 1: C++ Standard Issues**

```
error: 'concept' requires 'c++20' or greater
```

**Symptoms:**
- Cannot compile C++20 code
- Compiler doesn't recognize C++20 features
- Build fails due to standard issues

**Solutions:**

1. **Check compiler version:**
   ```bash
   g++ --version
   clang++ --version
   ```

2. **Use compiler-specific flags:**
   ```bash
   # GCC
   cmake .. -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF
   
   # Clang
   cmake .. -DCMAKE_CXX_COMPILER=clang++
   ```

3. **Update compiler:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install g++-11
   
   # Fedora/CentOS
   sudo dnf install gcc11
   
   # macOS with Homebrew
   brew install gcc
   ```

**Error 2: Header Inclusion**

```
fatal error: 'header.hpp': No such file or directory
```

**Symptoms:**
- Missing header files
- Cannot find include dependencies
- Build fails due to missing includes

**Solutions:**

1. **Check include paths:**
   ```bash
   find . -name "*.hpp" -o -name "*.h" | head -20
   ```

2. **Verify CMake include directories:**
   ```cmake
   target_include_directories(target PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
   ```

3. **Use absolute paths:**
   ```cmake
   target_include_directories(target PUBLIC /absolute/path/to/headers)
   ```

4. **Check directory structure:**
   ```bash
   ls -la src/
   find src -name "*.hpp" -type f
   ```

**Error 3: Linker Errors**

```
undefined reference to `functionName'
```

**Symptoms:**
- Linker errors during final link
- Missing function implementations
- Unresolved symbols

**Solutions:**

1. **Check library dependencies:**
   ```cmake
   target_link_libraries(target PUBLIC dependency_lib)
   ```

2. **Use CMake target dependencies:**
   ```cmake
   add_dependencies(target dependency_target)
   ```

3. **Check static vs dynamic linking:**
   ```cmake
   # Static linking
   target_link_libraries(target STATIC dependency_lib)
   
   # Dynamic linking
   target_link_libraries(target DYNAMIC dependency_lib)
   ```

4. **Verify linking order:**
   ```cmake
   target_link_libraries(target PUBLIC dependency_lib1 dependency_lib2)
   ```

### Runtime Errors

**Error 1: Missing Shared Libraries**

```
cannot open shared object file: No such file or directory: libname.so
```

**Symptoms:**
- Cannot run executables
- Missing shared libraries
- Runtime linkage errors

**Solutions:**

1. **Set LD_LIBRARY_PATH:**
   ```bash
   export LD_LIBRARY_PATH=/path/to/libraries:$LD_LIBRARY_PATH
   ```

2. **Install system libraries:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libname-dev
   
   # Fedora/CentOS
   sudo dnf install libname-devel
   ```

3. **Use RPATH:**
   ```cmake
   target_link_options(target PRIVATE "$ORIGIN")
   ```

4. **Install packages system-wide:**
   ```bash
   sudo cmake --build . --target install
   ```

## Python Bindings Troubleshooting

### Python Import Errors

**Error 1: ModuleNotFoundError: No module named 'pynlm'**

**Symptoms:**
- Cannot import pynlm module
- Python cannot find the module
- Module installation failed

**Solutions:**

1. **Check Python path:**
   ```bash
   python3 -c "import sys; print(sys.path)"
   python3 -c "import pynlm; print(pynlm.__version__)"
   ```

2. **Install Python package:**
   ```bash
   pip3 install ./python
   pip3 install -e ./python/setup.py
   ```

3. **Build Python module:**
   ```bash
   mkdir build && cd build
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON
cmake --build . --target pynlm
   ```

4. **Check module location:**
   ```bash
   find . -name "pynlm*.so" -o -name "pynlm*.pyd"
   ```

**Error 2: AttributeError: module 'pynlm' has no attribute 'Brain'**

**Symptoms:**
- Module imports but lacks expected attributes
- Python bindings incomplete
- Binding definitions missing

**Solutions:**

1. **Check binding definitions:**
   ```cpp
   PYBIND11_MODULE(pynlm, m) {
       py::class_<Brain>(m, "Brain")
           .def(py::init<>())
           .def("method", &Brain::method);
   }
   ```

2. **Rebuild Python bindings:**
   ```bash
   cmake --build . --target pynlm
   ```

3. **Verify C++ code:**
   ```bash
   grep -r "class.*Brain" src/
   ```

### Python Environment Issues

**Error 1: Python Version Mismatch**

**Symptoms:**
- Incompatible Python versions
- Different Python environments
- Version conflicts

**Solutions:**

1. **Check Python version:**
   ```bash
   python3 --version
   python3 -c "import sys; print(sys.version)"
   ```

2. **Use specific Python version:**
   ```bash
   # Use virtual environment
   python3 -m venv env
   source env/bin/activate
   pip install pybind11
   ```

3. **Configure for specific Python:**
   ```bash
   cmake .. -DPython_EXECUTABLE=/path/to/python
   ```

**Error 2: Missing Python Dependencies**

**Symptoms:**
- Missing required Python packages
- Incomplete Python environment
- Dependency resolution failures

**Solutions:**

1. **Check requirements:**
   ```bash
   cat nlm_utilities/requirements.txt
   ```

2. **Install dependencies:**
   ```bash
   pip3 install -r nlm_utilities/requirements.txt
   ```

3. **Install in development mode:**
   ```bash
   pip3 install -e ./python
   ```

## Packaging Issues

### CPack Configuration Issues

**Error 1: CPack Generator Not Found**

```
CPACK_GENERATOR: "TGZ" is not a recognized generator
```

**Symptoms:**
- Cannot create packages
- CPack generators not available
- Package creation fails

**Solutions:**

1. **Check available generators:**
   ```bash
   cpack --help | grep -A 20 "generators"
   ```

2. **Use supported generators:**
   ```bash
   cpack --generator TGZ
   cpack --generator ZIP
   cpack --generator DEB
   cpack --generator RPM
   ```

3. **Update CMake version:**
   ```bash
   sudo apt-get update && sudo apt-get install -y cpack
   ```

**Error 2: Package Metadata Missing**

**Symptoms:**
- Incomplete package metadata
- Missing package information
- Package configuration errors

**Solutions:**

1. **Check CMake configuration:**
   ```cmake
   set(CPACK_PACKAGE_NAME "NLM")
   set(CPACK_PACKAGE_VERSION "0.1.0")
   set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neural Learning Machine")
   ```

2. **Set package dependencies:**
   ```cmake
   set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6")
   set(CPACK_RPM_PACKAGE_REQUIRES "glibc, libstdc++")
   ```

3. **Add component information:**
   ```cmake
   cpack_add_component(core
       DISPLAY_NAME "Core Library"
       DESCRIPTION "Core neural simulation library"
       REQUIRED
   )
   ```

### Package Creation Issues

**Error 1: Package Build Failure**

**Symptoms:**
- Package build fails
- Cannot create distribution packages
- Package contents incorrect

**Solutions:**

1. **Clean and rebuild:**
   ```bash
   rm -rf build-package
   mkdir build-package && cd build-package
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release
   cpack
   ```

2. **Check package permissions:**
   ```bash
   chmod 755 *.deb *.rpm
   ```

3. **Verify package structure:**
   ```bash
   dpkg -x *.deb output/
   rpm -ql *.rpm
   tar -tzf *.tar.gz | head -20
   ```

## Cross-Platform Troubleshooting

### macOS Issues

**Error 1: Xcode Command Line Tools Not Found**

**Symptoms:**
- Cannot compile on macOS
- Xcode tools not available
- Compiler not found

**Solutions:**

1. **Install Xcode Command Line Tools:**
   ```bash
   xcode-select --install
   ```

2. **Check Xcode version:**
   ```bash
   xcode-select --switch /Applications/Xcode.app/Contents/Developer
   ```

3. **Set environment variables:**
   ```bash
   export SDKROOT=$(xcodebuild -sdk macosx --show-sdk-path)
   export CPATH="$SDKROOT/usr/include:$CPATH"
   ```

**Error 2: Homebrew Not Available**

**Symptoms:**
- Cannot install Homebrew packages
- Package manager not found
- Dependency installation fails

**Solutions:**

1. **Install Homebrew:**
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **Add Homebrew to PATH:**
   ```bash
   echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zshrc
   ```

### Linux Issues

**Error 1: Missing System Libraries**

**Symptoms:**
- Missing system dependencies
- Build fails due to missing libraries
- Dynamic library not found

**Solutions:**

1. **Ubuntu/Debian:**
   ```bash
   sudo apt-get update
sudo apt-get install -y build-essential libpython3-dev python3-pip \
    libgmp-dev libmpfr-dev libmpc-dev libomp-dev \
    ninja-build ccache
   ```

2. **Fedora/CentOS:**
   ```bash
   sudo dnf groupinstall "Development Tools"
   sudo dnf install python3-devel python3-pip \
       gmp-devel mpfr-devel libmpc-devel omp-devel \
       ninja-build ccache
   ```

3. **Check library paths:**
   ```bash
   ldd /path/to/executable
   find /usr/lib -name "*libname*.so*" -type f
   ```

### Windows Issues

**Error 1: Visual Studio Not Found**

**Symptoms:**
- Cannot compile on Windows
- Visual Studio tools not available
- MSVC compiler not found

**Solutions:**

1. **Install Visual Studio:**
   - Install Visual Studio with C++ workload
   - Or install Visual Studio Build Tools

2. **Set up Visual Studio Environment:**
   ```bash
   # For Developer Command Prompt
   # Or using vcvarsall.bat
   call "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
   ```

3. **Use CMake with Windows Generator:**
   ```bash
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

**Error 2: Path Issues**

**Symptoms:**
- Windows path separators causing issues
- Environment path problems
- File system compatibility issues

**Solutions:**

1. **Use forward slashes:**
   ```cmake
   target_include_directories(target PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
   ```

2. **Set Windows environment variables:**
   ```bash
   set PATH=%PATH%;C:\Program Files\NLM\bin
   set PYTHONPATH=%PYTHONPATH%;C:\Python39\Lib\site-packages
   ```

## Performance Issues

### Build Performance

**Error 1: Slow Build Times**

**Symptoms:**
- Builds take too long
- Parallel compilation not working
- Disk I/O bottleneck

**Solutions:**

1. **Use parallel builds:**
   ```bash
   make -j$(nproc)
   cmake --build . --config Release -- -j$(nproc)
   ```

2. **Use Ninja:**
   ```bash
   cmake .. -GNinja
   cmake --build . --config Release
   ```

3. **Enable ccache:**
   ```bash
   cmake .. -DCMAKE_PROJECT_INCLUDE_BEFORE=ccache.cmake
   ```

4. **Use SSD or fast storage:**
   ```bash
   # Check disk performance
   sudo systemctl status rpmspd
   ```

**Error 2: High Memory Usage**

**Symptoms:**
- Build fails due to memory constraints
- Out of memory errors
- System becomes unresponsive

**Solutions:**

1. **Increase build memory:**
   ```bash
   cmake .. -DBUILD_SHARED_LIBS=ON
   cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
   ```

2. **Use swap space:**
   ```bash
   sudo swapon /swapfile
   ```

3. **Build incrementally:**
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4  # Build with limited jobs
   ```

### Runtime Performance

**Error 1: Slow Execution**

**Symptoms:**
- Programs run too slowly
- Performance below expectations
- Resource consumption high

**Solutions:**

1. **Enable performance optimizations:**
   ```bash
   cmake .. -DNLM_ENABLE_PERFORMANCE=ON
   ```

2. **Check compiler optimizations:**
   ```bash
   # Debug build for development
   cmake .. -DCMAKE_BUILD_TYPE=Release
   
   # Release build for production
   cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
   ```

3. **Profile the code:**
   ```bash
   # Use gprof
   g++ -g -O2 -pg source.cpp -o program
   ./program
   gprof program gmon.out > report.txt
   
   # Use perf (Linux)
   perf record ./program
   perf report
   ```

4. **Use optimized algorithms:**
   ```cpp
   // Replace slow algorithms with optimized versions
   std::vector<int> slowAlgorithm(n, 0);
   std::vector<int> fastAlgorithm(n, 0);
   std::fill(fastAlgorithm.begin(), fastAlgorithm.end(), 0);
   ```

## Advanced Debugging

### Debugging Techniques

#### Using GDB

**Example GDB session:**
```bash
cd build
gdb ./nlm
(gdb) break main.cpp:42
(gdb) run
(gdb) break brain.cpp:100
(gdb) continue
(gdb) print brain.neuron_count
(gdb) backtrace
(gdb) quit
```

#### Using Valgrind

```bash
cd build
valgrind --tool=memcheck --leak-check=full ./nlm
```

#### Using AddressSanitizer

```bash
cd build
cmake .. -DNLM_ENABLE_ASAN=ON
cmake --build . --config Release
cd build
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 ./nlm
```

#### Using ThreadSanitizer

```bash
cd build
cmake .. -DNLM_ENABLE_TSAN=ON
cmake --build . --config Release
cd build
TSAN_OPTIONS=halt_on_error=1 ./nlm
```

### Profiling

#### Using gprof

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PERFORMANCE=ON
cmake --build . --config Release -j4
./nlm
make gprof
gprof nlm gmon.out > profile.txt
```

#### Using Perf (Linux)

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd build
perf record ./nlm
perf report
```

#### Using Visual Studio Profiler (Windows)

1. **Launch Visual Studio**
2. **Create new project or open existing**
3. **Set up profiling:**
   - Debug -> Performance Profiler
   - Select "CPU Usage" or "Memory Usage"
   - Start profiling
4. **Analyze results**

### Memory Leak Detection

#### Valgrind

```bash
cd build
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./nlm
```

**Common Valgrind errors:**

1. **Invalid memory access:**
   ```
   Invalid read of size 8: ...
   ```

2. **Memory leaks:**
   ```
   Possibly lost: 1,024 bytes in 16 blocks
   ```

3. **Use-after-free:**
   ```
   Invalid read of size 4: ...
   ```

#### AddressSanitizer

```bash
cd build
cmake .. -DNLM_ENABLE_ASAN=ON
cmake --build . --config Release
cd build
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 ./nlm
```

## Diagnostic Tools

### Build Analysis

#### CMake Cache Analysis

```bash
cd build
cmake -L  # List variables
```

#### Dependency Analysis

```bash
cd build
make graph > dependencies.dot
dot -Tpng dependencies.dot -o dependencies.png
```

### Runtime Analysis

#### System Monitoring

```bash
# Linux
ps aux | grep nlm
top -p $(pgrep nlm)
iostat 1 10
df -h
free -h

# macOS
ps aux | grep nlm
top -pid $(pgrep nlm)
df -h
vm_stat
```

#### Performance Monitoring

```bash
# Linux with perf
perf record -g ./nlm
perf report

# Linux with strace
strace -ff ./nlm

# macOS with instrument
instruments ./nlm
```

### Network Analysis

#### Network Monitoring

```bash
# Monitor network traffic
tcpdump -i any port 80

# Check network connections
netstat -tulpn
ss -tulpn

# Monitor process network
lsof -i -p $(pgrep nlm)
```

## Getting Help

### Online Resources

1. **NLM Documentation:**
   - https://nlm.readthedocs.io
   - https://github.com/nlm-project/nlm/wiki

2. **Build Issues:**
   - Stack Overflow: tag with `[nlm] [cmake]`
   - GitHub Issues: https://github.com/nlm-project/nlm/issues

3. **Python Bindings:**
   - pybind11 Documentation: https://pybind11.readthedocs.io
   - Python Packages: https://pypi.org/project/pynlm

4. **Community Support:**
   - Discord: https://discord.gg/nlm
   - Reddit: r/neurallearningmachine
   - Mailing List: nlm-dev@nlm.org

### Local Support

1. **Build Logs:**
   - Save complete build logs: `cat build/CMakeFiles/Makefile.cmake`
   - Capture CMake output: `mkdir build && cd build && cmake .. 2>&1 | tee cmake.log`

2. **Configuration Files:**
   - Check CMake cache: `cd build && cat CMakeCache.txt | grep CONFIG`
   - List targets: `cd build && cmake --build . --target help`

3. **Environment Information:**
   ```bash
   uname -a
systeminfo
lscpu
grep "Memory" /proc/meminfo
   ```

4. **Compiler Information:**
   ```bash
   g++ --version
   python3 --version
   cmake --version
   ninja --version
   ```

### Emergency Support

If you're stuck with a build issue:

1. **Gather information:**
   - Save CMake output
   - Capture error messages
   - Note system configuration
   - Document build steps

2. **Create minimal test case:**
   ```bash
   mkdir test-build
   cd test-build
   echo 'cmake_minimum_required(VERSION 3.16)
project(test)' > CMakeLists.txt
   cmake .
   ```

3. **Check for common issues:**
   - CMake version
   - Compiler support
   - Python installation
   - System dependencies

4. **File an issue with information:**
   - Build logs
   - Error messages
   - System information
   - Steps to reproduce

### Next Steps

If you cannot resolve the issue:

1. **Check for updates:** Ensure you're using the latest version
2. **Try different approaches:** Test various build configurations
3. **Seek help:** Use online resources and community support
4. **Create an issue:** Provide comprehensive information
5. **Consider alternatives:** Use pre-built packages if available

## Conclusion

The NLM build system provides comprehensive troubleshooting guidance for various build and runtime scenarios. By following the steps outlined in this guide, you can effectively diagnose and resolve common build issues, optimize performance, and ensure successful deployment across different platforms.

Key troubleshooting strategies:

- **Systematic approach:** Start with basic checks and progress to advanced diagnostics
- **Reproduce the issue:** Create minimal test cases to isolate problems
- **Use proper tools:** Leverage debugging, profiling, and analysis tools
- **Document findings:** Keep detailed logs for future reference
- **Seek community help:** Use online resources and community support

Remember that build and deployment issues often have multiple solutions. Experiment with different approaches, and don't hesitate to seek help when needed.
