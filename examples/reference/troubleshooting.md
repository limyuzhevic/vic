# Troubleshooting

This document provides common issues and solutions for NLM (Neural Learning Machine) programming. It covers troubleshooting techniques for configuration, simulation, memory, performance, and other common problems.

## Overview

Troubleshooting is an essential part of working with NLM. This document provides systematic approaches to identify, diagnose, and resolve common issues that arise during NLM development and usage.

## Issue Categories

### 1. Configuration Issues

#### Problem 1: Configuration Validation Fails

**Description:** Configuration validation fails due to invalid configuration parameters.

**Symptoms:**
- Validation errors when loading configuration
- Configuration warnings during initialization
- Unexpected default values

**Root Cause:**
- Invalid configuration keys
- Values outside expected ranges
- Missing required configuration parameters
- Incorrect data types

**Solutions:**

1. **Check Configuration File:**
```python
# Check configuration file for errors
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Validate configuration
validation_errors = config.validateConfiguration()
if validation_errors:
    for error in validation_errors:
        print(f"Error: {error}")
```

2. **Use Configuration Schema:**
```python
# Create custom schema
from jsonschema import validate, ValidationError

configuration_schema = {
    "type": "object",
    "properties": {
        "neuron_count": {
            "type": "integer",
            "minimum": 1,
            "maximum": 100000
        },
        "region_count": {
            "type": "integer",
            "minimum": 1,
            "maximum": 10
        }
    },
    "required": ["neuron_count", "region_count"]
}

# Validate configuration
try:
    validate(config_dict, configuration_schema)
    print("Configuration is valid!")
except ValidationError as e:
    print(f"Validation error: {e}")
```

3. **Use Default Configuration:**
```python
# Use default configuration
config = pynlm.createDefaultConfig()
# Manually set required parameters
config.set("neuron_count", 1000)
config.set("region_count", 2)
```

4. **Debug Configuration Issues:**
```python
# Debug configuration issues
config = pynlm.createDefaultConfig()
config.set("neuron_count", 0)  # Invalid value
config.set("region_count", 11)  # Invalid value

# Check for errors
if config.has("neuron_count"):
    neuron_count = config.get("neuron_count")
    if neuron_count < 1 or neuron_count > 100000:
        print(f"Error: neuron_count must be between 1 and 100000, got {neuron_count}")
        
if config.has("region_count"):
    region_count = config.get("region_count")
    if region_count < 1 or region_count > 10:
        print(f"Error: region_count must be between 1 and 10, got {region_count}")
```

#### Problem 2: Configuration Not Loading

**Description:** Configuration fails to load from file.

**Symptoms:**
- File not found errors
- Permission errors
- Corrupted configuration files
- Invalid file format

**Root Cause:**
- File system issues
- Permission problems
- File format incompatibility
- Network connectivity issues (for remote files)

**Solutions:**

1. **Check File Path:**
```python
import os

# Check if file exists
config_file = "config.json"
if not os.path.exists(config_file):
    print(f"Error: Configuration file '{config_file}' not found")
    print(f"Current directory: {os.getcwd()}")
    
# Try absolute path
abs_path = os.path.abspath(config_file)
if os.path.exists(abs_path):
    print(f"File found at: {abs_path}")
    # Load configuration
    config.loadFromFile(abs_path)
```

2. **Check File Permissions:**
```python
import os

config_file = "config.json"
if os.path.exists(config_file):
    # Check if file is readable
    if not os.access(config_file, os.R_OK):
        print(f"Error: No read permission for '{config_file}'")
        
    # Try to change permissions (if you have rights)
    os.chmod(config_file, 0o644)
    print(f"Permissions changed for '{config_file}'")
```

3. **Try Different File Formats:**
```python
# Try JSON format
config.loadFromFile("config.json")

# Try INI format  
config.loadFromFile("config.ini")

# Try TXT format
config.loadFromFile("config.txt")
```

4. **Debug File Issues:**
```python
# Debug file issues
import os
import json

config_file = "config.json"

# Try to open and read file
try:
    with open(config_file, 'r') as f:
        content = f.read()
        print(f"File content (first 100 chars): {content[:100]}")
        
        # Try to parse JSON
        config_data = json.loads(content)
        print(f"Configuration keys: {list(config_data.keys())}")
        
except FileNotFoundError:
    print(f"Error: File '{config_file}' not found")
except json.JSONDecodeError as e:
    print(f"Error: Invalid JSON format: {e}")
except Exception as e:
    print(f"Error: {e}")
```

