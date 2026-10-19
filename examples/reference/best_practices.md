# Best Practices

This document provides comprehensive best practices for writing efficient and effective NLM (Neural Learning Machine) code. It covers coding standards, performance optimization, memory management, testing, documentation, and troubleshooting.

## Overview

Following best practices ensures that NLM code is:

- **Maintainable**: Easy to understand, modify, and extend
- **Performant**: Efficient in terms of time and memory usage
- **Reliable**: Robust, error-resistant, and well-tested
- **Documented**: Clear, comprehensive, and self-explanatory
- **Compatible**: Follows project conventions and standards

## Coding Standards

### 1. Python Style Guidelines

#### Code Formatting

Use PEP 8 style guidelines for Python code:

```python
# Good: Properly indented
def example_function():
    """Example function with proper docstring."""
    # Proper indentation
    for item in items:
        process_item(item)
        
    # Proper line length
    if len(long_variable_name) > some_threshold:
        print("Variable name is too long")
```

#### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Function | snake_case | `def get_brain_state():` | |
| Variable | snake_case | `brain_config = Config()` | |
| Class | PascalCase | `class NeuralNetwork:` | |
| Constant | UPPER_SNAKE_CASE | `MAX_NEURONS = 10000` | |

### 2. Code Organization

#### File Structure

Organize code into logical modules:

```
/workspace/app/
├── python/
│   ├── bindings.cpp          # C++ bindings
│   ├── CMakeLists.txt        # Build configuration
│   └── include/              # Header files
├── examples/                 # Example code
│   ├── quick_start/          # Quick start examples
│   ├── advanced/            # Advanced examples
│   ├── tutorials/           # Tutorial documentation
│   └── reference/           # Reference documentation
├── configs/                  # Configuration files
│   └── default.cfg          # Default configuration
├── docs/                     # Documentation
│   └── ***.md              # Documentation files
└── test_nlm_cli.py          # CLI test script
```

#### Module Organization

Organize Python code into modules:

```python
# /workspace/app/python/pynlm/
├── __init__.py              # Package initialization
├── brain.py                # Brain class
├── config.py               # Config class
├── memory.py               # Memory systems
├── neuromodulation.py       # Neuromodulation systems
├── planning.py             # Planning systems
├── development.py          # Development systems
└── world.py                # World systems
```

### 3. Error Handling

#### Exception Handling

Use appropriate exception handling:

```python
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigError(NLMError):
    """Configuration-related errors."""
    pass

class BrainError(NLMError):
    """Brain-related errors."""
    pass

def load_configuration(filename):
    """Load configuration from file."""
    try:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(filename)
        return config
    except IOError as e:
        raise ConfigError(f"Could not load configuration from {filename}: {e}")
    except Exception as e:
        raise NLMError(f"Unexpected error loading configuration: {e}")
```

#### Validation

Validate inputs and configurations:

```python
def validate_configuration(config):
    """Validate configuration parameters."""
    errors = []
    
    # Check required keys
    required_keys = ["neuron_count", "region_count"]
    for key in required_keys:
        if not config.has(key):
            errors.append(f"Missing required key: {key}")
    
    # Check value ranges
    if config.has("neuron_count"):
        neuron_count = config.get("neuron_count")
        if neuron_count < 1 or neuron_count > 100000:
            errors.append("neuron_count must be between 1 and 100000")
    
    if errors:
        raise ConfigError("Configuration validation failed:\n" + "\n".join(errors))
```

### 4. Documentation

#### Docstrings

Use comprehensive docstrings:

```python
def example_function(param1, param2):
    """
    This function does something useful.
    
    Args:
        param1 (int): The first parameter.
        param2 (str): The second parameter.
    
    Returns:
        dict: A dictionary with the results.
    
    Raises:
        ValueError: If param1 is negative.
    
    Example:
        >>> example_function(1, "test")
        {'result': 42}
    """
    # Implementation
```

#### Comments

Use comments sparingly and meaningfully:

```python
# Good: Explain why something is done a certain way
def optimize_memory_usage(data):
    # Use memory-efficient data structures for large datasets
    if len(data) > 1000000:
        return use_compressed_data_structure(data)
    return data

# Avoid: Obvious comments
# This function processes items
# Should be: Explain the complex logic
```

## Performance Optimization

### 1. Memory Management

