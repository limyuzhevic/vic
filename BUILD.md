# NLM (熙然) - Neural Learning Machine
# Build and Installation Guide

This document provides comprehensive instructions for building, installing, and developing with NLM. It covers multiple build options, installation methods, and development workflows.

## Quick Start

### For Users (Easiest Method)

```bash
# Build using the simple script
./scripts/build.sh

# Or using CMake directly
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install (optional)
make install
```

### For Developers (Full Development Setup)

```bash
# Clone the repository
cd /path/to
# Add git submodule initialization if needed
git submodule update --init --recursive

# Create build directory
mkdir build
cd build

# Configure with development options
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_PYTHON_BINDINGS=ON \
  -DBUILD_TESTS=ON \
  -DBUILD_DOCS=ON \
  -DBUILD_EXAMPLES=ON

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

## Build Options

### Build Types

| Type | Description | Best For |
|------|-------------|----------|
| `Debug` | Debug builds with symbols and assertions | Development, debugging |
| `Release` | Optimized builds without debug info | Production, performance |
| `RelWithDebInfo` | Release with debug symbols | Release with debugging |

### CMake Options

```bash
cmake .. [OPTIONS]

Common Options:
  -DCMAKE_BUILD_TYPE=Debug|Release|RelWithDebInfo
  -DENABLE_PYTHON_BINDINGS=ON|OFF      Enable Python bindings (default: ON)
  -DBUILD_TESTS=ON|OFF                  Build test suite (default: ON)
  -DBUILD_DOCS=ON|OFF                    Build documentation (default: OFF)
  -DBUILD_EXAMPLES=ON|OFF                Build example executables (default: ON)
  -DCMAKE_INSTALL_PREFIX=/path/to/prefix  Installation directory
  -DPYTHON_EXECUTABLE=/path/to/python    Python interpreter for bindings
  -Dpybind11_DIR=/path/to/pybind11       pybind11 installation path
  -DEXTRA_WARNINGS=ON|OFF                Enable extra compiler warnings (default: ON)
  -DSANITIZE=OFF                          Enable address sanitizer (debug builds only)
  -DCOVERAGE=OFF                          Enable code coverage (debug builds only)
```

### Build Targets

After running CMake, you can build specific targets:

```bash
# Build all targets (default)
make

# Build core libraries only
make nlm_core nlm_agent nlm_world

# Build main executables
make nlm nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo

# Build Python bindings
make pynlm

# Build test suite
make nlm_test

# Install all targets
make install

# Install specific targets
make install-targets
```

## Installation

### System Installation

```bash
# From build directory
make install

# Typical installation prefix
# On Linux: /usr/local
# On macOS: /usr/local
# On Windows: C:\Program Files\NLM

# Verify installation
which nlm  # Should show path to nlm executable
find /usr/local -name "*nlm*" -type f 2>/dev/null
```

### Python Package Installation

#### From Source (Recommended for Development)

```bash
# Install Python development dependencies
pip install pybind11 scikit-build-core pytest numpy cmake

# Build and install Python package
pip install .

# Or install in development mode (for editing)
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

#### Using Pre-built Wheels

```bash
# Check for pre-built wheels
pip search nlm  # Or browse PyPI

# Install from wheel
pip install nlm-0.1.0-py3-none-any.whl
```

### Environment Setup

#### Shell Configuration

Add NLM to your shell configuration file:

```bash
# Add to ~/.bashrc or ~/.zshrc
cat >> ~/.bashrc << EOF
# NLM Neural Learning Machine
export NLM_ROOT=/path/to/nlm/build
export PATH=$PATH:$NLM_ROOT/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NLM_ROOT/lib
EOF

# Source the configuration
source ~/.bashrc

# Verify
which nlm
ldd $(which nlm)  # Should show NLM library dependencies
```

#### Python Environment

