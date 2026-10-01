# Improved NLM Build and Installation Guide

## Overview

This guide provides comprehensive instructions for building and installing the NLM Python bindings with enhanced features, improved build process, and better development experience.

## Quick Start

### Option 1: Install from Release (Recommended)

```bash
# Install the latest release
pip install pynlm
```

### Option 2: Install from Source (Development)

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install in development mode
pip install -e .
```

### Option 3: Build and Install Custom

```bash
# Create build directory
cd nlm
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON

# Build
make -j$(nproc)

# Install
make install

# Install Python bindings
pip install -e ..
```

## Enhanced Installation Process

### 1. Prerequisites

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y cmake g++ git python3-dev python3-pip python3-venv
```

#### macOS
```bash
# Install Xcode command line tools
xcode-select --install

# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

brew install cmake git python3
```

#### Windows
```bash
# Install Chocolatey (package manager)
# Download and run: https://chocolatey.org/install

choco install cmake python git visualstudio2019buildtools
```

### 2. Recommended Python Environment

```bash
# Create and activate virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Upgrade pip and setuptools
pip install --upgrade pip setuptools wheel

# Install build dependencies
pip install scikit-build-core pybind11 numpy matplotlib scipy pandas seaborn

# Install optional dependencies
pip install notebook jupyter ipykernel
```

### 3. Advanced Build Configuration

#### Custom Build Options

```bash
cd nlm
mkdir build
cd build

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_COMPILER=g++ \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DNLM_ENABLE_TESTING=OFF \
    -DNLM_ENABLE_VIZ=ON \
    -DNLM_ENABLE_PYTHON=OFF \
    -DNLM_ENABLE_DOCS=OFF
```

#### Development Build

```bash
# Configure for development with debug symbols and Python debugging
cd nlm
mkdir build_dev
cd build_dev

cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DNLM_ENABLE_PYTHON=ON \
    -DNLM_ENABLE_TESTING=ON \
    -DNLM_ENABLE_PROFILING=ON \
    -DNLM_ENABLE_COVERAGE=ON
```

### 4. Building with Container (Docker)

#### Dockerfile for Development

```dockerfile
FROM python:3.11-slim

# Install system dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy requirements and pyproject.toml
COPY requirements.txt pyproject.toml ./

# Install Python dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Copy project source
COPY . .

# Build and install the project
RUN pip install -e .

# Create app directory
RUN mkdir -p /app/app

# Set environment variables
ENV PYTHONPATH=/app
ENV LD_LIBRARY_PATH=/usr/local/lib

# Expose ports for Jupyter
EXPOSE 8888

# Default command
CMD ["python", "-m", "nlm.cli"]
```

#### Docker Compose for Full Environment

```yaml
version: '3.8'

services:
  nlm:
    build: .
    ports:
      - "8888:8888"
    volumes:
      - ./examples:/app/examples
      - ./results:/app/results
    environment:
      - PYTHONPATH=/app
      - NLM_DATA_PATH=/app/data
    command: ["python", "-m", "nlm.cli", "run", "--help"]

  nlm-dev:
    build: .
    ports:
      - "8888:8888"
      - "6006:6006"  # TensorBoard
    volumes:
      - ./:/app
      - nlm_data:/app/data
    environment:
      - PYTHONPATH=/app
      - NLM_DATA_PATH=/app/data
      - NLM_DEBUG=1
    command: ["jupyter", "lab", "--ip=0.0.0.0", "--port=8888", "--no-browser"]

  nlm-test:
    build: .
    volumes:
      - ./:/app
    environment:
      - PYTHONPATH=/app
    command: ["pytest", "tests/", "-v", "--tb=short"]

volumes:
  nlm_data:
```

## Enhanced Build Scripts

### build_nlm.py

