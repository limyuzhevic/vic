# Development Environment

## Overview

This document provides comprehensive instructions for setting up and maintaining the NLM (熙然) development environment. It covers operating system setup, development tools, build configuration, and development practices.

## Supported Platforms

### 1. Primary Development Platform

**Operating Systems**:
- Linux distributions (Ubuntu, Debian, CentOS)
- macOS (latest versions)
- Windows (with WSL or Git Bash)

**Hardware Requirements**:
- CPU: x86_64 architecture (ARM64 support available)
- Memory: 4+ GB RAM (16+ GB recommended for full development)
- Storage: 1+ GB free space for build artifacts
- Display: Optional (for documentation viewing)

**Minimum Recommended Specs**:
- CPU: Quad-core processor or better
- RAM: 8 GB minimum, 16 GB recommended
- SSD: Preferred for faster build times

### 2. Secondary Development Platforms

**Container-based Development**:
- Docker (with NVIDIA Container Toolkit for GPU acceleration)
- Singularity (for HPC environments)

**Cloud Development**:
- AWS EC2 instances
- Google Cloud Platform VMs
- Microsoft Azure Virtual Machines

## Development Setup

### 1. System Preparation

#### Linux (Ubuntu/Debian)

```bash
# Update package lists
sudo apt update

# Install essential development tools
sudo apt install -y build-essential cmake git vim curl wget

# Install additional dependencies for NLM
sudo apt install -y libomp-dev libjsoncpp-dev libboost-all-dev

# Install documentation generation tools
sudo apt install -y doxygen graphviz plantuml

# Set up development directories
mkdir -p ~/nlm/{src,build,docs,configs,scripts}
cd ~/nlm
```

#### macOS

```bash
# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Homebrew packages
brew install cmake git vim curl wget omp
brew install doxygen graphviz plantuml

# Install NLM-specific dependencies
brew install jsoncpp boost

# Set up development directory
mkdir -p ~/nlm/{src,build,docs,configs,scripts}
cd ~/nlm
```

#### Windows (WSL2)

```bash
# Install WSL2 if not present
wsl --install

# After installing WSL2, open Ubuntu terminal and run:
# (Same commands as Linux above)
```

### 2. Environment Configuration

#### Shell Configuration

**Bash Profile** (`~/.bashrc`):
```bash
# NLM environment variables
export NLM_ROOT="$HOME/nlm"
export NLM_BUILD_DIR="$NLM_ROOT/build"
export NLM_SOURCE_DIR="$NLM_ROOT/src"
export NLM_DOCS_DIR="$NLM_ROOT/docs"
export NLM_CONFIG_DIR="$NLM_ROOT/configs"

# CMake configuration
export CMAKE_PREFIX_PATH="$NLM_BUILD_DIR;$CMAKE_PREFIX_PATH"
export PKG_CONFIG_PATH="$NLM_BUILD_DIR/lib/pkgconfig:$PKG_CONFIG_PATH"

# Python for documentation generation
export PATH="$HOME/.local/bin:$PATH"
```

**PowerShell Profile** (`$PROFILE`):
```powershell
# NLM environment variables
$env:NLM_ROOT = "$env:USERPROFILE\nlm"
$env:NLM_BUILD_DIR = "$env:NLM_ROOT\build"
$env:NLM_SOURCE_DIR = "$env:NLM_ROOT\src"
$env:NLM_DOCS_DIR = "$env:NLM_ROOT\docs"
$env:NLM_CONFIG_DIR = "$env:NLM_ROOT\configs"

# Add CMake to PATH
$env:PATH = "$env:NLM_BUILD_DIR;C:$env:PATH"
```

#### IDE Configuration

**Visual Studio Code Setup**:
```json
// .vscode/settings.json
cmd.c_cpp.intelliSenseEngine = "protocomplete"
cmd.c_cpp.errorSquiggles = "enabled"
cmd.c_cpp.suggest.SuggestFunctionArguments = true
cmd.c_cpp.llvm.diagnostics = "clangd"
cpp.syntaxHighlighting.overrideBrackets = true
cpp.commentStyle = "legacy"

// .vscode/tasks.json
tasks:
  - label: "Build NLM"
    type: shell
    command: "${workspaceFolder}/build.sh"
    group: build
    problemMatcher: "$msCompile"

  - label: "Run Tests"
    type: shell
    command: "cd build && ctest --output-on-failure"
    group: test
    problemMatcher: "$msTest"

  - label: "Generate Documentation"
    type: shell
    command: "cd build && make docs"
    group: build
    problemMatcher: "$msCompile"
```

