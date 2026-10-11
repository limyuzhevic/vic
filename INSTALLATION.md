# NLM Installation Guide

This document provides comprehensive instructions for installing the Neural Learning Machine (NLM) framework using the unified build system.

## Installation Overview

NLM supports multiple installation modes and package managers. Choose the installation method that best suits your needs and environment.

## Quick Installation

### Minimal Installation (Core Only)

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -DNLM_BUILD_MINIMAL=ON -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Install to system (requires sudo)
sudo cmake --install .
```

### Full Installation with Python Bindings

```bash
# Create build directory
mkdir build
cd build

# Configure with all components
cmake .. \
  -DNLM_BUILD_TESTS=ON \
  -DNLM_BUILD_PYTHON_BINDINGS=ON \
  -DNLM_BUILD_CLI=ON \
  -DNLM_BUILD_DEMOS=ON \
  -DNLM_BUILD_FULL=ON

# Build
cmake --build . -- -j$(nproc)

# Install
sudo cmake --install .
```

## Installation Methods

### 1. Standard CMake Installation

#### Step 1: Clone Repository

```bash
git clone https://github.com/nlm/nlm.git
cd nlm
```

#### Step 2: Create Build Directory

```bash
mkdir build
cd build
```

#### Step 3: Configure CMake

Choose one of the following configurations:

##### Debug Build (Development)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DNLM_BUILD_TESTS=ON \
  -DNLM_BUILD_PYTHON_BINDINGS=ON \
  -DNLM_BUILD_CLI=ON \
  -DNLM_BUILD_DEMOS=ON \
  -DNLM_BUILD_DEV=ON
```

##### Release Build (Production)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DNLM_BUILD_TESTS=ON \
  -DNLM_BUILD_PYTHON_BINDINGS=ON \
  -DNLM_BUILD_CLI=ON \
  -DNLM_BUILD_DEMOS=ON \
  -DNLM_BUILD_FULL=ON
```

##### Minimal Build (Core Only)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DNLM_BUILD_MINIMAL=ON
```

##### Package Manager Build

###### With Conan

```bash
# Install Conan if not present
curl -L https://raw.githubusercontent.com/conan-io/conan/master/conanfile.py -o conanfile.py
python3 conanfile.py install

# Configure with Conan
cmake .. \
  -DNLM_ENABLE_CONAN=ON \
  -DNLM_BUILD_TESTS=ON \
  -DNLM_BUILD_PYTHON_BINDINGS=ON
```

###### With vcpkg

```bash
# Install vcpkg if not present
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install nlohmann-json yaml-cpp pybind11

# Configure with vcpkg
set(VCPKG_ROOT=$(pwd)/vcpkg)
mkdir build
cd build
cmake .. -G "Ninja" -A x64 -DVCPKG_ROOT=%VCPKG_ROOT%
```

#### Step 4: Build Project

```bash
# With Unix Makefiles
cmake --build . -- -j$(nproc)

# With Ninja (preferred)
cmake --build . -j$(nproc)
```

#### Step 5: Run Tests (Optional)

```bash
ctest --output-on-failure
```

#### Step 6: Install Project

```bash
# System-wide installation (requires sudo)
sudo cmake --install .

# User installation (no sudo required)
cmake --install . --prefix ~/.local
```

### 2. Python Build Script Installation

The NLM build system includes a Python automation script for easier installation.

#### Step 1: Check Python Version

```bash
python3 --version
# Should be 3.6 or higher
```

#### Step 2: Run Build Script

```bash
# Install all components
python3 build.py --components all --install --prefix /usr/local

# Install only core components
python3 build.py --components core --install --prefix /usr/local

# Install with Conan package manager
python3 build.py --components all --conan --install

# Install with vcpkg package manager
python3 build.py --components all --vcpkg --install

# Custom build directory
python3 build.py --components all --install --prefix ~/.local --build-dir custom_build
```

#### Step 3: Verify Installation

```bash
# Check if executables are available
which nlm
which nlm_phase3_demo
which nlm_phase4_demo

# Check if libraries are available
ldconfig -p | grep nlm
```

### 3. Shell Script Installation

For users who prefer shell scripts, we provide an installation script.

#### Step 1: Make Script Executable