```bash
# Create and activate virtual environment
python -m venv nlm_env
source nlm_env/bin/activate

# Install NLM and dependencies
pip install -e .

# Verify installation
python -c "
import pynlm
print('NLM version:', pynlm.__version__)

# Quick test
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print('Brain created successfully with', brain.getTotalNeuronCount(), 'neurons')
"
```

## Development Setup

### Prerequisites

```bash
# Install system dependencies
# Ubuntu/Debian
 sudo apt update
sudo apt install build-essential cmake git python3-dev python3-pip libboost-all-dev

# macOS
brew update
brew install cmake python3

# Windows (using WSL or Native Windows)
# Install from https://www.cmake.org/download/
# Install Python from https://www.python.org/downloads/
```

### Git Configuration

```bash
# Initialize and update submodules
cd /path/to/nlm
git submodule update --init --recursive

# Create development branch if needed
git checkout -b dev

# Set up git user (if not already configured)
git config user.name "Your Name"
git config user.email "your.email@example.com"
```

### IDE Integration

#### Visual Studio Code

Create `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build NLM",
            "type": "shell",
            "command": "mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j$(nproc)",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": "$cmake",
            "options": {
                "cwd": "${workspaceFolder}"
            }
        },
        {
            "label": "Run Tests",
            "type": "shell",
            "command": "cd build && ctest --output-on-failure",
            "group": "test",
            "problemMatcher": "$ctest",
            "options": {
                "cwd": "${workspaceFolder}"
            }
        }
    ]
}
```

#### CLion (JetBrains IDE)

1. File → Open → `build/` directory
2. Import CMake project
3. Set CMake build type to Debug
4. Create run/debug configurations for executables

### Code Quality Tools

#### Clang-Format

```bash
# Install clang-format
# Ubuntu/Debian
sudo apt install clang-format

# macOS
brew install clang-format

# Format all C++ files
find src -name "*.cpp" -name "*.h" -exec clang-format -i {} \;

# Check formatting
find src -name "*.cpp" -name "*.h" -exec clang-format --dry-run {} \;
```

#### Cppcheck

```bash
# Install cppcheck
# Ubuntu/Debian
sudo apt install cppcheck

# Run static analysis
cppcheck --enable=all --std=c++20 src/
```

#### Doxygen Documentation

Generate API documentation:

```bash
cmake .. -DBUILD_DOCS=ON
make doc

# Open documentation
open build/docs/html/index.html
```

## Advanced Build Options

### Custom Compiler and Flags

```bash
# Use specific compiler
cd build
cmake .. \
  -DCMAKE_CXX_COMPILER=g++ \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp"

# Additional linker flags
make LDFLAGS="-static-libstdc++"
```

### External Dependencies

#### pybind11

```bash
# Using system pybind11
cmake .. -Dpybind11_DIR=/usr/local/include/pybind11

# Using git submodule
cmake .. -DPYBIND11_SOURCE_DIR=../pybind11

# Using custom installation
cmake .. -Dpybind11_DIR=/opt/pybind11/cmake
```

#### Boost

```bash
# Check Boost availability
cmake .. -DBOOST_ROOT=/usr/local/boost

# Use system Boost
cmake .. -DBOOST_USE_STATIC_LIBS=ON -DBOOST_USE_SHARED_LIBS=OFF
```

### Cross-Compilation

```bash
# Cross-compile for ARM
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/arm-linux-gnueabihf.cmake \
  -DCMAKE_BUILD_TYPE=Release

# Cross-compile for Windows
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/windows-toolchain.cmake \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
```

## Build Troubleshooting

### Common Issues and Solutions

#### Issue: CMake cannot find Python

**Solution:**

```bash
pip install pybind11 scikit-build-core cmake
cmake .. -DPython_EXECUTABLE=$(which python)
```

#### Issue: pybind11 not found

**Solution:**

```bash
# Install pybind11
pip install pybind11

# Or use CMake's FetchContent
cmake .. -Dpybind11_FETCHCONTENT=ON

# Or specify path
cmake .. -Dpybind11_DIR=/path/to/pybind11/cmake
```

