# NLM Python API - Comprehensive Guide

## Overview

This document provides comprehensive documentation for the enhanced Python API of NLM (Neural Learning Machine). The API has been completely redesigned to provide Pythonic interfaces, better error handling, and advanced utilities for both beginners and experts.

## Quick Start

### Installation

```bash
# Build and install the package
cd /workspace/app
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e python/
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

## Core API

### Types and Enums

The NLM framework provides several type and enum classes:

```python
# Neuron identifiers
neuron_id = pynlm.NeuronId()
neuron_id.value = 12345

# Synapse identifiers  
synapse_id = pynlm.SynapseId(67890)

# Region and population identifiers
region_id = pynlm.RegionId()
population_id = pynlm.PopulationId()

# Enumeration types
neuron_type = pynlm.NeuronType.Excitatory
synapse_type = pynlm.SynapseType.GapJunction
development_stage = pynlm.DevelopmentalStage.Maturation
firing_state = pynlm.FiringState.Active
```

### Core Classes

#### Config
Configuration manager for NLM systems.

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("neural.region_count", 5)
config.set("performance.optimization_level", "high")

# Get configuration values
region_count = config.get("neural.region_count")
if region_count:
    print(f"Region count: {region_count}")

# Validate configuration
errors, warnings = pynlm.validateConfiguration(config)
for error in errors:
    print(f"ERROR: {error}")
for warning in warnings:
    print(f"WARNING: {warning}")

# Export configuration
pynlm.exportConfiguration(config, "my_config.json")

# Import configuration
imported_config = pynlm.importConfigFromJson("my_config.json")
```

#### Brain
The main neural simulation brain class.

```python
# Create brain with configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Basic statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Run simulation
for i in range(1000):
    brain.step(i)

# Get detailed statistics
stats = {
    'neurons': brain.getTotalNeuronCount(),
    'synapses': brain.getTotalSynapseCount(),
    'spikes': brain.getTotalSpikeCount(),
    'firing_rate': brain.getAverageFiringRate(),
    'stage': brain.getDevelopmentalStage(),
}
```

#### PythonicBrain (Enhanced Interface)

The PythonicBrain class provides an enhanced, Pythonic interface with properties and convenience methods:

```python
# Create PythonicBrain
pythonic_brain = pynlm.PythonicBrain(brain)

# Use Pythonic properties
print(f"Neurons: {pythonic_brain.total_neuron_count}")
print(f"Firing rate: {pythonic_brain.average_firing_rate:.2f} Hz")
print(f"Development stage: {pythonic_brain.development_stage}")

# Get summary
print(pythonic_brain.get_summary())

# Get detailed statistics
detailed_stats = pythonic_brain.get_detailed_stats()
print(f"Detailed stats: {detailed_stats}")

# Enable all systems
pythonic_brain.enable_all_systems()

# Run optimized simulation
optimized_stats = pythonic_brain.run_optimized_simulation(500, 0.1)
```

#### SimpleWorld
Environment simulation system.

```python
# Create world
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, vision_width=32, vision_height=32)
world.reset()

# Update world
world.update(0.1)

# Get sensory percept
percept = world.get_sensory_percept()
print(f"Percept: {percept}")

# Apply motor commands
world.applyMotorCommand(pynlm.Action(pynlm.ActionType.MoveForward), world.get_simulation_time())
```

#### AgentBrain
Bridge connecting brain to world.

```python
# Create agent brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Process sensory input
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()
print(f"Action: {action.get_name()}")

# Apply neuromodulation
agent.apply_reward_modulation(reward=1.0, predicted_reward=0.5)

# Enable systems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)
```

#### SensoryInput and Derived Classes

```python
# Vision input
vision = pynlm.Vision(width=32, height=32, channels=3)
vision.setData([0.1] * 32 * 32 * 3)  # Example data

# Audio input
audio = pynlm.Audio(sample_rate=44100, num_samples=1024)

# Internal signals
internal = pynlm.InternalSignals()
internal.addSignal(0.5)

# Action
move_action = pynlm.Action(pynlm.ActionType.MoveForward)
action_with_params = pynlm.Action(pynlm.ActionType.Eat, [0.5, 0.3])
```

## Advanced Features

### Brain Sessions

The BrainSession class provides context manager functionality for running complete simulation sessions:

```python
# Method 1: Using context manager
with pynlm.createBrainSession(brain, world, agent) as session:
    session.run_simulation(1000, 0.1)
    print(session.get_summary())

# Method 2: Manual management
session = pynlm.createBrainSession(brain, world, agent)
try:
    session.run_simulation(1000, 0.1)
    print(session.get_summary())
finally:
    session.cleanup()
```

### Configuration Presets

The ConfigurationManager provides predefined presets for different use cases:

```python
from pynlm.config import ConfigPresets

# Research configuration (flexible, less optimized)
research_config = ConfigPresets.get_research()

# Optimization configuration (maximum performance)
optimization_config = ConfigPresets.get_optimization()

# Development configuration (learning focused)
development_config = ConfigPresets.get_development()

# Replay configuration (memory consolidation focused)
replay_config = ConfigPresets.get_replay()
```

### Experiment Management

The ExperimentManager provides tools for running systematic experiments:

```python
from pynlm.utils import ExperimentManager

# Create experiment manager
experiment_manager = ExperimentManager()

# Define parameter grid
param_grid = {
    'neural.region_count': [1, 5, 10],
    'performance.optimization_level': ['low', 'medium', 'high'],
    'plasticity.stdp.enabled': [True, False],
}

# Run grid search
results = experiment_manager.run_grid_search(
    brain_factory=lambda params: pynlm.createBrain(pynlm.ConfigPresets.get_default()),
    world_factory=lambda params: pynlm.createSimpleWorld(),
    agent_factory=lambda params: pynlm.createAgentBrain(pynlm.createBrain(pynlm.ConfigPresets.get_default())),
    param_grid=param_grid,
    evaluation_metric='final_spike_count'
)

# Analyze results
for result in results:
    if result['success']:
        print(f"Params: {result['parameters']}")
        print(f"Result: {result['metrics']}")
```

### Performance Profiling

The PerformanceProfiler helps identify bottlenecks:

```python
from pynlm.utils import PerformanceProfiler

profiler = PerformanceProfiler()

# Profile operations
def expensive_operation():
    time.sleep(0.1)
    return sum(range(1000))

result = profiler.profile_operation('expensive_calculation', expensive_operation)

# Get performance report
report = profiler.get_timing_report()
print(report)
```

## Mini-Projects

### Project 1: Silent Brain Test

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run silent test
for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Project 3: Complete Agent with Learning

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Run agent with periodic status updates
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Project 4: Advanced Session Management

```python
import pynlm

# Create all components
config = pynlm.createOptimizedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run advanced session with monitoring
with pynlm.createBrainSession(brain, world, agent) as session:
    session.run_simulation(500, 0.1)
    print("=== Session Summary ===")
    print(session.get_summary())
    
    # Get detailed metrics
    stats = session.get_stats()
    print(f"Final firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"Total spikes: {stats['total_spikes']}")

# Post-simulation analysis
pythonic_brain = pynlm.PythonicBrain(brain)
print(f"\n=== Analysis ===")
print(f"Neurons per region: {brain.getTotalNeuronCount() / brain.getRegionCount():.1f}")
print(f"Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### Project 5: Configuration Validation and Export

```python
import pynlm

# Create and modify configuration
config = pynlm.createOptimizedConfig()

# Add custom settings
config.set("custom.setting1", "value1")
config.set("custom.setting2", 42)

# Validate configuration
errors, warnings = pynlm.validateConfiguration(config)
print(f"Errors: {len(errors)}")
print(f"Warnings: {len(warnings)}")

# Get human-readable summary
print("\n=== Configuration Summary ===")
print(pynlm.printConfigSummary(config))

# Export to JSON
pynlm.exportConfigToJson(config, "my_experiment_config.json")
print("Configuration exported to my_experiment_config.json")

# Import from JSON
imported_config = pynlm.importConfigFromJson("my_experiment_config.json")
print("Configuration imported successfully")
```

## Error Handling

### Custom Exceptions

The NLM framework provides custom exception classes for different error types:

```python
import pynlm
from pynlm.core import NLMError, ValidationError, SimulationError, ConfigError

# Catch general NLM errors
try:
    # Some NLM operation
    pass
except NLMError as e:
    print(f"NLM error: {e}")

# Catch specific error types
try:
    config = pynlm.importConfigFromJson("nonexistent.json")
except ConfigError as e:
    print(f"Configuration error: {e}")

try:
    session.run_simulation(1000, 0.1)
except SimulationError as e:
    print(f"Simulation error: {e}")

try:
    config.set("invalid.key", "value")
except ValidationError as e:
    print(f"Validation error: {e}")
```

## Performance Tips

### 1. Use Optimized Configurations

```python
# Always use optimized configurations for better performance
config = pynlm.createOptimizedConfig()
```

### 2. Batch Operations

```python
# When possible, batch operations to reduce overhead
# Example: Run multiple simulations in a loop
for i in range(num_simulations):
    brain = pynlm.createBrain(pynlm.createOptimizedConfig())
    brain.initialize()
    # ... run simulation ...
```

### 3. Monitor Performance

```python
from pynlm.utils import PerformanceProfiler

profiler = PerformanceProfiler()

# Profile your simulation code
def run_simulation_with_monitoring():
    config = pynlm.createOptimizedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # ... run simulation ...
    
    world = pynlm.createSimpleWorld()
    # ... etc ...

