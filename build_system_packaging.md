# Build System and Packaging Improvements

## Overview

This document describes comprehensive build system and packaging improvements for the NLM (Neural Learning Machine) system. The enhancements focus on modern build tools, packaging formats, deployment strategies, and continuous integration while maintaining backward compatibility and improving developer experience.

## Build System Architecture

### 1. Modern Build Tools
```cmake
// NLM Build Configuration
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 6.0.0 LANGUAGES CXX)

// Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTING "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_DOCUMENTATION "Build documentation" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_MSVC_STATIC_RUNTIME "Use static MSVC runtime" OFF)

// C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

// Build types
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

// Project configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectConfig.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)
```

### 2. CMake Configuration Management
```cmake
// External dependencies management
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

// Library configurations
add_library(nlm_core STATIC
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

add_library(nlm_brain STATIC
    src/brain/Brain.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralRegion.cpp
    src/brain/NeuralPopulation.cpp
)

add_library(nlm_agent STATIC
    src/agent/AgentBrain.cpp
    src/agent/SensoryPercept.cpp
)

add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
    src/world/Observation.cpp
)

// Executable targets
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PRIVATE nlm_core nlm_brain nlm_agent nlm_world)
```

### 3. Python Bindings Build
```cmake
// Python bindings configuration
if(BUILD_PYTHON_BINDINGS)
    # Find pybind11
    find_package(pybind11 REQUIRED)
    
    # Python bindings target
    pybind11_add_module(pynlm
        python/bindings.cpp
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core nlm_brain)
    
    # Set Python module properties
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        OUTPUT_NAME "pynlm"
        MACOSX_RPATH ON
    )
    
    # Install Python module
    install(TARGETS pynlm
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
    )
    
    # Install Python package metadata
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/python/pyproject.toml
        ${CMAKE_CURRENT_SOURCE_DIR}/python/README.md
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages/pynlm
    )
endif()
```

### 4. Testing Infrastructure
```cmake
// Testing configuration
if(BUILD_TESTING)
    enable_testing()
    
    # Add test subdirectory
    add_subdirectory(tests)
    
    # Configure tests
    set(TEST_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/nlm/tests)
    
    # Test coverage
    if(ENABLE_COVERAGE)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Coverage.cmake)
    endif()
    
    # Sanitizer options
    if(ENABLE_ASAN)
        target_link_options(nlm_test PRIVATE "-fsanitize=address")
    endif()
    
    if(ENABLE_UBSAN)
        target_link_options(nlm_test PRIVATE "-fsanitize=undefined")
    endif()
endif()
```

### 5. Documentation Build
```cmake
// Documentation configuration
if(BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED)
    find_package(Graphviz REQUIRED)
    
    # Doxygen configuration
    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_PROJECT_NAME "NLM")
    set(DOXYGEN_PROJECT_NUMBER "6.0")
    set(DOXYGEN_SHORT_TITLE "NLM - Neural Learning Machine")
    set(DOXYGEN_PROJECT_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/docs/logo.png")
    
    # Generate documentation
    find_program(DOXYGEN_EXECUTABLE doxygen)
    if(DOXYGEN_EXECUTABLE)
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
    
    # Markdown documentation
    add_custom_target(markdown_docs
        COMMAND python3 -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/docs/requirements.txt
        COMMAND mkdocs build --clean --strict
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
        COMMENT "Building Markdown documentation with MkDocs"
        VERBATIM
    )
endif()
```

## Packaging System

