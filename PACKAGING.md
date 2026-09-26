# Build System and Packaging Improvements

## Build System Enhancements

### 1. Modern CMake Configuration
**Location**: `CMakeLists.txt` and build scripts

#### 1.1 C++20 Standard Compliance
```cmake
# Modern C++20 features with cross-platform support
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific optimizations
if(MSVC)
    add_compile_options(/W4 /permissive- /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-march=native -mtune=native)
endif()
```

#### 1.2 Package Configuration
```cmake
# Comprehensive package configuration
include(GNUInstallDirs)

# Python bindings integration
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Installation targets
set(INSTALL_CONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/nlm)

install(TARGETS nlm nlm_core nlm_agent nlm_world
    EXPORT nlmTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT nlmTargets
    FILE nlmConfig.cmake
    NAMESPACE nlm::
    DESTINATION ${INSTALL_CONFIG_DIR}
)
```

### 2. Python Package Integration
**Location**: `pyproject.toml` and setup scripts

#### 2.1 Modern Python Packaging
```toml
[build-system]
requires = [
    "scikit-build-core>=0.5.0",
    "pybind11>=2.11.0",
    "numpy>=1.20",
    "pytest>=7.0",
    "pytest-benchmark>=3.2.3"
]
build-backend = "scikit_build_core.build"

[project]
name = "pynlm"
version = "0.1.0"
description = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
readme = "README.md"
license = {text = "MIT"}
requires-python = ">=3.8"
authors = [
    {name = "NLM Authors", email = "contact@nlm-project.org"}
]
keywords = ["neural", "simulation", "spiking", "brain", "learning", "AI"]
classifiers = [
    "Development Status :: 3 - Alpha",
    "Intended Audience :: Science/Research",
    "Intended Audience :: Developers",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Programming Language :: C++",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Topic :: Software Development :: Libraries :: Python Modules",
]
dependencies = []

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "pytest-benchmark>=3.2.3",
    "numpy>=1.20",
    "matplotlib>=3.5",
    "sphinx>=4.3",
    "sphinx-rtd-theme>=0.5",
    "black>=21.12b0",
    "isort>=5.10",
    "mypy>=0.910",
    "clang-format>=13.0",
    "pre-commit>=2.15"
]

[project.urls]
Homepage = "https://github.com/nlm-project/nlm"
Documentation = "https://nlm.readthedocs.io"
Repository = "https://github.com/nlm-project/nlm.git"
"Issue Tracker" = "https://github.com/nlm-project/nlm/issues"
"CI/CD" = "https://github.com/nlm-project/nlm/actions"
"Discord" = "https://discord.gg/nlm"
"Twitter" = "https://twitter.com/nlm_project"

[tool.scikit-build]
cmake.minimum-version = "3.16"
cmake.build-type = "Release"
cmake.args = [
    "-DCMAKE_CXX_STANDARD=20",
    "-DCMAKE_CXX_STANDARD_REQUIRED=ON",
    "-DPYTHON_EXECUTABLE={python}",
    "-DBUILD_SHARED_LIBS=OFF",  # Static libraries for stability
    "-DNLM_ENABLE_TESTS=ON",
    "-DNLM_ENABLE_BENCHMARKS=ON",
]

[tool.scikit-build.cmake.define]
NLM_SOURCE_DIR = "src"
NLM_BUILD_TESTS = ON
NLM_BUILD_BENCHMARKS = ON
NLM_PYTHON_BINDINGS = ON

[tool.pytest.ini_options]
testpaths = ["tests"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
markers = [
    "slow: marks tests as slow (deselect by default)",
    "integration: marks integration tests",
    "performance: marks performance benchmarks",
    "benchmark: marks benchmark tests"
]
addopts = [
    "-v",
    "--tb=short",
    "--timeout=300",
    "-m "
    "not slow"
]
```

