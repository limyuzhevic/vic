# NLM Troubleshooting Guide

## Overview

This comprehensive troubleshooting guide addresses common issues encountered when using NLM, from installation and build problems to runtime errors and performance issues. It provides systematic approaches to diagnosing and resolving problems, along with preventive measures.

## Problem Classification

### 1. Build and Installation Issues
Problems occur during compilation, dependency installation, or system setup.

### 2. Runtime Issues
Problems occur during simulation execution, plugin loading, or data processing.

### 3. Configuration Issues
Problems occur during setup, parameter configuration, or file I/O.

### 4. Performance Issues
Problems occur during benchmarking or large-scale simulations.

### 5. Compatibility Issues
Problems occur across different platforms, compilers, or software versions.

## Common Build Issues

### Issue 1: CMake Cannot Find Python

**Symptom**: CMake error "Could NOT find Python" or "Could NOT find Python Interpreter"

**Diagnosis**:
```bash
# Check Python installation
which python3
python3 --version

# Check for Python development headers
ls /usr/include/python3.10
```

**Solutions**:

#### Solution 1: Install Python Development Packages
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install python3-dev python3-pip

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install python3-devel

# macOS
brew install python3
```

#### Solution 2: Specify Python Path
```bash
# Configure CMake with specific Python
cmake .. -DPython_EXECUTABLE=/usr/local/bin/python3

# Or create Python symlink
sudo ln -s /usr/local/bin/python3 /usr/local/bin/python
```

#### Solution 3: Build Python with Development Flags
```bash
# Build Python from source with development flags
./configure --enable-optimizations --with-lto
make -j$(nproc)
sudo make altinstall
```

**Prevention**:
- Always check Python development packages before building
- Use virtual environments for development
- Document your Python installation for future reference

### Issue 2: Missing Required Dependencies

**Symptom**: Build errors like "error: ‘foo’ was not declared in this scope" or "undefined reference to ‘bar’"

**Diagnosis**:
```bash
# Check for common development packages
sudo apt-get install build-essential

# Check for specific NLM dependencies
ls /usr/include/{pybind11,SDL2,Eigen}
```

**Solutions**:

#### Solution 1: Install Common Dependencies
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git pkg-config

# Python bindings dependencies
sudo apt-get install python3-dev python3-pip pybind11-dev

# Scientific computing libraries
sudo apt-get install libeigen3-dev libeigen3-extras-dev
```

#### Solution 2: Use Conda for Dependency Management
```bash
# Install Miniconda (if not already installed)
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
sudo bash Miniconda3-latest-Linux-x86_64.sh

# Create environment with NLM dependencies
conda create -n nlm-env python=3.8 pip pybind11 scikit-build-core
conda activate nlm-env

# Install NLM dependencies
pip install numpy scipy matplotlib
```

#### Solution 3: Check pybind11 Version
```bash
# pybind11 version compatibility
python3 -c "import pybind11; print(pybind11.__version__)"

# Ensure minimum version
python3 -c "import pybind11; assert pybind11.__version__ >= '2.11.0'"
```

**Prevention**:
- Maintain a requirements.txt or environment.yml file
- Document all dependencies in a dependencies.md file
- Use package managers consistently

### Issue 3: Compilation Errors

**Symptom**: Various compilation errors during make

**Diagnosis**:
```bash
# Get specific error details
gmake -j4 2>&1 | grep -A5 "error:"

# Check for specific issues
gmake -j4 2>&1 | grep -i "undefined\| undeclared\| missing\| syntax"
```

**Solutions**:

#### Solution 1: Update Compiler and Libraries
```bash
# Update compiler
gcc --version
# If old version, update:
sudo apt-get install gcc-11 g++-11

# Update libraries
glib2.0
libstdc++
```

#### Solution 2: Fix Specific Error Types

**Error Type: Syntax errors**
```bash
# Check for C++ version issues
cmake .. -DCMAKE_CXX_STANDARD=20
cmake .. -DCMAKE_CXX_FLAGS="-std=c++20 -O2"
```