#### Problem 3: Configuration Not Applied

**Description:** Configuration changes are not applied correctly.

**Symptoms:**
- Configuration changes ignored
- Unexpected default values
- Configuration not saved
- Configuration loaded but not used

**Root Cause:**
- Configuration not properly set before use
ίαςCorrect configuration loading
- Configuration reset accidentally
- Configuration overwritten by defaults

**Solutions:**

1. **Apply Configuration Before Use:**
```python
# Create configuration
config = pynlm.createDefaultConfig()

# Apply configuration
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("plasticity_learning_rate", 0.01)

# Create brain with configuration
brain = pynlm.createBrain(config)
brain.initialize()  # Configuration applied here
```

2. **Debug Configuration Application:**
```python
# Debug configuration application
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

print(f"Configuration applied:")
print(f"  neuron_count: {config.get('neuron_count')}")
print(f"  region_count: {config.get('region_count', 'default')}")

# Create brain and check configuration
brain = pynlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

3. **Check Configuration Source:**
```python
# Check configuration source
config = pynlm.createDefaultConfig()

# Set configuration from different sources
config.set("neuron_count", 1000)  # Direct setting
config.loadFromFile("config.json")  # File loading
config.loadFromArgs(sys.argv)  # Command line loading

# The last operation determines the final configuration
print(f"Final neuron_count: {config.get('neuron_count')}")
```

### 2. Simulation Issues

#### Problem 1: Simulation Fails to Start

**Description:** Simulation fails to start or crashes immediately.

**Symptoms:**
- Simulation crashes on startup
- No output or error messages
- Unexpected termination
- Memory errors

**Root Cause:**
- Invalid brain configuration
- Memory allocation issues
- Invalid world configuration
- Agent initialization errors

**Solutions:**

1. **Check Brain Configuration:**
```python
# Create minimal valid configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Small, manageable size
config.set("region_count", 1)

# Create brain
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain created successfully")
except Exception as e:
    print(f"Brain creation error: {e}")
    # Try with even smaller configuration
    config.set("neuron_count", 10)
    config.set("region_count", 1)
    brain = pynlm.createBrain(config)
    brain.initialize()
```

2. **Check World Configuration:**
```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world properly
try:
    world.configure(10, 10, 8, 8)  # Small world
    world.reset()
    print("World configured successfully")
except Exception as e:
    print(f"World configuration error: {e}")
    # Try with default world
    world = pynlm.createSimpleWorld()
    world.configure(5, 5, 5, 5)
    world.reset()
```

3. **Check Agent Initialization:**
```python
# Create agent
agent = pynlm.createAgentBrain(brain)

try:
    agent.initialize(world)
    print("Agent initialized successfully")
except Exception as e:
    print(f"Agent initialization error: {e}")
    # Check if agent has required methods
    if hasattr(agent, 'initialize'):
        agent.initialize(world)
    else:
        print("Agent does not have initialize method")
```

#### Problem 2: Simulation Runs Slowly

**Description:** Simulation performance is poor.

**Symptoms:**
- Simulation too slow
- High memory usage
- Long execution times
- System unresponsiveness

**Root Cause:**
- Too many neurons or synapses
- Inefficient algorithms
- Memory allocation issues
- Python performance limitations

**Solutions:**

1. **Reduce Simulation Size:**
```python
# Use smaller configuration for testing
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Reduced from 1000
config.set("region_count", 1)  # Reduced from 2

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step} completed")
```

2. **Use Efficient Data Structures:**
```python
# Use numpy arrays for efficiency
import numpy as np

class EfficientBrain:
    def __init__(self, config):
        neuron_count = config.get("neuron_count")
        self.neurons = np.zeros(neuron_count, dtype=np.float32)
        self.synapses = np.zeros((neuron_count, neuron_count), dtype=np.float32)
        
    def step(self, step):
        # Use vectorized operations
        self.neurons += np.random.normal(0, 0.1, len(self.neurons))
```

3. **Profile Performance:**
```python
import time
import memory_profiler

# Profile simulation performance
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)

brain = pynlm.createBrain(config)
brain.initialize()

