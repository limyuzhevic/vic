# NLM - Troubleshooting Guide

## Overview

This guide helps you diagnose and resolve common issues when working with NLM (Neural Learning Machine). It covers problems ranging from setup issues to runtime errors, with step-by-step solutions and best practices.

## Table of Contents

1. [Installation and Setup Issues](#installation-and-setup-issues)
2. [Build and Compilation Problems](#build-and-compilation-problems)
3. [Runtime Errors](#runtime-errors)
4. [Simulation Performance Issues](#simulation-performance-issues)
5. [Agent Behavior Problems](#agent-behavior-problems)
6. [Memory and Resource Issues](#memory-and-resource-issues)
7. [Configuration and State Management](#configuration-and-state-management)
8. [Advanced Debugging Techniques](#advanced-debugging-techniques)

---

## Installation and Setup Issues

### "Cannot import pynlm: No module named 'pynlm'"

**Problem**: Python module is not installed or not in the Python path.

**Solution**:

```bash
# Method 1: Install from source (recommended)
pip install .

# Method 2: Install in development mode
pip install -e .

# Method 3: Install specific version
pip install pynlm==0.1.0

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

**Why this happens**:
- Python package not installed
- Virtual environment issues
- PYTHONPATH configuration problems
- Multiple Python versions on system

**Additional tips**:
- Check `pip list | grep pynlm` to verify installation
- Try `python -m pip install --user .` for user-level installation
- Ensure you're in the correct directory with `pyproject.toml`

### "ImportError: libpynlm.so: cannot open shared object file"

**Problem**: Dynamic library dependencies missing.

**Solution**:

```bash
# On Linux/Unix systems
dldd $(python -c "import pynlm; print(pynlm.__file__.replace('__init__.py', 'pynlm.so'))")

# Install missing dependencies
 sudo apt-get install libpython3.8-dev

# Reinstall with debug symbols
pip install --force-reinstall --no-binary :all: .

# Or rebuild with appropriate flags
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-Wall -O2"
```

**Check dependencies**:
```bash
# Check for required system libraries
ldd $(find . -name "*.so" 2>/dev/null) | grep -E "not found|missing"

# Install common dependencies
 sudo apt-get install libstdc++6 libpython3.8 python3-dev
```

### "Permission denied when installing NLM"

**Problem**: Insufficient permissions for installation.

**Solution**:

```bash
# Install with user permissions
pip install --user .

# Or use virtual environment
python -m venv nlm_env
source nlm_env/bin/activate
pip install .

# Or use sudo (not recommended for development)
 sudo pip install .
```

---

## Build and Compilation Problems

### "CMake Error: Could not find Python libraries"

**Problem**: CMake cannot find Python development libraries.

**Solution**:

```bash
# Install Python development packages
apt-get update
apt-get install python3-dev python3-venv

# Set CMake Python variables
cmake .. -DPython_EXECUTABLE=$(which python3) \
         -DPYTHON_LIBRARY=$(python3-config --libdir)/libpython3.8.so \
         -DPYTHON_INCLUDE_DIR=$(python3-config --includes)

# Or let CMake find Python automatically
cmake .. -DPython_EXECUTABLE=$(which python3)
```

**Verification**:
```bash
# Check if Python development files exist
ls /usr/include/python3.8/
ls /usr/lib/x86_64-linux-gnu/libpython3.8.so*
```

### "CMakeError: Unknown compiler specified"

**Problem**: No C++ compiler found or configured.

**Solution**:

```bash
# Install C++ compiler
apt-get install g++ gcc

# Or use clang
apt-get install clang

# Verify compiler availability
which g++
which gcc

# Configure with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Or let CMake find the best compiler
cmake ..
```

### "Build fails with 'undefined reference to'` errors

**Problem**: Incomplete build or linking issues.

**Solution**:

```bash
# Clean build and rebuild from scratch
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Check for missing symbols
nm -u src/*.o | sort > missing_symbols.txt

# Or try incremental build
make -j$(nproc)

# If still failing, try clean rebuild
make clean
cmake . --build . --clean-first
```

**Debug with symbols**:
```bash
# Build with debug information
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Check specific compilation errors
gmake brain.o
gmake agent.o
```

---

## Runtime Errors

### "Segmentation fault (core dumped)"

**Problem**: Program crashes accessing invalid memory.

**Solution**:

#### Step-by-Step Diagnosis

```python
# 1. Ensure proper initialization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # CRITICAL - must call this first!

# 2. Step through simulation
for step in range(100):
    brain.step(step)  # Safe to step after initialization

# 3. Check for null pointers
try:
    world.update(0.1)
except Exception as e:
    print(f"Error: {e}")
```

**Common causes**:
- Calling `brain.step()` before `brain.initialize()`
- Using uninitialized variables
- Memory corruption from buffer overflows
- Infinite loops causing stack overflow

**Debugging techniques**:
```bash
# Generate core dump for analysis
gcore --output=core_file $(pidof nlm_phase6_demo)

# Run with valgrind for memory errors
valgrind --tool=memcheck ./nlm_phase6_demo

# Run with address sanitizer
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
make
./nlm_phase6_demo
```

### "RuntimeError: Brain not initialized"

**Problem**: Brain object used without calling initialize().

**Solution**:

```python
import pynlm

# Correct usage pattern
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # Always required!

# Then safe to use
brain.step(0)

# Or use context manager
try:
    brain.initialize()
    brain.step(0)
except Exception as e:
    print(f"Error: {e}")
finally:
    # Cleanup if needed
    pass
```

### "Configuration parameter not found"

**Problem**: Trying to access configuration parameter that doesn't exist.

**Solution**:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Check if parameter exists before using
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")
else:
    print("Parameter not found, using default")

# Get all available parameters
keys = config.getKeys()
print("Available parameters:", keys)

# Use correct parameter names
config.set("brain.neuron_count", 1000)  # Correct parameter name
```

**Common parameter names**:
- Brain parameters: `brain.neuron_count`, `brain.synapse_density`
- Plasticity: `plasticity.stdp.enable`, `plasticity.hebbian.threshold`
- Neuromodulation: `neuromod.dopamine.scale`, `neuromod.curiosity.enable`

### "ModuleNotFoundError: No module named 'numpy'"

**Problem**: Missing Python dependency.

**Solution**:

```bash
# Install required dependencies
pip install numpy scipy matplotlib pandas

# Or install from requirements file
pip install -r requirements.txt

# Install with scikit-build-core (for bindings)
pip install scikit-build-core pybind11
```

---

## Simulation Performance Issues

### "Simulation too slow"

**Problem**: Simulation execution time exceeds expectations.

**Solution**:

#### Performance Analysis

```python
import time
import pynlm

# Create performance monitor
class PerformanceMonitor:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.timings = []
        self.step_count = 0
    
    def stepWithMonitoring(self, step):
        start_time = time.time()
        
        # Execute simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        self.timings.append(end_time - start_time)
        self.step_count += 1
        
        return end_time - start_time
    
    def getStats(self):
        if not self.timings:
            return None
        
        return {
            'avg_step_time': sum(self.timings) / len(self.timings),
            'min_step_time': min(self.timings),
            'max_step_time': max(self.timings),
            'steps_per_second': self.step_count / sum(self.timings),
            'total_time': sum(self.timings)
        }

# Use the monitor
monitor = PerformanceMonitor(brain, agent, world)
for step in range(100):
    monitor.stepWithMonitoring(step)

print("Performance stats:", monitor.getStats())
```

#### Optimization Techniques

```python
# 1. Reduce brain size
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller brain = faster

# 2. Enable performance optimizations
config.set("performance.optimization.enable", True)
config.set("performance.optimization.vectorization", True)

# 3. Adjust simulation timestep
config.set("world.timestep", 0.05)  # Larger timestep = faster

# 4. Reduce plasticity computations
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)  # Lower rate

# 5. Enable parallel processing (if available)
config.set("performance.optimization.parallel", True)
```

### "Memory usage too high"

**Problem**: Simulation consumes excessive memory.

**Solution**:

```python
import pynlm

# Configure memory-efficient settings
config = pynlm.createDefaultConfig()

# Enable memory pool optimization
config.set("performance.memory_pool.enable", True)

# Use sparse connectivity
config.set("brain.synapse_density", 0.01)  # Sparse connections

# Limit neural populations
config.set("brain.neuron_count", 1000)  # Reasonable size

# Monitor memory usage
import psutil
process = psutil.Process()
memory_mb = process.memory_info().rss / 1024 / 1024
print(f"Memory usage: {memory_mb:.2f} MB")
```

**Memory monitoring script**:
```python
#!/usr/bin/env python3
import pynlm
import psutil
import time

def runMemoryMonitor(steps=100):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    process = psutil.Process()
    memory_samples = []
    
    for step in range(steps):
        brain.step(step)
        
        # Record memory usage
        memory_mb = process.memory_info().rss / 1024 / 1024
        memory_samples.append(memory_mb)
        
        if step % 10 == 0:
            print(f"Step {step}: {memory_mb:.2f} MB")
    
    # Analyze results
    avg_memory = sum(memory_samples) / len(memory_samples)
    max_memory = max(memory_samples)
    
    print(f"Average memory: {avg_memory:.2f} MB")
    print(f"Maximum memory: {max_memory:.2f} MB")
    print(f"Memory growth: {max_memory - memory_samples[0]:.2f} MB")

runMemoryMonitor()
```

---

## Agent Behavior Problems

### "Agent doesn't learn or explore"

**Problem**: Agent shows no learning behavior or exploration.

**Solution**:

```python
import pynlm

# Check if learning subsystems are enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)     # Learn from rewards
agent.enableStructuralPlasticity(True) # Grow new connections  
agent.enableDevelopment(True)          # Brain matures over time
agent.enableCuriosity(True)            # Explore new things
agent.enableNoveltyDetection(True)     # Detect novel stimuli

# Verify subsystems are working
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
```

#### Check for learning stagnation

```python
# Monitor learning progress over time
curiosity_history = []
novelty_history = []
reward_history = []

for step in range(1000):
    # Run simulation
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record learning metrics
    curiosity_history.append(agent.getCuriosityLevel())
    novelty_history.append(agent.getNoveltyLevel())
    
    if step % 100 == 0:
        print(f"Step {step}: Curiosity={curiosity_history[-1]:.3f}, Novelty={novelty_history[-1]:.3f}")

# Analyze learning curves
import matplotlib.pyplot as plt

plt.figure(figsize=(12, 4))

plt.subplot(1, 3, 1)
plt.plot(curiosity_history)
plt.title('Curiosity Over Time')
plt.ylabel('Level')

plt.subplot(1, 3, 2)
plt.plot(novelty_history)
plt.title('Novelty Over Time')
plt.ylabel('Level')

plt.subplot(1, 3, 3)
plt.hist(reward_history, bins=20)
plt.title('Reward Distribution')
plt.ylabel('Frequency')

plt.tight_layout()
plt.show()
```

### "Agent gets stuck in local optima"

**Problem**: Agent stops exploring and doesn't try new behaviors.

**Solution**:

```python
# 1. Increase exploration rates
agent.setExplorationRate(0.3)  # Higher exploration
agent.setCuriosityThreshold(0.1)  # Lower novelty threshold

# 2. Add intrinsic motivation
agent.enableIntrinsicMotivation(True)

# 3. Implement random exploration occasionally
def exploreRandomly(agent, world):
    if random.random() < 0.1:  # 10% chance of random exploration
        action = pynlm.createAction()
        action.setType(pynlm.ActionType.Explore)
        return action
    return agent.decodeMotorCommand()

# 4. Add memory-based exploration
agent.enableWorkingMemory(True)  # Remember past experiences
agent.enablePrediction(True)     # Predict future outcomes
```

**Exploration strategies**:
```python
# Strategy 1: Epsilon-greedy exploration
epsilon = 0.1  # 10% random actions
if random.random() < epsilon:
    action = getRandomAction()
else:
    action = agent.decodeMotorCommand()

# Strategy 2: Thompson sampling
# Sample from action value distributions
for action in possible_actions:
    samples[action] = sample_action_value(action)
best_action = max(samples, key=samples.get)

# Strategy 3: Upper Confidence Bound (UCB)
# Balance exploration vs exploitation
ucb_value = estimate_value(action) + c * math.sqrt(log(total_steps) / visits[action])
```

---

## Memory and Resource Issues

### "Out of memory (OOM)"

**Problem**: Simulation runs out of available memory.

**Solution**:

```python
import pynlm
import os

# Configure memory-efficient settings
config = pynlm.createDefaultConfig()

# Limit simulation size
config.set("brain.neuron_count", 500)    # Smaller brain
config.set("brain.synapse_density", 0.01) # Sparse connections

# Enable memory pool optimization
config.set("performance.memory_pool.enable", True)
config.set("performance.event_queue.size", 5000)  # Smaller queue

# Force garbage collection
import gc
gc.collect()

# Monitor memory usage
import psutil
process = psutil.Process()
memory_before = process.memory_info().rss / 1024 / 1024

brain = pynlm.createBrain(config)
brain.initialize()

memory_after = process.memory_info().rss / 1024 / 1024
print(f"Memory before: {memory_before:.2f} MB")
print(f"Memory after: {memory_after:.2f} MB")
print(f"Memory increase: {memory_after - memory_before:.2f} MB")
```

**System-level solutions**:
```bash
# Increase swap space
 sudo swapctl -s
 sudo swapon -s

# Monitor system memory
free -h
 cat /proc/meminfo | grep -E "MemTotal|MemFree|SwapTotal|SwapFree"

# Kill memory-hungry processes
 ps aux --sort=-%mem | head -10

# Use memory-efficient alternatives
# Instead of:
# large_list = list(range(10000000))

# Use:
import array
# large_array = array.array('i', range(10000000))
```

### "File handle leak"

**Problem**: Too many open file handles.

**Solution**:

```python
import pynlm
import os
import gc

# Use context managers for file operations
class SafeFileHandler:
    def __init__(self, filename):
        self.filename = filename
        self.file = None
    
    def __enter__(self):
        self.file = open(self.filename, 'w')
        return self.file
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.file:
            self.file.close()
        gc.collect()

# Use context managers for checkpoints
checkpoint_file = "simulation_checkpoint.bin"
try:
    with SafeFileHandler(checkpoint_file) as f:
        brain.save(checkpoint_file)
finally:
    # Ensure file is closed
    if os.path.exists(checkpoint_file):
        print(f"Checkpoint saved: {checkpoint_file}")

# Monitor open file handles
import psutil
process = psutil.Process()
open_files = process.open_files()
print(f"Open files: {len(open_files)}")

# Close unnecessary files
for f in open_files:
    if 'temp' in f.path.lower():
        print(f"Closing temporary file: {f.path}")
```

---

## Configuration and State Management

### "Configuration not saved or loaded properly"

**Problem**: Configuration changes not persisted.

**Solution**:

```python
import pynlm
import json
import os

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Save to file with error handling
try:
    config.saveToFile("my_config.json")
    print("Configuration saved successfully")
except Exception as e:
    print(f"Error saving config: {e}")
    # Try with absolute path
    config.saveToFile(os.path.abspath("my_config.json"))

# Load configuration with verification
try:
    config.loadFromFile("my_config.json")
    print("Configuration loaded successfully")
    
    # Verify loaded values
    if config.has("brain.neuron_count"):
        print(f"Loaded neuron count: {config.get('brain.neuron_count')}")
    if config.has("plasticity.stdp.enable"):
        print(f"Loaded STDP setting: {config.get('plasticity.stdp.enable')}")
        
except FileNotFoundError:
    print("Configuration file not found")
    # Create default config
    config = pynlm.createDefaultConfig()
except Exception as e:
    print(f"Error loading config: {e}")
```

### "Checkpoint file corrupted or unreadable"

**Problem**: Checkpoint files are damaged or unreadable.

**Solution**:

```python
import pynlm
import os

# Create backup before saving
checkpoint_file = "simulation_001.bin"
backup_file = "simulation_001_backup.bin"

# Check if checkpoint exists and is valid
if os.path.exists(checkpoint_file):
    # Test file accessibility
    test_size = os.path.getsize(checkpoint_file)
    print(f"Checkpoint file size: {test_size} bytes")
    
    # Try to load checkpoint
    try:
        test_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        test_brain.load(checkpoint_file)
        print("Checkpoint file is valid")
    except Exception as e:
        print(f"Invalid checkpoint: {e}")
        # Create backup
        if os.path.exists(checkpoint_file):
            import shutil
            shutil.copy2(checkpoint_file, backup_file)
            print(f"Backup created: {backup_file}")
else:
    print("Checkpoint file does not exist")
```

### "Parameter validation errors"

**Problem**: Invalid configuration parameters.

**Solution**:

```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()

# Set parameters with type checking
try:
    config.setInt("brain.neuron_count", 1000)  # Integer parameter
    print("Set neuron count successfully")
except ValueError as e:
    print(f"Invalid neuron count: {e}")

try:
    config.setFloat("brain.synapse_density", 0.05)  # Float parameter
    print("Set synapse density successfully")
except ValueError as e:
    print(f"Invalid synapse density: {e}")

try:
    config.setBool("plasticity.stdp.enable", True)  # Boolean parameter
    print("Set STDP setting successfully")
except ValueError as e:
    print(f"Invalid STDP setting: {e}")

# Use correct parameter ranges
config.set("brain.neuron_count", 100)  # Reasonable range: 10-10000
config.set("brain.synapse_density", 0.001)  # Reasonable range: 0.001-0.5
```

---

## Advanced Debugging Techniques

### Using Python's Debugging Tools

```python
import pynlm
import pdb
import traceback

# Enable debug mode
config = pynlm.createDefaultConfig()
config.set("debug.mode", True)

# Create brain with error handling
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Set breakpoint for debugging
    pdb.set_trace()  # Pause execution for interactive debugging
    
    brain.step(0)
    
except Exception as e:
    print(f"Error occurred: {e}")
    print("Full traceback:")
    traceback.print_exc()
    
    # Get detailed error information
    print(f"Error type: {type(e).__name__}")
    print(f"Error module: {e.__module__}")
```

### Debug Output and Logging

```python
import pynlm
import logging

# Setup logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_debug.log'),
        logging.StreamHandler()
    ]
)

logger = logging.getLogger('nlm')

# Run simulation with logging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(10):
    logger.debug(f"Step {step}: Starting")
    
    world.update(0.1)
    logger.debug(f"World state updated")
    
    percept = world.getSensoryPercept()
    logger.debug(f"Sensory input: {percept}")
    
    agent.processSensoryInput(percept)
    logger.debug(f"Agent processed input")
    
    brain.step(step)
    logger.debug(f"Brain step completed")
    
    action = agent.decodeMotorCommand()
    logger.debug(f"Action decoded: {action.getType()}")
    
    world.applyMotorCommand(action, world.getSimulationTime())
    logger.debug(f"Action applied")
    
    logger.info(f"Step {step} completed")
```

### Memory Leak Detection

```python
import pynlm
import tracemalloc
import gc

# Start memory tracing
tracemalloc.start()

# Run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Take initial snapshot
snapshot1 = tracemalloc.take_snapshot()

# Run some steps
for step in range(100):
    brain.step(step)

# Force garbage collection
gc.collect()

# Take final snapshot
snapshot2 = tracemalloc.take_snapshot()

# Compare snapshots
top_stats = snapshot2.compare_to(snapshot1, 'lineno')

print("Top memory allocations:")
for stat in top_stats[:10]:
    print(stat)

# Stop tracing
tracemalloc.stop()
```

### Performance Profiling

```python
import pynlm
import cProfile
import pstats
import io

# Profile simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

def runSimulation(steps=100):
    for step in range(steps):
        brain.step(step)

# Profile with cProfile
pr = cProfile.Profile()
pr.enable()
runSimulation(100)
pr.disable()

# Print profiling results
s = io.StringIO()
ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
ps.print_stats()

print("Profiling results:")
print(s.getvalue())
```

---

## Quick Reference: Common Error Messages and Solutions

| Error Message | Solution |
|---------------|----------|
| `ImportError: No module named 'pynlm'` | Run `pip install .` or `pip install -e .` |
| `RuntimeError: Brain not initialized` | Call `brain.initialize()` before any steps |
| `Segmentation fault` | Check for null pointers, use valgrind |
| `Configuration parameter not found` | Verify parameter names, use `config.has()` |
| `Out of memory` | Reduce brain size, enable memory pools |
| `Agent not learning` | Enable learning subsystems, check curiosity |
| `Checkpoint file error` | Use absolute paths, verify file permissions |
| `Performance too slow` | Enable optimizations, adjust timestep |

---

## Getting Help

### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issues and Bug Reports**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

### Community Support

- **Slack/Discord**: Join the NLM community server
- **Email**: support@nlm-project.org
- **Stack Overflow**: Tag with `nlm` or `pynlm`

### Local Help

```bash
# Get version and system information
python -c "import pynlm; print('NLM Version:', pynlm.__version__)"
python -c "import sys; print('Python:', sys.version)"

# Check system resources
free -h
nvidia-smi  # if using GPU

# Get help with documentation
python -m pynlm --help  # if help module exists
```

---

## Conclusion

Effective troubleshooting involves:

1. **Prevention is better than cure**: Always initialize components before use
2. **Monitor regularly**: Use logging and performance monitoring
3. **Use the right tools**: valgrind for memory issues, profilers for performance
4. **Document issues**: Keep a log of problems and solutions
5. **Know when to ask for help**: Use online resources and community support

Remember that complex simulations can have obscure issues. Don't hesitate to seek help when you encounter problems you cannot solve. The NLM community is here to help you succeed!
