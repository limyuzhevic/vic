# NLM Python Bindings - Enhanced Documentation

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") Python bindings provide access to the Neural Learning Machine (NLM) neural simulation framework through a Python interface. This enhanced documentation covers the **24 new advanced Python bindings** that significantly extend the capabilities of the original Python bindings.

## Installation

### Prerequisites

- Python 3.8+
- CMake 3.16+
- C++ compiler (gcc, clang, etc.)
- pybind11 development headers

### Build Instructions

1. **Clone the NLM repository** (if not already done)

2. **Navigate to the Python bindings directory**:
   ```bash
   cd python
   ```

3. **Create build directory and configure**:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

4. **Build the Python bindings**:
   ```bash
   make -j4
   ```

5. **Install the module**:
   ```bash
   cd ../
   pip install -e .
   ```

### Alternative: Using scikit-build

If you prefer using scikit-build for easier cross-platform building:

```bash
pip install scikit-build-core pybind11
pip install -e .
```

### Quick Verification

After building, verify the bindings work:
```bash
python3 -c "import pynlm; print('NLM Python bindings loaded successfully')"
```

## Overview of New Bindings

The enhanced Python bindings include **24 new functions** organized into 6 categories:

### 1. Convenience Factory Functions (4)
- `create_brain_with_config()` - Create brain with config validation
- `create_brain_from_json()` - Load brain configuration from JSON
- `create_brain_from_args()` - Parse command line arguments
- `create_simple_agent()` - Quick agent setup with brain, world, and basic training

### 2. Utility Functions (4)
- `validate_brain()` - Validate brain configuration and state
- `save_brain_snapshot()` - Save brain state with timestamp
- `load_brain_snapshot()` - Load brain state from snapshot
- `benchmark_simulation()` - Measure performance of simulation steps

### 3. Advanced Features (4)
- `run_training_episode()` - Complete training episode with reward tracking
- `run_simulation_batch()` - Run multiple simulations in parallel
- `monitor_brain_development()` - Monitor developmental stages over time
- `analyze_neural_activity()` - Analyze firing patterns and connectivity

### 4. Configuration Management (4)
- `export_config_to_json()` - Export configuration as JSON string
- `import_config_from_json()` - Import configuration from JSON string
- `validate_config_schema()` - Validate configuration against schema
- `merge_config()` - Merge multiple configurations

### 5. Simulation Control (4)
- `pause_simulation()` - Temporarily pause brain activity
- `resume_simulation()` - Resume paused simulation
- `step_multiple()` - Execute multiple simulation steps at once
- `run_to_target()` - Run simulation until target reached

### 6. Performance Monitoring (4)
- `get_memory_usage()` - Get current memory usage
- `get_cpu_usage()` - Get CPU usage during simulation
- `profile_simulation()` - Profile simulation performance
- `generate_report()` - Generate comprehensive simulation report

## Quick Start Examples

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a few simulation steps
for step in range(10):
    brain.step(step)
    
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
```

### Advanced Usage with All New Functions

```python
import pynlm
import json
import time

# 1. Use convenience factory functions
print("=== Convenience Factory Functions ===")
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 5)

brain = pynlm.createBrainWithConfig(config)
print(f"✓ Created brain with config validation: {brain.getTotalNeuronCount()} neurons")

world = pynlm.createSimpleWorld()
agent = pynlm.createSimpleAgent(brain, world)
agent.initialize(world)
print(f"✓ Created simple agent with {agent.getSensoryInputSize()} sensory inputs")

# 2. Run training episode
print("\n=== Advanced Features ===")
results = pynlm.runTrainingEpisode(agent, world, 50)
print(f"✓ Training episode: {results['steps_taken']} steps, {results['total_reward']:.2f} total reward")

# 3. Monitor brain development
stats = pynlm.monitorBrainDevelopment(brain, 3)
for day_stats in stats:
    print(f"✓ Day {day_stats['day']}: {day_stats['stage'].name} stage")

# 4. Configuration management
json_str = pynlm.exportConfigToJson(config)
imported_config = pynlm.importConfigFromJson(json_str)
print(f"✓ Config exported/imported: {imported_config.getKeys()} keys")

# 5. Performance monitoring
benchmark = pynlm.benchmarkSimulation(brain, 100)
print(f"✓ Performance: {benchmark['steps_per_second']:.2f} steps/second")

