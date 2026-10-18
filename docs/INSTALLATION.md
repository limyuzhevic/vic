# NLM - Installation and Setup Guide

## Overview

This guide covers everything you need to install, build, and use the NLM (Neural Learning Machine) neural simulation framework. The project provides both C++ libraries and Python bindings for creating and running neural simulations.

## Prerequisites

### System Requirements
- **Operating System**: Linux, macOS, or Windows (with WSL)
- **Memory**: At least 4GB RAM (8GB+ recommended for complex simulations)
- **Disk Space**: 500MB+ for build artifacts and simulations

### Software Dependencies

#### For C++ Development
```bash
# Ubuntu/Debian
 sudo apt-get update
sudo apt-get install build-essential cmake git pkg-config

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake git pkg-config

# macOS
brew install cmake git pkg-config

# Windows (Visual Studio + WSL)
# Follow Microsoft documentation for WSL setup
```

#### For Python Bindings
```bash
# Python package manager
pip install pybind11 scikit-build-core numpy pytest

# Optional: Python development headers (for some platforms)
sudo apt-get install python3-dev
brew install python
```

#### For Advanced Features
```bash
# For visualization or additional tools
pip install matplotlib scipy pandas
```

## Installation Methods

### Method 1: Quick Install (Recommended for Users)

This method installs the Python package with pre-built binaries where available:

```bash
# Navigate to project directory
cd /path/to/nlm

# Install the Python package
pip install .

# Or install in development mode (for contributors)
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

### Method 2: Full Build from Source (Recommended for Developers)

This method builds the complete C++ library and Python bindings:

```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake (choose your build type)
cmake .. -DCMAKE_BUILD_TYPE=Release
# Or for debugging:
# cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build everything
make -j$(nproc)

# Run tests (optional but recommended)
ctest --output-on-failure

# Install the Python bindings (if using pip install . didn't work)
pip install .
```

### Method 3: Python Development Setup

For working on Python bindings specifically:

```bash
cd /path/to/nlm

# Create virtual environment (recommended)
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install dependencies
pip install pybind11 scikit-build-core numpy pytest matplotlib

# Install in development mode
pip install -e .

# Test installation
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('Basic test passed!')"
```

## Building the C++ Components

### Building with CMake

#### Basic Build
```bash
cd /path/to/nlm
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Build Options

**Debug Build** (with symbols and assertions):
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

**Release Build** (optimized):
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Custom Installation**:
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
```

#### Available Targets
After building, you'll have:

```
Build outputs:
├── nlm                    # Main simulation executable
├── nlm_core               # Core brain library
├── nlm_agent              # Agent system library
├── nlm_world              # World simulation library
├── nlm_phase3_demo         # Phase 3 demonstration
├── nlm_phase4_demo         # Phase 4 demonstration
├── nlm_test               # Unit test suite
└── libpython3.X/site-packages/pynlm  # Python bindings
```

### Building Python Bindings Specifically

If you only need the Python bindings:

```bash
cd /path/to/nlm
pip install .
```

This uses scikit-build-core to build the Python bindings automatically.

## Verification

### Test Installation

#### Basic Functionality Test
```bash
python -c "
import pynlm
print('NLM version:', pynlm.__version__)

# Test basic brain creation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f'Created brain with {brain.getTotalNeuronCount()} neurons')

# Test simulation step
brain.step(0)
print(f'After step 0: {brain.getFiringNeuronCount()} neurons firing')
print('Basic installation test PASSED')
"
```

#### Comprehensive Test Suite
```bash
# Run unit tests
pytest tests/

# Or run the C++ test executable
./build/nlm_test
```

#### Example Tests
```bash
# Test example scripts
python examples/basic_brain.py
python examples/agent_world.py
python examples/learning_loop.py
```

## Configuration

### Environment Variables

```bash
# Set simulation parameters
export NLM_DEFAULT_NEURON_COUNT=1000
export NLM_DEFAULT_SYNAPSE_DENSITY=0.1

# Set debug/verbose output
export NLM_DEBUG=1
export NLM_LOG_LEVEL=DEBUG
```

### Configuration Files

Configuration files can be placed in:
- `~/.config/nlm/` (user-specific)
- `./configs/` (project-specific)

Example config file (`configs/my_config.json`):
```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

Load configuration:
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("configs/my_config.json")
brain = pynlm.createBrain(config)
```

## Development Setup

### For Contributors

#### Complete Development Environment
```bash
cd /path/to/nlm

# Create and activate virtual environment
python -m venv nlm_dev
source nlm_dev/bin/activate

