# Improved NLM Python Interface

This document provides comprehensive information about the improved NLM (Neural Learning Machine) Python interface, including new features, improvements, and usage examples.

## Overview

The NLM Python interface has been significantly improved to provide:
1. **Better API design** with Pythonic features
2. **Enhanced usability** through context managers and property access
3. **Advanced debugging and monitoring** capabilities
4. **Improved memory management** and performance optimization
5. **Comprehensive documentation and examples**

## Key Improvements

### 1. Modular Architecture
- Split bindings into separate, maintainable modules
- Clear separation of concerns
- Easier to extend and modify

### 2. Pythonic API Design
- Context managers for resource management
- Property-style access for cleaner code
- Iterators for easy collection traversal
- Better error handling with Python exceptions

### 3. Enhanced Features
- Advanced simulation control
- Memory management utilities
- Performance monitoring
- Debugging tools
- Experiment management

### 4. Improved Documentation
- Comprehensive API reference
- Detailed examples
- Migration guides
- Feature comparisons

## Quick Start Guide

### Installation
```bash
# Install the improved NLM Python interface
pip install -e .

# Or install from source
pip install scikit-build-core pybind11 pytest numpy
pip install .
```

### Basic Usage
```python
import pynlm

# Create a default brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.total_neurons} neurons")
```

### Advanced Usage with Context Manager
```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with context manager
with pynlm.simulation_session(brain, world, agent) as session:
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

print(f"Episode completed with {brain.getTotalSpikeCount()} spikes")
```

## Core API Reference

### Main Classes

#### `createDefaultConfig()`
Creates a default configuration for the brain.

```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.15)
brain = pynlm.createBrain(config)
```

#### `createBrain(config)`
Creates a brain with the given configuration.

```python
brain = pynlm.createBrain(config)
brain.initialize()
```

#### `createSimpleWorld()`
Creates a simple 2D world for simulation.

```python
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
```

#### `createAgentBrain(brain)`
Creates an agent brain interface.

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

### Enhanced Brain Class

The `Brain` class now provides property-style access:

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Property access
print(f"Neurons: {brain.total_neurons}")
print(f"Synapses: {brain.total_synapses}")
print(f"Firing rate: {brain.average_firing_rate:.2f}")
print(f"Development stage: {brain.developmental_stage}")

# Property modification
brain.timestep = 0.01
brain.set_neuromodulation_level(0.5)
```

### Enhanced Agent Class

The `Agent` class provides advanced features:

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)
agent.enable_structural_plasticity(True)

# Get advanced metrics
print(f"Neuromodulation level: {agent.get_neuromodulation_level():.2f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"Novelty level: {agent.getNoveltyLevel():.2f}")
print(f"Prediction error: {agent.getPredictionError():.2f}")
```

### Context Managers

#### `simulation_session(brain, world, agent)`
Context manager for simulation sessions.

```python
with pynlm.simulation_session(brain, world, agent) as session:
    for step in range(500):
        # Run simulation
        pass

# Session automatically cleans up
```

#### `agent_session(brain, world)`
Context manager for agent sessions.

```python
with pynlm.agent_session(brain, world) as session:
    for step in range(200):
        # Run agent episode
        pass

# Session automatically cleans up
```

### Memory Management

#### `MemoryManager(brain)`
Helper for memory management operations.

```python
memory_manager = pynlm.MemoryManager(brain)

# Optimize working memory
memory_manager.optimize_working_memory(1000)

# Consolidate memories
memory_manager.consolidate_memories(0.3)

# Get memory statistics
stats = memory_manager.get_memory_stats()
print(f"Working memory traces: {stats['working_memory']['active_traces']}")
```

### Performance Monitoring

#### `performance_monitor(brain, world, agent)`
Performance monitoring with real-time metrics.

```python
perf_monitor = pynlm.performance_monitor(brain, world, agent)

# Run simulation with monitoring
results = perf_monitor.run_with_monitoring(1000, 0.01)

print(f"Performance: {results['steps_per_second']:.1f} steps/s")
print(f"Memory efficiency: {results['memory_efficiency']:.2f}")
```

### Debugging Tools

#### `debug_session(brain, world, agent)`
Advanced debugging session.

```python
debug_session = pynlm.debug_session(brain, world, agent)
debug_report = debug_session.debug_simulation(100, 0.01)

print(f"Debug report: {debug_report}")
```

## Advanced Features