```bash
chmod +x install.sh
```

#### Step 2: Run Installation Script

```bash
# Install all components
sudo ./install.sh --components all

# Install only core components
./install.sh --components core --prefix ~/.local

# Custom build type
./install.sh --build-type Debug --dev
```

## Component Installation

### Core Libraries

The core libraries include:

- `nlm_core`: Main neural network core
- `nlm_agent`: Agent-brai n interface
- `nlm_world`: Environment simulation
- `nlm_phase3`: Phase 3 experiments
- `nlm_phase4`: Phase 4 experiments

**Installation location:** `${prefix}/lib/`

### Executables

The executables include:

- `nlm`: Main NLM executable
- `nlm_phase3_demo`: Phase 3 demo
- `nlm_phase4_demo`: Phase 4 demo
- `nlm_advanced_cli`: Advanced CLI and plugin system

**Installation location:** `${prefix}/bin/`

### Python Bindings

Python bindings provide Python interface to NLM:

- `pynlm`: Python module

**Installation location:** `${prefix}/lib/pythonX.Y/site-packages/`

### Tests

Test executables are installed for verification:

- `nlm_test`: Unit tests

**Installation location:** `${prefix}/bin/`

### Documentation

Documentation includes:

- API documentation
- User guides
- Examples
- Tutorials

**Installation location:** `${prefix}/share/doc/nlm/`

## Configuration After Installation

### Add NLM to PATH

```bash
# Add NLM binaries to PATH
cat >> ~/.bashrc << EOF
export PATH="$PATH:/usr/local/bin"
EOF
source ~/.bashrc
```

### Add NLM to LD_LIBRARY_PATH

```bash
# Add NLM libraries to library path
cat >> ~/.bashrc << EOF
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
EOF
source ~/.bashrc
```

### Add NLM to CPLUS_INCLUDE_PATH

```bash
# Add NLM headers to include path
cat >> ~/.bashrc << EOF
export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/usr/local/include"
EOF
source ~/.bashrc
```

### Python Package Installation

If you installed Python bindings, you can test them:

```bash
# Test Python bindings
python3 -c "import pynlm; print(pynlm.__version__)"

# Or install as pip package
pip3 install ~/.local/lib/python3.10/site-packages/pynlm*
```

## Verification

### Check Installation

Run the following commands to verify your installation:

```bash
# Check if NLM is installed
nlm --help

# Run a simple test
nlm_test

# Test Python bindings
python3 -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('Success!')"
```

### Run Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test types
ctest -R "test_plugin"
ctest -R "test_advanced_cli"
```

## Uninstallation

### Uninstall NLM

```bash
# Remove files installed by CMake
sudo cmake --uninstall .

