# Enhanced Python Bindings for NLM

## Overview

This directory contains the enhanced Python bindings for the NLM (Neural Learning Machine) framework. The enhanced bindings provide significant improvements over the original implementation, including better error handling, enhanced APIs, advanced training utilities, visualization support, model export capabilities, and improved user experience.

## Key Features

### 1. Enhanced API Design
- Modular architecture with separate namespaces (config, brain, training, visualization, export, memory)
- Comprehensive docstrings for all bindings
- Context managers for automatic resource cleanup
- Enhanced error handling with custom exceptions

### 2. Advanced Training Utilities
- `run_episode()`: Run single training episodes with detailed metrics
- `train()`: Full training loop with callbacks and stop conditions
- `evaluate()`: Performance evaluation with reward statistics
- `EpisodeResult`: Comprehensive metrics collection and reporting

### 3. Visualization and Dashboard
- `plot_training_metrics()`: Generate training plots
- `visualize_brain()`: Brain structure visualization
- `create_training_dashboard()`: Web-based training dashboard
- Integration with matplotlib and web technologies

### 4. Model Export Capabilities
- `export_to_tensorflow()`: TensorFlow SavedModel export
- `export_to_onnx()`: ONNX format export
- `export_to_pickle()`: Python pickle export
- Custom export options and validation

### 5. Memory Management
- `optimize_memory()`: Python garbage collection optimization
- `get_memory_stats()`: Memory usage statistics
- `clone_brain()`: Deep copy of brain state
- `serialize_brain()`/`deserialize_brain()`: Brain serialization

### 6. Error Handling and Validation
- Custom exceptions: NLMError, ValidationError, TrainingError, SerializationError
- Comprehensive parameter validation
- Type checking and safety
- Configuration validation

### 7. Configuration Management
- `create_default_config()`: Create default configuration
- `validate_config()`: Configuration validation
- `get_default_config()`: Access default configuration
- `merge_config()`: Merge configuration dictionaries

### 8. Context Manager Support
- `BrainSession`: Automatic resource cleanup with context manager support
- Exception safety
- Nested context support
- Automatic reset and cleanup

## Files

### Core Bindings
- `bindings.cpp`: Original Python bindings (enhanced)
- `bindings_enhanced.cpp`: Enhanced Python bindings module
- `bindings_enhanced.h`: Header file for enhanced bindings

### Build System
- `CMakeLists.txt`: Enhanced build system with both original and enhanced modules

### Documentation
- `README.md`: This file
- `ENHANCED_BINDINGS.md`: Comprehensive documentation for enhanced bindings
- `BUILD_ENHANCED.md`: Build instructions for enhanced bindings
- `PYTHON_BINDINGS_IMPROVEMENTS.md`: Detailed summary of improvements

### Examples
- `example_enhanced.py`: Comprehensive usage examples
- `test_bindings_enhanced.cpp`: Unit tests for enhanced bindings

### Additional Documentation
- `../ENHANCED_BINDINGS.md`: Enhanced bindings documentation
- `../README.md`: Project root README
- `../PYTHON_BINDINGS_IMPROVEMENTS.md`: Detailed improvements summary

## Building

### Build Enhanced Bindings Only

```bash
mkdir build_enhanced
cd build_enhanced
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON -DBUILD_ENHANCED=ON
make -j$(nproc)
```

### Build Both Original and Enhanced

```bash
mkdir build_both
cd build_both
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON -DBUILD_ENHANCED=ON
make -j$(nproc)
```

### Build Original Only

```bash
mkdir build_original
cd build_original
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON -DBUILD_ENHANCED=OFF
make -j$(nproc)
```

### Installation

```bash
cd build_enhanced
cmake --install . --prefix "$(python -c 'import site; print(site.getsitepackages()[0])')"
```

## Usage Examples

### Basic Usage (Original API)

```python
import pynlm as nlm

config = nlm.Config()
brain = nlm.Brain(config)
brain.initialize()

world = nlm.SimpleWorld()
world.configure(100, 100, 16, 16)

agent = nlm.create_agent_brain(brain)
agent.initialize(world)

for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step, float(step) * 0.1)
    action = brain.produceAction()
```

### Advanced Usage (Enhanced API)

```python
import pynlm_enhanced as nlm

# Create enhanced configuration
config = nlm.config.create_training_config(
    learning_rate=0.001,
    hidden_neurons=1000,
    population_size=100
)

# Create brain with validation
brain = nlm.create_brain(config, initialize_now=True)

# Create world and agent
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Training with callbacks
def episode_callback(episode_num, result):
    print(f"Episode {episode_num}: Reward={result.total_reward:.2f}, Success={'✓' if result.success else '✗'}")

metrics = nlm.train(
    brain=brain,
    agent=agent,
    world=world,
    num_episodes=1000,
    steps_per_episode=100,
    callback=episode_callback
)

# Visualization
nlm.visualization.plot_training_metrics(metrics)
nlm.visualization.create_training_dashboard(metrics, "training_dashboard")

# Export model
nlm.export.export_to_pickle(brain, "model.pkl")
```

### Context Manager Usage

```python
import pynlm_enhanced as nlm

# Create components
config = nlm.create_default_config()
brain = nlm.create_brain(config)
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)

# Use context manager for automatic cleanup
with nlm.create_brain_session(brain, agent, world) as session:
    for step in range(50):
        success = session.step(step, float(step) * 0.1)
        if not success:
            break
        
        # Get current state
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = brain.produceAction()

# Session automatically cleaned up
print("Training session completed and cleaned up")
```

## Testing

```bash
cd /path/to/nlm/python

# Run C++ tests
./test_bindings_enhanced

# Run Python examples
python example_enhanced.py
```

## Additional Documentation

- `ENHANCED_BINDINGS.md`: Comprehensive documentation for enhanced bindings
- `BUILD_ENHANCED.md`: Build instructions for enhanced bindings
- `PYTHON_BINDINGS_IMPROVEMENTS.md`: Detailed summary of improvements

## Backward Compatibility

The enhanced bindings maintain full backward compatibility with the original API:

- All original functions work exactly as before
- Original module name: `pynlm`
- Enhanced module name: `pynlm_enhanced`
- No breaking changes to existing code

## Migration Guide

To upgrade from the original to enhanced API:

1. **Configuration**: Use `nlm.config.create_training_config()` for enhanced configurations
2. **Brain Creation**: Use `nlm.create_brain()` for enhanced brain creation with validation
3. **Training**: Use `nlm.train()` for advanced training with callbacks and metrics
4. **Visualization**: Use `nlm.visualization.*` functions for enhanced visualization
5. **Export**: Use `nlm.export.*` functions for model export
6. **Memory Management**: Use `nlm.memory.*` functions for memory optimization
7. **Context Management**: Use `nlm.create_brain_session()` for automatic cleanup

## Support

For help with the enhanced Python bindings:

1. **Documentation**: Check `ENHANCED_BINDINGS.md` and `BUILD_ENHANCED.md`
2. **Examples**: Look at `example_enhanced.py`
3. **Testing**: Run the tests with `./test_bindings_enhanced`
4. **Build Issues**: Check the build logs and CMake configuration

## License

MIT License. See ../LICENSE for details.