### 1. Debian Package Structure
```bash
# Debian package structure
# -*- dpkg-divert -*-:
/usr/lib/python3/dist-packages/pynlm
dpkg-divert keep /usr/lib/python3/dist-packages/pynlm

# NLM packages
/usr/lib/x86_64-linux-gnu/libnlm_core.so.6.0
/usr/lib/x86_64-linux-gnu/libnlm_core.so.6
/usr/lib/x86_64-linux-gnu/libnlm_brain.so.6.0
/usr/lib/x86_64-linux-gnu/libnlm_brain.so.6
/usr/lib/x86_64-linux-gnu/libnlm_agent.so.6.0
/usr/lib/x86_64-linux-gnu/libnlm_agent.so.6
/usr/lib/x86_64-linux-gnu/libnlm_world.so.6.0
/usr/lib/x86_64-linux-gnu/libnlm_world.so.6

# Executables
/usr/bin/nlm
/usr/bin/nlm_phase6_demo
/usr/bin/nlm_phase3_demo
/usr/bin/nlm_phase4_demo

# Configuration files
/etc/nlm/
/etc/nlm/nlm.conf
/etc/nlm/nlm.xml

# Documentation
/usr/share/doc/nlm/html
/usr/share/doc/nlm/markdown

# Python packages
/usr/lib/python3/dist-packages/pynlm-6.0.0-py3.8.egg-info
/usr/lib/python3/dist-packages/pynlm/__init__.py
/usr/lib/python3/dist-packages/pynlm/core.py
/usr/lib/python3/dist-packages/pynlm/brain.py
/usr/lib/python3/dist-packages/pynlm/world.py
/usr/lib/python3/dist-packages/pynlm/agent.py
/usr/lib/python3/dist-packages/pynlm/experiments.py
```

### 2. Package Scripts
```bash
#!/bin/bash
# NLM Package Build Script

# Build packages for multiple distributions
# Usage: build_packages <distribution> <architecture>

BUILD_PACKAGES="build_packages"
PACKAGE_MANAGER="yum"
TARGET_DISTRIBUTION="centos"
TARGET_ARCH="x86_64"

if [ $# -ge 1 ]; then
    TARGET_DISTRIBUTION="$1"
fi

if [ $# -ge 2 ]; then
    TARGET_ARCH="$2"
fi

case $TARGET_DISTRIBUTION in
    "centos"|"rhel"|"rocky"|"almalinux")
        echo "Building RPM packages for $TARGET_DISTRIBUTION on $TARGET_ARCH"
        build_rpm_packages
        ;;
    "debian"|"ubuntu"|"mint")
        echo "Building DEB packages for $TARGET_DISTRIBUTION on $TARGET_ARCH"
        build_deb_packages
        ;;
    "fedora"|"opensuse")
        echo "Building RPM packages for $TARGET_DISTRIBUTION on $TARGET_ARCH"
        build_rpm_packages
        ;;
    *)
        echo "Unsupported distribution: $TARGET_DISTRIBUTION"
        exit 1
        ;;
esac

# Build dependencies
setup_build_dependencies()

# Build main package
build_main_package()

# Build development package
build_development_package()

# Build Python package
build_python_package()

# Build documentation package
build_documentation_package()

# Create meta packages
create_meta_packages()

# Verify packages
verify_packages()

# Clean up
cleanup_build_artifacts

echo "Package build completed successfully"
```

### 3. RPM Package Specification
```spec
# RPM Package Specification for NLM
Summary: Neural Learning Machine - Artificial Brain Simulation Framework
Name: nlm
Version: 6.0.0
Release: 1%{?dist}
License: MIT
Group: Science/Math
Source: https://github.com/nlm-project/nlm/archive/v6.0.0.tar.gz

# Package requirements
Requires: glibc >= 2.17
Requires: gcc-c++ >= 4:4.8.5
Requires: cmake >= 3.16
Requires: python3 >= 3.8
Requires: pybind11 >= 2.11.0
Requires: make >= 3.82
Requires: libcurl4

# Package description
%description
NLM (Neural Learning Machine) is an experimental artificial developmental brain project.
It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation,
synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and
structural plasticity. NLM is designed to create a neural system that learns from experience
and develops increasingly complex abilities through interaction with an environment.

This package contains:
- Core NLM libraries and executables
- Python bindings for NLM
- Development headers and static libraries
- Configuration files
- Documentation

# Package files
%files -f %{_builddir}/nlm-files.list
%attr(0755, root, root) /usr/bin/nlm
%attr(0755, root, root) /usr/bin/nlm_phase6_demo
%attr(0755, root, root) /usr/bin/nlm_phase3_demo
%attr(0755, root, root) /usr/bin/nlm_phase4_demo
%attr(0644, root, root) /usr/share/doc/nlm/html/*
%attr(0644, root, root) %{_mandir}/man1/nlm.1
%attr(0644, root, root) %{_datadir}/nlm/nlm.conf

# Python files
%files -a python_files
%{python3_sitelib}/pynlm*
%{python3_sitelib}/pynlm/*
```

