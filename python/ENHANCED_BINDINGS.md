# Enhanced NLM Python Bindings

## Overview

This document describes the enhanced Python bindings for the NLM (Neural Learning Machine) framework. These bindings provide a comprehensive interface to the neural simulation capabilities, including advanced utilities, error handling, and high-level APIs for training and evaluation.

## Key Features

### 1. Enhanced API Design

**Modular Structure:**
- Separated into logical modules (`config`, `brain`, `training`, `visualization`, `export`, `memory`)
- Each module provides specialized functionality
- Clean, Pythonic interface with comprehensive documentation

**Pythonic API:**
- Context managers for automatic resource cleanup
- Property access and validation
- Type conversion utilities
- Enhanced error handling with custom exceptions

### 2. Advanced Training Utilities

**Training Framework:**
- `run_episode()`: Run single training episodes with detailed metrics
- `train()`: Full training loop with callbacks and stop conditions
- `evaluate()`: Performance evaluation with reward statistics
- `EpisodeResult`: Detailed metrics collection and reporting

**Metrics and Monitoring:**
- Reward statistics (mean, std, max, min)
- Success tracking and error handling
- Action value tracking
- Customizable metrics collection

### 3. Visualization and Dashboard

**Visualization Tools:**
- `plot_training_metrics()`: Generate training plots
- `visualize_brain()`: Brain structure visualization
- `create_training_dashboard()`: Web-based training dashboard
- Integration with matplotlib and web technologies

### 4. Model Export Capabilities

**Export Formats:**
- `export_to_tensorflow()`: TensorFlow SavedModel export
- `export_to_onnx()`: ONNX format export
- `export_to_pickle()`: Python pickle export
- Custom export options and validation

### 5. Memory Management

**Memory Utilities:**
- `optimize_memory()`: Python garbage collection optimization
- `get_memory_stats()`: Memory usage statistics
- `clone_brain()`: Deep copy of brain state
- `serialize_brain()`/ `deserialize_brain()`: Brain serialization

### 6. Error Handling and Validation

**Custom Exceptions:**
- `NLMError`: Base exception for all NLM errors
- `ValidationError`: Configuration and input validation errors
- `TrainingError`: Training-specific errors
- `SerializationError`: File I/O and serialization errors

**Input Validation:**
- Comprehensive parameter validation
- Type checking and safety
- Configuration validation

### 7. Configuration Management

**Enhanced Config API:**
- `create_default_config()`: Create default configuration
- `validate_config()`: Configuration validation
- `get_default_config()`: Access default configuration
- `merge_config()`: Merge configuration dictionaries

## Installation

### Build from Source

```bash
# Clone the repository
cd /path/to/nlm

# Build with enhanced bindings
mkdir build
mkdir build/python

cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON

# Build the enhanced module specifically
cd python
mkdir build_enhanced
cd build_enhanced
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j$(nproc)
```

### Using pip

```bash
# Install via pip (if available)
pip install pynlm_enhanced
```

## Quick Start Guide

### 1. Basic Usage

```python
import pynlm_enhanced as nlm

# Create default configuration
config = nlm.create_default_config()

# Configure with training settings
config = nlm.config.create_training_config(
    learning_rate=0.01,
    hidden_neurons=1000,
    population_size=100
)

# Create brain with validation
brain = nlm.create_brain(config, initialize_now=True)

# Create world and agent
world = nlm.create_simple_world()
world.configure(width=100, height=100, vision_width=16, vision_height=16)

agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Create context manager for safe usage
with nlm.create_brain_session(brain, agent, world) as session:
    # Run a single step
    session.step(1, 0.0)
    
    # Get observations
    percept = world.getSensoryPercept()
    
    # Process and get action
    agent.processSensoryInput(percept)
    action = brain.produceAction()
```

### 2. Training with Enhanced APIs

```python
import pynlm_enhanced as nlm

# Create components
config = nlm.config.create_training_config(
    learning_rate=0.001,
    hidden_neurons=2000,
    population_size=200
)

brain = nlm.create_brain(config)
world = nlm.create_simple_world()
agent = nlm.create_agent_brain(brain)

# Enable training features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Define callback function
def episode_callback(episode_num, result):
    print(f"Episode {episode_num}: Reward = {result.total_reward:.2f}, "
          f"Success = {result.success}")

# Run training with metrics
print("Starting training...")
metrics = nlm.train(
    brain=brain,
    agent=agent,
    world=world,
    num_episodes=1000,
    steps_per_episode=100,
    render=False,
    callback=episode_callback,
    stop_condition=lambda: False  # Custom stop condition
)

# Save metrics
nlm.save_metrics(metrics, "training_metrics.json")
```

