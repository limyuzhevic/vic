# NLM Troubleshooting Guide

## Overview

The Troubleshooting Guide provides solutions to common issues encountered when using the NLM framework. It includes step-by-step instructions for diagnosing and resolving problems in neural simulations.

## Common Issues and Solutions

### Issue 1: "Brain is not initialized"

**Error Message:**
```
SimulationError: Brain is not initialized. Call initialize() first.
```

**Cause:** The brain object has not been properly initialized before calling simulation methods.

**Solution:**
```python
# Problem: Calling step() without initialization
brain = pynlm.createBrain()
try:
    brain.step(0)
    print("ERROR: Should have thrown SimulationError")
except pynlm.SimulationError as e:
    print(f"Error: {e}")
    # SOLUTION: Call brain.initialize() first
    brain.initialize()
    brain.step(0)
    print("Fixed: Brain initialized successfully")
```

**Prevention:**
- Always call `brain.initialize()` before using simulation methods
- Use context managers for automatic initialization
- Check `brain.is_initialized()` before operations

### Issue 2: Recording File Not Found

**Error Message:**
```
FileNotFoundError: [Errno 2] No such file or directory: 'recording.json'
```

**Cause:** Recording was not enabled or the simulation steps were insufficient.

**Solution:**
```python
# PROBLEM: Recording file not created
brain = pynlm.createBrain()
brain.initialize()

# Enable recording
brain.enable_recording("recording.json")
brain.step(0)
brain.disable_recording()

# Check if file was created
import os
if not os.path.exists("recording.json"):
    print("Recording file not found")
    # SOLUTION: Check if recording was enabled correctly
    print("Check: Recording should be enabled before simulation steps")
```

**Prevention:**
- Always enable recording before starting simulation steps
- Use try-catch blocks for file operations
- Check file permissions before recording

### Issue 3:

**Error Message:**
```
PythonError: Invalid neuron type: InvalidType
```

**Cause:** Attempting to convert an invalid string to a neuron type.

**Solution:**
```python
# PROBLEM: Invalid neuron type
try:
    neuron_type = pynlm.string_to_neuron_type("InvalidType")
    print("ERROR: Should have thrown PythonError")
except pynlm.PythonError as e:
    print(f"Error: {e}")
    # SOLUTION: Use valid neuron type strings
    neuron_type = pynlm.string_to_neuron_type("Excitatory")
    print(f"Fixed: Using valid type 'Excitatory' -> {neuron_type}")
```

**Prevention:**
- Use the available neuron type constants
- Validate neuron type strings before conversion
- Use error handling for type conversions

### Issue 4: Memory Usage Too High

**Error Message:**
```
MemoryError: Cannot allocate memory for large neural network
```

**Cause:** Attempting to create a neural network that exceeds available memory.

**Solution:**
```python
# PROBLEM: Memory allocation error
import pynlm
import psutil
import os

# Check available memory
process = psutil.Process(os.getpid())
available_memory = process.memory_info().available / 1024 / 1024  # MB

print(f"Available memory: {available_memory:.2f} MB")

# Create smaller configuration
if available_memory < 500:  # Less than 500MB available
    print("Memory is limited - using smaller configuration")
    config = {
        "neuron_count": 100,
        "region_count": 3,
        "connection_probability": 0.05
    }
else:
    print("Sufficient memory - using larger configuration")
    config = {
        "neuron_count": 1000,
        "region_count": 10,
        "connection_probability": 0.1
    }

brain = pynlm.createBrain(config)
brain.initialize()
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

**Prevention:**
- Monitor memory usage during simulation
- Use configuration validation before brain creation
- Implement memory-efficient data structures

### Issue 5: Performance Degradation

**Error Message:**
```
PerformanceWarning: Simulation step taking longer than expected
```

**Cause:** Simulation performance degrading over time.

**Solution:**
```python
import pynlm
import time

# Monitor simulation performance
brain = pynlm.createBrain()
brain.initialize()

