# NLM Troubleshooting Guide

# NLM Troubleshooting Guide

## Overview

This guide provides solutions to common issues encountered when using the NLM (Neural Learning Machine) project. It covers build problems, runtime errors, performance issues, and configuration problems.

## Build Issues

### CMake Cannot Find Python

**Error:** CMake cannot find Python interpreter or development libraries.

**Symptoms:**
- CMake errors about Python not found
- Python-related configuration failures
- Missing pybind11 or other Python dependencies

**Solutions:**

#### Solution 1: Install Python Dependencies
```bash
# Install system packages (Ubuntu/Debian)
apt-get update
apt-get install python3-dev python3-pip python3-venv

# Install Python packages
pip3 install pybind11 scikit-build-core numpy
```

#### Solution 2: Configure CMake with Python Path
```bash
# Find Python path
which python3
python3 -c "import sys; print(sys.executable)"

# Configure CMake with explicit Python path
cmake .. -DPython_EXECUTABLE=/path/to/python3
```

#### Solution 3: Build Python Bindings Separately
```bash
# Build Python bindings only
cd python
pip install -e .
```

### Compilation Errors

**Error:** Various compilation errors during build process.

**Common Causes:**
- Missing compiler support for C++20 features
- Incomplete source files
- Missing header dependencies
- Incompatible compiler versions

**Solutions:**

#### Solution 1: Check Compiler Requirements
```bash
# Check compiler version
g++ --version
clang++ --version

# Ensure C++20 support
# GCC: 9.0+ (C++20 support added in 9.1)
# Clang: 7.0+ (full C++20 support in 9.0)
```

#### Solution 2: Verify Build Dependencies
```bash
# Check for required libraries
ldconfig -p | grep -E "pthread|stdc\+\+"

# Install missing dependencies (Ubuntu/Debian)
apt-get install build-essential cmake libstdc++-9-dev
```

#### Solution 3: Clean Build
```bash
# Remove build directory and rebuild
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Linker Errors

**Error:** Linker cannot find required libraries or objects.

**Symptoms:**
- Undefined references to functions
- Missing symbols in libraries
- Multiple definition errors

**Solutions:**

#### Solution 1: Check Library Paths
```bash
# List available libraries
ldconfig -p | grep nlm

# Add library paths to CMake
cmake .. -DCMAKE_LIBRARY_PATH=/path/to/libraries
```

#### Solution 2: Update Library Dependencies
```bash
# Reinstall or update libraries
apt-get update && apt-get install --reinstall libstdc++-dev
```

#### Solution 3: Use Static Libraries
```bash
# Link against static libraries instead of dynamic
cmake .. -DBUILD_SHARED_LIBS=OFF
```

## Runtime Issues

### ImportError: No Module Named 'pynlm'

**Error:** Python cannot import the NLM module.

**Symptoms:**
- `ImportError: No module named 'pynlm'`
- Python cannot find the compiled extension

**Solutions:**

#### Solution 1: Reinstall Python Module
```bash
# Force reinstall to ensure all files are present
pip install --force-reinstall .

# Or install in development mode
pip install -e .
```

#### Solution 2: Check Build Output
```bash
# Verify the module was built
find . -name "*.so" -o -name "*.pyd" | head

# Check if pynlm.so exists
ls -la python/pynlm*.so
```

#### Solution 3: Python Path Configuration
```python
# Add the module to Python path
import sys
sys.path.append('/path/to/nlm/python')

# Or install globally
pip install .
```

### Segmentation Faults

**Error:** Program crashes with segmentation fault.

**Symptoms:**
- `Segmentation fault (core dumped)`
- Crash without clear error message
- Often occurs after running for a while

**Solutions:**

#### Solution 1: Check Initialization Order
```cpp
// Correct order in C++
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);
brain->initialize();  // MUST call initialize() before step()