### 3. Visualization

```python
import pynlm_enhanced as nlm

# Load or create metrics
metrics = nlm.load_metrics("training_metrics.json")

# Generate training plots
nlm.visualization.plot_training_metrics(
    metrics,
    output_file="training_progress.png",
    show_plots=True
)

# Visualize brain structure
nlm.visualization.visualize_brain(
    brain,
    output_file="brain_structure.png",
    show_neurons=True,
    show_connections=True,
    show_regions=False
)

# Create interactive dashboard
nlm.visualization.create_training_dashboard(
    metrics,
    output_dir="training_dashboard"
)
```

### 4. Model Export

```python
import pynlm_enhanced as nlm

# Export to TensorFlow
nlm.export.export_to_tensorflow(
    brain,
    "tensorflow_model",
    input_names=["sensory_input"],
    output_names=["action_output"]
)

# Export to ONNX
nlm.export.export_to_onnx(
    brain,
    "brain_model.onnx",
    input_shapes=[[1, 3, 32, 32]],
    input_names=["vision"],
    output_shapes=[[1, 10]],
    output_names=["action"]
)

# Export to pickle
nlm.export.export_to_pickle(
    brain,
    "brain.pkl",
    include_weights=True,
    include_config=True
)
```

## API Reference

### Core Classes

#### Config
- `create_default_config()`: Create empty config
- `loadFromFile(filepath)`: Load from JSON
- `loadFromArgs(args)`: Load from command line args
- `saveToFile(filepath)`: Save to JSON
- `has(key)`: Check if key exists
- `getKeys()`: Get all keys
- `clear()`: Clear all entries

#### Brain
- `initialize()`: Initialize brain
- `step(step, time=0.0)`: Perform simulation step
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `produceAction()`: Generate motor action
- `getTotalNeuronCount()`: Get neuron count
- `getTotalSynapseCount()`: Get synapse count

#### AgentBrain
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted_reward)`: Apply reward
- `reset()`: Reset agent

#### SimpleWorld
- `configure(width, height, vision_width, vision_height)`: Configure world
- `reset()`: Reset world
- `update(timestep)`: Update simulation
- `getSensoryPercept()`: Get sensory perception
- `applyMotorCommand(cmd, current_time)`: Apply motor command

#### BrainSession (Context Manager)
- `__enter__()`: Enter context
- `__exit__()`: Exit context (auto-cleanup)
- `step(step, time)`: Execute step
- `reset()`: Reset session
- `initialize()`: Initialize session

### Utility Functions

#### Configuration
- `create_default_config()`: Create default config
- `validate_config(config)`: Validate configuration
- `get_default_config()`: Get default configuration
- `merge_config(config, overrides)`: Merge configuration

#### Training
- `run_episode(brain, agent, world, max_steps, render, callback)`: Run episode
- `train(brain, agent, world, num_episodes, steps_per_episode, render, callback, stop_condition)`: Training loop
- `evaluate(brain, agent, world, num_episodes, steps_per_episode)`: Evaluate performance
- `save_metrics(metrics, filepath, format)`: Save metrics

#### Visualization
- `plot_training_metrics(metrics, output_file, show_plots)`: Generate training plots
- `visualize_brain(brain, output_file, show_neurons, show_connections, show_regions)`: Visualize brain
- `create_training_dashboard(metrics, output_dir)`: Create web dashboard

#### Export
- `export_to_tensorflow(brain, output_path, input_names, output_names)`: Export to TensorFlow
- `export_to_onnx(brain, output_path, input_shapes, input_names, output_shapes, output_names)`: Export to ONNX
- `export_to_pickle(brain, output_path, include_weights, include_config)`: Export to pickle

#### Memory
- `optimize_memory()`: Optimize Python memory
- `get_memory_stats()`: Get memory statistics
- `clone_brain(brain)`: Clone brain
- `serialize_brain(brain)`: Serialize brain
- `deserialize_brain(data)`: Deserialize brain

### Custom Exceptions

#### NLMError
Base exception for all NLM errors.

#### ValidationError
Configuration and input validation errors.

#### TrainingError
Training-specific errors.

#### SerializationError
File I/O and serialization errors.

## Examples

### Complete Training Example

```python
import pynlm_enhanced as nlm
import time