### 1. Dynamic Simulation Control
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Dynamic parameter adjustment
brain.set_curiosity_level(0.3)
brain.set_plasticity_modifier(0.8)
brain.set_development_rate(1.0)
```

### 2. Environment Management
```python
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.set_max_energy(100.0)
world.set_energy_decay_rate(0.01)

# Add objects
from pynlm import WorldObject, WorldObjectType
resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0, radius=0.5)
world.addObject(resource)
```

### 3. Experiment Management
```python
from advanced_features import ExperimentManager, StatisticalAnalyzer

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

experiment = ExperimentManager(brain, world, agent)

# Run parameter sweep
base_params = {"neuron_count": 1000, "connection_probability": 0.15}
param_ranges = {"neuron_count": [500, 1000, 2000], "curiosity": [0.1, 0.5, 0.9]}
results = experiment.run_parameter_sweep(base_params, param_ranges, 20)

# Analyze results
analysis = StatisticalAnalyzer().analyze_results(results)
```

### 4. Advanced Memory Control
```python
memory_manager = pynlm.MemoryManager(brain)

# Automatic memory optimization
memory_manager.optimize_memory_automatically()

# Profile memory usage
memory_profile = memory_manager.profile_memory_usage(60)
print(f"Memory profile: {memory_profile}")
```

## Migration Guide

### From Old API to New API

#### Old API (Limited)
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    
neuron_count = brain.getTotalNeuronCount()
```

#### New API (Improved)
```python
import pynlm

# Create brain with context manager
with pynlm.simulation_session() as brain:
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
    
    print(f"Neurons: {brain.total_neurons}")
```

### Key Differences

1. **Context Managers**: New API uses context managers for automatic resource cleanup
2. **Property Access**: New API provides property-style access for cleaner code
3. **Enhanced Features**: New API includes advanced debugging, monitoring, and memory management
4. **Better Error Handling**: New API provides more informative error messages
5. **Extensible**: New API is designed for easy extension

## Examples

### 1. Complete Simulation with Learning
```python
import pynlm
import time

def run_learning_simulation():
    # Setup
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 1500)
    config.set("brain.connection_probability", 0.18)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    # Run episode
    start_time = time.time()
    
    for step in range(800):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    end_time = time.time()
    
    print(f"Learning simulation completed in {end_time - start_time:.2f}s")
    print(f"Final stats: {brain.getTotalSpikeCount()} spikes, "
          f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return brain

# Run simulation
brain = run_learning_simulation()
```

### 2. Parameter Optimization
```python
import pynlm
from advanced_features import ExperimentManager

def optimize_parameters():
    base_config = pynlm.create_default_config()
    base_config.set("brain.neuron_count", 1000)
    
    # Parameter ranges to test
    param_ranges = {
        "neuron_count": [500, 1000, 1500, 2000],
        "connection_probability": [0.1, 0.15, 0.2],
        "curiosity": [0.1, 0.3, 0.5, 0.7, 0.9]
    }
    
    results = []
    
    # Run parameter sweep
    for neuron_count in param_ranges["neuron_count"]:
        for connection_prob in param_ranges["connection_probability"]:
            for curiosity in param_ranges["curiosity"]:
                print(f"Testing: neurons={neuron_count}, "
                      f"conn={connection_prob}, curiosity={curiosity}")
                
                # Create brain with these parameters
                config = pynlm.create_default_config()
                config.set("brain.neuron_count", neuron_count)
                config.set("brain.connection_probability", connection_prob)
                
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                world = pynlm.createSimpleWorld()
                world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
                world.reset()
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                agent.enable_curiosity(True)
                agent.set_curiosity_level(curiosity)
                
                # Run simulation
                for step in range(200):
                    world.update(0.1)
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                
                # Collect results
                result = {
                    "neuron_count": neuron_count,
                    "connection_probability": connection_prob,
                    "curiosity": curiosity,
                    "total_spikes": brain.getTotalSpikeCount(),
                    "average_firing_rate": brain.getAverageFiringRate(),
                    "final_curiosity": brain.getCuriosityLevel(),
                    "simulation_stable": abs(brain.getCuriosityLevel() - 0.5) < 0.2
                }
                
                results.append(result)
                print(f"  Result: {result}")
    
    return results

# Run optimization
results = optimize_parameters()
```

