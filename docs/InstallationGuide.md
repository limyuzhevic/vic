# NLM Installation Guide

This guide provides comprehensive instructions for installing and using the NLM (Neural Learning Machine) project on various platforms.

## Quick Start

### Prerequisites

- CMake 3.20 or higher
- C++ compiler (g++ 7+, clang 5+, MSVC 2017+)
- Python 3.6+ (optional, for Python bindings)

### Linux/macOS

```bash
# Clone the repository
git clone https://github.com/yourusername/nlm.git
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DNLM_BUILD_TESTS=ON

# Build
make -j$(nproc)

# Run tests (optional)
ctest --output-on-failure

# Install to system location (requires sudo)
sudo make install
```

### Windows

```powershell
# Using Windows Command Prompt
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release

# Run tests
ctest -C Release

# Install
cmake --install . --config Release --prefix "C:\Program Files\NLM"
```

## Detailed Installation

### 1. Dependency Installation

#### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install \
    cmake \
    build-essential \
    libpython3-dev \
    python3-pip \
    python3-dev \
    doxygen \
    graphviz
```

#### Linux (CentOS/RHEL)

```bash
sudo yum install \
    cmake3 \
    gcc-c++ \
    python3-devel \
    python3-pip \
    doxygen \
    graphviz
```

#### macOS

```bash
# Install via Homebrew
brew install cmake \
    python3 \
    doxygen \
    graphviz
```

### 2. Configure Build

Create a build directory and configure CMake:

```bash
mkdir build
cd build
cmake .. [OPTIONS]
```

#### Common Options

| Option | Description | Example |
|--------|-------------|---------|
| `-DCMAKE_BUILD_TYPE` | Build type | `-DCMAKE_BUILD_TYPE=Release` |
| `-DNLM_BUILD_TESTS` | Build tests | `-DNLM_BUILD_TESTS=ON` |
| `-DNLM_BUILD_PYTHON` | Build Python bindings | `-DNLM_BUILD_PYTHON=ON` |
| `-DNLM_ENABLE_SANITIZERS` | Enable sanitizers | `-DNLM_ENABLE_SANITIZERS=ON` |
| `-DCMAKE_INSTALL_PREFIX` | Installation directory | `-DCMAKE_INSTALL_PREFIX=/opt/nlm` |
| `-G` | Generator | `-G "Unix Makefiles"` |

#### Example Configurations

**Development Build**

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_BUILD_TESTS=ON \
    -DNLM_ENABLE_SANITIZERS=ON \
    -DNLM_ENABLE_PROFILING=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local
```

**Production Build**

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=ON \
    -DNLM_BUILD_TESTS=ON \
    -DNLM_ENABLE_CCACHE=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local
```

**Python Development**

```bash
cmake .. \
    -DNLM_BUILD_PYTHON=ON \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DNLM_ENABLE_PROFILING=ON
```

### 3. Building

#### Standard Build

```bash
cmake --build .
```

#### Parallel Build (recommended)

```bash
cmake --build . -- -j$(nproc)
```

#### Build Specific Targets

```bash
# Build specific targets
cmake --build . --target nlm_core
cmake --build . --target nlm_agent
cmake --build . --target nlm_world

# Build and install specific targets
cmake --build . --target nlm install
```

#### Cross-Compilation

```bash
# Example: ARM cross-compilation
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake
cmake --build . --target all
```

### 4. Testing

#### Run Tests

```bash
# Run all tests
ctest

# Run tests with verbose output
ctest -V

# Run tests with output on failure
ctest --output-on-failure

# Run specific test types
ctest -R "test_type"
```

#### Test Categories

| Test Type | Description |
|-----------|-------------|
| Unit Tests | Individual component tests |
| Integration Tests | Component interaction tests |
| Performance Tests | Performance benchmarks |
| Python Tests | Python bindings tests |

### 5. Installation

#### Standard Installation

```bash
cmake --install . --prefix /usr/local
```

#### Custom Installation Prefix

```bash
cmake --install . --prefix /opt/nlm
cmake --install . --prefix ~/local
```

#### Installation Verification

After installation, verify the installation:

```bash
# Check installed files
ls -la /usr/local/lib/libnlm*
ls -la /usr/local/include/nlm/
ls -la /usr/local/bin/nlm*

# Test installed executable
/usr/local/bin/nlm --help
```

#### Python Installation Verification

```bash
# Check Python bindings
python3 -c "import pynlm; print(pynlm.__version__)"

# Test Python bindings
python3 -c "from pynlm import createBrain; print('Success')"
```

### 6. Python Bindings

#### Building Python Bindings

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON=ON
cmake --build .
```

#### Installing Python Bindings

```bash
# Install Python module globally
cmake --install . --prefix /usr/local

# Or install to user site
python3 -m pip install --user ./python/build/pynlm*.whl
```

#### Using Python Bindings

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.step()

# Save brain state
brain.save("brain_state.json")
```

### 7. Package Management

#### Creating Distribution Packages

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_TESTS=ON
cmake --build .
cpack
```

#### Package Types

| Package Type | Platform | Format |
|--------------|----------|--------|
| DEB | Debian/Ubuntu | .deb |
| RPM | RedHat/CentOS | .rpm |
| TGZ | Unix-like | .tar.gz |
| DMG | macOS | .dmg |
| MSI | Windows | .msi |