// Run simulation
for (int step = 0; step < 1000; ++step) {
    brain->step(step);  // Safe after initialize()
}
```

#### Solution 2: Check Resource Limits
```bash
# Increase stack size for large simulations
ulimit -s unlimited

# Check memory usage
ulimit -m
ulimit -v
```

#### Solution 3: Enable Debug Symbols
```bash
# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run with valgrind for memory debugging
valgrind --tool=memcheck ./nlm --mode test
```

#### Solution 4: Check Configuration
```ini
# Ensure configuration is valid
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000  # Not too high
region_count = 2
```

### Memory Issues

**Error:** Excessive memory usage or out-of-memory errors.

**Symptoms:**
- High memory usage during simulation
- `Out of memory` errors
- System slowdown

**Solutions:**

#### Solution 1: Reduce Network Size
```ini
# For better memory efficiency
neuron_count = 1000
region_count = 3
connection_probability = 0.05  # Lower connectivity
```

#### Solution 2: Optimize Configuration
```ini
# Reduce memory overhead
log.level = WARN        # Less log data
log.to_file = false     # Disable file logging
log.max_size = 10       # Smaller log files
simulation.max_steps = 1000  # Fewer steps
```

#### Solution 3: Monitor Memory Usage
```python
import pynlm
import psutil
import os

def monitor_memory(brain, duration_steps=1000):
    """Monitor memory usage during simulation."""
    process = psutil.Process(os.getpid())
    
    for step in range(duration_steps):
        brain.step(step)
        
        # Check memory every 100 steps
        if step % 100 == 0:
            memory_mb = process.memory_info().rss / 1024 / 1024
            print(f"Step {step}: Memory usage = {memory_mb:.2f} MB")
            
            # Warn if memory exceeds threshold
            if memory_mb > 1000:  # 1GB threshold
                print(f"WARNING: High memory usage ({memory_mb:.2f} MB)")

# Run memory monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
monitor_memory(brain, duration_steps=1000)
```

### Performance Issues

**Error:** Slow simulation or performance degradation.

**Symptoms:**
- Long simulation times
- High CPU usage
- Frame rate drops in visualization

**Solutions:**

#### Solution 1: Optimize Configuration
```ini
# Performance optimizations
simulation_timestep = 0.01     # Larger timestep (less accuracy, faster)
log.level = WARN                # Less logging overhead
log.to_file = false             # Disable file logging
plasticity.stdp.enable = false # Disable if not critical
plasticity.hebbian.enable = false  # Disable if not critical
```

#### Solution 2: Use Release Build
```bash
# Build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Solution 3: Reduce Network Complexity
```ini
# Simplify network for better performance
neuron_count = 500
region_count = 2
connection_probability = 0.05
plasticity.stdp.learning_rate = 0.001
```

#### Solution 4: Hardware Optimization
```bash
# Use multiple CPU cores
export OMP_NUM_THREADS=$(nproc)
make -j$(nproc)

# Check system resources
free -h
df -h
```

## Configuration Issues

### Invalid Configuration Values

**Error:** Configuration contains invalid or incompatible values.

**Symptoms:**
- Unexpected simulation behavior
- Configuration parsing errors
- Simulation fails to start

**Solutions:**

#### Solution 1: Use Default Configuration
```bash
# Use default configuration
./nlm --config configs/default.cfg
```

#### Solution 2: Validate Configuration
```bash
# Create validation script
#!/bin/bash
cat > validate_config.sh << 'EOF'
#!/bin/bash

# Check if config file exists
if [ ! -f "$1" ]; then
    echo "Configuration file not found: $1"
    exit 1
fi

# Basic validation
if grep -q "neuron_count = 0" "$1"; then
    echo "ERROR: neuron_count cannot be 0"
    exit 1
fi

if grep -q "simulation_timestep = 0" "$1"; then
    echo "ERROR: simulation_timestep cannot be 0"
    exit 1
fi

echo "Configuration validation passed"
EOF

chmod +x validate_config.sh
./validate_config.sh configs/my_config.cfg
```