**Error Type: Memory errors**
```bash
# Increase stack size
ulimit -s unlimited

# Check memory availability
free -h
```

**Error Type: Linker errors**
```bash
# Check for missing libraries
g++ -v 2>&1 | grep "only --with-lib"  # If using homebrew on macOS

# Reinstall dependencies
brew install libomp
```

**Prevention**:
- Always use recent compiler versions
- Check library compatibility
- Maintain a clean build directory

## Runtime Issues

### Issue 1: Segmentation Faults

**Symptom**: Program crashes with "Segmentation fault" or "Illegal instruction"

**Diagnosis**:
```bash
# Check memory usage before crash
./nlm --neurons=10000 --steps=100 2>&1 | head -50

# Check for memory issues
valgrind --tool=memcheck ./nlm --neurons=10000 --steps=100

# Check for threading issues
gdb ./nlm
# (gdb) run --neurons=10000 --steps=100
```

**Solutions**:

#### Solution 1: Memory Issues
```bash
# Use valgrind to find memory errors
valgrind --tool=memcheck --leak-check=full ./nlm --neurons=10000

# Check for buffer overflows
valgrind --tool=memcheck --track-origins=yes ./nlm --neurons=10000

# Run with address sanitizer
export ASAN_OPTIONS=detect_leaks=1
./nlm --neurons=10000 --steps=100
```

#### Solution 2: Initialization Issues
```bash
# Check initialization order
# Ensure brain.initialize() is called before brain.step()
# Ensure config is loaded before creating brain
```

#### Solution 3: Threading Issues
```bash
# Disable threading for debugging
./nlm --threads=1 --neurons=10000

# Check for race conditions
./nlm --debug --neurons=10000
```

**Prevention**:
- Always call brain.initialize() before step()
- Use valgrind for debugging memory issues
- Test with single thread first

### Issue 2: Import Errors

**Symptom**: "ImportError: cannot import name 'pynlm'" or "ModuleNotFoundError: No module named 'pynlm'"

**Diagnosis**:
```bash
# Check Python path
python3 -c "import sys; print(sys.path)"

# Check if pynlm is installed
python3 -c "import pynlm; print(pynlm.__version__)" 2>&1

# Check installation directory
find . -name "*pynlm*" -type f
```

**Solutions**:

#### Solution 1: Reinstall Python Module
```bash
# Force reinstall
pip install --force-reinstall .

# Or reinstall from source
pip uninstall pynlm
pip install -e .
```

#### Solution 2: Check Python Environment
```bash
# Check current Python environment
which python3
python3 -m pip show pynlm

# Create virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate
pip install -e .
```

#### Solution 3: Fix Installation Path
```bash
# Check installation location
python3 -c "import pynlm; print(pynlm.__file__)"

# Reinstall if in wrong location
pip install --prefix=/usr/local .
```

**Prevention**:
- Always install in active Python environment
- Document Python installation for troubleshooting
- Use virtual environments for development

### Issue 3: Simulation Errors

**Symptom**: Runtime errors during simulation like "NaN detected" or "Infinite value"

**Diagnosis**:
```bash
# Check for numerical issues
./nlm --debug --neurons=10000 --steps=100

# Check for NaN values
python3 -c "import numpy as np; print(np.isnan(some_value))"

# Check for infinite values
python3 -c "import numpy as np; print(np.isinf(some_value))"
```

**Solutions**:

#### Solution 1: Numerical Stability
```bash
# Use double precision
./nlm --precision=double --neurons=10000

# Clamp values
export NLM_CLAMP_VALUES=1
```

#### Solution 2: Parameter Tuning
```bash
# Reduce learning rate
./nlm --learning_rate=0.0001

# Increase regularization
./nlm --regularization=0.01
```

#### Solution 3: Memory Issues
```bash
# Check memory usage
./nlm --memory_profile --neurons=100000

# Reduce memory usage
./nlm --memory_efficient --neurons=100000
```