**CLion Setup**:
```xml
<!-- .idea/cmake.xml -->
<project version="4">
  <component name="CMakeWorkspace">
    <option name="cmakeExecutablePath" value="/usr/local/bin/cmake" />
    <option name="generatorName" value="Unix Makefiles" />
    <option name="builder">CMakeQMakeRunner</option>
  </component>
</project>

<!-- .idea/vcs.xml -->
<project version="4">
  <component name="VcsManager">
    <GIT path="$PROJECT_DIR$" />
  </component>
</project>

<!-- .idea/dataSources.xml -->
<project version="4">
  <component name="DataSourceManager">
    <dataSource name="NLM" url="file://$PROJECT_DIR$/src" />
  </component>
</project>
```

### 3. Source Code Management

#### Git Configuration

**Global Git Settings**:
```bash
# Configure user information (if not already configured)
git config --global user.name "NLM Development Team"
git config --global user.email "dev@nlm.ai"

# Configure line endings (Linux/macOS)
git config --global core.autocrlf false

# Configure branch naming rules
git config --global init.defaultBranch main

# Enable verbose logging for troubleshooting
git config --global log.pretty oneline
git config --global log.abbrev 8
```

**Repository Structure**:
```bash
# Create NLM repository
mkdir nlm
cd nlm

# Initialize git repository
git init
git remote add origin https://github.com/nlm/nlm.git

# Create initial structure
mkdir -p src/{brain,dynamics,plasticity,neuromodulation,memory,prediction,cognition,sensory,motor,environment,experiments}

mkdir -p tests
mkdir -p docs/{user-manual,tutorial,basic,advanced,api-reference,technical,reference,supporting}

mkdir -p configs
mkdir -p docs/user-manual
mkdir -p docs/tutorial
mkdir -p docs/api-reference
mkdir -p docs/technical
mkdir -p docs/reference
mkdir -p docs/supporting

# Add initial files
touch README.md LICENSE CONTRIBUTING.md DEVELOPING.md RUNNING.md

# Create .gitignore file
cat > .gitignore << 'EOF'
# Build directories
build/
cmake-build*

# IDE files
.idea/
.vscode/
*.swp
*.swo

# macOS
.DS_Store

# Windows
*.nt
*.exe

# Python
__pycache__/
*.pyc

# Documentation output
html/
latex/
docs/api-reference/html/

# Temporary files
*.tmp
*.temp

# Log files
*.log
EOF

# Add and commit initial files
git add .
git commit -m "Initial NLM project structure

- Core source directories
- Documentation structure
- Basic project configuration

Co-authored-by: OpenHands <openhands@all-hands.dev>"
```

#### Branching Strategy

**Feature Branch Workflow**:
```bash
# Start with main branch
git checkout main
git pull origin main

# Create feature branch
git checkout -b feature/neuromodulation-integration

# Make changes
git add src/neuromodulation/
git commit -m "feat(neuromodulation): Add dopamine and curiosity systems"

# Push feature branch
git push origin feature/neuromodulation-integration

# Create pull request
# Review and merge into main
```

**Release Branch Workflow**:
```bash
# Create release branch from main
git checkout -b release/v1.0.0

# Make release-specific changes
git add README.md

# Build and test the release
git commit -m "release: Prepare for version 1.0.0"
git push origin release/v1.0.0

# Merge into main after successful release
# git checkout main
# git merge --no-ff release/v1.0.0
# git tag -a v1.0.0 -m "Version 1.0.0"
```

## Build Environment Setup

### 1. Build System Configuration

#### CMake Configuration

**Minimum CMake Version**: 3.10
**Recommended Tools**: Ninja (faster build)

**CMake Options**:
```bash
# Standard build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Development build with debugging
mkdir debug-build
cd debug-build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON -DENABLE_DOXYGEN=ON

# Performance-optimized build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DMINIMAL_BUILD=OFF \
          -DOPTIMIZE_FOR_PERFORMANCE=ON -DVECTORIZATION=ON

# GPU-accelerated build (if available)
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_CUDA=ON -DUSE_HIP=ON
```

**CMake Configuration File**:
```cmake
# CMake configuration for NLM project
cmake_minimum_required(VERSION 3.10)
project(NLM VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    add_definitions(-DDEBUG -D_DEBUG)
else()
    set(CMAKE_BUILD_TYPE Release)
    add_definitions(-DNDEBUG)
endif()

# Compiler optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /arch:AVX2")
endif()

# Parallel build support
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

# Installation paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find dependencies
find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)
find_package(JSON REQUIRED)

# Include NLM source directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

# Add source files
file(GLOB_RECURSE NLM_SOURCES "src/*.cpp" "src/*/*.cpp")

# Create executable
add_executable(nlm ${NLM_SOURCES})

# Link libraries
target_link_libraries(nlm PRIVATE ${CMAKE_THREAD_LIBS_INIT} OpenMP::OpenMP_CXX)

# Install targets
install(TARGETS nlm DESTINATION bin)

# Documentation generation
if(ENABLE_DOXYGEN)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()
```

