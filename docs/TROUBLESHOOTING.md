# NLM Troubleshooting Guide

## Overview

This troubleshooting guide provides comprehensive information for diagnosing and resolving issues with NLM (Neural Learning Machine) applications. It covers common problems, their symptoms, solutions, and debugging strategies.

## Common Issues and Solutions

### 1. Import and Installation Issues

#### Issue 1: ImportError - No module named 'pynlm'

**Symptoms**:
- Python cannot import pynlm module
- Installation appears successful but module not found

**Possible Causes**:
1. Incorrect installation
2. Python path issues
3. Environment conflicts
4. Missing dependencies

**Solutions**:

**Solution 1: Verify Installation**
```bash
# Check if pynlm is installed
pip list | grep pynlm

# Install pynlm
pip install pynlm

# Or install in development mode
pip install -e .
```

**Solution 2: Check Python Path**
```python
import sys
print(sys.path)

# Add current directory to Python path
import os
os.environ['PYTHONPATH'] = '/path/to/nlm/python:' + os.environ['PYTHONPATH']

# Or install with correct environment
pip install --user .
```

**Solution 3: Check Dependencies**
```python
import pynlm

# Try to import all required modules
try:
    import numpy
    print("NumPy: OK")
except ImportError:
    print("NumPy: Not installed")

try:
    import scipy
    print("SciPy: OK")
except ImportError:
    print("SciPy: Not installed")

try:
    import matplotlib
    print("Matplotlib: OK")
except ImportError:
    print("Matplotlib: Not installed")
```

**Solution 4: Install Dependencies**
```bash
# Install required dependencies
pip install numpy scipy matplotlib pybind11 scikit-build-core pytest

# Install NLM
pip install .
```

#### Issue 2: ModuleNotFoundError - 'nlm'

**Symptoms**:
- Python cannot find 'nlm' module
- Import statements fail

**Possible Causes**:
1. Module name mismatch
2. Incorrect import statements
3. Installation issues
4. File system issues

**Solutions**:

**Solution 1: Check Module Name**
```python
# Check module name
import nlm
print(f"Module name: {nlm.__name__}")

# Check version
print(f"Module version: {nlm.__version__}")
```

**Solution 2: Correct Import Statements**
```python
# Correct import statements
import pynlm
from pynlm import Brain, Config
from nlm import createBrain, createDefaultConfig
```

**Solution 3: Check Installation**
```bash
# Check installed modules
pip show nlm
pip show pynlm

# Install if not present
pip install nlm
pip install pynlm
```

**Solution 4: Check Module Structure**
```bash
# Check module structure
find . -name "*.py" -type f | grep -E "(nlm|pynlm)" | head -20

# Check __init__.py files
find . -name "__init__.py" -type f | head -10
```

### 2. Configuration Issues

#### Issue 3: Configuration Validation Errors

**Symptoms**:
- Configuration validation fails
- Error messages about invalid values
- Configuration not being applied

**Possible Causes**:
1. Invalid configuration values
2. Missing required keys
3. Type mismatches
4. Schema validation failures

**Solutions**:

**Solution 1: Check Configuration Values**
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()

# Check if required keys are present
required_keys = ["neuron_count", "learning_rate"]
for key in required_keys:
    if not config.has(key):
        print(f"Missing required key: {key}")

# Check configuration values
neuron_count = config.getInt("neuron_count")
if neuron_count < 100 or neuron_count > 1000000:
    print(f"Invalid neuron count: {neuron_count}")

learning_rate = config.getDouble("learning_rate")
if learning_rate < 0.0 or learning_rate > 1.0:
    print(f"Invalid learning rate: {learning_rate}")
```

**Solution 2: Use Configuration Validation**
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("neuron_count", 1000)
config.set("learning_rate", 0.01)
config.set("enable_curiosity", True)

# Validate configuration
if config.validateAgainstSchema():
    print("Configuration is valid")
else:
    print("Configuration is invalid")
    print(f"Validation errors: {config.getValidationErrors()}")
```

**Solution 3: Check Schema**
```python
import pynlm

# Create configuration with schema
config = pynlm.Config::createBuilder()
    .add("neuron_count", pynlm.ConfigSchema("neuron_count", pynlm.ConfigSchema::Type::Integer, true))
        .withRange(100, 1000000)
        .withDefault(1000)
    .add("learning_rate", pynlm.ConfigSchema("learning_rate", pynlm.ConfigSchema::Type::Double, false))
        .withRange(0.0, 1.0)
        .withDefault(0.01)
    .add("enable_curiosity", pynlm.ConfigSchema("enable_curiosity", pynlm.ConfigSchema::Type::Boolean, false))
        .withDefault(true)
    .build()

brain = pynlm.createBrain(config)
brain.initialize()
```

