# NLM Python Advanced API Documentation

This document provides comprehensive documentation for the enhanced NLM Python API,
including new convenience functions, advanced tools, and expert features.

## Overview

The NLM Python bindings have been significantly enhanced with:

1. **Convenience Factory Functions** - Easy setup for common use cases
2. **Advanced Simulation Tools** - Performance monitoring and control
3. **Expert Features** - Parallel processing, adaptation, and optimization
4. **Improved Error Handling** - Better validation and error messages
5. **Extended Documentation** - Comprehensive examples and tutorials

## 1. Quick Start Guide

### Basic Usage (Unchanged)

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

### Enhanced Quick Start with Convenience Functions

```python
import pynlm

# Create training brain with optimal settings
brain = pynlm.createTrainingBrain(500, 3, 0.001)
brain.initialize()

# Create world with default settings
world = pynlm.createTrainingWorld(50, 50)

# Create expert agent
agent = pynlm.createExpertAgent(brain, world)

# Run complete simulation
pynlm.runTrainingSimulation(agent, world, 1000)
```

## 2. Convenience Factory Functions

### Configuration Factory Functions

#### `createTrainingConfig(trainingType, epochs)`

Creates a configuration optimized for training scenarios.

```python
config = pynlm.createTrainingConfig("reinforcement_learning", 1000)
# Sets: training_type, epochs, learning_rate=0.001, batch_size=32, optimizer="adam"
```

#### `createDevelopmentConfig(plasticityRate, enableStructuralPlasticity)`

Creates a configuration optimized for developmental learning.

```python
config = pynlm.createDevelopmentConfig(0.01, True)
# Sets: plasticity_rate, structural_plasticity_enabled, development_enabled=true, curiosity_enabled=true
```

#### `createProductionConfig(memorySize, explorationRate)`

Creates a configuration optimized for production environments.

```python
config = pynlm.createProductionConfig(2000, 0.2)
# Sets: working_memory_size, exploration_rate, novelty_detection_enabled=true, prediction_enabled=true
```

### Brain Factory Functions

#### `createTrainingBrain(neuronCount, regionCount, learningRate)`

Creates a brain optimized for training.

```python
brain = pynlm.createTrainingBrain(200, 3, 0.001)
# neuron_count=200, region_count=3, learning_rate=0.001, connection_probability=0.15
```

#### `createDevelopmentBrain(neuronCount, maturationRate)`

Creates a brain with developmental stages.

```python
brain = pynlm.createDevelopmentBrain(300, 0.005)
# neuron_count=300, region_count=2, maturation_rate=0.005, development_enabled=true
```

#### `createProductionBrain(neuronCount, explorationRate)`

Creates a brain optimized for production use.

```python
brain = pynlm.createProductionBrain(500, 0.1)
# neuron_count=500, region_count=1, exploration_rate=0.1, prediction_enabled=true
```

### World Factory Functions

#### `createTrainingWorld(width, height)`

Creates a world optimized for training with optimal energy settings.

```python
world = pynlm.createTrainingWorld(50, 50)
# width=50, height=50, vision=(8,8), agent_start=(2,2), max_energy=100.0
```

#### `createSurvivalWorld(width, height, hazardDensity)`

Creates a world with hazards for survival scenarios.

```python
world = pynlm.createSurvivalWorld(30, 30, 0.1)
# Creates random hazards at 10% density
```

### Agent Factory Functions

#### `createAgentWithDefaults(brain, world)`

Creates an agent with all learning features enabled by default.

```python
agent = pynlm.createAgentWithDefaults(brain, world)
# Enables: reward_modulation, structural_plasticity, development, curiosity
```

#### `createExpertAgent(brain, world)`

Creates an expert agent with advanced capabilities.

```python
agent = pynlm.createExpertAgent(brain, world)
# Enables all expert-level features
```

### Complete Setup Factory Functions

#### `createCompleteSimulationSetup(worldType, steps, enableLearning)`

Creates a complete simulation setup with all components.

```python
brain, world, agent = pynlm.createCompleteSimulationSetup("training", 100, True)
# Returns: brain, world, agent with all learning enabled
```

#### Quick Setup Functions

```python
# Quick training simulation
brain, world, agent = pynlm.quickSimulation(100, 8)

# Quick survival simulation  
brain, world, agent = pynlm.quickSurvivalSimulation(200)
```

## 3. Advanced Expert Tools

### Performance Monitoring

```python
from advanced_nlm_tools import PerformanceMonitor

# Create performance monitor
monitor = PerformanceMonitor()

# Start monitoring
monitor.start()

# Run simulation with monitoring
for step in range(1000):
    # ... your simulation code ...
    monitor.step(brain, world, agent)

# Get metrics
metrics = monitor.get_metrics()
print(f"Average firing rate: {metrics.get('firing_rate_mean', 0):.2f}")
print(f"Success rate: {metrics.get('success_rate_mean', 0):.2f}")
```