#### Build Scripts

**Build Script** (`build.sh`):
```bash
#!/bin/bash
# build.sh - NLM build script

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

# Parse command line arguments
BUILD_TYPE="Release"
ENABLE_TESTS=true
ENABLE_DOXYGEN=true
NINJA=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --no-tests)
            ENABLE_TESTS=false
            shift
            ;;
        --no-docs)
            ENABLE_DOXYGEN=false
            shift
            ;;
        --ninja)
            NINJA=true
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --debug           Build in debug mode"
            echo "  --release         Build in release mode (default)"
            echo "  --no-tests        Disable tests"
            echo "  --no-docs         Disable documentation generation"
            echo "  --ninja          Use Ninja build system"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
if [ "$NINJA" = true ]; then
    cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
          -DENABLE_TESTS=$ENABLE_TESTS \
          -DENABLE_DOXYGEN=$ENABLE_DOXYGEN \
          -G "Ninja"
else
    cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
          -DENABLE_TESTS=$ENABLE_TESTS \
          -DENABLE_DOXYGEN=$ENABLE_DOXYGEN
fi

# Build the project
if [ "$NINJA" = true ]; then
    ninja -j$(nproc)
else
    make -j$(nproc)
fi

# Run tests if enabled
if [ "$ENABLE_TESTS" = true ]; then
    echo "Running tests..."
    ctest --output-on-failure
fi

# Generate documentation if enabled
if [ "$ENABLE_DOXYGEN" = true ]; then
    echo "Generating documentation..."
    make docs
fi

echo "Build completed successfully!"
```

**Quick Build Script** (`quick_build.sh`):
```bash
#!/bin/bash
# quick_build.sh - Quick build script for NLM development

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Quick build with default settings
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON -DENABLE_DOXYGEN=ON
make -j$(nproc)

# Run Phase 6 demo (integration test)
if [ -f "nlm_phase6_demo" ]; then
    echo "Running Phase 6 integration test..."
    ./nlm_phase6_demo
else
    echo "Phase 6 demo not found. Building examples..."
    # Build example experiments
    make nlm_phase6_demo
    ./nlm_phase6_demo
fi

echo "Quick build completed successfully!"
```

### 2. Testing Environment

#### Test Framework Setup

**Google Test Configuration**:
```cmake
# Enable Google Test
enable_testing()

# Add NLM tests
file(GLOB NLM_TEST_SOURCES "tests/*.cpp")
add_executable(nlm_tests ${NLM_TEST_SOURCES})
target_link_libraries(nlm_tests PRIVATE nlm gtest_main)

# Add test to CTest
add_test(NAME nlm_tests COMMAND nlm_tests)
```

**Test Configuration**:
```bash
# Run all tests
cd build
ctest

# Run specific test categories
ctest -R "test_neuron"
ctest -R "test_plasticity"
ctest -R "test_memory"
ctest -R "test_integration"

# Run with verbose output
ctest -V

# Generate test report
ctest --output-on-failure > test_report.txt
```

**Performance Testing**:
```bash
# Run performance benchmarks
./benchmark

# Run specific benchmark suites
./benchmark --suite=neural_computation
./benchmark --suite=memory_system
./benchmark --suite=plasticity
./benchmark --suite=prediction

# Profile performance
perf record -g ./nlm_phase6_demo
perf report
```

### 3. Documentation Environment

#### Documentation Tools

