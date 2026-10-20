# NLM Platform-Specific Installation Guides

Comprehensive installation guides for NLM (Neural Learning Machine) on different platforms and operating systems. This documentation covers installation on Linux, macOS, Windows, and cloud environments.

## Table of Contents

1. [Installation Overview](#installation-overview)
2. [Linux Installation](#linux-installation)
3. [macOS Installation](#macos-installation)
4. [Windows Installation](#windows-installation)
5. [Cloud Installation](#cloud-installation)
6. [Development Environment Setup](#development-environment-setup)
7. [Package Manager Installation](#package-manager-installation)
8. [Build from Source](#build-from-source)
9. [Installation Verification](#installation-verification)
10. [Platform-Specific Considerations](#platform-specific-considerations)
11. [Troubleshooting Installation](#troubleshooting-installation)
12. [Platform Support Matrix](#platform-support-matrix)

## Installation Overview

NLM supports installation on multiple platforms and operating systems. This guide provides comprehensive installation instructions for different environments, ensuring smooth setup and operation across various platforms.

### Supported Platforms

- **Linux**: Ubuntu, Debian, Red Hat, CentOS, Fedora, Arch Linux
- **macOS**: macOS 10.15+, macOS 11+, macOS 12+
- **Windows**: Windows 10, Windows 11, Windows Server
- **Cloud**: AWS, Google Cloud, Azure, Docker containers
- **Development**: Local development environments, virtual machines

### Installation Methods

1. **Package Manager**: Using system package managers
2. **pip Installation**: Python package installation
3. **Build from Source**: Compiling from source code
4. **Docker**: Containerized installation
5. **Conda**: Conda package management

### Installation Requirements

**Minimum Requirements:**
- **Python**: 3.8+ (recommended 3.9+ for optimal performance)
- **CMake**: 3.16+ (for building from source)
- **Compiler**: C++ compiler with C++20 support
- **Memory**: 1GB RAM (minimum), 4GB+ recommended
- **Disk Space**: 500MB for NLM core, additional space for examples and documentation

**Recommended Requirements:**
- **Python**: 3.10+ for best compatibility
- **CMake**: 3.20+ for advanced features
- **Compiler**: gcc 9+, clang 10+, or MSVC 2019+
- **Memory**: 8GB+ for large-scale simulations
- **Disk Space**: 2GB+ for full installation

## Linux Installation

### Ubuntu and Debian

#### Package Manager Installation
```bash
# Update package list
sudo apt update

# Install NLM dependencies
sudo apt install -y \
    build-essential \
    cmake \
    git \
    python3-pip \
    python3-venv \
    python3-dev \
    pkg-config \
    libssl-dev \
    libffi-dev

# Create virtual environment
python3 -m venv ~/nlm_env
source ~/nlm_env/bin/activate

# Install NLM via pip
pip install pynlm

# Create NLM project directory
mkdir -p ~/nlm_projects

# Clone NLM repository (if needed)
git clone https://github.com/your-org/nlm.git
```

#### Development Installation
```bash
# Clone NLM repository
if [ ! -d "~/nlm" ]; then
    git clone https://github.com/your-org/nlm.git ~/nlm
fi

# Navigate to NLM directory
cd ~/nlm

# Create virtual environment if not exists
if [ ! -d ".venv" ]; then
    python3 -m venv .venv
fi

# Activate virtual environment
source .venv/bin/activate

# Install NLM in development mode
pip install -e .

# Install additional dependencies for development
pip install pytest pytest-cov black flake8 mypy
```

#### Installation with NVIDIA CUDA (if available)
```bash
# Check if CUDA is available
nvidia-smi

# Install CUDA dependencies (if needed)
sudo apt install -y nvidia-cuda-toolkit

# Set environment variables
export NVIDIA_DRIVER_CAPABILITIES=compute,utility
export CUDA_VISIBLE_DEVICES=0

# Install NLM with CUDA support
pip install -e .[cuda]
```

### Red Hat and CentOS

#### Package Manager Installation
```bash
# Install system dependencies
sudo yum update -y
sudo yum install -y \
    gcc \
    gcc-c++ \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-devel \
    pkgconfig \
    openssl-devel \
    ffi-devel

# Create virtual environment
python3 -m venv /opt/nlm_env
source /opt/nlm_env/bin/activate

# Install NLM
pip install pynlm

# Setup NLM home directory
mkdir -p /opt/nlm
chmod 755 /opt/nlm
```

### Arch Linux

#### Package Manager Installation
```bash
# Install NLM from community repository
sudo pacman -S nlm

# Or install via pip
sudo pacman -S python-pip
pip install pynlm

# For development version
git clone https://aur.archlinux.org/nlm.git
sudo pacman -S python3-pip
pip install -e ./nlm
```

### Custom Linux Distribution

#### Generic Linux Installation
```bash
# Download NLM source
curl -L https://github.com/your-org/nlm/archive/refs/tags/v1.0.0.tar.gz -o nlm-1.0.0.tar.gz
tar -xzf nlm-1.0.0.tar.gz

# Navigate to source directory
cd nlm-1.0.0

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Install
sudo make install

# Set up Python bindings
pip install -e .
```

## macOS Installation

### Homebrew Installation

#### Using Homebrew
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install NLM dependencies
brew install cmake git python python3

# Create virtual environment
python3 -m venv ~/nlm_env
source ~/nlm_env/bin/activate

# Install NLM via pip
pip install pynlm

# Set up NLM configuration
mkdir -p ~/Library/Application Support/NLM
chmod 755 ~/Library/Application Support/NLM
```

#### Development Installation
```bash
# Clone NLM repository
cd ~
if [ ! -d "nlm" ]; then
    git clone https://github.com/your-org/nlm.git
fi

# Navigate to NLM directory
cd nlm

# Create virtual environment
python3 -m venv .venv
source .venv/bin/activate

# Install NLM in development mode
pip install -e .

# Install additional dependencies
pip install pytest pytest-cov black flake8 mypy
```

#### Apple Silicon (M1, M2, M3) Support
```bash
# For Apple Silicon systems, ensure Python is built for ARM64
# Install Python 3.8+ for ARM64
brew install python3

# Create virtual environment with ARM64
python3 -m venv --architecture arm64 ~/nlm_env
source ~/nlm_env/bin/activate

# Install NLM with Apple Silicon optimizations
pip install -e . --no-build-isolation

# Verify Apple Silicon support
python -c "import platform; print(platform.machine())"
```

### Manual macOS Installation

#### Manual Installation Steps
```bash
# Download NLM source
download https://github.com/your-org/nlm/archive/refs/tags/v1.0.0.tar.gz
 tar -xzf v1.0.0.tar.gz

cd nlm-1.0.0

# Configure with CMake
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..

# Build with optimization for macOS
make -j$(sysctl -n hw.ncpu)

# Install
make install

# Setup Python bindings
pip3 install -e .
```

## Windows Installation

### Using Chocolatey

#### Chocolatey Package Manager
```powershell
# Install Chocolatey if not already installed
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12; Invoke-Expression ((New-Object Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install NLM via Chocolatey
choco install nlm

# For development version
choco install git
choco install cmake --installargs 'ARCH=x64'

# Create virtual environment
python -m venv C:\nlm_env
C:\nlm_env\Scripts\activate

# Install NLM
pip install pynlm
```

### Using Python (WSL or native)

#### Windows Subsystem for Linux (WSL)
```bash
# Install WSL if not already installed
wsl --install

# Open WSL terminal
# Follow Linux installation instructions above
```

#### Native Windows Installation
```powershell
# Install Python (get from python.org)
# Download: https://www.python.org/downloads/windows/

# Open Command Prompt or PowerShell
python -m ensurepip --upgrade
python -m pip install --upgrade pip

# Install NLM
pip install pynlm

# Create virtual environment
python -m venv nlm_env
nlm_env\Scripts\activate.bat

# Install additional dependencies
pip install pytest pytest-cov black flake8 mypy
```

### GUI Installation (Windows)

#### Using NLM Installer
```powershell
# Download NLM installer from GitHub Releases
# Run installer with default options
# Choose installation directory
# Complete installation wizard
```

#### Manual Installation Steps
```powershell
# Download NLM source
Invoke-WebRequest -Uri "https://github.com/your-org/nlm/archive/refs/tags/v1.0.0.zip" -OutFile "nlm-1.0.0.zip"

# Extract zip file
Expand-Archive -Path "nlm-1.0.0.zip" -Destination "C:\nlm"

# Navigate to source directory
cd "C:\nlm\nlm-1.0.0"

# Configure with CMake
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64

# Build using Visual Studio
# Open Developer Command Prompt for VS 2019
cmake --build . --config Release

# Install
cmake --install . --config Release

# Setup Python bindings
cd ..
pip install -e .
```

## Cloud Installation

### AWS EC2

#### EC2 Instance Setup
```bash
# Launch EC2 instance
aws ec2 run-instances \
    --image-id ami-0c02fb55956c7d316 \
    --instance-type t3.large \
    --key-name your-key-pair \
    --security-group-ids sg-xxxxxxxx \
    --subnet-id subnet-xxxxxxxx

# Connect to instance
ssh -i your-key-pair.pem ec2-user@ec2-xx-xx-xx-xx.compute-1.amazonaws.com

# Update system
sudo yum update -y
sudo yum install -y git python3 python3-pip

# Create virtual environment
python3 -m venv ~/nlm_env
source ~/nlm_env/bin/activate

# Install NLM
pip install pynlm

# Install additional dependencies
pip install numpy scipy matplotlib
```

#### AWS Fargate (Container)
```bash
# Create ECR repository
aws ecr create-repository --repository-name nlm --region us-west-2

# Build Docker image
docker build -t nlm:latest .

# Push to ECR
docker tag nlm:latest 123456789.dkr.ecr.us-west-2.amazonaws.com/nlm:latest
docker push 123456789.dkr.ecr.us-west-2.amazonaws.com/nlm:latest

# Run container
aws ecs run-task \
    --cluster your-cluster \
    --task-definition nlm-task \
    --launch-type FARGATE \
    --network-configuration "awsvpc:={\"subnets\":["subnet-xxxxx"]}"
```

### Google Cloud Platform (GCP)

#### Google Compute Engine
```bash
# Create instance group gcloud compute instances create nlm-instance \
    --zone us-central1-a \
    --machine-type n1-standard-4 \
    --image-family ubuntu-2004-lts \
    --image-project ubuntu-os-cloud \
    --boot-disk-size 20GB \
    --scopes cloud-platform

# Connect to instance
gcloud ssh nlm-instance --zone us-central1-a

# Install dependencies
sudo apt update && sudo apt install -y python3-pip python3-venv

# Setup NLM
cd ~
python3 -m venv nlm_env
source nlm_env/bin/activate
pip install pynlm
```

#### Google Cloud Run (Container)
```bash
# Create Artifact Registry repository
gcloud artifacts repositories create docker-repo --location=us-central1 --repository-format=docker

# Build and push Docker image
docker build -t us-central1-docker.pkg.dev/your-project/nlm/nlm:latest .
docker push us-central1-docker.pkg.dev/your-project/nlm/nlm:latest

# Deploy to Cloud Run
gcloud run services create nlm-service \
    --image=us-central1-docker.pkg.dev/your-project/nlm/nlm:latest \
    --platform=managed \
    --region=us-central1 \
    --allow-unauthenticated
```

### Microsoft Azure

#### Azure Virtual Machines
```bash
# Create virtual machine
az vm create \
    --resource-group myResourceGroup \
    --name nlm-vm \
    --image Ubuntu2004 \
    --admin-username azureuser \
    --generate-ssh-keys \
    --size Standard_DS2_v2

# Connect to VM
az vm ssh --resource-group myResourceGroup --name nlm-vm --username azureuser

# Install dependencies
sudo apt update && sudo apt install -y python3-pip python3-venv

# Setup NLM
cd ~
source /home/azureuser/.bashrc
```

#### Azure Container Instances
```bash
# Create container instance
az container create \
    --resource-group myResourceGroup \
    --name nlm-container \
    --image nlm:latest \
    --registry-login-server your-registry.azurecr.io \
    --registry-username your-username \
    --registry-password your-password \
    --ports 80 \
    --ip-address public
```

## Development Environment Setup

### Virtual Environment Setup

#### Using venv
```bash
# Create virtual environment
python3 -m venv nlm_env

# Activate virtual environment
# On Linux/macOS
source nlm_env/bin/activate

# On Windows
nlm_env\Scripts\activate.bat

# Upgrade pip
pip install --upgrade pip
pip install --upgrade setuptools wheel
```

#### Using conda
```bash
# Create conda environment
conda create -n nlm python=3.9
conda activate nlm

# Install NLM
conda install -c conda-forge pynlm

# Install development dependencies
conda install pytest pytest-cov black flake8 mypy
```

### Jupyter Notebook Setup

#### Setup Jupyter with NLM
```bash
# Install Jupyter
pip install jupyter jupyterlab

# Create Jupyter kernel for NLM
python -m ipykernel install --user --name=nlm

# Start Jupyter notebook
jupyter notebook

# Start JupyterLab
jupyter lab
```

#### Example Jupyter Notebook
```python
# Save as example.ipynb
{
 "cells": [
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "# NLM Jupyter Example",
    "",
    "This is an example Jupyter notebook demonstrating NLM usage."
   ]
  },
  {
   "cell_type": "code",
   "execution_count": null,
   "metadata": {},
   "outputs": [],
   "source": [
    "import pynlm as nlm",
    "",
    "# Create brain",
    "config = nlm.createDefaultConfig()",
    "brain = nlm.createBrain(config)",
    "brain.initialize()",
    "",
    "# Run simulation",
    "for i in range(100):",
    "    brain.step(i)",
    "",
    "# Show results",
    "print(f\"Total spikes: {brain.getTotalSpikeCount()}\")"
   ]
  }
 ]
}
```

## Package Manager Installation

### Pip Installation

#### Standard Installation
```bash
# Install NLM via pip
pip install pynlm

# Install with additional dependencies
pip install pynlm[dev]
pip install pynlm[performance]
pip install pynlm[research]
```

#### Development Installation
```bash
# Install NLM in development mode
pip install -e .

# Install with all optional dependencies
pip install -e .[all]

# Install specific dependency groups
pip install -e .[cuda]
pip install -e .[gpu]
pip install -e .[web]
pip install -e .[docs]
```

### Conda Installation

#### Conda Installation
```bash
# Create conda environment
conda create -n nlm python=3.9
conda activate nlm

# Install NLM from conda-forge
conda install -c conda-forge pynlm

# For development version
conda install -c conda-forge pytorch torchvision torchaudio
conda install -c conda-forge pip
pip install -e .
```

### Package Manager Comparison

#### Package Managers

| Package Manager | Installation Speed | Dependency Resolution | Platform Support | Documentation |
|----------------|-------------------|----------------------|------------------|---------------|
| pip             | Fast              | Excellent            | All platforms    | Comprehensive |
| conda           | Medium            | Very good            | All platforms    | Good          |
| apt (Debian)    | Very fast        | Good                  | Linux only       | Good          |
| yum (RHEL)      | Fast              | Good                  | RHEL-based       | Limited       |
| brew (macOS)    | Medium            | Good                  | macOS only       | Good          |

## Build from Source

### Building NLM from Source

#### Prerequisites
```bash
# Install build dependencies
sudo apt update && sudo apt install -y \
    build-essential \
    cmake \
    git \
    python3-dev \
    pkg-config \
    libssl-dev \
    libffi-dev \
    python3-setuptools \
    python3-wheel
```

#### Build Steps
```bash
# Clone NLM repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DPYTHON_INCLUDE_DIR=$(python3-config --includes) \
    -DPYTHON_LIBRARIES=$(python3-config --ldflags)

# Build
make -j$(nproc)

# Install Python bindings
pip install -e .

# Install C++ libraries (if needed)
sudo make install
```

#### Build Options
```bash
# Configure with specific options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_BENCHMARKS=OFF \
    -DNLM_ENABLE_CUDA=OFF \
    -DPYTHON_EXECUTABLE=/usr/bin/python3 \
    -DNLM_INSTALL_PREFIX=/usr/local

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build for specific architecture
cmake .. -DCMAKE_OSX_ARCHITECTURES=x86_64
```

### Building Python Bindings

#### Building Python Bindings
```python
# Test Python bindings
python3 -c "import pynlm; print(pynlm.__version__)"

# Create test script
test_script.py
```

```python
import pynlm as nlm

# Test basic functionality
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Test simulation
for i in range(10):
    brain.step(i)

print(f"Test passed: {brain.getTotalNeuronCount()} neurons")
```

## Installation Verification

### Verification Script

Create a verification script to test NLM installation:

```python
#!/usr/bin/env python3
"""
NLM Installation Verification Script

This script verifies that NLM is properly installed and working.
"""

import sys
import platform
import pynlm as nlm

def check_python_version():
    """Check Python version."""
    print(f"Python version: {platform.python_version()}")
    
    # Check version compatibility
    major, minor, _ = map(int, platform.python_version().split('.')[:3])
    
    if major == 3 and minor >= 8:
        print("✓ Python version compatible")
        return True
    else:
        print(f"✗ Python version {platform.python_version()} not supported")
        return False

def check_nlm_import():
    """Check NLM import."""
    try:
        print(f"NLM version: {nlm.__version__}")
        print("✓ NLM imported successfully")
        return True
    except ImportError as e:
        print(f"✗ NLM import failed: {e}")
        return False

def test_basic_functionality():
    """Test basic NLM functionality."""
    try:
        # Create configuration
        config = nlm.createDefaultConfig()
        print("✓ Configuration created")
        
        # Create brain
        brain = nlm.createBrain(config)
        print("✓ Brain created")
        
        # Initialize brain
        brain.initialize()
        print("✓ Brain initialized")
        
        # Test basic operations
        for step in range(10):
            brain.step(step)
        
        # Check results
        neuron_count = brain.getTotalNeuronCount()
        firing_count = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        
        print(f"✓ Basic functionality test passed")
        print(f"  Neurons: {neuron_count}")
        print(f"  Firing neurons: {firing_count}")
        print(f"  Total spikes: {total_spikes}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def check_documentation():
    """Check documentation files."""
    import os
    
    docs_path = "docs"
    if os.path.exists(docs_path):
        print(f"✓ Documentation found in {docs_path}")
        
        # Check for key documentation files
        required_docs = ["README.md", "quick_start.md", "basic_concepts.md"]
        found_docs = []
        
        for doc in required_docs:
            if os.path.exists(os.path.join(docs_path, doc)):
                found_docs.append(doc)
        
        print(f"✓ Found {len(found_docs)}/{len(required_docs)} required documentation files")
        return len(found_docs) >= len(required_docs) / 2  # At least half required
    else:
        print("✗ Documentation not found")
        return False

def main():
    """Main verification function."""
    print("=== NLM Installation Verification ===\n")
    
    checks = [
        ("Python Version", check_python_version),
        ("NLM Import", check_nlm_import),
        ("Basic Functionality", test_basic_functionality),
        ("Documentation", check_documentation)
    ]
    
    results = []
    for name, check in checks:
        print(f"Checking {name}...")
        try:
            result = check()
            results.append((name, result))
        except Exception as e:
            print(f"✗ Check {name} failed with exception: {e}")
            results.append((name, False))
        
        print()
    
    # Summary
    print("=== Verification Summary ===")
    passed = sum(1 for _, result in results if result)
    total = len(results)
    
    for name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{name}: {status}")
    
    print(f"\nResults: {passed}/{total} checks passed")
    
    if passed == total:
        print("🎉 All checks passed! NLM is properly installed.")
        return 0
    else:
        print("❌ Some checks failed. Please fix installation issues.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
```

### Verification Steps

1. **Check Python Version**: Ensure Python 3.8+ is installed
2. **Test NLM Import**: Verify NLM can be imported
3. **Test Basic Functionality**: Run basic brain operations
4. **Check Documentation**: Verify documentation files exist

## Platform-Specific Considerations

### Linux Considerations

#### Performance Considerations
```bash
# Optimize for Linux performance
sudo sysctl -w vm.swappiness=10
sudo sysctl -w net.core.somaxconn=65535

# Set up swap file if needed
sudo fallocate -l 2G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

#### Security Considerations
```bash
# Set up secure file permissions
mkdir -p /opt/nlm
chmod 755 /opt/nlm

# Set up secure directory structure
mkdir -p /opt/nlm/{config,data,logs}
chmod 700 /opt/nlm/config
chmod 755 /opt/nlm/data
chmod 755 /opt/nlm/logs
```

### macOS Considerations

#### Compatibility
```bash
# Check for macOS compatibility
python3 -c "import platform; print(platform.system())"

# Check for required frameworks
xcode-select --install
```

#### Performance Considerations
```bash
# Optimize for macOS performance
sysctl -w hw.optional_cpu_levels=1

# Check memory usage
vm_stat
```

### Windows Considerations

#### Path Considerations
```powershell
# Use forward slashes in paths
set PYTHONPATH=%USERPROFILE%\nlm;%PYTHONPATH%

# Set up environment variables
$env:PYTHONPATH = "$env:PYTHONPATH;$env:USERPROFILE\nlm"
$env:NLM_HOME = "C:\nlm"
```

#### File Permissions
```powershell
# Set up file permissions
icacls C:\nlm /grant Users:(OI)(CI)F
icacls C:\nlm\config /grant Users:(OI)(CI)R
icacls C:\nlm\data /grant Users:(OI)(CI)W
icacls C:\nlm\logs /grant Users:(OI)(CI)R
```

## Troubleshooting Installation

### Common Installation Issues

#### Python Import Issues
```python
# Check Python path
import sys
print(sys.path)

# Check NLM module location
import pynlm
print(pynlm.__file__)

# Try to import with explicit path
import importlib.util
spec = importlib.util.spec_from_file_location("pynlm", "/path/to/pynlm.py")
pynlm = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pynlm)
```

#### Build Issues
```bash
# Clean build directory
rm -rf build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Dependency Issues
```bash
# Install missing dependencies
pip install missing_dependency

# Check for circular dependencies
pip install --no-deps package_name

# Use virtual environment
python3 -m venv test_env
source test_env/bin/activate
pip install package_name
```

#### Environment Issues
```bash
# Check Python environment
python3 --version
which python3

# Check pip version
pip --version

# Install/upgrade pip
pip install --upgrade pip
pip install --upgrade setuptools wheel
```

### Installation Help

If you encounter installation issues, please provide the following information:

1. **Platform Information**: Operating system, architecture, version
2. **Python Information**: Python version, path, environment
3. **NLM Version**: NLM version if available
4. **Error Messages**: Full error messages and stack traces
5. **Installation Steps**: Steps taken to reproduce the issue
6. **System Information**: Memory, CPU, disk space

## Platform Support Matrix

| Platform | Installation Method | Status | Notes |
|----------|-------------------|--------|-------|
| Linux (Ubuntu) | pip, apt | ✅ Supported | Best support |
| Linux (Debian) | pip, apt | ✅ Supported | Good support |
| Linux (Red Hat) | pip, yum | ✅ Supported | Good support |
| Linux (Arch) | pip, pacman | ✅ Supported | Medium support |
| macOS | pip, brew | ✅ Supported | Good support |
| Windows | pip, chocolatey | ✅ Supported | Medium support |
| AWS EC2 | pip, docker | ✅ Supported | Good support |
| Google Cloud | pip, docker | ✅ Supported | Good support |
| Azure | pip, docker | ✅ Supported | Medium support |
| Docker | docker | ✅ Supported | Excellent support |

### Platform-Specific Features

#### Linux Features
- **Native support**: Full support with native system integration
- **Package management**: Integration with apt, yum, pacman
- **Performance optimization**: Native performance optimizations
- **Container support**: Docker and container integration

#### macOS Features
- **Apple Silicon support**: ARM64 architecture support
- **Framework integration**: Integration with macOS frameworks
- **Development tools**: XCode command line tools
- **Sandbox support**: App Sandbox compatibility

#### Windows Features
- **GUI installation**: Windows installer support
- **Package manager**: Chocolatey integration
- **IDE integration**: Visual Studio integration
- **PowerShell support**: PowerShell script support

#### Cloud Features
- **AWS support**: EC2 and Fargate support
- **Google Cloud**: Compute Engine and Cloud Run support
- **Azure support**: Virtual Machines and Container Instances
- **Container support**: Docker and container orchestration

## Conclusion

The NLM platform-specific installation guides provide comprehensive installation instructions for different platforms and environments. This documentation ensures:

### Platform Coverage

1. **Linux Installation**: Ubuntu, Debian, Red Hat, CentOS, Arch Linux
2. **macOS Installation**: macOS with Homebrew and manual installation
3. **Windows Installation**: Chocolatey, Python, and GUI installation
4. **Cloud Installation**: AWS, Google Cloud, Azure, and container deployment
5. **Development Environment**: Virtual environments, Jupyter, and development tools

### Installation Methods

1. **Package Manager**: Using system package managers for quick installation
2. **pip Installation**: Python package installation with dependencies
3. **Build from Source**: Compiling from source code for custom builds
4. **Docker**: Containerized installation for consistent deployment
5. **Conda**: Conda package management for scientific Python environments

### Installation Verification

1. **Verification Scripts**: Automated installation verification
2. **Testing**: Basic functionality testing
3. **Documentation**: Documentation file verification
4. **Compatibility**: Platform compatibility checking

### Troubleshooting Support

1. **Common Issues**: Address installation problems and solutions
2. **Platform-Specific**: Platform-specific considerations and workarounds
3. **Performance**: Performance optimization and tuning
4. **Community**: Community support and resources

The NLM platform-specific installation guides provide comprehensive installation instructions across all supported platforms, ensuring smooth setup and operation in any environment.
