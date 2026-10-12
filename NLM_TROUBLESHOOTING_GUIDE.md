# NLM Troubleshooting Guide and FAQ

**Comprehensive guide to solving common NLM problems and answering frequently asked questions**

## Overview

This document provides detailed troubleshooting guidance for NLM users, covering the most common issues and providing step-by-step solutions. It includes a comprehensive FAQ section addressing frequently asked questions about NLM's functionality, limitations, and best practices.

## Quick Reference

### Common Error Messages and Solutions

| Error Message | Likely Cause | Solution |
|---------------|-------------|----------|
| "Brain not initialized" | `brain.initialize()` not called | Always call `brain.initialize()` before `brain.step()` |
| "ImportError: No module named 'pynlm'" | Python bindings not installed | Install NLM Python bindings with `pip install .` |
| "Segmentation fault" | Memory access violation | Check array bounds, validate input parameters |
| "Zero firing neurons" | No input current or incorrect thresholds | Inject current, adjust `v_thresh`, verify configuration |
| "Agent not moving" | No motor commands generated | Check sensory input, debug brain activity |
| "Everything is 0" | Simulation needs warm-up | Run more steps, provide initial input |

## Common Issues and Solutions

### Issue 1: "Brain Not Doing Anything"

**Symptoms:**
- Zero firing neurons after initialization
- No changes in brain state across simulation steps
- Simulation appears frozen or inactive

**Root Causes:**

1. **Brain not properly initialized**
   - `brain.initialize()` not called before simulation
   - Brain created but not activated
   - Configuration parameters invalid

2. **Insufficient input current**
   - No sensory input to activate neurons
   - Thresholds too high or resting potential too low
   - No external stimulation

3. **Incorrect configuration**
   - Invalid neuron count or synapse density
   - Missing critical configuration parameters
   - Incorrect data types or ranges

**Solutions:**

#### Solution 1: Check Brain Initialization

```python
import pynlm

# Ensure proper initialization
config = pynlm.Config()
config.set("brain.neuron_count", 100)

brain = pynlm.Brain(config)
brain.initialize()  # <- Always call this!

# Verify brain was created
assert brain.getTotalNeuronCount() == 100
print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
```

#### Solution 2: Provide Input Current

```python
import pynlm

# Create and initialize brain
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

# Inject current to ensure neuron activity
for step in range(100):
    # Inject current to sensory neurons
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
    brain.step(step)  # Step the brain
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Solution 3: Check Configuration

```python
import pynlm

# Verify critical configuration
config = pynlm.Config()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)  # Connection probability
config.set("brain.v_thresh", -50.0)  # Threshold potential
config.set("brain.v_reset", -75.0)   # Reset potential
config.set("brain.tau_mem", 20.0)    # Membrane time constant

# Validate parameter types
assert isinstance(config.get("brain.neuron_count"), int)
assert isinstance(config.get("brain.synapse_density"), float)
assert isinstance(config.get("brain.plasticity_enabled"), bool)
```

#### Solution 4: Increase Simulation Steps

```python
import pynlm

# Create brain
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

# Brains need time to "warm up"
print("Running simulation to allow brain to activate...")
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        firing = brain.getFiringNeuronCount()
        print(f"Step {step}: {firing} firing neurons")
        
        # If still zero, debug further
        if firing == 0 and step > 500:
            print("WARNING: Brain still inactive after 500 steps!")
            print("Consider injecting current or adjusting configuration.")
```

### Issue 2: "Agent Isn't Moving"

**Symptoms:**
- Zero motor commands generated
- Agent remains in place
- No interaction with environment

**Root Causes:**

1. **No sensory input available**
   - World not configured properly
   - No percepts generated
   - Agent-brain interface not established

2. **Brain not receiving percepts**
   - `agent.processSensoryInput()` not called
   - Brain initialization issues
   - Sensory transduction problems

3. **Motor decoding failed**
   - Brain not active enough
   - Motor neurons not properly configured
   - Decoding algorithm issues

4. **Action application failed**
   - World not accepting commands
   - Invalid action parameters
   - World boundary issues

**Solutions:**

#### Solution 1: Debug Sensory Processing

```python
import pynlm