#### Efficient Data Structures

Use appropriate data structures:

```python
# Good: Use efficient data structures
class EfficientBrain:
    def __init__(self, neuron_count):
        # Use lists instead of sets for large collections
        self.neurons = [0.0] * neuron_count
        self.synapses = []
        
    def process_neurons(self):
        # Use vectorized operations where possible
        return [neuron * 2 for neuron in self.neurons]
```

#### Memory Efficiency

Manage memory carefully:

```python
class MemoryEfficientBrain:
    def __init__(self, config):
        # Allocate memory only when needed
        self.neurons = None
        self.synapses = None
        self.config = config
        
    def initialize(self):
        """Initialize brain only when needed."""
        if self.neurons is None:
            self.neurons = [0.0] * self.config.get("neuron_count")
            self.synapses = self.create_synapses()
            
    def create_synapses(self):
        """Create synapses efficiently."""
        # Use efficient synapse creation
        neuron_count = self.config.get("neuron_count")
        return [(i, (i + 1) % neuron_count) for i in range(neuron_count)]
```

### 2. Computational Efficiency

#### Optimize Loops

Optimize loops for better performance:

```python
# Good: Use vectorized operations
import numpy as np

class VectorizedBrain:
    def __init__(self, config):
        self.config = config
        
    def update_neurons(self):
        # Use vectorized operations
        neurons = np.array(self.neurons)
        return neurons * 2.0
        
    def process_sensory_input(self, input_data):
        # Use efficient sensory processing
        input_array = np.array(input_data)
        return np.dot(input_array, self.weights)
```

#### Avoid Redundant Computations

Cache results to avoid redundant computations:

```python
class CachedBrain:
    def __init__(self, config):
        self.config = config
        self._cached_computations = {}
        
    def compute_something(self, input_data):
        # Check cache first
        cache_key = hash(str(input_data))
        if cache_key in self._cached_computations:
            return self._cached_computations[cache_key]
            
        # Compute and cache result
        result = self._compute_input(input_data)
        self._cached_computations[cache_key] = result
        return result
```

### 3. Concurrency

#### Parallel Processing

Use parallel processing for large computations:

```python
import concurrent.futures

class ParallelBrain:
    def __init__(self, config):
        self.config = config
        
    def process_neurons_parallel(self, neurons):
        # Use parallel processing for large neuron sets
        with concurrent.futures.ThreadPoolExecutor() as executor:
            results = executor.map(self.process_neuron, neurons)
        return list(results)
        
    def process_neuron(self, neuron):
        # Process single neuron
        return neuron * 2.0
```

## Memory Management

### 1. Memory Allocation

Allocate memory efficiently:

```python
class MemoryManagedBrain:
    def __init__(self, config):
        # Allocate memory with bounds checking
        neuron_count = config.get("neuron_count")
        if neuron_count > 1000000:
            raise ValueError("Neuron count too large for available memory")
            
        # Use memory pools where possible
        self.neuron_pool = self.create_neuron_pool(neuron_count)
        
    def create_neuron_pool(self, size):
        """Create memory pool for neurons."""
        return [0.0] * size
```

### 2. Memory Cleanup

Clean up memory properly:

```python
class CleanableBrain:
    def __init__(self):
        self.neurons = None
        self.synapses = None
        
    def cleanup(self):
        """Clean up allocated memory."""
        self.neurons = None
        self.synapses = None
        
    def __del__(self):
        """Destructor for cleanup."""
        self.cleanup()
```

### 3. Memory Monitoring

Monitor memory usage:

```python
class MemoryMonitoredBrain:
    def __init__(self, config):
        self.config = config
        import psutil
        self.process = psutil.Process()
        
    def check_memory_usage(self):
        """Check current memory usage."""
        memory_info = self.process.memory_info()
        return memory_info.rss / (1024 * 1024)  # MB
        
    def warn_if_memory_high(self):
        """Warn if memory usage is too high."""
        memory_mb = self.check_memory_usage()
        if memory_mb > 1000:  # 1GB
            print(f"Warning: High memory usage: {memory_mb:.2f} MB")
```

## Testing

### 1. Unit Testing

Write comprehensive unit tests:

```python
import unittest
import pynlm

class TestBrain(unittest.TestCase):
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 100)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
    def tearDown(self):
        """Clean up test environment."""
        if hasattr(self, 'brain'):
            del self.brain
            
    def test_brain_initialization(self):
        """Test brain initialization."""
        self.assertIsNotNone(self.brain)
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
        
    def test_brain_step(self):
        """Test brain step functionality."""
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(1)
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), initial_spikes)
        
    def test_configuration(self):
        """Test configuration."""
        self.assertTrue(self.config.has("neuron_count"))
        self.assertEqual(self.config.get("neuron_count"), 100)
```

### 2. Integration Testing

Test system integration:

```python
class TestBrainIntegration(unittest.TestCase):
    def test_brain_world_integration(self):
        """Test integration between brain and world."""
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 10, 10)
        world.reset()
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        # Verify integration
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        
    def test_memory_systems_integration(self):
        """Test integration of memory systems."""
        # Create brain with memory systems
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        # Test working memory
        neuron_id = pynlm.NeuronId(1)
        working_memory.store(neuron_id, 0.8)
        value = working_memory.recall(neuron_id)
        self.assertEqual(value, 0.8)
        
        # Test episodic memory
        episode = pynlm.EpisodicMemoryItem(
            timestamp=1,
            neurons=[neuron_id],
            values=[0.9],
            metadata="Test episode"
        )
        episodic_memory.storeEpisode(episode)
        self.assertEqual(episodic_memory.getEpisodeCount(), 1)
        
        # Test associative memory
        associative_memory.addAssociation(neuron_id, pynlm.NeuronId(2), 0.7)
        associations = associative_memory.getAssociations(neuron_id)
        self.assertIn(pynlm.NeuronId(2), associations)
```

### 3. Performance Testing

Test performance characteristics:

```python
import time

class TestBrainPerformance(unittest.TestCase):
    def test_initialization_performance(self):
        """Test brain initialization performance."""
        start_time = time.time()
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        end_time = time.time()
        initialization_time = end_time - start_time
        
        # Initialization should be fast (< 1 second for 1000 neurons)
        self.assertLess(initialization_time, 1.0, 
                       f"Initialization took too long: {initialization_time:.2f}s")
        
    def test_step_performance(self):
        """Test brain step performance."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure step performance
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        step_time = (end_time - start_time) / 100
        print(f"Average step time: {step_time:.4f}s")
        
        # Each step should be fast (< 0.01 seconds for 500 neurons)
        self.assertLess(step_time, 0.01, 
                       f"Step too slow: {step_time:.4f}s")
```

### 4. Stress Testing

Test under stress conditions:

```python
class TestBrainStress(unittest.TestCase):
    def test_large_brain(self):
        """Test with large brain configuration."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 10000)  # Large brain
        
        # Should handle large brains
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should not crash
        self.assertIsNotNone(brain)
        self.assertGreaterEqual(brain.getTotalNeuronCount(), 10000)
        
    def test_memory_usage(self):
        """Test memory usage with large simulation."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 2000)
        
        # Measure memory usage
        import psutil
        process = psutil.Process()
        initial_memory = process.memory_info().rss
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(1000):
            brain.step(step)
            
        final_memory = process.memory_info().rss
        memory_increase = (final_memory - initial_memory) / (1024 * 1024)  # MB
        
        # Memory increase should be reasonable (< 500MB for 2000 neurons)
        self.assertLess(memory_increase, 500,
                       f"Memory increase too high: {memory_increase:.2f}MB")
```

## Documentation

### 1. Code Documentation

Maintain comprehensive documentation:

```python
def example_function():
    """
    This function does something useful.
    
    This function processes input data and returns results. It handles
    various edge cases and provides detailed error messages.
    
    Returns:
        dict: A dictionary containing the processed results.
            The dictionary includes:
            - 'result': The main computation result
            - 'metadata': Additional information about the computation
            - 'errors': Any errors encountered during processing
            
    Raises:
        ValueError: If the input data is invalid.
        IOError: If there are issues with file I/O.
        
    Examples:
        >>> example_function([1, 2, 3])
        {'result': 6, 'metadata': {'count': 3}, 'errors': []}
        
        >>> example_function([])
        {'result': 0, 'metadata': {'count': 0}, 'errors': ['Empty input']}
        
    Note:
        This function is designed to handle large datasets efficiently
        using vectorized operations. For small datasets, consider using
        the simpler alternative function.
    """
    # Implementation
```