#### 2.2 Build Scripts
**File**: `scripts/build.py`
```python
#!/usr/bin/env python3
"""
NLM Build Automation Script

Comprehensive build system for NLM with multiple build configurations
and validation steps.
"""

import argparse
import subprocess
import sys
import os
import platform
from pathlib import Path

class NLMBuilder:
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.build_dir = self.project_root / "build"
        self.dist_dir = self.project_root / "dist"
        
    def build_cmake(self, config="Release", parallel=True, tests=True):
        """Build using CMake"""
        print(f"Building NLM with CMake ({config})...")
        
        build_args = [
            "cmake",
            "..",
            f"-DCMAKE_BUILD_TYPE={config}",
            "-DCMAKE_CXX_STANDARD=20",
            "-DPYTHON_EXECUTABLE=$(which python)",
            "-DNLM_ENABLE_TESTS=ON",
            "-DNLM_ENABLE_BENCHMARKS=ON",
        ]
        
        if platform.system() == "Windows":
            build_args.extend(["-G", "Ninja"])
        
        # Configure
        subprocess.run(["cmake"] + build_args, cwd=self.build_dir, check=True)
        
        # Build
        cmd = ["cmake", "--build", "."]
        if parallel:
            cmd.extend(["--parallel", "$(nproc)"])
            
        subprocess.run(cmd, cwd=self.build_dir, check=True)
        
        if tests:
            self.run_tests()
    
    def build_python(self, develop=False, editable=False):
        """Build Python bindings"""
        print("Building Python bindings...")
        
        cmd = ["pip", "install", "."]
        if editable or develop:
            cmd.append("-e")
            
        subprocess.run(cmd, cwd=self.project_root, check=True)
    
    def run_tests(self, coverage=False, benchmarks=False):
        """Run tests and benchmarks"""
        print("Running tests...")
        
        # Run C++ tests
        if (self.build_dir / "nlm_test").exists():
            subprocess.run([str(self.build_dir / "nlm_test")], check=True)
        
        # Run Python tests
        subprocess.run(["pytest", "-v", "tests/"], cwd=self.project_root, check=True)
        
        if benchmarks:
            self.run_benchmarks()
        
        if coverage:
            self.generate_coverage()
    
    def run_benchmarks(self):
        """Run performance benchmarks"""
        print("Running benchmarks...")
        
        # Build and run benchmarks if they exist
        bench_dir = self.build_dir / "benchmarks"
        if bench_dir.exists():
            subprocess.run(["make", "all"], cwd=bench_dir, check=True)
            
            # Run benchmarks
            for bench in bench_dir.glob("*benchmark*"):
                if bench.is_file() and os.access(bench, os.X_OK):
                    subprocess.run([str(bench)], check=True)
    
    def generate_coverage(self):
        """Generate test coverage reports"""
        print("Generating coverage reports...")
        
        # Python coverage
        subprocess.run([
            "coverage", "run", "-m", "pytest", "tests/",
            "&&", "coverage", "report", "-m", "--include="
            "*/src/*"
        ], cwd=self.project_root, shell=True, check=True)
        
        # C++ coverage (if available)
        if (self.build_dir / "coverage"):
            subprocess.run([
                "cd", str(self.build_dir),
                "&&", "lcov", "--capture", "--directory", ".",
                "--output-file", "coverage.info",
                "&&", "genhtml", "coverage.info",
                "--output-directory", "coverage-report"
            ], shell=True, check=True)
    
    def install(self, prefix=None):
        """Install NLM system-wide"""
        print("Installing NLM...")
        
        install_cmd = ["cmake", "--install", "."]
        if prefix:
            install_cmd.extend(["--prefix", prefix])
            
        subprocess.run(install_cmd, cwd=self.build_dir, check=True)
        
        # Install Python package
        subprocess.run(["pip", "install", "."], cwd=self.project_root, check=True)
    
    def clean(self):
        """Clean build artifacts"""
        print("Cleaning build artifacts...")
        
        # Remove build directory
        import shutil
        if self.build_dir.exists():
            shutil.rmtree(self.build_dir)
        
        # Remove dist directory
        if self.dist_dir.exists():
            shutil.rmtree(self.dist_dir)
        
        # Remove Python cache files
        for pycache in self.project_root.rglob("__pycache__"):
            shutil.rmtree(pycache)
        
        for pyc in self.project_root.rglob("*.pyc"):
            pyc.unlink()
    
    def validate_build(self):
        """Validate the build by running smoke tests"""
        print("Validating build...")
        
        # Check if key executables exist
        executables = [
            "nlm",
            "nlm_phase3_demo",
            "nlm_phase4_demo",
            "nlm_test"
        ]
        
        for exe in executables:
            exe_path = self.build_dir / exe
            if not exe_path.exists():
                print(f"Warning: Expected executable not found: {exe}")
        
        # Test Python import
        try:
            import pynlm
            print(f"Python bindings version: {pynlm.__version__}")
        except ImportError as e:
            print(f"Error importing pynlm: {e}")
            raise

def main():
    parser = argparse.ArgumentParser(description="NLM Build Automation Script")
    
    parser.add_argument(
        "--build-type", choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
        default="Release", help="Build type"
    )
    
    parser.add_argument(
        "--parallel", action="store_true", default=True,
        help="Use parallel build"
    )
    
    parser.add_argument(
        "--tests", action="store_true", default=True,
        help="Run tests after build"
    )
    
    parser.add_argument(
        "--python", action="store_true", default=True,
        help="Build Python bindings"
    )
    
    parser.add_argument(
        "--install", action="store_true", default=False,
        help="Install after build"
    )
    
    parser.add_argument(
        "--clean", action="store_true", default=False,
        help="Clean before building"
    )
    
    parser.add_argument(
        "--validate", action="store_true", default=False,
        help="Validate build after completion"
    )
    
    parser.add_argument(
        "--prefix", type=str, default=None,
        help="Installation prefix"
    )
    
    args = parser.parse_args()
    
    builder = NLMBuilder()
    
    if args.clean:
        builder.clean()
    
    builder.build_dir.mkdir(exist_ok=True)
    
    if args.build_type and args.build_type.lower() != "cmake":
        builder.build_cmake(args.build_type, args.parallel, args.tests)
    
    if args.python:
        builder.build_python()
    
    if args.install:
        builder.install(args.prefix)
    
    if args.validate:
        builder.validate_build()
    
    print("Build completed successfully!")

if __name__ == "__main__":
    main()
```