# Check if we have sensory input
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

percept = world.getSensoryPercept()
print(f"Vision available: {percept.getVisionWidth()} x {percept.getVisionHeight()}")
print(f"Internal signals: {percept.getInternal()}")

# Create brain and agent
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

agent = pynlm.AgentBrain(brain)
agent.initialize(world)

# Process sensory input
agent.processSensoryInput(percept)
print(f"Brain active after input: {brain.getFiringNeuronCount()} firing neurons")
```

#### Solution 2: Check Agent Configuration

```python
import pynlm

# Create and configure agent
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

agent = pynlm.AgentBrain(brain)
agent.initialize(world)

# Enable motor subsystems
agent.enableMotorDecoding(True)  # If this exists
agent.enableActionGeneration(True)  # If this exists

# Debug motor activity
brain.step(0)
motor_neurons = brain.getMotorNeurons()  # If this exists
if motor_neurons:
    print(f"Motor neuron count: {len(motor_neurons)}")
    for neuron in motor_neurons[:5]:
        print(f"  Neuron V={neuron.getMembranePotential()}")
```

#### Solution 3: Debug Motor Command Generation

```python
import pynlm

# Complete setup
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.AgentBrain(brain)
agent.initialize(world)

# Run simulation with debugging
for step in range(50):
    world.update(0.1)
    
    # Get and debug sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Step brain
    brain.step(step)
    
    # Check brain activity
    firing = brain.getFiringNeuronCount()
    print(f"Step {step}: Firing={firing}, Total Spikes={brain.getTotalSpikeCount()}")
    
    # Try to get motor command
    action = agent.decodeMotorCommand()
    if action:
        print(f"  Action generated: {action}")
        world.applyMotorCommand(action, world.getSimulationTime())
    else:
        print(f"  No action generated - brain may be inactive")
```

### Issue 3: "Everything is 0"

**Symptoms:**
- All values showing 0
- No activity detected
- Simulation appears dead

**Root Causes:**

1. **Random seed issues**
   - Deterministic behavior leading to no activation
   - Zero initialization
   - Fixed seed causing same outcomes

2. **Initialization sequence problems**
   - Wrong order of operations
   - Missing initialization steps
   - State corruption

3. **Parameter validation failures**
   - Invalid parameter types
   - Out-of-range values
   - Missing required parameters

4. **Type conversion issues**
   - String vs numeric comparisons
   - Boolean vs integer confusion
   - Enum vs string mismatches

**Solutions:**

#### Solution 1: Check Random Seeds

```python
import pynlm
import numpy as np

# Set consistent random seed for reproducibility
config = pynlm.Config()
config.set("random_seed", 42)  # Fixed seed

brain = pynlm.Brain(config)
brain.initialize()

# Verify parameters are set correctly
print(f"Random seed: {config.get('random_seed')}")
print(f"Neuron count: {brain.getTotalNeuronCount()}")
```

#### Solution 2: Verify Parameter Types

```python
import pynlm

# Check parameter types match expectations
config = pynlm.Config()

# Test valid parameters
config.set("brain.neuron_count", 100)  # int
config.set("brain.synapse_density", 0.1)  # float
config.set("brain.plasticity_enabled", True)  # bool
config.set("brain.name", "test_brain")  # string

# Verify types
print(f"Neuron count (int): {config.get('brain.neuron_count')} - Type: {type(config.get('brain.neuron_count'))}")
print(f"Synapse density (float): {config.get('brain.synapse_density')} - Type: {type(config.get('brain.synapse_density'))}")
print(f"Plasticity enabled (bool): {config.get('brain.plasticity_enabled')} - Type: {type(config.get('brain.plasticity_enabled'))}")
print(f"Name (string): {config.get('brain.name')} - Type: {type(config.get('brain.name'))}")

