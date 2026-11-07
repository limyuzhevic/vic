# Enhanced NLM Python API - Documentation

## Overview

This document describes the enhanced NLM Python API improvements, including new abstractions, convenience functions, and advanced features for working with the Neural Learning Machine (NLM) neural simulation framework.

## What Was Improved

### 1. Enhanced Python Bindings

The original NLM Python bindings were comprehensive but could benefit from better abstractions and higher-level APIs. The improvements include:

#### **Higher-Level Convenience Functions**
- `create_simple_experiment()` - Complete experiment setup with brain, world, and agent
- `run_episode()` - High-level episode execution with optional callbacks
- `create_advanced_world()` - Advanced world simulation with terrain, resources, and hazards

#### **Enhanced Experiment Management**
- `NLMExperiment` class - Comprehensive experiment manager with monitoring, checkpointing, and analysis
- Built-in performance monitoring and statistics collection
- Callback system for custom event handling

#### **Advanced World Simulation**
- `AdvancedWorld` class - Enhanced world simulation with terrain generation
- Resource and hazard management
- Agent state tracking and metrics

### 2. New Features

#### **1. Simplified Experiment Setup**
```python
from advanced_api import create_simple_experiment

# Create complete experiment setup
experiment = create_simple_experiment(
    brain_size=1000,
    world_width=30,
    world_height=30,
    config_dict={
        'brain.connection_probability': 0.15,
        'random_seed': 42
    }
)
```

#### **2. High-Level Experiment Management**
```python
from advanced_api import NLMExperiment

# Create experiment manager with monitoring
exp_manager = NLMExperiment({
    'brain.neuron_count': 800,
    'brain.connection_probability': 0.12
})

# Add callbacks for monitoring
exp_manager.add_step_callback(lambda state: print(f"Step {state['step_count']}"))
exp_manager.add_episode_callback(lambda state: print("Episode complete!"))

# Run experiment
exp_manager.start_experiment()
result = exp_manager.run_episode(max_steps=1000, checkpoint_interval=100)

# Save/load checkpoints
exp_manager.save_checkpoint("my_experiment")
```

#### **3. Advanced World Simulation**
```python
from advanced_api import create_advanced_world

# Create world with terrain, resources, and hazards
world = create_advanced_world(
    width=60,
    height=40,
    enable_terrain=True,
    enable_resources=True,
    enable_hazards=True
)

# Run simulation
for step in range(100):
    percept = world.update(0.1)
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.apply_action(action, current_time)
```

#### **4. Batch Processing**
```python
from advanced_api import batch_run_experiments

# Run multiple experiments with different configurations
configs = [
    {'brain.neuron_count': 500, 'connection_probability': 0.1},
    {'brain.neuron_count': 1000, 'connection_probability': 0.15},
    {'brain.neuron_count': 1500, 'connection_probability': 0.12},
]

results = batch_run_experiments(configs, episodes_per_config=10)
```

#### **5. Performance Benchmarking**
```python
from advanced_api import benchmark_performance

# Benchmark performance across configurations
configs = [
    {'brain.neuron_count': 500, 'connection_probability': 0.1},
    {'brain.neuron_count': 800, 'connection_probability': 0.12},
]

benchmark_results = benchmark_performance(configs, num_runs=5)
```

### 3. Key Improvements

#### **Before vs After**

| Feature | Original API | Enhanced API |
|---------|-------------|-------------|
| Experiment Setup | Manual creation of brain/world/agent | One function call: `create_simple_experiment()` |
| Monitoring | Manual logging | Built-in monitoring with callbacks |
| Checkpointing | Manual file I/O | `save_checkpoint()` / `load_checkpoint()` |
| Performance Analysis | Manual calculation | `analyze_performance()` method |
| World Simulation | Basic world only | Advanced world with terrain/resources/hazards |
| Batch Processing | External loops | `batch_run_experiments()` function |
| Benchmarking | Manual timing | `benchmark_performance()` function |

#### **Code Quality Improvements**

1. **Better Abstraction Layers**: High-level APIs that hide complex C++ details
2. **Enhanced Error Handling**: Comprehensive exception handling and validation
3. **Performance Monitoring**: Built-in timing and metrics collection
4. **State Management**: Better experiment state tracking and persistence
5. **Callback System**: Flexible event-driven programming
6. **Type Safety**: Improved type hints and validation
7. **Documentation**: Comprehensive docstrings and examples