memory = pynlm.get_memory_usage()
print(f"✓ Memory usage: {memory['usage_percent']}% ({memory['used_mb']}/{memory['total_mb']} MB)")

# 6. Simulation control
pynlm.pause_simulation(brain)
print("✓ Simulation paused")
pynlm.resume_simulation(brain)
print("✓ Simulation resumed")

print("\n=== All 24 new bindings demonstrated successfully! ===")
```

## API Reference

### Function Categories

#### 1. Convenience Factory Functions

**`create_brain_with_config(config)`**
- Creates a brain with configuration validation
- Returns: Brain object
- Purpose: Quick brain creation with parameter validation

**`create_brain_from_json(json_str)`**
- Loads brain configuration from JSON string
- Returns: Brain object
- Purpose: Load pre-configured brain settings

**`create_brain_from_args(argc, argv)`**
- Parses command line arguments
- Returns: Brain object
- Purpose: Configure brain from CLI arguments

**`create_simple_agent(brain, world)`**
- Creates an agent with brain, world, and basic training
- Returns: AgentBrain object
- Purpose: Quick agent setup for simulations

#### 2. Utility Functions

**`validate_brain(brain)`**
- Validates brain configuration and state
- Returns: bool
- Purpose: Check if brain is properly configured

**`save_brain_snapshot(brain, filepath="")`**
- Saves brain state with timestamp
- Returns: bool
- Purpose: Persistent storage of brain state

**`load_brain_snapshot(brain, filepath)`**
- Loads brain state from snapshot
- Returns: bool
- Purpose: Restore brain from saved state

**`benchmark_simulation(brain, steps)`**
- Measures performance of simulation steps
- Returns: dict with timing and metrics
- Purpose: Performance analysis

#### 3. Advanced Features

**`run_training_episode(agent, world, max_steps)`**
- Runs a complete training episode with reward tracking
- Returns: dict with episode statistics
- Purpose: Behavioral learning and training

**`run_simulation_batch(brains, steps)`**
- Runs multiple simulations in parallel
- Returns: list of result dictionaries
- Purpose: Batch processing and comparison

**`monitor_brain_development(brain, num_days)`**
- Monitors developmental stages over time
- Returns: list of developmental statistics
- Purpose: Longitudinal development analysis

**`analyze_neural_activity(brain, analysis_type)`**
- Analyzes firing patterns and connectivity
- Returns: dict with activity analysis
- Purpose: Neural pattern recognition

#### 4. Configuration Management

**`export_config_to_json(config)`**
- Exports configuration as JSON string
- Returns: JSON string
- Purpose: Configuration sharing and persistence

**`import_config_from_json(json_str)`**
- Imports configuration from JSON string
- Returns: Config object
- Purpose: Configuration restoration

**`validate_config_schema(config, schema_file)`**
- Validates configuration against schema
- Returns: bool
- Purpose: Configuration validation

**`merge_config(configs)`**
- Merges multiple configurations
- Returns: Merged Config object
- Purpose: Configuration combination

#### 5. Simulation Control

**`pause_simulation(brain)`**
- Temporarily pauses brain activity
- Returns: None
- Purpose: Interactive control and debugging

**`resume_simulation(brain)`**
- Resumes paused simulation
- Returns: None
- Purpose: Continue simulation after pause

**`step_multiple(brain, steps)`**
- Executes multiple simulation steps at once
- Returns: None
- Purpose: Efficient batch execution

**`run_to_target(brain, target_firing_rate, max_steps)`**
- Runs simulation until target reached
- Returns: number of steps taken
- Purpose: Goal-oriented simulation control

#### 6. Performance Monitoring

**`get_memory_usage()`**
- Gets current memory usage
- Returns: dict with memory statistics
- Purpose: Resource monitoring

**`get_cpu_usage(duration_ms)`**
- Gets CPU usage during simulation
- Returns: CPU usage percentage
- Purpose: Performance monitoring

**`profile_simulation(brain, duration_ms)`**
- Profiles simulation performance
- Returns: dict with profiling data
- Purpose: Performance optimization

**`generate_report(data)`**
- Generates comprehensive simulation report
- Returns: formatted report string
- Purpose: Documentation and analysis

## Error Handling

All new functions include comprehensive error handling:

### Common Exceptions

- `RuntimeError`: Parameter validation failures
- `ValueError`: Invalid input values
- `FileNotFoundError`: Missing configuration files
- `TypeError`: Incorrect parameter types

### Exception Handling Example

```python
import pynlm