# Install development dependencies
pip install -e .[dev]

# Install additional tools
pip install pre-commit black flake8 mypy

# Set up git hooks (optional)
pre-commit install
```

#### Running Tests
```bash
# Run Python tests
pytest tests/ -v

# Run C++ tests (if built)
./build/nlm_test

# Run example scripts
python examples/basic_brain.py
```

#### Code Quality
```bash
# Format code
black src/ python/ examples/

# Check linting
flake8 src/ python/ examples/

# Type checking (if using mypy)
mypy src/
```

### Build Troubleshooting

#### Common Issues and Solutions

**Issue 1: CMake cannot find Python**
```bash
# Solution:
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Issue 2: Compiler errors**
```bash
# Solution: Ensure you have a C++20 compatible compiler
# Check with:
g++ --version
# Should be GCC 10+ or Clang 12+
```

**Issue 3: Python import error**
```bash
# Solution:
pip install --force-reinstall .
```

**Issue 4: Memory during build**
```bash
# Solution: Increase swap or use smaller build
export MAKEFLAGS="-j$(nproc)"
# Or build with fewer cores:
make -j4
```

## Advanced Installation Options

### Custom Build Types

#### Fast Build (Limited Features)
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_FAST_BUILD=ON
```

#### Debug Build (All Features)
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_DEBUG=ON
```

#### Production Build (Optimized)
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_OPT_LEVEL=3
```

### Cross-Platform Builds

#### Building for Different Platforms
```bash
# For different architectures
cmake .. -DCMAKE_CXX_FLAGS="-march=native -O3"

# For static linking
cmake .. -DBUILD_SHARED_LIBS=OFF

# For minimal dependencies
cmake .. -DNLM_MINIMAL=ON
```

## Post-Installation Setup

### Create Development Workspace
```bash
# Create directories for your projects
mkdir -p ~/nlm_projects/{examples,tutorials,experiments}

# Create configuration directory
mkdir -p ~/.config/nlm

# Set up Git (if contributing)
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

### Initialize Project
```bash
# Create a new NLM project
mkdir my_project
cd my_project
cp ../examples/basic_brain.py .
python basic_brain.py
```

## Migration Guide

### From Previous Versions

If you're upgrading from an older version of NLM:

1. **Configuration**: Old configuration formats may need updating
2. **API Changes**: Some method signatures have been refined
3. **Dependencies**: New version requirements

**Migration Steps**:
```bash
# Backup old configuration
mv ~/.config/nlm ~/config/nlm_backup

# Update to new version
pip install --upgrade pynlm

# Test your code
python your_script.py
```

## Support and Community

### Getting Help
- **Documentation**: Read this guide and `docs/` directory
- **Examples**: Check `examples/` directory
- **Issues**: Report bugs on GitHub
- **Discussions**: Join community forums

### Common Questions

**Q: Why use NLM?**
A: NLM provides a brain-inspired neural simulation framework that integrates multiple cognitive systems (memory, prediction, learning, development) into a coherent artificial brain.

**Q: Who should use NLM?**
A: Researchers, students, and developers interested in neural computation, artificial intelligence, and brain-inspired computing.

**Q: What's different from other frameworks?**
A: NLM focuses on developmental cognitive architecture rather than simple deep learning, with integrated memory systems and neuromodulation.

**Q: Can I use NLM commercially?**
A: Yes, under the MIT license. See `LICENSE` file for details.

### Resources

- **Documentation**: `docs/` directory
- **Examples**: `examples/` directory  
- **Source Code**: `src/` and `python/` directories
- **Tests**: `tests/` directory
- **Issues**: Project GitHub repository
- **Contributing**: `CONTRIBUTING.md` (if available)

## Quick Reference

### Installation Commands
```bash
# Quick install (users)
pip install .

# Development setup (contributors)
python -m venv nlm_dev
source nlm_dev/bin/activate
pip install -e .[dev]

# Full build (if needed)
mkdir build && cd build && cmake .. && make -j$(nproc)
```

### Verification Commands
```bash
# Test installation
python -c "import pynlm; print('NLM installed successfully')"

# Run tests
pytest tests/

# Run examples
python examples/basic_brain.py
```

### Getting Help
```bash
# Documentation
ls docs/

# Examples
ls examples/

# Common issues
# See troubleshooting section above
```

## License

NLM is distributed under the MIT License. See `LICENSE` file for details.

---

*Last Updated: $(date +%Y-%m-%d)*
*Version: 0.1.0*