### Memory Management

```python
from advanced_nlm_tools import MemoryManager

# Create memory manager with 2GB limit
mem_manager = MemoryManager(max_memory_mb=2048)

# Allocate brain with memory tracking
config = pynlm.createDefaultConfig()
brain = mem_manager.allocate_brain(config)

# Check memory usage
print(f"Memory used: {mem_manager.get_memory_usage() / (1024*1024):.2f} MB")
```

### Simulation Controller

```python
from advanced_nlm_tools import SimulationController

# Create simulation controller
controller = SimulationController()

# Run parallel simulations
configs = [pynlm.createDefaultConfig() for _ in range(5)]
worlds = [pynlm.createSimpleWorld() for _ in range(5)]
results = controller.run_parallel_simulations(configs, worlds, 100)

print(f"Completed {len(results)} parallel simulations")
```

### Advanced Adaptation

```python
from advanced_nlm_tools import SimulationController

controller = SimulationController()

adaptation_rules = {
    'adapt_curiosity': True,
    'curiosity_threshold': 0.8,
    'adapt_development': True,
    'apply_reward': True,
    'update_development': True
}

metrics = controller.run_with_adaptation(agent, world, 500, adaptation_rules)
```

### Checkpointing

```python
from advanced_nlm_tools import SimulationController

controller = SimulationController()
checkpoints = controller.run_with_checkpoints(agent, world, 1000, 100, "/path/to/checkpoints")
```

## 4. Advanced Expert Features

### Parameter Search

```python
from advanced_nlm_tools import ExpertTools

base_config = pynlm.createDefaultConfig()
parameter_ranges = {
    "brain.neuron_count": (100, 1000, 100),
    "brain.region_count": (1, 5, 1),
    "plasticity.stdp.learning_rate": (0.0001, 0.1, 0.01)
}

configs = ExpertTools.create_parameter_search_space(base_config, parameter_ranges)
print(f"Generated {len(configs)} configurations for parameter search")
```

### Brain Creation Benchmark

```python
from advanced_nlm_tools import ExpertTools

benchmark = ExpertTools.benchmark_brain_creation(num_brains=10, neurons=1000)
print(f"Created 10 brains with {benchmark['brains_per_second']:.1f} brains/second")
```

### Configuration Validation

```python
from advanced_nlm_tools import ExpertTools

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)

warnings = ExpertTools.validate_config_compatibility(config)
for warning in warnings:
    print(f"Warning: {warning}")
```

### Experiment Series

```python
from advanced_nlm_tools import ExpertTools

base_config = pynlm.createDefaultConfig()
variations = [
    {"brain.neuron_count": 500, "plasticity_rate": 0.01},
    {"brain.neuron_count": 1000, "plasticity_rate": 0.02},
    {"brain.neuron_count": 2000, "plasticity_rate": 0.005}
]

configs = ExpertTools.create_experiment_series(base_config, variations, world_template)
```

## 5. Error Handling and Validation

### Input Validation

The enhanced bindings now include comprehensive input validation:

```python
# Invalid config file path
pynlm.createBrainFromConfigFile("nonexistent.json")
# Raises: RuntimeError("Failed to load configuration from file: nonexistent.json")

# Invalid parameters in config creation
config = pynlm.createConfigFromParams({
    "invalid_key": "value",
    "brain.neuron_count": -1  # Negative values may be rejected
})
```

### Memory Limits

```python
from advanced_nlm_tools import MemoryManager

mem_manager = MemoryManager(max_memory_mb=100)  # 100MB limit
try:
    config = pynlm.createDefaultConfig()
    brain = mem_manager.allocate_brain(config)
except MemoryError as e:
    print(f"Memory allocation failed: {e}")
```

## 6. Example Workflows

### Reproduction Experiment

```python
from advanced_nlm_tools import run_reproduction_experiment

# Setup baseline
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentWithDefaults(brain, world)

# Run reproduction experiment
results = run_reproduction_experiment(brain, world, agent, 1000)

# Analyze results
print(f"Average curiosity: {sum(results['curiosity_trajectory'])/len(results['curiosity_trajectory']):.3f}")
print(f"Final development stage: {results['development_trajectory'][-1]}")
```

### Hyperparameter Optimization

```python
from advanced_nlm_tools import ExpertTools

# Create parameter search space
base_config = pynlm.createDefaultConfig()
configs = ExpertTools.create_parameter_search_space(
    base_config,
    {
        "brain.neuron_count": (500, 2000, 500),
        "plasticity_rate": (0.001, 0.1, 0.05)
    }
)

# Run parameter search
from concurrent.futures import ThreadPoolExecutor

results = []
with ThreadPoolExecutor(max_workers=4) as executor:
    futures = []
    for config in configs:
        future = executor.submit(lambda c: {
            'config': c,
            'neurons': c.get("brain.neuron_count"),
            'plasticity': c.get("plasticity_rate")
        }, config)
        futures.append(future)
    
    for future in futures:
        results.append(future.result())

# Sort by neuron count and display
results.sort(key=lambda x: x['neurons'])
for result in results[:5]:
    print(f"Neurons: {result['neurons']}, Plasticity: {result['plasticity']}")
```