**Doxygen Configuration**:
```ini
# Doxyfile configuration
PROJECT_NAME           = "NLM -熙然"
PROJECT_NUMBER         = "1.0.0"
PROJECT_BRIEF          = "An Experimental Artificial Developmental Brain"
OUTPUT_DIRECTORY       = html
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
GENERATE_MAN           = NO
GENERATE_XML           = YES
HTML_OUTPUT            = html
XML_OUTPUT             = xml

INPUT                  = src/
FILE_PATTERNS          = *.hpp *.cpp
EXCLUDE_PATTERNS       = */tests/* */build/* */html/*

CLASS_DIAGRAMS        = YES
GRAPHICAL_HIERARCHY    = YES
INTERCLASS_GRAPH        = YES

ALPHABETICAL_INDEX     = YES
GENERATE_TOC_HEADINGS   = YES
INTERNAL_DOCS         = YES

EXAMPLE_PATH           = examples/
EXAMPLE_PATTERNS       = *.cpp
EXAMPLE_FILES         = *

INPUT_FILTER           =
FILTER_PATTERNS        = *.cpp
FILTER_SOURCE_FILES    = YES
FILTER_SOURCE_PATTERNS  = *.hpp

CPP_FILE_EXTENSIONS     = .c .cpp .cc .cxx .cppm .ixx .cpp.template
C_FILE_EXTENSIONS       = .c .cc .C
OBJDUMP_EXTENSIONS      = .obj
CXX_WRAPPER            =
IMPLICIT_INCLUDE_FILES = header.h

OPTIMIZE_OUTPUT_FOR_C  = NO

GENERATE_LAMARVE_XML   = NO
XML_PROGRAMLISTING    = NO

EXTENDED_SYNTAX       = NO
PYTHON_DOC          = NO

GENERATE_INLINEDOC    = NO
INLINE_SOURCES       = NO

GENERATE_HTML_STYLESHEET = YES
HTML_EXTRA_FILES       = style.css
GENERATE_DOCSET       = NO
DOCSET_FEEDNAME        = "Doxygen documentation"
DOCSET_FEEDDESCRIPTION = "Doxygen generated documentation for NLM."
DOCSET_PID_NAME       = org.kilo.NLM
DOCSET_PUBLISHER_URL  = http://www.kilo.ai/
DOCSET_PUBLISHER_NAME = "NLM Development Team"
DOCSET_PUBLISHER_EMAIL = "dev@nlm.ai"

GENERATE_QHP            = NO
QCH_FILE              = "nlm.qch.html"
QCH_TITLE              = "NLM Documentation"
QCH_BASE_URL           = http://www.kilo.ai/
QCH_COPYRIGHT         = "Copyright © 2026 NLM Development Team"

ENUM_VALUES_DOXYGEN   = YES
RTF_OUTPUT_FORMAT     = NO
GENERATE_RTF         = NO
COMPACT_LATEX         = NO
GENERATE_MANUALS     = NO
GENERATE_XML_INTERNAL = NO
```

**Static Site Generation**:
```yaml
# .jekyll.yml
name: NLM Documentation
description: NLM (熙然) - An Experimental Artificial Developmental Brain
url: https://docs.nlm.ai
baseurl: /

markdown: kramdown

default_locale: en-US
theme: minima

exclude:
  - README.md
  - CONTRIBUTING.md
  - DEVELOPING.md
  - RUNNING.md
```

#### Documentation Build Script

```bash
#!/bin/bash
# build_docs.sh - Build documentation for NLM

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DOCS_DIR="${PROJECT_ROOT}/docs"

# Generate API documentation from C++ code
cd "${PROJECT_ROOT}"

if command -v doxygen >/dev/null 2>&1; then
    echo "Generating API documentation with Doxygen..."
    
    # Create Doxyfile if it doesn't exist
    if [ ! -f "Doxyfile" ]; then
        doxygen -g
        # Configure Doxyfile
        sed -i 's/^PROJECT_NAME.*/PROJECT_NAME = "NLM -熙然"/' Doxyfile
        sed -i 's/^PROJECT_NUMBER.*/PROJECT_NUMBER = "1.0.0"/' Doxyfile
        sed -i 's/^GENERATE_HTML.*/GENERATE_HTML = YES/' Doxyfile
        sed -i 's/^HTML_OUTPUT.*/HTML_OUTPUT = html/' Doxyfile
        sed -i 's/^INPUT.*/INPUT = src/' Doxyfile
    fi
    
    doxygen Doxyfile
    
    # Copy generated API docs to docs directory
    mkdir -p "${DOCS_DIR}/api-reference/html"
    cp -r html/* "${DOCS_DIR}/api-reference/" 2>/dev/null || true
    
    echo "API documentation generated successfully."
else
    echo "Doxygen not found. Install with: sudo apt install doxygen"
    exit 1
fi

# Generate static site (if using Jekyll)
if command -v bundle >/dev/null 2>&1 && [ -f "${DOCS_DIR}/.jekyll.yml" ]; then
    echo "Building static site..."
    cd "${DOCS_DIR}"
    bundle exec jekyll build --destination ../docs/website
    echo "Static site built successfully."
fi

echo "Documentation build completed successfully!"
```

### 4. Container Development Environment

#### Docker Configuration

