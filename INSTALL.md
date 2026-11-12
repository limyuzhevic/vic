# NLM (熙然) - Installation and Setup Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This guide provides comprehensive instructions for setting up and running the NLM system on your local machine.

## System Requirements

### Minimum Requirements
- **Operating System**: Linux, macOS, or Windows 10+
- **CPU**: Multi-core processor (4+ cores recommended)
- **RAM**: 8 GB minimum, 16 GB+ recommended
- **Disk Space**: 2 GB free space for build and runtime
- **Compiler**: GCC 9+ or Clang 10+ (for native build)

### Recommended Requirements for Development
- **RAM**: 16 GB or more
- **Disk Space**: Additional 2 GB for test builds and temporary files
- **Additional Tools**: Git, CMake 3.15+, make/ninja build system

## Quick Start (15 minutes)

### Option 1: Git Clone + CMake Build (Recommended)

```bash
# Clone the repository
cd /path/to
https://github.com/your-repo/nlm.git

# Create build directory and configure
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Run the Phase 6 integration test
./nlm_phase6_demo
```

### Option 2: Build from Archive

Download the latest release archive from the releases page, extract it, and follow the same build steps as above.

## Detailed Installation Guide

### Step 1: Prerequisites Installation

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y build-essential cmake git wget unzip libomp-dev
```

#### macOS (using Homebrew)
```bash
brew update
brew install cmake git wget unzip
```

#### Fedora/CentOS/RHEL
```bash
sudo dnf install -y gcc gcc-c++ cmake git make wget unzip
```

#### Windows (using Chocolatey)
```cmd
choco install cmake git visualstudio2022-workload-nativedevelopment
```

### Step 2: Repository Setup

#### Clone from GitHub
```bash
cd /path/to/your/workspace
git clone https://github.com/your-repo/nlm.git
cd nlm

# Fetch all tags and branches
git fetch --tags --all

# Checkout the latest stable release (Phase 6)
git checkout v6.0.0
```

#### If you have an existing repository
```bash
cd existing/repo/path
# Ensure all branches are available
git fetch --tags --all
# Switch to Phase 6
cd nlm
```

### Step 3: Build Configuration

Navigate to the repository root and create a build directory:

```bash
cd /path/to/nlm
mkdir build
cd build
```

Run CMake with your preferred configuration options:

```bash
# Release build (optimized)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Debug build (with debugging symbols)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Custom build options
-DNLM_PHASE=6
-DBUILD_TESTS=ON
-DBUILD_BENCHMARKS=ON
```

### Step 4: Build the System

```bash
# Build with parallel jobs (recommended for faster build)
make -j4

# Or use all cores
make -j$(nproc)

# Build specific targets (e.g., only Phase 6 demo)
make nlm_phase6_demo
```

### Step 5: Verify Build Success

Check if the build completed successfully:

```bash
# List the built executables
ls -la bin/  # Depending on your install prefix

# Test that executables run
./bin/nlm_phase6_demo --help

# Run a simple sanity check
./bin/nlm --version
```

## Configuration Options

### Available Build Configurations

```bash
-DCMAKE_BUILD_TYPE=Release    # Optimized build for performance
-DCMAKE_BUILD_TYPE=Debug      # Build with debug symbols
-DCMAKE_BUILD_TYPE=RelWithDebInfo  # Release with debug info
-DCMAKE_BUILD_TYPE=MinSizeRel  # Optimized for size

-DNLM_PHASE=6              # Use Phase 6 integration
-DNLM_PHASE=5              # Use Phase 5 (legacy)

-DBUILD_TESTS=ON           # Build test suite
-DBUILD_BENCHMARKS=ON       # Build performance benchmarks
-DBUILD_DOCS=ON             # Build documentation

-DINSTALL_PREFIX=/usr/local # Custom installation directory
```

### Configuration File Setup

The NLM system uses configuration files in the `configs/` directory:

```bash
# View available configuration options
cat configs/default.cfg

# Create custom configuration
cp configs/default.cfg my_custom.cfg

# Modify configuration as needed
# (or use command-line overrides)
```

## Building Tests

### Run Available Tests

After build, you can run the test suite:

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test category
ctest -R "brain"
ctest -R "memory"
ctest -R "neural"

# Run tests with verbose output
cmake --build . --target test
```

### Test Categories

- **Unit Tests**: Test individual components and algorithms
- **Integration Tests**: Test system interactions and interfaces
- **Performance Tests**: Benchmark critical performance metrics
- **Regression Tests**: Verify backward compatibility

## Running NLM Systems

### Available Executables

After building, you have access to several executables:

```bash
# Phase 6: Full integration test
./bin/nlm_phase6_demo

# Phase 2: Basic neural simulation
./bin/nlm

# Phase 3: Advanced neural dynamics
./bin/nlm_phase3_demo

# Phase 4: Complex behaviors
./bin/nlm_phase4_demo

# Phase 5: Advanced cognition (if built)
./bin/nlm_phase5_demo

# Performance benchmarks
./bin/nlm_benchmark
```

### Running Demos

#### Phase 6 Demo (Recommended)
```bash
# Run for 1000 simulation steps
./bin/nlm_phase6_demo --steps 1000

# With custom configuration
./bin/nlm_phase6_demo --config my_config.cfg --steps 500

# With verbose output
./bin/nlm_phase6_demo --verbose

# Run interactive mode (if available)
./bin/nlm_phase6_demo --interactive
```