# Measure execution time
start_time = time.time()
for step in range(100):
    brain.step(step)
end_time = time.time()

print(f"Total execution time: {end_time - start_time:.2f} seconds")
print(f"Average time per step: {(end_time - start_time) / 100:.4f} seconds")

# Measure memory usage
mem_usage_before = memory_profiler.memory_usage()[0]
for step in range(100):
    brain.step(step)
mem_usage_after = memory_profiler.memory_usage()[0]

print(f"Memory usage increase: {mem_usage_after - mem_usage_before:.2f} MB")
```

#### Problem 3: Simulation Crashes Mid-way

**Description:** Simulation crashes during execution.

**Symptoms:**
- Sudden termination
- Memory errors
- Segmentation faults
- Python exceptions

**Root Cause:**
- Memory issues
- Infinite loops
- Division by zero
- Invalid operations

**Solutions:**

1. **Add Error Handling:**
```python
try:
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Simulation error at step {step}: {e}")
    # Handle error and continue
    continue
```

2. **Use Try-Catch for Individual Steps:**
```python
for step in range(100):
    try:
        brain.step(step)
        print(f"Step {step} completed")
    except Exception as e:
        print(f"Step {step} failed: {e}")
        # Record failed step for debugging
        failed_steps.append(step)
        # Optionally break or continue
        continue
```

3. **Debug with Smaller Steps:**
```python
# Use smaller step size for debugging
for step in range(10):  # Reduced from 100
    try:
        brain.step(step)
        print(f"Step {step} completed")
    except Exception as e:
        print(f"Step {step} failed: {e}")
        # Debug the error
        import traceback
        traceback.print_exc()
        break
```

### 3. Memory Issues

#### Problem 1: Memory Usage Too High

**Description:** Memory usage is excessively high.

**Symptoms:**
- System memory exhaustion
- Swapping
- Application termination
- Performance degradation

**Root Cause:**
- Too many neurons or synapses
- Inefficient data structures
- Memory leaks
- Large data allocations

**Solutions:**

1. **Reduce Memory Usage:**
```python
# Use memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Reduced from 1000
config.set("region_count", 1)  # Reduced from 2

# Use memory-efficient brain implementation
class MemoryEfficientBrain:
    def __init__(self, config):
        neuron_count = config.get("neuron_count")
        # Use smaller data types
        self.neurons = np.zeros(neuron_count, dtype=np.float32)
        self.synapses = np.zeros((neuron_count, neuron_count), dtype=np.float32)
```

2. **Monitor Memory Usage:**
```python
import psutil
import os

# Monitor memory usage
process = psutil.Process(os.getpid())

for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        memory_info = process.memory_info()
        memory_mb = memory_info.rss / (1024 * 1024)
        print(f"Step {step}: {memory_mb:.2f} MB")
        
        if memory_mb > 1000:  # 1GB limit
            print("Warning: High memory usage!")
            break
```

3. **Use Memory Pooling:**
```python
class MemoryPoolBrain:
    def __init__(self, config):
        self.config = config
        self.memory_pool = MemoryPool()
        self.current_allocation = 0
        
    def allocate(self, size):
        """Allocate memory from pool."""
        if self.current_allocation + size > self.config.get("neuron_count"):
            raise MemoryError("Insufficient memory in pool")
            
        self.current_allocation += size
        return self.memory_pool.allocate(size)
        
    def deallocate(self, size):
        """Deallocate memory to pool."""
        self.current_allocation -= size
        self.memory_pool.deallocate(size)
```

#### Problem 2: Memory Leaks

**Description:** Memory is not being properly freed.

**Symptoms:**
- Increasing memory usage
- Memory not freed after cleanup
- System memory exhaustion

**Root Cause:**
- Circular references
- Global variables
- Unreleased resources
- Improper cleanup

**Solutions:**

1. **Use Weak References:**
```python
import weakref

class ReferenceCountedBrain:
    def __init__(self):
        self.strong_ref = []
        self.weak_ref = weakref.WeakSet()
        
    def add_strong_ref(self, obj):
        self.strong_ref.append(obj)
        
    def add_weak_ref(self, obj):
        self.weak_ref.add(obj)
        
    def cleanup(self):
        self.strong_ref.clear()
        self.weak_ref.clear()
