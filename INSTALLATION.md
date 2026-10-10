# NLM Installation Guide

## Overview

This document provides comprehensive installation instructions for the NLM (熙然) Neural Simulation Project. NLM is an experimental artificial developmental brain project that simulates spiking neural networks with multiple interacting systems including memory, neuromodulation, prediction, and cognition.

## Prerequisites

### Hardware Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| CPU | Dual-core | Multi-core |
| Memory | 4 GB | 16 GB |
| Disk Space | 500 MB | 2 GB |
| GPU | Optional | Optional (for visualization) |

### Software Dependencies

#### C++ Development Environment

- **Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **Standard Library**: C++20 or later
- **CMake**: Version 3.16 or higher

#### Python Environment

- **Python**: 3.8+ (required for Python bindings)
- **Development Tools**: pip, setuptools
- **Required Libraries**: numpy, pybind11, scikit-build-core

#### Optional Dependencies

- **CUDA**: For GPU acceleration (if supported)
- **OpenGL/Vulkan**: For visualization
- **LibSDL2**: For window management
- **LibPNG**: For image output

## Installation Methods

### Method 1: Building from Source (Recommended)

This method provides the most control and access to all features.

#### Step 1: Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/your-username/nlm.git
ncd nlm

# Create and activate development environment (optional)
python -m venv nlm_env
source nlm_env/bin/activate

# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy
```

#### Step 2: Build the C++ Core

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (Release build for production)
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DPYTHON_EXECUTABLE=$(which python) \
         -DBUILD_PYTHON_BINDINGS=ON \
         -DBUILD_EXAMPLES=ON \
         -DBUILD_TESTS=ON

# Build all targets
make -j$(nproc)

# Optional: Install system-wide (requires sudo)
# sudo make install
```

#### Step 3: Install Python Package

```bash
# Install the Python bindings
cd ..  # Go back to repository root
pip install -e .  # Install in development mode

# Or install from build directory
cd build
pip install .  # Install from built distribution
```

#### Step 4: Verify Installation

```bash
# Test C++ executable
./nlm --help

# Test Python bindings
python -c "import pynlm; print(pynlm.__version__)"

# Run basic tests
python -m pytest tests/ -v
```

### Method 2: Using Pre-built Packages

#### Ubuntu/Debian

```bash
# Add repository (if available)
sudo apt-get update
sudo apt-get install nlm-dev nlm-runtime

# Or manually install dependencies
sudo apt-get install build-essential cmake python3-dev \
                   python3-pip libboost-all-dev
```

#### macOS (using Homebrew)

```bash
# Install dependencies
brew install cmake python3

# Clone and build
https://github.com/your-username/nlm.git
cd nlm
./install_macos.sh
```

#### Windows

##### Using Chocolatey

```bash
choco install cmake python visualstudio2019buildtools

# Clone and build
https://github.com/your-username/nlm.git
cd nlm
.
install_windows.bat
```

##### Using WSL (Windows Subsystem for Linux)

Follow the Linux instructions above within WSL.

### Method 3: Docker (Development Environment)

#### Standard Development Container

```bash
# Build Docker image
docker build -t nlm-dev .

# Run development container
docker run -it --rm \
    -v $(pwd)/workspace:/workspace \
    -w /workspace nlm-dev
```

#### For Production

```bash
# Pull production image (if available)
docker pull yourusername/nlm:latest

# Run container
docker run --rm yourusername/nlm:latest
```

## Configuration Options

### CMake Configuration

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) |
| `BUILD_PYTHON_BINDINGS` | ON | Build Python bindings |
| `BUILD_EXAMPLES` | ON | Build example programs |
| `BUILD_TESTS` | ON | Build test suite |
| `ENABLE_CUDA` | OFF | Enable CUDA acceleration |
| `ENABLE_MPI` | OFF | Enable MPI parallelization |
| `NLM_ENABLE_PROFILING` | OFF | Enable performance profiling |

### Python Configuration

```python
# nlm/config.py
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save configuration
config.saveToFile("my_config.json")
```

## Environment Setup

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `NLM_LOG_LEVEL` | info | Logging level (trace, debug, info, warn, error, critical) |
| `NLM_VERBOSE` | false | Enable verbose output |
| `NLM_CONFIG_FILE` | unset | Path to configuration file |
| `NLM_THREADS` | auto | Number of threads to use |
| `PYNLM_DEBUG` | false | Enable Python binding debugging |

### Shell Configuration

Add to your shell configuration file (`~/.bashrc`, `~/.zshrc`, etc.):

```bash
# Add NLM to PATH
export PATH=$PATH:/usr/local/bin

# Set environment variables
export NLM_LOG_LEVEL=info
export NLM_THREADS=auto
```

## Post-Installation Steps

### 1. Configure Development Environment

#### Create Project Configuration

```bash
# Create project configuration directory
mkdir -p ~/.config/nlm

# Create initial configuration
mkdir -p configs
cat > configs/default.cfg << 'EOF'
# Default NLM configuration
neuron_count = 1000
max_steps = 1000
timestep = 0.001
output_format = "text"
enable_profiling = false
enable_validation = true
parallel = 4
EOF
```

