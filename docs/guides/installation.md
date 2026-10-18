# NLM Installation and Setup Guide

## Overview

This guide covers the complete installation, setup, and environment configuration for the NLM (Neural Learning Machine) project. It includes instructions for building from source, installing Python bindings, and configuring the development environment.

## System Requirements

### Hardware Requirements

| Component | Minimum | Recommended | Notes |
|-----------|---------|-------------|-------|
| CPU | 2 cores | 8+ cores | SIMD optimizations available |
| RAM | 4 GB | 16+ GB | Larger for complex simulations |
| Storage | 1 GB | 10+ GB | For build artifacts and models |
| GPU | Optional | Optional | Not required for basic operation |

### Software Requirements

#### Operating Systems

- **Linux**: Ubuntu 20.04+, CentOS 8+, Fedora 38+
- **macOS**: macOS 12+, Xcode command line tools
- **Windows**: Windows 10+, WSL2 or native Windows Subsystem for Linux

#### Development Tools

- **C++ Compiler**: GCC 10+, Clang 12+, MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: Make, Ninja, or Meson
- **Git**: Version 2.25+ for cloning repositories

#### Python Dependencies

```python
# For Python bindings development
# Install before building from source
pip install pybind11 scikit-build-core pytest numpy
```

#### Optional Dependencies

- **Python IDE**: VS Code, PyCharm, or CLion
- **Debugger**: GDB, LLDB, or Visual Studio Code debugger
- **Profiler**: perf, valgrind, or Intel VTune
- **Documentation Tools**: Sphinx, Doxygen, or MkDocs

## Installation Methods

### Method 1: Building from Source (Recommended)

This method provides the most control and is recommended for development.

#### Step 1: Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/your-org/nlm.git
ncd nlm

# Verify the clone
ls -la
git status
```

#### Step 2: Verify Dependencies

```bash
# Check compiler version
$(CXX --version || echo "GCC not found")
$(CXX --version)

# Check CMake version
cmake --version

# Check Python version
python3 --version
python3 -m pip --version

# Check build tools
which make
which ninja
```

#### Step 3: Create Build Directory

```bash
# Create and navigate to build directory
mkdir -p build
cd build

# Alternative: Use out-of-tree builds
mkdir -p build/debug
cd build/debug
```

#### Step 4: Configure Build

```bash
# Basic configuration
cmake .. -DCMAKE_BUILD_TYPE=Release

# Advanced configuration options
# cmake .. -DCMAKE_BUILD_TYPE=Debug \
#           -DCMAKE_CXX_COMPILER=g++ \
#           -DCMAKE_C_COMPILER=gcc \
#           -DPYTHON_EXECUTABLE=$(which python3) \
#           -DENABLE_TESTS=ON \
#           -DENABLE_PYTHON_BINDINGS=ON \
#           -DENABLE_VISUALIZATION=OFF

# Configuration options:
#   CMAKE_BUILD_TYPE: Release, Debug, RelWithDebInfo, MinSizeRel
#   ENABLE_TESTS: Build test suite (ON/OFF)
#   ENABLE_PYTHON_BINDINGS: Build Python bindings (ON/OFF)
#   ENABLE_VISUALIZATION: Build visualization tools (ON/OFF)
#   PYTHON_EXECUTABLE: Path to Python interpreter
#   INSTALL_PREFIX: Installation directory (default: /usr/local)
```

#### Step 5: Build the Project

```bash
# Build with all targets (parallel)
make -j$(nproc)

# Or use CMake's build tool
cmake --build . --config Release --parallel $(nproc)

# Build specific targets
make nlm              # Main simulation executable
make nlm_test         # Test suite
make nlm_core         # Core library
make nlm_agent        # Agent library
make nlm_world        # World library
```

#### Step 6: Verify Build

```bash
# Check build outputs
ls -la

# Verify executables work
./nlm --help          # Show help for main executable
./nlm_test --help     # Show help for test suite

# Run basic functionality test
./nlm --version

# Run tests if built
ctest --output-on-failure
```

### Method 2: Using Package Manager

#### Debian/Ubuntu

```bash
# Install system dependencies
apt-get update
apt-get install -y build-essential cmake git python3 python3-pip python3-dev

# Clone and build
cd /tmp
git clone https://github.com/your-org/nlm.git
mkdir -p nlm/build
cd nlm/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### macOS with Homebrew

```bash
# Install dependencies
brew install cmake git python3

# Clone and build
cd /tmp
git clone https://github.com/your-org/nlm.git
cd nlm
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

### Method 3: Using Docker (Development)

```bash
# Build Docker image
docker build -t nlm-dev .