#### Solution 3: Fix Common Configuration Errors
| Error | Cause | Solution |
|-------|-------|----------|
| `neuron_count = 0` | Invalid value | Set to at least 100 |
| `simulation_timestep = 0` | Invalid value | Set to positive value (e.g., 0.001) |
| `plasticity.stdp.learning_rate = 0` | Invalid value | Set to positive value (e.g., 0.001) |
| `random_seed = -1` | Invalid range | Use non-negative integer |

### Configuration File Format Issues

**Error:** Configuration file format is incorrect.

**Symptoms:**
- `Config file error: Could not parse config file`
- Missing or malformed key-value pairs
- Unexpected parsing errors

**Solutions:**

#### Solution 1: Check File Format
```ini
# Correct format
key = value  # Comment

# Incorrect formats
key=value      # Missing spaces
key = value   # Multiple spaces (OK)
key = value    # Tabs (OK)
key = value    # Line breaks (problematic)
```

#### Solution 2: Use Proper Syntax
```ini
# Valid configuration example
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
plasticity.stdp.enable = true
log.level = INFO
log.to_file = true
log.filename = simulation.log
```

#### Solution 3: Validate with Parser
```cpp
// Example validation code
bool validateConfig(const std::string& configFile) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configFile)) {
        return false;
    }
    
    // Check required values
    if (!config->has("random_seed")) return false;
    if (!config->has("simulation_timestep")) return false;
    if (!config->has("neuron_count")) return false;
    
    // Validate ranges
    int64_t neurons = config->getOr<int64_t>("neuron_count", 0);
    if (neurons < 100) return false;
    
    double timestep = config->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) return false;
    
    return true;
}
```

## Python Binding Issues

### pybind11 Errors

**Error:** Various pybind11-specific errors when using Python bindings.

**Symptoms:**
- Type mismatches
- Missing symbols
- Import errors

**Solutions:**

#### Solution 1: Rebuild Python Module
```bash
# Remove existing module
pip uninstall -y pynlm

# Rebuild and install
pip install -e .
```

#### Solution 2: Check Version Compatibility
```python
# Check Python version
import sys
print(f"Python {sys.version}")

# Check pybind11 version
import pybind11
print(f"pybind11 {pybind11.__version__}")

# Check numpy version
import numpy
print(f"NumPy {numpy.__version__}")
```

#### Solution 3: Fix Type Mismatches
```cpp
// Example fix for type mismatches
py::class_<Config>(m, "Config")
    .def("set", &Config::set, py::arg("key"), py::arg("value"),
         "Set a configuration value")
    .def("get", [](Config& self, const std::string& key) {
        return self.getOr<std::string>(key, "");
    }, py::arg("key"), "Get configuration value as string");
```

### Python API Errors

**Error:** Errors when using the NLM Python API.

**Symptoms:**
- Method not found
- Parameter errors
- Return type mismatches

**Solutions:**

#### Solution 1: Check Method Signatures
```python
# Check available methods
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# List available methods
methods = [method for method in dir(brain) if not method.startswith('_')]
for method in methods:
    print(method)
```

#### Solution 2: Verify Parameter Types
```python
# Verify parameter types
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check configuration parameters
config = brain.getConfig()
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Active neurons: {brain.getFiringNeuronCount()}")
```

#### Solution 3: Use Correct Imports
```python
# Ensure correct imports
import pynlm
from pynlm import Brain, Config, createBrain, createDefaultConfig

# Use classes correctly
brain = createBrain(createDefaultConfig())
brain.initialize()
```

## Visualization Issues

### Visualization Not Starting

**Error:** Visualization window doesn't appear or crashes immediately.

**Symptoms:**
- No visualization window appears
- Visualization crashes on startup
- Empty or blank visualization

**Solutions:**