result = profiler.profile_operation('simulation_run', run_simulation_with_monitoring)
report = profiler.get_timing_report()
print(f"Performance: {report}")
```

### 4. Memory Management

```python
# Use context managers for automatic cleanup
with pynlm.createBrainSession(brain, world, agent) as session:
    # Run simulation
    session.run_simulation(1000, 0.1)
    # Automatic cleanup

# For large simulations, consider checkpointing
if brain.getTotalSpikeCount() > 10000:
    brain.save("checkpoint.brain")
```

## Best Practices

### 1. Always Initialize Brains

```python
brain = pynlm.createBrain(config)
brain.initialize()  # Never forget this!
```

### 2. Use PythonicBrain for Enhanced Features

```python
pythonic_brain = pynlm.PythonicBrain(brain)
pythonic_brain.enable_all_systems()  # Enable all systems at once
```

### 3. Validate Configuration Before Use

```python
config = pynlm.createOptimizedConfig()
errors, warnings = pynlm.validateConfiguration(config)
if errors:
    raise ValueError(f"Invalid configuration: {errors}")
```

### 4. Handle Errors Gracefully

```python
try:
    # Your simulation code
    pass
except (NLMError, ValidationError, SimulationError) as e:
    print(f"Simulation failed: {e}")
    # Implement fallback or recovery logic
```

### 5. Use Appropriate Timesteps

```python
# Use smaller timesteps for accuracy, larger for performance
brain.step(i)  # Small timestep (1 unit)
# vs
world.update(0.01)  # Very small timestep (0.01 seconds)
```

## Migration Guide

### From Old API to New API

#### Changes in Class Names

| Old Class | New Class | Notes |
|-----------|-----------|-------|
| Config | Config | Same name, enhanced functionality |
| Brain | Brain | Same class, enhanced API |
| AgentBrain | AgentBrain | Same class, enhanced API |
| SimpleWorld | SimpleWorld | Same class, enhanced API |

#### New Methods and Properties

| Feature | Old Way | New Way |
|---------|---------|---------|
| Statistics | `brain.getTotalNeuronCount()` | `pythonic_brain.total_neuron_count` (property) |
| Configuration | Manual validation | `pynlm.validateConfiguration(config)` |
| Export/Import | Manual file operations | `pynlm.exportConfigToJson()`, `pynlm.importConfigFromJson()` |
| Session Management | Manual setup/cleanup | `pynlm.createBrainSession()` with context manager |
| Configuration Presets | Manual setup | `ConfigPresets.get_optimization()`, `ConfigPresets.get_research()` |

#### Example Migration

```python
# OLD WAY
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(1000):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")

# NEW WAY
pythonic_brain = pynlm.PythonicBrain(pynlm.createBrain(config))
pythonic_brain.enable_all_systems()

with pynlm.createBrainSession(pythonic_brain._brain, world, agent) as session:
    session.run_simulation(1000, 0.1)

print(f"Total spikes: {pythonic_brain.total_spike_count}")
```

## Frequently Asked Questions

### Q: What is the difference between `Brain` and `PythonicBrain`?

A: `Brain` is the original C++ wrapped class with standard method calls. `PythonicBrain` is an enhanced wrapper that provides:

- Pythonic properties (e.g., `total_neuron_count` instead of `getTotalNeuronCount()`)
- Convenience methods (e.g., `enable_all_systems()`)
- Better type hints and documentation
- Improved error handling

### Q: When should I use `BrainSession`?

A: Use `BrainSession` when you need:

- Automatic resource cleanup with context managers
- Complete simulation management
- Statistics collection and reporting
- Error handling and recovery

### Q: What are configuration presets for?

A: Configuration presets provide ready-to-use configurations for different use cases:

- `get_default()`: Standard balanced configuration
- `get_research()`: Flexible configuration for research
- `get_optimization()`: Maximum performance configuration
- `get_development()`: Learning-focused configuration
- `get_replay()`: Memory consolidation focused configuration

### Q: How do I profile my simulations?

A: Use the `PerformanceProfiler` class:

```python
from pynlm.utils import PerformanceProfiler

profiler = PerformanceProfiler()
result = profiler.profile_operation('simulation', run_simulation)
report = profiler.get_timing_report()
```

### Q: Can I run multiple simulations in parallel?

A: Yes, NLM supports concurrent simulations using the PythonicBrain interface:

```python
import threading

def run_simulation(i):
    brain = pynlm.createBrain(pynlm.createOptimizedConfig())
    # ... run simulation ...

threads = []
for i in range(10):
    thread = threading.Thread(target=run_simulation, args=(i,))
    thread.start()
    threads.append(thread)

for thread in threads:
    thread.join()
```

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM Architecture](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)

## License

MIT License

## Version

NLM Python Package v1.0.0-enhanced

---

**Note:** This documentation covers the enhanced NLM Python API. For information about the C++ core or other components, refer to the respective documentation files in the `docs/` directory.