**Solution 4: Check File I/O**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Save to file
try:
    config.saveToFile("config.json")
    print("Configuration saved successfully")
except Exception as e:
    print(f"Error saving configuration: {e}")

# Load from file
try:
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile("config.json")
    print("Configuration loaded successfully")
except Exception as e:
    print(f"Error loading configuration: {e}")
```

### 3. Brain Issues

#### Issue 4: Brain Initialization Errors

**Symptoms**:
- Brain fails to initialize
- Exception during initialization
- Brain in invalid state

**Possible Causes**:
1. Invalid configuration
2. Missing dependencies
3. Memory issues
4. System resource limitations

**Solutions**:

**Solution 1: Check Configuration**
```python
import pynlm

# Create brain with default configuration
try:
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("learning_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain initialized successfully")
    
except Exception as e:
    print(f"Brain initialization failed: {e}")
    print("Check configuration values")
```

**Solution 2: Check System Resources**
```python
import pynlm
import psutil
import os

# Check available memory
process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Available memory: {memory_info.available / 1024 / 1024:.2f} MB")

# Check CPU usage
print(f"CPU usage: {psutil.cpu_percent()}%")

# Create brain with smaller configuration
try:
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)  # Smaller brain
    config.set("learning_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain initialized with smaller configuration")
    
except Exception as e:
    print(f"Brain initialization failed: {e}")
```

**Solution 3: Use Simple Brain**
```python
import pynlm

# Create brain with default configuration
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain initialized with default configuration")
    
except Exception as e:
    print(f"Brain initialization failed: {e}")
    print("Try using default configuration")
```

**Solution 4: Check Brain State**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Check brain state
print(f"Brain created: {brain is not None}")
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Synapse count: {brain.getTotalSynapseCount()}")

# Initialize brain
try:
    brain.initialize()
    print("Brain initialized successfully")
    
except Exception as e:
    print(f"Brain initialization failed: {e}")
```

#### Issue 5: Simulation Errors

**Symptoms**:
- Simulation fails during execution
- Unexpected errors during step processing
- Brain in inconsistent state

**Possible Causes**:
1. Invalid step numbers
2. Configuration issues
3. Memory problems
4. Timing issues

**Solutions**:

**Solution 1: Check Step Processing**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with error checking
try:
    for step in range(100):
        brain.step(step)
        print(f"Step {step} completed")
        
except Exception as e:
    print(f"Error at step {step}: {e}")
    print("Check step number and brain state")
```

**Solution 2: Use Smaller Simulation**
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run smaller simulation
try:
    for step in range(10):
        brain.step(step)
        print(f"Step {step} completed")
        
    print("Simulation completed successfully")
    
except Exception as e:
    print(f"Simulation failed: {e}")
    print("Try with smaller simulation or check configuration")
```

**Solution 3: Check Brain State**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check brain state before simulation
print(f"Initial neuron count: {brain.getTotalNeuronCount()}")
print(f"Initial synapse count: {brain.getTotalSynapseCount()}")
print(f"Initial firing neurons: {brain.getFiringNeuronCount()}")

# Run simulation
for step in range(10):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### 4. Agent Issues

#### Issue 6: Agent Initialization Errors

**Symptoms**:
- Agent fails to initialize
- Exception during agent initialization
- Agent in invalid state

**Possible Causes**:
1. Invalid brain
2. Invalid world
3. Configuration issues
4. Missing dependencies

**Solutions**:

**Solution 1: Check Brain**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check brain state
print(f"Brain created: {brain is not None}")
print(f"Neuron count: {brain.getTotalNeuronCount()}")
```

**Solution 2: Check World**
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
print(f"World created: {world is not None}")
print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
```

**Solution 3: Check Agent**
```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)

# Create agent
try:
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("Agent initialized successfully")
    
except Exception as e:
    print(f"Agent initialization failed: {e}")
    print("Check brain and world state")
```

**Solution 4: Use Simple Agent**
```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)

# Create agent with default configuration
try:
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("Agent initialized with default configuration")
    
except Exception as e:
    print(f"Agent initialization failed: {e}")
    print("Try with default configuration")
```

### 5. World Issues

#### Issue 7: World Initialization Errors

**Symptoms**:
- World fails to initialize
- Exception during world initialization
- World in invalid state

**Possible Causes**:
1. Invalid dimensions
2. Configuration issues
3. Missing dependencies
4. System resource limitations

**Solutions**:

**Solution 1: Check World Dimensions**
```python
import pynlm

# Create world with default configuration
try:
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    print("World initialized successfully")
    
except Exception as e:
    print(f"World initialization failed: {e}")
    print("Check world dimensions")
```

**Solution 2: Use Smaller World**
```python
import pynlm

# Create world with smaller dimensions
try:
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)  # Smaller world
    world.reset()
    print("World initialized with smaller dimensions")
    
except Exception as e:
    print(f"World initialization failed: {e}")
    print("Try with smaller dimensions")
```

