# NLM - Installation Guide

**Date:** 2026-10-13
**Audience:** All users
**Level:** Installation guide

---

## Quick Summary

This guide provides **complete instructions** for installing NLM (Neural Learning Machine) on your system. Whether you're installing for development, testing, or production use, this guide covers all installation methods.

## Table of Contents

1. [Quick Start Installation](#quick-start-installation)
2. [Standard Installation Methods](#standard-installation-methods)
3. [Development Installation](#development-installation)
4. [Advanced Installation Options](#advanced-installation-options)
5. [Post-Installation Setup](#post-installation-setup)
6. [Troubleshooting](#troubleshooting)

---

## Quick Start Installation

### For Users (Quick Install)

The fastest way to install NLM is to use the Python package. This requires:

#### Prerequisites
```bash
# Check your system
python3 --version
pip3 --version
```

#### Quick Install Command
```bash
# Navigate to NLM directory
cd /workspace/app

# Install using pip
pip3 install .

# Or install in development mode (recommended for development)
pip3 install -e .
```

**After installation:**
- NLM is ready to use
- Test with: `python3 -c "import pynlm; print(pynlm.__version__)"`
- Run examples: `python3 examples_comprehensive.py`

### For Developers (Full Installation)

#### Prerequisites
```bash
# Install system dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y build-essential cmake git python3-dev python3-pip

# Install Python dependencies
pip3 install pybind11 scikit-build-core pytest numpy matplotlib
```

#### Full Installation Steps
```bash
# Clone NLM repository (if not already done)
git clone https://github.com/nlm/nlm.git

# Navigate to NLM directory
cd nlm

# Build and install
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=ON
cmake --build . -j$(nproc)

# Install Python package
cd ..
pip3 install -e .
```

## Standard Installation Methods

### Method 1: Python Package Installation (Recommended)

#### Automatic Installation (pip)
```bash
# Using pip (automatically handles all dependencies)
cd /workspace/app
pip3 install .
```

#### Manual Installation
```bash
# Install Python dependencies
pip3 install pybind11 scikit-build-core pytest numpy matplotlib

# Build and install
python3 setup.py build_ext --inplace
pip3 install -e .
```

### Method 2: From Source

#### Source Repository
```bash
# Clone the repository
git clone https://github.com/nlm/nlm.git

# Navigate to source directory
cd nlm

# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_PYTHON_BINDINGS=ON \
         -DNLM_ENABLE_ADVANCED_API=ON \
         -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON

# Build
make -j$(nproc)

# Install C++ components (optional)
sudo make install

# Install Python package
cd ..
pip3 install -e .
```

### Method 3: Docker Installation

#### Using Official Docker Image
```bash
# Pull NLM Docker image
docker pull nlm/nlm:latest

# Run NLM container
docker run -it --rm nlm/nlm:latest

# Interactive session with NLM
docker run -it --rm \
    -v $(pwd)/workspace:/workspace \
    nlm/nlm:latest \
    bash -c "cd /workspace && python3 examples_comprehensive.py"
```

#### Building Custom Docker Image
```bash
# Create Dockerfile
cat > Dockerfile << 'EOF'
FROM ubuntu:20.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev

# Set Python path
ENV PYTHONUNVER=3.8
ENV PYTHONPATH=/usr/lib/python3.8/site-packages

# Install Python dependencies
COPY requirements.txt .
RUN pip3 install -r requirements.txt

# Clone NLM
RUN git clone https://github.com/nlm/nlm.git
WORKDIR /nlm

# Build NLM
RUN mkdir build && cd build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j$(nproc)

# Install Python package
RUN pip3 install -e .

# Set working directory
WORKDIR /

# Default command
CMD ["python3", "-c", "import pynlm; print('NLM installed successfully!')"]