**Dockerfile**:
```dockerfile
# Dockerfile for NLM development environment
FROM ubuntu:22.04

LABEL maintainer="NLM Development Team"
LABEL description="Development environment for NLM (熙然) brain simulation project"
LABEL version="1.0.0"

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    wget \
    vim \
    tmux \
    pkg-config \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# Install Python and required packages
RUN apt-get update && apt-get install -y python3 python3-pip python3-venv \
    && rm -rf /var/lib/apt/lists/*

# Install NLM dependencies
RUN pip3 install \
    doxygen-graphviz \
    markdown2rst \
    sphinx \
    sphinx-rtd-theme \
    mkdocs \
    pymdown-extensions

# Set working directory
WORKDIR /app

# Copy project files
COPY . /app/nlm
WORKDIR /app/nlm

# Create build directory
RUN mkdir -p build

# Build NLM
RUN cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4

# Install development tools
RUN cd / && \
    git clone https://github.com/pypa/pipenv.git && \
    pip3 install --user pipenv && \
    rm -rf pip

# Set up shell environment
RUN echo "source /usr/share/nvm/bash_completion.sh" >> /root/.bashrc

# Create development user
RUN useradd -m -s /bin/bash developer
USER developer
WORKDIR /app/nlm

# Install development tools for user
RUN sudo -u developer pip3 install --user poetry
homeworker() {
    poetry install
}

# Set up aliases and functions
RUN echo "alias ll='ls -la'" >> /home/developer/.bashrc
echo "alias make='make -j4'" >> /home/developer/.bashrc
echo "alias test='cd build && ctest --output-on-failure'" >> /home/developer/.bashrc
echo "alias docs='cd build && make docs'" >> /home/developer/.bashrc

# Expose ports for services (if needed)
EXPOSE 8080 8000

# Default command
CMD ["/bin/bash"]
```

**Docker Compose Configuration**:
```yaml
# docker-compose.yml
version: '3.8'

services:
  nlm-dev:
    build: .
    volumes:
      - ./nlm:/app/nlm
      - ./build:/app/nlm/build
      - ./docs:/app/nlm/docs
      - ./configs:/app/nlm/configs
    ports:
      - "8080:8080"
      - "8000:8000"
    environment:
      - NLM_ENV=development
      - DISPLAY=${DISPLAY}
    command: tail -f /dev/null

  nlm-test:
    build: .
    volumes:
      - ./nlm:/app/nlm
    command: >
      /bin/bash -c "
        cd /app/nlm/build
        ctest --output-on-failure
        ./nlm_phase6_demo
      "

  nlm-docs:
    build: .
    volumes:
      - ./nlm:/app/nlm
      - ./docs:/app/nlm/docs
    command: >
      /bin/bash -c "
        cd /app/nlm
        ./build.sh --no-tests --no-docs
        make docs
      "
    ports:
      - "80:80"
```

**Docker Development Workflow**:
```bash
# Build and run development container
 docker-compose up -d

# Execute commands in container
 docker-compose exec nlm-dev bash

# Run tests
 docker-compose exec nlm-dev /bin/bash -c "cd build && ctest --output-on-failure"

# Generate documentation
docker-compose exec nlm-dev /bin/bash -c "cd build && make docs"

# Stop containers
docker-compose down
```

## Environment Management

### 1. Virtual Environment Setup

**Python Virtual Environment**:
```bash
# Create virtual environment
python3 -m venv ~/.nlm-venv
source ~/.nlm-venv/bin/activate

# Install required Python packages
pip install -r requirements.txt
pip install poetry
pip install sphinx sphinx-rtd-theme mkdocs

# Install NLM development tools
pip install doxygen-graphviz
```

**Poetry Configuration**:
```toml
# pyproject.toml
[tool.poetry]
name = "nlm"
version = "1.0.0"
description = "An Experimental Artificial Developmental Brain"
authors = ["NLM Development Team <dev@nlm.ai>"]
license = "MIT"

[tool.poetry.dependencies]
python = "^3.8"
cpplint = "^1.6.0"
doc8 = "^0.1.0"
mdformat = "^0.6.0"

[tool.poetry.dev-dependencies]
cmake = "^3.20.0"
pytest = "^6.0.0"

[build-system]
requires = ["poetry-core"]
build-backend = "poetry.core.masonry.api"
```

### 2. Configuration Management

**Environment Variables**:
```bash
# NLM Environment Variables
export NLM_ENV="development"  # development, testing, production
export NLM_LOG_LEVEL="INFO"   # TRACE, DEBUG, INFO, WARN, ERROR, FATAL
export NLM_LOG_FILE="nlm.log"
export NLM_RANDOM_SEED="42"
export NLM_DISABLE_GPU="false"
export NLM_MEMORY_LIMIT="4G"

# Build configuration
export CMAKE_BUILD_TYPE="Release"
export ENABLE_TESTS=true
export ENABLE_DOXYGEN=true

# Performance settings
export NLM_NUM_THREADS=4
export NLM_SIMULATION_STEPS=1000
export NLM_BENCHMARK_ITERATIONS=10
```

**Configuration File** (`~/.nlm/config`):
```bash
# NLM configuration file
# Format: key = value

[simulation]
timestep = 0.001
steps = 1000
random_seed = 42
parallel_enabled = true
simd_optimization = true

[brain]
neuron_count = 1024
region_count = 4
plasticity_enabled = true
synaptic_density = 0.1
connection_range = 3

[memory]
working_memory_capacity = 256
episodic_memory_size = 1024
consolidation_interval = 500

[neuromodulation]
dopamine_enabled = true
curiosity_enabled = true
novelty_enabled = true
prediction_error_enabled = true

[environment]
world_type = "SimpleWorld"
reward_structure = "delayed"
visual_field = 90
```

