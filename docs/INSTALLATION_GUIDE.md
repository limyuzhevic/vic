# NLM Installation and Setup Guide

## Overview

This guide provides comprehensive instructions for installing and setting up the NLM (Neural Learning Machine) framework. It covers both the C++ core installation and Python bindings setup, including dependencies, build options, and configuration.

## System Requirements

### Minimum Requirements

| Component | Minimum Requirement | Recommended |
|-----------|-------------------|-------------|
| Operating System | Linux, macOS, Windows | Linux |
| C++ Compiler | GCC 10+, Clang 12+, MSVC 2019+ | GCC 10+, Clang 12+ |
| CMake | 3.16+ | 3.16+ |
| Python | 3.8+ | 3.8+ |
| RAM | 4 GB | 16 GB+ |
| Disk Space | 500 MB | 2 GB+ |

### Recommended Hardware

For optimal NLM performance:

- **CPU**: Multi-core processor (8+ cores recommended)
- **Memory**: 16 GB+ for large-scale simulations
- **Storage**: SSD for faster file I/O
- **GPU**: Optional for accelerated computation (future feature)

## Installation Methods

### Method 1: From Source (Recommended)

#### Prerequisites

Install required dependencies:

**Ubuntu/Debian:**
```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install \
    build-essential \
    cmake \
    git \
    python3-dev \
    python3-pip \
    python3-venv \
    libreadline-dev \
    libncurses5-dev \
    libboost-all-dev \
    libeigen3-dev \
    libhdf5-dev \
    libblas-dev \
    liblapack-dev
```

**macOS:**
```bash
# Install Homebrew if not installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git python3 libomp make
```

**Windows:**
```powershell
# Install Chocolatey if not installed
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12; Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install dependencies
choco install visualstudio2019-workload-vctools cmake git python --installargs 'ADD_CMAKE_TO_PATH=System'
```

#### Build Steps

1. **Clone the Repository**

```bash
# Clone NLM repository
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

2. **Build the C++ Core**

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON

# Build with all optimizations
make -j$(nproc)

# Run tests (optional)
ctest --output-on-failure
```

3. **Install Python Dependencies**

```bash
# Install Python package manager
pip install --upgrade pip setuptools wheel

# Install required Python packages
pip install pybind11 scikit-build-core pytest numpy

# Install NLM Python bindings in development mode
pip install -e .
```

4. **Set Up Environment**

```bash
# Add NLM to system path (optional)
echo 'export PATH=$PATH:/path/to/nlm/build' >> ~/.bashrc
source ~/.bashrc

# Set up Python virtual environment (recommended)
python3 -m venv ~/nlm_env
source ~/nlm_env/bin/activate
pip install -r requirements.txt
```

5. **Verify Installation**

```bash
# Test C++ core
./build/nlm --help

# Test Python bindings
python3 -c "import pynlm; print(f'NLM Version: {pynlm.__version__}')"

# Run quick test
python3 -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f'Brain created: {brain.getTotalNeuronCount()} neurons')
for i in range(10):
    brain.step(i)
print(f'Test completed: {brain.getTotalSpikeCount()} spikes generated')
"
```

### Method 2: Using Package Manager

#### Conda (Recommended for Scientific Computing)

```bash
# Create conda environment
conda create -n nlm python=3.8 numpy scipy
conda activate nlm

# Install NLM from conda channel (if available)
conda install -c nlm-project nlm

# Or install from source
pip install -e .
```

#### Docker

```bash
# Pull NLM Docker image
docker pull nlm-project/nlm:latest

# Run container
mkdir -p ~/nlm_data
docker run -v ~/nlm_data:/workspace/app nlm-project/nlm:latest

# For development
mkdir -p nlm_dev
cd nlm_dev
docker clone https://github.com/nlm-project/nlm.git
docker build -t nlm-dev .
```

#### Homebrew (macOS)

```bash
# Add NLM repository to Homebrew
brew tap nlm-project/nlm

# Install NLM
brew install nlm-project/nlm/nlm
```

### Method 3: Using Pre-built Binaries

#### Binary Releases

Download pre-built binaries from the releases page:

1. Visit the NLM GitHub releases page
2. Download the appropriate binary for your platform
3. Extract and run:

```bash
# Linux/macOS
cd /path/to/nlm
cp nlm /usr/local/bin/
chmod +x /usr/local/bin/nlm

# Windows
# Use Git Bash or WSL
mv nlm.exe C:/\nlm\nlm.exe
```

#### Python Package Installation

```bash
# Install from PyPI (if available)
pip install pynlm

# Or install latest development version
pip install --upgrade --force-reinstall -e .
```

## Configuration and Setup

### Environment Configuration

#### Set Up Development Environment

Create a development environment with all necessary tools:

```bash
#!/bin/zsh

# Create development directory
mkdir -p ~/nlm_dev
cd ~/nlm_dev

# Clone NLM repository
git clone https://github.com/nlm-project/nlm.git

cd nlm

# Create build directory
mkdir build
cd build

# Configure with all options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PERFORMANCE_ENABLED=ON \
    -DNLM_PYTHON_BINDINGS=ON \
    -DNLM_VISION_SUPPORT=ON \
    -DNLM_AUDIO_SUPPORT=ON \
    -DBUILD_TESTING=ON

# Build
make -j$(nproc)

# Set up Python environment
cd ..
python3 -m venv env
source env/bin/activate
pip install -e .

# Add scripts to PATH
export PATH="$(pwd)/build:$PATH"
```

#### Environment Variables

Set environment variables for NLM:

```bash
# Add to ~/.bashrc or ~/.zshrc

# NLM installation path
export NLM_HOME="/path/to/nlm"
export PATH="$NLM_HOME/build:$PATH"

# Python module path
export PYTHONPATH="$NLM_HOME/src:$PYTHONPATH"

# Configuration directory
export NLM_CONFIG_DIR="$NLM_HOME/configs"

# Performance settings
export NLM_PERFORMANCE="optimized"
export NLM_THREADS=$(nproc)

# Logging
export NLM_LOG_LEVEL="INFO"
export NLM_LOG_FILE="$NLM_HOME/logs/nlm.log"
```

### Configuration Management

#### Default Configuration Files

Copy default configuration files to your home directory:

```bash
# Create configuration directory
mkdir -p ~/.config/nlm

# Copy default configurations
cp build/configs/*.json ~/.config/nlm/

# Create custom configuration directory
mkdir -p ~/.nlm
mkdir -p ~/.nlm/experiments
mkdir -p ~/.nlm/checkpoints
mkdir -p ~/.nlm/logs
```

#### Configuration File Templates

Create custom configuration files:

**Basic configuration:**

```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "checkpoint_interval": 1000,
    "log_level": "INFO"
}
```

**Exploration configuration:**

```json
{
    "neuron_count": 800,
    "region_count": 3,
    "connection_probability": 0.15,
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.02
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        },
        "structural": {
            "enable": true,
            "creation_rate": 0.001
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true,
            "scale": 0.2
        }
    },
    "development": {
        "enable": true,
        "plasticity_rate": 1.5
    }
}
```

### Performance Optimization

#### Build Optimizations

Use these CMake options for best performance:

```bash
# Release build with all optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PERFORMANCE_ENABLED=ON \
    -DNLM_VECTORIZATION=ON \
    -DNLM_MPI_SUPPORT=OFF \
    -DNLM_OPENMP=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"

# Use specific compiler optimizations
export CXXFLAGS="-O3 -march=native -flto"
export LDFLAGS="-flto"
```

#### Runtime Optimizations

Set runtime environment variables:

```bash
# Enable runtime optimizations
export OMP_NUM_THREADS=$(nproc)
export MKL_NUM_THREADS=$(nproc)
export OPENBLAS_NUM_THREADS=$(nproc)

# Memory management
export MALLOC_ARENA_MAX_SIZE=2G
export MALLOC_CONF="background_thread:true,quota:2G"

# NLM-specific optimizations
export NLM_PERFORMANCE="max"
export NLM_EVENT_QUEUE_SIZE=65536
export NLM_SPARSE_CONNECTIVITY="enabled"
```

