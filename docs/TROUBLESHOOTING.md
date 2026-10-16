# NLM Troubleshooting Guide

## Overview

This document provides solutions to common issues encountered when using NLM (Neural Learning Machine). It covers problems ranging from simple configuration errors to complex simulation issues, with step-by-step solutions and preventive measures.

## Table of Contents

1. [Getting Started Issues](#getting-started-issues)
2. [Configuration Problems](#configuration-problems)
3. [Simulation Issues](#simulation-issues)
4. [Performance Problems](#performance-problems)
5. [Memory and Resource Issues](#memory-and-resource-issues)
6. [Learning and Plasticity Issues](#learning-and-plasticity-issues)
7. [Advanced Troubleshooting](#advanced-troubleshooting)
8. [Debugging Techniques](#debugging-techniques)
9. [Community Support](#community-support)

## Getting Started Issues

### Issue 1: "Failed to initialize brain!"

**Problem**: Brain initialization fails immediately after creation.

**Common Causes**:
1. Configuration validation errors
2. Missing required parameters
3. Invalid parameter values
4. Insufficient memory
5. Corrupted installation

**Solutions**:

#### Solution 1: Check Configuration Validity
```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()

# Set parameters one by one to identify the problematic one
config.set("brain.neuron_count", 100, pynlm.ConfigSource.Runtime)
config.set("simulation_timestep", 0.001, pynlm.ConfigSource.Runtime)

# Validate configuration
if not config.has("brain.neuron_count"):
    print("Missing brain.neuron_count")
if not config.has("simulation_timestep"):
    print("Missing simulation_timestep")

# Check parameter ranges
neuron_count = config.getOr[int]("brain.neuron_count", 0)
if neuron_count < 10 or neuron_count > 100000:
    print(f"Invalid neuron count: {neuron_count}")
    
timestep = config.getOr[float]("simulation_timestep", 0.0)
if timestep <= 0.0 or timestep > 1.0:
    print(f"Invalid timestep: {timestep}")

# Create brain after validation
brain = pynlm.createBrain(config)
if (!brain.initialize()) {
    print("Brain initialization failed!")
    // Debug further
}
```

#### Solution 2: Use Safe Configuration
```python
# Use minimal working configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100, pynlm.ConfigSource.Default)
config.set("simulation_timestep", 0.001, pynlm.ConfigSource.Default)

brain = pynlm.createBrain(config)
if (!brain.initialize()) {
    // Debug with detailed logging
    // Check system resources
    // Verify installation
}
```

#### Solution 3: System Resource Check
```python
import psutil
import pynlm

# Check available memory
memory = psutil.virtual_memory()
if memory.available < 1024 * 1024 * 1024:  // Less than 1GB
    print("Insufficient memory for simulation")
    // Try with fewer neurons or disable memory systems

# Check CPU resources
cpu_count = psutil.cpu_count()
if cpu_count < 2:
    print("System has limited CPU resources")
    // Optimize for single-core performance

# Create configuration with available resources
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", min(1000, memory.available // 10000), pynlm.ConfigSource.Default)
```

### Issue 2: "ImportError: No module named 'pynlm'"

**Problem**: Python module cannot be imported.

**Common Causes**:
1. Installation not completed
2. Python path issues
3. Missing dependencies
4. Environment conflicts

**Solutions**:

#### Solution 1: Reinstall Python Module
```bash
# Force reinstall
pip install --force-reinstall .

# Or install in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

#### Solution 2: Check Python Environment
```bash
# Check Python version
python --version

# Check pip version
pip --version

# List installed packages
pip list | grep pynlm

# Check if module files exist
find . -name "*.so" -o -name "*.pyd" | head -10
```

#### Solution 3: Fix Python Path
```python
import sys
print("Python path:", sys.path)

import pynlm
print("pynlm location:", pynlm.__file__)
```

#### Solution 4: Install Dependencies
```bash
# Install required dependencies
pip install pybind11 scikit-build-core numpy pytest

# Install from source
pip install .

# Or install specific build
python -m pip install --upgrade build
python -m build
pip install dist/*.whl
```

### Issue 3: "CMake cannot find Python"

**Problem**: CMake build fails to find Python interpreter.

**Common Causes**:
1. Python not installed
2. Multiple Python versions
3. PATH issues
4. CMake version too old

**Solutions**:

#### Solution 1: Install Python
```bash
# Ubuntu/Debian
 sudo apt-get update
sudo apt-get install python3 python3-pip python3-dev

# macOS
brew install python

# Windows (via Chocolatey)
choco install python
```

#### Solution 2: Set Python Executable
```bash
# Find Python executable
which python3
python3 --version

# Set CMake Python path
cmake .. -DPython_EXECUTABLE=$(which python3)

# Or specify version
cmake .. -DPython3_EXECUTABLE=/usr/bin/python3
```

#### Solution 3: Install Dependencies First
```bash
# Install build dependencies
pip install pybind11 scikit-build-core

# Then run CMake
cmake .. -DPython_EXECUTABLE=$(which python3)
make -j$(nproc)
```

#### Solution 4: Use Virtual Environment
```bash
# Create virtual environment
python -m venv venv
source venv/bin/activate

# Install dependencies
pip install pybind11 scikit-build-core

# Build
cmake ..
make -j$(nproc)

# Install in virtual environment
pip install -e .
```

## Configuration Problems

### Issue 1: "Configuration validation failed for key: [key]"

**Problem**: Configuration parameter validation fails.

**Common Causes**:
1. Parameter value out of range
2. Parameter not registered
3. Invalid parameter type
4. Required parameter missing

**Solutions**:

#### Solution 1: Check Parameter Validation
```python
import pynlm

config = pynlm.createDefaultConfig()

# Try setting parameter with error handling
try {
    config.set("brain.time_constant", 50.0f, pynlm.ConfigSource.File);
    print("Time constant set successfully");
} catch (const std::runtime_error& e) {
    print(f"Validation error: {e.what()}");
    // Parameter may be out of range
    // Check valid range in documentation
}

// Check parameter metadata
std::string paramName = "brain.time_constant";
if (config.has(paramName)) {
    auto value = config.get<float>(paramName);
    print(f"Current value: {value}")
}
```

#### Solution 2: Use Configuration Documentation
```python
# Check valid parameter ranges
valid_ranges = {
    "brain.time_constant": (1.0f, 100.0f),
    "brain.resting_potential_default": (-90.0f, -50.0f),
    "brain.threshold_default": (-80.0f, -40.0f),
    "plasticity.stdp.learning_rate": (0.0f, 1.0f),
    "neuromod.dopamine.scale": (0.0f, 2.0f)
}

# Validate parameters
def validate_parameter(key, value):
    if key in valid_ranges:
        min_val, max_val = valid_ranges[key]
        return min_val <= value && value <= max_val
    return True
```

#### Solution 3: Use Command Line Override
```bash
# Override problematic parameters with command line
./nlm --brain.time_constant=50.0 --plasticity.stdp.learning_rate=0.01
```

### Issue 2: "Required configuration parameter missing"

**Problem**: Configuration is missing required parameters.

**Common Causes**:
1. Configuration file format incorrect
2. Parameter names misspelled
3. Missing required parameters
4. Configuration file corrupted

**Solutions**:

#### Solution 1: Check Configuration File
```bash
# View configuration file
less configs/default.cfg

# Check for required parameters
cat configs/default.cfg | grep -E "(random_seed|simulation_timestep|neuron_count)"
```

#### Solution 2: Use Default Configuration
```python
# Always use default configuration as fallback
config = pynlm.createDefaultConfig()

# Load from file if available
if config.loadFromFile("configs/my_config.cfg") {
    print("Configuration loaded from file");
} else {
    print("Using default configuration");
    // Set any required parameters manually
    config.set("brain.neuron_count", 1000, pynlm.ConfigSource.Default);
}
```

#### Solution 3: Validate Configuration Before Use
```python
bool validate_configuration(const std::shared_ptr<Config>& config) {
    // Check required parameters
    std::vector<std::string> required = {"brain.neuron_count", "simulation_timestep"};
    
    for (const auto& param : required) {
        if (!config->has(param)) {
            std::cout << "Missing required parameter: " << param << std::endl;
            return false;
        }
    }
    
    // Check parameter values
    int neuron_count = config->getOr<int>("brain.neuron_count", 0);
    if (neuron_count < 10) {
        std::cout << "Ne neuron count too small: " << neuron_count << std::endl;
        return false;
    }
    
    return true;
}
```

### Issue 3: "Parameter type mismatch"

**Problem**: Configuration parameter has wrong type.

**Common Causes**:
1. Setting string as float
2. Setting bool as int
3. Type conversion issues
4. Configuration file format wrong

**Solutions**:

#### Solution 1: Check Parameter Types
```python
import pynlm

config = pynlm.createDefaultConfig()

# Check parameter types
def check_parameter_type(key, expected_type):
    if config.has(key) {
        auto value = config.get(key);  // Returns std::optional<T>
        if (value) {
            // Check if type matches
            return true;
        }
    }
    return false;

// Example checks
if check_parameter_type("brain.neuron_count", "int") {
    neuron_count = config.getOr[int]("brain.neuron_count", 0);
    print(f"Neuron count (int): {neuron_count}");
}

if check_parameter_type("simulation_timestep", "float") {
    timestep = config.getOr[float]("simulation_timestep", 0.0);
    print(f"Timestep (float): {timestep}");
}
```

#### Solution 2: Correct Type Usage
```python
# Correct way to set parameters
config.set("brain.neuron_count", 1000, pynlm.ConfigSource.File)  // int
config.set("simulation_timestep", 0.001, pynlm.ConfigSource.File)  // float
config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)  // bool

# Avoid type confusion
config.set("brain.time_constant", "50.0", pynlm.ConfigSource.File)  // Wrong - string
config.set("brain.time_constant", 50.0, pynlm.ConfigSource.File)  // Correct - float
```

## Simulation Issues

### Issue 1: "Brain is not learning"

**Problem**: Simulation runs but no learning occurs.

**Common Causes**:
1. Plasticity rules disabled
2. No correlated activity
3. Learning rates too small
4. Network not connected

**Solutions**:

#### Solution 1: Check Plasticity Rules
```python
import pynlm

config = pynlm.createDefaultConfig()

// Enable all plasticity rules
config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)
config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.File)
config.set("plasticity.structural.enable", True, pynlm.ConfigSource.File)

// Check plasticity parameters
if config.getOr[bool]("plasticity.stdp.enable", False) {
    learning_rate = config.getOr[float]("plasticity.stdp.learning_rate", 0.001);
    print(f"STDP learning rate: {learning_rate}");
}
```

#### Solution 2: Check Network Connectivity
```python
// Create brain and check connections
brain = pynlm.createBrain(config);
brain.initialize();

// Get regions and check connectivity
region = brain.getRegion(0);
if (region) {
    auto neurons = region->getAllNeurons();
    std::cout << "Number of neurons: " << neurons.size() << std::endl;
    
    auto synapses = region->getSynapses();
    std::cout << "Number of synapses: " << synapses.size() << std::endl;
    
    // Check if neurons are connected
    if (neurons.size() > 1 && synapses.size() == 0) {
        std::cout << "WARNING: Neurons but no synapses - check connectivity probability" << std::endl;
    }
}
```

#### Solution 3: Increase Learning Parameters
```python
// Increase learning rates for visible learning
config.set("plasticity.stdp.learning_rate", 0.01, pynlm.ConfigSource.File)
config.set("plasticity.hebbian.learning_rate", 0.05, pynlm.ConfigSource.File)

// Increase structural plasticity rate
config.set("plasticity.structural.synaptogenesis_rate", 0.001, pynlm.ConfigSource.File)
```

### Issue 2: "Simulation too fast (no activity)"

**Problem**: Neurons fire very little or not at all.

**Common Causes**:
1. No input to neurons
2. Threshold too high
3. Membrane time constant too small
4. Network too sparse

**Solutions**:

#### Solution 1: Check Input Sources
```python
// Inject current into neurons to stimulate activity
brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
brain->injectCurrentToNeurons(NeuronType::Internal, 30.0f);
```

#### Solution 2: Adjust Neuron Parameters
```python
// Adjust neuron parameters for better activity
config.set("brain.threshold_default", -45.0f, pynlm.ConfigSource.File)  // Lower threshold
config.set("brain.time_constant", 50.0f, pynlm.ConfigSource.File)  // Slower dynamics
config.set("brain.leak_conductance_default", 20.0f, pynlm.ConfigSource.File)  // Less leak
```

#### Solution 3: Increase Network Size
```python
// Larger network produces more activity
config.set("brain.neuron_count", 2000, pynlm.ConfigSource.File)
config.set("region_count", 2, pynlm.ConfigSource.File)
```

### Issue 3: "Simulation too slow"

**Problem**: Simulation runs very slowly.

**Common Causes**:
1. Too many neurons
2. Too many connections
3. Expensive plasticity rules
4. Inefficient timestep

**Solutions**:

#### Solution 1: Optimize Configuration
```python
// Optimize for speed
config.set("brain.neuron_count", 500, pynlm.ConfigSource.File)  // Fewer neurons
config.set("region_count", 1, pynlm.ConfigSource.File)  // Single region
config.set("simulation_timestep", 0.01, pynlm.ConfigSource.File)  // Larger timestep
config.set("plasticity.structural.enable", False, pynlm.ConfigSource.File)  // Disable structural plasticity
config.set("memory.episodic.enabled", False, pynlm.ConfigSource.File)  // Disable episodic memory
config.set("cognitive.attention.enabled", False, pynlm.ConfigSource.File)  // Disable attention
```

#### Solution 2: Use Performance Mode
```python
// Create performance-optimized configuration
auto perf_config = pynlm.createDefaultConfig();
perf_config.set("brain.neuron_count", 1000, pynlm.ConfigSource.File);
perf_config.set("simulation_timestep", 0.005, pynlm.ConfigSource.File);
perf_config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File);
perf_config.set("plasticity.hebbian.enable", False, pynlm.ConfigSource.File);  // Disable expensive Hebbian
```

### Issue 4: "NaN or infinite values"

**Problem**: Simulation produces NaN or infinite values.

**Common Causes**:
1. Numerical instability
2. Invalid parameter values
3. Memory corruption
4. Division by zero

**Solutions**:

#### Solution 1: Check Numerical Stability
```python
// Ensure parameter values are valid and within bounds
config.set("brain.time_constant", 20.0f, pynlm.ConfigSource.File)  // Valid range: 1.0-100.0
config.set("brain.resting_potential_default", -70.0f, pynlm.ConfigSource.File)  // Valid range: -90.0 to -50.0
config.set("brain.threshold_default", -55.0f, pynlm.ConfigSource.File)  // Valid range: -80.0 to -40.0
```

#### Solution 2: Use Stable Algorithm
```python
// Use stable simulation parameters
config.set("simulation_timestep", 0.001, pynlm.ConfigSource.File)  // Small timestep for stability
config.set("plasticity.stdp.learning_rate", 0.001, pynlm.ConfigSource.File)  // Small learning rate
config.set("plasticity.hebbian.learning_rate", 0.01, pynlm.ConfigSource.File)  // Moderate learning rate
```

#### Solution 3: Add Debug Output
```python
// Add debug output to detect numerical issues
std::cout << "Configuration check:" << std::endl;
std::cout << "  neuron_count: " << config.getOr<int>("brain.neuron_count", 0) << std::endl;
std::cout << "  timestep: " << config.getOr<double>("simulation_timestep", 0.0) << std::endl;
std::cout << "  time_constant: " << config.getOr<float>("brain.time_constant", 0.0f) << std::endl;
```

## Performance Problems

### Issue 1: "Simulation too slow for large networks"

**Problem**: Large network simulations are too slow.

**Common Causes**:
1. Memory bandwidth limitations
2. Cache inefficiencies
3. Algorithm complexity
4. I/O bottlenecks

**Solutions**:

#### Solution 1: Optimize Configuration
```python
// Optimize for performance
config.set("brain.neuron_count", 5000, pynlm.ConfigSource.File)  // Medium size
config.set("region_count", 2, pynlm.ConfigSource.File)  // Multiple regions
config.set("simulation_timestep", 0.001, pynlm.ConfigSource.File)  // Reasonable timestep
config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)  // STDP is faster than Hebbian
config.set("plasticity.hebbian.enable", False, pynlm.ConfigSource.File)  // Disable expensive Hebbian
config.set("memory.working.enabled", False, pynlm.ConfigSource.File)  // Disable working memory
```

#### Solution 2: Use Parallel Processing
```python
// Enable parallel processing if available
config.set("brain.parallel_processing", True, pynlm.ConfigSource.File)
```

#### Solution 3: Optimize Hardware Usage
```bash
// Use multiple CPU cores
export OMP_NUM_THREADS=$(nproc)
make -j$(nproc)

// For GPU acceleration (if available)
export NLM_GPU_ACCELERATION=1
cmake .. -DNLM_USE_CUDA=ON
```

### Issue 2: "Memory usage too high"

**Problem**: Simulation uses too much memory.

**Common Causes**:
1. Too many neurons
2. Memory systems enabled
3. Inefficient data structures
4. Memory leaks

**Solutions**:

#### Solution 1: Reduce Memory Footprint
```python
// Reduce memory usage
config.set("brain.neuron_count", 1000, pynlm.ConfigSource.File)  // Fewer neurons
config.set("memory.working.enabled", False, pynlm.ConfigSource.File)  // Disable working memory
config.set("memory.episodic.enabled", False, pynlm.ConfigSource.File)  // Disable episodic memory
config.set("memory.associative.enabled", False, pynlm.ConfigSource.File)  // Disable associative memory
```

#### Solution 2: Use Memory-Efficient Configuration
```python
// Memory-efficient configuration
auto mem_config = pynlm.createDefaultConfig();
mem_config.set("brain.neuron_count", 500, pynlm.ConfigSource.File);
mem_config.set("simulation_timestep", 0.01, pynlm.ConfigSource.File);  // Larger timestep
mem_config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File);
mem_config.set("plasticity.hebbian.enable", False, pynlm.ConfigSource.File);
```

#### Solution 3: Monitor Memory Usage
```python
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()

std::cout << "Memory usage:" << std::endl;
std::cout << "  RSS: " << memory_info.rss / 1024 / 1024 << " MB" << std::endl;
std::cout << "  VMS: " << memory_info.vms / 1024 / 1024 << " MB" << std::endl;
```

## Memory and Resource Issues

### Issue 1: "Out of memory"

**Problem**: Simulation fails due to insufficient memory.

**Common Causes**:
1. System memory full
2. Memory leak in code
3. Configuration uses too much memory
4. Memory fragmentation

**Solutions**:

#### Solution 1: Increase System Memory
```bash
// Check available memory
free -h

// If memory is low:
sudo systemctl stop some-service  // Stop unnecessary services
sudo swapoff -a  // Disable swap temporarily
```

#### Solution 2: Reduce Memory Requirements
```python
// Reduce configuration memory usage
config.set("brain.neuron_count", 500, pynlm.ConfigSource.File)  // Fewer neurons
config.set("memory.working.enabled", False, pynlm.ConfigSource.File)  // Disable memory systems
config.set("memory.episodic.enabled", False, pynlm.ConfigSource.File)  // Disable episodic memory
```

#### Solution 3: Monitor Memory Usage
```python
#include <psutil>

void monitor_memory_usage() {
    auto process = psutil.Process(getpid());
    auto memory_info = process.memory_info();
    
    std::cout << "Memory Usage:" << std::endl;
    std::cout << "  RSS: " << memory_info.rss / 1024 / 1024 << " MB" << std::endl;
    std::cout << "  VMS: " << memory_info.vms / 1024 / 1024 << " MB" << std::endl;
    
    // Check if memory usage is too high
    if (memory_info.rss > 2 * 1024 * 1024 * 1024) {  // More than 2GB
        std::cout << "WARNING: High memory usage!" << std::endl;
    }
}
```

### Issue 2: "Memory leak"

**Problem**: Memory usage increases over time without bound.

**Common Causes**:
1. Dynamic memory allocation not freed
2. Circular references
3. Global variables not released
4. Resource cleanup not called

**Solutions**:

#### Solution 1: Add Memory Management
```cpp
// Add memory management to clean up resources
class MemoryManager {
public:
    static void cleanup() {
        // Free dynamic memory
        // Clear global variables
        // Close file handles
        // Release resources
    }
};

// Use memory manager
void cleanup_simulation() {
    MemoryManager::cleanup();
    // Also clean up brain and world objects
    if (brain) brain->reset();
    if (world) world->reset();
}
```

#### Solution 2: Use Smart Pointers
```cpp
// Use smart pointers to manage memory automatically
auto brain = std::make_shared<Brain>(config);
auto world = std::make_shared<SimpleWorld>();

// Smart pointers will automatically free memory
// No need for manual delete operations
```

#### Solution 3: Monitor Memory Over Time
```cpp
void monitor_memory_over_time() {
    auto process = psutil.Process(getpid());
    
    for (int i = 0; i < 100; ++i) {
        auto memory_info = process.memory_info();
        std::cout << "Step " << i << ": " 
                  << memory_info.rss / 1024 / 1024 << " MB" << std::endl;
        
        // Check for memory leak
        if (i > 10 && memory_info.rss > previous_memory * 1.5) {
            std::cout << "WARNING: Possible memory leak detected!" << std::endl;
            break;
        }
        
        previous_memory = memory_info.rss;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
```

## Learning and Plasticity Issues

### Issue 1: "STDP not working"

**Problem**: STDP learning rule is not producing weight changes.

**Common Causes**:
1. STDP disabled
2. No spike timing correlations
3. STDP parameters too small
4. Spike recording not working

**Solutions**:

#### Solution 1: Check STDP Configuration
```python
// Enable STDP
config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)

// Check STDP parameters
if config.getOr[bool]("plasticity.stdp.enable", False) {
    learning_rate = config.getOr[float]("plasticity.stdp.learning_rate", 0.001);
    print(f"STDP learning rate: {learning_rate}");
    
    if learning_rate < 0.001 {
        print("WARNING: Learning rate too small - try increasing");
        config.set("plasticity.stdp.learning_rate", 0.01, pynlm.ConfigSource.File);
    }
}
```

#### Solution 2: Create Spike Timing Correlations
```cpp
// Create spike timing correlations to trigger STDP
// Pre before post -> LTP (potentiation)
// Post before pre -> LTD (depression)

void create_stdp_pattern(Brain* brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 2) return;
    
    Neuron* pre_neuron = neurons[0];
    Neuron* post_neuron = neurons[1];
    
    // Create pre-before-post pattern (LTP)
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        pre_neuron->injectCurrent(60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        
        // Fire post-synaptic neuron
        post_neuron->injectCurrent(60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
}
```

#### Solution 3: Debug Spike Recording
```cpp
// Check if spikes are being recorded
brain->recordSpike(Timestamp currentTime);
auto spike_history = neuron->getSpikeHistory();
std::cout << "Spike history size: " << spike_history.size() << std::endl;

// Check synapse spike recording
for (auto& syn : synapses) {
    auto pre_spikes = syn->getPreSpikeHistory();
    auto post_spikes = syn->getPostSpikeHistory();
    
    std::cout << "Synapse " << syn->getId() << ": " 
              << pre_spikes.size() << " pre spikes, " 
              << post_spikes.size() << " post spikes" << std::endl;
}
```

### Issue 2: "Hebbian learning not working"

**Problem**: Hebbian learning rule is not producing weight changes.

**Common Causes**:
1. Hebbian disabled
2. No correlated activity
3. Hebbian parameters too small
4. Activity statistics not computed

**Solutions**:

#### Solution 1: Check Hebbian Configuration
```python
// Enable Hebbian
config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.File)

// Check Hebbian parameters
if config.getOr[bool]("plasticity.hebbian.enable", False) {
    learning_rate = config.getOr[float]("plasticity.hebbian.learning_rate", 0.01);
    print(f"Hebbian learning rate: {learning_rate}");
}
```

#### Solution 2: Create Correlated Activity
```cpp
// Create correlated activity for Hebbian learning
// Neurons that fire together should increase their connection strength

void create_hebbian_pattern(Brain* brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 3) return;
    
    // Create correlated activity
    for (int trial = 0; trial < 100; ++trial) {
        // Fire multiple neurons simultaneously (correlation)
        for (size_t i = 0; i < 3 && i < neurons.size(); ++i) {
            neurons[i]->injectCurrent(30.0f);
        }
        
        brain->step(trial, trial * 0.001);
    }
}
```

#### Solution 3: Check Hebbian Statistics
```cpp
// Check if Hebbian statistics are being computed
syn->enablePlasticity(false, true, false);  // Enable only Hebbian

// Check activity statistics
auto pre_activity = calculate_activity(syn->getPreSpikeHistory());
auto post_activity = calculate_activity(syn->getPostSpikeHistory());

float correlation = pre_activity * post_activity;
std::cout << "Hebbian correlation: " << correlation << std::endl;
```

## Advanced Troubleshooting

### Issue 1: "Deterministic simulations not reproducible"

**Problem**: Simulations produce different results even with same parameters.

**Common Causes**:
1. Random seed not set
2. System time used as seed
3. Non-deterministic algorithms
4. External random sources

**Solutions**:

#### Solution 1: Set Random Seed
```python
// Always set random seed for reproducibility
config.set("random_seed", 42, pynlm.ConfigSource.File)

// Or use command line argument
./nlm --random_seed=12345
```

#### Solution 2: Use Deterministic Random
```cpp
// Ensure all random sources are deterministic
RandomGenerator rng;
rng.seed(12345);

// Use deterministic algorithms
std::mt19937 gen(12345);  // Deterministic Mersenne Twister
std::uniform_real_distribution<float> dist(0.0f, 1.0f);
```

#### Solution 3: Disable Non-Deterministic Features
```python
// Disable features that use system time
config.set("brain.use_system_time", false, pynml.ConfigSource.File)

// Use fixed seed for all random operations
config.set("random.seed", 12345, pynml.ConfigSource.File)
```

### Issue 2: "Configuration files not loading"

**Problem**: Configuration files fail to load.

**Common Causes**:
1. File path incorrect
2. File permissions
3. File format incorrect
4. File corrupted

**Solutions**:

#### Solution 1: Check File Path
```python
// Check if file exists
std::ifstream file("configs/my_config.cfg");
if (file.good()) {
    std::cout << "Configuration file exists" << std::endl;
} else {
    std::cout << "Configuration file not found" << std::endl;
    // Try alternative path
}
```

#### Solution 2: Check File Format
```bash
# Check file format
cat configs/my_config.cfg

# Should look like:
# brain.neuron_count = 1000
# simulation_timestep = 0.001
# plasticity.stdp.enable = true
```

#### Solution 3: Create Test Configuration
```python
// Create test configuration
config = pynlm.createDefaultConfig();
config.set("brain.neuron_count", 100, pynlm.ConfigSource.File);
config.saveToFile("test_config.cfg");

// Then load the test file
if config.loadFromFile("test_config.cfg") {
    std::cout << "Test configuration loaded successfully" << std::endl;
}
```

### Issue 3: "Performance varies between runs"

**Problem**: Simulation performance varies between runs.

**Common Causes**:
1. System load variations
2. Memory pressure
3. Disk I/O
4. Network activity

**Solutions**:

#### Solution 1: Run Multiple Tests
```bash
// Run simulation multiple times
for i in {1..5}; do
    echo "Run $i:"
    /path/to/nlm --neuron_count=1000 --timestep=0.001
    echo "---"
done
```

#### Solution 2: Average Results
```python
import statistics

def average_performance():
    times = []
    
    for i in range(5):
        # Run simulation
        start_time = time.time()
        // Run simulation
        end_time = time.time()
        
        times.append(end_time - start_time)
    
    avg_time = statistics.mean(times)
    std_dev = statistics.stdev(times)
    
    print(f"Average time: {avg_time:.3f} seconds")
    print(f"Standard deviation: {std_dev:.3f} seconds")
    
    return avg_time
```

#### Solution 3: Use Benchmark Mode
```bash
// Run in benchmark mode for consistent results
export NLM_BENCHMARK_MODE=1
/path/to/nlm --neuron_count=1000 --timestep=0.001
```

## Debugging Techniques

### Technique 1: Configuration Debugging

#### Debug Configuration Issues
```python
import pynlm

def debug_configuration(config_path) {
    config = pynlm.createDefaultConfig();
    
    // Try to load configuration
    if config.loadFromFile(config_path) {
        std::cout << "Configuration loaded successfully" << std::endl;
        
        // Check all parameters
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            std::cout << "  " << key << ": ";
            
            // Try to get parameter value
            try {
                if (key.find("int") != std::string::npos) {
                    int value = config.getOr<int>(key, 0);
                    std::cout << value << std::endl;
                } else if (key.find("float") != std::string::npos) {
                    float value = config.getOr<float>(key, 0.0f);
                    std::cout << value << std::endl;
                } else if (key.find("bool") != std::string::npos) {
                    bool value = config.getOr<bool>(key, false);
                    std::cout << (value ? "true" : "false") << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "ERROR: " << e.what() << std::endl;
            }
        }
    } else {
        std::cout << "Failed to load configuration" << std::endl;
    }
}
```

### Technique 2: Memory Debugging

#### Monitor Memory Usage
```python
import psutil
import os

process = psutil.Process(os.getpid())

void monitor_memory_usage() {
    auto memory_info = process.memory_info();
    
    std::cout << "Memory Usage:" << std::endl;
    std::cout << "  RSS: " << memory_info.rss / 1024 / 1024 << " MB" << std::endl;
    std::cout << "  VMS: " << memory_info.vms / 1024 / 1024 << " MB" << std::endl;
    
    // Check for memory issues
    if (memory_info.rss > 2 * 1024 * 1024 * 1024) {  // More than 2GB
        std::cout << "WARNING: High memory usage detected!" << std::endl;
    }
}
```

### Technique 3: Performance Debugging

#### Measure Simulation Performance
```python
import time

void measure_performance(const std::string& description, std::function<void()> simulation) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    simulation();  // Run simulation
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << description << ": " << duration.count() << " ms" << std::endl;
}
```

### Technique 4: Spike Debugging

#### Debug Spike Recording and Processing
```cpp
void debug_spike_processing(Brain* brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) return;
    
    std::cout << "Spike Processing Debug:" << std::endl;
    for (size_t i = 0; i < neurons.size() && i < 5; ++i) {
        auto neuron = neurons[i];
        auto spike_history = neuron->getSpikeHistory();
        
        std::cout << "  Neuron " << i << ": " << spike_history.size() << " spikes" << std::endl;
        
        if (!spike_history.empty()) {
            std::cout << "    Last spike: " << spike_history.back() << std::endl;
        }
    }
}
```

### Technique 5: Plasticity Debugging

#### Debug Plasticity Updates
```cpp
void debug_plasticity_updates(Brain* brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto synapses = region->getSynapses();
    if (synapses.empty()) return;
    
    std::cout << "Plasticity Debug:" << std::endl;
    int plasticity_count = 0;
    
    for (auto& syn : synapses) {
        if (syn->getPlasticityFlags().hebbian || 
            syn->getPlasticityFlags().stdp || 
            syn->getPlasticityFlags().reward_modulated) {
            
            plasticity_count++;
            float current_weight = syn->getWeight();
            std::cout << "  Synapse " << syn->getId() << ": weight=" << current_weight"";
            
            if (syn->getPlasticityFlags().hebbian) std::cout << " Hebbian";
            if (syn->getPlasticityFlags().stdp) std::cout << " STDP";
            if (syn->getPlasticityFlags().reward_modulated) std::cout << " Reward";
            std::cout << std::endl;
        }
    }
    
    std::cout << "Total synapses with plasticity: " << plasticity_count << std::endl;
}
```

## Community Support

### Getting Help

#### Online Resources
- **GitHub Issues**: Report bugs and request features
- **Documentation**: Read comprehensive documentation
- **Forum**: Ask questions and share experiences
- **Discord**: Join community discussions

#### How to Report Issues
```bash
# Create a detailed bug report
# Include:
1. Reproduction steps
2. Expected vs actual behavior
3. Error messages
4. Configuration used
5. System information
6. Logs if available
```

#### How to Get Help
```python
# Create minimal reproduction case
import pynlm

// Create minimal configuration
config = pynlm.createDefaultConfig();
config.set("brain.neuron_count", 100, pynlm.ConfigSource.File);

// Create and initialize brain
brain = pynlm.createBrain(config);
brain.initialize();

// Try to reproduce the issue
// ... your reproduction code here ...

// If you encounter issues, save:
// 1. Configuration used
// 2. Error messages
// 3. System information
// 4. Reproduction steps
```

### Contributing to NLM

#### Report Issues
- Use GitHub Issues for bug reports
- Include reproduction steps
- Provide system information
- Share configuration files

#### Request Features
- Use GitHub Issues for feature requests
- Describe use case
- Provide example usage
- Suggest implementation approach

#### Contribute Code
- Fork the repository
- Create a feature branch
- Implement changes
- Write tests
- Create documentation
- Submit pull request

#### Improve Documentation
- Fix typos and errors
- Add examples
- Clarify confusing sections
- Add diagrams and illustrations

## Further Reading

- [API_REFERENCE.md](docs/API_REFERENCE.md) - Complete API documentation
- [TUTORIAL.md](docs/TUTORIAL.md) - Step-by-step tutorial
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization techniques
- [CONFIG_GUIDE.md](docs/CONFIG_GUIDE.md) - Configuration system guide
- [HOW_TO_USE.md](HOW_TO_USE.md) - Python API usage examples

---

*Document Version: 1.0*
*Last Updated: October 16, 2026*
*For issues or feedback: Please report at https://github.com/your-org/nlm/issues