# Or remove manually
 sudo rm -rf /usr/local/bin/nlm
 sudo rm -rf /usr/local/lib/*nlm*
 sudo rm -rf /usr/local/include/nlm
 sudo rm -rf /usr/local/share/doc/nlm
```

### Remove Configuration Files

```bash
# Remove build directory
rm -rf build

# Remove package manager cache
rm -rf ~/.conan2
rm -rf ~/.vcpkg
```

## Platform-Specific Installation

### Linux

#### Ubuntu/Debian

```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install cmake ninja-build git python3-dev

# Install NLM
python3 build.py --components all --install
```

#### Fedora/RHEL/CentOS

```bash
# Install system dependencies
sudo dnf install cmake make gcc-c++ git python3-devel

# Install NLM
python3 build.py --components all --install
```

### macOS

#### Using Homebrew

```bash
# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake ninja python3

# Install NLM
python3 build.py --components all --install
```

#### Using Conda

```bash
# Create conda environment
conda create -n nlm python=3.8 cmake ninja
conda activate nlm

# Install NLM
python3 build.py --components all --install
```

### Windows

#### Using Visual Studio

```bash
# Install Visual Studio with C++ workload
# Open Developer Command Prompt

cd NLM
s mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
cmake --install . --prefix "C:\Program Files\NLM"
```

#### Using Windows Subsystem for Linux (WSL)

```bash
# Install WSL if not present
wsl --install

# Use the Linux installation methods above
```

## Troubleshooting

### Installation Issues

#### "Permission Denied"

**Solution:** Use sudo for system-wide installation or specify a user directory:

```bash
# User installation
python3 build.py --install --prefix ~/.local

# System installation
sudo python3 build.py --install --prefix /usr/local
```

#### "CMake not found"

**Solution:** Install CMake:

```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
# Download from https://cmake.org/download/
```

#### "Compiler not supported"

**Solution:** Install a C++20 compatible compiler:

```bash
# Ubuntu/Debian
sudo apt-get install g++

# macOS
brew install gcc

# Windows
# Use Visual Studio 2019 or later
```

#### "Python bindings not found"

**Solution:** Ensure Python development packages are installed:

```bash
# Ubuntu/Debian
sudo apt-get install python3-dev python3-pip

# macOS
brew install python3

# Windows
# Install Python from https://www.python.org/downloads/
```

#### "Package manager not found"

**Solution:** Install package manager:

```bash
# Conan
curl -L https://raw.githubusercontent.com/conan-io/conan/master/conanfile.py -o conanfile.py
python3 conanfile.py install

# vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
```

### Build Issues

#### "Out of memory"

**Solution:** Build with fewer parallel jobs:

```bash
cmake --build . -- -j2
```

#### "Disk space full"

**Solution:** Clean build directory and temporary files:

```bash
# Clean build directory
rm -rf build

# Clean cache files
rm -rf ~/.cache/cmake
rm -rf ~/.conan2
```

#### "Header not found"

**Solution:** Check include paths and installation prefix:

```bash
# Check include directory
find /usr/local/include -name "nlm" -type d

# Reinstall with correct prefix
python3 build.py --install --prefix /usr/local
```

## Advanced Installation

### Development Installation

For development, use the development build mode:

```bash
python3 build.py --components dev --install --prefix ~/.local
```

### Packaging Installation

To create distribution packages:

```bash
# Create DEB package (Ubuntu/Debian)
python3 build.py --components all --cpack --install

# Create RPM package (Fedora/RHEL/CentOS)
python3 build.py --components all --cpack --install

# Create TGZ archive
python3 build.py --components all --cpack --install
```

### Cross-Compilation

For cross-compilation, set the appropriate toolchain:

```bash
# Example: Cross-compile for ARM
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake -DCMAKE_BUILD_TYPE=Release
```

## Performance Optimization

### Build Caching

Use build caching to speed up repeated builds:

```bash
# First build
python3 build.py --components all

# Subsequent builds (incremental)
python3 build.py --components all
```

### Parallel Build

Use parallel build to speed up compilation:

```bash
# With multiple cores
cmake --build . -- -j$(nproc)

# With specific number of cores
cmake --build . -- -j8
```

### Dependency Management

Use package managers to speed up dependency installation:

```bash
# With Conan
python3 build.py --components all --conan --install

# With vcpkg
python3 build.py --components all --vcpkg --install
```

## Cleanup

### Remove Build Directory

```bash
rm -rf build
```

### Remove Installation

```bash
# System-wide installation (requires sudo)
sudo rm -rf /usr/local/bin/nlm
 sudo rm -rf /usr/local/lib/*nlm*
 sudo rm -rf /usr/local/include/nlm
 sudo rm -rf /usr/local/share/doc/nlm

# User installation
 rm -rf ~/.local/bin/nlm
 rm -rf ~/.local/lib/*nlm*
 rm -rf ~/.local/include/nlm
 rm -rf ~/.local/share/doc/nlm
```

### Remove Package Manager Cache

```bash
# Conan
rm -rf ~/.conan2

# vcpkg
rm -rf ~/.vcpkg
```

## Support

For installation issues, refer to:

1. **Documentation:** README-build.md, INSTALLATION.md, BUILD.md
2. **GitHub Issues:** https://github.com/nlm/nlm/issues
3. **Community Forum:** https://community.nlm.org
4. **Discord:** https://discord.nlm.org

## Conclusion

The NLM installation system provides flexible options for installing the Neural Learning Machine framework. Whether you need a minimal core installation or a full development environment with all features, the unified build system makes it easy to get NLM up and running quickly.

For the most up-to-date information, refer to the documentation files and GitHub repository.

---

*Last updated: $(date -u +%Y-%m-%d)*
