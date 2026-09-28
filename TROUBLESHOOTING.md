# NLM Troubleshooting Guide - Common Problems and Solutions

## Overview

This guide provides solutions to common problems encountered when using NLM (Neural Learning Machine). It covers issues from basic setup problems to advanced integration challenges.

## Section 1: Setup and Installation Issues

### Problem 1: "ImportError: No module named 'pynlm'"

**Symptoms:**
- Python cannot import pynlm module
- Module not found in Python path

**Solutions:**

**1. Install the module properly:**
```bash
# Install from source (recommended)
pip install -e .

# Or install from wheel
pip install dist/pynlm-*.whl

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

**2. Check build dependencies:**
```bash
# Ensure required dependencies are installed
pip install pybind11 scikit-build-core numpy pytest

# Rebuild if dependencies changed
pip install -e . --force-reinstall
```

**3. Check CMake configuration:**
```bash
# Ensure CMake can find Python
cmake .. -DPython_EXECUTABLE=$(which python)

# Or specify Python path
cmake .. -DPython_EXECUTABLE=/usr/bin/python3
```

**4. Environment check:**
```python
import sys
import platform

print(f"Python version: {sys.version}")
print(f"Platform: {platform.system()}")
print(f"Architecture: {platform.machine()}")

# Check if pynlm can be imported
try:
    import pynlm
    print(f"pynlm imported successfully: {pynlm.__version__}")
except ImportError as e:
    print(f"Import failed: {e}")
    print("Please check installation and environment variables")
```

### Problem 2: "CMakeError: The following arguments were not found: Python3"

**Symptoms:**
- CMake configuration fails
- Cannot find Python development headers

**Solutions:**

**1. Install Python development packages:**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install python3-dev python3-pip python3-venv

# CentOS/RHEL
 sudo yum groupinstall "Development Tools"
 sudo yum install python3-devel python3-pip

# macOS (using Homebrew)
brew install python3
```

**2. Specify Python location:**
```bash
# Find Python executable
which python3
ls /usr/bin/python*

# Use specific version
cmake .. -DPython_EXECUTABLE=/usr/bin/python3
```

**3. Install pybind11 separately:**
```bash
pip install pybind11
```

**4. Manual build process:**
```bash
# Create build directory
mkdir build && cd build

# Configure with specific Python path
cmake -DPython3_ROOT_DIR=/usr/local/bin .. -DCMAKE_BUILD_TYPE=Debug

# Build step by step
make -j$(nproc) pynlm
```

### Problem 3: "Segmentation fault (core dumped)"

**Symptoms:**
- Program crashes immediately after launch
- Core file may be generated
- Often happens with C++ executables

**Solutions:**

**1. Check initialization order:**
```python
# Common mistake: Initialize brain after using it
# WRONG:
brain = pynlm.createBrain(config)
action = brain.produceAction()  # Crashes - not initialized
brain.initialize()

# RIGHT:
brain = pynlm.createBrain(config)
brain.initialize()
action = brain.produceAction()  # Works
```

**2. Check configuration validity:**
```python
# Create configuration properly
config = pynlm.createDefaultConfig()
# Set required parameters
config.set("brain.neuron_count", 100)

# Verify configuration
if not config.has("brain.neuron_count"):
    print("Error: Missing required parameter")
    sys.exit(1)
```

**3. Check memory allocation:**
```python
# For large networks, ensure enough memory
import os
import psutil

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")

# If memory is limited, reduce network size
config.set("brain.neuron_count", 500)  # Instead of 10000
```

**4. Debug with symbols:**
```bash
# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run with GDB
gdb ./nlm
(gdb) run
# Then use standard GDB commands
```

## Section 2: Runtime Issues

### Problem 4: "Brain is not initialized"

**Symptoms:**
- Methods like `step()`, `produceAction()` fail
- Crashes or unexpected behavior

**Solutions:**

**1. Always call initialize():**
```python
import pynlm

# Create and initialize properly
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # MUST call this!

# Use the brain
for i in range(100):
    brain.step(i)
```

**2. Reset between simulations:**
```python
# Reset before re-running simulation
brain.reset()
brain.initialize()  # Re-initialize after reset

for episode in range(10):
    brain.reset()
    brain.initialize()
    for step in range(100):
        brain.step(step)
```