**Prevention**:
- Always test with small-scale simulations first
- Monitor numerical stability
- Use gradual parameter adjustments

## Configuration Issues

### Issue 1: Configuration File Errors

**Symptom**: "Error loading config file" or "Invalid configuration"

**Diagnosis**:
```bash
# Check config file syntax
python3 -m json.tool config.json

# Check for missing required fields
cat config.json | grep -v "required"
```

**Solutions**:

#### Solution 1: Fix Configuration Syntax
```bash
# Use proper JSON syntax
cat > config.json << EOF
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    }
}
EOF
```

#### Solution 2: Validate Configuration
```bash
# Use NLM config validator
./nlm --validate-config config.json

# Create default config
./nlm --create-default-config > config.json
```

#### Solution 3: Check File Permissions
```bash
# Check file permissions
ls -la config.json

# Fix permissions
chmod 644 config.json
```

**Prevention**:
- Always validate JSON files before use
- Use backup configuration files
- Document configuration file format

### Issue 2: Plugin Configuration Errors

**Symptom**: "Plugin configuration error" or "Invalid plugin parameters"

**Diagnosis**:
```bash
# Check plugin configuration
./nlm --plugin-config-validate plugin_config.json

# List available plugins
./nlm --plugin-list
```

**Solutions**:

#### Solution 1: Fix Plugin Configuration
```bash
# Use proper plugin configuration format
cat > plugin_config.json << EOF
{
    "plugin_name": "my_plugin",
    "parameters": {
        "learning_rate": 0.01,
        "trace_decay": 0.1
    }
}
EOF
```

#### Solution 2: Check Plugin Dependencies
```bash
# Check if required dependencies are installed
./nlm --plugin-check-deps my_plugin

# Install missing dependencies
./nlm --plugin-install-deps my_plugin
```

#### Solution 3: Reset Plugin Configuration
```bash
# Reset to defaults
./nlm --plugin-reset my_plugin

# Recreate configuration
./nlm --plugin-create-config my_plugin
```

**Prevention**:
- Always test plugin configuration before use
- Document plugin parameter requirements
- Use configuration validation

## Performance Issues

### Issue 1: Slow Performance

**Symptom**: "Performance too slow" or "Benchmark failed"

**Diagnosis**:
```bash
# Check performance metrics
./nlm --performance-profile --neurons=10000

# Check specific bottlenecks
./nlm --benchmark --neurons=10000 --detailed
```

**Solutions**:

#### Solution 1: Performance Optimization
```bash
# Use optimized build
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..

# Enable SIMD optimizations
./nlm --simd-enabled --neurons=10000

# Use sparse connectivity
./nlm --sparse-connectivity --neurons=10000
```

#### Solution 2: Memory Optimization
```bash
# Check memory usage
./nlm --memory-profile --neurons=10000

# Use memory pools
./nlm --memory-pools --neurons=10000

# Reduce memory footprint
./nlm --memory-efficient --neurons=10000
```

#### Solution 3: Parallel Processing
```bash
# Enable multi-threading
./nlm --threads=4 --neurons=10000

# Use parallel processing
./nlm --parallel-processing --neurons=10000
```

**Prevention**:
- Always benchmark performance before optimization
- Profile to find specific bottlenecks
- Optimize one component at a time

### Issue 2: Memory Issues

**Symptom**: "Memory allocation failed" or "Out of memory"

**Diagnosis**:
```bash
# Check memory usage
./nlm --memory-usage --neurons=10000

# Check for memory leaks
valgrind --leak-check=full ./nlm --neurons=10000
```

**Solutions**:

#### Solution 1: Memory Management
```bash
# Use memory pools
./nlm --memory-pools --neurons=10000

# Check for memory leaks
./nlm --memory-leak-check --neurons=10000

# Reduce memory usage
./nlm --memory-efficient --neurons=10000
```

#### Solution 2: Resource Management
```bash
# Check system resources
free -h

# Close other applications
halt

# Use swap space if needed
swapon /dev/swap
```

