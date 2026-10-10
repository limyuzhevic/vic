# NLM Python API Debugging Guide

## Overview

This document provides comprehensive guidance on troubleshooting and debugging the NLM Python API. It covers common debugging scenarios, error handling techniques, and tools for diagnosing and resolving performance and functional issues.

## Table of Contents

- [Debugging Overview](#debugging-overview)
- [Common Debugging Scenarios](#common-debugging-scenarios)
  - [Import and Initialization Issues](#import-and-initialization-issues)
  - [Configuration Problems](#configuration-problems)
  - [Simulation Errors](#simulation-errors)
  - [Performance Issues](#performance-issues)
  - [Memory Problems](#memory-problems)
  - [I/O Issues](#io-issues)
- [Debugging Tools and Techniques](#debugging-tools-and-techniques)
  - [Python Debuggers](#python-debuggers)
  - [NLM-Specific Debuggers](#nlm-specific-debuggers)
  - [Logging and Monitoring](#logging-and-monitoring)
  - [Performance Profiling](#performance-profiling)
  - [Memory Profiling](#memory-profiling)
- [Error Handling and Recovery](#error-handling-and-recovery)
  - [Exception Hierarchy](#exception-hierarchy)
  - [Error Recovery Strategies](#error-recovery-strategies)
  - [Error Reporting](#error-reporting)
- [Troubleshooting Guide](#troubleshooting-guide)
  - [Step-by-Step Debugging](#step-by-step-debugging)
  - [Common Error Messages](#common-error-messages)
  - [Solutions and Workarounds](#solutions-and-workarounds)
- [Testing and Validation](#testing-and-validation)
  - [Unit Testing](#unit-testing)
  - [Integration Testing](#integration-testing)
  - [Performance Testing](#performance-testing)
  - [Load Testing](#load-testing)
- [Documentation and Resources](#documentation-and-resources)
  - [Documentation References](#documentation-references)
  - [Community Support](#community-support)
  - [Issue Tracking](#issue-tracking)
- [Best Practices for Debugging](#best-practices-for-debugging)
  - [Systematic Debugging Approach](#systematic-debugging-approach)
  - [Minimal Reproduction Examples](#minimal-reproduction-examples)
  - [Version Compatibility](#version-compatibility)
  - [Configuration Management](#configuration-management)

## Debugging Overview

### Importance of Debugging

Debugging is essential for:

1. **Development Efficiency**: Quickly identify and fix issues during development
2. **Production Stability**: Resolve issues in deployed systems
3. **Performance Optimization**: Identify and eliminate performance bottlenecks
4. **User Experience**: Provide timely support and resolve user issues
5. **System Reliability**: Ensure consistent and reliable operation

### Debugging Process

The debugging process typically follows these steps:

1. **Problem Identification**: Clearly define the problem and gather relevant information
2. **Hypothesis Formation**: Formulate hypotheses about potential causes
3. **Reproduction**: Create reproducible test cases
4. **Investigation**: Systematically investigate each hypothesis
5. **Solution Implementation**: Implement and test fixes
6. **Validation**: Verify that the fix resolves the issue
7. **Prevention**: Implement measures to prevent similar issues

### Debugging Strategies

1. **Isolation**: Isolate the problem to a specific component
2. **Simplification**: Simplify the problem to understand root causes
3. **Pattern Recognition**: Identify patterns in errors and behavior
4. **Incremental Testing**: Test changes incrementally
5. **Tool-Assisted Debugging**: Use appropriate debugging tools

## Common Debugging Scenarios

### Import and Initialization Issues

#### Symptoms

- Import errors (`ModuleNotFoundError`, `ImportError`)
- Initialization failures
- Missing dependencies
- Version conflicts

#### Common Causes

```python
# Example of import errors
try:
    import nlm
except ImportError as e:
    print(f"Import error: {e}")
    # Check if nlm is installed
    import subprocess
    subprocess.run(["pip", "install", "pynlm"])
```

#### Solutions

1. **Installation Issues**

```bash
# Check if NLM is installed
pip list | grep nlm

# Install or update NLM
pip install --upgrade pynlm

# Install with development dependencies
pip install -e .[dev]
```

2. **Environment Issues**

```python
import sys
print(f"Python version: {sys.version}")
print(f"Python path: {sys.path}")

# Check for NLM in Python path
import nlm
print(f"NLM version: {nlm.__version__}")
```

3. **Dependency Issues**

```python
# Check for required dependencies
try:
    import numpy
    import scipy
    import matplotlib
    print("All dependencies available")
except ImportError as e:
    print(f"Missing dependency: {e}")
    # Install missing dependencies
    import subprocess
    subprocess.run(["pip", "install", "numpy", "scipy", "matplotlib"])
```

### Configuration Problems

#### Symptoms

- Configuration validation errors
- Invalid parameter values
- Missing required parameters
- Configuration file format errors

#### Common Causes

```python
# Example of configuration errors
try:
    config = nlm.Config()
    config.set('brain.simulated_neurons', -100)  # Invalid value
    config.validate()
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Check valid range
    print("brain.simulated_neurons must be a positive integer")
```

#### Solutions

1. **Configuration Validation**

```python
from nlm import ConfigurationError

def validateConfiguration(config):
    """Validate configuration parameters."""
    try:
        config.validate()
        print("Configuration is valid")
        return True
    except ConfigurationError as e:
        print(f"Configuration error: {e}")
        return False

# Test validation
config = nlm.Config()
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)
validateConfiguration(config)
```

2. **Configuration File Issues**

```python
# Load configuration from file
def loadConfigFromFile(filename):
    """Load configuration from JSON file."""
    try:
        config = nlm.Config()
        config.loadFromFile(filename)
        print(f"Configuration loaded from {filename}")
        return config
    except Exception as e:
        print(f"Error loading configuration: {e}")
        # Create default configuration
        return nlm.create_default_config()

# Create example configuration
def createExampleConfig():
    """Create example configuration file."""
    config = nlm.create_default_config()
    config.set('brain.simulated_neurons', 1000)
    config.set('world.width', 20)
    config.set('world.height', 20)
    config.saveToFile('example_config.json')
    print("Example configuration created: example_config.json")

createExampleConfig()
```

### Simulation Errors

#### Symptoms

- Simulation crashes
- Runtime errors
- Invalid state errors
- Resource exhaustion

#### Common Causes

```python
# Example of simulation errors
try:
    # Run simulation
    for step in range(100):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, step)
        world.update(0.001)
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
    # Check for common issues
    print("Check: World and brain are properly initialized")
    print("Check: Action is valid for current world state")
```

#### Solutions

1. **Simulation Initialization**

```python
def safeSimulationInit():
    """Safely initialize simulation with error checking."""
    try:
        # Create configuration
        config = nlm.create_default_config()
        
        # Create brain
        brain = nlm.create_brain(config)
        
        # Create world
        world = nlm.create_simple_world()
        world.configure(20, 20, 64, 64)
        world.setAgentStart(10, 10)
        
        # Initialize brain
        brain.initialize(world)
        
        # Create agent brain
        agent_brain = nlm.create_agent_brain(brain)
        agent_brain.initialize(brain, world)
        
        print("Simulation initialized successfully")
        return brain, world, agent_brain
        
    except Exception as e:
        print(f"Simulation initialization failed: {e}")
        # Clean up resources
        try:
            brain.save("partial_brain_state.json")
        except:
            pass
        raise

# Safe simulation run
def safeSimulationRun(brain, world, agent_brain, steps=100):
    """Safely run simulation with error recovery."""
    try:
        for step in range(steps):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            result = world.applyMotorCommand(action, step)
            if not result.wasSuccessful():
                print(f"Action failed at step {step}: {result.getError()}")
                continue
            
            # Update world
            world.update(0.001)
            
            # Record metrics
            neurons_active = brain.getActiveNeuronCount()
            
        print(f"Simulation completed successfully with {steps} steps")
        return True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        # Save partial state
        try:
            brain.save(f"failed_brain_{step}.json")
        except:
            pass
        return False
```

2. **Error Recovery**

```python
# Error recovery with checkpoints
def simulateWithRecovery(config, steps=100, checkpoint_interval=20):
    """Run simulation with checkpoint-based recovery."""
    from nlm import expert
    
    # Create checkpoint manager
    checkpoint_manager = expert.CheckpointManager(
        checkpoint_dir="./checkpoints",
        auto_checkpoint=True,
        checkpoint_interval=checkpoint_interval
    )
    
    # Create simulation systems
    brain = nlm.create_brain(config)
    world = nlm.create_simple_world()
    world.configure(20, 20, 64, 64)
    world.setAgentStart(10, 10)
    
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    try:
        for step in range(steps):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, step)
            world.update(0.001)
            
            # Create checkpoint periodically
            if step % checkpoint_interval == 0:
                checkpoint_name = checkpoint_manager.create_checkpoint(
                    f"checkpoint_{step}",
                    {
                        'brain_state': brain.getStatistics(),
                        'agent_state': world.getAgentBody(),
                        'step': step
                    }
                )
                print(f"Created checkpoint: {checkpoint_name}")
        
        print(f"Simulation completed successfully with {steps} steps")
        return True
        
    except Exception as e:
        print(f"Simulation failed at step {step}: {e}")
        
        # Try to restore from last checkpoint
        try:
            last_checkpoint = checkpoint_manager.get_checkpoint(f"checkpoint_{step - checkpoint_interval}")
            if last_checkpoint:
                print(f"Restoring from checkpoint: checkpoint_{step - checkpoint_interval}")
                # Restore state
                restored_state = checkpoint_manager.restore_checkpoint(
                    f"checkpoint_{step - checkpoint_interval}",
                    {'step': step - checkpoint_interval}
                )
                print("Restored successfully")
                return True
        except Exception as restore_error:
            print(f"Failed to restore from checkpoint: {restore_error}")
        
        return False
```

### Performance Issues

#### Symptoms

- Slow simulation steps
- High memory usage
- Poor scalability
- Resource contention

#### Common Causes

```python
# Example of performance debugging
def profileSimulationPerformance():
    """Profile simulation performance."""
    from nlm import expert
    
    # Create performance monitor
    monitor = expert.PerformanceMonitor()
    
    # Create performance profiler
    profiler = expert.PerformanceProfiler()
    
    # Profile key operations
    operations = [
        "sensory_processing",
        "neural_computation",
        "action_selection",
        "world_update"
    ]
    
    # Start profiling for each operation
    for operation in operations:
        profiler.startProfiling(operation)
        
        # Simulate work
        time.sleep(0.001)
        
        execution_time = profiler.stopProfiling(operation)
        print(f"{operation}: {execution_time:.6f} seconds")
    
    # Get performance report
    report = profiler.getPerformanceReport()
    
    # Identify bottlenecks
    bottlenecks = report['bottleneck_operations']
    print(f"Performance bottlenecks: {bottlenecks}")
    
    return report

# Run performance profiling
performance_report = profileSimulationPerformance()
```

#### Solutions

1. **Performance Profiling**

```python
from nlm import expert
import time

def detailedPerformanceAnalysis():
    """Perform detailed performance analysis."""
    
    # Create comprehensive profiler
    profiler = expert.PerformanceProfiler()
    
    # Profile simulation pipeline
    def profileSimulationStep():
        """Profile individual simulation step."""
        
        # Profile sensory processing
        profiler.startProfiling("sensory_input")
        # ... sensory processing code ...
        sensory_time = profiler.stopProfiling("sensory_input")
        
        # Profile neural computation
        profiler.startProfiling("neural_computation")
        # ... neural computation code ...
        neural_time = profiler.stopProfiling("neural_computation")
        
        # Profile action selection
        profiler.startProfiling("action_selection")
        # ... action selection code ...
        action_time = profiler.stopProfiling("action_selection")
        
        # Profile world update
        profiler.startProfiling("world_update")
        # ... world update code ...
        world_time = profiler.stopProfiling("world_update")
        
        return {
            'sensory_time': sensory_time,
            'neural_time': neural_time,
            'action_time': action_time,
            'world_time': world_time,
            'total_time': sensory_time + neural_time + action_time + world_time
        }
    
    # Run profiling
    step_results = []
    for _ in range(10):
        result = profileSimulationStep()
        step_results.append(result)
    
    # Analyze results
    total_times = [r['total_time'] for r in step_results]
    avg_time = sum(total_times) / len(total_times)
    max_time = max(total_times)
    min_time = min(total_times)
    
    print(f"Performance Analysis:")
    print(f"  Average step time: {avg_time:.6f} seconds")
    print(f"  Minimum step time: {min_time:.6f} seconds")
    print(f"  Maximum step time: {max_time:.6f} seconds")
    print(f"  Time variation: {(max_time - min_time) / avg_time * 100:.2f}%")
    
    return {
        'average_time': avg_time,
        'max_time': max_time,
        'min_time': min_time,
        'variation': (max_time - min_time) / avg_time * 100
    }

# Run performance analysis
performance_analysis = detailedPerformanceAnalysis()
```

2. **Memory Profiling**

```python
from nlm import expert

def analyzeMemoryUsage():
    """Analyze memory usage patterns."""
    
    # Create memory profiler
    profiler = expert.MemoryProfiler()
    
    # Take initial snapshot
    snapshot1 = profiler.take_snapshot("start")
    
    # Run simulation
    for step in range(100):
        # ... simulation code ...
        pass
    
    # Take final snapshot
    snapshot2 = profiler.take_snapshot("end")
    
    # Analyze memory growth
    analysis = profiler.analyze_growth(300.0)
    
    print(f"Memory Analysis:")
    print(f"  Growth rate: {analysis['growth_rate']:.2%}")
    print(f"  Trend: {analysis['trend']}")
    
    # Check for memory leaks
    if analysis['growth_rate'] > 0.01:
        print("  WARNING: High memory growth detected - potential memory leak")
    
    return analysis

# Run memory analysis
memory_analysis = analyzeMemoryUsage()
```

### Memory Problems

#### Symptoms

- High memory usage
- Memory leaks
- Out of memory errors
- Memory fragmentation

#### Common Causes

```python
# Example of memory debugging
def debugMemoryUsage():
    """Debug memory usage issues."""
    import tracemalloc
    from nlm import expert
    
    # Start memory tracking
    tracemalloc.start()
    
    # Take initial snapshot
    snapshot1 = tracemalloc.take_snapshot()
    
    # Run memory-intensive operations
    for i in range(100):
        # Allocate memory
        data = list(range(10000))  # ~80KB
        
        # Process data
        result = sum(data) / len(data) if data else 0
        
        # Clear reference to free memory
        del data
        del result
    
    # Force garbage collection
    import gc
    gc.collect()
    
    # Take final snapshot
    snapshot2 = tracemalloc.take_snapshot()
    
    # Compare snapshots
    top_stats = snapshot2.compare_to(snapshot1, 'lineno')
    
    print(f"Memory Usage Comparison:")
    for stat in top_stats[:10]:
        print(f"  {stat}")
    
    # Get current memory usage
    current, peak = tracemalloc.get_traced_memory()
    print(f"\nCurrent memory usage: {current / 1024 / 1024:.2f} MB")
    print(f"Peak memory usage: {peak / 1024 / 1024:.2f} MB")
    
    tracemalloc.stop()
    
    return {
        'current_mb': current / 1024 / 1024,
        'peak_mb': peak / 1024 / 1024,
        'top_stats': top_stats[:10]
    }

# Run memory debugging
memory_debug = debugMemoryUsage()
```

#### Solutions

1. **Memory Pool Usage**

```python
from nlm import expert

# Create memory pool
pool = expert.MemoryPool(8192, 65536)

# Allocate memory from pool
memory_block = pool.allocate(1024)

# Use memory block
# ... process data ...

# Free memory block
pool.free(memory_block)

# Get pool statistics
stats = pool.getStats()
print(f"Memory pool statistics: {stats}")
```

2. **Memory Leak Detection**

```python
import gc
from nlm import expert

# Enable debug mode for garbage collection
gc.set_debug(gc.DEBUG_STATS)

# Create object with reference cycles to test garbage collection
class Node:
    def __init__(self, value):
        self.value = value
        self.children = []
    
    def add_child(self, child):
        self.children.append(child)

# Create objects with reference cycles
root = Node(1)
child1 = Node(2)
child2 = Node(3)

root.add_child(child1)
root.add_child(child2)

child1.add_child(Node(4))
child2.add_child(Node(5))

# Count objects before garbage collection
objects_before = len(gc.get_objects())

# Clear references and force garbage collection
del root
del child1
del child2
gc.collect()

# Count objects after garbage collection
objects_after = len(gc.get_objects())

print(f"Objects before GC: {objects_before}")
print(f"Objects after GC: {objects_after}")
print(f"Objects collected: {objects_before - objects_after}")
```

### I/O Issues

#### Symptoms

- Slow file operations
- Serialization bottlenecks
- Data transfer delays
- Resource contention

#### Common Causes

```python
# Example of I/O debugging
import json
import time
from nlm import expert

def debugFileOperations():
    """Debug file operation performance."""
    
    # Create test data
    test_data = list(range(10000))
    
    # Test JSON serialization performance
    start_time = time.time()
    for _ in range(100):
        json_str = json.dumps(test_data)
    json_time = time.time() - start_time
    
    # Test NLM serialization performance
    from nlm import expert
    start_time = time.time()
    for _ in range(100):
        # Simulate NLM serialization
        serialized = expert.vector_to_json(test_data)
    nlm_time = time.time() - start_time
    
    # Test file write performance
    start_time = time.time()
    with open('test_output.json', 'w') as f:
        json.dump(test_data, f)
    write_time = time.time() - start_time
    
    # Test file read performance
    start_time = time.time()
    with open('test_output.json', 'r') as f:
        loaded_data = json.load(f)
    read_time = time.time() - start_time
    
    print(f"File Operation Performance:")
    print(f"  JSON serialization: {json_time:.4f} seconds")
    print(f"  NLM serialization: {nlm_time:.4f} seconds")
    print(f"  File write: {write_time:.4f} seconds")
    print(f"  File read: {read_time:.4f} seconds")
    
    # Cleanup
    import os
    os.remove('test_output.json')
    
    return {
        'json_time': json_time,
        'nlm_time': nlm_time,
        'write_time': write_time,
        'read_time': read_time
    }

# Run file operation debugging
io_performance = debugFileOperations()
```

#### Solutions

1. **Batch I/O Operations**

```python
from nlm import expert

# Create batch processor for I/O operations
batch = expert.BatchProcessor("io_batch")

# Add I/O operations to batch
batch.addOperation(lambda: load_large_file("data1.json"))
batch.addOperation(lambda: load_large_file("data2.json"))
batch.addOperation(lambda: process_loaded_data())
batch.addOperation(lambda: save_results("output.json"))

# Execute batch operations
batch.execute()
```

2. **Efficient Serialization**

```python
from nlm import expert

# Use efficient serialization for large data
def efficientSerialization(data):
    """Efficiently serialize large data."""
    # Use NLM's vector serialization for numeric data
    if isinstance(data, list) and all(isinstance(x, (int, float)) for x in data):
        return expert.vector_to_json(data)
    
    # Use NLM's object serialization for complex data
    return expert.object_to_json(data)

# Deserialize efficiently
def efficientDeserialization(json_str):
    """Efficiently deserialize data."""
    # Check if it's a vector
    if json_str.strip().startswith('[') and json_str.strip().endswith(']'):
        return expert.json_to_vector(json_str)
    
    # Otherwise, it's an object
    return expert.json_to_object(json_str)
```

## Debugging Tools and Techniques

### Python Debuggers

#### pdb (Python Debugger)

```python
# Basic pdb usage
import pdb

def debugFunction():
    # ... code to debug ...
    pdb.set_trace()  # Stop here for interactive debugging
    # ... more code ...

# Set breakpoint
import sys

def buggyFunction():
    x = 1
    y = 0
    # This will cause ZeroDivisionError
    result = x / y
    return result

# Run with debugger
if __name__ == "__main__":
    try:
        buggyFunction()
    except:
        import pdb
        pdb.post_mortem()
```

#### ipdb (Improved Python Debugger)

```bash
# Install ipdb
pip install ipdb

# Use ipdb for better debugging experience
import ipdb

def debugWithIpdb():
    ipdb.set_trace()  # Better debugging interface
    # ... debug code ...
```

#### PyCharm Debugger

```python
# PyCharm debugging features
import sys
import debugpy

# Enable debugging
if __name__ == "__main__":
    # Configure debugger
    debugpy.configure_http_port(61234)
    debugpy.listen(61234)
    
    # Run in debug mode
    # ... code to debug ...
```

### NLM-Specific Debuggers

#### NLM Debug Mode

```python
import nlm

# Enable NLM debug mode
nlm.Config.setDebugMode(True)

# Set log level
nlm.Config.setLogLevel("DEBUG")

# Enable detailed error reporting
nlm.Config.enableDetailedErrorReporting(True)

# Example of debugging with NLM features
def debugNLMSetup():
    """Debug NLM setup with enhanced logging."""
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
    
    logger = logging.getLogger('nlm.debug')
    
    try:
        # Create configuration
        logger.info("Creating configuration...")
        config = nlm.create_default_config()
        logger.info(f"Configuration created: {config}")
        
        # Create brain
        logger.info("Creating brain...")
        brain = nlm.create_brain(config)
        logger.info(f"Brain created: {brain}")
        
        # Create world
        logger.info("Creating world...")
        world = nlm.create_simple_world()
        world.configure(20, 20, 64, 64)
        world.setAgentStart(10, 10)
        logger.info(f"World created: {world}")
        
        # Initialize brain
        logger.info("Initializing brain...")
        brain.initialize(world)
        logger.info("Brain initialized successfully")
        
    except Exception as e:
        logger.error(f"Error during setup: {e}", exc_info=True)
        raise

# Run debugging
if __name__ == "__main__":
    debugNLMSetup()
```

#### Debug Visualization

```python
from nlm import expert
import matplotlib.pyplot as plt

def visualizeDebugData():
    """Visualize debug data for analysis."""
    
    # Create debug data
    debug_data = {
        'steps': list(range(100)),
        'neuron_activities': [i * 0.5 for i in range(100)],
        'action_frequency': [i * 0.2 for i in range(100)],
        'memory_usage': [i * 0.1 for i in range(100)]
    }
    
    # Create visualization
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Neuron activities plot
    axes[0, 0].plot(debug_data['steps'], debug_data['neuron_activities'])
    axes[0, 0].set_title('Neuron Activities')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Activity Level')
    
    # Action frequency plot
    axes[0, 1].plot(debug_data['steps'], debug_data['action_frequency'])
    axes[0, 1].set_title('Action Frequency')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Frequency')
    
    # Memory usage plot
    axes[1, 0].plot(debug_data['steps'], debug_data['memory_usage'])
    axes[1, 0].set_title('Memory Usage')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Memory Usage (MB)')
    
    # Combined visualization
    axes[1, 1].plot(debug_data['steps'], debug_data['neuron_activities'], label='Neuron Activities')
    axes[1, 1].plot(debug_data['steps'], debug_data['action_frequency'], label='Action Frequency')
    axes[1, 1].set_title('Combined Debug Visualization')
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('Value')
    axes[1, 1].legend()
    
    plt.tight_layout()
    plt.savefig('debug_visualization.png', dpi=150)
    plt.show()
    
    print("Debug visualization saved as 'debug_visualization.png'")

# Run visualization
visualizeDebugData()
```

### Logging and Monitoring

#### Logging Configuration

```python
import logging
import nlm

# Configure logging for NLM
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_application.log'),
        logging.StreamHandler()
    ]
)

# Create NLM logger
logger = logging.getLogger('nlm.application')

# Example of logging in NLM application
class NLMApplication:
    def __init__(self):
        self.logger = logging.getLogger('nlm.application')
        self.config = nlm.Config()
    
    def runSimulation(self, steps=100):
        """Run simulation with logging."""
        self.logger.info(f"Starting simulation with {steps} steps")
        
        try:
            # Create configuration
            self.logger.debug("Creating configuration...")
            config = nlm.create_default_config()
            
            # Create brain
            self.logger.debug("Creating brain...")
            brain = nlm.create_brain(config)
            
            # Create world
            self.logger.debug("Creating world...")
            world = nlm.create_simple_world()
            world.configure(20, 20, 64, 64)
            world.setAgentStart(10, 10)
            
            # Initialize brain
            self.logger.debug("Initializing brain...")
            brain.initialize(world)
            
            # Run simulation
            for step in range(steps):
                self.logger.debug(f"Step {step}/{steps}")
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent_brain = nlm.create_agent_brain(brain)
                agent_brain.processSensoryInput(percept)
                
                # Generate action
                action = agent_brain.decodeMotorCommand()
                
                # Apply action
                result = world.applyMotorCommand(action, step)
                
                # Log action result
                if result.wasSuccessful():
                    self.logger.debug(f"Action successful at step {step}")
                else:
                    self.logger.warning(f"Action failed at step {step}: {result.getError()}")
                
                # Update world
                world.update(0.001)
                
                # Log performance
                neurons_active = brain.getActiveNeuronCount()
                self.logger.debug(f"Neurons active: {neurons_active}")
            
            self.logger.info(f"Simulation completed successfully with {steps} steps")
            return True
            
        except Exception as e:
            self.logger.error(f"Simulation failed: {e}", exc_info=True)
            return False

# Run application with logging
if __name__ == "__main__":
    app = NLMApplication()
    app.runSimulation(100)
```

#### Performance Monitoring

```python
from nlm import expert
import time

class PerformanceMonitor:
    """Performance monitoring for NLM simulations."""
    
    def __init__(self):
        self.metrics = {
            'step_times': [],
            'neuron_activities': [],
            'memory_usage': [],
            'cpu_usage': []
        }
        self.start_time = None
        self.end_time = None
    
    def startMonitoring(self):
        """Start performance monitoring."""
        self.start_time = time.time()
        print(f"Performance monitoring started at {self.start_time}")
    
    def stopMonitoring(self):
        """Stop performance monitoring."""
        self.end_time = time.time()
        print(f"Performance monitoring stopped at {self.end_time}")
        
        # Calculate total time
        total_time = self.end_time - self.start_time
        print(f"Total monitoring time: {total_time:.2f} seconds")
        
        return {
            'total_time': total_time,
            'metrics': self.metrics
        }
    
    def recordStep(self, neurons_active: int, step_time: float, memory_usage: float):
        """Record step performance metrics.
        
        Args:
            neurons_active: Number of active neurons
            step_time: Time for current step
            memory_usage: Memory usage during step
        """
        self.metrics['step_times'].append(step_time)
        self.metrics['neuron_activities'].append(neurons_active)
        self.metrics['memory_usage'].append(memory_usage)
        
        # Print step information
        print(f"Step completed: {step_time:.6f}s, Neurons active: {neurons_active}, Memory: {memory_usage:.2f} MB")
    
    def getPerformanceReport(self):
        """Get performance report.
        
        Returns:
            Dictionary with performance metrics
        """
        if not self.metrics['step_times']:
            return {}
        
        total_time = self.end_time - self.start_time
        steps_completed = len(self.metrics['step_times'])
        
        report = {
            'total_time': total_time,
            'steps_completed': steps_completed,
            'steps_per_second': steps_completed / total_time if total_time > 0 else 0,
            'avg_step_time': sum(self.metrics['step_times']) / len(self.metrics['step_times']),
            'min_step_time': min(self.metrics['step_times']),
            'max_step_time': max(self.metrics['step_times']),
            'avg_neuron_activity': sum(self.metrics['neuron_activities']) / len(self.metrics['neuron_activities']),
            'avg_memory_usage': sum(self.metrics['memory_usage']) / len(self.metrics['memory_usage'])
        }
        
        return report

# Use performance monitor
monitor = PerformanceMonitor()
monitor.startMonitoring()

# Run simulation with monitoring
for step in range(100):
    # Simulate work
    time.sleep(0.001)
    
    # Record metrics
    monitor.recordStep(
        neurons_active=500 + step * 2,
        step_time=0.001,
        memory_usage=50 + step * 0.1
    )

# Get performance report
report = monitor.getPerformanceReport()
print(f"\nPerformance Report:")
print(f"  Total time: {report['total_time']:.2f} seconds")
print(f"  Steps per second: {report['steps_per_second']:.2f}")
print(f"  Average step time: {report['avg_step_time']:.6f} seconds")
print(f"  Average neuron activity: {report['avg_neuron_activity']:.1f}")
print(f"  Average memory usage: {report['avg_memory_usage']:.2f} MB")
```

### Performance Profiling

#### Detailed Performance Profiling

```python
from nlm import expert
import time
import cProfile
import pstats
from io import StringIO

class DetailedProfiler:
    """Detailed performance profiler for NLM."""
    
    def __init__(self):
        self.profiler = cProfile.Profile()
        self.function_timings = {}
    
    def startProfiling(self, function_name: str):
        """Start profiling for a function.
        
        Args:
            function_name: Name of function to profile
        """
        self.profiler.enable()
        self.current_function = function_name
        self.start_time = time.time()
    
    def stopProfiling(self, function_name: str) -> float:
        """Stop profiling for a function.
        
        Args:
            function_name: Name of function to stop profiling
            
        Returns:
            Execution time
        """
        self.profiler.disable()
        end_time = time.time()
        execution_time = end_time - self.start_time
        
        # Store function timing
        self.function_timings[function_name] = execution_time
        
        return execution_time
    
    def profileSimulation(self, simulation_function, *args, **kwargs):
        """Profile entire simulation.
        
        Args:
            simulation_function: Function to profile
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Function result and performance data
        """
        # Start profiler
        self.profiler.enable()
        start_time = time.time()
        
        # Run simulation
        result = simulation_function(*args, **kwargs)
        
        # Stop profiler
        end_time = time.time()
        total_time = end_time - start_time
        
        # Get profiling statistics
        stream = StringIO()
        stats = pstats.Stats(self.profiler, stream=stream)
        stats.sort_stats('cumulative')
        
        # Get top 10 functions by cumulative time
        top_functions = []
        for func, (cc, nc, tt, ct, callers) in stats.stats.items():
            if ct > 0:
                top_functions.append((func, ct, tt))
        
        top_functions.sort(key=lambda x: x[1], reverse=True)
        
        # Extract relevant profiling information
        profiling_data = {
            'total_time': total_time,
            'function_timings': self.function_timings,
            'top_functions': top_functions[:10],
            'total_calls': stats.total_calls,
            'primitive_calls': stats.primitive_calls
        }
        
        # Disable profiler
        self.profiler.disable()
        
        return result, profiling_data

# Use detailed profiler
profiler = DetailedProfiler()

# Profile a simulation function
def simulation_function():
    """Example simulation function."""
    for i in range(100):
        # Simulate simulation work
        time.sleep(0.001)
    
    return "Simulation completed"

# Run profiling
result, profiling_data = profiler.profileSimulation(simulation_function)

print(f"\nProfiling Results:")
print(f"  Total time: {profiling_data['total_time']:.2f} seconds")
print(f"  Number of steps: {len(profiling_data['function_timings'])}")
print(f"  Total calls: {profiling_data['total_calls']}")
print(f"  Primitive calls: {profiling_data['primitive_calls']}")

print(f"\nTop 10 Functions by Cumulative Time:")
for i, (func, cumulative_time, total_time) in enumerate(profiling_data['top_functions']):
    print(f"  {i+1}. {func}: {cumulative_time:.4f}s (total: {total_time:.4f}s)")

print(f"\nFunction Timings:")
for func, exec_time in profiling_data['function_timings'].items():
    print(f"  {func}: {exec_time:.6f}s")
```

#### Memory Profiling

```python
import tracemalloc
import gc
from nlm import expert

class MemoryProfiler:
    """Memory profiler for NLM."""
    
    def __init__(self):
        self.snapshots = {}
        self.memory_growth_data = []
    
    def takeSnapshot(self, tag: str = ""):
        """Take memory snapshot.
        
        Args:
            tag: Snapshot tag
            
        Returns:
            Snapshot
        """
        # Force garbage collection
        gc.collect()
        
        # Take snapshot
        snapshot = tracemalloc.take_snapshot()
        
        # Store snapshot
        self.snapshots[tag] = snapshot
        
        # Get current memory usage
        current, peak = tracemalloc.get_traced_memory()
        self.memory_growth_data.append({
            'tag': tag,
            'current_mb': current / 1024 / 1024,
            'peak_mb': peak / 1024 / 1024,
            'timestamp': time.time()
        })
        
        print(f"Memory snapshot taken: {tag}")
        print(f"  Current: {current / 1024 / 1024:.2f} MB")
        print(f"  Peak: {peak / 1024 / 1024:.2f} MB")
        
        return snapshot
    
    def compareSnapshots(self, tag1: str, tag2: str):
        """Compare two memory snapshots.
        
        Args:
            tag1: First snapshot tag
            tag2: Second snapshot tag
            
        Returns:
            Comparison statistics
        """
        if tag1 not in self.snapshots or tag2 not in self.snapshots:
            raise ValueError(f"Snapshot tags {tag1} or {tag2} not found")
        
        # Compare snapshots
        top_stats = self.snapshots[tag2].compare_to(self.snapshots[tag1], 'lineno')
        
        print(f"Memory usage change from {tag1} to {tag2}:")
        for stat in top_stats[:10]:
            print(f"  {stat}")
        
        return {
            'tag1': tag1,
            'tag2': tag2,
            'top_stats': top_stats[:10],
            'total_memory_change': sum(stat.size_diff for stat in top_stats)
        }
    
    def analyzeMemoryGrowth(self, time_window: float = 60.0):
        """Analyze memory growth over time.
        
        Args:
            time_window: Time window for analysis (seconds)
            
        Returns:
            Memory growth analysis
        """
        if len(self.memory_growth_data) < 2:
            return {}
        
        # Filter data within time window
        current_time = time.time()
        recent_data = [
            data for data in self.memory_growth_data
            if current_time - data['timestamp'] <= time_window
        ]
        
        if len(recent_data) < 2:
            return {}
        
        # Calculate growth rate
        initial_data = recent_data[0]
        final_data = recent_data[-1]
        
        time_diff = final_data['timestamp'] - initial_data['timestamp']
        memory_diff = final_data['current_mb'] - initial_data['current_mb']
        
        growth_rate = (memory_diff / time_diff) if time_diff > 0 else 0
        
        # Determine trend
        memory_values = [data['current_mb'] for data in recent_data]
        if len(memory_values) >= 3:
            # Simple trend detection
            if memory_values[-1] > memory_values[0] + 1:
                trend = "increasing"
            elif memory_values[-1] < memory_values[0] - 1:
                trend = "decreasing"
            else:
                trend = "stable"
        else:
            trend = "insufficient_data"
        
        analysis = {
            'initial_memory_mb': initial_data['current_mb'],
            'final_memory_mb': final_data['current_mb'],
            'memory_change_mb': memory_diff,
            'time_window_seconds': time_diff,
            'growth_rate_mb_per_second': growth_rate,
            'growth_rate_percent': (growth_diff / initial_data['current_mb'] * 100) if initial_data['current_mb'] > 0 else 0,
            'trend': trend
        }
        
        print(f"Memory Growth Analysis (last {time_window} seconds):")
        print(f"  Initial memory: {analysis['initial_memory_mb']:.2f} MB")
        print(f"  Final memory: {analysis['final_memory_mb']:.2f} MB")
        print(f"  Memory change: {analysis['memory_change_mb']:.2f} MB")
        print(f"  Growth rate: {analysis['growth_rate_mb_per_second']:.6f} MB/second")
        print(f"  Trend: {analysis['trend']}")
        
        return analysis

# Use memory profiler
memory_profiler = MemoryProfiler()

# Start memory tracking
tracemalloc.start()

# Take initial snapshot
memory_profiler.takeSnapshot("start")

# Run simulation
for step in range(100):
    # Simulate memory allocation
    data = list(range(10000))
    result = sum(data) / len(data) if data else 0
    
    # Clear reference
    del data
    del result
    
    # Force garbage collection periodically
    if step % 10 == 0:
        gc.collect()
    
    # Record memory snapshot periodically
    if step % 20 == 0:
        memory_profiler.takeSnapshot(f"step_{step}")

# Take final snapshot
memory_profiler.takeSnapshot("end")

# Compare snapshots
comparison = memory_profiler.compareSnapshots("start", "end")

# Analyze memory growth
analysis = memory_profiler.analyzeMemoryGrowth(300.0)  # Analyze last 5 minutes

# Cleanup
tracemalloc.stop()
```

## Error Handling and Recovery

### Exception Hierarchy

#### NLM Exception Types

```python
from nlm import NLMError, ConfigurationError, SimulationError, ValidationError

# Base exception class
class NLMError(Exception):
    """Base exception for all NLM errors."""
    
    def __init__(self, message: str, error_code: str = "general"):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.timestamp = time.time()
    
    def __str__(self):
        return f"[{self.error_code}] {self.message}"

# Configuration error
class ConfigurationError(NLMError):
    """Configuration-related errors."""
    
    def __init__(self, message: str, config_key: str = ""):
        super().__init__(message, "configuration")
        self.config_key = config_key

# Simulation error
class SimulationError(NLMError):
    """Simulation-related errors."""
    
    def __init__(self, message: str, step: int = 0):
        super().__init__(message, "simulation")
        self.step = step

# Validation error
class ValidationError(NLMError):
    """Validation-related errors."""
    
    def __init__(self, message: str, validation_rule: str = ""):
        super().__init__(message, "validation")
        self.validation_rule = validation_rule
```

#### Exception Handling

```python
from nlm import NLMError, ConfigurationError, SimulationError, ValidationError

def handleNLMExceptions():
    """Example of NLM exception handling."""
    
    try:
        # Code that might raise NLM exceptions
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        world = nlm.create_simple_world()
        world.configure(20, 20, 64, 64)
        world.setAgentStart(10, 10)
        
        brain.initialize(world)
        
        # Run simulation
        for step in range(100):
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            world.update(0.001)
        
    except ConfigurationError as e:
        print(f"Configuration error: {e}")
        print(f"  Error code: {e.error_code}")
        print(f"  Configuration key: {e.config_key}")
        
    except SimulationError as e:
        print(f"Simulation error: {e}")
        print(f"  Step: {e.step}")
        print(f"  Timestamp: {e.timestamp}")
        
    except ValidationError as e:
        print(f"Validation error: {e}")
        print(f"  Validation rule: {e.validation_rule}")
        
    except NLMError as e:
        print(f"NLM error: {e}")
        print(f"  Error code: {e.error_code}")
        print(f"  Timestamp: {e.timestamp}")
        
    except Exception as e:
        print(f"Unexpected error: {e}")
        # Convert to NLM error
        if not isinstance(e, NLMError):
            raise NLMError(f"Unexpected error in NLM simulation: {e}", "unexpected") from e

# Run exception handling example
if __name__ == "__main__":
    handleNLMExceptions()
```

### Error Recovery Strategies

#### Recovery with Checkpoints

```python
from nlm import expert

class SimulationRecovery:
    """Simulation recovery with checkpoint support."""
    
    def __init__(self, checkpoint_dir: str = "./checkpoints"):
        """Initialize recovery system.
        
        Args:
            checkpoint_dir: Directory for checkpoints
        """
        self.checkpoint_manager = expert.CheckpointManager(
            checkpoint_dir=checkpoint_dir,
            auto_checkpoint=True,
            checkpoint_interval=20.0
        )
        self.recovery_attempts = 0
        self.max_recovery_attempts = 3
    
    def runWithRecovery(self, config, steps=100):
        """Run simulation with recovery.
        
        Args:
            config: Simulation configuration
            steps: Number of simulation steps
            
        Returns:
            Simulation result and recovery status
        """
        try:
            # Create simulation systems
            brain = nlm.create_brain(config)
            world = nlm.create_simple_world()
            world.configure(20, 20, 64, 64)
            world.setAgentStart(10, 10)
            
            brain.initialize(world)
            
            agent_brain = nlm.create_agent_brain(brain)
            agent_brain.initialize(brain, world)
            
            # Run simulation
            for step in range(steps):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent_brain.processSensoryInput(percept)
                
                # Generate action
                action = agent_brain.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, step)
                world.update(0.001)
                
                # Record performance
                neurons_active = brain.getActiveNeuronCount()
                
                # Recovery attempt
                if step > 0 and step % 30 == 0:
                    recovery_result = self.attemptRecovery(step, brain, world)
                    if not recovery_result:
                        print(f"Recovery failed at step {step}")
                        return {
                            'success': False,
                            'steps_completed': step,
                            'recovery_attempts': self.recovery_attempts,
                            'error': 'Recovery failed'
                        }
            
            print(f"Simulation completed successfully with {steps} steps")
            return {
                'success': True,
                'steps_completed': steps,
                'recovery_attempts': self.recovery_attempts,
                'error': None
            }
            
        except Exception as e:
            print(f"Simulation failed: {e}")
            recovery_result = self.attemptRecovery(steps, brain, world)
            
            if recovery_result:
                print(f"Successfully recovered from failure at step {steps}")
                return {
                    'success': True,
                    'steps_completed': steps,
                    'recovery_attempts': self.recovery_attempts,
                    'error': None
                }
            else:
                print(f"Failed to recover from failure at step {steps}")
                return {
                    'success': False,
                    'steps_completed': steps,
                    'recovery_attempts': self.recovery_attempts,
                    'error': str(e)
                }
    
    def attemptRecovery(self, step: int, brain, world):
        """Attempt to recover from simulation failure.
        
        Args:
            step: Current simulation step
            brain: Brain instance
            world: World instance
            
        Returns:
            True if recovery successful, False otherwise
        """
        self.recovery_attempts += 1
        
        if self.recovery_attempts > self.max_recovery_attempts:
            print(f"Maximum recovery attempts ({self.max_recovery_attempts}) exceeded")
            return False
        
        try:
            # Try to restore from last checkpoint
            checkpoint_name = f"checkpoint_{step - 20}"
            last_checkpoint = self.checkpoint_manager.get_checkpoint(checkpoint_name)
            
            if last_checkpoint:
                print(f"Found checkpoint: {checkpoint_name}")
                
                # Restore simulation state
                restored_state = self.checkpoint_manager.restore_checkpoint(
                    checkpoint_name,
                    {
                        'step': step - 20,
                        'brain_state': brain.getStatistics(),
                        'world_state': world.getAgentBody()
                    }
                )
                
                print("Successfully restored from checkpoint")
                return True
            else:
                print(f"No checkpoint found for recovery at step {step}")
                return False
                
        except Exception as e:
            print(f"Recovery attempt failed: {e}")
            return False

# Use recovery system
recovery_system = SimulationRecovery()

# Run simulation with recovery
result = recovery_system.runWithRecovery(
    config=nlm.create_default_config(),
    steps=100
)

print(f"\nSimulation Result:")
print(f"  Success: {result['success']}")
print(f"  Steps completed: {result['steps_completed']}")
print(f"  Recovery attempts: {result['recovery_attempts']}")
if result['error']:
    print(f"  Error: {result['error']}")
```

### Error Reporting

#### Error Reporting System

```python
import traceback
import json
from datetime import datetime
from nlm import NLMError

class ErrorReporter:
    """Error reporting for NLM applications."""
    
    def __init__(self, report_file: str = "nlm_errors.log"):
        """Initialize error reporter.
        
        Args:
            report_file: File for error reports
        """
        self.report_file = report_file
        self.error_count = 0
        self.error_details = []
    
    def reportError(self, error: Exception, context: str = "", simulation_state: dict = None):
        """Report error with context.
        
        Args:
            error: Exception that occurred
            context: Context where error occurred
            simulation_state: Current simulation state
        """
        self.error_count += 1
        
        # Get error information
        error_info = {
            'timestamp': datetime.now().isoformat(),
            'error_type': type(error).__name__,
            'error_message': str(error),
            'context': context,
            'simulation_state': simulation_state,
            'stack_trace': traceback.format_exc(),
            'error_code': getattr(error, 'error_code', 'unknown')
        }
        
        # Store error details
        self.error_details.append(error_info)
        
        # Log error to file
        with open(self.report_file, 'a') as f:
            f.write(json.dumps(error_info, indent=2))
            f.write('\n')
        
        # Print error to console
        print(f"ERROR REPORTED ({self.error_count}):")
        print(f"  Timestamp: {error_info['timestamp']}")
        print(f"  Type: {error_info['error_type']}")
        print(f"  Message: {error_info['error_message']}")
        print(f"  Context: {error_info['context']}")
        print(f"  Error code: {error_info['error_code']}")
        
        if simulation_state:
            print(f"  Simulation state: {simulation_state}")
        
        # Return error report for further processing
        return error_info
    
    def getErrorSummary(self):
        """Get error summary.
        
        Returns:
            Dictionary with error summary
        """
        if not self.error_details:
            return {
                'total_errors': 0,
                'error_types': {},
                'first_error': None,
                'last_error': None
            }
        
        # Count errors by type
        error_types = {}
        for error in self.error_details:
            error_type = error['error_type']
            error_types[error_type] = error_types.get(error_type, 0) + 1
        
        return {
            'total_errors': self.error_count,
            'error_types': error_types,
            'first_error': self.error_details[0]['timestamp'],
            'last_error': self.error_details[-1]['timestamp']
        }
    
    def generateReport(self):
        """Generate comprehensive error report.
        
        Returns:
            Comprehensive error report
        """
        summary = self.getErrorSummary()
        
        report = {
            'summary': summary,
            'errors': self.error_details,
            'recommendations': self._generateRecommendations()
        }
        
        return report
    
    def _generateRecommendations(self):
        """Generate recommendations based on errors."""
        recommendations = []
        
        # Analyze error patterns
        error_types = self.getErrorSummary()['error_types']
        
        if 'ConfigurationError' in error_types:
            recommendations.append(
                "Review configuration settings and validation rules"
            )
        
        if 'SimulationError' in error_types:
            recommendations.append(
                "Check simulation initialization and parameter values"
            )
        
        if 'ValidationError' in error_types:
            recommendations.append(
                "Review input validation and parameter ranges"
            )
        
        if len(self.error_details) > 5:
            recommendations.append(
                "High error rate detected - review system configuration and parameters"
            )
        
        return recommendations

# Use error reporter
error_reporter = ErrorReporter()

# Example of error reporting in simulation
def simulateWithErrorReporting(steps=100):
    """Run simulation with error reporting."""
    try:
        # Create simulation systems
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        world = nlm.create_simple_world()
        world.configure(20, 20, 64, 64)
        world.setAgentStart(10, 10)
        
        brain.initialize(world)
        
        agent_brain = nlm.create_agent_brain(brain)
        agent_brain.initialize(brain, world)
        
        # Run simulation
        for step in range(steps):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            result = world.applyMotorCommand(action, step)
            
            # Check for errors
            if not result.wasSuccessful():
                error_reporter.reportError(
                    NLMError(f"Action failed at step {step}: {result.getError()}"),
                    context=f"action_application_step_{step}",
                    simulation_state={
                        'step': step,
                        'neurons_active': brain.getActiveNeuronCount(),
                        'agent_position': world.getAgentBody().position,
                        'action': action
                    }
                )
            
            # Update world
            world.update(0.001)
        
        print(f"Simulation completed successfully with {steps} steps")
        return True
        
    except Exception as e:
        error_reporter.reportError(
            e,
            context="simulation_execution",
            simulation_state={
                'steps_completed': step if 'step' in locals() else 0,
                'brain_stats': brain.getStatistics() if 'brain' in locals() else None,
                'world_state': world.getAgentBody() if 'world' in locals() else None
            }
        )
        
        # Generate error report
        report = error_reporter.generateReport()
        
        print(f"\nError Report Generated:")
        print(f"  Total errors: {report['summary']['total_errors']}")
        print(f"  Error types: {report['summary']['error_types']}")
        print(f"  Recommendations: {report['recommendations']}")
        
        return False

# Run simulation with error reporting
success = simulateWithErrorReporting(100)
```

## Troubleshooting Guide

### Step-by-Step Debugging

#### Basic Debugging Steps

1. **Reproduce the Problem**
   - Create minimal test case
   - Verify consistent reproduction
   - Document the issue

2. **Gather Information**
   - Collect error messages
   - Note system configuration
   - Record simulation parameters
   - Document steps leading to error

3. **Isolate the Problem**
   - Test individual components
   - Remove potential causes one by one
   - Use simplified test cases

4. **Form Hypotheses**
   - Identify potential causes
   - Prioritize likely causes
   - Plan systematic testing

5. **Test Hypotheses**
   - Create tests for each hypothesis
   - Use controlled experiments
   - Document results

6. **Implement Fix**
   - Apply fix to root cause
   - Test fix thoroughly
   - Verify no regression

7. **Document Solution**
   - Document the problem and solution
   - Update relevant documentation
   - Share with team

#### Example: Step-by-Step Debugging

```python
"""Example of step-by-step debugging"""

def debugNeuronActivityIssue():
    """Debug neuron activity issue."""
    
    print("=== Step 1: Reproduce the Problem ===")
    
    # Create minimal test case
    config = nlm.create_default_config()
    config.set('brain.simulated_neurons', 100)  # Small simulation
    
    brain = nlm.create_brain(config)
    world = nlm.create_simple_world()
    world.configure(10, 10, 32, 32)
    world.setAgentStart(5, 5)
    
    brain.initialize(world)
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    # Run simulation and check neuron activity
    for step in range(50):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, step)
        world.update(0.001)
        
        neurons_active = brain.getActiveNeuronCount()
        
        if neurons_active < 10:
            print(f"Neuron activity issue at step {step}: {neurons_active} active neurons")
            break
    
    print("\n=== Step 2: Gather Information ===")
    print(f"  Configuration: {config}")
    print(f"  Brain stats: {brain.getStatistics()}")
    print(f"  Agent brain: {agent_brain}")
    print(f"  World size: {world.width}x{world.height}")
    
    # Check configuration parameters
    print("\nConfiguration parameters:")
    for key, value in config.items():
        print(f"  {key}: {value}")
    
    print("\n=== Step 3: Isolate the Problem ===")
    print("Testing different configuration parameters...")
    
    # Test different neuron counts
    for neuron_count in [50, 100, 200, 500]:
        test_config = nlm.create_default_config()
        test_config.set('brain.simulated_neurons', neuron_count)
        
        test_brain = nlm.create_brain(test_config)
        test_world = nlm.create_simple_world()
        test_world.configure(10, 10, 32, 32)
        test_world.setAgentStart(5, 5)
        
        test_brain.initialize(test_world)
        test_agent_brain = nlm.create_agent_brain(test_brain)
        test_agent_brain.initialize(test_brain, test_world)
        
        # Run quick simulation
        for step in range(10):
            percept = test_world.getSensoryPercept()
            test_agent_brain.processSensoryInput(percept)
            action = test_agent_brain.decodeMotorCommand()
            test_world.applyMotorCommand(action, step)
            test_world.update(0.001)
        
        neurons_active = test_brain.getActiveNeuronCount()
        print(f"  Neurons {neuron_count}: {neurons_active} active neurons")
    
    print("\n=== Step 4: Form Hypotheses ===")
    hypotheses = [
        "Neuron count too low for activity",
        "World configuration too small",
        "Agent brain not properly initialized",
        "Sensory processing not working correctly",
        "Action application not affecting world"
    ]
    
    print("Possible hypotheses:")
    for i, hypothesis in enumerate(hypotheses, 1):
        print(f"  {i}. {hypothesis}")
    
    print("\n=== Step 5: Test Hypotheses ===")
    
    # Test hypothesis 1: Increase neuron count
    print("\nTesting hypothesis 1: Increase neuron count...")
    large_config = nlm.create_default_config()
    large_config.set('brain.simulated_neurons', 1000)
    
    large_brain = nlm.create_brain(large_config)
    large_world = nlm.create_simple_world()
    large_world.configure(20, 20, 64, 64)
    large_world.setAgentStart(10, 10)
    
    large_brain.initialize(large_world)
    large_agent_brain = nlm.create_agent_brain(large_brain)
    large_agent_brain.initialize(large_brain, large_world)
    
    # Run simulation
    for step in range(50):
        percept = large_world.getSensoryPercept()
        large_agent_brain.processSensoryInput(percept)
        action = large_agent_brain.decodeMotorCommand()
        large_world.applyMotorCommand(action, step)
        large_world.update(0.001)
    
    neurons_active = large_brain.getActiveNeuronCount()
    print(f"  Result with 1000 neurons: {neurons_active} active neurons")
    
    if neurons_active > 50:
        print("  Hypothesis 1 confirmed - insufficient neurons")
        return "Solution: Increase neuron count in configuration"
    
    # Test hypothesis 2: Larger world
    print("\nTesting hypothesis 2: Larger world...")
    world_config = nlm.create_grid_world_config()
    world_config.set('world.width', 30)
    world_config.set('world.height', 30)
    
    brain = nlm.create_brain(world_config)
    world = nlm.create_simple_world()
    world.configure(30, 30, 64, 64)
    world.setAgentStart(15, 15)
    
    brain.initialize(world)
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    # Run simulation
    for step in range(50):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, step)
        world.update(0.001)
    
    neurons_active = brain.getActiveNeuronCount()
    print(f"  Result with larger world: {neurons_active} active neurons")
    
    if neurons_active > 50:
        print("  Hypothesis 2 confirmed - world size was too small")
        return "Solution: Increase world size in configuration"
    
    print("\n=== Step 6: Implement Fix ===")
    print("Fix: Update configuration with appropriate parameters")
    
    # Create fixed configuration
    fixed_config = nlm.create_default_config()
    fixed_config.set('brain.simulated_neurons', 500)
    fixed_config.set('world.width', 20)
    fixed_config.set('world.height', 20)
    
    print("Fixed configuration:")
    print(f"  Neurons: {fixed_config.get('brain.simulated_neurons')}")
    print(f"  World size: {fixed_config.get('world.width')}x{fixed_config.get('world.height')}")
    
    return "Configuration fixed"

# Run debugging
result = debugNeuronActivityIssue()
print(f"\n=== Debugging Result ===")
print(f"  {result}")
```

### Common Error Messages

#### Configuration Errors

- `"brain.simulated_neurons must be a positive integer"`
- `"world.width must be between 1 and 100"`
- `"neuromodulation.dopamine_influence must be between 0 and 1"`

#### Simulation Errors

- `"Brain not initialized"`
- `"Invalid action type"`
- `"World boundary exceeded"`
- `"Neuron index out of range"`

#### Performance Errors

- `"Memory pool allocation failed"`
- `"SIMD operation not supported"`
- `"Parallel processing unavailable"`
- `"Memory usage exceeded limit"`

### Solutions and Workarounds

#### Quick Fixes

1. **Increase Resource Limits**
```python
# Fix memory issues
config = nlm.create_high_performance_config()

# Fix performance issues
config = nlm.create_minimal_config()
```

2. **Adjust Simulation Parameters**
```python
# Fix simulation issues
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)
config.set('world.height', 20)
```

3. **Use Alternative Methods**
```python
# Use alternative action methods
action = nlm.Action.createRestAction()  # Instead of move action
```

#### Long-Term Solutions

1. **Configuration Optimization**
```python
# Optimize configuration for specific use case
if neuron_count > 10000:
    config = nlm.create_high_performance_config()
else:
    config = nlm.create_default_config()
```

2. **Performance Optimization**
```python
# Optimize performance with expert APIs
from nlm import expert

pool = expert.MemoryPool(8192, 65536)
processor = expert.SIMDProcessor(256)
```

3. **Error Handling**
```python
# Add comprehensive error handling
try:
    # Simulation code
    pass
except nlm.SimulationError as e:
    # Handle simulation errors
    pass
except nlm.ConfigurationError as e:
    # Handle configuration errors
    pass
```

## Testing and Validation

### Unit Testing

#### Unit Testing Framework

```python
import unittest
import nlm
from nlm import expert

class TestNLMCore(unittest.TestCase):
    """Unit tests for NLM core functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = nlm.create_default_config()
        self.world = nlm.create_simple_world()
        self.world.configure(20, 20, 64, 64)
        self.world.setAgentStart(10, 10)
        
        self.brain = nlm.create_brain(self.config)
        self.brain.initialize(self.world)
        
        self.agent_brain = nlm.create_agent_brain(self.brain)
        self.agent_brain.initialize(self.brain, self.world)
    
    def tearDown(self):
        """Clean up test fixtures."""
        # Clean up resources
        del self.agent_brain
        del self.brain
        del self.world
        del self.config
    
    def testBrainInitialization(self):
        """Test brain initialization."""
        self.assertIsNotNone(self.brain)
        stats = self.brain.getStatistics()
        self.assertGreater(stats['total_neurons'], 0)
    
    def testWorldConfiguration(self):
        """Test world configuration."""
        self.assertEqual(self.world.width, 20)
        self.assertEqual(self.world.height, 20)
        self.assertEqual(self.world.view_distance, 64)
    
    def testAgentInitialization(self):
        """Test agent brain initialization."""
        self.assertIsNotNone(self.agent_brain)
        body = self.world.getAgentBody()
        self.assertIsNotNone(body)
    
    def testSensoryProcessing(self):
        """Test sensory processing."""
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Process sensory input
        self.agent_brain.processSensoryInput(percept)
        
        # Verify processing
        action = self.agent_brain.decodeMotorCommand()
        self.assertIsNotNone(action)
    
    def testActionApplication(self):
        """Test action application."""
        body_before = self.world.getAgentBody()
        
        # Generate and apply action
        action = self.agent_brain.decodeMotorCommand()
        result = self.world.applyMotorCommand(action, 0)
        
        # Verify result
        self.assertTrue(result.wasSuccessful())
        
        body_after = self.world.getAgentBody()
        # Position may or may not change depending on action

class TestNLMExpert(unittest.TestCase):
    """Unit tests for NLM expert APIs."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.memory_pool = expert.MemoryPool(1024, 8192)
        self.simd_processor = expert.SIMDProcessor(256)
        self.parallel_processor = expert.ParallelProcessor(4)
    
    def tearDown(self):
        """Clean up test fixtures."""
        # Clean up resources
        pass
    
    def testMemoryPool(self):
        """Test memory pool functionality."""
        # Allocate memory
        block = self.memory_pool.allocate(1024)
        self.assertIsNotNone(block)
        
        # Free memory
        self.memory_pool.free(block)
        
        # Get statistics
        stats = self.memory_pool.getStats()
        self.assertIn('allocations', stats)
    
    def testSIMDProcessor(self):
        """Test SIMD processor functionality."""
        # Test vector processing
        data = [1.0, 2.0, 3.0, 4.0]
        operation = lambda x: [v * 2 for v in x]
        
        result = self.simd_processor.process_vector(data, operation)
        self.assertEqual(len(result), len(data))
        
        # Verify results
        for i, (input_val, output_val) in enumerate(zip(data, result)):
            self.assertEqual(output_val, input_val * 2)
    
    def testParallelProcessor(self):
        """Test parallel processor functionality."""
        # Test data
        data = list(range(100))
        
        # Process with parallel processor
        results = self.parallel_processor.distribute_work(
            lambda chunk: [x * 2 for x in chunk],
            data
        )
        
        # Verify results
        self.assertEqual(len(results), len(data))
        for i, (input_val, output_val) in enumerate(zip(data, results)):
            self.assertEqual(output_val, input_val * 2)

# Run unit tests
if __name__ == '__main__':
    unittest.main()
```

### Integration Testing

#### Integration Testing Framework

```python
"""Integration testing for NLM"""
import unittest
import nlm
from nlm import expert
import time

class TestNLMIntegration(unittest.TestCase):
    """Integration tests for NLM."""
    
    def setUp(self):
        """Set up integration test environment."""
        self.config = nlm.create_high_performance_config()
        self.world = nlm.create_simple_world()
        self.world.configure(30, 30, 128, 128)
        self.world.setAgentStart(15, 15)
        
        self.brain = nlm.create_brain(self.config)
        self.brain.initialize(self.world)
        
        self.agent_brain = nlm.create_agent_brain(self.brain)
        self.agent_brain.initialize(self.brain, self.world)
        
        # Setup expert systems
        self.memory_pool = expert.MemoryPool(8192, 65536)
        self.simd_processor = expert.SIMDProcessor(512)
        self.parallel_processor = expert.ParallelProcessor(8)
    
    def tearDown(self):
        """Clean up integration test environment."""
        # Clean up resources
        del self.agent_brain
        del self.brain
        del self.world
        del self.config
        del self.memory_pool
        del self.simd_processor
        del self.parallel_processor
    
    def testEndToEndSimulation(self):
        """Test end-to-end simulation."""
        # Run simulation
        start_time = time.time()
        
        for step in range(100):
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input with expert optimizations
            processed_percept = self.simd_processor.process_vector(
                percept, lambda x: [v * 0.5 for v in x]
            )
            
            # Process with agent brain
            self.agent_brain.processSensoryInput(processed_percept)
            
            # Generate action with parallel processing
            action = self.parallel_processor.distribute_work(
                self.agent_brain.decodeMotorCommand, [self.agent_brain]
            )[0]
            
            # Apply action
            result = self.world.applyMotorCommand(action, step)
            self.assertTrue(result.wasSuccessful())
            
            # Update world
            self.world.update(0.001)
        
        end_time = time.time()
        execution_time = end_time - start_time
        
        # Verify results
        self.assertLess(execution_time, 30.0)  # Should complete within 30 seconds
        
        stats = self.brain.getStatistics()
        self.assertGreater(stats['total_neurons'], 0)
        self.assertGreater(stats['active_neurons'], 0)
    
    def testExpertIntegration(self):
        """Test expert APIs integration."""
        # Test memory pool integration
        block = self.memory_pool.allocate(1024)
        self.assertIsNotNone(block)
        self.memory_pool.free(block)
        
        # Test SIMD integration
        data = [1.0, 2.0, 3.0, 4.0]
        processed = self.simd_processor.process_vector(
            data, lambda x: [v * 0.5 for v in x]
        )
        self.assertEqual(len(processed), len(data))
        
        # Test parallel integration
        results = self.parallel_processor.distribute_work(
            lambda chunk: [x * 2 for x in chunk],
            list(range(50))
        )
        self.assertEqual(len(results), 50)
    
    def testPerformanceMonitoring(self):
        """Test performance monitoring integration."""
        # Create performance monitor
        monitor = nlm.create_performance_monitor()
        
        # Record simulation metrics
        for step in range(50):
            neurons_active = self.brain.getActiveNeuronCount()
            monitor.recordStep(neurons_active, 0, 0.001)
        
        # Get performance report
        report = monitor.getPerformanceReport()
        
        # Verify report
        self.assertIn('simulation_speed', report)
        self.assertIn('avg_iteration_time', report)
        self.assertIn('avg_active_neurons', report)
        
        self.assertGreater(report['simulation_speed'], 0)
        self.assertLess(report['avg_iteration_time'], 1.0)
        self.assertGreater(report['avg_active_neurons'], 0)

# Run integration tests
if __name__ == '__main__':
    unittest.main()
```

### Performance Testing

#### Performance Testing Framework

```python
"""Performance testing for NLM"""
import unittest
import time
import tracemalloc
import nlm
from nlm import expert

class TestNLMPerformance(unittest.TestCase):
    """Performance tests for NLM."""
    
    def testMemoryUsage(self):
        """Test memory usage."""
        tracemalloc.start()
        
        # Take initial snapshot
        snapshot1 = tracemalloc.take_snapshot()
        
        # Run memory-intensive operations
        for i in range(100):
            data = list(range(10000))
            result = sum(data) / len(data) if data else 0
            del data
            del result
        
        # Take final snapshot
        snapshot2 = tracemalloc.take_snapshot()
        
        # Compare snapshots
        top_stats = snapshot2.compare_to(snapshot1, 'lineno')
        
        # Check for memory leaks
        total_memory_change = sum(stat.size_diff for stat in top_stats)
        self.assertLess(total_memory_change, 1024 * 1024)  # Less than 1MB change
        
        tracemalloc.stop()
    
    def testSimulationSpeed(self):
        """Test simulation speed."""
        # Create configuration
        config = nlm.create_high_performance_config()
        config.set('brain.simulated_neurons', 5000)
        
        # Create simulation systems
        world = nlm.create_simple_world()
        world.configure(20, 20, 64, 64)
        world.setAgentStart(10, 10)
        
        brain = nlm.create_brain(config)
        brain.initialize(world)
        
        agent_brain = nlm.create_agent_brain(brain)
        agent_brain.initialize(brain, world)
        
        # Run simulation
        start_time = time.time()
        
        for step in range(200):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, step)
            world.update(0.001)
        
        end_time = time.time()
        execution_time = end_time - start_time
        
        # Verify performance
        steps_per_second = 200 / execution_time
        self.assertGreater(steps_per_second, 100)  # Should process at least 100 steps/second
    
    def testExpertPerformance(self):
        """Test expert APIs performance."""
        # Create expert systems
        memory_pool = expert.MemoryPool(8192, 65536)
        simd_processor = expert.SIMDProcessor(512)
        parallel_processor = expert.ParallelProcessor(8)
        
        # Test data
        data = list(range(10000))
        
        # Test SIMD performance
        start_time = time.time()
        for _ in range(100):
            processed = simd_processor.process_vector(
                data, lambda x: [v * 0.5 for v in x]
            )
        simd_time = time.time() - start_time
        
        # Test parallel performance
        start_time = time.time()
        results = parallel_processor.distribute_work(
            lambda chunk: [x * 2 for x in chunk],
            data
        )
        parallel_time = time.time() - start_time
        
        # Verify performance
        self.assertLess(simd_time, 1.0)  # SIMD processing should be fast
        self.assertLess(parallel_time, 1.0)  # Parallel processing should be fast
        
        # Verify results
        self.assertEqual(len(results), len(data))
    
    def testScalability(self):
        """Test scalability with different neuron counts."""
        neuron_counts = [1000, 5000, 10000, 20000]
        
        for neuron_count in neuron_counts:
            # Create configuration
            config = nlm.create_high_performance_config()
            config.set('brain.simulated_neurons', neuron_count)
            
            # Create simulation systems
            world = nlm.create_simple_world()
            world.configure(20, 20, 64, 64)
            world.setAgentStart(10, 10)
            
            brain = nlm.create_brain(config)
            brain.initialize(world)
            
            agent_brain = nlm.create_agent_brain(brain)
            agent_brain.initialize(brain, world)
            
            # Run simulation
            start_time = time.time()
            
            for step in range(100):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent_brain.processSensoryInput(percept)
                
                # Generate action
                action = agent_brain.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, step)
                world.update(0.001)
            
            end_time = time.time()
            execution_time = end_time - start_time
            
            # Verify scalability
            steps_per_second = 100 / execution_time
            self.assertGreater(steps_per_second, 50)  # Should process at least 50 steps/second

# Run performance tests
if __name__ == '__main__':
    unittest.main()
```

### Load Testing

#### Load Testing Framework

```python
"""Load testing for NLM"""
import unittest
import threading
import time
import nlm
from nlm import expert
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

class TestNLMLoad(unittest.TestCase):
    """Load tests for NLM."""
    
    def testConcurrentSimulations(self):
        """Test concurrent simulations."""
        # Configuration
        config = nlm.create_high_performance_config()
        config.set('brain.simulated_neurons', 2000)
        
        # Number of concurrent simulations
        num_simulations = 5
        
        # Run concurrent simulations
        start_time = time.time()
        
        with ThreadPoolExecutor(max_workers=num_simulations) as executor:
            # Submit simulation tasks
            futures = []
            for i in range(num_simulations):
                future = executor.submit(self.runSingleSimulation, config)
                futures.append(future)
            
            # Wait for all simulations to complete
            results = []
            for future in futures:
                try:
                    result = future.result(timeout=300.0)  # 5 minute timeout
                    results.append(result)
                except Exception as e:
                    self.fail(f"Simulation failed: {e}")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # Verify results
        self.assertEqual(len(results), num_simulations)
        self.assertGreater(total_time, 0)
        
        # Calculate throughput
        total_steps = sum(r['steps_completed'] for r in results)
        steps_per_second = total_steps / total_time
        self.assertGreater(steps_per_second, 100)  # Should process at least 100 steps/second
    
    def runSingleSimulation(self, config):
        """Run a single simulation.
        
        Args:
            config: Simulation configuration
            
        Returns:
            Simulation results
        """
        # Create simulation systems
        world = nlm.create_simple_world()
        world.configure(20, 20, 64, 64)
        world.setAgentStart(10, 10)
        
        brain = nlm.create_brain(config)
        brain.initialize(world)
        
        agent_brain = nlm.create_agent_brain(brain)
        agent_brain.initialize(brain, world)
        
        # Run simulation
        for step in range(50):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, step)
            world.update(0.001)
        
        return {
            'steps_completed': 50,
            'final_neurons': brain.getActiveNeuronCount(),
            'world_position': world.getAgentBody().position
        }
    
    def testProcessScaling(self):
        """Test process-based scaling."""
        # Configuration
        config = nlm.create_high_performance_config()
        config.set('brain.simulated_neurons', 3000)
        
        # Number of processes
        num_processes = 4
        
        # Run process-based simulations
        start_time = time.time()
        
        with ProcessPoolExecutor(max_workers=num_processes) as executor:
            # Submit simulation tasks
            futures = []
            for i in range(num_processes):
                future = executor.submit(self.runSingleSimulation, config)
                futures.append(future)
            
            # Wait for all simulations to complete
            results = []
            for future in futures:
                try:
                    result = future.result(timeout=300.0)  # 5 minute timeout
                    results.append(result)
                except Exception as e:
                    self.fail(f"Simulation failed: {e}")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # Verify results
        self.assertEqual(len(results), num_processes)
        self.assertGreater(total_time, 0)
        
        # Calculate throughput
        total_steps = sum(r['steps_completed'] for r in results)
        steps_per_second = total_steps / total_time
        self.assertGreater(steps_per_second, 50)  # Should process at least 50 steps/second
    
    def testMemoryScaling(self):
        """Test memory scaling with larger simulations."""
        import tracemalloc
        
        neuron_counts = [1000, 5000, 10000, 20000]
        
        for neuron_count in neuron_counts:
            tracemalloc.start()
            
            # Create configuration
            config = nlm.create_high_performance_config()
            config.set('brain.simulated_neurons', neuron_count)
            
            # Create simulation systems
            world = nlm.create_simple_world()
            world.configure(20, 20, 64, 64)
            world.setAgentStart(10, 10)
            
            brain = nlm.create_brain(config)
            brain.initialize(world)
            
            agent_brain = nlm.create_agent_brain(brain)
            agent_brain.initialize(brain, world)
            
            # Run simulation
            for step in range(50):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent_brain.processSensoryInput(percept)
                
                # Generate action
                action = agent_brain.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, step)
                world.update(0.001)
            
            # Get memory usage
            current, peak = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            
            # Convert to MB
            peak_mb = peak / 1024 / 1024
            
            # Verify memory usage scales appropriately
            # For 1000 neurons: < 50MB
            # For 5000 neurons: < 200MB
            # For 10000 neurons: < 400MB
            # For 20000 neurons: < 800MB
            
            if neuron_count == 1000:
                self.assertLess(peak_mb, 50)
            elif neuron_count == 5000:
                self.assertLess(peak_mb, 200)
            elif neuron_count == 10000:
                self.assertLess(peak_mb, 400)
            elif neuron_count == 20000:
                self.assertLess(peak_mb, 800)

# Run load tests
if __name__ == '__main__':
    unittest.main()
```

## Documentation and Resources

### Documentation References

#### Key Documentation Files

- **Main Documentation**: `/docs/README.md` - Complete project documentation
- **API Reference**: `/docs/API_REFERENCE.md` - Detailed API documentation
- **Tutorials**: `/docs/TUTORIAL.md` - Step-by-step tutorials
- **Performance Guide**: `/docs/PERFORMANCE.md` - Performance optimization guide
- **Debugging Guide**: `/docs/DEBUGGING.md` - Troubleshooting and debugging guide
- **Examples**: `/examples/` - Example scripts and demos
- **Configuration**: `/configs/` - Configuration examples

#### Documentation Structure

```
docs/
├── README.md                    # Main documentation
├── API_REFERENCE.md              # API documentation
├── TUTORIAL.md                   # Tutorials
├── PERFORMANCE.md                # Performance guide
├── DEBUGGING.md                   # Debugging guide
├── MIGRATION.md                   # Migration guide
└── examples/                     # Example scripts
    ├── basic_demo.py
    ├── expert_demo.py
    ├── performance_demo.py
    ├── visualization_demo.py
    ├── research_demo.py
    └── production_demo.py

configs/
├── recommended.json
├── performance_tuned.json
└── research_setup.json
```

### Community Support

#### Support Channels

- **Documentation**: [NLM Documentation](https://github.com/Kilo-Org/kilocode/tree/main/docs)
- **Issue Tracker**: [GitHub Issues](https://github.com/Kilo-Org/kilocode/issues)
- **Discussion Forum**: [GitHub Discussions](https://github.com/Kilo-Org/kilocode/discussions)
- **Slack**: [NLM Slack Channel](https://slack.kilo.ai)
- **Discord**: [NLM Discord Server](https://discord.kilo.ai)

#### Support Resources

1. **Getting Started Guide**: `/docs/README.md` - Complete getting started guide
2. **API Documentation**: `/docs/API_REFERENCE.md` - Detailed API reference
3. **Tutorial Series**: `/docs/TUTORIAL.md` - Step-by-step tutorials
4. **Performance Optimization**: `/docs/PERFORMANCE.md` - Performance optimization guide
5. **Debugging Guide**: `/docs/DEBUGGING.md` - Troubleshooting and debugging
6. **Migration Guide**: `/docs/MIGRATION.md` - Migration from previous versions

### Issue Tracking

#### Issue Categories

1. **Bug Reports**: Actual bugs or unexpected behavior
2. **Feature Requests**: New features or enhancements
3. **Documentation Issues**: Problems with documentation
4. **Performance Issues**: Performance problems or optimization opportunities
5. **Compatibility Issues**: Problems with different environments or versions

#### Issue Reporting

```bash
# Create a new issue
gh issue create --title "Issue title" --body "Issue description"

# Example issue template
## Bug Report

### Description
[Description of the bug]

### Steps to Reproduce
1. [Step 1]
2. [Step 2]
3. [Step 3]

### Expected Behavior
[Expected behavior]

### Actual Behavior
[Actual behavior]

### Environment
- NLM version: [version]
- Python version: [version]
- Operating system: [version]
- Configuration: [description]

### Logs
[Logs or error messages]

## Additional Context
[Any additional context or information]
```

## Best Practices for Debugging

### Systematic Debugging Approach

1. **Define the Problem Clearly**
   - Identify specific issue
   - Document all known information
   - Note environment and configuration

2. **Create Reproducible Test Case**
   - Minimal reproduction
   - Consistent results
   - Isolated testing

3. **Use Appropriate Tools**
   - Select right debugging tools
   - Configure tools properly
   - Use tool features effectively

4. **Document Everything**
   - Record all findings
   - Document hypotheses
   - Track testing results

5. **Follow Up on Fixes**
   - Verify fix effectiveness
   - Test edge cases
   - Monitor for regressions

### Minimal Reproduction Examples

#### Example 1: Neuron Activity Issue

```python
"""Minimal reproduction of neuron activity issue"""
import nlm

# Create minimal test case
config = nlm.create_default_config()
config.set('brain.simulated_neurons', 100)  # Small simulation

brain = nlm.create_brain(config)
world = nlm.create_simple_world()
world.configure(10, 10, 32, 32)
world.setAgentStart(5, 5)

brain.initialize(world)
agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(brain, world)

# Test neuron activity
print(f"Initial neurons: {brain.getStatistics()['total_neurons']}")

# Run simulation
for step in range(20):
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, step)
    world.update(0.001)
    
    neurons_active = brain.getActiveNeuronCount()
    print(f"Step {step}: {neurons_active} active neurons")
    
    if neurons_active < 5:
        print(f"WARNING: Low neuron activity at step {step}")
        break
```

#### Example 2: Configuration Issue

```python
"""Minimal reproduction of configuration issue"""
import nlm

# Test different configuration values
for neuron_count in [100, 500, 1000, 5000]:
    config = nlm.create_default_config()
    config.set('brain.simulated_neurons', neuron_count)
    
    brain = nlm.create_brain(config)
    world = nlm.create_simple_world()
    world.configure(10, 10, 32, 32)
    world.setAgentStart(5, 5)
    
    brain.initialize(world)
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    # Quick test
    for step in range(5):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, step)
        world.update(0.001)
    
    neurons_active = brain.getActiveNeuronCount()
    print(f"Neurons {neuron_count}: {neurons_active} active")
```

### Version Compatibility

#### Compatibility Checking

```python
import nlm

# Check NLM version
print(f"NLM version: {nlm.__version__}")

# Check Python version
import sys
print(f"Python version: {sys.version}")

# Check for required features
if hasattr(nlm, 'expert'):
    print("Expert APIs available")
else:
    print("Expert APIs not available")

# Check configuration compatibility
config = nlm.create_default_config()
try:
    config.validate()
    print("Configuration validation: OK")
except Exception as e:
    print(f"Configuration validation: FAILED - {e}")
```

### Configuration Management

#### Configuration Validation

```python
from nlm import ConfigurationError

def validateConfiguration(config):
    """Validate configuration parameters."""
    try:
        config.validate()
        print("Configuration is valid")
        return True
    except ConfigurationError as e:
        print(f"Configuration error: {e}")
        return False

# Test validation
config = nlm.create_default_config()
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)

if validateConfiguration(config):
    print("Configuration valid - ready to use")
else:
    print("Configuration invalid - fix issues")
```

## Conclusion

### Importance of Debugging

Debugging is essential for:

1. **Development Efficiency**: Quickly identify and fix issues
2. **Production Stability**: Resolve issues in deployed systems
3. **Performance Optimization**: Identify and eliminate bottlenecks
4. **User Experience**: Provide timely support
5. **System Reliability**: Ensure consistent operation

### Key Takeaways

1. **Systematic Approach**: Follow a systematic debugging process
2. **Documentation**: Document everything for future reference
3. **Tools**: Use appropriate debugging tools
4. **Testing**: Create comprehensive tests
5. **Collaboration**: Share findings with team

### Next Steps

1. **Practice Debugging**: Apply debugging techniques to real issues
2. **Contribute**: Share debugging solutions with community
3. **Improve**: Enhance debugging tools and processes
4. **Educate**: Teach debugging best practices
5. **Maintain**: Keep documentation updated

### Acknowledgments

This debugging guide was created to help users effectively troubleshoot NLM issues and optimize their simulations.

## License

MIT License. See LICENSE file for details.

## Author

NLM Team