## Development Tools Setup

### 1. Code Quality Tools

**Pre-commit Hooks** (`.pre-commit-config.yaml`):
```yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: check-added-large-files
      - id: check-ast
      - id: check-yaml
      - id: end-of-file-fixer
      - id: trailing-whitespace

  - repo: https://github.com/psf/black
    rev: 22.12.0
    hooks:
      - id: black
        language_version: python3

  - repo: https://github.com/PyCQA/isort
    rev: 5.11.4
    hooks:
      - id: isort

  - repo: https://github.com/PyCQA/flake8
    rev: 5.0.4
    hooks:
      - id: flake8

  - repo: https://github.com/pre-commit/mirrors-clang-format
    rev: v14.0.6
    hooks:
      - id: clang-format
        types: [c++]

  - repo: https://github.com/editorconfig-checker/editorconfig-checker
    rev: 2.6.0
    hooks:
      - id: editorconfig-checker
```

**Code Style Validation**:
```bash
# Install pre-commit
a pip install pre-commit

# Install hooks
pre-commit install

# Run all hooks
pre-commit run --all-files
```

### 2. Continuous Integration

**GitHub Actions Configuration** (`.github/workflows/ci.yml`):
```yaml
name: NLM CI/CD

on:
  push:
    branches: [main, development]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
        compiler: [g++, clang++]
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Set up environment
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake g++ clang python3-pip
      
      - name: Install Python dependencies
        run: pip3 install -r requirements.txt
      
      - name: Configure CMake
        run: |
          mkdir build
          cd build
          cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
      
      - name: Build
        run: |
          cd build
          make -j4
      
      - name: Test
        run: |
          cd build
          ctest --output-on-failure
      
      - name: Generate Documentation
        run: |
          cd build
          make docs
      
      - name: Upload Artifacts
        uses: actions/upload-artifact@v3
        with:
          name: nlm-build-${{ matrix.build_type }}
          path: |
            build/nlm
            build/docs
```

**Code Coverage**:
```bash
# Install coverage tools
pip install coverage pytest-cov

# Run tests with coverage
coverage run -m pytest tests/
coverage report --include="src/nlm/*"
coverage html
```

### 3. Development Workflow Tools

**Project Management Board** (`.github/projects.yml`):
```yaml
columns:
  - name: To Do
  - name: In Progress
  - name: In Review
  - name: Done

items:
  - id: 1
    name: Implement neural population management
    column: To Do
    labels:
      - enhancement
      - good first issue
  
  - id: 2
    name: Add STDP plasticity implementation
    column: In Progress
    labels:
      - enhancement
  
  - id: 3
    name: Document API reference
    column: In Review
    labels:
      - documentation
  
  - id: 4
    name: Add unit tests for memory systems
    column: Done
    labels:
      - testing
```

**Progress Tracking**:
```bash
# Generate project report
gitlog --pretty=format:"%h %ad | %s%d" --date=short --graph --all > project_log.txt

cat project_log.txt
```

## Advanced Development Setup

### 1. Performance Development Environment

**Performance Profiling**:
```bash
# Install performance analysis tools
apt-get install linux-tools-common linux-tools-generic

# Profile application performance
perf record -g ./nlm_phase6_demo
perf report > performance_report.txt

# Analyze memory usage
valgrind --tool=massif ./nlm_phase6_demo
ms_print massif.out.12345 > memory_analysis.txt

# Profile with OProfile
opcontrol --setup
opcontrol --start-profiling
./nlm_phase6_demo
opcontrol --stop-profiling
opreport -p /app/nlm/build/nlm > oprofile_report.txt
```

**Performance Optimization**:
```bash
# Generate performance benchmarks
./benchmark --suite=all --output=perf_results.json

# Analyze performance bottlenecks
python3 analyze_performance.py perf_results.json

# Generate performance report
./generate_performance_report.py perf_results.json > performance_report.md
```

### 2. Development Container Setup

**Docker Development Workflow**:
```bash
# Start development container
docker-compose up -d

# Execute commands in container
docker-compose exec nlm-dev bash

# Run tests inside container
docker-compose exec nlm-dev /bin/bash -c "cd build && ctest --output-on-failure"

# Generate documentation inside container
docker-compose exec nlm-dev /bin/bash -c "cd build && make docs"

# Copy results back to host
docker cp nlm-dev:build /path/to/host/build/

# Stop containers
docker-compose down
```

**Development Container Commands**:
```bash
# Build development image
docker-compose build

# Run with custom configuration
docker-compose run --rm nlm-dev /bin/bash -c "
    cmake .. -DENABLE_TESTS=ON -DENABLE_DOXYGEN=ON
    make -j4
    ctest --output-on-failure
"

# Debug mode with container
docker-compose -f docker-compose.yml -f docker-compose.debug.yml up -d
```