### 3. Development Tools Integration
**File**: `tools/cmake_format.py`
```python
#!/usr/bin/env python3
"""
CMake Format and Validation Tool

Automates CMake formatting and validation for the NLM project.
"""

import subprocess
import sys
from pathlib import Path

def format_cmake_files():
    """Format CMake files using cmake-format"""
    print("Formatting CMake files...")
    
    cmake_files = Path(__file__).parent.parent.rglob("*.cmake")
    cmake_files = list(cmake_files) + list(Path(__file__).parent.parent.glob("CMakeLists.txt"))
    
    for cmake_file in cmake_files:
        try:
            result = subprocess.run(
                ["cmake-format", "--in-place", str(cmake_file)],
                check=True, capture_output=True, text=True
            )
            print(f"Formatted: {cmake_file}")
        except subprocess.CalledProcessError:
            print(f"Warning: Failed to format {cmake_file}")
            print(f"Error: {result.stderr}")

def validate_cmake_files():
    """Validate CMake files"""
    print("Validating CMake files...")
    
    cmake_files = Path(__file__).parent.parent.rglob("*.cmake")
    cmake_files = list(cmake_files) + list(Path(__file__).parent.parent.glob("CMakeLists.txt"))
    
    for cmake_file in cmake_files:
        try:
            result = subprocess.run(
                ["cmake", "-P", str(cmake_file)],
                cwd=cmake_file.parent, check=True, capture_output=True, text=True
            )
            print(f"Valid: {cmake_file}")
        except subprocess.CalledProcessError:
            print(f"Error validating {cmake_file}")
            print(f"Output: {result.stdout}")
            print(f"Error: {result.stderr}")
            return False
    
    return True

if __name__ == "__main__":
    format_cmake_files()
    if not validate_cmake_files():
        sys.exit(1)
    print("CMake validation completed successfully!")
```