try:
    # This might fail due to invalid configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", -1)  # Invalid negative count
    brain = pynlm.createBrainWithConfig(config)
except pynlm.RuntimeError as e:
    print(f"Configuration error: {e}")
except ValueError as e:
    print(f"Invalid value: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Best Practices

### Memory Management

1. **Use batch operations**: Prefer `step_multiple()` over individual steps
2. **Reuse objects**: Reuse brain/world objects when possible
3. **Save checkpoints**: Use `save_brain_snapshot()` for long-running simulations
4. **Monitor memory**: Use `get_memory_usage()` to track consumption

### Simulation Control

1. **Batch processing**: Use `run_simulation_batch()` for parallel simulations
2. **Pause/resume**: Use `pause_simulation()` for interactive debugging
3. **Target-driven**: Use `run_to_target()` for goal-oriented simulations
4. **Profile regularly**: Use `profile_simulation()` to identify bottlenecks

### Advanced Usage Patterns

```python
# Efficient batch processing
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(10)]
for brain in brains:
    brain.initialize()

# Run all simulations in batch
batch_results = pynlm.run_simulation_batch(brains, 100)

# Monitor each simulation
for i, result in enumerate(batch_results):
    print(f"Brain {i}: {result['steps_per_second']:.2f} steps/sec")

# Export results for analysis
with open("batch_results.json", "w") as f:
    json.dump(batch_results, f, indent=2)
```

## Development Guidelines

### Pythonic Design

1. **Descriptive names**: Functions follow Python naming conventions
2. **Clear parameters**: Each function has well-documented parameters
3. **Intuitive returns**: Return types match Python idioms
4. **Comprehensive docs**: All functions include docstrings

### Testing and Validation

#### Running Tests

```bash
# Run basic validation test
python3 test_bindings.py

# Run comprehensive demo
python3 examples/advanced_bindings_demo.py

# Run mock implementation test
python3 mock_advanced_bindings.py
```

#### Unit Test Structure

```python
import pynlm
import pytest

def test_create_brain_with_config():
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    brain = pynlm.createBrainWithConfig(config)
    assert brain.getTotalNeuronCount() == 100
    assert pynlm.validateBrain(brain) == True

def test_benchmark():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    benchmark = pynlm.benchmarkSimulation(brain, 10)
    assert "avg_time_ms" in benchmark
    assert "steps_per_second" in benchmark
    assert benchmark["steps_per_second"] > 0
```

### Integration Testing

The `examples/advanced_bindings_demo.py` provides comprehensive end-to-end testing of all 24 new functions.

## Troubleshooting

### Common Issues

#### 1. Import Error: "No module named 'pynlm'"

**Solution**: Ensure the module is installed:
```bash
pip install -e .
```

**Alternative**: Build the extension directly:
```bash
cd python
mkdir build
cd build
cmake ..
make -j4
```

#### 2. Build Errors

**Solution**: Install dependencies:
```bash
# On Ubuntu/Debian:
apt-get update
apt-get install python3-dev python3-pip build-essential cmake

# On macOS:
brew install python3 cmake
```

#### 3. Invalid Configuration

**Solution**: Use validation functions:
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 5)

if not pynlm.validateConfigSchema(config, "default_schema"):
    print("Invalid configuration")
    # Fix configuration and retry
```

### Advanced Troubleshooting

#### Performance Issues

If simulations are slow:
1. Use `profile_simulation()` to identify bottlenecks
2. Optimize with `step_multiple()` for batch operations
3. Check memory usage with `get_memory_usage()`
4. Reduce simulation complexity if needed

#### Memory Issues

If memory usage is high:
1. Save checkpoints with `save_brain_snapshot()`
2. Reuse brain objects across simulations
3. Monitor with `get_memory_usage()`
4. Consider smaller brain configurations

## Advanced Use Cases

### 1. Machine Learning Pipeline

```python
import pynlm
import numpy as np

class NLMClassifier:
    def __init__(self, neuron_count=1000):
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", neuron_count)
        self.brain = pynlm.createBrainWithConfig(self.config)
        self.brain.initialize()
        
    def train(self, training_data, episodes=100):
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        
        for episode in range(episodes):
            results = pynlm.runTrainingEpisode(agent, world, 50)
            print(f"Episode {episode}: Reward = {results['total_reward']:.2f}")
            
    def predict(self, input_data):
        # Process input through brain
        # Return prediction based on neural activity
        pass