### 3. Documentation Development Environment

**Static Site Generation**:
```bash
# Install static site generators
pip install mkdocs mkdocs-material

# Build documentation
mkdocs build

# Serve documentation locally
mkdocs serve

# Deploy to GitHub Pages
mkdocs gh-deploy
```

**Documentation Templates**:
```bash
# Create documentation templates
mkdir -p docs/templates

# User manual template
cat > docs/templates/user-manual.md << 'EOF'
# {title}

## Overview
{overview}

## Prerequisites
{requirements}

## Installation
{installation}

## Usage
{usage}

## Configuration
{configuration}

## Troubleshooting
{troubleshooting}

## References
{references}
EOF

# Tutorial template
cat > docs/templates/tutorial.md << 'EOF'
# {title}

## Prerequisites
{prerequisites}

## {section1}
{content1}

## {section2}
{content2}

## Advanced Topics
{advanced}

## Summary
{summary}
EOF
```

## Environment Maintenance

### 1. Regular Maintenance Tasks

**Update Dependencies**:
```bash
# Update package lists
apt-get update

# Upgrade installed packages
apt-get upgrade -y

# Update documentation tools
pip install --upgrade pip
pip install --upgrade poetry
pip install --upgrade doxygen-graphviz
```

**Clean Build Artifacts**:
```bash
# Clean build directories
cd /path/to/nlm
rm -rf build
rm -rf docs/api-reference/html
rm -rf docs/website

# Clean temporary files
rm -f *.log
rm -f *.tmp
rm -f *.temp

# Clean IDE files
rm -f .idea/workspace.xml
rm -f .vscode/tasks.json
rm -f .vscode/settings.json
```

**Update Documentation**:
```bash
# Update API documentation
cd nlm
make docs

# Update website content
cd docs
mkdocs build
mkdocs gh-deploy
```

### 2. Backup and Recovery

**Backup Configuration**:
```bash
#!/bin/bash
# backup_nlm.sh - Backup NLM configuration and documentation

BACKUP_DIR="$HOME/nlm-backups"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"

mkdir -p "$BACKUP_DIR"

# Backup source code
git archive -o "$BACKUP_DIR/nlm-source-$TIMESTAMP.tar.gz" main

# Backup configuration files
mkdir -p "$BACKUP_DIR/config"
cp -r ~/.nlm/config "$BACKUP_DIR/config/" 2>/dev/null || true
ncp -r ~/.bashrc "$BACKUP_DIR/" 2>/dev/null || true

# Backup documentation
mkdir -p "$BACKUP_DIR/docs"
cp -r docs "$BACKUP_DIR/docs/$TIMESTAMP"

# Backup build artifacts
if [ -d "build" ]; then
    cp -r build "$BACKUP_DIR/build/$TIMESTAMP"
fi

# Create backup manifest
cat > "$BACKUP_DIR/backup_manifest_$TIMESTAMP.txt" << EOF
Backup created: $TIMESTAMP
Source: $(git rev-parse HEAD)
Configuration: $(git config user.name) <$(git config user.email)>
Files backed up:
- Source code
- Configuration files
- Documentation
- Build artifacts
EOF

echo "Backup created successfully: $BACKUP_DIR/backup_manifest_$TIMESTAMP.txt"
```

**Recovery Procedure**:
```bash
#!/bin/bash
# restore_nlm.sh - Restore NLM from backup

BACKUP_DIR="$HOME/nlm-backups"
TIMESTAMP="$1"

if [ -z "$TIMESTAMP" ]; then
    echo "Usage: $0 <backup-timestamp>"
    echo "Available backups:"
    ls -1 "$BACKUP_DIR" 2>/dev/null | grep -E '^[0-9]{8}_[0-9]{6}$' || echo "No backups found"
    exit 1
fi

BACKUP_PATH="$BACKUP_DIR/backup_manifest_$TIMESTAMP.txt"

if [ ! -f "$BACKUP_PATH" ]; then
    echo "Backup not found: $TIMESTAMP"
    exit 1
fi

echo "Restoring from backup: $TIMESTAMP"

# Restore source code
cd /path/to/nlm
git checkout --ours .

# Restore configuration files
if [ -d "$BACKUP_DIR/config/$TIMESTAMP" ]; then
    cp -r "$BACKUP_DIR/config/$TIMESTAMP/." ~/.nlm/ 2>/dev/null || true
fi

# Restore documentation
if [ -d "$BACKUP_DIR/docs/$TIMESTAMP" ]; then
    cp -r "$BACKUP_DIR/docs/$TIMESTAMP/." docs/ 2>/dev/null || true
fi

# Restore build artifacts
if [ -d "$BACKUP_DIR/build/$TIMESTAMP" ]; then
    cp -r "$BACKUP_DIR/build/$TIMESTAMP/." build/ 2>/dev/null || true
fi

echo "Recovery completed successfully"
```