#### Package Installation

**Linux Packages**

```bash
# Ubuntu/Debian
sudo dpkg -i nlm_*.deb

# RedHat/CentOS
sudo rpm -i nlm-*.rpm
```

**macOS Packages**

```bash
# .dmg
hdiutil attach nlm-*.dmg
cp -r nlm /Applications/
diskutil eject /Volumes/nlm
```

**Windows Packages**

```powershell
# .msi
msiexec /i nlm-*.msi
```

### 8. Configuration

#### Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `NLM_CONFIG_FILE` | Path to configuration file | `configs/default.cfg` |
| `NLM_LOG_LEVEL` | Logging level | `INFO` |
| `CCACHE_DIR` | CCache cache directory | System default |
| `ASAN_OPTIONS` | ASan options | System default |

#### Configuration File Format

NLM uses JSON configuration files:

```json
{
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

#### Configuration Loading

NLM supports multiple configuration sources:

1. **Default Configuration**: Built-in defaults
2. **Configuration File**: `configs/default.cfg`
3. **Command Line Arguments**: `--config file.json`
4. **Programmatic**: API calls

### 9. Platform-Specific Installation

#### Windows-Specific Considerations

```bash
# Windows paths use backslashes
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
cmake --install . --config Release --prefix "C:\Program Files\NLM"
```

#### macOS-Specific Considerations

```bash
# System paths
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --install . --prefix=/usr/local

# User paths
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --install . --prefix=$HOME/.local

# RPATH configuration
cmake .. -DCMAKE_INSTALL_RPATH="@executable_path/../lib"
```

#### Linux-Specific Considerations

```bash
# System paths (recommended for system-wide installation)
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --install . --prefix=/usr/local

# User paths
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --install . --prefix=$HOME/.local

# RPATH configuration
cmake .. -DCMAKE_INSTALL_RPATH="$ORIGIN/../lib"
```

### 10. Troubleshooting

#### Common Issues and Solutions

**Issue**: CMake not found
```bash
# Install CMake
sudo apt-get install cmake  # Ubuntu/Debian
brew install cmake         # macOS
# Download from cmake.org for Windows
```

**Issue**: Compiler not found
```bash
# Install compiler
sudo apt-get install g++     # Ubuntu/Debian
brew install gcc             # macOS
# Install Visual Studio for Windows
```

**Issue**: Python dependencies not found
```bash
# Install Python and pip
python3 -m pip install --upgrade pip
python3 -m pip install pybind11 numpy
```

**Issue**: Memory issues during build
```bash
# Increase swap space
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Issue**: Permission denied during installation
```bash
sudo cmake --install . --prefix /usr/local
```

**Issue**: Library path issues
```bash
# Set RPATH
cmake .. -DCMAKE_INSTALL_RPATH="$ORIGIN/../lib"
cmake --install . --prefix /usr/local
```

#### Build Log Debugging

```bash
# Verbose build
cmake .. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
cmake --build . VERBOSE=1

# Set compiler flags
cmake .. -DCMAKE_CXX_FLAGS="-O2 -g"
```

### 11. Uninstallation

#### Standard Uninstallation

```bash
# Uninstall from system location
sudo rm -rf /usr/local/lib/libnlm*
sudo rm -rf /usr/local/include/nlm
sudo rm -rf /usr/local/bin/nlm*
```

#### Remove Package Manager Packages

**Ubuntu/Debian**
```bash
sudo dpkg -r --force-depends nlm
```

**CentOS/RHEL**
```bash
sudo rpm -e --nosignatures nlm
```

**macOS**
```bash
rm -rf /Applications/nlm
```

**Windows**
```powershell
# Remove from Programs and Features
start "Programs and Features" appwiz.cpl
```

### 12. Verification Checklist

After installation, verify:

- [ ] `nlm` executable exists
- [ ] `nlm_phase3_demo` executable exists (if experiments enabled)
- [ ] `nlm_phase4_demo` executable exists (if experiments enabled)
- [ ] Python module `pynlm` can be imported
- [ ] Header files are accessible
- [ ] Library files are correctly linked
- [ ] Configuration files are present
- [ ] Documentation is available

### 13. Support and Resources

#### Online Resources

- **Documentation**: `docs/CMakeUsage.md`, `docs/DevelopmentWorkflow.md`
- **Community**: GitHub Discussions
- **Issues**: GitHub Issues
- **Examples**: `examples/` directory

#### Local Resources

```bash
# Build scripts
./build.sh          # Build and test script
./benchmark.sh      # Performance benchmark script

# Documentation
open docs/CMakeUsage.md
open docs/DevelopmentWorkflow.md

# Examples
./examples/run_examples.sh
```

#### Getting Help

```bash
# Build help
./build.sh help

# CMake help
cmake -P help.cmake

# Project documentation
make docs    # If documentation target exists
```

## Conclusion

This installation guide provides comprehensive instructions for installing and using the NLM project. The modern CMake build system ensures:

- **Easy Installation**: Simple `cmake .. && make install` workflow
- **Cross-Platform Support**: Works on Linux, macOS, and Windows
- **Flexible Configuration**: Extensive build options for different use cases
- **Package Management**: Full CPack integration for distribution
- **Python Integration**: Seamless Python bindings support
- **Testing**: Comprehensive test suite for verification

The NLM project is ready for production deployment with modern development tools and practices.