```python
#!/usr/bin/env python3
"""
Enhanced NLM build script with pre-build checks and post-build verification.
"""

import os
import sys
import subprocess
import platform
import argparse
from pathlib import Path

class NLMBuilder:
    def __init__(self, source_dir=None):
        self.source_dir = Path(source_dir) if source_dir else Path.cwd()
        self.build_dir = self.source_dir / "build"
        self.setup_pyproject()
        
    def setup_pyproject(self):
        """Update pyproject.toml with enhanced settings."""
        pyproject_path = self.source_dir / "pyproject.toml"
        
        with open(pyproject_path, 'r') as f:
            content = f.read()
        
        # Add enhanced configuration
        enhanced_content = content + """

[project.optional-dependencies]
full = [
    "matplotlib>=3.5.0",
    "numpy>=1.20.0", 
    "scipy>=1.7.0",
    "pandas>=1.3.0",
    "seaborn>=0.11.0",
    "plotly>=5.0.0",
    "jupyter>=1.0.0",
]

[project.scripts]
nlm = "nlm.cli:main"

[project.gui-scripts]
nlm-gui = "nlm.gui:main"

[tool.scikit-build.cmake.define]
NLM_ENABLE_VIZ = true
NLM_ENABLE_PYTHON = true
NLM_ENABLE_DOCS = true
NLM_ENABLE_TESTS = true
NLM_ENABLE_PROFILING = true

[tool.scikit-build.cmake.define.Feature]
name = "NLM_BACKEND"
description = "C++ backend implementation"
options = ["optimized", "debug", "minimal"]
default = "optimized"

[tool.scikit-build.cmake.define.Feature]
name = "NLM_PRECISION" 
description = "Numerical precision mode"
options = ["double", "single", "custom"]
default = "double"
"""
        
        with open(pyproject_path, 'w') as f:
            f.write(enhanced_content)
    
    def check_prerequisites(self):
        """Check if all prerequisites are installed."""
        checks = [
            ("CMake", "cmake --version"),
            ("C++ Compiler", "g++ --version"),
            ("Python", "python3 --version"),
            ("pip", "pip --version"),
        ]
        
        failed = []
        
        for name, command in checks:
            try:
                subprocess.run(command, shell=True, check=True, 
                              capture_output=True, timeout=30)
                print(f"✓ {name} found")
            except (subprocess.CalledProcessError, subprocess.TimeoutExpired):
                print(f"✗ {name} not found")
                failed.append(name)
        
        if failed:
            raise RuntimeError(f"Missing prerequisites: {', '.join(failed)}")
    
    def configure(self, options=None):
        """Configure the build with custom options."""
        options = options or {}
        
        cmake_args = [
            "cmake",
            str(self.source_dir),
            f"-DCMAKE_BUILD_TYPE={'Release' if options.get('release', True) else 'Debug'}",
            f"-DBUILD_SHARED_LIBS={'ON' if options.get('shared', True) else 'OFF'}",
            f"-DNLM_BACKEND={options.get('backend', 'optimized')}",
            f"-DNLM_PRECISION={options.get('precision', 'double')}",
        ]
        
        if platform.system() == "Windows":
            cmake_args.extend([
                "-G", "Visual Studio 16 2019",
                "-A", "x64"
            ])
        
        print(f"Configuring with: {' '.join(cmake_args)}")
        
        self.build_dir.mkdir(exist_ok=True)
        result = subprocess.run(cmake_args, cwd=self.build_dir, capture_output=True, text=True)
        
        if result.returncode != 0:
            print("CMake configuration failed:")
            print(result.stdout)
            print(result.stderr)
            raise RuntimeError("CMake configuration failed")
        
        print("✓ Configuration completed successfully")
    
    def build(self, jobs=None):
        """Build the project."""
        if jobs is None:
            jobs = os.cpu_count() or 4
        
        make_args = ["make", "-j", str(jobs)]
        
        print(f"Building with {jobs} jobs...")
        result = subprocess.run(make_args, cwd=self.build_dir, capture_output=True, text=True)
        
        if result.returncode != 0:
            print("Build failed:")
            print(result.stdout)
            print(result.stderr)
            raise RuntimeError("Build failed")
        
        print("✓ Build completed successfully")
    
    def install(self):
        """Install the built project."""
        make_args = ["make", "install"]
        
        print("Installing...")
        result = subprocess.run(make_args, cwd=self.build_dir, capture_output=True, text=True)
        
        if result.returncode != 0:
            print("Installation failed:")
            print(result.stdout)
            print(result.stderr)
            raise RuntimeError("Installation failed")
        
        print("✓ Installation completed successfully")
    
    def verify_installation(self):
        """Verify the installation works correctly."""
        print("Verifying installation...")
        
        test_script = self.source_dir / "test_installation.py"
        with open(test_script, 'w') as f:
            f.write("""
import pynlm
import sys

print("Testing NLM installation...")

# Test basic functionality
try:
    config = pynlm.createDefaultConfig()
    print("✓ Configuration created")
    
    brain = pynlm.createBrain(config)
    print("✓ Brain created")
    
    brain.initialize()
    print("✓ Brain initialized")
    
    brain.step(0)
    print("✓ Brain step executed")
    
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Test AgentBrain
    agent = pynlm.createAgentBrain(brain)
    print("✓ Agent created")
    
    # Test world
    world = pynlm.createSimpleWorld()
    print("✓ World created")
    
    print("All tests passed!")
    
except Exception as e:
    print(f"Installation verification failed: {e}")
    sys.exit(1)
""")
        
        try:
            result = subprocess.run([sys.executable, str(test_script)], 
                                  capture_output=True, text=True, timeout=60)
            
            if result.returncode == 0:
                print("✓ Installation verification passed")
                print(result.stdout)
            else:
                print("✗ Installation verification failed")
                print(result.stdout)
                print(result.stderr)
                return False
        finally:
            test_script.unlink()
        
        return True
    
    def build_and_install(self, **kwargs):
        """Complete build and install process."""
        print("Starting NLM build and installation...")
        
        try:
            self.check_prerequisites()
            self.configure(kwargs)
            self.build(kwargs.get('jobs'))
            self.install()
            
            if self.verify_installation():
                print("\n✓ NLM installation completed successfully!")
                print("\nTo use NLM:")
                print("  python3 -c 'import pynlm; print(pynlm.__version__)'")
                print("  python3 -m nlm.cli --help")
                return True
            else:
                raise RuntimeError("Installation verification failed")
        except Exception as e:
            print(f"\n✗ Installation failed: {e}")
            return False

def main():
    parser = argparse.ArgumentParser(description="Enhanced NLM build script")
    parser.add_argument("--release", action="store_true", default=True,
                       help="Build in release mode (default)")
    parser.add_argument("--shared", action="store_true", default=True,
                       help="Build shared libraries (default)")
    parser.add_argument("--backend", choices=["optimized", "debug", "minimal"], 
                       default="optimized", help="Build backend (default: optimized)")
    parser.add_argument("--precision", choices=["double", "single", "custom"], 
                       default="double", help="Numerical precision (default: double)")
    parser.add_argument("--jobs", type=int, help="Number of parallel jobs for build")
    parser.add_argument("--source-dir", help="Source directory (default: current directory)")
    
    args = parser.parse_args()
    
    builder = NLMBuilder(args.source_dir)
    
    kwargs = {
        'release': args.release,
        'shared': args.shared,
        'backend': args.backend,
        'precision': args.precision
    }
    
    if args.jobs:
        kwargs['jobs'] = args.jobs
    
    success = builder.build_and_install(**kwargs)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
```