### Testing and Validation

#### Unit Tests

Run unit tests after installation:

```bash
# Run C++ unit tests
cd build
ctest --output-on-failure

# Run Python tests
python3 -m pytest tests/ -v

# Run integration tests
python3 -m pytest tests/integration/ -v --cov=pynlm
```

#### Performance Tests

Run performance benchmarks:

```bash
# Run Python performance benchmarks
python3 nlm_python_benchmarks.py

# Run C++ performance benchmarks
./build/nlm --benchmark --neurons=10000 --steps=10000

# Run memory usage tests
./build/nlm --memory-benchmark
```

#### Example Tests

Run example scripts to verify installation:

```bash
# Run Python examples
python3 nlm_examples.py
python3 nlm_visualization_demo.py
python3 nlm_cli.py basic --steps 100

# Run C++ examples
./build/nlm_phase3_demo
./build/nlm_phase4_demo
```

### Troubleshooting

#### Common Issues and Solutions

**Issue 1: Python Bindings Not Found**

*Problem:* Python cannot import pynlm.

*Solution:*

```bash
# Check if Python can find the module
python3 -c "import sys; print(sys.path)"

# Install with verbose output
pip install -v -e .

# Check for build artifacts
find . -name "*.so" -o -name "*.pyd" -o -name "*.pyc"

# Clean and reinstall
pip uninstall pynlm
pip install -e .
```

**Issue 2: CMake Configuration Issues**

*Problem:* CMake fails to configure.

*Solution:*

```bash
# Check system requirements
./build/nlm --check-system

# Clean build directory
rm -rf build
mkdir build
cd build

# Try different build type
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Check for specific errors
cmake .. 2>&1 | grep -i error
```

**Issue 3: Compilation Errors**

*Problem:* Compiler errors during build.

*Solution:*

```bash
# Check compiler version
g++ --version

# Install missing dependencies
# For example on Ubuntu:
sudo apt-get install g++-10

# Set compiler
cmake .. -DCMAKE_CXX_COMPILER=g++-10

# Clean build and retry
make clean
make -j$(nproc)
```

**Issue 4: Runtime Errors**

*Problem:* Runtime errors when running simulations.

*Solution:*

```bash
# Check environment variables
printenv | grep NLM

# Set appropriate environment variables
export LD_LIBRARY_PATH=./build:$LD_LIBRARY_PATH
export PYTHONPATH=./src:$PYTHONPATH

# Run with debugger
gdb ./build/nlm

# Or run with valgrind
valgrind --tool=memcheck ./build/nlm --steps 100
```

#### Debugging Tools

**NLM Debug Mode**

Enable debug mode for detailed output:

```bash
# Set debug environment variable
export NLM_DEBUG=true
export NLM_LOG_LEVEL=DEBUG

# Run with debug output
./build/nlm --debug --steps 100

# Python debug mode
python3 -c "
import pynlm
import logging

# Enable debug logging
logging.basicConfig(level=logging.DEBUG)

# Create configuration with debug settings
config = pynlm.createDefaultConfig()
config.set('brain.debug', True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with debug output
for step in range(10):
    brain.step(step)
    if step % 5 == 0:
        brain.logStatus()
"
```

**Memory Debugging**

Check for memory issues:

```bash
# Run with valgrind (Linux)
valgrind --leak-check=full --track-origins=yes ./build/nlm --steps 100

# Run with address sanitizer (clang)
clang++ -fsanitize=address -g -O0 src/main.cpp -o debug_nlm
./debug_nlm --steps 100

# Python memory profiling
python3 -m memory_profiler nlm_examples.py
```

**Performance Profiling**

Profile performance:

```bash
# Linux perf
perf record -g ./build/nlm --steps 100
perf report

# Python cProfile
python3 -m cProfile -o profile.stats nlm_examples.py
python3 -c "import pstats; p = pstats.Stats('profile.stats'); p.sort_stats('cumulative').print_stats(20)"

# GNU time
/usr/bin/time -v ./build/nlm --steps 100
```

### Advanced Setup

#### Custom Build Options

Create custom build with specific options:

```bash
# Create custom CMake configuration
cat > custom_cmake_options.cmake << EOF
cmake_minimum_required(VERSION 3.16)
project(NLM Custom)

# Set custom options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Performance optimizations
add_compile_options(-O3 -march=native -flto)
add_link_options(-flto -Wl,--gc-sections)

# Memory pools
add_definitions(-DNLM_MEMORY_POOLS)

# Event queues
add_definitions(-DNLM_EVENT_QUEUES)

# SIMD vectorization
add_definitions(-DNLM_SSE -DNLM_AVX2 -DNLM_AVX512)

# Sparse connectivity
add_definitions(-DNLM_SPARSE_CONNECTIVITY)

# Parallel processing
add_definitions(-DNLM_OPENMP)
set(OPENMP_THREADS 8)
EOF

# Configure with custom options
cmake .. -C custom_cmake_options.cmake -DCMAKE_BUILD_TYPE=Release
```

#### Development Environment Setup

Set up a complete development environment:

```bash
#!/bin/zsh

# Create development workspace
mkdir -p ~/workspace/nlm
cd ~/workspace/nlm

# Clone all related repositories
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Clone supporting repositories
git clone https://github.com/nlm-project/nlm-examples.git examples
git clone https://github.com/nlm-project/nlm-tools.git tools

# Set up development Python environment
python3 -m venv ~/nlm_dev
conda activate nlm_dev

# Install all Python dependencies
pip install -r requirements.txt
pip install -e .

# Set up git hooks
mkdir .git/hooks
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
# Run code quality checks
if command -v clang-format &> /dev/null; then
    clang-format -i $(git diff --cached --name-only | grep -E '\.cpp$|\.h$')
fi

if command -v black &> /dev/null; then
    black $(git diff --cached --name-only | grep -E '\.py$')
fi
EOF
chmod +x .git/hooks/pre-commit

# Create development shortcuts
cat > Makefile << 'EOF'
.PHONY: all build test examples docs clean

all: build test

build:
	cd build
	make -j$(nproc)

test:
	cd ..
	python3 -m pytest tests/ -v

examples:
	cd ..
	python3 nlm_examples.py

docs:
	cd ..
	doxygen doxygen.conf

clean:
	rm -rf build
	rm -rf *.log
	rm -rf *.prof
	rm -rf python/__pycache__/
EOF

# Create documentation
mkdir -p docs
cat > docs/README.md << 'EOF'
# NLM Development Documentation

## Building
```bash
make build
```

## Testing
```bash
make test
```

## Examples
```bash
make examples
```

## Documentation
```bash
make docs
```

## Cleaning
```bash
make clean
```
EOF
```

# Create IDE configuration files
cat > .vscode/tasks.json << 'EOF'
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build",
            "type": "shell",
            "command": "make build",
            "group": "build",
            "problemMatcher": "$gcc-error",
            "options": {
                "cwd": "${workspaceFolder}/build"
            }
        },
        {
            "label": "Test",
            "type": "shell",
            "command": "make test",
            "group": "test",
            "problemMatcher": "$gcc-error"
        }
    ]
}
EOF
```
```

#### Continuous Integration

Set up CI/CD pipeline:

```yaml
# .github/workflows/ci.yml
name: NLM CI/CD

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: '3.9'
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake build-essential libomp-dev
        pip install -r requirements.txt
    
    - name: Build and test
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j$(nproc)
        ctest --output-on-failure
    
    - name: Python tests
      run: |
        python3 -m pytest tests/ -v --cov=pynlm
    
    - name: Performance benchmarks
      run: |
        python3 nlm_python_benchmarks.py

  benchmarks:
    runs-on: ubuntu-latest
    if: github.event_name == 'push' && github.ref == 'refs/heads/main'
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Run benchmarks
      run: |
        python3 nlm_python_benchmarks.py > benchmarks.json
        # Upload benchmarks as artifact
        echo "Benchmarks run successfully"
```

## Post-Installation Checklist

After installation, verify the setup with this checklist:

### Python Bindings

- [ ] Python imports successfully: `import pynlm`
- [ ] Basic brain creation works
- [ ] Configuration loading works
- [ ] Example scripts run
- [ ] CLI tool works