# Track performance over time
step_times = []
for step in range(100):
    start_time = time.time()
    brain.step(step)
    step_time = time.time() - start_time
    step_times.append(step_time)
    
    # Check for performance degradation
    if step > 10 and step_time > step_times[step-10] * 2:
        print(f"Warning: Performance degradation detected at step {step}")
        print(f"  Current step time: {step_time:.4f}s")
        print(f"  Previous step time: {step_times[step-10]:.4f}s")
        
        # Performance optimization suggestions
        if brain.getTotalNeuronCount() > 500:
            print("  Suggestion: Consider reducing neuron count for better performance")
        if brain.getTotalSynapseCount() > 5000:
            print("  Suggestion: Consider reducing synapse count for better performance")
        
        # Reset brain if performance is severely degraded
        if step_time > 1.0:  # Step taking more than 1 second
            print("  Resetting brain to restore performance...")
            brain.reset()
            brain.initialize()
    
    print(f"Step {step}: {step_time:.4f}s")
```

**Prevention:**
- Monitor step execution times
- Implement performance profiling
- Use batch operations for better performance
- Regular brain reset for long simulations

### Issue 6: Error Handling Issues

**Error Message:**
```
Uncaught Exception: ...
```

**Cause:** Insufficient error handling in user code.

**Solution:**
```python
# PROBLEM: Unhandled exception
def safe_simulation(brain, steps):
    """Run simulation with comprehensive error handling."""
    try:
        for step in range(steps):
            brain.step(step)
            
    except pynlm.SimulationError as e:
        print(f"Simulation error at step {step}: {e}")
        # Attempt recovery
        try:
            brain.reset()
            brain.initialize()
            print("Brain recovered - continuing simulation")
        except Exception as recovery_error:
            print(f"Recovery failed: {recovery_error}")
            raise
            
    except pynlm.ValidationError as e:
        print(f"Validation error: {e}")
        # Fix validation issue and retry
        print("Attempting to fix validation issue...")
        
    except pynlm.PythonError as e:
        print(f"Python interface error: {e}")
        # Check Python version or interface compatibility
        
    except Exception as e:
        print(f"Unexpected error: {e}")
        # Log detailed error information
        import traceback
        traceback.print_exc()
        
    finally:
        print("Simulation completed or terminated")

# Usage
brain = pynlm.createBrain()
brain.initialize()
safe_simulation(brain, 100)
```

**Prevention:**
- Implement comprehensive error handling
- Use specific exception types for different error conditions
- Log detailed error information for debugging
- Implement recovery strategies where possible

## Debugging Techniques

### Debug Mode

Enable detailed debugging for complex issues:

```python
import pynlm

# Enable debug mode
brain = pynlm.createBrain()
brain.initialize()

# Set debug level
debug_level = "detailed"  # Options: "basic", "detailed", "verbose"

# Run simulation with debug output
for step in range(50):
    brain.step(step)
    if debug_level != "basic":
        print(f"Step {step}: Firing neurons = {brain.getFiringNeuronCount()}")
        if debug_level == "detailed":
            print(f"  Network stats: {brain.get_stats()}")
        elif debug_level == "verbose":
            print(f"  All metrics: {brain.analyze_network()}")
```

### Memory Profiling

Profile memory usage to identify memory leaks:

```python
import pynlm
import psutil
import os
import gc

# Start memory profiling
brain = pynlm.createBrain()
process = psutil.Process(os.getpid())

# Take initial memory measurement
gc.collect()
initial_memory = process.memory_info().rss / 1024 / 1024  # MB

print(f"Initial memory: {initial_memory:.2f} MB")

# Run simulation steps
for step in range(100):
    brain.step(step)
    
    # Measure memory every 10 steps
    if step % 10 == 0:
        gc.collect()
        current_memory = process.memory_info().rss / 1024 / 1024
        memory_increase = current_memory - initial_memory
        
        print(f"Step {step}: Memory = {current_memory:.2f} MB "
              f"(Increase: {memory_increase:.2f} MB)")
        
        # Detect potential memory leaks
        if memory_increase > 100:  # More than 100MB increase
            print(f"  WARNING: Possible memory leak detected!")
            print(f"  Consider: Clearing unused references, optimizing data structures")