### build.sh (Bash Build Script)

```bash
#!/bin/bash

# Enhanced NLM build script with error handling and logging

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
LOG_FILE="$PROJECT_ROOT/build.log"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1" | tee -a "$LOG_FILE"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1" | tee -a "$LOG_FILE"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "$LOG_FILE"
}

# Function to check prerequisites
check_prerequisites() {
    log_info "Checking prerequisites..."
    
    local errors=()
    
    # Check CMake
    if ! command -v cmake &> /dev/null; then
        errors+=("CMake not found. Please install CMake (version 3.16 or higher)")
    else
        local cmake_version=$(cmake --version | head -n1)
        log_info "Found CMake: $cmake_version"
    fi
    
    # Check C++ compiler
    if ! command -v g++ &> /dev/null; then
        errors+=("g++ not found. Please install a C++ compiler")
    else
        local gcc_version=$(g++ --version | head -n1)
        log_info "Found g++: $gcc_version"
    fi
    
    # Check Python
    if ! command -v python3 &> /dev/null; then
        errors+=("Python3 not found. Please install Python 3.8 or higher")
    else
        local python_version=$(python3 --version)
        log_info "Found Python: $python_version"
    fi
    
    if [ ${#errors[@]} -gt 0 ]; then
        log_error "Prerequisites check failed:"
        for error in "${errors[@]}"; do
            log_error "  $error"
        done
        return 1
    fi
    
    log_info "All prerequisites satisfied"
}

# Function to configure build
configure_build() {
    local build_type="$1"
    local shared_libs="$2"
    local backend="$3"
    local precision="$4"
    
    log_info "Configuring build with:"
    log_info "  Build type: $build_type"
    log_info "  Shared libraries: $shared_libs"
    log_info "  Backend: $backend"
    log_info "  Precision: $precision"
    
    local cmake_args=(
        "-DCMAKE_BUILD_TYPE=$build_type"
        "-DBUILD_SHARED_LIBS=$shared_libs"
        "-DNLM_BACKEND=$backend"
        "-DNLM_PRECISION=$precision"
    )
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        cmake_args+=("-DCMAKE_CXX_COMPILER=g++")
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        cmake_args+=("-DCMAKE_CXX_COMPILER=clang++")
    fi
    
    log_info "Running CMake configuration..."
    
    if ! mkdir -p "$PROJECT_ROOT/build"; then
        log_error "Failed to create build directory"
        return 1
    fi
    
    cd "$PROJECT_ROOT/build"
    
    if ! cmake "${cmake_args[@]}" ..; then
        log_error "CMake configuration failed"
        return 1
    fi
    
    log_info "Configuration completed successfully"
}

# Function to build project
build_project() {
    local jobs="$1"
    
    log_info "Building project..."
    
    if [ -z "$jobs" ]; then
        jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    fi
    
    log_info "Using $jobs parallel jobs"
    
    if ! make -j"$jobs"; then
        log_error "Build failed"
        return 1
    fi
    
    log_info "Build completed successfully"
}

# Function to install project
install_project() {
    log_info "Installing project..."
    
    if ! make install; then
        log_error "Installation failed"
        return 1
    fi
    
    log_info "Installation completed successfully"
}

# Function to verify installation
verify_installation() {
    log_info "Verifying installation..."
    
    cd "$PROJECT_ROOT"
    
    local python_script="test_installation.py"
    
    cat > "$python_script" << 'EOF'
import pynlm
import sys

print("Testing NLM installation...")

# Test basic functionality
try:
    config = pynlm.createDefaultConfig()
    print("✓ Configuration created")
    
    brain = pynlm.createBrain(config)
    print("✓ Brain created")
    
    brain.initialize()
    print("✓ Brain initialized")
    
    brain.step(0)
    print("✓ Brain step executed")
    
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Test AgentBrain
    agent = pynlm.createAgentBrain(brain)
    print("✓ Agent created")
    
    # Test world
    world = pynlm.createSimpleWorld()
    print("✓ World created")
    
    print("All tests passed!")
    
except Exception as e:
    print(f"Installation verification failed: {e}")
    sys.exit(1)
EOF
    
    if ! python3 "$python_script"; then
        log_error "Installation verification failed"
        rm "$python_script"
        return 1
    fi
    
    rm "$python_script"
    log_info "Installation verification passed"
}

# Main execution
main() {
    # Parse command line arguments
    BUILD_TYPE="Release"
    SHARED_LIBS="ON"
    BACKEND="optimized"
    PRECISION="double"
    JOBS=""
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --debug)
                BUILD_TYPE="Debug"
                shift
                ;;
            --static)
                SHARED_LIBS="OFF"
                shift
                ;;
            --backend)
                BACKEND="$2"
                shift 2
                ;;
            --precision)
                PRECISION="$2"
                shift 2
                ;;
            --jobs)
                JOBS="$2"
                shift 2
                ;;
            --help)
                echo "Usage: $0 [options]"
                echo ""
                echo "Options:"
                echo "  --debug              Build in Debug mode"
                echo "  --static             Build static libraries"
                echo "  --backend <type>     Build backend (optimized, debug, minimal)"
                echo "  --precision <type>   Numerical precision (double, single, custom)"
                echo "  --jobs <num>         Number of parallel jobs"
                echo "  --help               Show this help"
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    # Start logging
    log_info "Starting NLM build process..."
    log_info "Project root: $PROJECT_ROOT"
    
    # Execute build steps
    if ! check_prerequisites; then
        exit 1
    fi
    
    if ! configure_build "$BUILD_TYPE" "$SHARED_LIBS" "$BACKEND" "$PRECISION"; then
        exit 1
    fi
    
    if ! build_project "$JOBS"; then
        exit 1
    fi
    
    if ! install_project; then
        exit 1
    fi
    
    if ! verify_installation; then
        exit 1
    fi
    
    log_info "NLM build and installation completed successfully!"
    log_info "\nNext steps:"
    log_info "  1. Install Python dependencies: pip install -r requirements.txt"
    log_info "  2. Test the installation: python3 -m pytest tests/ -v"
    log_info "  3. Run examples: python3 -m nlm.cli examples/"
    log_info "  4. Start using NLM: import pynlm; help(pynlm)"
}

# Run main function
main "$@"
```

