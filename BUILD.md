# NLM Build and Installation Guide

## Quick Start

```bash
# Clone the repository
mkdir nlm
cd nlm

# Build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install (optional)
sudo make install

# Run tests
ctest --output-on-failure
```

## Detailed Build Instructions

### Prerequisites

- **CMake**: Version 3.16 or higher
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **Ninja**: Optional (faster build)
- **Doxygen**: Optional (for documentation)

### Step 1: Clone Repository

```bash
# Clone NLM repository
git clone https://github.com/nlm-project/nlm.git
ncd nlm

# Or for development
# git clone --recursive https://github.com/nlm-project/nlm.git
```

### Step 2: Configure Build

Choose one of the following build configurations:

#### Standard Release Build (Recommended)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

#### Debug Build (with debugging symbols)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

#### Custom Build Options

```bash
mkdir build
cd build
cmake ..
```

This will bring up an interactive configuration menu. You can also specify options directly:

```bash
# Build with all features
cmake .. -DBUILD_PYTHON=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DBUILD_VISUALIZATION=ON

# Build with Python bindings only
cmake .. -DBUILD_PYTHON=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF

# Build with Address Sanitizer (development)
cmake .. -DENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug

# Build with Thread Sanitizer
cmake .. -DENABLE_THREAD_SANITIZER=ON -DCMAKE_BUILD_TYPE=Debug
```

### Step 3: Build the Project

```bash
# Standard build
make -j$(nproc)

# If using Ninja
ninja-build

# Build specific targets
make nlm               # Main executable
make nlm_test         # Test suite
make pynlm            # Python bindings (if enabled)
```

### Step 4: Install (Optional)

The default installation prefix is `/usr/local`. You can change this with `-DCMAKE_INSTALL_PREFIX`:

```bash
# System-wide installation (requires sudo)
sudo make install

# User installation (recommended for development)
make install DESTDIR=/path/to/prefix
```

### Step 5: Verify Installation

```bash
# Check if executables are installed
ls -la /usr/local/bin/ | grep nlm

# Check if libraries are installed
ls -la /usr/local/lib/ | grep nlm

# Check if headers are installed
ls -la /usr/local/include/nlm/

# Run the main executable
/usr/local/bin/nlm --help

# Run tests
/usr/local/bin/nlm_test
```

### Step 6: Run Tests

```bash
# Run the test suite
cd build
ctest --output-on-failure

# Or run specific test target
make test

# Run tests with verbose output
ctest -V
```

## Build Options

### Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON | Build test executables |
| `BUILD_EXAMPLES` | ON | Build example executables |
| `BUILD_PYTHON` | ON | Build Python bindings |
| `BUILD_DOCS` | OFF | Build documentation |
| `BUILD_VISUALIZATION` | ON | Build visualization support |
| `ENABLE_COVERAGE` | OFF | Enable code coverage reporting |
| `ENABLE_ASAN` | OFF | Enable Address Sanitizer |
| `ENABLE_UBSAN` | OFF | Enable Undefined Behavior Sanitizer |
| `ENABLE_THREAD_SANITIZER` | OFF | Enable Thread Sanitizer |

### Python Bindings Configuration

If Python bindings are enabled, they will be installed in the Python site-packages directory:

```bash
# Check Python module location
python -c "import pynlm; print(pynlm.__file__)"

# Verify installation
python -c "import pynlm; print(f'NLM version: {pynlm.__version__}')"
```

### Visualization Configuration

Visualization support is enabled by default. To disable it:

```bash
cmake .. -DBUILD_VISUALIZATION=OFF
```

## Platform-Specific Instructions

### Linux/Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake g++ python3-dev python3-pip python3-venv

# For development with Python bindings
sudo apt-get install pybind11-dev python3-pybind11

# Build and install
mkdir build
cd build
cmake ..
make -j$(nproc)

# Install
sudo make install
```

### macOS

```bash
# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"

# Install dependencies
brew install cmake python3 pybind11

# Build and install
mkdir build
cd build
cmake ..
make -j$(nproc)

# Install (use --with-python for custom Python)
sudo make install
```

### Windows

#### Using Visual Studio

```cmd
# Open Developer Command Prompt for VS
# Navigate to repository
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 16 2019" -A x64

# Build
cmake --build . --config Release -- /m

# Install
cmake --install . --config Release
```

#### Using MinGW/MSYS2

```bash
# Install MSYS2 from https://www.msys2.org/
# Update packages
sudo pacman -Syu

# Install build tools
sudo pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-python3

# Build (use mingw32-make)
mingw32-make -j$(nproc)
```

## Development Workflow

### Building with Python Bindings

```bash
# Build with Python bindings enabled
cd build
cmake .. -DBUILD_PYTHON=ON
make -j$(nproc)

# Install Python module
pip install -e .