### 2. API Documentation

Document public API:

```python
"""
NLM (Neural Learning Machine) Python API

This module provides Python bindings to the NLM neural simulation framework.
It exposes all advanced systems from Phase 6 of the NLM project.

Main Classes:
    - Config: Configuration management
    - Brain: Neural simulation brain
    - WorkingMemory: Transient memory storage
    - EpisodicMemory: Episodic memory system
    - AssociativeMemory: Associative memory system
    - Neuromodulator: Neuromodulation base class
    - Dopamine: Reward neuromodulator
    - Novelty: Novelty detection
    - Curiosity: Curiosity system
    - PredictionError: Prediction error system
    - PredictionSystem: Prediction system
    - NeuralPlanner: Action planning system
    - ConceptFormation: Concept formation system
    - AttentionalSelection: Attentional selection system
    - DevelopmentSystem: Neural development system

Factory Functions:
    - createBrain(config): Create brain with configuration
    - createSimpleWorld(): Create simple world
    - createAgentBrain(brain): Create agent brain interface
    - createDopamine(): Create dopamine neuromodulator
    - createNovelty(): Create novelty detection system
    - createCuriosity(): Create curiosity system
    - createPredictionError(): Create prediction error system
    - createPredictionSystem(): Create prediction system
    - createNeuralPlanner(): Create neural planner
    - createConceptFormation(): Create concept formation system
    - createAttentionalSelection(): Create attentional selection system
    - createDefaultConfig(): Create default configuration

Usage Examples:
    >>> import pynlm
    >>> 
    >>> # Create brain
    >>> config = pynlm.createDefaultConfig()
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> 
    >>> # Create world
    >>> world = pynlm.createSimpleWorld()
    >>> world.configure(20, 20, 12, 12)
    >>> world.reset()
    >>> 
    >>> # Create agent
    >>> agent = pynlm.createAgentBrain(brain)
    >>> agent.initialize(world)
    >>> 
    >>> # Run simulation
    >>> for step in range(100):
    ...     world.update(0.1)
    ...     percept = world.getSensoryPercept()
    ...     agent.processSensoryInput(percept)
    ...     brain.step(step)
    ...     action = agent.decodeMotorCommand()
    ...     world.applyMotorCommand(action, world.getSimulationTime())
    >>>

Note:
    This API is designed to be intuitive and Pythonic while providing
    access to all the advanced features of the NLM framework.
"""
```

### 3. Tutorial Documentation

Provide tutorial documentation:

```markdown
# Tutorial: Getting Started with NLM

## Introduction

This tutorial provides a comprehensive introduction to the NLM (Neural Learning Machine) Python API.

## What You'll Learn

- Create and initialize an NLM brain
- Set up a simple simulation environment
- Understand the agent-brain interface
- Run your first neural simulations
- Save and load brain states

## Prerequisites

- Python 3.6 or higher
- NLM Python bindings installed (`pip install pynlm`)

## Quick Start

1. **Create a brain with default configuration:**

   ```python
   import pynlm
   
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   ```

2. **Create and configure the world:**

   ```python
   world = pynlm.createSimpleWorld()
   world.configure(20, 20, 12, 12)
   world.reset()
   ```

3. **Create and initialize the agent:**

   ```python
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   ```

4. **Run the simulation:**

   ```python
   for step in range(100):
       world.update(0.1)
       percept = world.getSensoryPercept()
       agent.processSensoryInput(percept)
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())
   ```

## Conclusion

You now have a working NLM simulation!

## Next Steps

Explore the advanced examples and tutorials to learn more about NLM features.
```

## Troubleshooting

### Common Issues and Solutions

#### Problem 1: Import Errors
**Issue:** Cannot import pynlm module.
**Solution:** Ensure NLM Python bindings are installed correctly.

```bash
pip install pynlm
```

#### Problem 2: Brain Initialization Fails
**Issue:** Brain initialization fails with error.
**Solution:** Check configuration and ensure all required parameters are set.

```python
# Check configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Set reasonable neuron count
brain = pynlm.createBrain(config)
brain.initialize()  # Should succeed
```

#### Problem 3: Simulation Runs Slow
**Issue:** Simulation is too slow.
**Solution:** Reduce neuron count or world size.