### 4. Continuous Integration Configuration
**File**: `.github/workflows/ci.yml`
```yaml
name: NLM Continuous Integration

on:
  push:
    branches: [main, develop]
    tags: ['v*']
  pull_request:
    branches: [main, develop]

env:
  PYTHON_VERSION: '3.11'
  CMAKE_BUILD_TYPE: 'Release'
  CI: 'true'

jobs:
  test:
    name: Test (${{ matrix.os }})
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        include:
          - os: ubuntu-latest
            python-version: '3.11'
            cpp-compiler: 'g++'
          - os: windows-latest
            python-version: '3.11'
            cpp-compiler: 'msvc'
          - os: macos-latest
            python-version: '3.11'
            cpp-compiler: 'clang++'
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Set up Python ${{ env.PYTHON_VERSION }}
      uses: actions/setup-python@v4
      with:
        python-version: ${{ env.PYTHON_VERSION }}
    
    - name: Set up C++ compiler
      if: matrix.os == 'ubuntu-latest'
      run: |
        sudo update-apt
        sudo apt-get install -y g++ cmake build-essential
    
    - name: Set up C++ compiler (Windows)
      if: matrix.os == 'windows-latest'
      run: |
        choco install visualstudio2019buildtools
        choco install ninja
    
    - name: Set up C++ compiler (macOS)
      if: matrix.os == 'macos-latest'
      run: |
        brew install cmake ninja
    
    - name: Install Python dependencies
      run: |
        python -m pip install --upgrade pip
        pip install scikit-build-core pybind11 pytest numpy
        if [ -f requirements.txt ]; then pip install -r requirements.txt; fi
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. 
          -DCMAKE_BUILD_TYPE=${{ env.CMAKE_BUILD_TYPE }}
          -DPYTHON_EXECUTABLE=$(which python)
          -DNLM_ENABLE_TESTS=ON
          -DNLM_ENABLE_BENCHMARKS=ON
    
    - name: Build
      run: |
        cd build
        cmake --build . --parallel $(nproc)
    
    - name: Run C++ tests
      run: |
        cd build
        ./nlm_test
    
    - name: Run Python tests
      run: |
        pytest tests/ -v --tb=short
    
    - name: Run benchmarks
      run: |
        cd build
        ./benchmarks/nlm_benchmarks
    
    - name: Generate coverage reports
      run: |
        cd build
        lcov --capture --directory . --output-file coverage.info
        genhtml coverage.info --output-directory coverage-report
    
    - name: Upload coverage reports
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: coverage-reports-${{ matrix.os }}
        path: build/coverage-report
    
    - name: Upload test results
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: test-results-${{ matrix.os }}
        path: |
          build/test-results.xml
          tests/results/
    
    - name: Publish Test Results
      uses: EnricoSolidi/action-publish-test-results@v1
      if: always()
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        report_paths: build/test-results.xml

  package:
    name: Package Distribution
    runs-on: ubuntu-latest
    needs: test
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Set up Python ${{ env.PYTHON_VERSION }}
      uses: actions/setup-python@v4
      with:
        python-version: ${{ env.PYTHON_VERSION }}
    
    - name: Set up build dependencies
      run: |
        python -m pip install --upgrade pip
        pip install build scikit-build-core pybind11
    
    - name: Build Python package
      run: |
        python -m build --wheel --sdist
        pip install dist/*.whl
    
    - name: Test installation
      run: |
        python -c "import pynlm; print(f'NLM version: {pynlm.__version__}')"
    
    - name: Create package archive
      run: |
        mkdir -p dist/archive
        cp dist/*.whl dist/archive/
        cp dist/*.tar.gz dist/archive/
        tar -czvf dist/nlm-${{ github.sha }}.tar.gz -C dist/archive .
    
    - name: Upload packages
      uses: actions/upload-artifact@v3
      with:
        name: nlm-packages
        path: dist/
    
    - name: Publish to TestPyPI
      if: github.ref == 'refs/heads/develop'
      run: |
        pip install twine
        twine upload --repository testpypi dist/*.whl
    
    - name: Publish to PyPI
      if: startsWith(github.ref, 'refs/tags/v')
      run: |
        pip install twine
        twine upload dist/*.whl
```

