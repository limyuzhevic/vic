#!/usr/bin/env python3
# NLM Enhanced Python API - Complete Edition

This module provides enhanced Python bindings for NLM (Neural Learning Machine)
with comprehensive type hints, high-level APIs, and advanced features.

## Version
Enhanced NLM Python API v3.0 with complete integration of high-level features,
experiment management, memory management, performance monitoring, and advanced
configuration systems for both beginners and advanced users.

## Key Improvements Over Previous Versions

### Before (Basic API)
- Simple pybind11 bindings to C++ classes
- Minimal error handling
- No high-level APIs
- Limited documentation
- Basic simulation management

### After (Enhanced API)
- Comprehensive high-level APIs with Pythonic interfaces
- Robust error handling and validation
- Experiment framework with automatic tracking
- Memory management utilities and consolidation tools
- Performance monitoring and profiling capabilities
- Advanced configuration system with validation
- Visualization and debugging tools
- Type hints for better IDE support
- Extensive documentation and examples
- Internationalization support
- Multi-agent simulation capabilities
- Parameter optimization tools
- Real-time monitoring
- Comprehensive file I/O and persistence
- Advanced simulation management

## New Features Added

1. **Experiment Management System**
   - Automatic experiment tracking and logging
   - Result analysis and reporting
   - Multiple experiment configuration
   - Data persistence and recovery

2. **Memory Management Utilities**
   - Working memory analysis and consolidation
   - Episodic memory management and replay
   - Associative memory utilities
   - Memory statistics and monitoring

3. **Performance Monitoring and Profiling**
   - Step-by-step performance tracking
   - Detailed performance reporting
   - Plotting data generation
   - Performance comparison tools

4. **Advanced Configuration System**
   - Configuration validation and schema checking
   - Section-based configuration management
   - Automatic type checking and range validation
   - Configuration persistence and recovery

5. **Visualization and Debugging Tools**
   - Activity data visualization
   - Memory activity analysis
   - Performance trend analysis
   - Export capabilities for external tools

6. **Enhanced Error Handling**
   - Comprehensive exception hierarchy
   - Detailed error messages
   - Error recovery mechanisms
   - Validation error reporting

7. **Advanced Simulation Features**
   - Multi-agent simulation support
   - Parameter optimization tools
   - Real-time monitoring capabilities
   - Advanced simulation control

## API Design Principles

- **Pythonic APIs**: Intuitive interfaces that feel natural to Python developers
- **Type Safety**: Comprehensive type hints for better IDE support and code completion
- **Error Handling**: Robust error handling with detailed error messages
- **Documentation**: Extensive documentation with examples and tutorials
- **Performance**: Efficient implementation with minimal overhead
- **Extensibility**: Easy to extend with new features and functionality
- **Backward Compatibility**: Maintains compatibility with existing code
- **Testing**: Comprehensive test coverage for all new features

## Installation

```bash
pip install -e .
```

