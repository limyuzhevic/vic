# Enhanced Python Bindings for NLM

## Summary

This document describes the comprehensive enhancements made to the Python bindings (`bindings.cpp`) in the NLM (Neural Learning Machine) project. The enhanced bindings provide a powerful, Pythonic API for neural simulation with advanced features for scientific computing.

## Key Enhancements Implemented

### 1. Context Managers for Brain Simulations

**Features:**
- `BrainContext`: Context manager for automatic brain initialization and cleanup
- `WorldSimulation`: Context manager for world simulation lifecycle management
- `AgentSimulation`: Context manager for complete agent simulation
- Built-in `with` statement support for automatic resource management

**Example:**
```python
with pynlm.BrainContext(pynlm.createDefaultConfig()) as brain:
    brain.simulate(1000, time_step=0.1)
```

### 2. Enhanced Error Handling and Exception Translation

**Features:**
- Custom Python exception classes: `PythonError`, `ConfigurationError`, `SimulationError`, `MemoryError`
- Seamless translation from C++ exceptions to Python exceptions
- Detailed error messages with context
- Graceful handling of runtime errors and invalid operations

**Example:**
```python
try:
    brain.step(100)
except SimulationError as e:
    print(f"Simulation error: {e}")
except ConfigurationError as e:
    print(f"Configuration error: {e}")
```

### 3. Comprehensive Type Hints and Documentation

**Features:**
- Detailed docstrings for all classes and methods
- Type hints for function parameters and return values
- Pythonic method aliases (e.g., `step_with_time` instead of `step`)
- Parameter descriptions and usage examples
- Module-level documentation with feature overview

**Example:**
```python
"""
Brain simulation context manager

Args:
    config: NLM configuration object
    
Yields:
    Brain instance ready for simulation
    
Example:
    with pynlm.BrainContext(config) as brain:
        brain.simulate(1000)
"""
```

### 4. Visualization and Plotting Utilities

**Features:**
- `plot_neuron_activity()`: Generate firing rate plots
- `plot_region_activity()`: Plot regional neural activity
- `create_visualization()`: Import visualization data from CSV
- File output support for various plot formats
- Statistical plotting with customizable options

**Example:**
```python
# Generate neuron activity plot
firing_rates = pynlm.plot_neuron_activity(brain, steps=1000)
pynlm.plot_neuron_activity(brain, 500, "neuron_activity.png")

# Import visualization data
data_points = pynlm.create_visualization("data.csv", "scatter")
```

### 5. Debugging Tools and Performance Monitoring

**Features:**
- `profile_brain_step()`: Measure simulation performance
- `debug_brain_state()`: Get detailed brain state information
- Real-time profiling and timing measurements
- Memory usage tracking
- Performance benchmarking utilities

**Example:**
```python
# Profile brain performance
duration, avg_rate = pynlm.profile_brain_step(brain, 100)
print(f"Duration: {duration}ms, Average rate: {avg_rate}")

# Debug brain state
debug_info = pynlm.debug_brain_state(brain)
print(f"Total neurons: {debug_info['total_neurons']}")
```

### 6. Configuration Validation Tools

**Features:**
- `validate_config()`: Check configuration for errors
- Required key validation
- Range and type checking
- Detailed error messages for invalid configurations
- Automatic default value suggestions

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)

errors = pynlm.validate_config(config)
if errors:
    for error in errors:
        print(f"Configuration error: {error}")
else:
    print("Configuration is valid")
```

### 7. Memory Management Utilities

**Features:**
- `get_memory_usage()`: Estimate current memory consumption
- `clear_memory()`: Force garbage collection
- Memory tracking for long-running simulations
- Resource monitoring utilities

**Example:**
```python
# Monitor memory usage
memory = pynlm.get_memory_usage()
print(f"Memory usage: {memory} bytes")

# Clear memory
pynlm.clear_memory()
```

### 8. Simulation Statistics and Analysis Tools

**Features:**
- `analyze_simulation()`: Comprehensive statistical analysis
- E/I (Excitation/Inhibition) balance calculation
- Mean, standard deviation, min, max statistics
- Correlation analysis functions
- Pattern detection utilities

**Example:**
```python
# Analyze simulation statistics
stats = pynlm.analyze_simulation(brain, 100)
mean_rate, std_rate, min_rate, max_rate, firing_rates = stats

# Calculate E/I balance
ratio, balance_desc = pynlm.calculate_ei_balance(brain)
print(f"E/I balance: {ratio:.2f} ({balance_desc})")
```

### 9. Batch Operations and Utility Functions

**Features:**
- `batch_step()`: Efficient batch simulation execution
- `export_simulation_data()`: Export data to CSV format
- `import_simulation_data()`: Import data from CSV files
- High-performance batch processing
- Data serialization utilities

**Example:**
```python
# Batch simulation steps
spike_counts = pynlm.batch_step(brain, 1000)

# Export simulation data
pynlm.export_simulation_data(brain, "simulation.csv", 100)

# Import simulation data
data = pynlm.import_simulation_data("data.csv")
```

### 10. Scientific Python Integration

**Features:**
- Seamless numpy array integration
- `numpy_array()`: Convert Python lists to numpy arrays
- `from_numpy()`: Convert numpy arrays to Python lists
- Array manipulation utilities
- Scientific computing support

**Example:**
```python
import numpy as np