# Test Python bindings
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('Python bindings OK')"
```

### Building with All Features

```bash
cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DBUILD_PYTHON=ON -DBUILD_VISUALIZATION=ON -DENABLE_COVERAGE=ON
cd ..
make -j$(nproc)
```

### Building for Distribution

```bash
# Create distribution package
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPACKAGE_GENERATOR=TGZ
make package
```

# The distribution package will be created in the build directory

### Running Examples

#### C++ Examples

```bash
# Run main simulation
./build/nlm

# Run Phase 3 demo
./build/nlm_phase3_demo

# Run Phase 4 demo
./build/nlm_phase4_demo

# Run Phase 6 demo
./build/nlm_phase6_demo
```

#### Python Examples

```bash
# Run Python examples
python examples/python_example.py
python examples/agent_example.py
python examples/world_example.py
```

## Troubleshooting

### Common Issues

#### CMake cannot find Python

```bash
# Install Python development packages
# Ubuntu/Debian
sudo apt-get install python3-dev python3-pip

# macOS
brew install python3

# Then reconfigure
mkdir build
cd build
cmake .. -DPython_EXECUTABLE=$(which python3)
```

#### CMake cannot find pybind11

```bash
# Install pybind11 development package
# Ubuntu/Debian
sudo apt-get install pybind11-dev

# macOS
brew install pybind11

# Then reconfigure
mkdir build
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
```

#### Missing dependencies

```bash
# Install system dependencies
# Ubuntu/Debian
sudo apt-get install build-essential cmake g++ python3-dev python3-pip

# macOS
xcode-select --install
brew install cmake python3

# Reconfigure and rebuild
mkdir build
cd build
cmake ..
make -j$(nproc)
```

#### Compilation errors

```bash
# Try with simpler flags
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -Wno-error
make -j$(nproc)
```

#### Python bindings not working

```bash
# Check if pybind11 was found
mkdir build
cd build
cmake .. -DBUILD_PYTHON=ON
make -j$(nproc)

# Try to import
python -c "import sys; sys.path.insert(0, 'build'); import pynlm; print('Success')"
```

### Getting Help

#### CMake Output

```bash
# Get detailed CMake output
mkdir build
cd build
cmake .. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
```

#### Compiler Output

```bash
# Get full compiler output
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make VERBOSE=1
```

#### Environment Information

```bash
# Get system information
uname -a
python --version
python -c "import platform; print(platform.architecture())"

# Check for compiler support
${CXX} --version
${CXX} -std=c++20 -E - <<< "int main() { return 0; }" >/dev/null 2>&1 && echo "C++20 supported" || echo "C++20 not supported"
```

## Advanced Topics

### Cross-Compilation

To cross-compile for another platform:

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)

# For example, to cross-compile for ARM:
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/arm-linux-gnueabihf.toolchain.cmake
```

### Setting Compilation Flags

```bash
# Set specific compiler flags
cd build
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)
```

### Building with External Libraries

```bash
# Find and use external libraries
cd build
cmake .. -DUSE_EXTERNAL_JSON=ON -DEXTERNAL_JSON_DIR=/path/to/json/lib
cmake ..
make -j$(nproc)
```

## Migration from Previous Versions

### From CMake 3.15 to 3.16+

- No changes required
- CMake 3.16+ provides better dependency management

### From NLM 0.0.x to 0.1.0

- Updated project structure
- Improved build system
- Added Python bindings support
- Enhanced installation procedures

### Rebuilding for New Features

```bash
# Clean and rebuild with new features
cd build
cmake .. -DBUILD_PYTHON=ON -DBUILD_VISUALIZATION=ON
make clean
make -j$(nproc)
```

## Frequently Asked Questions

### Q: What Python versions are supported?
A: NLM supports Python 3.8 and above.

### Q: Can I disable Python bindings?
A: Yes, set `BUILD_PYTHON=OFF` during configuration.

### Q: How do I install Python bindings?
A: After building, run `pip install -e .` from the project root.

### Q: What are the default install locations?
A: On Linux, libraries go to `/usr/local/lib` and executables to `/usr/local/bin`.

### Q: How do I run tests?
A: Use `ctest --output-on-failure` from the build directory.

### Q: Can I build for multiple Python versions?
A: Yes, by setting `Python_ADDITIONAL_VERSIONS` in CMake.

## Support

For issues, questions, or feature requests:

1. **GitHub Issues**: Report bugs and issues at https://github.com/nlm-project/nlm/issues
2. **Discussions**: Ask questions and discuss features at https://github.com/nlm-project/nlm/discussions
3. **Documentation**: Check the latest documentation at https://nlm.readthedocs.io
4. **Examples**: View examples in the `examples/` directory

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Contributing

Contributions are welcome! Please see `CONTRIBUTING.md` for guidelines.

## Acknowledgments

- Special thanks to all contributors
- Thanks to the open-source community for their tools and libraries
- Thanks to the pybind11 team for excellent bindings
- Thanks to the CMake community for modern build practices

---

*Last updated: $(date +'%Y-%m-%d %H:%M:%S')*

This document is continuously updated. For the latest version, check the repository.  
