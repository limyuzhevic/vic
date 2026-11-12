# NLM Project Documentation
# Neural Learning Machine - Advanced Features Guide

## Overview

NLM (Neural Learning Machine) is an experimental computational brain project that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

This document covers the advanced features and improvements made to the NLM project, including enhanced commands, improved Python bindings, new experiment types, and comprehensive testing.

## Table of Contents

1. [Enhanced Command Line Interface](#enhanced-command-line-interface)
2. [Advanced Experiment Types](#advanced-experiment-types)
3. [Improved Python Bindings](#improved-python-bindings)
4. [Advanced Testing Suite](#advanced-testing-suite)
5. [Configuration Management](#configuration-management)
6. [Performance Optimization](#performance-optimization)
7. [Python Integration Guide](#python-integration-guide)
8. [Best Practices](#best-practices)

## Enhanced Command Line Interface

### Advanced Command Line Options

The NLM project now supports a comprehensive set of advanced command line options:

#### Core Options
```
--config=file.cfg          Load configuration from JSON file
--neurons=N               Set number of neurons (default: 500)
--regions=M                Set number of brain regions (default: 1)
--connection-prob=P        Set connection probability (default: 0.15)
--seed=N                   Set random seed (default: 42)
--steps=N                  Run N simulation steps (default: 1000)
--help                     Show this help message
```

#### Advanced STDP Options
```
--stdp-ltp=W               Set STDP LTP weight (default: 0.02)
--stdp-ltd=W               Set STDP LTD weight (default: 0.015)
--stdp-tau=W               Set STDP time constant (default: 20.0)
```

#### System Control Options
```
--enable-replay            Enable episodic memory replay
--enable-development       Enable developmental processes
--enable-curiosity         Enable curiosity-driven exploration
--verbose                  Enable verbose output
--quiet                    Suppress non-essential output
--debug-level=L            Set debug level (OFF/ERROR/WARN/INFO/DEBUG)
```

#### Advanced Experiment Types
```
--experiment-type=all      Run all experiments (default)
--experiment-type=connectivity    Run basic connectivity experiment
--experiment-type=plasticity      Run plasticity learning experiment
--experiment-type=stdp            Run STDP verification experiment
--experiment-type=stress_test     Run system stress test
--experiment-type=learning_curve  Run learning curve analysis
--experiment-type=stability_test  Run stability test
--experiment-type=plasticity_map  Run plasticity mapping experiment
```

#### Checkpoint and Save Options
```
--checkpoint=path          Save checkpoint to specified path
--save-interval=N          Save checkpoint every N steps
--metrics-file=FILE        Save metrics to file
--checkpoint-compare       Compare checkpoints across runs
```

### Usage Examples

#### Running Basic Experiments
```bash
# Run all experiments with default settings
./nlm

# Run basic connectivity experiment
./nlm --experiment-type=connectivity

# Run plasticity learning experiment
./nlm --experiment-type=plasticity --neurons=1000 --steps=2000
```

#### Running Advanced Experiments
```bash
# Run system stress test
./nlm --experiment-type=stress_test --neurons=2000 --steps=5000

# Run learning curve analysis
./nlm --experiment-type=learning_curve --steps=10000 --enable-development

# Run stability test with perturbations
./nlm --experiment-type=stability_test --enable-curiosity

# Run plasticity mapping with custom configuration
./nlm --experiment-type=plasticity_map --stdp-ltp=0.03 --stdp-ltd=0.018
```

#### Checkpoint and Advanced Features
```bash
# Run experiment with checkpointing
./nlm --checkpoint=./my_checkpoint.bin --save-interval=1000

# Run with verbose output and metrics logging
./nlm --verbose --metrics-file=metrics.csv

# Compare checkpoints across runs
./nlm --checkpoint-compare
```

## Advanced Experiment Types

### 1. Stress Test (`--experiment-type=stress_test`)

Tests system performance under high load conditions by:

- Enabling all plasticity types (STDP, Hebbian, structural)
- Injecting maximum current into multiple neurons
- Running extended simulation periods
- Monitoring system stability and firing rate limits

**Use Case:** System stability testing and performance optimization

### 2. Learning Curve Analysis (`--experiment-type=learning_curve`)

Tracks learning progress over extended simulation with:

- Controlled input pattern stimulation
- Continuous recording of synaptic weights
- Progress monitoring at regular intervals
- Analysis of weight changes over time

**Use Case:** Understanding learning dynamics and convergence

### 3. Stability Test (`--experiment-type=stability_test`)

Evaluates system stability with random perturbations:

- Applies random current perturbations at intervals
- Tracks firing rate variation over time
- Calculates stability coefficients
- Detects system instability

**Use Case:** Robustness testing and fault tolerance

### 4. Plasticity Mapping (`--experiment-type=plasticity_map`)

Maps synaptic plasticity across the entire network:

- Records initial synaptic weights
- Applies controlled learning patterns
- Maps weight changes across all synapses
- Analyzes plasticity distribution

**Use Case:** Understanding synaptic plasticity patterns

## Improved Python Bindings

### Enhanced Python API Features

The Python bindings now provide a more comprehensive and user-friendly interface:

#### Factory Functions
```python
# Create components with sensible defaults
from nlm import create_default_config
from nlm import create_brain
from nlm import create_simple_world
from nlm import create_agent_brain

config = create_default_config()
brain = create_brain(config)
world = create_simple_world()
agent = create_agent_brain(brain)
```

#### Enhanced AgentBrain Methods
```python
# Access system status as dictionary
status = agent.get_system_status()
print(status["reward_modulation"])  # True/False
print(status["curiosity"])         # True/False

# Get advanced metrics
metrics = agent.get_advanced_metrics()
print(f"Neuromodulation level: {metrics['neuromodulation_level']}")
print(f"Curiosity level: {metrics['curiosity_level']}")
print(f"Prediction error: {metrics['prediction_error']}")
```

#### Enhanced World Methods
```python
# Create objects easily
world.create_resource(5.0, 5.0)    # Creates resource at (5, 5)
world.create_hazard(15.0, 15.0)   # Creates hazard at (15, 15)

# Query objects efficiently
resources = world.get_nearby_resources(5.0, 5.0, 2.0)
hazards = world.get_nearby_hazards(15.0, 15.0, 2.0)

print(f"Resources nearby: {len(resources)}")
print(f"Hazards nearby: {len(hazards)}")
```

#### Performance Benchmarking
```python
from nlm import benchmark_performance

# Run performance benchmark
benchmark_performance(duration_seconds=60)

# Example output:
# Benchmark completed:
#   Steps per second: 9.8
#   Total spikes: 100000
#   Average firing rate: 25.5 Hz
```

#### Model Export and Import
```python
from nlm import export_model, import_model

# Save trained brain model
export_model("/path/to/model.bin", brain)

# Load saved model
imported_brain = import_model("/path/to/model.bin")
```

### Advanced Experiment Demos

#### Phase 3 Demo
```python
from nlm import run_phase3_demo

# Run the complete Phase 3 demonstration
run_phase3_demo()

# This runs the full world interaction simulation including:
# - Sensory processing
# - Motor action
# - Reward modulation
# - Development updates
# - Curiosity-driven exploration
```

#### Phase 6 Integration Test
```python
from nlm import run_phase6_integration_test

# Run Phase 6 integration setup test
run_phase6_integration_test()

# This tests the integrated brain systems:
# - Memory systems
# - Neuromodulation
# - Prediction systems
# - Development processes
# - Replay capabilities
```

## Advanced Testing Suite

### Comprehensive Test Coverage

The NLM project now includes a comprehensive test suite that covers:

#### Core Tests (Phase 2)
- `test_types.cpp` - Core type tests
- `test_config.cpp` - Configuration system tests
- `test_random.cpp` - Random number generator tests
- `test_clock.cpp` - Simulation clock tests
- `test_neuron.cpp` - LIF neuron tests
- `test_synapse.cpp` - Synapse tests
- `test_stdp.cpp` - STDP plasticity tests
- `test_brain.cpp` - Brain integration tests

#### Advanced Tests (New)
- `test_advanced.cpp` - Advanced feature tests
  - Advanced command line parsing
  - Enhanced Python bindings
  - Advanced experiment types
  - Checkpoint functionality
  - Batch processing

### Test Execution

Run all tests using the test executable:
```bash
./nlm_test
```

Run advanced tests specifically:
```bash
./nlm_advanced_test
```

### Test Coverage Details

**Advanced Test Coverage:**
- ✅ Advanced command line argument parsing
- ✅ Stress test implementation
- ✅ Learning curve analysis
- ✅ Stability perturbation testing
- ✅ Plasticity mapping
- ✅ Enhanced Python bindings validation
- ✅ System status monitoring
- ✅ Advanced metrics collection
- ✅ Checkpoint save/load functionality
- ✅ Batch processing validation

**Quality Assurance:**
- Comprehensive error handling
- Performance benchmarking
- System stability testing
- Backward compatibility validation
- Integration testing

## Configuration Management

### Configuration File Structure

The NLM configuration system supports multiple configuration sources:

#### Configuration Sources
1. **Default Configuration**: Built-in defaults for all parameters
2. **File Configuration**: JSON configuration files loaded from disk
3. **Command Line Configuration**: Override parameters via command line
4. **Runtime Configuration**: Dynamic settings during runtime

#### Configuration Parameters
```bash
# Example configuration file
configs/default.cfg

# Basic settings
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Structural plasticity
Synaptogenesis_rate = 0.001
Pruning_rate = 0.0001

# Integration settings
replay_interval = 100
consolidation_interval = 1000

# Neuromodulation
dopamine_baseline = 0.1
curiosity_baseline = 0.05
```

### Configuration Loading Order

1. Load defaults from internal constants
2. Override with file configuration (if specified)
3. Override with command line arguments (if specified)
4. Apply runtime configuration changes

### Configuration Validation

The configuration system validates:

- Parameter ranges
- Consistency between parameters
- Resource constraints
- Logical dependencies

## Performance Optimization

### Performance Enhancements

#### 1. Memory Management
- Optimized memory pools for neuron and synapse storage
- Efficient data structures for fast access
- Automatic garbage collection for Python bindings
- Configurable memory limits

#### 2. Computational Efficiency
- Optimized spike propagation algorithms
- Parallel processing capabilities
- SIMD vectorization support
- Efficient cache utilization

#### 3. Simulation Speed
- Event-driven simulation for sparse activity
- Adaptive timestep algorithms
- Lazy evaluation of unused components
- Hardware acceleration where available

### Performance Monitoring

#### Metrics Collection
```python
# Real-time performance monitoring
from nlm import benchmark_performance

# Custom performance monitoring
def monitor_performance(duration_seconds):
    start_time = time.time()
    # Run simulation
    end_time = time.time()
    
    metrics = {
        "total_steps": total_steps,
        "steps_per_second": total_steps / duration_seconds,
        "memory_usage": get_memory_usage(),
        "cpu_usage": get_cpu_usage(),
        "spike_rate": brain.getTotalSpikeCount() / duration_seconds
    }
    
    return metrics
```

### Scaling Guidelines

#### Single Machine Scaling
- Up to 10,000 neurons per instance
- Multiple regions for modular design
- Efficient load balancing
- Fault tolerance mechanisms

#### Cluster Computing
- Distributed simulation across multiple machines
- Synchronous checkpoint mechanisms
- Load balancing algorithms
- Recovery and failover capabilities

## Python Integration Guide

### Installation

#### Python Environment Setup
```bash
# Install NLM Python package
pip install nlm-python

# Or install from source
pip install ./python
```

#### Development Dependencies
```bash
# For development, install additional dependencies
pip install numpy scipy matplotlib pybind11
```

### Basic Usage

#### Getting Started
```python
#!/usr/bin/env python3
"""
Basic NLM usage example
"""
from nlm import create_default_config, create_brain, create_simple_world, create_agent_brain

def main():
    print("=== NLM Python Example ===")
    
    # Create components
    config = create_default_config()
    brain = create_brain(config)
    world = create_simple_world()
    agent = create_agent_brain(brain)
    
    # Initialize components
    brain.initialize()
    agent.initialize(world)
    
    # Configure agent systems
    agent.enableRewardModulation(true)
    agent.enableCuriosity(true)
    
    # Create some world objects
    world.create_resource(5.0, 5.0)
    world.create_hazard(15.0, 15.0)
    
    # Run simulation steps
    for step in range(100):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain simulation
        brain.step(step, step * 0.001)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        result = world.applyMotorCommand(action, step * 0.1)
        
        # Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update world
        world.update(0.1)
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"Step {step}: Reward = {result.reward}, "
                  f"Firing rate = {brain.getAverageFiringRate():.2f}")
    
    print("=== Simulation Complete ===")
    print(f"Final reward: {brain.getAverageFiringRate():.2f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

if __name__ == "__main__":
    main()
```

### Advanced Python Usage

#### Machine Learning Integration
```python
from nlm import create_brain, benchmark_performance

# Create training brain
brain = create_brain(create_default_config())
brain.initialize()

# Run training session
benchmark_performance(duration_seconds=30)

# Save trained model
brain.save("trained_model.bin")

# Load for inference
loaded_brain = create_brain(create_default_config())
loaded_brain.initialize()
loaded_brain.load("trained_model.bin")
```

#### Custom Agent Brain
```python
from nlm import create_brain, create_simple_world, create_agent_brain

class CustomAgentBrain:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = create_agent_brain(brain)
        self.agent.initialize(world)
        self.agent.enable_all_systems()
    
    def run_simulation(self, steps):
        for step in range(steps):
            # Custom decision logic
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Custom action selection based on brain state
            action = self.decode_custom_action()
            
            # Apply to world
            result = self.world.applyMotorCommand(action, step * 0.1)
            self.agent.applyRewardModulation(result.reward, 0.0)
            
            self.world.update(0.1)
            self.agent.updateDevelopment(0.1)
    
    def decode_custom_action(self):
        # Custom action selection logic
        # This could use machine learning models,
        # rule-based systems, or learned policies
        return self.agent.decodeMotorCommand()

# Use custom agent
brain = create_brain(create_default_config())
brain.initialize()
world = create_simple_world()
agent = CustomAgentBrain(brain, world)

agent.run_simulation(200)
```

### Error Handling and Debugging

#### Error Handling
```python
from nlm import create_brain

# Error handling with try-catch
import traceback

try:
    brain = create_brain(create_default_config())
    brain.initialize()
    
    # Simulation code here
    for step in range(100):
        brain.step(step, step * 0.001)
        
except Exception as e:
    print(f"Error occurred: {e}")
    traceback.print_exc()
    
    # Error recovery
    print("Attempting error recovery...")
    # Reset and retry
    brain = create_brain(create_default_config())
    brain.initialize()
```

#### Debugging Tools
```python
# Performance debugging
import time

start_time = time.time()
brain = create_brain(create_default_config())

# Debug mode information
import sys
if '--debug' in sys.argv:
    print("Debug mode enabled")
    # Additional debug output
    # Memory usage tracking
    # Performance profiling
    # Error logging

# Profiling
import cProfile
if '--profile' in sys.argv:
    profiler = cProfile.Profile()
    profiler.enable()
    # Run simulation
    profiler.disable()
    profiler.print_stats()
```

## Best Practices

### Development Best Practices

#### 1. Configuration Management
```python
# Use configuration files for different environments
config = create_default_config()

# Override with environment variables or command line
# Example: Set from command line argument
if '--production' in sys.argv:
    config.set('neuron_count', 10000)
```

#### 2. Memory Management
```python
# Monitor and manage memory usage
import psutil
import gc

def monitor_memory():
    process = psutil.Process()
    memory_info = process.memory_info()
    return memory_info.rss / 1024 / 1024  # MB

def safe_cleanup():
    # Force garbage collection
    gc.collect()
    
    # Clear large data structures
    if 'large_data' in globals():
        del large_data
```

#### 3. Error Handling and Recovery
```python
# Implement robust error handling
class NLMErrorHandler:
    def __init__(self):
        self.error_count = 0
        self.max_retries = 3
        self.recovery_strategies = []
    
    def execute_with_retry(self, func, *args, **kwargs):
        for attempt in range(self.max_retries):
            try:
                return func(*args, **kwargs)
            except Exception as e:
                self.error_count += 1
                print(f"Attempt {attempt + 1} failed: {e}")
                
                if attempt < self.max_retries - 1:
                    # Implement recovery strategy
                    self.recover_from_error(e)
                else:
                    raise
    
    def recover_from_error(self, error):
        # Implement recovery logic
        # Reset components
        # Reinitialize systems
        # Log error for analysis
        pass
```

### Performance Best Practices

#### 1. Batch Processing
```python
# Process in batches for better performance
batch_size = 100
results = []

for i in range(0, total_steps, batch_size):
    # Process batch
    batch_result = process_batch(i, batch_size)
    results.append(batch_result)
    
    # Save intermediate results
    if i % save_interval == 0:
        save_checkpoint(results)
```

#### 2. Resource Management
```python
# Use context managers for resource management
class NLMResourceManager:
    def __enter__(self):
        # Initialize resources
        self.config = create_default_config()
        self.brain = create_brain(self.config)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup resources
        if hasattr(self, 'brain'):
            # Save final state
            # Cleanup memory
            pass
```

### Testing Best Practices

#### 1. Comprehensive Test Coverage
```python
# Write comprehensive unit tests
test_config = {
    'neuron_count': 100,
    'enable_reward_modulation': True,
    'enable_curiosity': True
}

# Test edge cases
def test_edge_cases():
    # Test boundary conditions
    # Test error conditions
    # Test invalid inputs
    pass

# Test performance benchmarks
def test_performance():
    # Benchmark different configurations
    # Measure processing speed
    # Validate memory usage
    pass
```

#### 2. Integration Testing
```python
# Test component integration
import unittest

class NLMSystemTests(unittest.TestCase):
    def setUp(self):
        self.config = create_default_config()
        self.brain = create_brain(self.config)
        self.world = create_simple_world()
        self.agent = create_agent_brain(self.brain)
    
    def test_initialization(self):
        self.brain.initialize()
        self.agent.initialize(self.world)
        self.assertTrue(self.agent.isRewardModulationEnabled())
    
    def test_simulation(self):
        # Run complete simulation
        for step in range(100):
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step, step * 0.001)
            action = self.agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, step * 0.1)
        
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
    
    def test_error_handling(self):
        # Test error conditions
        with self.assertRaises(Exception):
            # Code that should raise an exception
            pass
```

## Conclusion

The NLM project has been significantly enhanced with:

### Key Improvements

1. **Enhanced Command Line Interface**: Comprehensive advanced command line options for all experiment types
2. **Advanced Experiment Types**: Stress testing, learning curve analysis, stability testing, and plasticity mapping
3. **Improved Python Bindings**: More Pythonic APIs, factory functions, and enhanced functionality
4. **Comprehensive Testing**: Advanced test suite covering all new features
5. **Configuration Management**: Improved configuration system with validation and consistency checking
6. **Performance Optimization**: Enhanced memory management and computational efficiency
7. **Documentation and Examples**: Comprehensive guides and best practices

### Impact

These improvements make the NLM project:

- **More Accessible**: Intuitive command line interface and Python APIs
- **More Powerful**: Advanced experiment types for comprehensive testing
- **More Reliable**: Comprehensive testing and error handling
- **More Performant**: Optimized memory and computational efficiency
- **More Extensible**: Modular design for future enhancements

### Future Directions

The foundation laid by these improvements enables:

1. **Machine Learning Integration**: Using NLM as a foundation for learned policies
2. **Multi-Agent Systems**: Simulating multiple interacting agents
3. **Real-world Applications**: Deploying NLM in practical scenarios
4. **Research Platforms**: Providing a flexible platform for neural research
5. **Educational Tools**: Creating accessible neural simulation tools

The NLM project continues to evolve as a leading platform for experimental neural computation, providing researchers and developers with powerful tools for exploring artificial neural systems.

---

*This documentation covers NLM project version 0.1.0 with advanced features added in the latest release.*

For technical support, bug reports, or feature requests, please visit the NLM GitHub repository at: https://github.com/nlm-project/nlm

---
**Last Updated**: November 2026
**License**: MIT
