Enhanced NLM Python API - v0.2.0

This module provides an advanced Python interface for the NLM neural simulation framework
with type hints, comprehensive documentation, and enhanced functionality.

## Overview

The NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator
that can learn, remember, and make decisions through neural computation. This enhanced
Python API provides:

- **Type-safe interface** with comprehensive type hints
- **Advanced simulation controls** and monitoring
- **Performance profiling** tools
- **Enhanced configuration validation**
- **Real-time simulation monitoring**
- **Environment building utilities**
- **Comprehensive learning metrics**

## Installation

```bash
pip install pynlm
# or install from source
pip install -e .
```

## Quick Start

### Basic Usage

```python
import pynlm

# Create and configure a brain
config = pynlm.create_default_config()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

brain = pynlm.create_brain(config)
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.get_firing_neuron_count()} neurons firing")
```

### Advanced Simulation

```python
import pynlm

# Configure advanced simulation
from pynlm import SimulationConfig, SimulationMonitor, PerformanceProfiler

config = SimulationConfig()
config.neuron_count = 5000
config.total_steps = 500
config.enable_curiosity = True
config.enable_development = True

brain = pynlm.create_brain(config)
world = pynlm.create_world()
agent = pynlm.create_agent(brain)

# Setup monitoring
monitor = SimulationMonitor(brain, world)
profiler = PerformanceProfiler()

# Run advanced simulation
results = pynlm.run_advanced_simulation(
    brain, world, agent, config, monitor, profiler
)

print(f"Performance: {results['efficiency_grade']}")
print(f"Learning metrics: {results['learning_metrics'].is_learning()}")
```

### Environment Building

```python
from pynlm import EnvironmentBuilder, create_world

# Build a custom environment
env = (EnvironmentBuilder()
       .set_dimensions(15, 15)
       .set_vision(8, 8)
       .add_resource(3, 3, 2.0)
       .add_resource(10, 10, 1.5)
       .add_hazard(12, 12, 0.5)
       .add_wall(7, 7)
       .set_agent_start(1, 1))

world = env.build_world()
```

## Key Classes

### LearningMetrics
Comprehensive metrics for tracking brain learning and performance.

```python
metrics = pynlm.LearningMetrics()
print(f"Firing rate: {metrics.firing_rate}")
print(f"Learning occurred: {metrics.is_learning()}")
print(f"Performance grade: {metrics.get_performance_grade()}")
```

### SimulationConfig
Advanced simulation configuration with validation.

```python
config = pynlm.SimulationConfig()
config.neuron_count = 3000
config.total_steps = 1000
config.enable_curiosity = True
config.enable_development = True

errors = config.validate()
if errors:
    for error in errors:
        print(f"Validation error: {error}")
```

### PerformanceProfiler
Performance profiling and benchmarking.

```python
profiler = pynlm.PerformanceProfiler()
profiler.start_profiling()

for step in range(100):
    brain.step(step)
    profiler.step(brain)

results = profiler.stop_profiling()
print(f"Steps per second: {results['steps_per_second']}")
print(f"Efficiency: {profiler.get_efficiency_rating()}")
```

### SimulationMonitor
Real-time monitoring and visualization.

```python
monitor = pynlm.SimulationMonitor(brain, world)

for step in range(100):
    world.update(0.1)
    monitor.step()

summary = monitor.get_summary()
print(f"Average firing rate: {summary['avg_firing_rate']}")
print(f"Final developmental stage: {summary['final_stage']}")
```

### EnvironmentBuilder
Builder class for creating complex environments.

```python
from pynlm import EnvironmentBuilder

builder = (EnvironmentBuilder()
          .set_dimensions(20, 20)
          .set_vision(10, 10)
          .add_resource(5, 5, 1.0)
          .add_resource(15, 15, 2.0)
          .set_agent_start(2, 2))

world = builder.build_world()
```

## Factory Functions

### Core Creation Functions

```python
# Create default configuration
config = pynlm.create_default_config()

# Create brain with configuration
brain = pynlm.create_brain(config)

# Create agent interface
agent = pynlm.create_agent(brain)

# Create world
world = pynlm.create_world()

# Create sensory inputs
vision = pynlm.create_vision_sensor(8, 8, 3)
audio = pynlm.create_audio_sensor(44100, 1024)
```

### Advanced Simulation Functions