# Run interactive development container
docker run -it --rm \
    -v $(pwd)/nlm:/workspace/nlm \
    -w /workspace/nlm \
    nlm-dev /bin/bash
```

## Python Installation

### Pre-built Python Package

If you want to use the Python API without building from source:

```bash
# Install via pip (if available)
pip install pynlm

# Or install from pre-built wheels
pip install pynlm-binary
```

### Building Python Bindings from Source

#### Step 1: Install Python Build Dependencies

```bash
# Install required Python packages
pip install pybind11 scikit-build-core pytest numpy
pip install build
```

#### Step 2: Build and Install Python Package

```bash
# Build from source
cd /path/to/nlm
pip install -e .  # Install in development mode

# Or build and install separately
python -m build
pip install dist/*.whl
```

#### Step 3: Verify Python Installation

```python
import pynlm
print(f"NLM version: {pynlm.__version__}")
print(f"Build info: {pynlm.__build_info__}")

# Test basic functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Created brain with {brain.total_neuron_count} neurons")
```

### Python Virtual Environment Setup

```bash
# Create virtual environment
python3 -m venv nlm_env

# Activate virtual environment
source nlm_env/bin/activate  # Linux/macOS
nlm_env\Scripts\activate     # Windows

# Install dependencies
pip install -r requirements.txt

# Install NLM in development mode
pip install -e .
```

## Development Environment Setup

### Environment Variables

Set the following environment variables for optimal NLM development:

```bash
# Basic NLM configuration
export NLM_HOME=/path/to/nlm
export PYTHONPATH=$NLM_HOME/src:$PYTHONPATH

# Build and simulation configuration
export NLM_BUILD_TYPE=Release
export NLM_THREADS=4

# Python bindings configuration
export PYNLM_MODULE_PATH=/path/to/nlm/python
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/nlm/build

# Logging configuration
export NLM_LOG_LEVEL=info
export NLM_LOG_FILE=nlm.log

# Development configuration
export NLM_DEVELOPMENT_MODE=1
export NLM_VALIDATION_STRICT=1
```

### IDE Configuration

#### VS Code Configuration

Create `/.vscode/c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "NLM Debug",
            "includePath": [
                "/path/to/nlm/src",
                "/usr/include",
                "/usr/local/include"
            ],
            "defines": [
                "NLM_VERSION=\"1.0\"",
                "NLM_PHASE=6"
            ],
            "intelliSenseMode": "gcc-x64",
            "compilerPath": "/usr/bin/g++",
            "browse": {
                "path": [
                    "/path/to/nlm/src"
                ],
                "limitSymbolsToIncludedHeaders": false
            }
        }
    ],
    "version": 4
}
```

Create `/.vscode/tasks.json` for build tasks:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "cd ${workspaceFolder} && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        {
            "label": "test",
            "type": "shell",
            "command": "cd ${workspaceFolder}/build && ctest --output-on-failure",
            "group": "test"
        },
        {
            "label": "run-demo",
            "type": "shell",
            "command": "cd ${workspaceFolder}/build && ./nlm_phase6_demo",
            "problemMatcher": []
        }
    ]
}
```

#### CLion Configuration

1. Import the CMake project:
   - File → Open → Select `build/CMakeLists.txt`
   - Or open the root `CMakeLists.txt` and let CLion generate build files

2. Configure run/debug configurations:
   - Run → Edit Configurations → Add → CMake Application
   - Main executable: `./nlm`
   - Working directory: `${ProjectFileDirectory}/build`

3. Set compiler flags and C++ standard:
   - File → Settings → Build, Execution, Deployment → Compiler
   - C++ standard: C++20
   - Additional options: `-O3 -march=native -fopenmp`

### Development Workflow

#### Standard Development Workflow

```bash
# Clone and checkout a specific feature branch
git clone https://github.com/your-org/nlm.git
ncd nlm
git checkout feature/memory-integration

# Create and switch to feature branch
git checkout -b my-feature

# Make changes
# ... edit files, run tests, etc.

# Run tests
make test

# Commit changes
git add modified_files
# git commit -m "Brief description of changes"

# Push to remote branch
git push origin my-feature

# Create pull request (if using GitHub)
git checkout main
git pull origin main
```

#### Testing Workflow

```bash
# Create test branch
test_branch="test/$(date +%Y%m%d)_$(git rev-parse --short HEAD)"
git checkout -b $test_branch

# Make test changes
# ... modify code for testing

# Run comprehensive tests
cd build
make test
./nlm_test --benchmark=all

# Run integration tests
./nlm --test-mode

# Commit test results
git add .
# git commit -m "Test changes and results"
```

## Build Optimization

### Performance Build Options

```bash
# Optimized release build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto" \
         -DENABLE_PYTHON_BINDINGS=ON \
         -DENABLE_VISUALIZATION=OFF \
         -DENABLE_TESTS=ON

# Debug build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DCMAKE_CXX_FLAGS_DEBUG="-O0 -g -DDEBUG_MODE" \
         -DENABLE_PYTHON_BINDINGS=OFF \
         -DENABLE_TESTS=ON
```

### Memory Optimization

```bash
# Use memory pools
cmake .. -DMEMORY_POOL_SIZE=4096 -DEVENT_QUEUE_SIZE=8192

# Enable sparse connectivity (default)
cmake .. -DSPARSE_CONNECTIVITY=ON

# SIMD optimizations
cmake .. -DSIMD_OPTIMIZATION=ON -DVECTORIZATION_LEVEL=AVX2
```

## Advanced Installation

### Custom Installation Prefix

```bash
# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
make install

# Verify installation
ls -la /opt/nlm/bin/
ls -la /opt/nlm/lib/
```

### Multi-Configuration Builds

```bash
# Create multiple build directories for different configurations
mkdir -p build/debug
mkdir -p build/release
cd build/debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cd ../release
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Cross-Compilation

```bash
# Cross-compile for ARM
cmake .. -DCMAKE_SYSTEM_NAME=Linux \
         -DCMAKE_SYSTEM_PROCESSOR=armv7l \
         -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
         -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
```

## Post-Installation Setup

### Verification Script

Create a verification script to test installation:

```bash
#!/bin/bash
# verify_nlm_installation.sh

set -e

echo "=== NLM Installation Verification ==="

# Check executable existence
if [ ! -f "build/nlm" ]; then
    echo "ERROR: NLM executable not found"
    exit 1
fi

echo "✓ NLM executable found"

# Check Python bindings
if python3 -c "import pynlm; print('Python bindings loaded successfully')"; then
    echo "✓ Python bindings work"
else
    echo "ERROR: Python bindings failed"
    exit 1
fi

# Run basic test
if [ -f "build/nlm_test" ]; then
    echo "Running basic tests..."
    build/nlm_test --basic-tests
    echo "✓ Basic tests passed"
fi

echo "=== Installation Verification Complete ==="
```

### Configuration File Setup

Create a sample configuration file:

```bash
# Create sample config
cat > sample_config.json << EOF
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
EOF

echo "Sample configuration created: sample_config.json"
```

### Documentation Setup

Generate documentation if Doxygen/Sphinx is available:

```bash
# Build documentation
mkdir -p docs/html
mkdir -p docs/latex

# Run Doxygen
doxygen Doxyfile

# Build HTML documentation
cmake --build . --target doc-html

# Build LaTeX documentation
cmake --build . --target doc-latex
```

## Troubleshooting

### Common Installation Issues

#### Issue 1: CMake Cannot Find Python

**Problem:** CMake cannot locate Python interpreter.

**Solution:**

```bash
# Method 1: Find Python path
python3 -c "import sys; print(sys.executable)"

# Method 2: Set PYTHON_EXECUTABLE
cmake .. -DPYTHON_EXECUTABLE=$(python3 -c "import sys; print(sys.executable)")

# Method 3: Install Python development packages
apt-get install python3-dev python3-pip  # Ubuntu/Debian
brew install python3                       # macOS
```

#### Issue 2: Compiler Does Not Support C++20

**Problem:** Compiler does not support required C++20 features.

**Solution:**

```bash
# Check compiler version and features
g++ --version

# Update compiler
apt-get install g++

# Or use Clang
clang++ --version

# Check for specific features
scanning C++20 support
g++ -std=c++20 -E - <<< "concept Test {} requires T;"  # Test concepts
```

#### Issue 3: Build Memory Exhaustion

**Problem:** Build process runs out of memory.

**Solution:**

```bash
# Reduce parallel build jobs
make -j2  # or make -j4 for 4 cores

# Or use separate build directories
mkdir -p build-small
cd build-small
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j1

# Clean build before rebuilding
git clean -fdx
rm -rf build
```

#### Issue 4: Python Bindings Build Failure

**Problem:** Python bindings fail to build.

**Solution:**

```bash
# Check for Python headers
python3 -m pybind11 --includes

# Install required system packages
apt-get install python3-dev python3-pip

# Clean build
cd nlm
rm -rf build
mkdir -p build
cd build
cmake .. -DPYTHON_EXECUTABLE=$(python3 -c "import sys; print(sys.executable)")
make -j$(nproc)
```

### Runtime Issues

#### Issue 1: Segmentation Fault

**Problem:** Program crashes with segmentation fault.

**Solution:**

```bash
# Enable debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run with GDB
gdb ./nlm

# Or use AddressSanitizer
export ASAN_OPTIONS=detect_leaks=1
cmake .. -DCMAKE_CXX_FLAGS_DEBUG="-fsanitize=address"
make -j$(nproc)
./nlm
```

#### Issue 2: Memory Usage Too High

**Problem:** Memory usage exceeds system limits.

**Solution:**

```bash
# Reduce simulation size
./nlm --neuron-count 500

# Optimize configuration
cp config/optimized.json configs/

# Use memory profiling
total_memory=$(free -h | grep '^Mem:' | awk '{print $2}')
echo "System memory: $total_memory"
```

#### Issue 3: Performance Too Slow

**Problem:** Simulation performance is slower than expected.

**Solution:**

```bash
# Check system performance
top -p $(pgrep nlm)

# Use performance counters
sperf record -g ./nlm

# Optimize configuration
./nlm --tune-performance

# Check for bottlenecks
strace -e trace=write ./nlm 2>&1 | grep -i "error\|fail"
```

## Cleanup and Maintenance

### Clean Build Directory

```bash
# Remove build directory completely
rm -rf build

# Or clean individual build configurations
rm -rf build/debug
rm -rf build/release
rm -rf build/small
```

### Remove Generated Files

```bash
# Remove generated documentation
rm -rf docs/html
rm -rf docs/latex
rm -rf docs/pdf

# Remove Python build artifacts
rm -rf dist/
rm -rf *.egg-info/
rm -rf __pycache__/
rm -rf *.pyc

# Remove IDE files
rm -rf .vscode/
rm -rf .idea/
```

### Update System Packages

```bash
# Update system packages (Linux)
apt-get update
apt-get upgrade

# Update Homebrew (macOS)
brew update
brew upgrade

# Clean package cache
apt-get clean
apt-get autoclean
brew cleanup
```

## Development Best Practices

### Pre-commit Checks

```bash
#!/bin/bash
# pre-commit.sh

# Check code style
auto-correct-files

# Run linting
git diff --cached --check

# Run type checking
check-types

# Run tests
make test

# Verify documentation builds
build-docs
```

### Commit Message Guidelines

```bash
# Commit format: type(scope): description

# Examples
feat(core): add working memory integration
fix(api): resolve configuration validation bug
docs(api): add Python API reference
deps: update to pybind11 2.11.0
chore: add pre-commit hooks
```

### Code Review Checklist

- [ ] Code follows project coding standards
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Configuration validated
- [ ] Dependencies updated
- [ ] Performance benchmarks met
- [ ] No memory leaks
- [ ] Thread safety verified
- [ ] Error handling complete

## Index

- System Requirements: 3-6
- Installation Methods: 7-31
- Method 1: Building from Source: 9-31
- Step 1: Clone Repository: 10-12
- Step 2: Verify Dependencies: 13-18
- Step 3: Create Build Directory: 19-20
- Step 4: Configure Build: 21-27
- Step 5: Build Project: 28-31
- Step 6: Verify Build: 32-38
- Method 2: Package Manager: 39-49
- Debian/Ubuntu: 40-45
- macOS with Homebrew: 46-49
- Method 3: Docker: 50-52
- Python Installation: 53-84
- Pre-built Python Package: 54-55
- Building Python Bindings: 56-84
- Step 1: Install Dependencies: 57-59
- Step 2: Build and Install: 60-69
- Verify Python Installation: 70-76
- Virtual Environment Setup: 77-84
- Development Environment Setup: 85-107
- Environment Variables: 86-95
- IDE Configuration: 96-112
- VS Code Configuration: 98-106
- CLion Configuration: 111-112
- Development Workflow: 113-136
- Testing Workflow: 125-136
- Build Optimization: 138-154
- Performance Build Options: 139-147
- Memory Optimization: 148-154
- Advanced Installation: 155-183
- Custom Installation Prefix: 156-165
- Multi-Configuration Builds: 166-172
- Cross-Compilation: 173-178
- Post-Installation Setup: 179-207
- Verification Script: 180-196
- Configuration File Setup: 197-201
- Documentation Setup: 202-207
- Troubleshooting: 208-277
- Common Installation Issues: 209-227
- Runtime Issues: 228-277
- Cleanup and Maintenance: 279-313
- Clean Build Directory: 280-283
- Remove Generated Files: 284-291
- Update System Packages: 292-295
- Development Best Practices: 297-320
- Pre-commit Checks: 298-304
- Commit Message Guidelines: 305-312
- Code Review Checklist: 314-320

---

*NLM Installation and Setup Guide Version 1.0*
*Phase 6 - Final Integration*
*Last Updated: 2026-09-05*