**Solution 3: Check World State**
```python
import pynlm

# Create world
try:
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    print(f"World created: {world is not None}")
    print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
    
    world.reset()
    print("World reset successfully")
    
except Exception as e:
    print(f"World initialization failed: {e}")
```

### 6. Memory Issues

#### Issue 8: Memory Usage Problems

**Symptoms**:
- High memory usage
- Memory leaks
- Out of memory errors

**Possible Causes**:
1. Large brain size
2. Inefficient memory usage
3. Memory leaks
4. System resource limitations

**Solutions**:

**Solution 1: Check Memory Usage**
```python
import pynlm
import psutil
import os

# Check available memory
process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Available memory: {memory_info.available / 1024 / 1024:.2f} MB")
print(f"Used memory: {memory_info.rss / 1024 / 1024:.2f} MB")

# Create brain with smaller configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Smaller brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

**Solution 2: Use Memory Optimization**
```python
import pynlm

# Create memory-optimized brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Smaller brain
config.set("brain.synapse_density", 0.05)  # Lower synapse density

brain = pynlm.createBrain(config)
brain.initialize()

print("Memory-optimized brain created")
```

**Solution 3: Check Memory Usage**
```python
import pynlm
import psutil
import os

# Check memory usage
process = psutil.Process(os.getpid())

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check memory before simulation
memory_before = process.memory_info().rss / 1024 / 1024  # MB
print(f"Memory before simulation: {memory_before:.2f} MB")

# Run simulation
for step in range(100):
    brain.step(step)

# Check memory after simulation
memory_after = process.memory_info().rss / 1024 / 1024  # MB
print(f"Memory after simulation: {memory_after:.2f} MB")

# Calculate memory increase
memory_increase = memory_after - memory_before
print(f"Memory increase: {memory_increase:.2f} MB")
```

### 7. Performance Issues

#### Issue 9: Performance Problems

**Symptoms**:
- Slow simulation
- Performance bottlenecks
- Resource contention

**Possible Causes**:
1. Large brain size
2. Inefficient algorithms
3. System resource limitations
4. Configuration issues

**Solutions**:

**Solution 1: Check Brain Size**
```python
import pynlm

# Create brain with smaller configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Smaller brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

**Solution 2: Use Batch Processing**
```python
import pynlm
from performance import BatchBrain

# Create batch brain
config = pynlm.createDefaultConfig()
batch_brain = BatchBrain(config)

# Configure batch processing
batch_brain.enableSIMD(True)
batch_brain.enableMultithreading(True)
batch_brain.enableMemoryPool(True)

# Process batch
batch_brain.processBatch(0, 999)

print("Batch processing completed")
```

**Solution 3: Check Performance**
```python
import pynlm
import time

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Time simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

# Calculate performance
steps_per_second = 1000 / (end_time - start_time)
print(f"Performance: {steps_per_second:.0f} steps/second")
```

### 8. Logging Issues

#### Issue 10: Logging Problems

**Symptoms**:
- No logs output
- Logging errors
- Log configuration issues

**Possible Causes**:
1. Logging not enabled
2. Configuration issues
3. File system issues
4. System resource limitations

**Solutions**:

**Solution 1: Check Logging Configuration**
```python
import pynlm

# Create brain with logging configuration
config = pynlm.createDefaultConfig()
config.set("logging.level", "INFO")
config.set("logging.file.enabled", True)
config.set("logging.file.path", "logs/nlm.log")

brain = pynlm.createBrain(config)
brain.initialize()

print("Brain created with logging configuration")
```

**Solution 2: Check Logger**
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check if logging is working
logger = brain.getLogger()
if logger:
    print("Logging is working")
    
    # Log message
    logger.log("INFO", "Test message")
else:
    print("Logging is not working")
```

**Solution 3: Use Simple Logging**
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Brain created with default logging configuration")
```

## Debugging Techniques

### 1. Debug with Print Statements

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add debug print statements
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing neurons: {brain.getFiringNeuronCount()}")

# Run simulation with debug prints
for step in range(10):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### 2. Debug with Logging

```python
import pynlm

# Create brain with logging configuration
config = pynlm.createDefaultConfig()
config.set("logging.level", "DEBUG")
config.set("logging.file.enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Log debug messages
logger = brain.getLogger()
if logger:
    logger.log("DEBUG", "Brain initialization completed")
    logger.log("INFO", "Starting simulation")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
        logger.log("DEBUG", f"Step {step} completed")
    
    logger.log("INFO", "Simulation completed")
```

### 3. Debug with Unit Tests