### Install requirements.txt

```bash
# requirements.txt
numpy>=1.20.0
scipy>=1.7.0
matplotlib>=3.5.0
pandas>=1.3.0
seaborn>=0.11.0
plotly>=5.0.0
jupyter>=1.0.0
ipykernel>=6.0.0

# Development dependencies
pytest>=7.0.0
black>=21.12b0
flake8>=4.0.0
mypy>=0.910
pylint>=2.12.0

# Documentation dependencies
doc8>=0.8.1
pydocstyle>=5.1.0

# Advanced analysis
statsmodels>=0.12.0
networkx>=2.6.0
scikit-learn>=1.0.0
```

### Development Workflow Script

```bash
#!/bin/bash

# Development workflow script for NLM
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

case "${1:-}" in
    setup)
        echo "Setting up development environment..."
        pip install -e .
        pip install -r requirements.txt
        pip install -e ".[full]"
        echo "✓ Development environment setup complete"
        ;;
    
    build)
        echo "Building NLM..."
        mkdir -p build
        cd build
        cmake ..
        make -j$(nproc)
        cd ..
        echo "✓ Build complete"
        ;;
    
    test)
        echo "Running tests..."
        python -m pytest tests/ -v --tb=short
        echo "✓ Tests complete"
        ;;
    
    format)
        echo "Formatting code..."
        black python/ src/ tests/ examples/
        flake8 python/ src/ tests/ examples/
        mypy python/ src/
        echo "✓ Code formatting complete"
        ;;
    
    docs)
        echo "Building documentation..."
        mkdocs build
        echo "✓ Documentation build complete"
        ;;
    
    examples)
        echo "Running examples..."
        python examples/example1_basic_usage.py
        python examples/example2_agent_training.py
        python examples/example3_visualization.py
        echo "✓ Examples complete"
        ;;
    
    all)
        echo "Running full development workflow..."
        $0 setup
        $0 build
        $0 format
        $0 test
        $0 docs
        $0 examples
        echo "✓ Full workflow complete"
        ;;
    
    *)
        echo "Usage: $0 {setup|build|test|format|docs|examples|all}"
        echo ""
        echo "Commands:"
        echo "  setup   - Set up development environment"
        echo "  build   - Build the project"
        echo "  test    - Run tests"
        echo "  format  - Format and lint code"
        echo "  docs    - Build documentation"
        echo "  examples - Run examples"
        echo "  all     - Run full development workflow"
        exit 1
        ;;
esac
```