**3. Check config before initialization:**
```python
# Validate configuration before use
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.05)

# Initialize
brain = pynlm.createBrain(config)
brain.initialize()
```

### Problem 5: "Zero activity - all neurons silent"

**Symptoms:**
- No neurons fire
- All activity is zero
- No learning or behavior

**Solutions:**

**1. Check initialization:**
```python
# Create brain and verify it has neurons
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Neurons created: {brain.getTotalNeuronCount()}")
print(f"Synapses created: {brain.getTotalSynapseCount()}")
```

**2. Ensure sensory input:**
```python
# Create world and agent for sensory input
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Provide sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

**3. Check neuron parameters:**
```python
# Adjust neuron properties for activity
config.set("brain.v_thresh", -55.0)  # Less negative = easier to fire
config.set("brain.v_rest", -65.0)   # Higher resting potential
config.set("brain.tau_mem", 10.0)   # Faster response
```

**4. Enable neuromodulation:**
```python
# Enable reward modulation for learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
```

**5. Add external input:**
```python
# Manually add input to neurons
# (This depends on the specific NLM API)
for i in range(10):
    brain.receiveSensoryInput(sensory_pattern)
```

### Problem 6: "Memory systems returning null pointers"

**Symptoms:**
- `brain.getWorkingMemory()` returns None
- `brain.getEpisodicMemory()` returns None
- Memory operations fail

**Solutions:**

**1. Check integration fixes:**
```cpp
// In Brain.cpp, ensure these functions are properly implemented
NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (!pImpl->workingMemory) {
        pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
        pImpl->workingMemory->initialize(this);
    }
    return pImpl->workingMemory.get();
}
```

**2. Python integration:**
```python
# Check if memory systems are available
brain = pynlm.createBrain(config)

# Try to access memory systems
try:
    working_memory = brain.getWorkingMemory()
    print(f"Working memory: {working_memory}")
except Exception as e:
    print(f"Working memory error: {e}")
```

**3. Integration test:**
```python
# Use the provided test script
test_integration_fix.py

# Or check BRAIN_INTEGRATION_FIXES_REPORT.md
# for integration status
```

**4. Manual memory system creation:**
```python
# If integration is missing, create memory systems manually
working_memory = pynlm.createWorkingMemory()
working_memory.initialize(brain)

# Then connect to brain
# This depends on the specific API
```

### Problem 7: "Performance issues - slow simulation"

**Symptoms:**
- Simulation takes too long
- Low frames per second
- Resource exhaustion

**Solutions:**

**1. Optimize configuration:**
```python
# Reduce network size for speed
config.set("brain.neuron_count", 500)  # Instead of 5000
config.set("brain.synapse_density", 0.03)  # Sparse connections

# Disable expensive features
config.set("plasticity.structural.enable", False)  # Disable structural plasticity
config.set("neuromod.curiosity.enable", False)     # Disable curiosity
```

**2. Enable SIMD optimizations:**
```bash
# Build with SIMD support
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)
```

**3. Use checkpoint system:**
```python
# For long simulations, save checkpoints
checkpoint_manager = pynlm.createCheckpointManager()
checkpoint_manager.setAutoSaveInterval(1000)

# Save progress
checkpoint_manager.saveCheckpoint("progress", brain)

# Resume later
checkpoint_manager.loadCheckpoint("progress", brain)
```

**4. Profile performance:**
```python
import time
start_time = time.time()

# Run simulation
for i in range(1000):
    brain.step(i)

end_time = time.time()
print(f"Simulation time: {end_time - start_time:.2f} seconds")
print(f"Steps per second: {1000 / (end_time - start_time):.1f}")
```

**5. Parallel processing:**
```cpp
// Enable multi-threading in C++ configuration
// In CMakeLists.txt or build configuration
add_definitions(-DNLM_PARALLEL -DNLM_NUM_THREADS=4)
```

### Problem 8: "Learning not happening"

**Symptoms:**
- No change in neuron weights
- No improvement in behavior
- Learning curves flat

**Solutions:**

**1. Check learning parameters:**
```python
# Adjust learning rate
config.set("plasticity.stdp.learning_rate", 0.001)  # Increase
config.set("plasticity.hebbian.enable", True)  # Enable Hebbian
```

**2. Ensure neuromodulation:**
```python
# Enable reward-based learning
agent.enableRewardModulation(True)