#### Solution 1: Check Visualization Dependencies
```bash
# Check for required libraries
ldconfig -p | grep -E "GLUT|OpenGL|glfw"

# Install missing dependencies (Ubuntu/Debian)
apt-get install freeglut3-dev libgl1-mesa-dev
```

#### Solution 2: Check Visualization Configuration
```ini
# Enable visualization in configuration
visualization.enabled = true
visualization.fps = 30
visualization.width = 1920
visualization.height = 1080
```

#### Solution 3: Run without Visualization
```bash
# Run demo mode without visualization
./nlm --mode demo --neurons 100 --steps 100

# Or use headless mode
./nlm --mode test
```

#### Solution 4: Check Display Environment
```bash
# Check if display is available
echo $DISPLAY

# Run with virtual display (if needed)
Xvfb :99 & export DISPLAY=:99
```

## File I/O Issues

### Cannot Save/Load Brain State

**Error:** Problems with saving or loading brain state files.

**Symptoms:**
- Save operation fails
- Load operation fails
- Corrupted save files

**Solutions:**

#### Solution 1: Check File Permissions
```bash
# Check directory permissions
ls -la /path/to/directory

# Ensure write permissions
chmod 755 /path/to/directory
```

#### Solution 2: Verify File Paths
```python
import pynlm

# Create brain and save
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use absolute path for saving
save_path = "/absolute/path/to/brain_state.nlm"
brain.save(save_path)

# Verify file was created
import os
print(f"File saved: {os.path.exists(save_path)}")
```

#### Solution 3: Check File Format
```python
# Ensure file format is correct
import pynlm

# Save and reload
brain.save("test_state.nlm")

# Create new brain and load
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.load("test_state.nlm")

# Verify they are the same
print(f"Neurons match: {brain.getTotalNeuronCount() == brain2.getTotalNeuronCount()}")
```

## Common Runtime Errors and Solutions

### Error Categories

| Error Category | Common Causes | Solutions |
|----------------|---------------|----------|
| Build Errors | Missing dependencies, compiler issues | Install dependencies, use correct compiler |
| Runtime Errors | Invalid configuration, resource limits | Validate configuration, check system resources |
| Memory Issues | Large networks, verbose logging | Reduce size, optimize configuration |
| Performance Issues | Complex plasticity, inefficient code | Simplify, use release build |
| Configuration Errors | Invalid values, file format | Use defaults, validate with parser |

### Quick Reference

#### Quick Fix Commands

```bash
# Reinstall Python bindings
pip install --force-reinstall -e .

# Rebuild with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Clear build artifacts
rm -rf build
```

#### Quick Validation Scripts

```bash
# Create quick validation script
cat > validate_nlm.sh << 'EOF'
#!/bin/bash

echo "=== NLM Quick Validation ==="

# Check executable exists
if [ ! -f "./nlm" ]; then
    echo "ERROR: NLM executable not found"
    exit 1
fi

# Run test mode quickly
echo "Running quick test..."
./nlm --mode test --neurons 100 --steps 100

if [ $? -eq 0 ]; then
    echo "✓ Basic test passed"
else
    echo "✗ Basic test failed"
    exit 1
fi

echo "✓ All validation checks passed"
EOF

chmod +x validate_nlm.sh
./validate_nlm.sh
```

## Getting Additional Help

### Online Resources

- **Documentation**: https://github.com/Kilo-Org/nlm/tree/main/docs
- **Issues**: https://github.com/Kilo-Org/nlm/issues
- **Discussions**: https://github.com/Kilo-Org/nlm/discussions

### Local Documentation

Check the following documentation files for more information:

- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions
- `docs/LIMITATIONS.md` - Current limitations
- `docs/PERFORMANCE.md` - Performance optimization guide

### Support Channels

1. **GitHub Issues**: Report bugs and request features
2. **Stack Overflow**: Ask questions with proper code examples
3. **Project Documentation**: Check README and HOW_TO_USE.md first
4. **Community Discussions**: Share experiences and solutions