## Conclusion

The NLM development environment is designed to provide a comprehensive, flexible, and efficient platform for neural simulation research and development. This setup includes:

1. **System Infrastructure**: Multiple platform support with containerization options
2. **Development Tools**: IDE integration, code quality tools, and project management
3. **Build Environment**: Comprehensive build scripts and configuration management
4. **Testing Framework**: Automated testing, performance benchmarking, and validation
5. **Documentation System**: Multi-format documentation generation and maintenance
6. **Environment Management**: Version control, backup, and recovery procedures

**Key Features**:

- **Cross-platform support**: Linux, macOS, Windows, and containers
- **Development efficiency**: Integrated tools and scripts
- **Performance optimization**: Profiling and benchmarking capabilities
- **Code quality assurance**: Pre-commit hooks and CI/CD integration
- **Documentation excellence**: Automated generation and maintenance
- **Community collaboration**: Version control and issue tracking

**Development Best Practices**:

1. **Consistency**: Follow established conventions and standards
2. **Automation**: Use scripts and automation for repetitive tasks
3. **Validation**: Thorough testing and validation at each stage
4. **Documentation**: Maintain comprehensive and accurate documentation
5. **Collaboration**: Use version control and code review
6. **Performance**: Profile and optimize for efficiency
7. **Security**: Follow security best practices

This development environment provides the foundation for:

- Research in neural computation and artificial brains
- Development of advanced neural simulation systems
- Education and training in computational neuroscience
- Collaboration across global research teams
- Innovation in artificial intelligence and machine learning

The NLM development environment is continuously evolving to support new research directions and technological advances while maintaining the core principles of scientific rigor and technical excellence.

## Further Reading

### Documentation Resources

1. **NLM Documentation**: https://docs.nlm.ai
2. **API Reference**: https://docs.nlm.ai/api
3. **User Manual**: https://docs.nlm.ai/user-manual
4. **Development Guide**: https://docs.nlm.ai/developer-guide

### Technical Documentation

1. **GitHub Repository**: https://github.com/nlm/nlm
2. **Issue Tracker**: https://github.com/nlm/nlm/issues
3. **Pull Requests**: https://github.com/nlm/nlm/pulls
4. **Discussions**: https://github.com/nlm/nlm/discussions

### Development Tools

1. **CMake Documentation**: https://cmake.org/documentation/
2. **Google Test**: https://github.com/google/googletest/
3. **Doxygen**: http://www.doxygen.org/
4. **GitHub Actions**: https://docs.github.com/en/actions

### Research Papers

1. Dayan, P., & Abbott, L. F. (2001). Theoretical Neuroscience. MIT Press.
2. Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). Neuronal Dynamics: From Single Neurons to Neural Networks. Cambridge University Press.
3. Song, S., Miller, K. D., & Abbott, L. F. (2000). Computational principles of network plasticity: empirical and theoretical results. Neural Networks, 13(4-5), 459-474.

### Community Resources

1. **NLM Forum**: https://forum.nlm.ai
2. **Documentation Slack**: https://nlm.ai/slack
3. **Twitter**: https://twitter.com/NLMBrain
4. **LinkedIn**: https://www.linkedin.com/company/nlm

## Contact Information

### 1. Technical Support

**For technical questions**:
- GitHub Discussions
- Stack Overflow (tagged `nlm`)
- Project maintainers

**For bug reports**:
- GitHub Issues
- Detailed reproduction steps
- Environment information

### 2. Development Support

**For development questions**:
- GitHub Issues with `question` label
- Development documentation
- Code examples and tutorials

**For contribution help**:
- Contributing guidelines
- Code review process
- Feature requests

### 3. Community Engagement

**For community discussions**:
- GitHub Discussions
- Social media channels
- Academic conferences

**For collaboration**:
- Research partnerships
- Industry collaborations
- Academic institutions

## Acknowledgements

### 1. Contributors

The NLM development environment benefits from contributions by:

- **OpenHands**: Documentation generation and development tools
- **NLM Development Team**: Core development and maintenance
- **Community Contributors**: Code contributions, bug fixes, and improvements
- **Research Collaborators**: Scientific contributions and validation

### 2. Open Source Projects

This environment builds on numerous open source projects:

**Build Systems**: CMake, Ninja
**Testing Frameworks**: Google Test
**Documentation Tools**: Doxygen, MkDocs
**Development Tools**: Git, Docker, VS Code

**Thank you to all open source contributors** who make this development environment possible.

## License

Development environment documentation is licensed under the MIT License.

## Copyright

© 2026 NLM Development Team. All rights reserved.