# NLM - Troubleshooting Guide

## Overview

This document provides comprehensive troubleshooting guidance for the enhanced NLM (Neural Learning Machine) project. It covers common issues, solutions, error message explanations, debugging techniques, and performance troubleshooting to help users resolve problems efficiently.

## Table of Contents

- [Common Issues and Solutions](#common-issues-and-solutions)
  - [Issue 1: "Brain not initialized" Error](#issue-1-brain-not-initialized-error)
  - [Issue 2: "High memory usage" Error](#issue-2-high-memory-usage-error)
  - [Issue 3: "Simulation too slow" Error](#issue-3-simulation-too-slow-error)
  - [Issue 4: "Agent not moving" Error](#issue-4-agent-not-moving-error)
  - [Issue 5: "Configuration validation failed" Error](#issue-5-configuration-validation-failed-error)

- [Error Message Explanations](#error-message-explanations)
  - [Configuration Errors](#configuration-errors)
  - [Brain Simulation Errors](#brain-simulation-errors)
  - [Performance Errors](#performance-errors)
  - [Memory Errors](#memory-errors)

- [Debugging Techniques](#debugging-techniques)
  - [Step-by-Step Debugging](#step-by-step-debugging)
  - [Error Tracking](#error-tracking)
  - [Performance Profiling](#performance-profiling)
  - [State Inspection](#state-inspection)

- [Performance Troubleshooting](#performance-troubleshooting)
  - [CPU Performance Issues](#cpu-performance-issues)
  - [Memory Performance Issues](#memory-performance-issues)
  - [I/O Performance Issues](#io-performance-issues)

## Common Issues and Solutions

### Issue 1: "Brain not initialized" Error

**Symptoms:**
- Simulation fails with "Brain not initialized" error
- Brain methods return default values
- Simulation produces no output

**Solutions:**
1. **Ensure proper initialization sequence:**
   ```python
   brain = createBrain(config)
   brain.initialize()  # Always call this first
   brain.step(0, 0.0)  # Then start simulation
   ```

2. **Check configuration validity:**
   ```python
   if not config.validate():
       print("Configuration has validation errors")
       config.loadFromFile("backup_config.json")
       brain.initialize()  # Re-initialize with valid config
   ```

3. **Debug initialization:**
   ```python
   # Add debug logging to brain.initialize()
   # Check if neurons are created properly
   # Verify synaptic connections
   ```

### Issue 2: "High memory usage" Error

**Symptoms:**
- Simulation crashes with memory error
- System slows down during simulation
- High memory usage in task manager

**Solutions:**
1. **Reduce network size:**
   ```python
   config.set("brain.neuron_count", 1000)  # Start with smaller network
   ```

2. **Use memory-efficient formats:**
   ```python
   # Use float32 instead of float64
   # Use compressed data formats
   # Batch operations instead of real-time processing
   ```

3. **Implement memory management:**
   ```python
   from python.nlm_advanced.performance import MemoryManager
   memory_manager = MemoryManager()
   memory_manager.start_monitoring()
   ```

### Issue 3: "Simulation too slow" Error

**Symptoms:**
- Simulation runs too slowly
- CPU usage is high but progress is slow
- Simulation takes too long to complete

**Solutions:**
1. **Reduce simulation complexity:**
   ```python
   config.set("brain.neuron_count", 1000)
   config.set("brain.synapse_density", 0.05)
   ```

2. **Use performance optimizations:**
   ```python
   from python.nlm_advanced.performance import PerformanceOptimizer
   optimizer = PerformanceOptimizer()
   optimizer.optimize_simulation()
   ```

3. **Implement parallel processing:**
   ```python
   from python.nlm_advanced.batch import ParallelExecutor
   executor = ParallelExecutor(num_workers=4)
   ```

### Issue 4: "Agent not moving" Error

**Symptoms:**
- Agent doesn't move in the world
- Agent actions are not applied
- World state remains unchanged

**Solutions:**
1. **Check action decoding:**
   ```python
   # Ensure brain has motor neurons
   action = agent.decodeMotorCommand()
   if action is None:
       print("No motor action decoded")
       return
   ```

2. **Verify world application:**
   ```python
   # Check if action is valid
   if world.isValidPosition(action.x, action.y):
       world.applyMotorCommand(action, world.getSimulationTime())
   else:
       print("Invalid action position")
   ```

3. **Debug sensory processing:**
   ```python
   # Check if agent is processing sensory input
   percept = world.getSensoryPercept()
   agent.processSensoryInput(percept)
   ```

### Issue 5: "Configuration validation failed" Error

**Symptoms:**
- Configuration validation fails
- Simulation stops before starting
- Error messages are unclear

**Solutions:**
1. **Get specific validation errors:**
   ```python
   if not config.validate():
       errors = config.get_validation_errors()
       print("Validation errors:")
       for error in errors:
           print(f"  - {error}")
   ```

2. **Fix configuration:**
   ```python
   config.set("learning_rate", 0.01)  # Fix learning rate
   config.set("batch_size", 32)     # Fix batch size
   ```

3. **Use schema validation:**
   ```python
   # Add schema validation
   schema = create_config_schema()
   config.add_schema(schema)
   
   if not config.validate_schema():
       schema_errors = config.get_schema_validation_errors()
       print("Schema validation errors:")
       for error in schema_errors:
           print(f"  - {error}")
   ```

## Error Message Explanations

### Configuration Errors

| Error Type | Message | Solution |
|------------|---------|----------|
| `CONFIG001` | "Invalid neuron count" | Set valid range: 100-100000 |
| `CONFIG002` | "Learning rate must be positive" | Set learning_rate > 0 |
| `CONFIG003` | "Invalid configuration schema" | Check template compatibility |
| `CONFIG004` | "Missing required parameter" | Add missing parameters |
| `CONFIG005` | "Validation failed" | Fix configuration parameters |

### Brain Simulation Errors

| Error Type | Message | Solution |
|------------|---------|----------|
| `BRAIN001` | "Brain not initialized" | Call brain.initialize() first |
| `BRAIN002` | "Invalid simulation step" | Check step number range |
| `BREL003` | "Neural instability detected" | Adjust neuron parameters |
| `BRAIN004` | "Memory allocation failed" | Reduce network size |
| `BRAIN005` | "Simulation timeout" | Reduce simulation complexity |

### Performance Errors

| Error Type | Message | Solution |
|------------|---------|----------|
| `PERF001` | "CPU usage too high" | Reduce simulation complexity |
| `PERF002` | "Memory usage exceeded" | Enable memory optimization |
| `PERF003` | "Simulation too slow" | Enable performance profiling |
| `PEREX004` | "Load imbalance detected" | Use load balancer |

### Memory Errors

| Error Type | Message | Solution |
|------------|---------|----------|
| `MEM001` | "Memory allocation failed" | Reduce network size |
| `MEM002` | "Memory limit exceeded" | Implement memory management |
| `MEM003` | "Garbage collection failed" | Check memory usage |
| `MEM004` | "Out of memory" | Free unused memory |

## Debugging Techniques

### 1. Step-by-Step Debugging

```python
from python.nlm_advanced.debug import StepDebugger

def debug_simulation(brain, total_steps=100):
    """
    Debug simulation with step-by-step control.
n    
    Args:
        brain: Brain to debug
        total_steps: Total number of steps
        
    Returns:
        Debug information
    """
    
    # Create step debugger
    step_debugger = StepDebugger(brain)
    step_debugger.enable_debug_mode()
    
    # Add breakpoints
    step_debugger.add_breakpoint(10, message="Check initial state")
    step_debugger.add_breakpoint(50, message="Check for instability")
    step_debugger.add_breakpoint(100, message="Check for pattern emergence")
    
    # Run simulation with debugging
    for step in range(total_steps):
        time_val = step * ends_with step.debug_step(time_val)
        
        if not debug_info:
            print(f"Debug pause at step {step}")
            input("Press Enter to continue...")
            step_debugger.resume()
            continue
        
        # Run normal simulation
        brain.step(step, time_val)
    
    # Generate debug report
    debug_report = step_debugger.generate_debug_report()
    return debug_report
```

### 2. Error Tracking

```python
from python.nlm_advanced.debug import ErrorTracker

def track_simulation_errors(brain):
    """
    Track and analyze simulation errors.
    
    Args:
        brain: Brain to track errors for
        
    Returns:
        Error statistics
    """
    
    # Create error tracker
    error_tracker = ErrorTracker()
    
    # Setup error handlers
    def on_critical_error(error):
        print(f"Critical error: {error['message']}")
    
    def on_recurring_error(error):
        print(f"Recurring error pattern: {error['type']}")
    
    error_tracker.add_error_handler("CRITICAL", on_critical_error)
    error_tracker.add_error_handler("RECURRING", on_recurring_error)
    
    # Run simulation and track errors
    for step in range(1000):
        try:
            brain.step(step, step * 0.01)
        except Exception as e:
            error_tracker.log_error(e, f"step_{step}")
    
    # Get error statistics
    error_stats = error_tracker.get_error_statistics()
    return error_stats
```

### 3. Performance Profiling

```python
from python.nlm_advanced.debug import PerformanceProfiler

def profile_simulation(brain, total_steps=1000):
    """
    Profile simulation performance.
    
    Args:
        brain: Brain to profile
        total_steps: Total number of steps
        
    Returns:
        Performance report
    """
    
    # Create performance profiler
    profiler = PerformanceProfiler(brain)
    profiler.start_profiling()
    
    # Run simulation
    for step in range(total_steps):
        brain.step(step, step * 0.01)
    
    # Stop profiling
    profiler.stop_profiling()
    
    # Generate performance report
    performance_report = profiler.generate_performance_report()
    return performance_report
```

### 4. State Inspection

```python
from python.nlm_advanced.debug import StateInspector

def analyze_simulation_state(brain):
    """
    Analyze neural state during simulation.
    
    Args:
        brain: Brain to analyze
        
    Returns:
        State analysis
    """
    
    # Create state inspector
    inspector = StateInspector(brain)
    
    # Analyze current state
    state_analysis = inspector.analyze_state()
    
    # Get region information
    regions = brain.getRegions()
    for region_id, region in regions.items():
        print(f"Region {region_id}: {region.getActiveNeuronCount()} active neurons")
    
    # Get global statistics
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return state_analysis
```

## Performance Troubleshooting

### CPU Performance Issues

**Symptoms:**
- High CPU usage but slow progress
- Simulation takes too long to complete
- System appears unresponsive

**Solutions:**
1. **Enable performance profiling:**
   ```python
   from python.nlm_advanced.debug import PerformanceProfiler
   profiler = PerformanceProfiler(brain)
   profiler.start_profiling()
   ```

2. **Use load balancing:**
   ```python
   from python.nlm_advanced.performance import LoadBalancer
   load_balancer = LoadBalancer(num_workers=4)
   ```

3. **Optimize data structures:**
   ```python
   # Use numpy arrays instead of Python lists
   # Use vectorized operations
   # Cache frequently accessed values
   ```

### Memory Performance Issues

**Symptoms:**
- Simulation crashes with memory error
- System slows down during simulation
- High memory usage in task manager

**Solutions:**
1. **Enable memory management:**
   ```python
   from python.nlm_advanced.performance import MemoryManager
   memory_manager = MemoryManager()
   memory_manager.start_monitoring()
   ```

2. **Use efficient data formats:**
   ```python
   # Use HDF5 for large datasets
   # Use compression for storage
   # Use memory pools for frequently accessed objects
   ```

3. **Implement garbage collection:**
   ```python
   import gc
   gc.collect()  # Force garbage collection
   ```

### I/O Performance Issues

**Symptoms:**
- Slow simulation startup
- Slow result saving
- Large memory usage for I/O

**Solutions:**
1. **Batch I/O operations:**
   ```python
   # Write results in batches
   # Group related operations
   # Use efficient file formats
   ```

2. **Use efficient formats:**
   ```python
   import h5py
   # Use HDF5 format for large datasets
   # Use compression for storage
   ```

3. **Implement caching:**
   ```python
   # Cache frequently accessed files
   # Use memory-mapped files
   # Implement lazy loading
   ```

## Conclusion

The NLM troubleshooting guide provides comprehensive coverage of common issues, solutions, error explanations, debugging techniques, and performance troubleshooting. It includes:

1. **Common issues and solutions** for typical simulation problems
2. **Error message explanations** with specific solutions
3. **Debugging techniques** for step-by-step analysis
4. **Performance troubleshooting** for CPU, memory, and I/O issues

The guide helps users resolve problems efficiently and create robust, maintainable neural simulations. It covers the full range of troubleshooting scenarios from basic initialization issues to advanced performance problems.

For further assistance or questions, please refer to the specific documentation sections or contact the NLM support team.