### Dockerfile for Production

```dockerfile
FROM python:3.11-slim-buster

# Install system dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy requirements and pyproject.toml
COPY pyproject.toml requirements.txt ./

# Install Python dependencies with optimization
RUN pip install --no-cache-dir --upgrade pip setuptools wheel && \
    pip install --no-cache-dir -r requirements.txt

# Copy project source
COPY . .

# Build and install the project with optimizations
RUN pip install --no-cache-dir -e . --no-build-isolation

# Create app directory
RUN mkdir -p /app/app /app/examples /app/data

# Set environment variables
ENV PYTHONPATH=/app
ENV LD_LIBRARY_PATH=/usr/local/lib
ENV PYTHONUNBUFFERED=1
ENV PYTHONDONTWRITEBYTECODE=1

# Create non-root user for security
RUN useradd -m -u 1000 nlmuser && \
    chown -R nlmuser:nlmuser /app
USER nlmuser

# Expose ports for Jupyter
EXPOSE 8888

# Health check
HEALTHCHECK --interval=30s --timeout=30s --start-period=5s --retries=3 \
    CMD python -c "import pynlm; print('NLM available')" || exit 1

# Default command
CMD ["python", "-m", "nlm.cli", "--help"]
```

## Environment Configuration

### Environment Variables