### 4. DEB Package Specification
```deb
# DEB Package Specification for NLM
Source: nlm
Section: science
Priority: extra
Maintainer: NLM Authors <authors@nlm-project.org>
Standards-Version: 4.6.0
Vcs-Browser: https://github.com/nlm-project/nlm
Vcs-Git: https://github.com/nlm-project/nlm.git

Package: nlm
Version: 6.0.0
Release: 1ubuntu1
Architecture: all
Essential: no
Category: science

# Package description
NLM (Neural Learning Machine) is an experimental artificial developmental brain project.
It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation,
synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and
structural plasticity. NLM is designed to create a neural system that learns from experience
and develops increasingly complex abilities through interaction with an environment.

# Package dependencies
Depends: libc6 (>= 2.14), libgcc-s1 | gcc-s1, libc6-dev, libstdc++6 (>= 4:4.8.5), libstdc++-5 (>= 5.1)
Depends: python3 (>= 3.8), python3-dev, python3-distutils, libpython3.8, libpython3.8-dev, python3-pip
Depends: cmake (>= 3.16), make, build-essential
Requires: libcurl4

# Package files
%files
/usr/bin/nlm
/usr/bin/nlm_phase6_demo
/usr/bin/nlm_phase3_demo
/usr/bin/nlm_phase4_demo
/usr/share/doc/nlm/html/
/usr/share/man/man1/nlm.1
/etc/nlm/

# Python files
/usr/lib/python3/dist-packages/pynlm-6.0.0-py3.8.egg-info/
/usr/lib/python3/dist-packages/pynlm/
```

## Container Deployment

### 1. Docker Configuration
```dockerfile
# Dockerfile for NLM
FROM ubuntu:22.04

LABEL maintainer="NLM Authors <authors@nlm-project.org>"
LABEL description="NLM - Neural Learning Machine"
LABEL version="6.0.0"
LABEL repository="https://github.com/nlm-project/nlm"

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    python3 \
    python3-dev \
    python3-pip \
    curl \
    wget \
    doxygen \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
RUN pip3 install --no-cache-dir \
    pybind11[global] \
    numpy \
    pyyaml \
    sphinx \
    sphinx-rtd-theme

# Clone NLM source code
WORKDIR /app
RUN git clone https://github.com/nlm-project/nlm.git .
RUN git checkout v6.0.0

# Build NLM
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_PYTHON_BINDINGS=ON && \
    make -j4

# Install Python bindings
RUN pip3 install -e .

# Create user for non-root execution
RUN useradd -m -u 1000 nlmuser
USER nlmuser

# Set working directory
WORKDIR /home/nlmuser

# Create data directory
RUN mkdir -p /home/nlmuser/data && chown -R nlmuser:nlmuser /home/nlmuser/data

# Copy example configurations
COPY examples/ /home/nlmuser/examples/
COPY configs/ /home/nlmuser/configs/

# Set default command
CMD ["nlm_phase6_demo"]

# Health check
HEALTHCHECK --interval=30s --timeout=30s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1
```