# Demonstrate invalid parameter handling
try:
    config.set("brain.invalid_param", "should_be_int")
    invalid_value = config.get("brain.invalid_param")
    if isinstance(invalid_value, str):
        print("WARNING: String stored where int expected!")
except Exception as e:
    print(f"Error with invalid parameter: {e}")
```

#### Solution 3: Debug Initialization

```python
import pynlm

# Log brain initialization steps
print("=== Brain Initialization Debug ===")
config = pynlm.Config()
print(f"Creating brain with config: {config.summary()}")

brain = pynlm.Brain(config)
print(f"Brain created: {brain.getTotalNeuronCount()} neurons")

brain.initialize()
print(f"Brain initialized successfully")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")

# Test step
brain.step(0)
print(f"After first step: Firing={brain.getFiringNeuronCount()}, Total Spikes={brain.getTotalSpikeCount()}")

# Check if brain is working
if brain.getFiringNeuronCount() == 0 and brain.getTotalSpikeCount() == 0:
    print("WARNING: Brain is inactive!")
    print("Suggestions:")
    print("1. Lower threshold potential (v_thresh)")
    print("2. Provide input current")
    print("3. Increase simulation steps")
    print("4. Check configuration parameters")
```

### Issue 4: "Segmentation Fault"

**Symptoms:**
- Program crashes with segmentation fault
- Memory access violations
- Stack overflow
- Core dump generated

**Root Causes:**

1. **Null pointer dereferences**
   - Uninitialized pointers
   - Objects not properly constructed
   - Memory management issues

2. **Invalid memory access**
   - Array index out of bounds
   - Pointer arithmetic errors
   - Memory corruption

3. **Stack overflow**
   - Infinite recursion
   - Deep call stacks
   - Large local variables

4. **Resource exhaustion**
   - Memory allocation failures
   - File descriptor limits
   - CPU time limits

**Solutions:**

#### Solution 1: Check Python Binding Usage

```python
import pynlm
import traceback

# Use brain in try-catch blocks
try:
    config = pynlm.Config()
    brain = pynlm.Brain(config)

    brain.initialize()
    brain.step(0)
    print("Brain operations completed successfully")
    
except Exception as e:
    print(f"Error occurred: {type(e).__name__}: {e}")
    print("\nFull traceback:")
    traceback.print_exc()
```

#### Solution 2: Validate Input Parameters

```python
def validate_config(config):
    """Validate all input parameters before use"""
    
    # Check parameter types match expectations
    if config.get("brain.neuron_count", 0) <= 0:
        raise ValueError("Neuron count must be positive")
    
    density = config.get("brain.synapse_density", 0)
    if density < 0 or density > 1:
        raise ValueError("Synapse density must be between 0 and 1")
    
    timestep = config.get("simulation_timestep", 0)
    if timestep <= 0:
        raise ValueError("Simulation timestep must be positive")
    
    print("✓ Configuration validation passed")

# Example usage
config = pynlm.Config()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)
config.set("simulation_timestep", 0.001)

validate_config(config)
```

#### Solution 3: Use Limited Resources

```python
import pynlm

# Start with small, manageable configurations
print("=== Progressive Brain Testing ===")

# Test 1: Start with minimal configuration
config1 = pynlm.Config()
config1.set("brain.neuron_count", 10)  # Start small
config1.set("brain.synapse_density", 0.05)  # Fewer connections

brain1 = pynlm.Brain(config1)
brain1.initialize()

print(f"Test 1: {brain1.getTotalNeuronCount()} neurons")
brain1.step(0)
print(f"  After step 0: {brain1.getFiringNeuronCount()} firing neurons")
print("✓ Basic test passed")

# Test 2: Gradually increase complexity
config2 = pynlm.Config()
config2.set("brain.neuron_count", 50)
config2.set("brain.synapse_density", 0.08)

brain2 = pynlm.Brain(config2)
brain2.initialize()

print(f"\nTest 2: {brain2.getTotalNeuronCount()} neurons")
for i in range(5):
    brain2.step(i)