#### All Demos
```bash
# Run all demos sequentially
./bin/nlm_phase2_demo
./bin/nlm_phase3_demo
./bin/nlm_phase4_demo

# Check if Phase 5 demo exists and run it
if [ -f "./bin/nlm_phase5_demo" ]; then
    ./bin/nlm_phase5_demo
fi
```

## Development Setup

### For Contributors

#### Git Configuration
```bash
# Configure git for the project
git config user.name "Your Name"
git config user.email "your.email@example.com"

# Set up proper line endings
git config core.autocrlf input
git config core.safecrlf auto

# Set up commit message template
cat > .git/hooks/commit-msg << 'EOF'
#!/bin/bash
if ! grep -q "^\[.*\]$" "$1"; then
    echo "Error: Commit message must start with [Category]"
    exit 1
fi
EOF
chmod +x .git/hooks/commit-msg
```

#### Working with Phases

NLM follows a phased development approach. Here's how to work with different phases:

```bash
# Switch to Phase 5 (legacy)
git checkout phase5

# Switch back to Phase 6 (current)
git checkout main

# Create a feature branch
cd nlm
git checkout -b feature/my-new-feature
```

### IDE Configuration

#### Visual Studio Code

Create a `.vscode` directory with:

```json
// .vscode/c_cpp_properties.json
cppProperties.json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/src",
                "${workspaceFolder}/include"
            ],
            "defines": ["NLM_PHASE=6"],
            "intelliSenseMode": "linux",
            "compilerPath": "/usr/bin/gcc"
        }
    ],
    "version": 4
}
```

#### CLion (JetBrains)

Create `cmake-toolchain.cmake` for custom toolchains if needed:

```bash
# CLion configuration
# File -> Project Structure -> SDKs
# Set up your compiler and include paths
```

## Environment Variables

### Required Environment Variables

```bash
# Set C++ standard
export CXXFLAGS="-std=c++17 -O3"

# Memory limits for large simulations
ulimit -v 4294967296  # 4GB virtual memory
ulimit -s 67108864   # 64MB stack size

# Performance tuning
export OMP_NUM_THREADS=4  # OpenMP thread count
```

### Optional Environment Variables

```bash
# NLM-specific environment variables
export NLM_LOG_LEVEL=info    # Logging level (debug, info, warning, error)
export NLM_CONFIG_PATH=./configs/default.cfg  # Configuration file
export NLM_DATA_PATH=./data                       # Data directory
export NLM_OUTPUT_PATH=./output                   # Output directory
```

## Troubleshooting

### Common Issues and Solutions

#### Build Issues

**Issue**: CMake cannot find required packages
```bash
# Install missing packages
# Ubuntu/Debian
sudo apt install libomp-dev libboost-all-dev

# macOS
brew install openmpi
brew install llvm
```

**Issue**: Insufficient memory during build
```bash
# Use a dedicated build directory
rm -rf build
mkdir build && cd build
cmake .. && make -j$(nproc)
```

#### Runtime Issues

**Issue**: Segmentation fault during simulation
```bash
# Check system resources
free -h
df -h

# Reduce simulation complexity
./bin/nlm_phase6_demo --steps 100

# Enable debugging
./bin/nlm_phase6_demo --debug
```

**Issue**: Configuration file errors
```bash
# Validate configuration
./bin/nlm_phase6_demo --validate-config ./configs/my_config.cfg

# Create configuration from defaults
cp configs/default.cfg ./configs/my_config.cfg
```

#### Performance Issues

**Issue**: Slow simulation performance
```bash
# Use optimized build
rm -rf build_optimized
mkdir build_optimized
cd build_optimized
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Profile performance
valgrind --tool=massif ./bin/nlm_phase6_demo --steps 100
```

### Getting Help

#### Online Resources
- **Documentation**: https://github.com/your-repo/nlm/blob/main/README.md
- **Issues**: https://github.com/your-repo/nlm/issues
- **Discussions**: https://github.com/your-repo/nlm/discussions

#### Community Support
```bash
# Join our Discord/Slack community
# (Add links when available)
```

## Advanced Topics

### Custom Build Types

#### Build with Custom CMake Options
```bash
# Create a separate build directory for custom options
cd nlm
mkdir build_custom
cd build_custom

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DNLM_PHASE=6 \
  -DBUILD_TESTS=ON \
  -DBUILD_BENCHMARKS=ON \
  -DNLM_ENABLE_MPI=ON \
  -DNLM_ENABLE_OPENMP=ON

make -j$(nproc)
```

### Running with Valgrind
```bash
cd nlm
mkdir build_valgrind
cd build_valgrind

cmake .. -DCMAKE_BUILD_TYPE=Debug
make

# Run with memory checking
valgrind --leak-check=full --track-origins=yes ./bin/nlm_phase6_demo --steps 50
```

### Profiling
```bash
# Generate profiling data
cd build
cmake --build . --target profile

# Use gprof or other profilers
./bin/nlm_phase6_demo --steps 100 2>&1 | gprof ./nlm_phase6_demo gmon.out
```

## Cleanup and Maintenance

### Cleaning Build Artifacts
```bash
# Remove build directories
rm -rf build build_custom build_valgrind build_optimized

# Clean git repository (careful!)
git clean -xdf

# Remove configuration files
cd nlm
rm -f configs/*.cfg
rm -f output/*
rm -f logs/*
```

### Updating Dependencies
```bash
# Update submodules if any
git submodule update --init --recursive

# Update version information
# Edit CMakeLists.txt or version files as needed
```

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

This project builds on the research and development of previous NLM phases and contributions from the open-source community.

## Support

For technical support, please open an issue on the GitHub repository.