### 4. Performance Features

#### **1. Built-in Performance Monitoring**
```python
# Performance metrics collected automatically
analysis = exp_manager.analyze_performance()

metrics = {
    'total_steps': analysis['total_steps'],
    'avg_reward_per_step': analysis['avg_reward_per_step'],
    'firing_rate': analysis['firing_rate_stats']['mean'],
    'efficiency': analysis['efficiency']['spikes_per_step'],
    'step_times': {
        'mean': analysis['avg_step_time'],
        'min': analysis['min_step_time'],
        'max': analysis['max_step_time']
    }
}
```

#### **2. Smart Checkpointing**
```python
# Automatic checkpointing at intervals
exp_manager.run_episode(max_steps=1000, checkpoint_interval=100)

# Manual checkpointing
exp_manager.save_checkpoint("experiment_name", "Description")

# Resume from checkpoint
exp_manager.load_checkpoint("experiment_name")
```

#### **3. Advanced Metrics Collection**
- Step-by-step timing information
- Reward tracking and statistics
- Firing rate analysis
- Spike efficiency metrics
- Curiosity and novelty levels

### 5. Usage Examples

#### **Example 1: Quick Start**
```python
import sys
sys.path.insert(0, 'path/to/enhanced_api')
from advanced_api import create_simple_experiment, run_episode

# Create and run experiment in one line
experiment = create_simple_experiment(world_width=20, world_height=20)
result = run_episode(experiment, max_steps=100)

print(f"Final reward: {result['performance_metrics']['total_reward']:.2f}")
print(f"Firing rate: {result['performance_metrics']['final_firing_rate']:.2f} Hz")
```

#### **Example 2: Advanced Experiment with Monitoring**
```python
from advanced_api import NLMExperiment

exp = NLMExperiment({
    'brain.neuron_count': 1500,
    'brain.connection_probability': 0.12
})

def custom_callback(state):
    # Custom logic for each step
    if state['step_count'] % 50 == 0:
        print(f"Progress: {state['step_count']} steps, "
              f"Curiosity: {state['agent'].getCuriosityLevel():.3f}")

exp.add_step_callback(custom_callback)
exp.start_experiment()
result = exp.run_episode(max_steps=500)

# Export results
exp.export_results("my_experiment.json")
```

#### **Example 3: Benchmarking**
```python
from advanced_api import benchmark_performance

configs = [
    {'brain.neuron_count': 500, 'connection_probability': 0.1},
    {'brain.neuron_count': 1000, 'connection_probability': 0.15},
    {'brain.neuron_count': 1500, 'connection_probability': 0.12},
]

results = benchmark_performance(configs, num_runs=3)

# Analyze results
for result in results['results']:
    print(f"Config with {result['config']['brain.neuron_count']} neurons:")
    print(f"  Mean execution time: {result['mean_time']:.3f}s")
    print(f"  Reward efficiency: {result['mean_reward_efficiency']:.3f}")
```

### 6. Integration with Existing Code

The enhanced API is fully backward compatible with the original NLM Python bindings:

```python
import pynlm

# Original API still works
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced API available separately
from advanced_api import create_simple_experiment, NLMExperiment
```

### 7. Installation and Usage

#### **Prerequisites**
```bash
pip install pynlm numpy
# Ensure NLM C++ library is built and available
```

#### **Basic Usage**
```bash
# Run the demo script
python demo_enhanced_api.py

# Import the enhanced API
from advanced_api import create_simple_experiment
```

#### **Building the Enhanced API**
The enhanced Python API is implemented as a separate module that:
1. Imports the original `pynlm` bindings
2. Extends them with higher-level abstractions
3. Provides additional convenience functions and classes

### 8. Best Practices

#### **1. Use the Enhanced API for Complex Experiments**
- Use `NLMExperiment` for experiments requiring monitoring
- Use `create_simple_experiment()` for quick setups
- Use `AdvancedWorld` for world simulations with complex environments

#### **2. Implement Callbacks for Custom Logic**
```python
exp_manager.add_step_callback(my_custom_logic)
exp_manager.add_episode_callback(my_post_episode_logic)
```

