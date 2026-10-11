# NLM Python Bindings - Enhanced API

This is an enhanced version of the NLM (Neural Learning Machine) Python bindings
with Pythonic properties, advanced features, and comprehensive error handling.

## Key Features

### 1. Pythonic Properties and Accessors
- **brain.spike_count** (alias for `brain.getTotalSpikeCount()`)
- **brain.synapse_count** (alias for `brain.getTotalSynapseCount()`)
- **brain.active_neurons** (alias for `brain.getActiveNeuronCount()`)
- **brain.firing_neurons** (alias for `brain.getFiringNeuronCount()`)
- **brain.ei_ratio** (alias for `brain.getExcitationInhibitionRatio()`)
- **brain.region_names** (convenience for getting region names)
- **agent.reward_modulation_enabled** (property access)
- **agent.curiosity_enabled** (property access)

### 2. Advanced Features for Advanced Users
- **brain.step_batch(steps)** - Process multiple steps at once
- **agent.process_sensory_batch(percepts)** - Process multiple sensory inputs
- **brain.get_summary()** - Get comprehensive brain status
- **world.configure_advanced(**kwargs)** - More flexible world configuration
- **brain.save_to_file(filename)** - Convenience wrapper for save()
- **brain.load_from_file(filename)** - Convenience wrapper for load()

### 3. Better Error Handling and Validation
- **PynlmError** - Base exception class
- **InvalidConfigurationError** - Configuration errors
- **NotInitializedError** - State errors
- **InvalidInputError** - Input validation errors

### 4. Enhanced Integration Features
- **pynlm.utils.get_version_info()** - Detailed version information
- **pynlm.utils.benchmark_brain(brain, steps)** - Performance benchmarking
- **pynlm.utils.compare_brains(brain1, brain2)** - Compare brain states
- **pynlm.utils.validate_brain(brain)** - Validate brain object
- **pynlm.utils.get_memory_usage()** - Get memory usage information

### 5. Memory and Performance Optimizations
- **brain.step_batch()** for efficient batch processing
- **agent.process_sensory_batch()** for parallel sensory processing
- **pynlm.utils.benchmark_brain()** for performance monitoring
- **pynlm.utils.get_memory_usage()** for memory monitoring

## Quick Start

### Basic Usage

```python
import pynlm

# Create configuration
config = pynlm.Config()
config.set("simulation_max_steps", 10000)

# Create brain
brain = pynlm.Brain(config)
brain.initialize()

# Use Pythonic properties
print(f"Spike count: {brain.spike_count}")
print(f"Active neurons: {brain.active_neurons}")

# Run batch of steps
brain.step_batch(100)

# Get brain summary
summary = brain.get_summary()
print(f"Brain summary: {summary}")
```

### Agent Integration

```python
import pynlm

# Create brain and agent
brain = pynlm.Brain()
agent = pynlm.create_agent_brain(brain)

# Configure agent
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Process sensory input
percept = pynlm.SensoryPercept()
percept.setVision([0.1, 0.2, 0.3])
agent.process_sensory_input(percept)

# Process multiple sensory inputs at once
percepts = [pynlm.SensoryPercept() for _ in range(10)]
agent.process_sensory_batch(percepts)
```

### Error Handling

```python
import pynlm

brain = pynlm.Brain()

try:
    if brain.step():
        print("Step successful")
except pynlm.InvalidConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.NotInitializedError as e:
    print(f"Not initialized: {e}")
except pynlm.InvalidInputError as e:
    print(f"Invalid input: {e}")
except pynlm.PynlmError as e:
    print(f"NLM error: {e}")
```

### Utility Functions

```python
import pynlm
import pynlm.utils

# Get version info
version_info = pynlm.utils.get_version_info()
print(f"Version: {version_info['version']}")

# Validate brain
if pynlm.utils.validate_brain(brain):
    print("Brain is valid")

# Benchmark performance
results = pynlm.utils.benchmark_brain(brain, steps=1000)
print(f"Average step time: {results['avg_step_time_ms']:.2f} ms")

# Compare two brains
comparison = pynlm.utils.compare_brains(brain1, brain2)
if comparison['has_differences']:
    print("Brains have differences")

# Get memory usage
memory = pynlm.utils.get_memory_usage()
print(f"Memory usage: {memory['rss_mb']:.2f} MB RSS")
```

### Context Manager

```python
import pynlm

# Use context manager for simulation setup/cleanup
try:
    with pynlm.simulation_manager() as sim:
        sim.brain.initialize()
        sim.brain.step(100)
        print("Simulation completed successfully")
except Exception as e:
    print(f"Simulation error: {e}")
```

## Error Handling

The enhanced bindings include comprehensive error handling:

- **PynlmError**: Base exception for all NLM errors
- **InvalidConfigurationError**: Thrown for configuration issues
- **NotInitializedError**: Thrown when objects are used before initialization
- **InvalidInputError**: Thrown for invalid input parameters

All errors include descriptive messages and, where appropriate, additional context.

## Building and Installation

The bindings are built using scikit-build with pybind11. To build:

```bash
pip install -e .
```

For development:

```bash
pip install -e .[dev]
pytest
```

## Testing

Run the test suite with:

```bash
pytest
```

Or run the example script:

```bash
python examples/basic_usage.py
```

## Examples

Several example scripts are provided in the `examples/` directory:

- `basic_usage.py`: Comprehensive demonstration of the enhanced API
- `error_handling.py`: Demonstration of error handling patterns
- `batch_operations.py`: Demonstration of batch processing features
- `utility_functions.py`: Demonstration of utility functions

## Python Conventions

The enhanced bindings follow Python conventions:

- **PEP 8**: Code follows Python style guidelines
- **Docstrings**: All public methods have comprehensive docstrings
- **Type hints**: Methods include type hints where appropriate
- **Properties**: Pythonic properties for commonly used attributes
- **Context managers**: Support for context manager patterns
- **Error handling**: Comprehensive error handling with descriptive messages

## Backward Compatibility

The enhanced bindings maintain backward compatibility:

- All existing method names and signatures are preserved
- New features are additive and do not break existing code
- Existing code will continue to work without modification

## Performance Considerations

The enhanced bindings include performance optimizations:

- **Batch operations**: Efficient processing of multiple steps/sensory inputs
- **Memory management**: Proper cleanup and memory monitoring
- **Caching**: Where appropriate, results are cached to avoid redundant computations
- **Asynchronous operations**: Support for asynchronous processing where beneficial

## Limitations

- Some features may require additional dependencies (e.g., psutil for memory monitoring)
- Performance may vary depending on system resources
- Some advanced features may have platform-specific limitations

## Contact

For questions, issues, or suggestions:

- Report issues at: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Discord: https://discord.gg/nlm-community

## License

MIT License. See LICENSE file for details.

---

Generated with enhanced Pythonic NLM Python bindings.
This implementation provides a modern, Pythonic interface to the NLM C++ neural simulation framework.