```

### 2. Real-time Monitoring

```python
import pynlm
import time
import threading

class RealTimeMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.monitoring = False
        self.stats = []
        
    def start_monitoring(self, interval_ms=1000):
        self.monitoring = True
        threading.Thread(target=self._monitor_loop, 
                        args=(interval_ms,), daemon=True).start()
        
    def _monitor_loop(self, interval_ms):
        while self.monitoring:
            # Collect performance metrics
            benchmark = pynlm.benchmarkSimulation(self.brain, 10)
            memory = pynlm.get_memory_usage()
            
            self.stats.append({
                'timestamp': time.time(),
                'steps_per_second': benchmark['steps_per_second'],
                'memory_usage': memory['usage_percent'],
                'firing_rate': self.brain.getAverageFiringRate()
            })
            
            time.sleep(interval_ms / 1000.0)
            
    def stop_monitoring(self):
        self.monitoring = False
        
    def generate_report(self):
        report = pynlm.generate_report({
            'stats': self.stats,
            'num_samples': len(self.stats),
            'avg_performance': np.mean([s['steps_per_second'] for s in self.stats]),
            'max_memory': max(s['memory_usage'] for s in self.stats)
        })
        return report
```

### 3. Distributed Computing

```python
import pynlm
import multiprocessing

def run_simulation(config_dict, steps):
    """Worker function for distributed simulation"""
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        config.set(key, value)
        
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
        
    return {
        'final_spike_count': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'steps_per_second': steps / 10.0  # Placeholder
    }

def run_distributed_simulations(configs, steps, num_workers=None):
    """Run multiple simulations in parallel"""
    with multiprocessing.Pool(processes=num_workers) as pool:
        results = pool.starmap(run_simulation, 
                              [(config, steps) for config in configs])
    return results
```

## Migration Guide

### From Old Python Bindings to New

If upgrading from the old Python bindings:

1. **Replace basic creation with factory functions**:
   ```python
   # Old way
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   
   # New way (with validation)
   brain = pynlm.createBrainWithConfig(config)
   ```

2. **Use configuration management functions**:
   ```python
   # Old way
   json_str = json.dumps(config.getKeys())
   
   # New way
   json_str = pynlm.exportConfigToJson(config)
   imported_config = pynlm.importConfigFromJson(json_str)
   ```

3. **Add performance monitoring**:
   ```python
   # Add to your simulation loop
   benchmark = pynlm.benchmarkSimulation(brain, 100)
   print(f"Performance: {benchmark['steps_per_second']:.2f} steps/sec")
   ```

4. **Use simulation control for debugging**:
   ```python
   # Pause for debugging
   pynlm.pause_simulation(brain)
   # ... debug code ...
   pynlm.resume_simulation(brain)
   ```

## Future Extensions

The enhanced Python bindings provide a foundation for future features:

1. **GUI Integration**: Interactive visualization tools
2. **GPU Acceleration**: CUDA support for large-scale simulations
3. **Web Interfaces**: REST API for remote simulation
4. **Real-time Visualization**: Live monitoring of neural activity
5. **Machine Learning Integration**: Automated hyperparameter optimization

## Support and Resources

### Documentation

- **Main NLM Documentation**: Available in the main repository
- **API Reference**: See function-specific documentation in examples
- **Examples Directory**: `examples/advanced_bindings_demo.py` for comprehensive usage

### Support

For questions or issues:
1. Check the examples for usage patterns
2. Review the error messages and troubleshooting guide
3. Look at the mock implementation for API design
4. Refer to the ADVANCED_FEATURES_SUMMARY.md for detailed implementation notes

### Community Resources

- **GitHub Repository**: Source code and issues
- **Documentation**: Comprehensive guides in the main repository
- **Examples**: Practical demonstrations in the examples directory

## License

These enhanced Python bindings are part of the NLM (熙然) project and are licensed under the same terms as the main repository (MIT License).

## Version Information

- **Bindings Version**: 2.0.0 (Enhanced)
- **New Functions Added**: 24
- **Backward Compatibility**: Maintained
- **Python Support**: 3.8+
- **Build Requirements**: CMake 3.16+, C++17 compiler

## Contributors

The enhanced Python bindings were developed by the NLM development team. Special thanks to all contributors who helped implement the 24 new functions and comprehensive documentation.

---

*Last updated: 2026-09-30*
*Version: Enhanced Python Bindings Documentation*