#### Issue: Compiler too old

**Solution:**

```bash
# Update compiler
cd build
cmake .. \
  -DCMAKE_CXX_STANDARD=20 \
  -DCMAKE_CXX_STANDARD_REQUIRED=ON \
  -DCMAKE_CXX_EXTENSIONS=OFF

# Or use modern compiler
export CC=gcc-11
export CXX=g++-11
cmake .. $OTHER_OPTIONS
```

#### Issue: Memory allocation failed

**Solution:**

```bash
# Reduce memory usage
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-O2 -m64"

# Use smaller configurations
cmake .. -DBRAIN_NEURON_COUNT=100 -DBRAIN_CONNECTION_PROBABILITY=0.05
```

#### Issue: Cannot find Boost libraries

**Solution:**

```bash
# Install Boost
# Ubuntu/Debian
sudo apt install libboost-all-dev

# macOS
brew install boost

# Set Boost path
cmake .. -DBOOST_ROOT=/usr/local/boost
```

### Build Logs

For detailed build logs, use:

```bash
cd build
make VERBOSE=1  # Show all commands

# Or redirect output to files
make > build.log 2>&1

# Monitor build progress
make -j$(nproc) | tee build_log.txt
```

## Post-Build Verification

### Verify Installation

```bash
# Check executables
ls -la build/bin/

# Check libraries
find build/lib -name "*.so" -o -name "*.dylib" -o -name "*.dll" | head -10

# Check Python bindings
python -c "import pynlm; print(pynlm.__file__)"

# Run basic tests
cd build
./nlm --help 2>/dev/null || ./nlm -h
./nlm_phase6_demo --help 2>/dev/null || ./nlm_phase6_demo -h

# Run unit tests
./nlm_test
```

### Performance Testing

```bash
# Run performance benchmarks
cd build
./benchmarks/brain_benchmark

# Memory usage check
valgrind --leak-check=full ./nlm_phase6_demo

# CPU profiling
perf record -g ./nlm_phase6_demo
perf report
```

## Development Workflow

### Typical Development Cycle

1. **Initialize**: `git checkout main && git submodule update --init --recursive`
2. **Build**: `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j$(nproc)`
3. **Test**: `ctest --output-on-failure`
4. **Develop**: Edit source files and rebuild
5. **Test Again**: Run specific tests or examples
6. **Commit**: `git add . && git commit -m "description"`
7. **Push**: `git push origin branch-name`

### Example Development Commands

```bash
# Start with fresh build
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Test after build
ctest --output-on-failure

# Make changes
nano src/brain/Neuron.cpp

# Rebuild and test
make -j$(nproc)
./nlm_phase6_demo

# If changes break tests
ctest --output-on-failure

# Commit changes
git add src/brain/Neuron.cpp build/
git commit -m "Fix neuron initialization"
git push origin dev
```

## Additional Resources

### Documentation

- **ARCHITECTURE.md**: Detailed system architecture
- **SCIENCE.md**: Scientific background and principles
- **ROADMAP.md**: Development roadmap and future plans
- **EXPERIMENTS.md**: Experiment descriptions and results
- **PHASE6_FINAL_AUDIT.md**: Phase 6 integration audit report

### Examples and Demos

- **Phase 2 Demo**: Basic neural computation demonstrations
- **Phase 3 Demo**: Environment interaction examples
- **Phase 4 Demo**: Cognitive function demonstrations
- **Phase 6 Demo**: Complete integration tests

### Community Resources

- **GitHub Issues**: Report bugs and request features
- **Documentation**: Comprehensive API and usage documentation
- **Examples**: Working code examples and tutorials
- **Support**: Community forums and technical support

## License

This software is licensed under the MIT License. See LICENSE file for details.

---

*For additional help and support, visit the NLM documentation or contact the development team.*

Generated using comprehensive build and installation guidelines for modern C++ projects.