### 2. Kubernetes Deployment
```yaml
# Kubernetes Deployment for NLM
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nlm
  labels:
    app: nlm
    version: "6.0.0"
spec:
  replicas: 1
  selector:
    matchLabels:
      app: nlm
  template:
    metadata:
      labels:
        app: nlm
        version: "6.0.0"
    spec:
      containers:
      - name: nlm
        image: nlm:latest
        imagePullPolicy: IfNotPresent
        command: ["nlm_phase6_demo"]
        args:
        - --config=/etc/nlm/nlm.conf
        - --checkpoint-dir=/data/checkpoints
        - --log-level=info
        env:
        - name: NLM_DATA_DIR
          value: "/data"
        - name: NLM_CHECKPOINT_DIR
          value: "/data/checkpoints"
        - name: NLM_LOG_LEVEL
          value: "info"
        resources:
          requests:
            cpu: 1000m
            memory: 2Gi
          limits:
            cpu: 2000m
            memory: 4Gi
        volumeMounts:
        - name: data
          mountPath: /data
        - name: config
          mountPath: /etc/nlm
        readinessProbe:
          exec:
            command: ["/usr/bin/test", "-f", "/tmp/nlm-ready"]
          initialDelaySeconds: 30
          periodSeconds: 10
        livenessProbe:
          exec:
            command: ["/usr/bin/test", "-f", "/tmp/nlm-healthy"]
          initialDelaySeconds: 60
          periodSeconds: 20
  volumes:
  - name: data
    persistentVolumeClaim:
      claimName: nlm-data-pvc
      readOnly: false
  - name: config
    configMap:
      name: nlm-config
      items:
      - key: nlm.conf
        path: nlm.conf
---
apiVersion: v1
kind: ConfigMap
metadata:
  name: nlm-config
spec:
  data:
    nlm.conf: |
      # NLM Configuration
      # Core settings
      random_seed = 42
      simulation_timestep = 0.001
      neuron_count = 1000
      region_count = 1
      connection_probability = 0.1
      
      # Plasticity settings
      stdp_ltp_weight = 0.01
      stdp_ltd_weight = 0.012
      stdp_tau = 20.0
      plasticity_hebbian_enable = true
      plasticity_structural_enable = true
      
      # Neuromodulation settings
      neuromod_dopamine_scale = 1.0
      neuromod_curiosity_enable = true
      neuromod_novelty_enable = true
      
      # Development settings
      development_initial_plasticity = 1.0
      development_enable = true
      
      # World settings
      world_width = 50
      world_height = 50
      world_vision_width = 8
      world_vision_height = 8
      
      # Checkpoint settings
      checkpoint_dir = "/data/checkpoints"
      checkpoint_interval = 1000
      checkpoint_max_files = 10
---
apiVersion: storage.k8s.io/v1
kind: StorageClass
metadata:
  name: nlm-storage
  annotations:
    storageclass.kubernetes.io/is-default-class: "true"
  provisioner: kubernetes.io/csi-rbd
  parameters:
    monitorName: "ceph"
    pool: "nlm-pool"
    userId: "nlm"
    userKey: "nlm-key"
    fsID: "nlm-fs"
    #cephConf: "mon://<mon_ip>:<secret>"
spec:
  reclaimPolicy: Retain
  volumeBindingMode: Immediate
  allowVolumeExpansion: true
---
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: nlm-data-pvc
spec:
  accessModes:
    - ReadWriteOnce
  storageClassName: nlm-storage
  resources:
    requests:
      storage: 100Gi
  volumeMode: Filesystem
```

### 3. Docker Compose Configuration
```yaml
# Docker Compose for NLM
version: '3.8'
services:
  nlm:
    image: nlm:latest
    build:
      context: .
      dockerfile: Dockerfile
    ports:
      - "8080:8080"
    volumes:
      - ./data:/app/data
      - ./configs:/etc/nlm
    environment:
      NLM_DATA_DIR: /app/data
      NLM_CHECKPOINT_DIR: /app/data/checkpoints
      NLM_LOG_LEVEL: info
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "/usr/bin/test", "-f", "/tmp/nlm-ready"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 30s
    networks:
      - nlm-network
    
  nlm-visualization:
    image: nlm-visualization:latest
    build:
      context: .
      dockerfile: Dockerfile.visualization
    ports:
      - "3000:3000"
    environment:
      NLM_HOST: nlm
      NLM_PORT: 8080
    depends_on:
      - nlm
    networks:
      - nlm-network
    
  nlm-monitoring:
    image: nlm-monitoring:latest
    build:
      context: .
      dockerfile: Dockerfile.monitoring
    ports:
      - "9090:9090"
    environment:
      NLM_HOST: nlm
      NLM_PORT: 8080
    depends_on:
      - nlm
    networks:
      - nlm-network

networks:
  nlm-network:
    driver: bridge
```

## Continuous Integration