```python
# Run a complete learning experiment
results = pynlm.run_learning_experiment(brain, world, agent, 500)

# Run steady-state test
steady_state = pynlm.run_steady_state_test(brain, 5.0)

# Benchmark performance
benchmarks = pynlm.benchmark_performance(config)
```

## Utility Functions

### Configuration and State Management

```python
# Validate configuration
errors = pynlm.validate_configuration(config)

# Export/import brain state
pynlm.export_brain_state(brain, "my_brain.bin")
brain = pynlm.import_brain_state("my_brain.bin")
```

### Reproducibility

```python
# Create reproducible environment
env_builder = pynlm.create_reproducible_environment(123)
world = env_builder.build_world()
```

## Examples

### Example 1: Learning Environment

```python
import pynlm

def create_learning_environment():
    """Create an environment for learning experiments."""
    # Create brain with learning capabilities
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.create_brain(config)
    world = pynlm.create_world()
    world.configure(15, 15, 6, 6)
    world.set_agent_start(1, 1)
    
    # Add resources for reward
    world.add_object(pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 1.0))
    world.add_object(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 2.0))
    
    agent = pynlm.create_agent(brain)
    
    # Setup monitoring
    monitor = pynlm.SimulationMonitor(brain, world)
    profiler = pynlm.PerformanceProfiler()
    
    return brain, world, agent, monitor, profiler

# Run learning experiment
brain, world, agent, monitor, profiler = create_learning_environment()
results = pynlm.run_learning_experiment(brain, world, agent, 300)

print("Learning experiment completed!")
print(f"Performance grade: {results['efficiency_grade']}")
print(f"Learning achieved: {results['learning_metrics'].is_learning()}")
```

### Example 2: Performance Benchmarking

```python
import pynlm
import matplotlib.pyplot as plt

def benchmark_scalability():
    """Benchmark performance across different network sizes."""
    config = pynlm.create_default_config()
    
    neuron_counts = [500, 1000, 2000, 5000, 10000]
    benchmarks = pynlm.benchmark_performance(config, neuron_counts)
    
    # Plot results
    neurons = [b['neurons'] for b in benchmarks]
    steps_per_sec = [b['steps_per_second'] for b in benchmarks]
    
    plt.figure(figsize=(10, 6))
    plt.loglog(neurons, steps_per_sec, 'bo-', linewidth=2, markersize=8)
    plt.xlabel('Number of Neurons')
    plt.ylabel('Steps per Second')
    plt.title('NLM Performance Scalability')
    plt.grid(True, alpha=0.3)
    plt.show()
    
    return benchmarks

benchmarks = benchmark_scalability()
for b in benchmarks:
    print(f"Neurons: {b['neurons']:6d}, "
          f"Steps/sec: {b['steps_per_second']:8.0f}, "
          f"Firing rate: {b['final_firing_rate']:6.3f}")
```

### Example 3: Development Tracking

```python
import pynlm

def track_development(brain, world, agent, duration=30):
    """Track brain development over time."""
    from pynlm import SimulationConfig, run_advanced_simulation
    
    config = SimulationConfig()
    config.total_steps = duration * 10  # 10 steps per second
    config.enable_development = True
    config.monitor_learning = True
    config.monitor_neuromodulation = True
    
    monitor = pynlm.SimulationMonitor(brain, world)
    profiler = pynlm.PerformanceProfiler()
    
    results = run_advanced_simulation(brain, world, agent, config, monitor, profiler)
    
    # Analyze development
    metrics = results['learning_metrics']
    print(f"Developmental stage: {metrics.developmental_stage}")
    print(f"Developmental age: {metrics.developmental_age:.2f}")
    print(f"Plasticity modifier: {metrics.plasticity_modifier:.3f}")
    
    # Track changes in neuromodulation
    print("\nNeuromodulation levels:")
    print(f"  Dopamine: {metrics.dopamine_level:.3f}")
    print(f"  Curiosity: {metrics.curiosity_level:.3f}")
    print(f"  Novelty: {metrics.novelty_level:.3f}")
    
    return results

# Create and track development
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
world = pynlm.create_world()
agent = pynlm.create_agent(brain)
agent.initialize(world)

results = track_development(brain, world, agent, 10)
```

## API Reference

### Constants

- `INVALID_NEURON_ID`: Invalid neuron identifier
- `INVALID_SYNAPSE_ID`: Invalid synapse identifier  
- `INVALID_REGION_ID`: Invalid region identifier
- `INVALID_POPULATION_ID`: Invalid population identifier