# Enable exploration
agent.enableCuriosity(True)
```

**3. Check reward signals:**
```python
# Provide meaningful rewards
reward = world.computeReward(agent)
agent.applyRewardModulation(reward, 0.0)
```

**4. Verify plasticity rules:**
```python
# Check that plasticity is active
brain.checkPlasticityActive()  # Example method
```

**5. Debug learning process:**
```python
# Log learning progress
for step in range(1000):
    before_weights = brain.getAllWeights()
    brain.step(step)
    after_weights = brain.getAllWeights()
    
    weight_change = calculate_weight_change(before_weights, after_weights)
    print(f"Step {step}: Weight change = {weight_change}")
```

## Section 3: Integration Issues

### Problem 9: "Phase 6 integration incomplete"

**Symptoms:**
- Some systems not connected
- Missing integration between components
- Incomplete brain loop

**Solutions:**

**1. Check INTEGATION.md:**
```markdown
# Review: INTEGATION.md
- Memory system integration status
- Prediction system connection
- Neuromodulation integration
- Development effects
```

**2. Use BRAIN_INTEGRATION_FIXES_REPORT.md:**
```markdown
# Review: BRAIN_INTEGRATION_FIXES_REPORT.md
- Fixed integrations
- Remaining issues
- Implementation checklist
```

**3. Manual integration:**
```cpp
// Example of connecting memory systems
void Brain::connectMemorySystems() {
    if (workingMemory && episodicMemory) {
        // Connect working memory to episodic memory
        episodicMemory->storeWorkingMemory(workingMemory->retrieve());
    }
}
```

**4. Integration test script:**
```python
#!/usr/bin/env python3
# test_integration_fix.py
# Use this to verify integration fixes

import subprocess
import sys

subprocess.run([sys.executable, "test_integration_fix.py"])
```

### Problem 10: "Checkpoint system not working"

**Symptoms:**
- save() returns false
- load() fails
- State not preserved

**Solutions:**

**1. Check CheckpointManager implementation:**
```cpp
// In Brain.cpp
bool Brain::save(const std::string& filepath) const {
    if (!pImpl->checkpointManager) {
        pImpl->checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    pImpl->checkpointManager->saveCheckpoint(filepath, this);
    return true;
}
```

**2. Ensure provider functions:**
```cpp
// Required provider functions for checkpointing
void Brain::saveNeuronState(NeuronCheckpointData& data) const {
    // Implement neuron state saving
}

void Brain::saveSynapseState(SynapseCheckpointData& data) const {
    // Implement synapse state saving
}
```

**3. Python checkpoint usage:**
```python
# Create checkpoint manager
checkpoint_manager = pynlm.createCheckpointManager()

# Save brain state
checkpoint_manager.saveCheckpoint("my_brain", brain)

# Load brain state
checkpoint_manager.loadCheckpoint("my_brain", brain)
```

## Section 4: Platform-Specific Issues

### Problem 11: "Python bindings not available on this platform"

**Symptoms:**
- pybind11 compilation fails
- Platform-specific compilation errors
- Missing platform libraries

**Solutions:**

**1. Check platform compatibility:**
```bash
# Check supported platforms
uname -a
# Should be Linux, macOS, or Windows
```

**2. Install platform-specific dependencies:**
```bash
# Linux
sudo apt-get install libpython3-dev build-essential

# macOS with Homebrew
brew install python3 cmake make

# Windows (using CMake GUI)
# Install Python from python.org
```

**3. Build platform-specific options:**
```bash
# Linux-specific
cmake .. -DCMAKE_CXX_COMPILER=g++

# macOS-specific
cmake .. -DCMAKE_CXX_COMPILER=clang++

# Windows-specific (using MSVC)
cmake .. -G "Visual Studio 16 2019"
```

### Problem 12: "Memory leaks or excessive memory usage"

**Symptoms:**
- Memory usage grows without bound
- Application slows down over time
- Out-of-memory errors

**Solutions:**

**1. Check for memory leaks:**
```cpp
// In Brain class, add memory tracking
void Brain::step(SimulationStep step, Timestamp time) {
    // Track memory before step
    size_t memoryBefore = getMemoryUsage();
    
    // ... step implementation ...
    
    // Track memory after step
    size_t memoryAfter = getMemoryUsage();
    if (memoryAfter > memoryBefore * 1.1) {
        std::cerr << "Warning: Memory usage increased by " 
                  << (memoryAfter - memoryBefore) << " bytes\n";
    }
}
```

**2. Clear memory when no longer needed:**
```python
# Clear references to large objects
brain = None
world = None
agent = None
import gc
gc.collect()
```

**3. Use smart pointers:**
```cpp
// Use std::unique_ptr or std::shared_ptr
// instead of raw pointers where appropriate
```

**4. Profile memory usage:**
```python
import tracemalloc

tracemalloc.start()

# Run simulation
for i in range(1000):
    brain.step(i)

current, peak = tracemalloc.get_traced_memory()
print(f"Peak memory usage: {peak / 1024 / 1024:.1f} MB")

tracemalloc.stop()
```

## Section 5: Advanced Issues

### Problem 13: "Configuration file errors"

**Symptoms:**
- JSON parsing errors
- Invalid configuration values
- File not found errors

**Solutions:**

**1. Check JSON syntax:**
```json
// Correct JSON format
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    }
}
```

**2. Use config.saveToFile() to create templates:**
```python
config = pynlm.createDefaultConfig()
config.saveToFile("template_config.json")
```

**3. Load configuration safely:**
```python
config = pynlm.createDefaultConfig()
try:
    config.loadFromFile("config.json")
    print("Configuration loaded successfully")