```python
import unittest
import pynlm

class DebugBrain(unittest.TestCase):
    """Debug brain functionality."""
    
    def setUp(self):
        """Set up test."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 100)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test."""
        del self.brain
    
    def testBrainInitialization(self):
        """Test brain initialization."""
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
    
    def testBrainStep(self):
        """Test brain step."""
        initial_firing = self.brain.getFiringNeuronCount()
        
        for step in range(10):
            self.brain.step(step)
        
        final_firing = self.brain.getFiringNeuronCount()
        self.assertGreaterEqual(final_firing, initial_firing)
```

### 4. Debug with Performance Profiling

```python
import pynlm
import time

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Profile performance
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

# Calculate performance
steps_per_second = 1000 / (end_time - start_time)
print(f"Performance: {steps_per_second:.0f} steps/second")

# Check memory usage
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")
```

### 5. Debug with System Information

```python
import pynlm
import psutil
import platform

# Print system information
print("=== System Information ===")
print(f"Platform: {platform.system()}")
print(f"Version: {platform.version()}")
print(f"Python: {platform.python_version()}")

# Print CPU information
print(f"CPU count: {psutil.cpu_count()}")
print(f"CPU usage: {psutil.cpu_percent()}%")

# Print memory information
memory_info = psutil.virtual_memory()
print(f"Total memory: {memory_info.total / 1024 / 1024:.2f} MB")
print(f"Available memory: {memory_info.available / 1024 / 1024:.2f} MB")

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

## Error Code Reference

### Brain Errors

| Error Code | Description | Solution |
|------------|-------------|----------|
| BRAIN_INIT_FAILED | Brain initialization failed | Check configuration |
| BRAIN_STEP_FAILED | Brain step processing failed | Check step number |
| BRAIN_MEMORY_ERROR | Brain memory error | Check memory usage |

### Configuration Errors

| Error Code | Description | Solution |
|------------|-------------|----------|
| CONFIG_VALIDATION_FAILED | Configuration validation failed | Check configuration values |
| CONFIG_FILE_ERROR | Configuration file error | Check file permissions |
| CONFIG_TYPE_ERROR | Configuration type error | Check type conversion |

### Agent Errors

| Error Code | Description | Solution |
|------------|-------------|----------|
| AGENT_INIT_FAILED | Agent initialization failed | Check brain and world |
| AGENT_STEP_FAILED | Agent step processing failed | Check agent state |
| AGENT_LEARNING_FAILED | Agent learning failed | Check learning configuration |

### World Errors

| Error Code | Description | Solution |
|------------|-------------|----------|
| WORLD_INIT_FAILED | World initialization failed | Check world configuration |
| WORLD_STEP_FAILED | World step processing failed | Check world state |
| WORLD_COLLISION_FAILED | World collision failed | Check collision detection |

### Performance Errors

| Error Code | Description | Solution |
|------------|-------------|----------|
| PERFORMANCE_TOO_SLOW | Performance too slow | Optimize brain size |
| PERFORMANCE_MEMORY_HIGH | Performance memory high | Check memory usage |
| PERFORMANCE_CPU_HIGH | Performance CPU high | Check CPU usage |

## Common Error Messages and Solutions

### Error: "Brain initialization failed"

**Solution**:
```python
import pynlm

# Create brain with default configuration
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
    
except Exception as e:
    print(f"Brain initialization failed: {e}")
    print("Check configuration values")
```

### Error: "Configuration validation failed"

**Solution**:
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("neuron_count", 1000)
config.set("learning_rate", 0.01)
config.set("enable_curiosity", True)

# Validate configuration
if config.validateAgainstSchema():
    print("Configuration is valid")
else:
    print("Configuration is invalid")
    print(f"Validation errors: {config.getValidationErrors()}")
```

### Error: "Memory usage too high"

**Solution**:
```python
import pynlm

# Create brain with memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Smaller brain
config.set("brain.synapse_density", 0.05)  # Lower synapse density

brain = pynlm.createBrain(config)
brain.initialize()

print("Memory-efficient brain created")
```

### Error: "Performance too slow"

**Solution**:
```python
import pynlm

# Create brain with performance optimizations
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable performance optimizations
brain.enableSIMD(True)
brain.enableMultithreading(True)
brain.enableMemoryPool(True)

# Process batch
brain.processBatch(0, 999)
```

## Conclusion

This troubleshooting guide provides comprehensive information for diagnosing and resolving issues with NLM applications. By following the techniques and solutions provided, you can effectively troubleshoot common issues and get your NLM applications working correctly.

Key points:
1. **Identify symptoms**: Understand the symptoms of each issue
2. **Check configuration**: Verify configuration values
3. **Check system resources**: Monitor system resource usage
4. **Use debugging techniques**: Use appropriate debugging techniques
5. **Apply solutions**: Apply the appropriate solutions
6. **Test changes**: Test changes after applying solutions

By following this troubleshooting guide, you can effectively diagnose and resolve issues with your NLM applications.