### 5. Docker Configuration
**File**: `Dockerfile`
```dockerfile
FROM ubuntu:22.04

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev \
    libpython3-dev \
    pkg-config \
    wget \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Set Python version
ENV PYTHON_VERSION=3.11
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.11 1

# Install Python dependencies
COPY pyproject.toml requirements.txt* /tmp/
RUN pip3 install --upgrade pip && \
    pip3 install scikit-build-core pybind11 numpy pytest

# Copy project files
WORKDIR /app
COPY . /app

# Build NLM
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3) && \
    cmake --build . --parallel $(nproc)

# Install Python package
RUN pip3 install -e .

# Create test user
RUN useradd -m -u 1000 appuser && chown -R appuser /app
USER appuser

# Default command
CMD ["bash"]
```

### 6. Development Environment Setup
**File**: `dev.env`
```bash
# Development Environment Variables

# Python configuration
PYTHONPATH=/workspace/nlm/src:$PYTHONPATH
PYTHONWARNINGS=all
PYTHONVERBOSE=1

# Build configuration
NLM_BUILD_TYPE=Debug
NLM_ENABLE_TESTS=ON
NLM_ENABLE_BENCHMARKS=ON
NLM_PYTHON_BINDINGS=ON

# Performance settings
NLM_NUM_THREADS=auto
NLM_VECTORIZATION=ON
NLM_MEMORY_POOL=ON

# Logging configuration
NLM_LOG_LEVEL=INFO
NLM_LOG_FILE=/workspace/nlm/logs/nlm.log
NLM_LOG_FORMAT=json

# Performance monitoring
NLM_ENABLE_MONITORING=ON
NLM_PERFORMANCE_THRESHOLD=0.9
NLM_MEMORY_THRESHOLD=80

# Development tools
CMAKE_FORMAT=true
CLANG_FORMAT=true
MYPY=true
BLACK=true
ISORT=true

# Testing configuration
PYTEST_COVERAGE=true
PYTEST_BENCHMARKS=true
PYTEST_SLOW_TESTS=true
NLM_TEST_TIMEOUT=300

# Feature flags
NLM_FEATURE_ADVANCED_MEMORY=ON
NLM_FEATURE_HIERARCHICAL_AGENTS=ON
NLM_FEATURE_ADVANCED_VISION=ON
NLM_FEATURE_PREDICTIVE_CODING=ON
```

### 7. Package Manifest
**File**: `PACKAGING.md`
```markdown
# NLM Packaging and Distribution

## Supported Platforms

### Linux
- **Ubuntu/Debian**: `apt install python3-nlm`
- **RPM-based**: `yum install nlm` or `dnf install nlm`
- **Flatpak**: `flatpak install flathub org.nlm.nlm`

### macOS
- **Homebrew**: `brew install nlm`
- **MacPorts**: `sudo port install nlm`

### Windows
- **Chocolatey**: `choco install nlm`
- **Scoop**: `scoop install nlm`
- **Windows Store**: `Get NLM from Microsoft Store`

### Web-based
- **WebAssembly**: `pip install pynlm-wasm`
- **Docker**: `docker run -p 8080:8080 nlm/nlm:latest`
- **Binder**: `https://mybinder.org/v2/gh/nlm-project/nlm/main`

## Installation Methods

### 1. Package Manager
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install python3-nlm

# macOS with Homebrew
brew install nlm

# Windows with Chocolatey
choco install nlm
```

### 2. pip Installation
```bash
# Standard pip installation
pip install pynlm

# Development installation
pip install -e .

# With optional dependencies
pip install pynlm[dev]