### 1. GitHub Actions Configuration
```yaml
# GitHub Actions for NLM
name: NLM Build and Test

on:
  push:
    branches: [main, develop]
    tags: ['v*', 'release-*']
  pull_request:
    branches: [main]
  workflow_dispatch:

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: ['3.8', '3.9', '3.10', '3.11', '3.12']
        build-type: ['release', 'debug']
    
    steps:
    - uses: actions/checkout@v3
      with:
        fetch-depth: 0
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v4
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install system dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          build-essential \
          cmake \
          gcc-9 \
          g++-9 \
          git \
          python3-dev \
          libpython3.8-dev \
          pybind11-dev \
          libcurl4-openssl-dev \
          doxygen \
          graphviz \
          python3-pip \
          python3-venv
    
    - name: Create virtual environment
      run: |
        python -m venv .venv
        source .venv/bin/activate
        python -m pip install --upgrade pip
        pip install -r requirements.txt
    
    - name: Set up C++ environment
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_PYTHON_BINDINGS=ON \
          -DBUILD_TESTING=ON \
          -DENABLE_COVERAGE=ON \
          -DENABLE_ASAN=OFF \
          -DENABLE_UBSAN=OFF
    
    - name: Build NLM
      run: |
        cd build
        make -j$(nproc)
    
    - name: Run Python tests
      run: |
        cd python
        pip install -e .
        pytest tests/ -v --cov=pynlm --cov-report=xml
    
    - name: Run C++ tests
      run: |
        cd build
        ./nlm_test
    
    - name: Run integration tests
      run: |
        cd tests
        ./integration_test
    
    - name: Generate coverage report
      uses: codecov/codecov-action@v3
      if: matrix.python-version == '3.11'
    
    - name: Build documentation
      run: |
        cd docs
        mkdocs build
    
    - name: Package artifacts
      run: |
        cd build
        cpack -G DEB
        cpack -G RPM
        
        mkdir -p artifacts
        cp *.deb *.rpm artifacts/ 2>/dev/null || true
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v3
      if: github.ref == 'refs/heads/main'
      with:
        name: nlm-packages-${{ matrix.python-version }}-${{ matrix.build-type }}
        path: artifacts/
        retention-days: 30
    
    - name: Deploy to staging
      if: github.ref == 'refs/heads/main'
      run: |
        # Deploy to staging environment
        curl -X POST ${{ secrets.STAGING_WEBHOOK }} \
          -H "Content-Type: application/json" \
          -d '{"repository": "${{ github.repository }}", "commit": "${{ github.sha }}"}'

  package:
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main' && startsWith(github.ref, 'refs/tags/v')
    
    steps:
    - uses: actions/checkout@v3
      with:
        fetch-depth: 0
    
    - name: Set up Python 3.11
      uses: actions/setup-python@v4
      with:
        python-version: '3.11'
    
    - name: Install system dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          build-essential \
          cmake \
          python3-dev \
          pybind11-dev \
          libcurl4-openssl-dev
    
    - name: Create virtual environment
      run: |
        python -m venv .venv
        source .venv/bin/activate
        python -m pip install --upgrade pip
        pip install -r requirements.txt
    
    - name: Build packages
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_PYTHON_BINDINGS=ON
        make -j$(nproc)
        cpack -G DEB
        cpack -G RPM
    
    - name: Create release
      uses: softprops/action-gh-release@v1
      with:
        tag_name: ${{ github.ref_name }}
        name: Release ${{ github.ref_name }}
        body: |
          ## Release Notes
          
          This is release ${{ github.ref_name }} of NLM.
          
          ### Changes since previous release:
          - Fixed critical bugs in neural plasticity
          - Added new visualization capabilities
          - Improved Python bindings
          - Enhanced configuration system
          
          ### Known issues:
          - Some performance regressions in large simulations
          - Memory usage improvements needed for long-running simulations
          
        draft: false
        prerelease: false
        files: |
          build/*.deb
          build/*.rpm
          build/nlm*.tar.gz
        token: ${{ secrets.GITHUB_TOKEN }}
```

## Development Environment Setup

### 1. Local Development Setup
```bash
#!/bin/bash
# Local development setup script

# Clone NLM repository
cd /workspace
if [ ! -d "nlm" ]; then
    git clone https://github.com/nlm-project/nlm.git
    cd nlm
else
    cd nlm
    git pull origin main
fi

# Setup Python virtual environment
cd nlm
python3 -m venv .venv
source .venv/bin/activate

# Install Python dependencies
pip install --upgrade pip
pip install -r requirements.txt

# Install development dependencies
pip install -e .

# Setup build directory
mkdir -p build
cd build

# Configure build
cmake .. -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_PYTHON_BINDINGS=ON \
  -DBUILD_TESTING=ON \
  -DENABLE_COVERAGE=ON \
  -DENABLE_ASAN=ON \
  -DENABLE_UBSAN=ON

# Build project
make -j$(nproc)

# Set up environment variables
export PYTHONPATH="$(pwd)/../python:$PYTHONPATH"
export LD_LIBRARY_PATH="$(pwd):$LD_LIBRARY_PATH"

# Install Python module in development mode
pip install -e .

# Create data directory
mkdir -p data/checkpoints

# Copy example configuration
cp ../configs/default.cfg ./nlm.conf || true

cd ..

echo "Development environment setup completed successfully"
echo "To start development, run:"
echo "  source .venv/bin/activate"
echo "  cd nlm"
echo "  make run  # or use your preferred build system"
```