### C++ Core

- [ ] Binary executes: `./nlm --help`
- [ ] Basic functionality works
- [ ] Configuration files load
- [ ] Performance benchmarks pass

### Environment

- [ ] Environment variables set correctly
- [ ] Paths configured properly
- [ ] Dependencies installed
- [ ] Permissions set

### Testing

- [ ] Unit tests pass
- [ ] Integration tests pass
- [ ] Performance benchmarks run
- [ ] Example scripts work

### Documentation

- [ ] Installation guide readable
- [ ] API documentation accessible
- [ ] Example scripts documented
- [ ] Troubleshooting guide available

## Support and Resources

### Getting Help

#### Online Resources

1. **NLM Documentation:** https://nlm.readthedocs.io
2. **NLM GitHub Repository:** https://github.com/nlm-project/nlm
3. **NLM Discussion Forum:** https://discourse.nlm-project.org
4. **NLM Slack Channel:** https://nlm-project.slack.com
5. **NLM Stack Overflow:** https://stackoverflow.com/questions/tagged/nlm

#### Support Channels

1. **GitHub Issues:** Report bugs and request features
2. **GitHub Discussions:** General discussion and questions
3. **Email Support:** support@nlm-project.org
4. **Documentation Issues:** Submit documentation errors
5. **Feature Requests:** Request new features and enhancements

### Training and Tutorials

#### Official Tutorials

1. **NLM Getting Started:** https://nlm.readthedocs.io/en/latest/tutorials/getting_started.html
2. **Python API Tutorial:** https://nlm.readthedocs.io/en/latest/tutorials/python_api.html
3. **C++ API Tutorial:** https://nlm.readthedocs.io/en/latest/tutorials/cpp_api.html
4. **Performance Optimization:** https://nlm.readthedocs.io/en/latest/tutorials/performance.html
5. **Advanced Configuration:** https://nlm.readthedocs.io/en/latest/tutorials/configuration.html

#### Community Resources

1. **NLM Blog:** https://nlm-project.org/blog/
2. **NLM YouTube Channel:** https://www.youtube.com/nlm-project
3. **NLM Twitter:** https://twitter.com/nlm_project
4. **NLM LinkedIn:** https://linkedin.com/company/nlm-project

### Feedback and Contributions

#### Contributing to NLM

1. **Fork the Repository:** Fork NLM on GitHub
2. **Create a Feature Branch:** Create a branch for your changes
3. **Make Changes:** Implement your changes
4. **Test Changes:** Run tests to ensure nothing is broken
5. **Create Pull Request:** Submit a pull request
6. **Code Review:** Respond to code review comments
7. **Merge Changes:** Wait for merge

#### Contributing Documentation

1. **Improve Documentation:** Fix errors and add examples
2. **Write Tutorials:** Create new tutorials and guides
3. **Translate Documentation:** Help make documentation available in multiple languages
4. **Review Documentation:** Review and improve existing documentation
5. **Submit Changes:** Submit documentation changes via pull request

### Acknowledgments

#### Contributors

This installation and setup guide was developed with contributions from:

- NLM Development Team
- Documentation Authors
- Community Contributors
- Testing Team
- Performance Engineers

#### Third-Party Libraries

NLM uses the following third-party libraries:

- **CMake:** Build system
- **pybind11:** Python bindings
- **C++ Standard Library:** Core functionality
- **Eigen:** Linear algebra
- **BLAS/LAPACK:** Basic linear algebra routines
- **OpenMP:** Parallel programming
- **Boost:** C++ libraries

#### Open Source Projects

NLM is built on open source foundations:

- **GitHub Actions:** CI/CD pipeline
- **Docker Hub:** Container images
- **ReadTheDocs:** Documentation hosting
- **Slack:** Community communication
- **Stack Overflow:** Q&A platform

## License

NLM is licensed under the MIT License. See the LICENSE file for more details.

## Copyright

Copyright (c) 2024 NLM Project. All rights reserved.

---

*This installation and setup guide is continuously updated. Please check the latest version for updates and improvements.*
