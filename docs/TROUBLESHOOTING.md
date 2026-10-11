# NLM Troubleshooting Guide

## Overview

This guide provides comprehensive troubleshooting information for NLM (Neural Learning Machine) neural simulation framework. It covers common issues, their causes, and solutions to help users diagnose and resolve problems efficiently.

## Common Issues and Solutions

### 1. Simulation Crashes

#### Symptom: Simulation crashes with segmentation faults or exceptions

**Common Causes:**

1. **Memory allocation errors**
2. **Invalid configuration parameters**
3. **Null pointer dereferences**
4. **Array bounds violations**
5. **Incompatible C++ runtime**

**Diagnostic Steps:**

```bash
# Run with debugging symbols
g++ -g -O2 -std=c++20 -I./src -I./python src/brain/Brain.cpp src/brain/Neuron.cpp src/brain/Synapse.cpp

# Run with valgrind for memory errors
valgrind --tool=memcheck ./nlm_simulation

# Check for runtime errors
ulimit -c unlimited
./nlm_simulation 2>&1 | grep -i "segmentation\|access violation\|bus error"
```

**Solutions:**

1. **Memory Issues:**
   ```python
   # Increase memory limits
   import resource
   resource.setrlimit(resource.RLIMIT_AS, (4*1024*1024*1024, -1))  # 4GB
   
   # Use memory pools
   config.set("performance.enable_memory_pools", True)
   ```

2. **Configuration Issues:**
   ```python
   # Validate configuration
   config = pynlm.createDefaultConfig()
   assert config.has("brain.neuron_count")
   assert config.getOr("brain.neuron_count", 1000) > 0
   assert config.getOr("brain.synapse_density", 0.1) >= 0.0
   assert config.getOr("brain.synapse_density", 0.1) <= 1.0
   ```

3. **Simulation Issues:**
   ```python
   # Initialize brain properly
   brain = pynlm.createBrain(config)
   brain.initialize()  # Always call initialize!
   ```

### 2. Poor Performance

#### Symptom: Simulation runs slower than expected

**Common Causes:**

1. **Large neuron/synapse counts**
2. **Inefficient configurations**
3. **Memory allocation overhead**
4. **Poor cache utilization**
5. **Inefficient algorithms**

**Diagnostic Steps:**

```bash
# Profile simulation performance
perf record -g ./nlm_simulation
perf report

# Monitor memory usage
/usr/bin/time -v ./nlm_simulation

# Check CPU usage
top -p $(pgrep nlm_simulation)
```

**Solutions:**

1. **Configuration Optimization:**
   ```python
   # Optimize for performance
   config.set("brain.neuron_count", 1000)  # Smaller but efficient
   config.set("brain.synapse_density", 0.05)  # Lower density
   config.set("performance.enable_memory_pools", True)
   ```

2. **Memory Pool Usage:**
   ```python
   # Enable memory pools
   config.set("performance.enable_memory_pools", True)
   config.set("performance.memory_pool.neuron_pool_size", 1024)
   ```

3. **SIMD Optimization:**
   ```python
   # Enable SIMD instructions
   config.set("performance.enable_simd", True)
   ```

4. **Cache Optimization:**
   ```python
   # Use cache-friendly sizes
   config.set("brain.neuron_count", 1024)  # Power of 2
   ```

### 3. Memory Issues

#### Symptom: Memory usage grows uncontrollably

**Common Causes:**

1. **Memory leaks**
2. **Unbounded growth in neural populations**
3. **Inefficient data structures**
4. **Large synapse counts**

**Diagnostic Steps:**

```bash
# Monitor memory usage
valgrind --tool=massif ./nlm_simulation

# Check memory usage with top
watch -n 1 'ps aux | grep nlm_simulation'

# Check system memory
free -h
```

**Solutions:**

1. **Memory Leak Detection:**
   ```python
   import pynlm
   import psutil
   import time
   
   class MemoryMonitor:
       def __init__(self):
           self.process = psutil.Process()
           self.memory_samples = []
       
       def record_memory(self):
           memory_info = self.process.memory_info()
           self.memory_samples.append(memory_info.rss)
       
       def detect_leak(self):
           if len(self.memory_samples) < 10:
               return False
           
           # Simple leak detection
           recent_growth = self.memory_samples[-1] - self.memory_samples[-5]
           return recent_growth > 10 * 1024 * 1024  # > 10MB growth
   
   monitor = MemoryMonitor()
   ```