#### **3. Use Checkpointing for Long-running Experiments**
```python
# Checkpoint every 100 steps for long experiments
exp_manager.run_episode(max_steps=10000, checkpoint_interval=100)

# Resume from checkpoint if needed
exp_manager.load_checkpoint("experiment_checkpoint")
```

#### **4. Monitor Performance**
```python
# Get performance analysis after experiment
analysis = exp_manager.analyze_performance()

# Monitor key metrics
metrics = {
    'execution_speed': analysis['avg_step_time'],
    'reward_efficiency': analysis['efficiency']['reward_per_second'],
    'neural_activity': analysis['firing_rate_stats']['mean']
}
```

#### **5. Combine Original and Enhanced APIs**
```python
import pynlm as nlm
from advanced_api import create_simple_experiment

# Use original API for custom configurations
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

# Use enhanced API for convenience
experiment = create_simple_experiment(config_dict={
    'brain.neuron_count': 2000,
    'world.width': 50
})
```

### 9. Migration Guide

#### **From Original to Enhanced API**

| Original Approach | Enhanced API |
|-------------------|-------------|
| Manual brain/world creation | `create_simple_experiment()` |
| Manual logging | `NLMExperiment` with callbacks |
| Manual checkpointing | `save_checkpoint()` / `load_checkpoint()` |
| Manual performance tracking | Built-in `analyze_performance()` |
| Basic world simulation | `AdvancedWorld` with terrain/resources/hazards |
| External batch loops | `batch_run_experiments()` |
| Manual benchmarking | `benchmark_performance()` |

#### **Migration Steps**

1. **Replace experiment creation**:
   ```python
   # Old
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   world = pynlm.createSimpleWorld()
   world.configure(20, 20, 10, 10)
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   
   # New
   experiment = create_simple_experiment(
       brain_size=1000,
       world_width=20,
       world_height=20
   )
   ```

2. **Add monitoring**:
   ```python
   # Old - manual logging
   for step in range(1000):
       if step % 100 == 0:
           print(f"Step {step}: reward={reward}")
   
   # New - built-in monitoring
   exp_manager.add_step_callback(lambda state: print(f"Step {state['step_count']}"))
   ```

3. **Add checkpointing**:
   ```python
   # Old - manual file I/O
   if step % 100 == 0:
       brain.save(f"checkpoint_step_{step}.bin")
   
   # New - automatic checkpointing
   result = exp_manager.run_episode(max_steps=1000, checkpoint_interval=100)
   ```

### 10. Technical Specifications

#### **API Version**: 2.0 (Enhanced)
#### **Dependencies**: pynlm, numpy, json, time, os
#### **Memory Usage**: Minimal overhead, delegates to original bindings
#### **Thread Safety**: Safe for concurrent experiment execution
#### **Performance**: No runtime overhead for direct binding usage

#### **File Structure**:
```
enhanced_api.py              # Core enhanced API implementation
demo_enhanced_api.py         # Demo script showing all features
advanced_api.py              # Extended version with advanced features
```

### 11. Future Enhancements

#### **Planned Improvements**:
1. **Distributed Computing**: Support for multi-GPU and distributed experiments
2. **Visualization API**: Integrated plotting and visualization tools
3. **Hyperparameter Optimization**: Built-in optimization for experiment parameters
4. **Docker Support**: Containerized experiments for easy deployment
5. **API Documentation**: Auto-generated API documentation
6. **Notebook Support**: Jupyter notebook integration
7. **Plugin System**: Extensible plugin architecture for custom components

#### **Research Areas**:
- Neural architecture search
- Automated experiment design
- Real-time visualization
- Cloud-based experiment execution

## Conclusion

The enhanced NLM Python API provides significant improvements over the original bindings:

1. **Simplified Usage**: Higher-level abstractions reduce boilerplate code
2. **Better Monitoring**: Built-in performance tracking and analysis
3. **Enhanced Functionality**: Advanced world simulation and benchmarking
4. **Improved Developer Experience**: Callbacks, checkpointing, and error handling
5. **Backward Compatibility**: Original API remains unchanged
6. **Production Ready**: Suitable for research, development, and deployment

These improvements make it easier to:
- Set up complex experiments quickly
- Monitor and analyze experiment performance
- Reproduce and share experiments
- Conduct systematic parameter studies
- Deploy experiments in production environments

The enhanced API maintains full compatibility with the original NLM framework while providing a much more user-friendly and feature-rich interface for working with neural simulation experiments.