# Latest development version
pip install git+https://github.com/nlm-project/nlm.git@main
```

### 3. System Package
```bash
# Build from source
sudo apt build-dep nlm
sudo apt install nlm
```

## Package Contents

### Core Package
- `pynlm/` - Python bindings module
- `nlm/` - Compiled libraries and executables
- `include/` - C++ header files
- `docs/` - Documentation

### Development Package
- `src/` - Complete source code
- `tests/` - Test suite
- `benchmarks/` - Performance benchmarks
- `examples/` - Example applications

### Documentation Package
- `README.md` - Main documentation
- `docs/` - Technical documentation
- `examples/` - Usage examples
- `tutorials/` - Learning materials

## Dependency Management

### Runtime Dependencies
- Python 3.8+
- NumPy (optional)
- System C++ libraries (for native extensions)

### Development Dependencies
- CMake 3.16+
- Python 3.8+
- Build tools (make, ninja)
- Testing framework (pytest)
- Documentation tools (sphinx)
- Code formatting tools (black, isort)

## System Requirements

### Minimum Requirements
- CPU: 64-bit processor (x86_64 or ARM64)
- Memory: 2GB RAM minimum
- Disk Space: 500MB for installation
- Python: 3.8 or later

### Recommended Requirements
- CPU: Multi-core processor (8+ cores recommended)
- Memory: 8GB RAM or more
- Disk Space: 2GB for development
- GPU: Optional for accelerated computations

## Post-Installation Setup

### 1. Environment Configuration
```bash
# Add NLM to PATH
export PATH=/usr/local/lib/nlm/bin:$PATH

# Set Python module path
export PYTHONPATH=/usr/local/lib/python3.11/site-packages:$PYTHONPATH

# Initialize NLM configuration
nlm-config --init
```

### 2. Basic Usage
```python
import pynlm

# Verify installation
print(f"NLM version: {pynlm.__version__}")

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a basic simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### 3. Development Environment
```bash
# Set up development environment
cd /workspace/nlm

# Install development dependencies
pip install -e .[dev]

# Generate documentation
sphinx-build docs/ docs/_build/

# Run tests
pytest tests/ -v --coverage

# Format code
black src/ tests/
isort src/ tests/

# Type checking
mypy src/ tests/
```

## Troubleshooting

### Common Issues

#### Issue 1: Python Module Not Found
```bash
# Solution: Check PYTHONPATH
export PYTHONPATH=/usr/local/lib/python3.11/site-packages:$PYTHONPATH

# Or reinstall
pip install --force-reinstall pynlm
```

#### Issue 2: C++ Libraries Not Found
```bash
# Solution: Check library path
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# On macOS
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

#### Issue 3: Build Errors
```bash
# Solution: Update build tools
sudo apt update && sudo apt install build-essential cmake

# Or rebuild from scratch
rm -rf build && mkdir build && cd build && cmake .. && make
```

## Support and Maintenance

### Support Channels
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discord Server**: https://discord.gg/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Email**: support@nlm-project.org

### Maintenance Tasks

#### Monthly
- [ ] Update dependencies
- [ ] Run full test suite
- [ ] Check for security vulnerabilities
- [ ] Update documentation

#### Quarterly
- [ ] Release new version
- [ ] Update CI/CD pipelines
- [ ] Performance benchmarking
- [ ] Bug triage and fixes

#### Annually
- [ ] Major version bump
- [ ] Platform compatibility testing
- [ ] Architecture review
- [ ] Strategic planning

## License and Copyright

### License
MIT License

Copyright (c) 2026 NLM Project Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

## Implementation Strategy

### Phase 1: Build System Foundation (Weeks 1-2)
1. Modernize CMake configuration
2. Add comprehensive build scripts
3. Set up GitHub Actions CI/CD
4. Create Docker and development environments

### Phase 2: Packaging and Distribution (Weeks 3-4)
1. Complete Python package configuration
2. Add package management support
3. Create comprehensive documentation
4. Set up testing and validation infrastructure

### Phase 3: Development Tools (Weeks 5-6)
1. Add code formatting tools
2. Implement testing frameworks
3. Create development environment setup
4. Add performance monitoring tools

### Phase 4: Integration and Validation (Weeks 7-8)
1. Comprehensive testing across platforms
2. Performance benchmarking
3. Documentation generation
4. Release preparation