2. **Memory Pool Usage:**
   ```python
   # Enable memory pools
   config.set("performance.enable_memory_pools", True)
   config.set("performance.memory_pool.enable_free_list", True)
   ```

3. **Memory Limit Configuration:**
   ```python
   # Set memory limits
   import resource
   resource.setrlimit(resource.RLIMIT_AS, (2*1024*1024*1024, -1))  # 2GB
   ```

### 4. Configuration Errors

#### Symptom: Invalid configuration or parameter errors

**Common Causes:**

1. **Missing required parameters**
2. **Invalid parameter values**
3. **Type mismatches**
4. **Incompatible combinations**

**Diagnostic Steps:**

```python
# Validate configuration
config = pynlm.createDefaultConfig()

# Check for required parameters
required_keys = ["brain.neuron_count", "brain.synapse_density"]
for key in required_keys:
    if not config.has(key):
        print(f"Missing required parameter: {key}")

# Validate parameter values
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 1 or neuron_count > 100000:
        print(f"Invalid neuron count: {neuron_count}")
    
if config.has("brain.synapse_density"):
    synapse_density = config.getOr("brain.synapse_density", 0.1)
    if synapse_density < 0.0 or synapse_density > 1.0:
        print(f"Invalid synapse density: {synapse_density}")
```

**Solutions:**

1. **Configuration Validation:**
   ```python
   def validate_config(config):
       errors = []
       
       # Check required parameters
       required_keys = ["brain.neuron_count", "brain.synapse_density"]
       for key in required_keys:
           if not config.has(key):
               errors.append(f"Missing required parameter: {key}")
       
       # Validate neuron count
       if config.has("brain.neuron_count"):
           neuron_count = config.getOr("brain.neuron_count", 1000)
           if neuron_count < 1 or neuron_count > 100000:
               errors.append(f"Invalid neuron count: {neuron_count}. Must be 1-100000")
       
       # Validate synapse density
       if config.has("brain.synapse_density"):
           synapse_density = config.getOr("brain.synapse_density", 0.1)
           if synapse_density < 0.0 or synapse_density > 1.0:
               errors.append(f"Invalid synapse density: {synapse_density}. Must be 0.0-1.0")
       
       return errors
   
   # Use validation
   errors = validate_config(config)
   if errors:
       for error in errors:
           print(f"Configuration error: {error}")
   ```

2. **Default Values:**
   ```python
   # Use safe defaults
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 1000)  # Safe default
   config.set("brain.synapse_density", 0.1)  # Safe default
   ```

3. **Parameter Ranges:**
   ```python
   # Define parameter ranges
   def is_valid_parameter(key, value):
       if key == "brain.neuron_count":
           return 1 <= value <= 100000
       elif key == "brain.synapse_density":
           return 0.0 <= value <= 1.0
       elif key == "brain.region_count":
           return 1 <= value <= 1000
       return True
   ```

### 5. Simulation Issues

#### Symptom: Simulation produces unexpected results

**Common Causes:**

1. **Incorrect initialization**
2. **Wrong configuration parameters**
3. **Numerical precision issues**
4. **Algorithm bugs**
5. **Random seed issues**

**Diagnostic Steps:**