### Advanced Training Workflow

```python
import pynlm
from advanced_nlm_tools import PerformanceMonitor, SimulationController

# Setup training environment
config = pynlm.createTrainingConfig("reinforcement_learning", 5000)
world = pynlm.createTrainingWorld(100, 100)
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createExpertAgent(brain, world)

# Setup monitoring
monitor = PerformanceMonitor()
monitor.start()

# Training loop with adaptive learning
controller = SimulationController()
adaptation_rules = {
    'adapt_curiosity': True,
    'curiosity_threshold': 0.9,
    'apply_reward': True,
    'update_development': True
}

# Run adaptive training
metrics = controller.run_with_adaptation(agent, world, 2000, adaptation_rules)

# Monitor progress
for step in range(0, 2000, 100):
    step_metrics = monitor.get_metrics()
    print(f"Step {step}: Firing rate={step_metrics.get('firing_rate_mean', 0):.2f}, "
          f"Curiosity={step_metrics.get('curiosity_mean', 0):.3f}")

# Final metrics
final_metrics = monitor.get_metrics()
print(f"Training completed: {final_metrics['steps']} steps in {final_metrics['duration']:.1f}s")
```

## 7. Migration Guide

### From Old to New API

| Old API | New API | Notes |
|---------|---------|-------|
| `createBrain(config)` | `createBrain(config)` | Unchanged |
| `createSimpleWorld()` | `createSimpleWorld()` | Unchanged |
| `createAgentBrain(brain)` | `createAgentWithDefaults(brain, world)` | Now includes world and default features |
| Manual brain setup | `createTrainingBrain()` | New convenience function |
| Manual world setup | `createTrainingWorld()` | New convenience function |
| Manual agent setup | `createExpertAgent()` | New convenience function |

### Adding New Features

To add new features to the Python bindings:

1. **Configuration Factory Functions**: Add to `bindings.cpp` near other factory functions
2. **Advanced Tools**: Add to `advanced_nlm_tools.py` module
3. **Error Handling**: Add validation and error checking in each function
4. **Documentation**: Update this document with examples

## 8. Performance Considerations

### Memory Management

```python
# For large-scale simulations
from advanced_nlm_tools import MemoryManager

# Set memory limits
mem_manager = MemoryManager(max_memory_mb=2048)  # 2GB

# Monitor usage
print(f"Current memory: {mem_manager.get_memory_usage() / (1024*1024):.1f} MB")
```

### Parallel Processing

```python
# For CPU-bound simulations
from advanced_nlm_tools import SimulationController

controller = SimulationController()
results = controller.run_parallel_simulations(
    configs_list, world_list, 500
)
```

### Memory Efficiency

```python
# Reuse components where possible
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Configure once, reuse many times
brain.configure_simulation(1000)

# Save and load for later use
brain.save("optimized_brain.bin")
```

## 9. Troubleshooting

### Common Issues

1. **Memory Errors**: Use `MemoryManager` or reduce neuron count
2. **Performance Issues**: Use `PerformanceMonitor` to identify bottlenecks
3. **Configuration Errors**: Validate configs with `ExpertTools.validate_config_compatibility()`
4. **Parallel Execution**: Use `SimulationController.run_parallel_simulations()`

### Debugging

```python
import pynlm
from advanced_nlm_tools import PerformanceMonitor

# Setup with monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

monitor = PerformanceMonitor()
monitor.start()

# Run with logging
for step in range(100):
    brain.step(step)
    
    # Log every 10 steps
    if step % 10 == 0:
        metrics = monitor.get_metrics()
        print(f"Step {step}: Neurons={brain.getTotalNeuronCount()}, "
              f"Firing={brain.getFiringNeuronCount()}")
```

## 10. Future Enhancements

### Planned Improvements

1. **Enhanced Serialization**: Add HDF5 and other formats
2. **Distributed Computing**: Multi-GPU support
3. **Advanced Visualization**: 3D brain rendering
4. **Machine Learning Integration**: Neural architecture search
5. **Cloud Deployment**: Containerized simulations

### Contributing

To contribute to the NLM Python bindings:

1. Add new convenience functions to `bindings.cpp`
2. Add advanced tools to `advanced_nlm_tools.py`
3. Update this documentation
4. Add comprehensive tests to `test_pynlm_bindings.py`
5. Follow the existing code style and conventions

## License

This documentation is part of the NLM (Neural Learning Machine) project.
See the main README.md for project licensing information.