#### Solution 3: Configuration Optimization
```bash
# Reduce memory usage
./nlm --reduce-memory --neurons=10000

# Use memory compression
./nlm --memory-compression --neurons=10000
```

**Prevention**:
- Always monitor memory usage during benchmarking
- Check system resources before running
- Use memory profiling tools

## Compatibility Issues

### Issue 1: Cross-Platform Issues

**Symptom**: Different behavior on different platforms

**Diagnosis**:
```bash
# Check platform information
uname -a

# Check compiler version
g++ --version

# Check for platform-specific issues
./nlm --platform-check
```

**Solutions**:

#### Solution 1: Platform-Specific Configuration
```bash
# macOS specific
export DYLD_LIBRARY_PATH=/usr/local/lib

# Linux specific
export LD_LIBRARY_PATH=/usr/local/lib

# Windows specific
set PATH=%PATH%;C:
```

#### Solution 2: Cross-Compiler Compatibility
```bash
# Use compatible compiler versions
export CXX=g++-11

# Check for compiler-specific features
./nlm --compiler-check
```

**Prevention**:
- Always test on target platform
- Document platform-specific requirements
- Use cross-platform build systems

### Issue 2: Python Version Issues

**Symptom**: Different behavior on different Python versions

**Diagnosis**:
```bash
# Check Python version
python3 --version

# Check for version-specific issues
./nlm --python-version-check
```

**Solutions**:

#### Solution 1: Use Compatible Python Version
```bash
# Use recommended Python version
python3 --version  # 3.8 or higher

# Create virtual environment
python3 -m venv nlm_env
```

#### Solution 2: Python Compatibility Layer
```bash
# Use compatibility layer
./nlm --python-compatibility

# Check for version-specific features
./nlm --python-version-11
```

**Prevention**:
- Always test on target Python version
- Use virtual environments
- Document Python version requirements

## Plugin-Specific Issues

### Issue 1: Plugin Loading Issues

**Symptom**: "Plugin load failed" or "Plugin initialization failed"

**Diagnosis**:
```bash
# Check plugin logs
./nlm --plugin-log-level=debug --plugin-list

# Check for plugin errors
./nlm --plugin-error-check
```

**Solutions**:

#### Solution 1: Fix Plugin Loading
```bash
# Check plugin file integrity
file /path/to/plugin.so

# Recompile plugin
./nlm --recompile-plugin

# Check plugin dependencies
./nlm --plugin-check-deps
```

#### Solution 2: Plugin Configuration
```bash
# Fix plugin configuration
./nlm --plugin-config-fix

# Reinstall plugin
./nlm --plugin-reinstall
```

**Prevention**:
- Always test plugin loading
- Use plugin integrity checking
- Document plugin requirements

### Issue 2: Plugin Performance Issues

**Symptom**: "Plugin performance too slow" or "Plugin benchmark failed"

**Diagnosis**:
```bash
# Check plugin performance
./nlm --plugin-performance-profile

# Check for performance bottlenecks
./nlm --plugin-benchmark
```

**Solutions**:

#### Solution 1: Plugin Performance Optimization
```bash
# Optimize plugin code
./nlm --plugin-optimize

# Check for performance bottlenecks
./nlm --plugin-profile
```

#### Solution 2: Resource Management
```bash
# Check plugin resource usage
./nlm --plugin-resource-check

# Optimize plugin resource usage
./nlm --plugin-optimize-resources
```

**Prevention**:
- Always benchmark plugin performance
- Profile plugin to find bottlenecks
- Optimize plugin code

## Advanced Troubleshooting

### 1. Debugging Techniques

#### GDB Debugging
```bash
# Start GDB
gdb ./nlm

# Run simulation
gdb> run --neurons=10000 --steps=100

# Check for segmentation faults
gdb> bt

# Check memory
gdb> info proc mappings

# Check threads
gdb> info threads
```