except Exception as e:
    print(f"Error loading configuration: {e}")
    # Use defaults instead
```

### Problem 14: "Debugging tips and techniques"

**Solutions:**

**1. Add print statements:**
```python
# Debug brain initialization
brain = pynlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Synapses: {brain.getTotalSynapseCount()}")

# Debug simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

**2. Use logging:**
```python
import logging

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger("nlm")

# Log important events
logger.debug("Brain initialized")
logger.debug(f"Neuron count: {brain.getTotalNeuronCount()}")
```

**3. Write test cases:**
```python
def test_basic_functionality():
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize
    brain.initialize()
    
    # Verify
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    # Verify results
    assert brain.getTotalSpikeCount() >= 0
    
    print("Basic functionality test passed")
```

### Problem 15: "Common coding mistakes"

**Mistakes to avoid:**

1. **Not initializing brain before use**
2. **Using uninitialized variables**
3. **Ignoring return values**
4. **Memory leaks**
5. **Configuration validation**
6. **Thread safety issues**
7. **Performance bottlenecks**
8. **Platform-specific bugs**

**Correct patterns:**
```python
# Always initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Always check return values
result = brain.someMethod()
if result.isSuccess():
    # Use result
else:
    # Handle error

# Always clean up
brain.reset()
brain = None
```

## Section 6: Getting Help

### Where to find additional help:

1. **Documentation files:**
   - `INTEGATION.md` - Integration guide
   - `PERFORMANCE.md` - Performance tuning
   - `PYTHON_ADVANCED.md` - Advanced Python usage
   - `docs/ARCHITECTURE.md` - System architecture
   - `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 audit report

2. **Source code:**
   - Read `bindings.cpp` for advanced usage
   - Check `src/agent/AgentBrain.cpp` for agent implementation
   - Review `src/brain/Brain.cpp` for brain integration

3. **Test files:**
   - `test_integration_fix.py` - Integration verification
   - `advanced_examples.py` - Advanced examples
   - `simple_python_example.py` - Basic examples

4. **Community resources:**
   - GitHub issues and pull requests
   - NLM documentation website
   - Research papers and publications

### Debug Checklist:

```bash
# Before reporting a bug:
# 1. Reproduce the issue
# 2. Check if it's a known issue
# 3. Simplify the test case
# 4. Add logging/prints
# 5. Check configuration
# 6. Verify environment
# 7. Document steps to reproduce

# When reporting a bug:
# - Include error messages
# - Include stack traces
# - Include configuration files
# - Include steps to reproduce
# - Include expected vs actual behavior
```

This troubleshooting guide covers the most common NLM issues and provides practical solutions. Use this as a starting point and always check the specific NLM documentation and source code for detailed information about particular issues.