### 3. Debugging with Advanced Features
```python
import pynlm

def debug_simulation():
    # Create brain with debugging enabled
    brain = pynlm.createBrain(pynlm.create_default_config())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable debugging
    brain.enable_debug_mode(True)
    brain.enable_state_inspection(True)
    
    print("Starting debugging session...")
    print("Commands: n (next step), s (show state), r (record), d (debug), q (quit)")
    
    for step in range(100):
        print(f"\nStep {step}:")
        
        command = input("Command (n/s/r/d/q): ").strip().lower()
        
        if command == 'q':
            break
        elif command == 'n':
            # Execute step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            brain.step(step)
            
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
        elif command == 's':
            # Show state
            brain.log_status()
        elif command == 'r':
            # Record state
            brain.record_state(f"manual_step_{step}")
            print(f"  State recorded")
        elif command == 'd':
            # Debug current state
            brain.debug_current_state()
            print(f"  Debug info displayed")
        else:
            print(f"Unknown command: {command}")
    
    print("\nDebug session completed")
    return brain

# Run debugging
brain = debug_simulation()
```

## Performance and Memory Considerations

### Memory Usage
```python
import pynlm

brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()

# Monitor memory usage
memory_manager = pynlm.MemoryManager(brain)

# Optimize memory based on usage
memory_manager.optimize_memory_automatically()

# Get memory statistics
stats = memory_manager.get_memory_stats()
print(f"Memory usage: {stats}")
```

### Performance Optimization
```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()

# Configure for performance
brain.set_simulation_timestep(0.005)
brain.set_neuromodulation_level(0.5)

# Monitor performance
perf_monitor = pynlm.performance_monitor(brain, None, None)

# Run simulation with monitoring
start_time = time.time()

for step in range(1000):
    brain.step(step)

end_time = time.time()

# Get performance report
performance_report = perf_monitor.get_performance_report()
print(f"Performance: {performance_report['steps_per_second']:.1f} steps/s")
print(f"Memory efficiency: {performance_report['memory_efficiency']:.2f}")
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain not initialized" error
**Solution**: Always call `brain.initialize()` before using the brain.

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # Required!
```

#### 2. "Invalid parameters" error
**Solution**: Check parameter ranges and types.

```python
# Check valid ranges
config = pynlm.createDefaultConfig()
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    if neurons <= 0:
        print("Invalid neuron count")
```

#### 3. Memory issues
**Solution**: Use memory management features.

```python
memory_manager = pynlm.MemoryManager(brain)
memory_manager.optimize_working_memory(500)
memory_manager.consolidate_memories(0.3)
```

#### 4. Performance issues
**Solution**: Use performance monitoring and optimization.

```python
perf_monitor = pynlm.performance_monitor(brain, world, agent)
results = perf_monitor.run_with_monitoring(1000, 0.01)
```

### Debugging Tips

1. **Enable detailed logging**: `brain.enable_debug_mode(True)`
2. **Use state inspection**: `brain.enable_state_inspection(True)`
3. **Monitor performance**: Use `performance_monitor`
4. **Manage memory**: Use `memory_manager`
5. **Use breakpoints**: Add breakpoints in debug sessions

## Future Enhancements

### Planned Features
1. **Advanced visualization**: Real-time visualization of brain activity
2. **Machine learning integration**: Integration with popular ML libraries
3. **Cloud deployment**: Support for cloud-based simulations
4. **Distributed computing**: Support for parallel simulations
5. **Advanced analytics**: More sophisticated analysis tools

### Extension Points
1. **Custom plugins**: Ability to add custom plugins
2. **Custom environments**: Support for custom world implementations
3. **Advanced learning**: Support for advanced learning algorithms
4. **Custom neuromodulation**: Support for custom neuromodulation systems

## Conclusion

The improved NLM Python interface provides:

1. **Better API design**: Pythonic features and context managers
2. **Enhanced functionality**: Advanced debugging, monitoring, and memory management
3. **Improved documentation**: Comprehensive examples and references
4. **Better error handling**: More informative error messages
5. **Extensible architecture**: Easy to extend and customize

These improvements make it easier to:
- Write cleaner, more maintainable code
- Run complex simulations with advanced control
- Monitor and optimize performance
- Debug complex issues
- Conduct systematic experiments
- Manage memory efficiently

The new API is designed to be more Pythonic while maintaining full compatibility with the underlying C++ implementation.

## References

- [NLM Documentation](docs/ARCHITECTURE.md)
- [Python Bindings Guide](HOW_TO_USE.md)
- [Advanced Features](advanced_features.py)
- [Examples](examples.py)
- [GitHub Repository](https://github.com/nlm-project/nlm)