```

2. **Use Context Managers:**
```python
class ManagedBrain:
    def __enter__(self):
        self.initialize()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.cleanup()
        
    def initialize(self):
        # Initialize brain
        pass
        
    def cleanup(self):
        # Cleanup resources
        pass
```

3. **Monitor Memory:**
```python
import tracemalloc

# Start tracing memory allocations
tracemalloc.start()

# Run simulation
for step in range(100):
    brain.step(step)

# Get memory statistics
current, peak = tracemalloc.get_traced_memory()
print(f"Current memory usage: {current / 1024 / 1024:.2f} MB")
print(f"Peak memory usage: {peak / 1024 / 1024:.2f} MB")

tracemalloc.stop()
```

### 4. Performance Issues

#### Problem 1: Simulation Too Slow

**Description:** Simulation execution time is too high.

**Symptoms:**
- Long execution times
- Low steps per second
- System unresponsiveness
- Performance bottlenecks

**Root Cause:**
- Inefficient algorithms
- Python performance limitations
- Memory access patterns
- Data structure inefficiencies

**Solutions:**

1. **Use Optimized Algorithms:**
```python
# Use vectorized operations
import numpy as np

class OptimizedBrain:
    def __init__(self, config):
        neuron_count = config.get("neuron_count")
        # Use vectorized operations
        self.neurons = np.random.normal(0, 1, neuron_count)
        self.synapses = np.random.normal(0, 0.1, (neuron_count, neuron_count))
        
    def step(self, step):
        # Use vectorized operations
        self.neurons += np.dot(self.synapses, self.neurons)
        
        # Apply non-linearity
        self.neurons = np.tanh(self.neurons)
```

2. **Use C Extensions:**
```python
# Use C extensions for performance
cimport pynlm
brain = c_pynlm.createBrain(config)
```

3. **Profile Performance:**
```python
import cProfile
import pstats

# Profile simulation performance
with cProfile.Profile() as pr:
    for step in range(100):
        brain.step(step)
        
# Generate statistics
stats = pstats.Stats(pr).sort_stats('cumulative')
stats.print_stats()
```

#### Problem 2: High CPU Usage

**Description:** CPU usage is excessively high.

**Symptoms:**
- System CPU usage near 100%
- System unresponsiveness
- Fan noise
- High temperature

**Root Cause:**
- Inefficient algorithms
- Infinite loops
- Concurrent tasks
- Busy wait loops

**Solutions:**

1. **Use Efficient Algorithms:**
```python
# Use efficient algorithms
class EfficientBrain:
    def __init__(self, config):
        self.config = config
        self.neurons = np.zeros(config.get("neuron_count"))
        
    def step(self, step):
        # Use efficient algorithm
        self.neurons = np.dot(self.synapses, self.neurons)
        
        # Apply non-linearity
        self.neurons = self.apply_non_linearity(self.neurons)
        
    def apply_non_linearity(self, neurons):
        # Use efficient non-linearity
        return np.tanh(neurons)
```

2. **Use Parallel Processing:**
```python
import concurrent.futures

class ParallelBrain:
    def __init__(self, config):
        self.config = config
        self.neurons = np.zeros(config.get("neuron_count"))
        
    def step(self, step):
        # Use parallel processing for large neuron sets
        with concurrent.futures.ThreadPoolExecutor() as executor:
            # Process neurons in parallel
            futures = [executor.submit(self.process_neuron, i) 
                      for i in range(len(self.neurons))]
            results = [f.result() for f in concurrent.futures.as_completed(futures)]
            
        self.neurons = np.array(results)
```

### 5. Development Issues

#### Problem 1: Development System Not Working

**Description:** Development system fails to function properly.

**Symptoms:**
- No developmental changes
- Constant developmental stage
- No plasticity modulation
- No structural changes

**Root Cause:**
- Development system not initialized
- Development system disabled
- Configuration issues
- Integration issues

**Solutions:**

1. **Check Development System:**
```python
# Get development system
development_system = brain.getDevelopmentSystem()

# Check if development system is enabled
print(f"Development system enabled: {development_system.isCriticalPeriod()}")

# Set developmental stage
development_system.setCurrentStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Update development system
for step in range(10):
    development_system.update(brain, step)
    print(f"Step {step}: Stage={development_system.getStageName()}, Plasticity={development_system.getPlasticityModifier():.3f}")