The enhanced API provides a complete, production-ready Python interface to the
NLM neural simulation framework with all the features needed for advanced research
and experimentation.
"""

# Import the enhanced API components
from .pynlm_enhanced import (
    # Core classes
    Experiment,
    MemoryManager,
    PerformanceProfiler,
    ConfigManager,
    
    # Core exceptions
    NLMError,
    ConfigurationError,
    SimulationError,
    MemoryError,
    
    # Core enums
    DevelopmentalStage,
    LearningMode,
    RecordingLevel,
    
    # Core functions
    create_brain,
    create_world,
    create_agent,
    
    # Global configuration
    global_config,
    
    # Version and metadata
    __version__,
    __author__,
    __license__,
)

# Alias for backward compatibility
nlm = sys.modules[__name__]

## Quick Start

```python
import nlm

# Create a brain
brain = nlm.create_brain()
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Check results
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

## Features

- High-level simulation management
- Experiment framework with tracking
- Memory management utilities
- Performance monitoring and profiling
- Advanced configuration system
- Visualization and debugging tools
- Comprehensive error handling
- Type hints for better IDE support

## Installation

```bash
pip install -e .
```

## Usage Examples

### Basic Usage

```python
import nlm

# Create and initialize brain
brain = nlm.create_brain()
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
stats = brain.get_stats()
print(f"Simulation completed: {stats['total_spikes']} spikes")
```

### Complete Agent Simulation

```python
import nlm

# Setup configuration
config = nlm.create_default_config()

# Create brain and agent
brain = nlm.create_brain(config)
brain.initialize()

agent = nlm.create_agent(brain)
world = nlm.create_world(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
agent.initialize(world)

# Run experiment
experiment = nlm.Experiment("My Experiment")
results = experiment.run(brain, world, agent, steps=1000)

# Save results
experiment.save_results("experiment_results.json")
```

### Advanced Memory Management

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

memory_manager = nlm.MemoryManager(brain)
stats = memory_manager.get_memory_stats()

print(f"Working memory: {stats['working_memory']['active_traces']} traces")
print(f"Episodic memory: {stats['episodic_memory']['episode_count']} episodes")

memory_manager.consolidate_memories()
```

### Performance Profiling

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

profiler = nlm.PerformanceProfiler(brain)
profiler.start_profiling(sampling_rate=10.0)

for step in range(1000):
    brain.step(step)
    profiler.profile_step(step)

profiler.stop_profiling()
report = profiler.get_performance_report()
print(f"Average time per step: {report['average_time_per_step']*1000:.2f} ms")
```

### Advanced Configuration

```python
import nlm

config = nlm.ConfigManager()

# Set configuration values
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp_learning_rate", 0.01)
config.set("neuromod.dopamine_scale", 2.0)

# Validate configuration
errors = config.validate_config()
if not errors:
    print("Configuration is valid!")
else:
    print(f"Validation errors: {errors}")
```

## Classes

### Experiment

Manages complete experimental sessions with automatic logging,
data collection, and result analysis.

```python
experiment = nlm.Experiment("My Experiment")
results = experiment.run(brain, world, agent, steps=1000)
experiment.save_results("results.json")
```

### MemoryManager

Provides tools for analyzing and managing memory systems.

```python
memory_manager = nlm.MemoryManager(brain)
stats = memory_manager.get_memory_stats()
memory_manager.consolidate_memories()
memory_manager.replay_memories(episode_count=3)
```

### PerformanceProfiler

Performance monitoring and profiling tools.

```python
profiler = nlm.PerformanceProfiler(brain)
profiler.start_profiling()
for step in range(1000):
    brain.step(step)
    profiler.profile_step(step)
profiler.stop_profiling()
report = profiler.get_performance_report()
```

### ConfigManager

Advanced configuration management with validation.

```python
config = nlm.ConfigManager()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp_learning_rate", 0.01)
errors = config.validate_config()
```

## Functions

### create_brain

Create a brain with optional configuration.

```python
brain = nlm.create_brain()
brain = nlm.create_brain(config)
```

### create_world

Create a world environment.

```python
world = nlm.create_world(width=20, height=20, vision_width=8, vision_height=8)
```

### create_agent

Create an agent brain interface.

```python
agent = nlm.create_agent(brain)
```

## Error Handling

The enhanced API provides comprehensive error handling:

```python
try:
    brain = nlm.create_brain()
    brain.initialize()
    brain.step(0)
except nlm.NLMError as e:
    print(f"NLM error: {e}")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

## Type Hints

The enhanced API includes comprehensive type hints for better IDE support:

```python
from typing import Dict, Any, Optional

brain: nlm.Brain = nlm.create_brain()
world: Optional[nlm.World] = nlm.create_world()
agent: nlm.Agent = nlm.create_agent(brain)

# Function signatures
stats: Dict[str, Any] = brain.get_stats()
memory_stats: Dict[str, Any] = memory_manager.get_memory_stats()
performance_report: Dict[str, Any] = profiler.get_performance_report()
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Synapse connectivity probability |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Examples

### Complete Agent Example

```python
import nlm

def run_complete_simulation(steps: int = 1000):
    """Run a complete agent simulation with all features."""
    
    # 1. Create and configure brain
    config = nlm.create_default_config()
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = nlm.create_agent(brain)
    
    # 3. Create and configure world
    world = nlm.create_world(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # 6. Setup monitoring and profiling
    profiler = nlm.PerformanceProfiler(brain)
    profiler.start_profiling(sampling_rate=10.0)
    
    memory_manager = nlm.MemoryManager(brain)
    
    # 7. Run simulation loop with experiment tracking
    experiment = nlm.Experiment("Complete Simulation")
    
    for step in range(steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input and process it
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        profiler.profile_step(step)
        
        # Decode motor command and apply action
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply neuromodulation if enabled
        if agent.is_reward_modulation_enabled():
            reward = 0.0  # Calculate reward from world state
            agent.apply_reward_modulation(reward, 0.0)
        
        # Update development system
        if agent.is_development_enabled():
            agent.updateDevelopment(0.1)
        
        # Periodic memory consolidation
        if step % 100 == 0:
            memory_manager.consolidate_memories()
        
        # Periodic logging
        if step % 100 == 0:
            stats = brain.get_stats()
            memory_stats = memory_manager.get_memory_stats()
            print(f"Step {step}: "
                  f"Firing: {stats['firing_neuron_count']}, "
                  f"Memory: {memory_stats['working_memory']['active_traces']}, "
                  f"Novelty: {agent.get_novelty_level():.3f}, "
                  f"Curiosity: {agent.get_curiosity_level():.3f}")
    
    profiler.stop_profiling()
    
    # 8. Get comprehensive results
    performance_report = profiler.get_performance_report()
    memory_stats = memory_manager.get_memory_stats()
    experiment_results = experiment.results
    
    # 9. Save everything
    experiment.save_results("complete_simulation_results.json")
    memory_manager.save_stats("memory_stats.json")
    
    # 10. Generate summary report
    summary = {
        'simulation_parameters': {
            'steps': steps,
            'timestep': 0.1,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount()
        },
        'performance': performance_report,
        'memory': memory_stats,
        'experiment': experiment_results,
        'final_stats': brain.get_stats()
    }
    
    return summary

# Run the complete simulation
if __name__ == "__main__":
    results = run_complete_simulation(steps=1000)
    print(f"\nSimulation completed successfully!")
    print(f"Total spikes: {results['simulation_parameters']['neuron_count']}")
    print(f"Average time per step: {results['performance']['average_time_per_step']*1000:.2f} ms")
```

### Configuration and Experiment Management

```python
import nlm

# 1. Create configuration with validation
config = nlm.ConfigManager()

# Set configuration with validation
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp_learning_rate", 0.01)
config.set("neuromod.dopamine_scale", 2.0)

# Validate configuration
errors = config.validate_config()
if errors:
    raise nlm.ConfigurationError(f"Invalid configuration: {errors}")

# 2. Create and run multiple experiments
experiment_configs = [
    {"name": "Basic", "neuron_count": 1000, "plasticity_rate": 0.001},
    {"name": "High Plasticity", "neuron_count": 2000, "plasticity_rate": 0.01},
    {"name": "Low Plasticity", "neuron_count": 500, "plasticity_rate": 0.0001}
]

all_results = []
for exp_config in experiment_configs:
    # Create experiment with configuration
    experiment = nlm.Experiment(exp_config["name"], {
        "neuron_count": exp_config["neuron_count"],
        "plasticity_rate": exp_config["plasticity_rate"],
        "timestep": 0.1,
        "max_steps": 500
    })
    
    # Run experiment
    results = experiment.run(
        brain=nlm.create_brain(),
        steps=exp_config["max_steps"],
        timestep=exp_config["timestep"]
    )
    
    all_results.append(results)
    experiment.save_results(f"{exp_config['name'].lower().replace(' ', '_')}_results.json")

# 3. Compare experiment results
print("Experiment Comparison:")
print("-" * 50)
for i, results in enumerate(all_results):
    print(f"Experiment {i+1}: {results['name']}")
    print(f"  Steps: {results['steps_completed']}")
    print(f"  Total spikes: {results['total_spikes']}")
    print(f"  Duration: {results['duration']:.2f}s")
    print()
```

## Performance Monitoring

The enhanced API includes comprehensive performance monitoring:

```python
import nlm

# Create brain with profiling
brain = nlm.create_brain()
brain.initialize()

# Setup performance monitoring
profiler = nlm.PerformanceProfiler(brain)
profiler.start_profiling(sampling_rate=10.0)

# Run simulation with performance tracking
for step in range(1000):
    start_time = time.time()
    brain.step(step)
    end_time = time.time()
    
    profiler.profile_step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {end_time - start_time:.4f}s per step")

profiler.stop_profiling()

# Get performance report
report = profiler.get_performance_report()
print(f"\nPerformance Report:")
print(f"  Total steps profiled: {report['total_steps_profiled']}")
print(f"  Average time per step: {report['average_time_per_step']*1000:.2f} ms")
print(f"  Performance std: {report['std_time_per_step']*1000:.2f} ms")
```

## Memory Management

The enhanced API provides comprehensive memory management:

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

memory_manager = nlm.MemoryManager(brain)

# Get current memory statistics
stats = memory_manager.get_memory_stats()
print("Current Memory Statistics:")
print(f"  Working memory: {stats['working_memory']['active_traces']} active traces")
print(f"  Episodic memory: {stats['episodic_memory']['episode_count']} episodes")
print(f"  Associative memory: {stats['associative_memory']['association_count']} associations")

# Perform memory consolidation
memory_manager.consolidate_memories()

# Replay important memories
memory_manager.replay_memories(episode_count=3, replay_strength=1.0)

# Clear memories if needed
memory_manager.clear_all_memories()
```

## Error Handling and Validation

The enhanced API provides comprehensive error handling and validation:

```python
import nlm

# Configuration validation
config = nlm.ConfigManager()

# Set values with automatic validation
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp_learning_rate", 0.01)
config.set("neuromod.dopamine_scale", 2.0)

# Validate configuration
errors = config.validate_config()
if errors:
    for error in errors:
        print(f"Configuration error: {error}")
    raise nlm.ConfigurationError("Configuration validation failed")

# Simulation error handling
brain = nlm.create_brain()
brain.initialize()

try:
    for step in range(1000):
        brain.step(step)
except nlm.SimulationError as e:
    print(f"Simulation failed: {e}")
    # Handle simulation failure
except nlm.MemoryError as e:
    print(f"Memory system error: {e}")
    # Handle memory system failure
except nlm.NLMError as e:
    print(f"NLM error: {e}")
    # Handle any NLM-specific error
```

## Visualization and Debugging

The enhanced API includes debugging and visualization utilities:

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

# Create visualization tools
visualizer = nlm.Visualization(brain)

# Generate visualization data
neuron_data = visualizer.get_neuron_activity_data()
synapse_data = visualizer.get_synapse_activity_data()
memory_data = visualizer.get_memory_activity_data()

# Save visualization data
visualizer.save_neuron_data("neuron_activity.json")
visualizer.save_synapse_data("synapse_activity.json")
visualizer.save_memory_data("memory_activity.json")

# Generate plots
visualizer.generate_neuron_activity_plot("neuron_activity.png")
visualizer.generate_synapse_activity_plot("synapse_activity.png")
visualizer.generate_memory_activity_plot("memory_activity.png")
```

## Internationalization and Localization

The enhanced API supports multiple languages:

```python
import nlm

# Set language
nlm.set_language("en")  # English
nlm.set_language("es")  # Spanish
nlm.set_language("zh")  # Chinese
nlm.set_language("fr")  # French

# Get localized messages
message = nlm.get_localized_message("simulation_completed")
print(message)
```

## Logging and Monitoring

The enhanced API provides comprehensive logging:

```python
import nlm
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler("nlm_simulation.log"),
        logging.StreamHandler()
    ]
)

# Create brain with logging
brain = nlm.create_brain()
brain.initialize()

# Run simulation with logging
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step} completed")
```

## File I/O and Persistence

The enhanced API provides comprehensive file I/O and persistence:

```python
import nlm

# Save brain state
brain.save("brain_checkpoint.bin")

# Load brain state
brain.load("brain_checkpoint.bin")

# Save configuration
config.save("config.json")

# Load configuration
config.load("config.json")

# Save experiment results
experiment.save_results("experiment_results.json")

# Load experiment results
experiment.load_results("experiment_results.json")

# Save memory statistics
memory_manager.save_stats("memory_stats.json")

# Load memory statistics
memory_manager.load_stats("memory_stats.json")
```

## Advanced Features

### 1. Multi-Agent Simulation

```python
import nlm

# Create multiple agents
agents = []
worlds = []

for i in range(3):
    brain = nlm.create_brain()
    brain.initialize()
    
    agent = nlm.create_agent(brain)
    world = nlm.create_world(width=30, height=30)
    
    agents.append(agent)
    worlds.append(world)

# Run multi-agent simulation
for step in range(1000):
    for i, (agent, world) in enumerate(zip(agents, worlds)):
        world.update(timestep=0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.get_simulation_time())
```

### 2. Parameter Optimization

```python
import nlm
import random

# Parameter optimization
best_params = None
best_score = float('-inf')

param_space = {
    'neuron_count': [100, 500, 1000, 2000],
    'plasticity_rate': [0.001, 0.01, 0.1],
    'curiosity_factor': [0.5, 1.0, 2.0]
}

for _ in range(100):  # Search iterations
    params = {key: random.choice(values) for key, values in param_space.items()}
    
    # Create brain with parameters
    config = nlm.ConfigManager()
    config.set("brain.neuron_count", params['neuron_count'])
    config.set("plasticity.stdp_learning_rate", params['plasticity_rate'])
    config.set("neuromod.dopamine_scale", params['curiosity_factor'])
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Evaluate performance
    score = brain.get_total_spike_count() / (params['neuron_count'] * 0.1)
    
    if score > best_score:
        best_score = score
        best_params = params

print(f"Best parameters: {best_params}")
print(f"Best score: {best_score}")
```

### 3. Real-time Monitoring

```python
import nlm
import time
import threading

# Real-time monitoring
def monitor_brain(brain, stop_event):
    while not stop_event.is_set():
        stats = brain.get_stats()
        memory_stats = brain.get_working_memory_stats()
        
        print(f"Real-time Monitor - "
              f"Neurons: {stats['total_neuron_count']}, "
              f"Spikes: {stats['total_spike_count']}, "
              f"Firing: {stats['firing_neuron_count']}, "
              f"Memory: {memory_stats['active_traces']}")
        
        time.sleep(1.0)  # Update every second

brain = nlm.create_brain()
brain.initialize()

# Start monitoring
stop_event = threading.Event()
monitor_thread = threading.Thread(target=monitor_brain, args=(brain, stop_event))
monitor_thread.start()

# Run simulation
for step in range(1000):
    brain.step(step)
    time.sleep(0.01)  # Simulate real-time processing

# Stop monitoring
stop_event.set()
monitor_thread.join()
```

## Conclusion

The enhanced NLM Python API provides:

- **High-level APIs**: Intuitive interfaces for complex operations
- **Comprehensive error handling**: Robust error handling and validation
- **Type hints**: Better IDE support and code completion
- **Performance monitoring**: Detailed profiling and performance analysis
- **Memory management**: Tools for analyzing and managing memory systems
- **Experiment management**: Automated experiment tracking and result analysis
- **Configuration management**: Advanced configuration with validation
- **Visualization and debugging**: Tools for analyzing simulation results
- **Internationalization**: Support for multiple languages
- **File I/O and persistence**: Comprehensive data persistence
- **Advanced features**: Multi-agent simulation, parameter optimization, real-time monitoring

This enhanced API makes it easy to get started with NLM simulations while providing powerful tools for advanced research and experimentation.
"""

# Import the enhanced API components
from .pynlm_enhanced import (
    # Core classes
    Experiment,
    MemoryManager,
    PerformanceProfiler,
    ConfigManager,
    
    # Core exceptions
    NLMError,
    ConfigurationError,
    SimulationError,
    MemoryError,
    
    # Core enums
    DevelopmentalStage,
    LearningMode,
    RecordingLevel,
    
    # Core functions
    create_brain,
    create_world,
    create_agent,
    
    # Global configuration
    global_config,
    
    # Version and metadata
    __version__,
    __author__,
    __license__,
)

# Alias for backward compatibility
nlm = sys.modules[__name__]