```python
# Use smaller configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Reduced
config.set("environment_width", 10)  # Reduced
config.set("environment_height", 10)  # Reduced
```

#### Problem 4: Memory Usage Too High
**Issue:** Memory usage is too high.
**Solution:** Use more memory-efficient data structures or reduce simulation size.

```python
# Use memory-efficient implementation
import numpy as np

class MemoryEfficientBrain:
    def __init__(self, config):
        # Use numpy arrays for efficiency
        neuron_count = config.get("neuron_count")
        self.neurons = np.zeros(neuron_count, dtype=np.float32)
```

#### Problem 5: Configuration Validation Fails
**Issue:** Configuration validation fails.
**Solution:** Enable schema validation and fix configuration errors.

```python
# Enable schema validation
config = pynlm.createDefaultConfig()
config.setValidationEnabled(True)

# Validate configuration
validation_errors = config.validateConfiguration()
if validation_errors:
    print("Configuration errors:", validation_errors)
    # Fix errors and validate again
```

### Error Messages

#### "Brain not found"
**Description:** Brain not found in file.
**Solution:** Ensure brain file exists or create new brain.

```python
# Check if file exists
import os
if os.path.exists("brain_file.nld"):
    brain.load("brain_file.nld")
else:
    print("Brain file not found, creating new brain")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
```

#### "Configuration error"
**Description:** Configuration error occurred.
**Solution:** Check configuration parameters and fix errors.

```python
# Check configuration parameters
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 2)

# Validate configuration
if config.has("neuron_count"):
    neuron_count = config.get("neuron_count")
    if neuron_count < 1:
        print("Error: neuron_count must be positive")
```

#### "Simulation error"
**Description:** Simulation error occurred.
**Solution:** Check simulation parameters and fix errors.

```python
# Check world configuration
world = pynlm.createSimpleWorld()
world.configure(20, 20, 12, 12)  # Valid configuration

# Check agent initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Should succeed
```

### Debugging Tips

#### Enable Debug Logging

```python
import pynlm

# Enable debug logging
config = pynlm.createDefaultConfig()
config.set("log_level", "DEBUG")
config.set("log_to_file", True)
config.set("log_filename", "nlm_debug.log")

brain = pynlm.createBrain(config)
brain.initialize()

# Enable brain logging
brain.logStatus()
```

#### Memory Debugging

```python
import pynlm
import psutil
import os

# Monitor memory usage
process = psutil.Process(os.getpid())

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check memory usage
memory_info = process.memory_info()
print(f"Memory usage: {memory_info.rss / (1024 * 1024):.2f} MB")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        memory_info = process.memory_info()
        print(f"Step {step}: {memory_info.rss / (1024 * 1024):.2f} MB")
```

#### Performance Debugging

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
brain = pynlm.createBrain(config)
brain.initialize()

# Measure performance
start_time = time.time()
for step in range(100):
    brain.step(step)
end_time = time.time()

print(f"Total time for 100 steps: {end_time - start_time:.2f} seconds")
print(f"Average time per step: {(end_time - start_time) / 100:.4f} seconds")
```

## Project Guidelines

### Code Quality Standards

1. **Follow PEP 8** for Python code style.
2. **Use meaningful variable names** that describe their purpose.
3. **Write comprehensive docstrings** for all public functions.
4. **Use type hints** for better code documentation.
5. **Avoid code duplication** through refactoring.

### Development Workflow

1. **Test first**: Write tests before implementing functionality.
2. **Iterative development**: Develop and test incrementally.
3. **Code review**: Review code for quality and consistency.
4. **Documentation**: Maintain up-to-date documentation.
5. **Performance optimization**: Profile and optimize performance.

### Version Control

1. **Use git for version control**.
2. **Write meaningful commit messages**.
3. **Create pull requests** for code changes.
4. **Use feature branches** for new development.
5. **Merge changes** regularly.

## Conclusion

Following these best practices ensures that NLM code is:

- **Maintainable**: Easy to understand and modify
- **Performant**: Efficient in terms of time and memory usage
- **Reliable**: Robust and well-tested
- **Documented**: Clear and comprehensive
- **Compatible**: Follows project standards and conventions

These practices will help you write high-quality NLM code that is easy to maintain, efficient, and reliable.

---

*Best Practices document provides comprehensive guidelines for writing efficient and effective NLM code.*