print(f"  After 5 steps: {brain2.getFiringNeuronCount()} firing neurons")
print("✓ Intermediate test passed")

# Test 3: Full-scale configuration
config3 = pynlm.Config()
config3.set("brain.neuron_count", 100)
config3.set("brain.synapse_density", 0.1)

brain3 = pynlm.Brain(config3)
brain3.initialize()

print(f"\nTest 3: {brain3.getTotalNeuronCount()} neurons")
for i in range(20):
    brain3.step(i)
print(f"  After 20 steps: {brain3.getFiringNeuronCount()} firing neurons")
print("✓ Full-scale test passed")
```

### Issue 5: "Import Errors"

**Symptoms:**
- `ImportError: No module named 'pynlm'`
- `ModuleNotFoundError: No module named 'pynlm'`
- `AttributeError: module 'pynlm' has no attribute`

**Root Causes:**

1. **Installation issues**
   - Not properly installed NLM
   - Wrong installation method
   - Missing dependencies

2. **Path configuration problems**
   - PYTHONPATH not set correctly
   - Module not in search path
   - Virtual environment issues

3. **Missing dependencies**
   - pybind11 not installed
   - scikit-build-core missing
   - NumPy not available

4. **Version mismatches**
   - Python version incompatibility
   - NLM version mismatch
   - Dependency version conflicts

**Solutions:**

#### Solution 1: Check Python Installation

```python
import sys
import subprocess

print("Python installation check:")
print(f"Python executable: {sys.executable}")
print(f"Python version: {sys.version}")
print(f"Python path: {sys.path}")

# Check if pynlm is importable
try:
    import pynlm
    print(f"\n✓ pynlm imported successfully")
    print(f"pynlm version: {pynlm.__version__}")
    
except ImportError as e:
    print(f"\n✗ ImportError: {e}")
    print("\nTrying to install dependencies...")
    
    # Install required dependencies
    subprocess.check_call([sys.executable, "-m", "pip", "install", "pybind11", "scikit-build-core", "numpy"])
    
    # Try importing again
    try:
        import pynlm
        print(f"✓ pynlm imported successfully after dependency installation")
        
    except ImportError as e2:
        print(f"✗ Still cannot import pynlm: {e2}")
        print("\nPlease install NLM properly:")
        print("  cd /path/to/nlm")
        print("  pip install .")
```

#### Solution 2: Install Dependencies

```bash
# Install required dependencies
pip install pybind11 scikit-build-core numpy

# Install NLM from source
pip install .

# Or install in development mode
pip install -e .
```

#### Solution 3: Check Installation Path

```python
import sys
import os

print("Installation path check:")
print(f"Current working directory: {os.getcwd()}")
print(f"Python path: {sys.path}")

# Try to find pynlm module
import pkgutil
if pkgutil.find_loader("pynlm"):
    print("✓ pynlm module found")
    
    # Get module location
    module = pkgutil.get_loader("pynlm")
    if module:
        print(f"Module location: {module.get_filename()}")
else:
    print("✗ pynlm module not found")
    print("\nPossible solutions:")
    print("1. Install pynlm: pip install pynlm")
    print("2. Check PYTHONPATH environment variable")
    print("3. Activate correct virtual environment")
    print("4. Ensure NLM is properly built")
```

## Performance Issues

### High Memory Usage

**Symptoms:**
- Memory usage exceeds available RAM
- System becomes unresponsive
- Swap file heavily used

**Solutions:**

#### Solution 1: Profile Memory Usage

```python
import tracemalloc
import pynlm

# Start tracing
 tracemalloc.start()

# Create and run brain
config = pynlm.Config()
config.set("brain.neuron_count", 1000)
brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get memory usage
current, peak = tracemalloc.get_traced_memory()
print(f"Current memory usage: {current / 1024 ** 2:.2f} MB")
print(f"Peak memory usage: {peak / 1024 ** 2:.2f} MB")