#### Valgrind Debugging
```bash
# Run with Valgrind
valgrind --tool=memcheck ./nlm --neurons=10000

# Check for memory leaks
valgrind --leak-check=full ./nlm --neurons=10000

# Check for buffer overflows
valgrind --track-origins=yes ./nlm --neurons=10000
```

### 2. Profiling Techniques

#### Performance Profiling
```bash
# Run with performance profiling
./nlm --performance-profile --neurons=10000

# Check for performance bottlenecks
./nlm --benchmark --detailed --neurons=10000
```

#### Memory Profiling
```bash
# Run with memory profiling
valgrind --tool=cachegrind ./nlm --neurons=10000

# Check memory usage
./nlm --memory-profile --neurons=10000
```

### 3. System-Level Debugging

#### Check System Resources
```bash
# Check CPU usage
sar -u 1 10

# Check memory usage
sar -r 1 10

# Check disk I/O
sar -b 1 10

# Check network I/O
sar -n DEV 1 10
```

#### Check System Configuration
```bash
# Check kernel parameters
sysctl -a

# Check file descriptors
ulimit -n

# Check process limits
halt
```

## Prevention and Best Practices

### 1. Pre-Build Checks

#### Check Dependencies
```bash
# Check for required dependencies
./nlm --check-dependencies

# Install missing dependencies
./nlm --install-dependencies

# Update dependencies
./nlm --update-dependencies
```

#### Check System Configuration
```bash
# Check system configuration
./nlm --check-system

# Fix system configuration
./nlm --fix-system

# Test system configuration
./nlm --test-system
```

### 2. Pre-Run Checks

#### Check Configuration
```bash
# Check configuration syntax
./nlm --validate-config

# Check configuration values
./nlm --check-config-values

# Fix configuration issues
./nlm --fix-config
```

#### Check Resources
```bash
# Check system resources
./nlm --check-resources

# Check for available resources
./nlm --check-available-resources

# Allocate resources
./nlm --allocate-resources
```

### 3. Best Practices

#### 1. Always Test with Small Scale
```bash
# Test with small scale
./nlm --neurons=1000 --steps=100

# Test with medium scale
./nlm --neurons=10000 --steps=100
```

#### 2. Monitor Performance
```bash
# Monitor performance
./nlm --performance-monitor

# Check for performance issues
./nlm --performance-check
```

#### 3. Use Appropriate Configurations
```bash
# Use appropriate configurations
./nlm --config=development
./nlm --config=production

# Check for configuration issues
./nlm --config-check
```

#### 4. Document Issues
```bash
# Document issues
./nlm --document-issue

# Check documentation
./nlm --check-documentation
```

## Emergency Procedures

### 1. Immediate Problem Resolution

#### Stop Simulation
```bash
# Stop simulation
./nlm --stop-simulation

# Force stop
./nlm --force-stop
```

#### Clear Resources
```bash
# Clear memory
./nlm --clear-memory

# Clear disk
./nlm --clear-disk

# Clear network
./nlm --clear-network
```

### 2. System Recovery

#### Check System Status
```bash
# Check system status
./nlm --system-status

# Check for errors
./nlm --check-errors

# Fix system
./nlm --fix-system
```

#### Restore System
```bash
# Restore system
./nlm --restore-system

# Check system
./nlm --check-system
```

## Conclusion

The NLM Troubleshooting Guide provides comprehensive instructions for diagnosing and resolving common issues encountered when using NLM. It covers:

1. **Build and Installation Issues**: Problems during compilation and dependency installation
2. **Runtime Issues**: Problems during simulation execution and plugin loading
3. **Configuration Issues**: Problems during setup and parameter configuration
4. **Performance Issues**: Problems during benchmarking and large-scale simulations
5. **Compatibility Issues**: Problems across different platforms and software versions

By following these guidelines, users can effectively diagnose and resolve issues with NLM, ensuring smooth and efficient operation of the simulation system.

For further assistance, refer to:
- NLM documentation
- Plugin development guides
- Performance optimization manuals
- System administration guides