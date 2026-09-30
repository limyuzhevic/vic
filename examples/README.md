# NLM Python Bindings - Examples Documentation

This directory contains examples demonstrating the new enhanced Python bindings for NLM (Neural Learning Machine).

## Overview

These examples showcase all the new functions added to the Python bindings as requested:

### New Functions Added (24 total)

#### 1. Convenience Factory Functions (4)
- `create_brain_with_config()` - Create brain with config validation
- `create_brain_from_json()` - Load brain configuration from JSON
- `create_brain_from_args()` - Parse command line arguments
- `create_simple_agent()` - Quick agent setup with brain, world, and basic training

#### 2. Utility Functions (4)
- `validate_brain()` - Validate brain configuration and state
- `save_brain_snapshot()` - Save brain state with timestamp
- `load_brain_snapshot()` - Load brain state from snapshot
- `benchmark_simulation()` - Measure performance of simulation steps

#### 3. Advanced Features (4)
- `run_training_episode()` - Complete training episode with reward tracking
- `run_simulation_batch()` - Run multiple simulations in parallel
- `monitor_brain_development()` - Monitor developmental stages over time
- `analyze_neural_activity()` - Analyze firing patterns and connectivity

#### 4. Configuration Management (4)
- `export_config_to_json()` - Export configuration as JSON string
- `import_config_from_json()` - Import configuration from JSON string
- `validate_config_schema()` - Validate configuration against schema
- `merge_config()` - Merge multiple configurations

#### 5. Simulation Control (4)
- `pause_simulation()` - Temporarily pause brain activity
- `resume_simulation()` - Resume paused simulation
- `step_multiple()` - Execute multiple simulation steps at once
- `run_to_target()` - Run simulation until target reached

#### 6. Performance Monitoring (4)
- `get_memory_usage()` - Get current memory usage
- `get_cpu_usage()` - Get CPU usage during simulation
- `profile_simulation()` - Profile simulation performance
- `generate_report()` - Generate comprehensive simulation report

## Available Examples

### 1. `advanced_bindings_demo.py`
**Comprehensive demonstration of all 24 new functions**

- Demonstrates all new Python bindings in action
- Organized by function categories
- Shows practical usage patterns
- Includes error handling and validation

**Run with:**
```bash
python3 examples/advanced_bindings_demo.py
```

**Features:**
- Complete walkthrough of all 24 functions
- Real-world usage examples
- Performance demonstrations
- Error handling examples

### 2. `advanced_features.py`
**Alternative demonstration of new features**

- Similar to advanced_bindings_demo.py but with different focus
- Emphasizes different aspects of the new API
- Provides additional usage patterns

**Run with:**
```bash
python3 examples/advanced_features.py
```

## Installation

### Prerequisites

- Python 3.8+
- CMake 3.16+
- A C++ compiler (gcc, clang, etc.)

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

### Quick Test

After building, verify the bindings work:
```bash
python3 -c "import pynlm; print('NLM Python bindings loaded successfully')"
```

## Running Examples

### Quick Verification

Run the simple test script to verify the bindings:
```bash
python3 test_bindings.py
```

### Advanced Features Demo

Run the comprehensive demo:
```bash
python3 examples/advanced_bindings_demo.py
```

### Alternative Demo

Run the advanced features example:
```bash
python3 examples/advanced_features.py
```

### Mock Implementation

Run the mock implementation (doesn't require C++ compilation):
```bash
python3 mock_advanced_bindings.py
```

## API Documentation

Each function has comprehensive docstrings that explain:
- Purpose and use case
- Parameters and their types
- Return values
- Error conditions

### Example Usage Patterns

Here are some common patterns:

#### Creating a Brain

```python
import pynlm

# Using default config
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Using convenience function with validation
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 5)
brain = pynlm.createBrainWithConfig(config)
```

#### Training an Agent

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run training episode
results = pynlm.runTrainingEpisode(agent, world, 100)
print(f"Total reward: {results['total_reward']}")
```

#### Performance Monitoring

```python
import pynlm

# Benchmark performance
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run benchmark
benchmark = pynlm.benchmarkSimulation(brain, 100)
print(f"Steps per second: {benchmark['steps_per_second']}")

# Profile simulation
profile = pynlm.profile_simulation(brain, 2000)
print(f"Average step time: {profile['avg_time_ms']} ms")
```

#### Configuration Management

```python
import pynlm

# Create and export config
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)