### Type Definitions

- `NeuronType`: Neuron type enumeration
- `ActionType`: Action type enumeration
- `MotorCommand`: Motor command enumeration
- `DevelopmentalStage`: Developmental stage enumeration
- `SynapseType`: Synapse type enumeration
- `WorldObjectType`: World object type enumeration

### Core Classes

- `Config`: Configuration class for NLM system
- `Brain`: Central neural simulation brain class
- `AgentBrain`: Agent brain interface connecting NLM brain to world
- `SimpleWorld`: Simple 2D world for NLM simulation
- `Vision`: Vision sensory input
- `Audio`: Audio sensory input
- `InternalSignals`: Internal signals sensory input
- `Action`: Action representation for motor output
- `WorldObject`: World object representation
- `AgentBody`: Agent body state
- `ActionResult`: Action result from world
- `SensoryPercept`: Sensory percept data

### Advanced Classes

- `LearningMetrics`: Comprehensive metrics for tracking learning and performance
- `SimulationConfig`: Advanced simulation configuration with validation
- `PerformanceProfiler`: Performance profiling and benchmarking
- `SimulationMonitor`: Real-time monitoring and visualization
- `EnvironmentBuilder`: Builder class for creating complex environments

### Factory Functions

- `create_default_config()`: Create a default configuration
- `create_brain(config)`: Create a new brain with configuration
- `create_agent(brain)`: Create an agent brain interface
- `create_world()`: Create a new simple world
- `create_vision_sensor(width, height, channels)`: Create vision sensor
- `create_audio_sensor(sample_rate, num_samples)`: Create audio sensor

### Utility Functions

- `run_advanced_simulation(brain, world, agent, config, monitor, profiler)`: Run advanced simulation
- `run_learning_experiment(brain, world, agent, num_steps)`: Run learning experiment
- `run_steady_state_test(brain, duration, timestep)`: Run steady-state test
- `benchmark_performance(brain_template, neuron_counts, steps)`: Benchmark performance
- `validate_configuration(config)`: Validate configuration
- `export_brain_state(brain, filepath)`: Export brain state
- `import_brain_state(filepath)`: Import brain state
- `create_reproducible_environment(seed)`: Create reproducible environment

## Performance Tips

1. **Use appropriate neuron counts**: For real-time applications, consider 1000-5000 neurons
2. **Enable efficient plasticity**: Only enable STDP and Hebbian when needed
3. **Monitor performance**: Use PerformanceProfiler to identify bottlenecks
4. **Use steady-state testing**: Validate network behavior with steady-state tests
5. **Batch processing**: Process multiple steps at once when appropriate

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that neurons have appropriate connectivity

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neuron connections

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### Performance Issues

**Slow simulation speed**
- Reduce neuron count
- Disable unnecessary plasticity mechanisms
- Reduce simulation timestep
- Enable performance optimizations

**Memory issues**
- Monitor memory usage with PerformanceProfiler
- Reduce neuron and synapse counts
- Use checkpointing for long simulations

### Configuration Issues

**Invalid configuration**
- Use `validate_configuration()` to check for errors
- Check that all values are within valid ranges
- Ensure required parameters are set

## Contributing

### Development Guidelines

1. **Code style**: Follow existing C++/Python coding conventions
2. **Documentation**: Keep documentation up-to-date with code changes
3. **Testing**: Add comprehensive tests for new functionality
4. **Type hints**: Use type hints for all public APIs
5. **Error handling**: Implement robust error handling and validation

### Building from Source

```bash
# Build the C++ library
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .

# Run tests
ctest --output-on-failure
```

### License

This project is licensed under the MIT License. See the LICENSE file for details.

## Support

For questions, issues, or discussions:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Discord: https://discord.gg/nlm

## Acknowledgments

This enhanced Python API builds upon the original NLM framework developed by:
- The NLM Project researchers and contributors
- pybind11 for Python bindings
- The open-source scientific Python ecosystem

## Version History

- **v0.2.0**: Added comprehensive type hints, advanced configuration, monitoring tools
- **v0.1.0**: Original Python bindings release

## Future Improvements

Planned enhancements for future versions:
- Advanced visualization tools
- Machine learning integration
- Distributed simulation support
- Enhanced debugging capabilities
- Integration with scientific computing libraries