# Stop tracing
tracemalloc.stop()
```

#### Solution 2: Reduce Memory Footprint

```python
import pynlm

# Use more efficient configurations
config = pynlm.Config()
config.set("brain.neuron_count", 100)  # Smaller brain
config.set("brain.synapse_density", 0.05)  # Fewer synapses

brain = pynlm.Brain(config)
brain.initialize()

print(f"Reduced memory configuration:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Brain size: {brain.getBrainSize()} bytes")
```

### Slow Performance

**Symptoms:**
- Simulation runs slowly
- Bottlenecks in neural computation
- Long execution times

**Solutions:**

#### Solution 1: Profile Performance

```python
import time
import pynlm

# Create brain
config = pynlm.Config()
config.set("brain.neuron_count", 500)
brain = pynlm.Brain(config)
brain.initialize()

# Measure execution time
start_time = time.time()

for step in range(1000):
    brain.step(step)

end_time = time.time()

print(f"Execution time: {end_time - start_time:.2f} seconds")
print(f"Average time per step: {(end_time - start_time) / 1000:.4f} seconds")
print(f"Steps per second: {1000 / (end_time - start_time):.1f}")
```

#### Solution 2: Optimize Configuration

```python
import pynlm

# Optimize for speed
config = pynlm.Config()
config.set("brain.neuron_count", 500)  # Reasonable size
config.set("simulation_timestep", 0.001)  # Faster timestep
config.set("plasticity.enabled", False)  # Disable plasticity for speed

brain = pynlm.Brain(config)
brain.initialize()

print("Performance-optimized configuration:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Plasticity enabled: {config.get('plasticity.enabled')}")
```

## Platform-Specific Issues

### Windows-Specific Issues

**Symptoms:**
- Path separator issues
- DLL loading problems
- Memory alignment issues

**Solutions:**

#### Solution 1: Use Path Module

```python
import os
from pathlib import Path

# Use Path for cross-platform compatibility
config_path = Path("config.json")
if config_path.exists():
    # Load configuration
    config = pynlm.Config()
    # ... configuration loading logic

# Save configuration
config.saveToFile(str(config_path))
```

#### Solution 2: Check DLL Dependencies

```python
import ctypes
import sys

# Check if required DLLs are available
required_dlls = ["kernel32.dll", "user32.dll"]
for dll in required_dlls:
    if not ctypes.util.find_library(dll):
        print(f"Warning: {dll} not found")

# Try to import and check
print(f"Python version: {sys.version}")
print(f"Platform: {sys.platform}")
```

## Common Error Messages

### "Invalid configuration parameter"

**Solution:**
```python
# Check parameter validation
config = pynlm.Config()
config.set("brain.neuron_count", 100)  # Valid
config.set("brain.synapse_density", 1.5)  # Invalid (> 1.0)
# This will raise ValueError
```

### "Neuron threshold not reached"

**Solution:**
```python
# Check neuron threshold configuration
config = pynlm.Config()
config.set("brain.v_thresh", -50.0)  # Reasonable threshold
config.set("brain.v_reset", -75.0)   # Reset potential
config.set("brain.tau_mem", 20.0)   # Membrane time constant
```

### "Memory allocation failed"

**Solution:**
```python
import psutil
import os

# Check memory allocation
process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 ** 2:.2f} MB")

# Free memory if needed
import gc
gc.collect()
```

## Debug Information

### Enable Debug Logging

```python
import logging

# Enable comprehensive debug logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('debug.log')
    ]
)

# Use debug logging in your code
logger = logging.getLogger(__name__)

logger.debug(f"Brain configuration: {config.summary()}")
logger.debug(f"Neuron count: {brain.getTotalNeuronCount()}")
logger.debug(f"Synapse count: {brain.getTotalSynapseCount()}")
```

### Collecting Debug Information

```python
def collect_debug_info(brain, config, step):
    debug_info = {
        "step": step,
        "neuron_count": brain.getTotalNeuronCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount(),
        "excitatory_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "configuration": config.summary(),
        "developmental_stage": brain.getDevelopmentalStage(),
    }
    return debug_info