```

2. **Debug Development Issues:**
```python
# Debug development issues
development_system = brain.getDevelopmentSystem()

# Check development system properties
print(f"Current stage: {development_system.getCurrentStage()}")
print(f"Stage name: {development_system.getStageName()}")
print(f"Plasticity modifier: {development_system.getPlasticityModifier():.3f}")
print(f"Critical period: {development_system.isCriticalPeriod()}")
```

#### Problem 2: Neuromodulation System Not Working

**Description:** Neuromodulation system fails to function properly.

**Symptoms:**
- No neuromodulation effects
- Constant neuromodulation levels
- No learning enhancement
- No behavior modification

**Root Cause:**
- Neuromodulation system not initialized
- Neuromodulation system disabled
- Configuration issues
- Integration issues

**Solutions:**

1. **Check Neuromodulation System:**
```python
# Get neuromodulation systems
dopamine = pynlm.createDopamine()
curiosity = pynlm.createCuriosity()
novelty = pynlm.createNovelty()
prediction_error = pynlm.createPredictionError()

# Check neuromodulation levels
print(f"Dopamine level: {dopamine.getLevel():.3f}")
print(f"Curiosity level: {curiosity.getCuriosityLevel():.3f}")
print(f"Novelty level: {novelty.getLevel():.3f}")
print(f"Prediction error: {prediction_error.getPredictionError():.3f}")
```

2. **Enable Neuromodulation:**
```python
# Enable neuromodulation
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Use neuromodulation in simulation
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent (neuromodulation active)
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 6. Integration Issues

#### Problem 1: System Integration Fails

**Description:** Integration between brain, world, and agent fails.

**Symptoms:**
- System initialization fails
- Integration errors
- Missing functionality
- Unexpected behavior

**Root Cause:**
- Integration code issues
- Missing dependencies
- Configuration issues
- Initialization order

**Solutions:**

1. **Check Integration Code:**
```python
# Check integration code
try:
    # Initialize components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Integration successful!")
    
except Exception as e:
    print(f"Integration error: {e}")
    import traceback
    traceback.print_exc()
```

2. **Debug Integration:**
```python
# Debug integration
print("Step 1: Create brain")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print("✓ Brain created")

print("Step 2: Create world")
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()
print("✓ World created")

print("Step 3: Create agent")
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
print("✓ Agent created")

print("Step 4: Run simulation")
for step in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
print("✓ Simulation completed")
```

### 7. User Interface Issues

#### Problem 1: Console Output Issues

**Description:** Console output is not displayed properly.

**Symptoms:**
- No output displayed
- Incorrect output format
- Output truncated
- Encoding issues

**Root Cause:**
- Console configuration issues
- Encoding issues
- Output formatting
- Terminal issues

**Solutions:**

1. **Check Console Configuration:**
```python
# Check console configuration
import sys
import os

print("Python version:", sys.version)
print("Platform:", sys.platform)
print("Default encoding:", sys.getdefaultencoding())

# Set console encoding
sys.stdout.reconfigure(encoding='utf-8')
sys.stderr.reconfigure(encoding='utf-8')

print("Console encoding set to UTF-8")
```

2. **Use Print Function:**
```python
# Use print function
print("Hello, world!")
print("Simulation result: step completed")
print("Error message: Something went wrong")
```

3. **Check Terminal Settings:**
```python
# Check terminal settings
import platform

print(f"Platform: {platform.system()} {platform.release()}")
print(f"Terminal: {platform.node()}")
print(f"Python executable: {platform.python_version()}")
```

#### Problem 2: File Output Issues

**Description:** File output is not created properly.

**Symptoms:**
- File not created
- File empty
- Permission errors
- Format errors

**Root Cause:**
- File system issues
- Permission problems
- Format issues
- Path issues

**Solutions:**

1. **Check File System:**
```python
import os

# Check current directory
print(f"Current directory: {os.getcwd()}")

# Check if directory exists
output_dir = "output"
if not os.path.exists(output_dir):
    os.makedirs(output_dir)
    print(f"Created directory: {output_dir}")

# Create output file
output_file = os.path.join(output_dir, "output.txt")
try:
    with open(output_file, 'w') as f:
        f.write("Simulation output\n")
        print(f"File created: {output_file}")
except Exception as e:
    print(f"Error creating file: {e}")
```