```bash
# NLM configuration variables
export NLM_DATA_PATH="/path/to/nlm/data"
export NLM_MAX_MEMORY_GB=8
export NLM_NUM_THREADS=4
export NLM_LOG_LEVEL="INFO"
export NLM_DEBUG_MODE="false"
export NLM_BACKEND="optimized"
export NLM_PRECISION="double"

# Performance tuning
export OMP_NUM_THREADS=4
export MKL_NUM_THREADS=4
export OPENBLAS_NUM_THREADS=4
```

### Configuration Files

#### config.yaml
```yaml
# NLM configuration template
nlm:
  simulation:
    max_steps: 1000
    timestep: 0.1
    neuron_count: 1000
    synapse_density: 0.1
  
  learning:
    enable_reward_modulation: true
    enable_curiosity: true
    enable_structural_plasticity: true
    enable_development: true
    
  world:
    width: 20
    height: 20
    vision_width: 8
    vision_height: 8
    
  visualization:
    enabled: true
    output_format: "png"
    dpi: 300
```

## Troubleshooting

### Common Issues and Solutions

#### 1. CMake Configuration Error

**Problem:** CMake cannot find Python interpreter.

**Solution:**
```bash
# Find Python executable location
which python3

# Configure with explicit Python path
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
```

#### 2. Missing Dependencies

**Problem:** Compilation fails due to missing C++ libraries.

**Solution (Ubuntu/Debian):**
```bash
sudo apt install -y build-essential cmake libomp-dev libboost-all-dev
```

**Solution (macOS):**
```bash
brew install cmake llvm
export CC=$(brew --prefix llvm)/bin/clang
export CXX=$(brew --prefix llvm)/bin/clang++
```

#### 3. Python Module Import Error

**Problem:** `import pynlm` fails after installation.

**Solution:**
```bash
# Check if module is installed
pip list | grep pynlm

# Install in development mode
pip install -e .

# Check for errors
python -c "import pynlm; print(pynlm.__version__)"
```

#### 4. Build Performance Issues

**Problem:** Build takes too long.

**Solution:**
```bash
# Use parallel build
make -j$(nproc)

# Or use ninja if available
if command -v ninja &> /dev/null; then
    cmake .. -G Ninja
    ninja
fi
```

#### 5. Memory Issues

**Problem:** Build fails due to insufficient memory.

**Solution:**
```bash
# Reduce build parallelism
make -j2

# Or build incrementally
for step in configure build install; do
    echo "=== $step ==="
    time make $step
    echo ""
done
```

### Performance Monitoring

#### Build Monitoring

```bash
#!/bin/bash
# monitor_build.sh - Monitor build performance

BUILD_LOG="build.log"
MEMORY_LOG="memory_usage.log"
CPU_LOG="cpu_usage.log"

# Start background monitoring
{ while true; do
    ps -o pid,ppid,cmd,%mem,%cpu --forest | grep -E "(cmake|make)" | head -10 >> "$MEMORY_LOG"
    sleep 30
done } &

MONITOR_PID=$!

# Run build
echo "Starting build with monitoring..."
./build.sh --debug --jobs=2

# Stop monitoring
kill $MONITOR_PID

# Analyze logs
echo "Build completed. Analyzing logs..."

# Generate performance report
python3 << 'EOF'
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read memory usage
if os.path.exists('memory_usage.log'):
    memory_df = pd.read_csv('memory_usage.log', sep='\s+', header=None,
                           names=['pid', 'ppid', 'cmd', 'mem', 'cpu'])
    memory_df = memory_df[memory_df['pid'].notna()]
    
    print("Memory usage statistics:")
    print(f"  Peak memory: {memory_df['mem'].max():.1f}%")
    print(f"  Average memory: {memory_df['mem'].mean():.1f}%")
    
    # Plot memory usage over time
    plt.figure(figsize=(10, 6))
    memory_df['time'] = range(len(memory_df))
    plt.plot(memory_df['time'], memory_df['mem'], alpha=0.7)
    plt.title('Memory Usage During Build')
    plt.xlabel('Time (30-second intervals)')
    plt.ylabel('Memory Usage (%)')
    plt.savefig('memory_usage_plot.png')
    plt.close()
EOF

echo "Performance report saved to memory_usage_plot.png"
```