def main():
    # Initialize
    config = nlm.config.create_training_config(
        learning_rate=0.001,
        hidden_neurons=1000,
        population_size=100
    )
    
    brain = nlm.create_brain(config)
    world = nlm.create_simple_world()
    world.configure(100, 100, 16, 16)
    agent = nlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Training loop with metrics
    metrics = nlm.train(
        brain=brain,
        agent=agent,
        world=world,
        num_episodes=500,
        steps_per_episode=200,
        render=False,
        callback=lambda ep, res: print(f"Episode {ep}: {res.total_reward:.2f}")
    )
    
    # Evaluate
    avg_reward = nlm.evaluate(brain, agent, world, 50)
    print(f"Average reward: {avg_reward:.2f}")
    
    # Save results
    nlm.save_metrics(metrics, "results.json")
    nlm.visualization.plot_training_metrics(metrics)
    
    # Export model
    nlm.export.export_to_pickle(brain, "final_brain.pkl")
    
    print("Training completed successfully!")

if __name__ == "__main__":
    main()
```

### Advanced Usage with Context Manager

```python
import pynlm_enhanced as nlm

def advanced_usage():
    # Create components
    config = nlm.create_default_config()
    brain = nlm.create_brain(config)
    world = nlm.create_simple_world()
    agent = nlm.create_agent_brain(brain)
    
    # Use context manager for safe resource management
    with nlm.create_brain_session(brain, agent, world) as session:
        # Run multiple steps with error handling
        for step in range(100):
            try:
                success = session.step(step, float(step) * 0.01)
                if not success:
                    print(f"Step {step} failed")
                    break
                    
                # Get observations and process
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
            except nlm.ValidationError as e:
                print(f"Validation error: {e}")
                break
            except nlm.TrainingError as e:
                print(f"Training error: {e}")
                break
            except Exception as e:
                print(f"Unexpected error: {e}")
                break
    
    # Session automatically cleaned up
    print("Session completed and cleaned up")

advanced_usage()
```

## Performance Considerations

### Memory Management

1. **Use Context Managers**: Always use `BrainSession` context manager for automatic cleanup
2. **Monitor Memory**: Use `memory.get_memory_stats()` to track memory usage
3. **Optimize Regularly**: Call `memory.optimize_memory()` periodically

### Training Performance

1. **Batch Processing**: Process multiple episodes in batch when possible
2. **Parallel Processing**: Use multiple brains for parallel training
3. **Memory Optimization**: Consider using `memory.clone_brain()` for state copying

### Export Performance

1. **Format Selection**: Choose appropriate export format for your use case
2. **Compression**: Consider compressed formats for storage
3. **Incremental Export**: Export only changed components for large models

## Troubleshooting

### Common Issues

**Import Error:**
```bash
ModuleNotFoundError: No module named 'pynlm_enhanced'
```

Solution: Ensure the enhanced module is built and in your Python path.

**Initialization Error:**
```python
TrainingError: Brain initialization failed
```

Solution: Check configuration parameters and ensure required dependencies are installed.

**Export Error:**
```python
SerializationError: Cannot open file for writing
```

Solution: Check file permissions and disk space.

### Debugging Tips

1. **Enable Logging**: Use `brain.logStatus()` to debug brain state
2. **Check Metrics**: Monitor `EpisodeResult` for detailed error information
3. **Use Validation**: Validate all inputs before use
4. **Gradual Testing**: Test each component separately before integration

## Future Enhancements

### Planned Features

1. **Advanced Visualization**: Integration with matplotlib, plotly, and web-based dashboards
2. **Distributed Training**: Support for multi-GPU and distributed training
3. **Custom Trainers**: Plugin architecture for custom training algorithms
4. **Model Zoo**: Pre-trained models and transfer learning
5. **Performance Profiling**: Built-in profiling tools
6. **Hyperparameter Optimization**: Integration with Optuna, Hyperopt, etc.
7. **Adaptive Learning**: Dynamic adjustment of learning rates and hyperparameters

### Research Integration

1. **Neuroscientific Models**: Integration with biological neural models
2. **Explainable AI**: Tools for understanding brain behavior
3. **Transfer Learning**: Knowledge transfer between different tasks
4. **Meta-Learning**: Learning to learn with few examples

## License

MIT License. See LICENSE file for details.

## Acknowledgements

This enhanced bindings framework builds upon the existing NLM framework and provides significant improvements for Python users, including:

- Enhanced code organization and maintainability
- Comprehensive error handling and validation
- Advanced training and evaluation utilities
- Visualization and export capabilities
- Memory management and performance optimization

For more information, visit the NLM project documentation at https://nlm.readthedocs.io