# Convert to numpy array
numpy_arr = pynlm.numpy_array([1.0, 2.0, 3.0, 4.0, 5.0])
print(f"Type: {type(numpy_arr)}")

# Convert from numpy array
data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
python_list = pynlm.from_numpy(data)
```

### 11. Enhanced Class Methods and Pythonic APIs

**Features:**
- Dictionary conversion methods (`to_dict()`) for all classes
- Equality comparison operators
- Convenient method aliases
- Enhanced getters and setters
- Pythonic method naming conventions

**Example:**
```python
# Dictionary conversion
brain_dict = brain.to_dict()
agent_dict = agent.to_dict()
world_dict = world.to_dict()

# Enhanced getters
data = percept.get_all_signals()

# Convenience methods
brain.step_with_time(100, 0.1)
agent.process_input(percept)
```

### 12. Additional Convenience Functions

**Features:**
- `run_complete_simulation()`: High-level simulation wrapper
- Example demonstration functions
- Helper utilities for common workflows
- Debugging and testing utilities

**Example:**
```python
# Run complete simulation
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

results = pynlm.run_complete_simulation(config, world, 500, verbose=True)
spike_counts, curiosity_levels = results
```

## Usage Examples

### Example 1: Simplest Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Complete Agent Simulation with Context Manager

```python
import pynlm

# Create complete simulation
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()

with pynlm.BrainContext(config) as brain:
    with pynlm.AgentSimulation(brain) as agent:
        agent.initialize(world)
        
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### Example 3: Visualization and Analysis

```python
import pynlm

# Create and run brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Generate activity plot and analyze
firing_rates = pynlm.plot_neuron_activity(brain, 1000, "activity.png")
stats = pynlm.analyze_simulation(brain, 100)

# Debug and profile
debug_info = pynlm.debug_brain_state(brain)
duration, avg_rate = pynlm.profile_brain_step(brain, 100)

# Calculate E/I balance
ratio, description = pynlm.calculate_ei_balance(brain)
```

## Installation and Building

### Prerequisites
- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11 >= 2.11.0
- scikit-build-core

### Build Instructions

1. **From Source (Development Environment)**:
```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install -e .

# Or in development mode
pip install -e .
```

2. **Using pyproject.toml (Modern Python Packaging)**:
```bash
# Install with scikit-build-core
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Building with CMake (Alternative)

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)
```

### Verification

After building, verify the installation:

```python
import pynlm

# Check version and metadata
print(f"NLM Version: {pynlm.__version__}")
print(f"Description: {pynlm.__description__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Brain neurons: {brain.getTotalNeuronCount()}")
```

## Migration Guide

### From Basic to Enhanced API

**Before (Basic):**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

**After (Enhanced):**
```python
import pynlm

# Using context manager (recommended)
with pynlm.BrainContext(pynlm.createDefaultConfig()) as brain:
    brain.simulate(100)

# Or with explicit initialization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.simulate(100)  # Use convenience method
```

### Upgrading Existing Code

The enhanced bindings maintain backward compatibility while adding new features:

1. **Existing method calls** continue to work unchanged
2. **New context managers** provide automatic resource management
3. **New convenience methods** replace verbose boilerplate code
4. **Enhanced error handling** provides better debugging information
5. **New validation utilities** help catch configuration errors early

## Technical Implementation Details

### Architecture

The enhanced bindings follow these design principles:

1. **RAII Integration**: Context managers ensure proper resource cleanup
2. **Error Translation**: C++ exceptions are automatically converted to Python exceptions
3. **Pythonic API**: Methods follow Python conventions and naming patterns
4. **Type Safety**: Type hints and runtime checking prevent common errors
5. **Performance**: Batch operations and optimized code paths maintain performance
6. **Extensibility**: Modular design allows for easy extension

### Code Quality

- **Documentation**: Comprehensive docstrings with examples
- **Error Handling**: Graceful error handling with informative messages
- **Testing**: Example functions demonstrate proper usage
- **Compatibility**: Maintains backward compatibility with existing code
- **Performance**: Optimized for both development and production use

### Performance Considerations

- **Memory Management**: Automatic garbage collection and memory tracking
- **Batch Operations**: Efficient processing of multiple operations
- **Optimized Methods**: High-performance alternatives to verbose API calls
- **Resource Management**: Context managers ensure proper cleanup

## Acknowledgements

This enhancement builds upon the foundation provided by the original NLM Python bindings. The additional features were developed to provide a more comprehensive and user-friendly experience for scientific computing and neural simulation research.

The enhanced bindings maintain the core functionality while adding powerful new capabilities for:

- Interactive simulation and analysis
- Research and experimentation
- Educational purposes
- Production deployment

## Future Enhancements

Potential future improvements include:

1. **Real-time Visualization**: Interactive plotting and live monitoring
2. **Machine Learning Integration**: Enhanced ML model support
3. **Parallel Processing**: Multi-threading and GPU acceleration
4. **Cloud Integration**: Distributed computing capabilities
5. **Advanced Analytics**: Machine learning for pattern detection
6. **Plugin Architecture**: Extensible plugin system for custom functionality

---

*For more information and documentation, visit the NLM project repository.*