```

### Performance Profiling

Profile simulation performance to identify bottlenecks:

```python
import pynlm
import time

# Performance profiling
brain = pynlm.createBrain()
brain.initialize()

# Profile different aspects of simulation
print("=== Performance Profiling ===")

# 1. Step execution time
step_times = []
for step in range(50):
    start_time = time.time()
    brain.step(step)
    step_time = time.time() - start_time
    step_times.append(step_time)
    
    if step % 10 == 0:
        print(f"Step {step}: {step_time:.4f}s")

# Calculate performance statistics
avg_step_time = sum(step_times) / len(step_times)
max_step_time = max(step_times)
min_step_time = min(step_times)

print(f"\nPerformance Statistics:")
print(f"  Average step time: {avg_step_time:.4f}s")
print(f"  Maximum step time: {max_step_time:.4f}s")
print(f"  Minimum step time: {min_step_time:.4f}s")
print(f"  Step time variability: {(max_step_time - min_step_time):.4f}s")

# 2. Network analysis performance
print("\n=== Network Analysis Performance ===")

start_time = time.time()
stats = brain.get_stats()
analysis_time = time.time() - start_time
print(f"Network stats time: {analysis_time:.4f}s")

start_time = time.time()
network_analysis = brain.analyze_network()
analysis_time = time.time() - start_time
print(f"Network analysis time: {analysis_time:.4f}s")

start_time = time.time()
region_info = brain.get_network_info()
region_time = time.time() - start_time
print(f"Region info time: {region_time:.4f}s")

# 3. Type conversion performance
print("\n=== Type Conversion Performance ===")

# Test type conversion performance
import numpy as np

test_data = list(range(1000))

start_time = time.time()
for _ in range(100):
    array = pynlm.to_numpy_array(test_data)
conversion_time = time.time() - start_time
print(f"NumPy array conversion time: {conversion_time:.4f}s")

start_time = time.time()
for _ in range(100):
    vector = pynlm.to_vector(array)
reverse_conversion_time = time.time() - start_time
print(f"Vector conversion time: {reverse_conversion_time:.4f}s")
```

### Configuration Validation

Validate configuration to prevent runtime errors:

```python
import pynlm

def validate_configuration(config):
    """Validate configuration before brain creation."""
    errors = []
    
    # Check required keys
    required_keys = ["neuron_count", "region_count", "connection_probability"]
    for key in required_keys:
        if key not in config:
            errors.append(f"Missing required key: {key}")
    
    # Check value ranges
    if "neuron_count" in config:
        neuron_count = config["neuron_count"]
        if not isinstance(neuron_count, int) or neuron_count <= 0 or neuron_count > 100000:
            errors.append(f"Invalid neuron_count: {neuron_count}. Must be positive integer <= 100000")
    
    if "region_count" in config:
        region_count = config["region_count"]
        if not isinstance(region_count, int) or region_count <= 0 or region_count > 1000:
            errors.append(f"Invalid region_count: {region_count}. Must be positive integer <= 1000")
    
    if "connection_probability" in config:
        conn_prob = config["connection_probability"]
        if not isinstance(conn_prob, (int, float)) or conn_prob < 0 or conn_prob > 1:
            errors.append(f"Invalid connection_probability: {conn_prob}. Must be between 0 and 1")
    
    return errors

def create_brain_with_validation(config):
    """Create brain with configuration validation."""
    # Validate configuration
    validation_errors = validate_configuration(config)
    
    if validation_errors:
        print("Configuration validation errors:")
        for error in validation_errors:
            print(f"  - {error}")
        
        # Use default configuration if validation fails
        print("Using default configuration")
        config = pynlm.create_default_config_dict()
    
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    except Exception as e:
        print(f"Brain creation failed: {e}")
        # Try with reduced configuration
        print("Attempting recovery with smaller configuration")
        recovery_config = {
            "neuron_count": 100,
            "region_count": 3,
            "connection_probability": 0.1
        }
        
        try:
            brain = pynlm.createBrain(recovery_config)
            brain.initialize()
            return brain
        except Exception as recovery_error:
            print(f"Recovery failed: {recovery_error}")
            raise