## Testing and Validation

### Installation Testing

```python
# test_installation.py
import sys
import subprocess
import platform

def test_python_version():
    """Test Python version compatibility."""
    import platform
    version = platform.python_version()
    major, minor, _ = map(int, version.split('.'))
    
    if major < 3 or (major == 3 and minor < 8):
        print(f"❌ Python {version} is not supported. Minimum Python 3.8 required.")
        return False
    
    print(f"✓ Python {version} is compatible")
    return True

def test_pynlm_import():
    """Test that pynlm can be imported."""
    try:
        import pynlm
        print(f"✓ pynlm imported successfully (version: {pynlm.__version__})")
        return True
    except ImportError as e:
        print(f"❌ Failed to import pynlm: {e}")
        return False

def test_basic_functionality():
    """Test basic NLM functionality."""
    try:
        import pynlm
        
        # Test configuration
        config = pynlm.createDefaultConfig()
        print("✓ Configuration created")
        
        # Test brain
        brain = pynlm.createBrain(config)
        print("✓ Brain created")
        
        brain.initialize()
        print("✓ Brain initialized")
        
        brain.step(0)
        print("✓ Brain step executed")
        
        # Test world
        world = pynlm.createSimpleWorld()
        print("✓ World created")
        
        # Test agent
        agent = pynlm.createAgentBrain(brain)
        print("✓ Agent created")
        
        print("✓ All basic functionality tests passed")
        return True
    except Exception as e:
        print(f"❌ Basic functionality test failed: {e}")
        return False

def run_complete_test_suite():
    """Run complete test suite."""
    print("Running NLM installation test suite...")
    
    tests = [
        ("Python Version", test_python_version),
        ("pynlm Import", test_pynlm_import),
        ("Basic Functionality", test_basic_functionality),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n--- Testing: {test_name} ---")
        if test_func():
            passed += 1
        else:
            failed += 1
    
    print(f"\n=== Test Results ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    return failed == 0

if __name__ == "__main__":
    success = run_complete_test_suite()
    sys.exit(0 if success else 1)
```

### CI/CD Integration

#### GitHub Actions

```yaml
# .github/workflows/build.yml
name: NLM Build and Test

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        python-version: ['3.8', '3.9', '3.10', '3.11', '3.12']
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install system dependencies
      run: |
        if [[ "$RUNNER_OS" == "Linux" ]]; then
          sudo apt-get update
          sudo apt-get install -y cmake g++
        elif [[ "$RUNNER_OS" == "macOS" ]]; then
          brew install cmake
        fi
    
    - name: Install Python dependencies
      run: |
        python -m pip install --upgrade pip
        pip install scikit-build-core pybind11 numpy
        pip install -e .
    
    - name: Run tests
      run: |
        pytest tests/ -v --tb=short
    
    - name: Run installation tests
      run: |
        python test_installation.py
    
    - name: Upload test results
      if: always()
      uses: actions/upload-artifact@v3
      with:
        name: test-results-${{ matrix.os }}-python-${{ matrix.python-version }}
        path: test-results/
```

## Conclusion

This enhanced build and installation guide provides:

1. **Multiple installation options** for different use cases
2. **Advanced build configuration** with custom options
3. **Containerized deployment** for consistency
4. **Development workflow automation** for productivity
5. **Comprehensive testing** for reliability
6. **Troubleshooting guides** for quick problem resolution
7. **CI/CD integration** for automated validation
8. **Performance monitoring** for optimization

The enhanced installation process makes NLM easier to use while maintaining the power and flexibility of the C++ backend implementation. These improvements significantly reduce the barrier to entry for new users while providing advanced options for experienced developers.

## License

MIT License