### 2. Set Up Development Tools

#### Editor Configuration

For optimal development experience, configure your editor:

**VS Code Settings:**
```json
{
    "C_Cpp.clangdPath": "/usr/bin/clangd",
    "C_Cpp.intelliSenseEngine": "CMake Tools",
    "files.exclude": {
        "**/.git": true,
        "**/*.pyc": true,
        "**/__pycache__": true
    },
    "cmake.buildOnStart": false
}
```

**Clangd Integration:**
```bash
# Install clangd
python3 -m pip install clangd

# Generate compile_commands.json
cd build
```

### 3. Testing and Validation

#### Run Test Suite

```bash
# Run unit tests
cd build
ctest --output-on-failure

# Run Python tests
python -m pytest tests/ -v --cov=pynlm

# Run integration tests
python -m pytest tests/integration/ -v
```

#### Performance Validation

```bash
# Run benchmark suite
./benchmark/nlm_benchmark

# Profile with Valgrind (Linux)
sudo valgrind --leak-check=full ./nlm

# Memory profiling
python -m memory_profiler your_script.py
```

### 4. Documentation and Resources

#### Build Documentation

```bash
# Build Doxygen documentation
cd docs
doxygen Doxyfile

# Build Sphinx documentation
pip install sphinx sphinx-rtd-theme
make -C docs/html html
```

#### Access Online Resources

- **Documentation**: https://docs.nlm-project.org
- **API Reference**: https://api.nlm-project.org
- **Community Forum**: https://forum.nlm-project.org
- **Issue Tracker**: https://github.com/your-username/nlm/issues

## Troubleshooting

### Common Installation Issues

#### CMake Cannot Find Python

```bash
# On Ubuntu/Debian
sudo apt-get install python3-dev python3-pip

# On macOS
brew install python3

# Re-run CMake
cd build
cmake .. -DPython_EXECUTABLE=$(which python3)
```

#### Python Module Import Error

```bash
# Reinstall Python package
pip install --force-reinstall --no-deps -e .

# Clear Python cache
rm -rf __pycache__ *.pyc
python -m pip cache purge
```

#### Compiler Compatibility Issues

```bash
# Update compiler
sudo apt-get update
sudo apt-get install g++

# Check compiler version
g++ --version
```

#### Memory Issues During Build

```bash
# Use more memory
make -j$(nproc --ignore=2)

# Or build with less optimization
export CXXFLAGS="-O2 -g"
```

### Getting Help

#### Common Debug Commands

```bash
# Check system configuration
./nlm --system-info

# Validate configuration
./nlm --validate-config configs/default.cfg

# Run with debug output
NLM_LOG_LEVEL=debug ./nlm
```

#### Community Support

- **GitHub Issues**: https://github.com/your-username/nlm/issues
- **Slack Channel**: #nlm-support
- **Discord Server**: https://discord.gg/nlm
- **Email Support**: support@nlm-project.org

## Uninstallation

### Remove System Installation

```bash
# Remove system-wide installation
sudo make uninstall

# Remove Python package
pip uninstall pynlm

# Remove documentation
doc uninstall nlm
```

### Clean Development Environment

```bash
# Remove build directory
cd ..
rm -rf build

# Remove virtual environment (if created)
rm -rf nlm_env

# Clear Python cache
rm -rf __pycache__ *.pyc
```

## Advanced Installation Options

### Custom Build Configuration

#### Enable Experimental Features

```bash
# Enable experimental features
cmake .. \
    -DNLM_ENABLE_EXPERIMENTAL=ON \
    -DNLM_EXPERIMENTAL_FEATURES="learning,plasticity,memory"
```

#### Cross-Compilation

```bash
# Cross-compile for ARM
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain-arm.cmake \
    -DCMAKE_BUILD_TYPE=Release
```

#### Distribution Package

```bash
# Create distribution package
cd build
cpack --config CPackConfig.cmake
```

## Version Management

### Installation Versions

| Version | Description | Installation Command |
|---------|-------------|---------------------|
| `latest` | Latest stable release | `pip install nlm` |
| `stable` | Most recent stable release | `pip install nlm==1.0.0` |
| `dev` | Development version | `pip install git+https://github.com/your-username/nlm.git` |

### Upgrading

```bash
# Upgrade to latest version
pip install --upgrade pynlm

# Upgrade to specific version
pip install pynlm==1.0.0

# From development to stable
cd ..
git tag -d v1.0.0
```

## Conclusion

The NLM neural simulation project is now ready for use. You have successfully installed the core system, Python bindings, and development tools. The next step is to explore the examples and documentation to get started with neural simulation.

For complete usage instructions, refer to:
- `easy_usage.md` - Quick start guide
- `HOW_TO_USE.md` - Complete technical documentation
- `examples/` - Working examples
- `docs/` - Comprehensive documentation

If you encounter any issues during installation, refer to the troubleshooting section above or seek help through the project's community channels.