```python
# Validate simulation setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check simulation state
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

**Solutions:**

1. **Initialization Validation:**
   ```python
   def validate_simulation(brain):
       errors = []
       
       # Check neuron count
       if brain.getTotalNeuronCount() == 0:
           errors.append("No neurons initialized")
       
       # Check synapse count
       if brain.getTotalSynapseCount() == 0:
           errors.append("No synapses initialized")
       
       # Check firing neurons
       if brain.getFiringNeuronCount() < 0:
           errors.append("Negative firing neuron count")
       
       return errors
   
   errors = validate_simulation(brain)
   if errors:
       for error in errors:
           print(f"Simulation error: {error}")
   ```

2. **Random Seed Testing:**
   ```python
   # Test with different random seeds
   for seed in [42, 123, 456]:
       config = pynlm.createDefaultConfig()
       config.set("random.seed", seed)
       brain = pynlm.createBrain(config)
       brain.initialize()
       
       # Run simulation
       for step in range(100):
           brain.step(step)
       
       # Check if simulation completed without errors
       print(f"Seed {seed}: Simulation completed successfully")
   ```

3. **Parameter Testing:**
   ```python
   # Test with different parameter values
   parameter_tests = [
       {"neuron_count": 100, "synapse_density": 0.05},
       {"neuron_count": 1000, "synapse_density": 0.1},
       {"neuron_count": 5000, "synapse_density": 0.2},
   ]
   
   for test_params in parameter_tests:
       config = pynlm.createDefaultConfig()
       config.set("brain.neuron_count", test_params["neuron_count"])
       config.set("brain.synapse_density", test_params["synapse_density"])
       
       try:
           brain = pynlm.createBrain(config)
           brain.initialize()
           
           for step in range(100):
               brain.step(step)
           
           print(f"Test with {test_params['neuron_count']} neurons and {test_params['synapse_density']} synapse density: PASSED")
           
       except Exception as e:
           print(f"Test with {test_params['neuron_count']} neurons and {test_params['synapse_density']} synapse density: FAILED - {e}")
   ```

## Error Codes and Messages

### Common Error Messages

| Error Code | Message | Cause | Solution |
|------------|---------|-------|----------|
| `InvalidArgumentError` | "Invalid argument provided: parameter cannot be null" | Null pointer passed as argument | Check for null pointers before calling methods |
| `InvalidArgumentError` | "Invalid parameter value: must be positive" | Negative or zero parameter value | Use positive values for parameters |
| `InvalidArgumentError` | "Invalid configuration: required parameter missing" | Missing required configuration | Check configuration keys before use |
| `InvalidArgumentError` | "Invalid range: value out of bounds" | Parameter value out of valid range | Use valid parameter values |
| `RuntimeError` | "Memory allocation failed: insufficient memory" | Out of memory | Increase memory limits or reduce configuration |
| `RuntimeError` | "Simulation failed: initialization error" | Brain initialization failed | Check configuration and initialization process |
| `RuntimeError` | "Simulation failed: step execution error" | Brain step execution failed | Check for simulation errors |
| `ConfigurationError` | "Configuration error: invalid JSON format" | Invalid JSON file format | Use valid JSON format or simple key=value format |
| `ConfigurationError` | "Configuration error: YAML parsing failed" | Invalid YAML file format | Use valid YAML format |
| `ConfigurationError` | "Configuration error: file not found" | Configuration file not found | Check file path and permissions |
| `MemoryError` | "Memory pool error: pool exhausted" | Memory pool exhausted | Increase pool size or use larger chunks |

## Debugging Techniques

### 1. Debug Builds

```bash
# Build with debug symbols and assertions
g++ -g -O0 -std=c++20 -I./src -I./python src/brain/Brain.cpp src/brain/Neuron.cpp src/brain/Synapse.cpp

# Run with debug flags
export NLM_DEBUG=1
./nlm_simulation --debug
```

### 2. Logging Configuration

```python
import pynlm

# Enable debug logging
config = pynlm.createDefaultConfig()
config.set("logging.level", "DEBUG")
config.set("logging.file", "nlm_simulation.log")

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with logging
for step in range(100):
    brain.step(step)
    
    # Log step information
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### 3. Profiling

```bash
# Profile CPU usage
perf record -g -o perf.data ./nlm_simulation
perf report

# Profile memory usage
valgrind --tool=massif --stacks=yes ./nlm_simulation
massif_visualizer massif.out.12345
```

### 4. Test Output Capture

```python
import pynlm
import sys
import io

# Capture test output
output_capture = io.StringIO()
sys.stdout = output_capture

# Run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

# Restore stdout
sys.stdout = sys.__stdout__

# Print captured output
print(output_capture.getvalue())
```

## Common Pitfalls and Solutions

### Pitfall 1: Forgetting to Initialize

**Problem:** Brain must be initialized before use

**Solution:**
```python
# WRONG
brain = pynlm.createBrain(config)
brain.step(0)  # Crashes - brain not initialized

# CORRECT
brain = pynlm.createBrain(config)
brain.initialize()  # Always call initialize!
brain.step(0)  # Works correctly
```

### Pitfall 2: Large Configuration