# Collect debug info during simulation
for step in range(100):
    brain.step(step)
    debug_info = collect_debug_info(brain, config, step)
    # Log or save debug info
    print(f"Debug info at step {step}: {debug_info}")
```

## Final Tips

### 1. Start Simple

```python
# Start with the simplest possible configuration
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()
brain.step(0)
print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### 2. Test Incrementally

```python
# Test each component individually
# 1. Test configuration
config = pynlm.Config()
assert config.has("brain.neuron_count")

# 2. Test brain creation
brain = pynlm.Brain(config)
assert brain.getTotalNeuronCount() == 100

# 3. Test initialization
brain.initialize()
assert brain.getActiveNeuronCount() >= 0

# 4. Test simulation
brain.step(0)
assert brain.getFiringNeuronCount() >= 0
```

### 3. Use Print Statements

```python
# Add debug print statements
config = pynlm.Config()
print(f"Created config with {config.summary()}")

brain = pynlm.Brain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

brain.initialize()
print(f"Initialized brain with {brain.getActiveNeuronCount()} active neurons")

brain.step(0)
print(f"After first step: {brain.getFiringNeuronCount()} firing neurons")
```

### 4. Check Documentation

Always refer to the latest documentation for your version:
- Check README.md for basic usage
- Check HOW_TO_USE.md for detailed instructions
- Check easy_usage.md for quick examples
- Check API_REFERENCE.md for method details

## FAQ

### Q: What is the difference between `pynlm.Brain` and `pynlm.createBrain()`?

A: `pynlm.Brain` is the constructor class, while `pynlm.createBrain()` is a factory function that creates and initializes a brain with a given configuration. Use the factory function for normal usage.

### Q: How do I save and load brain states?

A: Use `brain.save("filename.bin")` to save and `brain.load("filename.bin")` to load. Configuration can be saved to JSON files with `config.saveToFile("config.json")`.

### Q: What's the difference between `step()` and `step(step, time)`?

A: `step(step)` uses the default timestep (usually 0.1), while `step(step, time)` allows you to specify the exact time for that step.

### Q: How do I enable all learning features?

A: Call all the enable methods: `agent.enableRewardModulation(True)`, `agent.enableStructuralPlasticity(True)`, `agent.enableDevelopment(True)`, and `agent.enableCuriosity(True)`.

### Q: What's the maximum number of neurons I can simulate?

A: This depends on your hardware and memory. Start small (100-1000 neurons) and gradually increase. For large-scale simulations, consider performance optimizations.

### Q: How do I handle different Python versions?

A: NLM should work with Python 3.8+. For specific issues, check the Python version and install the correct dependencies.

### Q: Can I use NLM in a virtual environment?

A: Yes! Just ensure all dependencies (pybind11, scikit-build-core, numpy) are installed in your virtual environment, and install NLM with `pip install .`

### Q: What's the difference between STDP and Hebbian learning?

A: STDP (Spike-Timing-Dependent Plasticity) depends on the precise timing of pre- and post-synaptic spikes, while Hebbian learning is correlation-based ("neurons that fire together, wire together").

### Q: How do I debug why my brain isn't firing?

A: Check: 1) Brain initialization, 2) Configuration parameters, 3) Input current, 4) Threshold settings, 5) Number of simulation steps. Use print statements and the debug functions provided.

### Q: Can I run NLM examples without installing NLM?

A: The examples require NLM to be installed. You can run them after building and installing NLM with `pip install .`.

### Q: What's the difference between `createSimpleWorld()` and `createDefaultConfig()`?

A: `createSimpleWorld()` creates an environment for the agent to interact with, while `createDefaultConfig()` creates brain configuration parameters. They serve different purposes in the NLM system.

This comprehensive troubleshooting guide and FAQ should help you resolve most issues with NLM. Remember to start simple, test incrementally, and use debugging tools to identify problems quickly.