2. **Check File Permissions:**
```python
import os

output_file = "output.txt"

# Check if file exists and is writable
if os.path.exists(output_file):
    if os.access(output_file, os.W_OK):
        print(f"File is writable: {output_file}")
    else:
        print(f"File is not writable: {output_file}")
        
        # Try to change permissions
        try:
            os.chmod(output_file, 0o644)
            print(f"Permissions changed for {output_file}")
        except Exception as e:
            print(f"Error changing permissions: {e}")
else:
    print(f"File does not exist: {output_file}")
```

### 8. Documentation Issues

#### Problem 1: Missing Documentation

**Description:** Documentation is missing or incomplete.

**Symptoms:**
- Missing API documentation
- Missing examples
- Missing tutorials
- Missing instructions

**Root Cause:**
- Documentation not written
- Documentation not maintained
- Documentation not updated

**Solutions:**

1. **Check Documentation:**
```python
# Check for documentation files
doc_files = ["README.md", "API_REFERENCE.md", "TUTORIALS/"]
for doc_file in doc_files:
    if os.path.exists(doc_file):
        print(f"Documentation found: {doc_file}")
    else:
        print(f"Documentation missing: {doc_file}")
```

2. **Generate Documentation:**
```python
# Generate documentation using Sphinx
import sphinx.cmd.build

# Build documentation
sphinx.cmd.build.main(['-b', 'html', 'docs', 'build/docs/html'])
print("Documentation built successfully")
```

3. **Create Documentation:**
```python
# Create documentation
with open("README.md", "w") as f:
    f.write("# NLM Python API\n")
    f.write("## Overview\n")
    f.write("This is the NLM Python API.\n")
    
with open("API_REFERENCE.md", "w") as f:
    f.write("# API Reference\n")
    f.write("## Classes\n")
    f.write("This document provides API reference.\n")
```

## Common Error Messages and Solutions

### Error: "Brain not found"
**Description:** Brain not found in file.
**Solution:** Ensure brain file exists or create new brain.

### Error: "Configuration error"
**Description:** Configuration error occurred.
**Solution:** Check configuration parameters and fix errors.

### Error: "Simulation error"
**Description:** Simulation error occurred.
**Solution:** Check simulation parameters and fix errors.

### Error: "Memory error"
**Description:** Memory allocation error.
**Solution:** Check memory usage and reduce allocation.

### Error: "Not implemented"
**Description:** Feature not implemented.
**Solution:** Check documentation for available features.

## Debugging Techniques

### 1. Use print Statements

```python
# Add debug print statements
for step in range(100):
    print(f"Step {step}: neurons = {len(brain.neurons)}, synapses = {len(brain.synapses)}")
    brain.step(step)
```

### 2. Use Logging

```python
import logging

# Configure logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_debug.log'),
        logging.StreamHandler()
    ]
)

# Log debug information
logger = logging.getLogger(__name__)
logger.debug(f"Step {step}: Processing")
```

### 3. Use Traceback

```python
try:
    # Code that might raise exceptions
    result = risky_operation()
except Exception as e:
    import traceback
    print("Error occurred:")
    traceback.print_exc()
```

### 4. Use Profiling

```python
import cProfile
import pstats

# Profile code execution
with cProfile.Profile() as pr:
    # Code to profile
    for step in range(100):
        brain.step(step)
        
# Generate statistics
stats = pstats.Stats(pr).sort_stats('cumulative')
stats.print_stats()
```

### 5. Use Memory Profiling

```python
import memory_profiler

# Profile memory usage
@memory_profiler.profile
def run_simulation():
    for step in range(100):
        brain.step(step)
        
run_simulation()
```

## Conclusion

Troubleshooting is an essential skill for NLM development and usage. By understanding common issues and their solutions, developers can:

- **Identify problems quickly:** Quickly identify the root cause of issues.
- **Resolve issues efficiently:** Apply proven solutions to resolve problems.
- **Prevent future issues:** Implement practices to prevent recurring issues.
- **Debug effectively:** Use systematic debugging techniques.

These troubleshooting techniques will help developers work more effectively with NLM and build robust, reliable neural simulations.

---

*Troubleshooting document provides systematic approaches to identify, diagnose, and resolve common NLM issues.*