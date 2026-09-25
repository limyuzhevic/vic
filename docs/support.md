# NLM Support Guide

## Overview

This guide provides comprehensive support information for Neural Learning Machine (NLM) users, including troubleshooting, common issues, getting help, and accessing resources for support and assistance.

## Getting Started with Support

### Initial Steps for New Users

#### Step 1: Read the Documentation

- **Documentation Hub**: [docs/index.md](#) - Complete overview of all documentation
- **Quick Start Guide**: Step-by-step introduction for new users
- **Python User Guide**: Comprehensive Python bindings documentation
- **Installation Guide**: Complete installation instructions

#### Step 2: Basic Troubleshooting

```bash
# 1. Check system requirements
# Ensure your system meets minimum requirements:
# - 8+ CPU cores
# - 16 GB RAM (32 GB+ recommended)
# - 100+ GB free disk space
# - CMake 3.16+
# - gcc 11+ or clang 14+

# 2. Verify build environment
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# 3. Run basic tests
ctest --output-on-failure
```

#### Step 3: Test Python Installation

```python
#!/usr/bin/env python3
"""Quick test for NLM Python installation"""
import sys
import pynlm

def test_basic_installation():
    print("Testing NLM Python installation...")
    
    # Test 1: Create configuration
    config = pynlm.createDefaultConfig()
    print("✓ Configuration created")
    
    # Test 2: Create brain
    brain = pynlm.createBrain(config)
    print("✓ Brain created")
    
    # Test 3: Initialize brain
    brain.initialize()
    print("✓ Brain initialized")
    
    # Test 4: Run simulation
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
    
    # Test 5: Check memory
    memory_stats = brain.getMemoryStats()
    print(f"✓ Memory stats: {memory_stats}")
    
    print("\nAll basic tests passed!")

if __name__ == "__main__":
    test_basic_installation()
```

## Common Issues and Solutions

### Issue Category: Installation Problems

#### Problem 1: CMake Configuration Errors

**Description**: CMake fails to configure due to missing dependencies or incorrect settings.

**Symptoms**:
```
cmake .. -DCMAKE_BUILD_TYPE=Release
-- The C++ compiler identification is unknown
-- Detecting C compiler version
-- Found CMake version 3.16.3
CMake Error at CMakeLists.txt:34 (project):
The source directory "..." does not appear to contain CMakeLists.txt.
```

**Solutions**:

**Solution 1: Check CMake version**
```bash
# Check your CMake version
cmake --version

# Upgrade if necessary
# Ubuntu: sudo apt update && sudo apt install cmake
# macOS: brew install cmake
# Windows: choco install cmake
```

**Solution 2: Check build directory**
```bash
# Ensure you're in the correct directory
cd /path/to/nlm
ls -la
```

**Solution 3: Use full path**
```bash
# Use absolute path to source directory
mkdir build
cd build
cmake /absolute/path/to/nlm
```

#### Problem 2: Missing Dependencies

**Description**: Build fails due to missing required libraries.

**Symptoms**:
```
CMake Error at CMakeLists.txt:5 (find_package):
Could NOT FIND PACKAGE OpenMP REQUIRED
```

**Solutions**:

**Solution 1: Install required packages**
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y build-essential libomp-dev git pkg-config

# macOS
xcode-select --install
brew install libomp

# Windows (with vcpkg)
# vcpkg install openmp-windows
```

**Solution 2: Use package manager**
```bash
# Conda (recommended)
conda create -n nlm-dev python=3.9
cs nlm-dev
pip install -e .

# Or install dependencies directly
conda install -c conda-forge scikit-build-core pybind11
```

#### Problem 3: Python Module Not Found

**Description**: Cannot import pynlm module after installation.

**Symptoms**:
```bash
python3 -c "import pynlm; print('Success')"
ImportError: No module named 'pynlm'
```

**Solutions**:

**Solution 1: Check Python path**
```bash
# List Python paths
python3 -c "import sys; print('\n'.join(sys.path))"

# Install package globally or in virtual environment
pip install -e .
# or
pip install pynlm
```

**Solution 2: Use virtual environment**
```bash
# Create and activate virtual environment
python -m venv nlm-env
source nlm-env/bin/activate

# Install NLM
pip install -e .

# Test installation
python3 -c "import pynlm; print('Success')"
```

**Solution 3: Manual installation**
```bash
# Add to Python path
cd /path/to/nlm/python
sys.path.append('/path/to/nlm/python')

# Import and test
python3 -c "import sys; sys.path.append('/path/to/nlm/python'); import pynlm; print('Success')"
```

### Issue Category: Runtime Problems

#### Problem 1: Brain Initialization Failures

**Description**: Brain initialization fails with error messages.

**Symptoms**:
```python
brain = pynlm.createBrain(config)
brain.initialize()
# Error: Brain initialization failed
```

**Solutions**:

**Solution 1: Check configuration**
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
print(f"Config created: {config}")

# Validate configuration
if not config.isValid():
    print("Invalid configuration!")
    # Fix configuration issues
    config.set("neuron_count", 100)
    config.set("synapse_max", 1000)
```

**Solution 2: Check system resources**
```bash
# Check available memory and CPU
free -h
nproc

# Reduce configuration if necessary
# Create lighter configuration for development
```

**Solution 3: Debug with logging**
```python
import pynlm
import logging

# Enable logging
logging.basicConfig(level=logging.DEBUG)

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with logging
for i in range(100):
    brain.step(i)
```

#### Problem 2: Simulation Errors

**Description**: Simulation fails during execution with runtime errors.

**Symptoms**:
```bash
./nlm_phase3_demo 5 200
Segmentation fault (core dumped)
```

**Solutions**:

**Solution 1: Use debug build**
```bash
# Create debug build
cd /path/to/nlm
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0"
make -j4

# Run with debugger
valgrind --tool=memcheck --leak-check=full ./nlm_phase3_demo 5 200
```

**Solution 2: Check input parameters**
```bash
# Check valid parameter ranges
# For NLM demos:
# - Number of episodes: positive integer (typically 5-50)
# - Steps per episode: positive integer (typically 100-1000)
# - Use --help for parameter descriptions
./nlm_phase3_demo --help
```

**Solution 3: Check memory limits**
```bash
# Monitor memory usage during simulation
# Use system monitoring tools
# or run with memory profiling
```

#### Problem 3: Performance Issues

**Description**: Simulation runs too slowly or hangs.

**Symptoms**:
```
# Simulation takes hours to complete
# or simulation hangs indefinitely
```

**Solutions**:

**Solution 1: Optimize configuration**
```python
import pynlm

# Create performance-optimized configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)  # Reduce neuron count
config.set("synapse_max", 5000)   # Reduce synapse count
config.set("simulation_step_size", 10)  # Increase step size
```

**Solution 2: Use performance profile**
```bash
# Run with performance profiling
cd build
perf record ./nlm_demo --profile
perf report
```

**Solution 3: Hardware optimization**
```bash
# Use multi-threading for faster compilation
# Use SSD storage for faster I/O
# Close other applications during simulation
```

### Issue Category: Python Binding Issues

#### Problem 1: Python Extension Build Failures

**Description**: Python extension fails to build from source.

**Symptoms**:
```
# During pip install -e .
# Error: Could not build extension
# Missing: Python.h or pybind11
```

**Solutions**:

**Solution 1: Check development tools**
```bash
# Install Python development headers
# Ubuntu: sudo apt install python3-dev
# macOS: brew install python
# Windows: Install Python from python.org
```

**Solution 2: Install pybind11**
```bash
# Conda
conda install -c conda-forge pybind11

# pip
pip install pybind11
```

**Solution 3: Manual build**
```bash
# Use scikit-build
pip install scikit-build-core pybind11
pip install -e .
```

#### Problem 2: Python API Errors

**Description**: Python API fails with runtime errors.

**Symptoms**:
```python
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)  # AttributeError: 'NoneType' object has no attribute 'step'
```

**Solutions**:

**Solution 1: Check object creation**
```python
import pynlm

# Test object creation
config = pynlm.createDefaultConfig()
print(f"Config type: {type(config)}")

brain = pynlm.createBrain(config)
print(f"Brain type: {type(brain)}")

if brain is None:
    print("Failed to create brain!")
    # Try alternative creation method
    brain = pynlm.Brain(config)
```

**Solution 2: Check for exceptions**
```python
import pynlm
import traceback

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
    print("Success!")
except Exception as e:
    print(f"Error: {e}")
    traceback.print_exc()
```

## Getting Help

### 1. Documentation Resources

#### Primary Documentation:

- **Documentation Hub**: [docs/index.md](#) - Complete overview of all documentation
- **Installation Guide**: [docs/installation.md](#) - Installation instructions
- **Development Guide**: [docs/development.md](#) - Development procedures
- **Contributing Guide**: [docs/contributing.md](#) - Contribution guidelines

#### User-Specific Documentation:

- **Quick Start Guide**: Step-by-step introduction for new users
- **Python User Guide**: Comprehensive Python bindings documentation
- **C++ Developer Guide**: For developers extending or modifying NLM
- **Performance User Guide**: For optimizing simulation performance
- **Configuration User Guide**: For advanced configuration management
- **Testing User Guide**: For running and developing tests

#### Reference Documentation:

- **Python API Reference**: Complete documentation for pynlm module
- **C++ API Reference**: Core documentation for C++ classes
- **Configuration Reference**: Detailed parameter documentation
- **Command-Line Reference**: All available options and their usage

### 2. Online Support Resources

#### GitHub Support:

- **Issue Tracker**: [Report bugs or request features](https://github.com/nlm-project/nlm/issues)
- **Discussions**: [Community discussions and questions](https://github.com/nlm-project/nlm/discussions)
- **Pull Requests**: [Review and contribute to improvements](https://github.com/nlm-project/nlm/pulls)

#### Community Support:

- **Slack Channel**: Join NLM Slack community
- **Discord Server**: Active development community
- **Forum**: Community Q&A and discussions

#### Email Support:

```
support@nlm-project.org
```

### 3. Support Request Process

#### Step 1: Prepare Information

```
# Collect the following information for your support request:

1. System Information:
   - Operating System (version)
   - Python version
   - NLM version
   - Hardware specifications

2. Error Information:
   - Exact error message
   - Stack trace (if available)
   - Reproduction steps
   - When the error occurred

3. Configuration Information:
   - NLM configuration file
   - Build type (Debug/Release)
   - Compiler version
   - Dependencies

4. Context Information:
   - What you were trying to accomplish
   - Expected behavior
   - Time of day and recent system activities
```

#### Step 2: Create Support Request

**Use GitHub Issues for:**
- Bug reports
- Feature requests
- Questions about the codebase

**Use Email for:**
- Urgent support requests
- Confidential information
- Detailed technical assistance

#### Step 3: Follow Up

```bash
# Example: Check GitHub issues for existing solutions
# 1. Search for similar issues
# 2. Check if there's a solution in comments
# 3. If not, create a new issue with collected information
# 4. Wait for response (usually 24-72 hours)
# 5. Provide additional information if requested
```

## Technical Support Channels

### 1. Professional Support

#### Enterprise Support Options:

```
Enterprise Support Contract:
- 24/7 technical support
- Dedicated support team
- SLA guarantees
- Priority issue resolution

Pricing: Contact sales@nlm-project.org
```

#### On-Demand Support Services:

```
Consulting Services:
- Code review and optimization
- Performance tuning
- Custom development
- Training and workshops

Pricing: Hourly rates starting from $150/hour
```

### 2. Community Support

#### Free Community Support:

- **GitHub Discussions**: Q&A and peer support
- **Slack/Discord**: Real-time chat with community
- **Forums**: Technical discussions and help
- **Stack Overflow**: Questions with NLM tags

#### Community Guidelines:

```
# 1. Be respectful and professional
# 2. Help others when possible
# 3. Share your solutions
# 4. Ask questions clearly
# 5. Document your findings
```

### 3. Advanced Support Services

#### Debugging and Analysis Services:

```
Remote Debugging:
- Live debugging sessions
- Code analysis and optimization
- Memory leak detection
- Performance profiling

Pricing: $200/hour
```

#### Custom Development Services:

```
Custom Extensions:
- Custom Python bindings
- New neural network architectures
- Specialized simulation environments
- Research-specific tools

Pricing: Project-based, starting from $5,000
```

## Troubleshooting Scripts

### 1. Diagnostic Script

```bash
#!/bin/bash
# scripts/diagnose_nlm.sh

set -e

# Function to log messages
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Function to check system requirements
check_system_requirements() {
    log "Checking system requirements..."
    
    # Check memory
    local total_mem=$(free -g | awk '/^Mem:/ {print $2}')
    if [ $total_mem -lt 16 ]; then
        log "WARNING: Low memory ($total_mem GB). Recommended: 16 GB+"
    fi
    
    # Check CPU cores
    local cpu_cores=$(nproc)
    if [ $cpu_cores -lt 4 ]; then
        log "WARNING: Low CPU cores ($cpu_cores). Recommended: 4+"
    fi
    
    # Check disk space
    local free_space=$(df -h . | awk 'NR==2 {print $4}')
    if [[ $free_space != *G ]]; then
        log "WARNING: Low disk space ($free_space). Recommended: 100+ GB"
    fi
}

# Function to check NLM installation
check_nlm_installation() {
    log "Checking NLM installation..."
    
    # Check CMake
    if ! command -v cmake &> /dev/null; then
        log "ERROR: CMake not found"
        return 1
    fi
    
    local cmake_version=$(cmake --version | head -1 | awk '{print $3}')
    log "CMake version: $cmake_version"
    
    # Check Python
    if ! command -v python3 &> /dev/null; then
        log "ERROR: Python3 not found"
        return 1
    fi
    
    local python_version=$(python3 --version 2>&1)
    log "Python version: $python_version"
    
    # Check NLM Python module
    if python3 -c "import pynlm; print('Success')" 2>/dev/null; then
        log "✓ NLM Python module available"
    else
        log "ERROR: NLM Python module not available"
        return 1
    fi
}

# Function to run basic tests
run_basic_tests() {
    log "Running basic tests..."
    
    if [ -d "build" ]; then
        cd build
        if ctest --output-on-failure -R "basic" 2>/dev/null; then
            log "✓ Basic tests passed"
        else
            log "ERROR: Basic tests failed"
            return 1
        fi
        cd ..
    else
        log "ERROR: Build directory not found"
        return 1
    fi
}

# Function to check configuration
check_configuration() {
    log "Checking configuration..."
    
    # Check for configuration files
    if [ -f "configs/default.json" ]; then
        log "✓ Default configuration found"
    else
        log "WARNING: Default configuration not found"
    fi
    
    # Check for log directory
    if [ -d "logs" ]; then
        log "✓ Log directory found"
    else
        log "WARNING: Log directory not found"
    fi
}

# Main diagnostic function
main() {
    log "Starting NLM diagnostic..."
    
    check_system_requirements
    check_nlm_installation
    run_basic_tests
    check_configuration
    
    log "Diagnostic completed."
    log "Check logs in: logs/diagnostic.log"
}

# Run main function
main
```

### 2. Error Recovery Script

```bash
#!/bin/bash
# scripts/recover_nlm.sh

set -e

# Function to log messages
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a recovery.log
}

# Function to reset build directory
reset_build() {
    log "Resetting build directory..."
    if [ -d "build" ]; then
        log "Removing existing build directory"
        rm -rf build
    fi
    
    log "Creating new build directory"
    mkdir build
    cd build
}

# Function to rebuild with debug mode
rebuild_debug() {
    log "Rebuilding in debug mode..."
    
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0"
    make -j$(nproc)
}

# Function to run with memory checking
run_with_memory_check() {
    log "Running with memory check..."
    
    valgrind --leak-check=full --track-origins=yes ./nlm_demo
}

# Main recovery function
main() {
    log "Starting NLM recovery..."
    
    # Reset build directory
    reset_build
    
    # Rebuild with debug mode
    rebuild_debug
    
    # Run tests
    if ctest --output-on-failure; then
        log "✓ All tests passed"
    else
        log "WARNING: Some tests failed. Running with memory check..."
        run_with_memory_check
    fi
    
    log "Recovery completed."
}

# Run main function
main
```

## Support SLA (Service Level Agreement)

### 1. Response Times

#### Community Support:
- **Bug Reports**: 48-72 hours
- **Feature Requests**: 1-2 weeks
- **Documentation Requests**: 1-2 weeks

#### Professional Support:
- **Standard Support**: 24 hours
- **Priority Support**: 4 hours
- **Emergency Support**: 1 hour

### 2. Resolution Times

#### Common Issues:
- **Configuration Issues**: 24 hours
- **Build Issues**: 48 hours
- **Runtime Errors**: 72 hours
- **Performance Issues**: 1 week

#### Critical Issues:
- **Segfaults**: 24 hours
- **Memory Leaks**: 48 hours
- **Data Corruption**: 24 hours

## Contact Information

### 1. Primary Support Channels

#### GitHub Support:
```
Repository: https://github.com/nlm-project/nlm
Issues: https://github.com/nlm-project/nlm/issues
Discussions: https://github.com/nlm-project/nlm/discussions
Pull Requests: https://github.com/nlm-project/nlm/pulls
```

#### Email Support:
```
support@nlm-project.org
```

### 2. Emergency Support

#### 24/7 Emergency Hotline:
```
Emergency Support: +1-800-NLM-HELP
International: +1-408-555-0123
```

### 3. Support Request Form

```
Online Support Form: https://nlm-project.org/support

Required Information:
- Name and contact information
- NLM version and configuration
- Error details and reproduction steps
- Urgency level (Low/Medium/High/Critical)
```

## Conclusion

This support guide provides comprehensive information for getting help with NLM. Whether you're a new user needing basic troubleshooting assistance or an advanced developer requiring technical support, NLM offers multiple channels to ensure you can get the help you need.

Remember to:

1. **Check documentation first** before seeking support
2. **Provide complete information** in support requests
3. **Be patient** with response times
4. **Follow up** on your support requests
5. **Share your solutions** to help others

For the most current support information and contact details, always refer to the official NLM support documentation.