# Export as JSON
json_str = pynlm.exportConfigToJson(config)

# Import back
imported = pynlm.importConfigFromJson(json_str)
```

#### Simulation Control

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation in chunks
pynlm.stepMultiple(brain, 100)  # 100 steps
# ... do other work ...
pynlm.stepMultiple(brain, 100)  # Another 100 steps

# Pause/resume
pynlm.pauseSimulation(brain)
# ... pause work ...
pynlm.resumeSimulation(brain)
```

## Error Handling

All new functions include proper error handling:

1. **Validation Errors**: Functions check parameters and throw `RuntimeError` on invalid input
2. **File I/O Errors**: Save/load operations report success/failure
3. **Configuration Errors**: Invalid configurations are detected early
4. **Type Safety**: Python type checking ensures correct usage

### Exception Handling

```python
import pynlm

try:
    # This might fail
    brain = pynlm.createBrainWithConfig(invalid_config)
except pynlm.RuntimeError as e:
    print(f"Brain creation failed: {e}")
```

## Performance Considerations

### Memory Management

- All functions use smart pointers for automatic memory management
- Large simulations should use batch operations when possible
- Memory snapshots can be saved to disk for long-running simulations

### Simulation Control

- Use `step_multiple()` for efficient batch execution
- Pause/resume is useful for interactive debugging
- Monitor performance with profiling to identify bottlenecks

### Parallel Processing

- Use `run_simulation_batch()` for running multiple independent simulations
- Each simulation gets its own memory space
- Results are collected in a structured format for analysis

## Testing

### Unit Tests

The examples serve as functional tests. For unit testing, you can:

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

### Integration Tests

Run the full demo:
```bash
python3 examples/advanced_bindings_demo.py
```

This tests all 24 new functions end-to-end.

## Tips & Tricks

### Common Pitfalls

1. **Forgetting to initialize the brain**: Always call `initialize()` before using a brain
2. **Ignoring configuration validation**: Use `createBrainWithConfig()` for validation
3. **Memory leaks**: Python bindings handle memory automatically
4. **Invalid JSON**: Use proper JSON strings for configuration import/export

### Performance Optimization

1. **Batch operations**: Use `stepMultiple()` instead of many small steps
2. **Reuse objects**: Reuse brain/world objects when possible
3. **Profile first**: Use `profile_simulation()` to identify bottlenecks
4. **Monitor memory**: Use `get_memory_usage()` to track memory consumption

### Best Practices

1. **Separate setup and execution**: Configure once, run many times
2. **Use snapshots**: Save intermediate states for debugging
3. **Monitor developmental stages**: Use `monitorBrainDevelopment()` for long simulations
4. **Analyze patterns**: Use `analyzeNeuralActivity()` for insights into brain behavior

## Future Extensions

These bindings provide a foundation for more advanced features:

- **Distributed computing**: Run simulations across multiple machines
- **GPU acceleration**: Leverage GPU for large-scale neural simulations
- **Web interfaces**: Create web-based simulation dashboards
- **Real-time monitoring**: Monitor neural activity in real-time

## Support

For questions or issues:
1. Check the NLM documentation in the main repository
2. Look at the examples for usage patterns
3. Refer to the docstrings in the bindings for detailed API documentation

## License

These examples are part of the NLM project and are licensed under the same terms as the main repository.

## Version Information

- **Examples Version**: 1.0
- **Python Bindings**: Enhanced with 24 new functions
- **Supported Python**: 3.8+
- **Build System**: CMake 3.16+

---

*Last updated: 2026-09-30*
*Documentation for NLM Python Bindings Enhanced Examples*