**Problem:** Configuration with too many neurons/synapses

**Solution:**
```python
# Use realistic configuration
small_config = pynlm.createDefaultConfig()
small_config.set("brain.neuron_count", 1000)  # Reasonable size
small_config.set("brain.synapse_density", 0.1)

# Test with small configuration first
brain = pynlm.createBrain(small_config)
brain.initialize()
for step in range(100):
    brain.step(step)
```

### Pitfall 3: Invalid File Paths

**Problem:** Configuration file not found

**Solution:**
```python
import os
import tempfile

# Create temporary file with configuration
temp_file = tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False)
temp_file.write('{"brain.neuron_count": 1000, "brain.synapse_density": 0.1}')
temp_file.close()

# Validate file exists
if os.path.exists(temp_file.name):
    config = pynlm.createDefaultConfig()
    config.loadFromFile(temp_file.name)
    
    # Use configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Clean up
    os.unlink(temp_file.name)
else:
    print(f"Configuration file not found: {temp_file.name}")
```

### Pitfall 4: Random Seed Issues

**Problem:** Non-reproducible results due to random seeds

**Solution:**
```python
import pynlm

# Set random seed for reproducibility
config = pynlm.createDefaultConfig()
config.set("random.seed", 42)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Same results with same seed
brain2 = pynlm.createBrain(config)
brain2.initialize()
for step in range(100):
    brain2.step(step)

# Results should be identical
print(f"Results match: {brain.getTotalSpikeCount() == brain2.getTotalSpikeCount()}")
```

### Pitfall 5: Memory Management

**Problem:** Memory leaks and inefficient memory usage

**Solution:**
```python
import pynlm
import gc

# Enable memory pools
config = pynlm.createDefaultConfig()
config.set("performance.enable_memory_pools", True)

# Force garbage collection
gc.enable()

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with memory monitoring
for step in range(1000):
    brain.step(step)
    
    # Periodically force garbage collection
    if step % 100 == 0:
        gc.collect()
```

## Help and Support

### Getting Help

**Online Resources:**
- Documentation: https://docs.nlp-framework.org
- GitHub Repository: https://github.com/nlm-project/nlm
- Issue Tracker: https://github.com/nlm-project/nlm/issues
- Discussion Forum: https://discuss.nlp-framework.org

**Contact Support:**
- Email: support@nlm-framework.org
- Slack: https://slack.nlp-framework.org

### Reporting Issues

**When reporting an issue, please include:**

1. **System Information:**
   - Operating system
   - Python version
   - NLM version
   - Compiler version

2. **Error Details:**
   - Full error message
   - Stack trace (if available)
   - Steps to reproduce
   - Expected vs actual behavior

3. **Test Case:**
   - Minimal reproduction case
   - Configuration files
   - Log files (if available)

### Example Issue Report Template

```
## Issue Report

**System Information:**
- Operating System: Ubuntu 20.04
- Python Version: 3.8.10
- NLM Version: 0.1.0
- Compiler Version: g++ 9.4.0

**Error Details:**
- Error Message: "RuntimeError: Simulation failed: step execution error"
- Stack Trace: [stack trace here]
- Steps to Reproduce:
  1. Create configuration with brain.neuron_count=1000
  2. Initialize brain
  3. Run simulation for 100 steps
  4. Observe crash at step 50
- Expected Behavior: Simulation completes successfully
- Actual Behavior: Simulation crashes with segmentation fault

**Test Case:**
```python
import pynlm

# Configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

**Log Files:**
- nlm_simulation.log (if logging enabled)
- brain.log (if brain logging enabled)

**Additional Information:**
- Profiling data: [attach profiling data]
- Memory usage: [attach memory usage data]
- Configuration files: [attach configuration files]
```

## Conclusion

This troubleshooting guide provides comprehensive information for diagnosing and resolving NLM issues. By following these guidelines, you can effectively troubleshoot problems, optimize performance, and ensure reliable operation of your neural simulations.

Remember to:
1. Always initialize the brain before use
2. Use realistic configuration parameters
3. Enable appropriate logging for debugging
4. Monitor performance and memory usage
5. Test with different configurations to identify issues
6. Document any problems and solutions for future reference

For additional help, please refer to the official documentation or contact the NLM support team.