### 2. IDE Configuration
```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build NLM",
            "type": "shell",
            "command": "mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j$(nproc)",
            "group": "build",
            "problemMatcher": "$gcc",
            "options": {
                "cwd": "${workspaceFolder}/nlm"
            }
        },
        {
            "label": "Run Tests",
            "type": "shell",
            "command": "./build/nlm_test",
            "group": "test",
            "problemMatcher": "$gcc",
            "options": {
                "cwd": "${workspaceFolder}/nlm/tests"
            }
        },
        {
            "label": "Python Tests",
            "type": "shell",
            "command": "source .venv/bin/activate && pytest tests/ -v",
            "group": "test",
            "problemMatcher": [
                {
                    "owner": "python",
                    "source": "${workspaceFolder}/nlm"
                }
            ],
            "options": {
                "cwd": "${workspaceFolder}/nlm"
            }
        },
        {
            "label": "Generate Documentation",
            "type": "shell",
            "command": "mkdocs build",
            "group": "build",
            "problemMatcher": [],
            "options": {
                "cwd": "${workspaceFolder}/nlm/docs"
            }
        }
    ]
}
```

### 3. Environment Configuration
```bash
# Environment configuration script
#!/bin/bash
# Environment setup for NLM development

# Create environment file
 cat > .env << EOF
# NLM Development Environment

# Core settings
NLM_DATA_DIR=./data
NLM_CHECKPOINT_DIR=./data/checkpoints
NLM_CONFIG_FILE=./configs/default.cfg
NLM_LOG_LEVEL=info

# Python settings
PYTHONPATH=./python:$PYTHONPATH
PYTHONIOENCODING=utf-8

# Build settings
CMAKE_BUILD_TYPE=Debug
BUILD_TESTING=1
BUILD_PYTHON_BINDINGS=1

# Performance settings
OMP_NUM_THREADS=$(nproc)

# Simulation settings
NLM_SIMULATION_STEPS=1000
NLM_SIMULATION_TIMESTEP=0.001

# Monitoring settings
NLM_MONITORING_INTERVAL=1000
NLM_MONITORING_ENABLED=true

# Visualization settings
NLM_VISUALIZATION_ENABLED=true
NLM_VISUALIZATION_TYPE=3d

# Monitoring alerts
NLM_ALERTS_ENABLED=true
NLM_ALERT_THRESHOLD_CPU=80.0
NLM_ALERT_THRESHOLD_MEMORY=85.0

EOF

# Load environment
echo "Loading environment variables..."
source .env

# Set executable permissions
chmod +x .venv/bin/python .venv/bin/pip
chmod +x build/nlm*

# Create log directory
mkdir -p logs
echo "Development environment setup completed"
```

## Conclusion

The build system and packaging improvements provide comprehensive tooling for NLM development, testing, and deployment:

1. **Modern Build System**: CMake-based build system with configurable options
2. **Container Deployment**: Docker containers for consistent deployment across environments
3. **Package Management**: RPM and DEB package creation for distribution
4. **Continuous Integration**: GitHub Actions for automated testing and deployment
5. **Development Tools**: IDE configurations and environment setup scripts
6. **Performance Optimization**: Build configurations for different optimization levels
7. **Documentation Integration**: Automated documentation building and deployment
8. **Release Management**: Package creation and distribution for releases

These enhancements improve:
- Developer experience with comprehensive tooling
- Deployment flexibility with containerization
- Distribution with standardized packages
- Quality assurance with CI/CD integration
- Maintainability with modern build practices
- Scalability with container orchestration

The build system and packaging improvements significantly enhance NLM's development workflow, deployment capabilities, and operational efficiency while maintaining backward compatibility and providing a robust foundation for future enhancements.