# Usage
config = {
    "neuron_count": 500,
    "region_count": 10,
    "connection_probability": 0.2,
    "learning_rate": 0.05  # This key is not validated but will be accepted
}

brain = create_brain_with_validation(config)
print(f"Brain created successfully with {brain.getTotalNeuronCount()} neurons")
```

## Advanced Debugging

### Custom Debug Extensions

Extend debugging capabilities for specific use cases:

```python
import pynlm
from typing import Dict, Any, List

class CustomDebugger:
    """Custom debugging extension for NLM."""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.debug_data = {
            'steps': [],
            'events': [],
            'metrics': {},
            'errors': []
        }
    
    def log_step(self, step: int, event: str, details: Dict[str, Any] = None):
        """Log detailed step information."""
        step_data = {
            'step': step,
            'event': event,
            'timestamp': time.time(),
            'details': details or {},
            'brain_state': self._capture_brain_state(step)
        }
        
        self.debug_data['steps'].append(step_data)
        self.debug_data['events'].append(event)
        
        # Store metrics
        self.debug_data['metrics'][step] = self._collect_metrics(step)
    
    def _capture_brain_state(self, step: int) -> Dict[str, Any]:
        """Capture detailed brain state."""
        return {
            'simulation_time': self.brain.getSimulationTime(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'network_stats': self.brain.get_stats(),
            'network_analysis': self.brain.analyze_network(),
            'region_count': self.brain.getRegionCount()
        }
    
    def _collect_metrics(self, step: int) -> Dict[str, Any]:
        """Collect performance metrics."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        return {
            'cpu_usage': process.cpu_percent() / 100.0,
            'memory_usage': process.memory_info().rss / 1024 / 1024,  # MB
            'step_time': 0.0  # Would be measured externally
        }
    
    def analyze_debug_data(self) -> Dict[str, Any]:
        """Analyze collected debug data."""
        analysis = {
            'step_count': len(self.debug_data['steps']),
            'events': self._analyze_events(),
            'performance': self._analyze_performance(),
            'patterns': self._analyze_patterns(),
            'anomalies': self._detect_anomalies()
        }
        
        return analysis
    
    def _analyze_events(self) -> Dict[str, Any]:
        """Analyze event patterns."""
        from collections import Counter
        event_counts = Counter(self.debug_data['events'])
        
        return {
            'total_events': len(self.debug_data['events']),
            'event_types': dict(event_counts),
            'event_frequency': {event: count / len(self.debug_data['events']) 
                               for event, count in event_counts.items()}
        }
    
    def _analyze_performance(self) -> Dict[str, Any]:
        """Analyze performance metrics."""
        if not self.debug_data['metrics']:
            return {}
        
        cpu_values = [m['cpu_usage'] for m in self.debug_data['metrics'].values()]
        memory_values = [m['memory_usage'] for m in self.debug_data['metrics'].values()]
        
        return {
            'average_cpu': sum(cpu_values) / len(cpu_values) if cpu_values else 0,
            'peak_cpu': max(cpu_values) if cpu_values else 0,
            'average_memory': sum(memory_values) / len(memory_values) if memory_values else 0,
            'peak_memory': max(memory_values) if memory_values else 0
        }
    
    def _analyze_patterns(self) -> Dict[str, Any]:
        """Analyze patterns in the data."""
        firing_rates = []
        for step_data in self.debug_data['steps']:
            firing_rates.append(step_data['brain_state']['firing_rate'])
        
        # Simple pattern detection
        patterns = {
            'firing_rate_trend': 'increasing' if firing_rates[-1] > firing_rates[0] else 'decreasing',
            'firing_rate_stability': 'stable' if max(firing_rates) - min(firing_rates) < 0.1 else 'variable',
            'simulation_time_distribution': 'uniform'  # Would need more sophisticated analysis
        }
        
        return patterns
    
    def _detect_anomalies(self) -> List[Dict[str, Any]]:
        """Detect anomalies in the data."""
        anomalies = []
        
        # Detect performance anomalies
        if self.debug_data['metrics']:
            for step, metrics in self.debug_data['metrics'].items():
                if metrics['cpu_usage'] > 0.9:  # CPU usage > 90%
                    anomalies.append({
                        'step': step,
                        'type': 'high_cpu_usage',
                        'severity': 'high',
                        'value': metrics['cpu_usage']
                    })
                
                if metrics['memory_usage'] > 1000:  # Memory > 1000MB
                    anomalies.append({
                        'step': step,
                        'type': 'high_memory_usage',
                        'severity': 'medium',
                        'value': metrics['memory_usage']
                    })
        
        return anomalies

# Usage example
brain = pynlm.createBrain()
brain.initialize()

custom_debugger = CustomDebugger(brain)

# Run simulation with custom debugging
for step in range(100):
    brain.step(step)
    custom_debugger.log_step(step, 'simulation_step')

# Analyze debug data
analysis = custom_debugger.analyze_debug_data()
print("Debug Analysis Results:")
print(f"Total steps: {analysis['step_count']}")
print(f"Event types: {analysis['events']['event_types']}")
print(f"Performance - Average CPU: {analysis['performance']['average_cpu']:.2f}")
print(f"Performance - Peak Memory: {analysis['performance']['peak_memory']:.2f} MB")
```

## Tools and Resources

### Debugging Tools

#### NLM Debugging Utilities

The NLM framework includes built-in debugging utilities:

```bash
# Run with debug flags
python3 -m pynlm --debug-level=detailed

# Generate debug report
python3 -m pynlm --generate-debug-report

# Profile performance
python3 -m pynlm --profile-performance

# Validate configuration
python3 -m pynlm --validate-config config.json
```

#### Third-Party Debugging Tools

Popular third-party tools for neural network debugging:

- **TensorFlow Debugger**: For TensorFlow-based neural networks
- **PyTorch Debugger**: For PyTorch-based neural networks  
- **Neuron Visualization**: Tools for visualizing neural activity
- **Memory Profilers**: Tools like `memory_profiler` and `tracemalloc`

### Online Resources

#### Documentation

- **NLM Documentation**: https://nlm.readthedocs.io
- **API Reference**: https://nlm.readthedocs.io/en/latest/api
- **Tutorials**: https://nlm.readthedocs.io/en/latest/tutorials

#### Support

- **GitHub Issues**: Report bugs and issues at https://github.com/NLM/NLM/issues
- **Stack Overflow**: Ask questions with `nlm` tag
- **Discord Server**: https://discord.gg/nlm
- **Community Forum**: https://community.nlm.ai

#### Examples

- **Example Repository**: https://github.com/NLM/examples
- **Benchmarks**: https://github.com/NLM/benchmarks
- **Tutorials**: https://github.com/NLM/tutorials

## Conclusion

Effective debugging is essential for developing and maintaining NLM neural simulations. This troubleshooting guide provides:

1. **Comprehensive Problem Solutions**: Detailed solutions to common issues
2. **Prevention Strategies**: Techniques to avoid common problems
3. **Advanced Debugging Tools**: Custom debugging extensions and analysis methods
4. **Performance Monitoring**: Tools to track and optimize simulation performance
5. **Configuration Validation**: Validation techniques to prevent runtime errors

By following the debugging techniques and troubleshooting methods outlined in this guide, developers can:

- Quickly identify and resolve simulation issues
- Optimize performance and resource usage
- Implement comprehensive error handling
- Maintain high code quality and reliability

The NLM framework provides extensive debugging capabilities while supporting both beginner and advanced debugging needs. Use these tools and techniques to create robust, high-performance neural network simulations.
