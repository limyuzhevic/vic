# Advanced NLM Python API Guide

## Overview

This guide provides comprehensive documentation for the advanced features of the NLM (Neural Learning Machine) Python API. It covers Pythonic wrapper classes, advanced configuration systems, performance monitoring tools, experiment management, and custom environment creation.

## Table of Contents

1. [Introduction](#introduction)
2. [Pythonic Wrapper Classes](#pythonic-wrapper-classes)
   - [EnhancedBrain](#enhancedbrain)
   - [World](#world)
   - [Agent](#agent)
   - [MemoryManager](#memorymanager)
3. [Advanced Configuration API](#advanced-configuration-api)
   - [PyConfig](#pyconfig)
   - [EnvironmentConfig](#environmentconfig)
   - [ValidatedConfig](#validatedconfig)
4. [Performance Monitoring](#performance-monitoring)
   - [PerformanceProfiler](#performanceprofiler)
   - [MemoryMonitor](#memorymonitor)
   - [CPUMonitor](#cpumonitor)
   - [SimulationProfiler](#simulationprofiler)
5. [Experiment Management](#experiment-management)
   - [AdvancedExperiment](#advancedexperiment)
   - [ExperimentSuite](#experimentsuite)
6. [Custom Neuron Populations](#custom-neuron-populations)
7. [Batch Processing](#batch-processing)
8. [Advanced Usage Examples](#advanced-usage-examples)
   - [Parameter Sweep Experiment](#parameter-sweep-experiment)
   - [Multi-Environment Testing](#multi-environment-testing)
   - [Agent Research](#agent-research)
   - [Performance Benchmarking](#performance-benchmarking)
9. [Configuration Files](#configuration-files)
10. [Troubleshooting](#troubleshooting)

## Introduction

The enhanced NLM Python API provides a comprehensive interface for neural simulation research and experimentation. It builds upon the original C++ bindings with:

- **Pythonic wrapper classes** that provide intuitive interfaces
- **Advanced configuration systems** with JSON/YAML support
- **Performance monitoring** tools for optimization
- **Experiment management** for systematic research
- **Custom neuron population** creation for specialized research
- **Batch processing** utilities for large-scale simulations

## Pythonic Wrapper Classes

### EnhancedBrain

The `EnhancedBrain` class provides a Pythonic interface to the NLM brain simulation system.

#### Key Features

- **Context manager support** for automatic resource management
- **Property-based access** for easy configuration
- **Convenience methods** for common operations
- **Enhanced error handling** with Pythonic exceptions

#### Basic Usage

```python
from nlm import EnhancedBrain, Config

# Create enhanced brain with default configuration
brain = EnhancedBrain()

# Initialize with custom configuration
config = Config()
config.setInt("world.width", 200)
config.setInt("world.height", 200)
brain = EnhancedBrain(config)

# Context manager support
with EnhancedBrain() as brain:
    brain.initialize()
    # Use brain here
    pass  # Brain is automatically cleaned up

# Access brain properties
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

#### Advanced Features

```python
# Set developmental stage
brain.setDevelopmentalStage(DevelopmentalStage.Maturation)

# Inject current into specific neuron
brain.injectCurrent(neuron_id, 0.5)

# Inject current to all neurons of a specific type
brain.injectCurrentToNeurons(NeuronType.Excitatory, 0.3)

# Save and load brain state
brain.save("brain_checkpoint.json")
brain.load("brain_checkpoint.json")

# Get brain statistics
stats = {
    'total_neurons': brain.getTotalNeuronCount(),
    'total_synapses': brain.getTotalSynapseCount(),
    'active_neurons': brain.getActiveNeuronCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'average_firing_rate': brain.getAverageFiringRate(),
    'total_spikes': brain.getTotalSpikeCount(),
    'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio()
}
```

### World

The `World` class provides an enhanced interface to the NLM world simulation.

#### Key Features

- **Configuration presets** for common world setups
- **Convenient action methods** for agent control
- **World state management** for easy reset and modification
- **Position and boundary checking**

#### Basic Usage

```python
from nlm import World, Action, ActionType

# Create world
world = World()

# Configure world
world.configure(
    width=100,
    height=100,
    vision_width=10,
    vision_height=10
)

# Set agent starting position
world.setAgentStart(50, 50)

# Get current state
agent_body = world.getAgentBody()
percept = world.getPercept()

# Apply actions
action = Action(ActionType.MoveForward)
world.applyAction(action)

# Step world
world.step(0.01)  # 0.01 time steps

# Check position
if world.isValidPosition(55, 55):
    print("Position is valid")

# Reset world
world.reset()
```

#### Advanced Features

```python
# Set world parameters
world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.005)

# Set random seed for reproducible results
world.setRandomSeed(42)

# Get world dimensions
width, height = world.getWidth(), world.getHeight()
max_energy = world.getMaxEnergy()
energy_decay = world.getEnergyDecayRate()

# Context manager support
with World() as world:
    # Use world here
    pass
```

### Agent

The `Agent` class provides high-level abstractions for agent-based research.

#### Key Features

- **Batch processing** for efficient simulation
- **State management** for tracking agent state
- **Memory integration** for working and episodic memory
- **Convenient step methods** for simple control

#### Basic Usage

```python
from nlm import Agent, Action

# Create agent
agent = Agent()

# Initialize agent
agent.initialize()

# Step agent (simple interface)
action = agent.step()

# Get agent state
state = agent.getState()
print(f"Agent position: {state['x']}, {state['y']}")
print(f"Agent energy: {state['energy']}")

# Batch processing (efficient for research)
actions = agent.stepBatch(100)  # Get 100 actions

# Reset agent for new episode
agent.reset()

# Context manager support
with Agent() as agent:
    # Use agent here
    pass
```

#### Advanced Features

```python
# Get memory manager
memory_manager = agent.getMemoryManager()

# Working memory operations
memory_manager.storeWorkingMemory("temp", "value")
retrieved = memory_manager.getWorkingMemory("temp")

# Episodic memory operations
memory_manager.storeEpisodicMemory("episode1", ["event1", "event2"])
episode = memory_manager.getEpisodicMemory("episode1")

# Associative memory operations
memory_manager.storeAssociation("key1", "key2", 0.8)
strength = memory_manager.getAssociation("key1", "key2")

# Memory consolidation
memory_manager.consolidateMemory()
```

### MemoryManager

The `MemoryManager` class provides comprehensive memory system management.

#### Key Features

- **Working memory** for transient information storage
- **Episodic memory** for experience recording and replay
- **Associative memory** for pattern association
- **Memory consolidation** for long-term storage
- **Memory replay** for reinforcement learning

#### Basic Usage

```python
from nlm import MemoryManager

# Create memory manager for brain
memory_manager = MemoryManager(brain)

# Working memory operations
memory_manager.storeWorkingMemory("current_focus", "goal_achieved")
working_value = memory_manager.getWorkingMemory("current_focus")
memory_manager.clearWorkingMemory()

# Episodic memory operations
memory_manager.storeEpisodicMemory("day_1", [
    "explored_room",
    "found_resource",
    "avoided_hazard"
])
episode = memory_manager.getEpisodicMemory("day_1")

# Associative memory operations
memory_manager.storeAssociation("reward", "approach", 1.0)
memory_manager.storeAssociation("danger", "avoid", 0.9)
strength = memory_manager.getAssociation("reward", "approach")

# Memory consolidation
memory_manager.consolidateMemory()

# Memory replay
memory_manager.replayMemory("day_1")
```

## Advanced Configuration API

### PyConfig

The `PyConfig` class provides enhanced configuration management with JSON/YAML support.

#### Key Features

- **Typed value access** with automatic conversion
- **Property-based configuration** for easy access
- **File loading** with JSON and YAML support
- **Command line integration**
- **Validation support** through ValidatedConfig

#### Basic Usage

```python
from nlm import Config, PyConfig

# Create configuration
config = PyConfig()

# Set values using typed methods
config.setInt("world.width", 200)
config.setFloat("energy.decay_rate", 0.01)
config.setBool("agent.enabled", True)
config.setString("experiment.name", "my_experiment")

# Get values using typed methods
width = config.getInt("world.width", 100)
decay_rate = config.getFloat("energy.decay_rate", 0.05)
enabled = config.getBool("agent.enabled", False)
name = config.getString("experiment.name", "default")

# Load from file
config.loadFromFile("config.json")
config.loadFromYAML("config.yaml")

# Save to file
config.saveToFile("experiment_config.json")

# Configuration keys
keys = config.getKeys()
print(f"Available keys: {keys}")

# Check existence
if config.has("world.width"):
    print("World width is configured")

# String representation
print(config.toString())
```

#### Advanced Features

```python
# Create configuration from command line
config = Config()
config.loadFromArgs(2, ["--width", "200", "--height", "200"])

# Environment variable overrides
env_config = EnvironmentConfig()
env_config.loadFromEnvironment(config)

# Configuration validation
validated_config = ValidatedConfig(config)
validated_config.addValidationRule("world.width", lambda x: x > 0, "Width must be positive")
validated_config.addValidationRule("energy.decay_rate", lambda x: 0 <= x <= 1, "Decay rate must be between 0 and 1")

if validated_config.isValid():
    print("Configuration is valid")
else:
    print(f"Validation errors: {validated_config.getValidationErrors()}")
```

### EnvironmentConfig

The `EnvironmentConfig` class loads configuration from environment variables.

#### Key Features

- **Environment variable loading** for system-wide configuration
- **Type-safe access** with automatic conversion
- **Override capability** for runtime configuration
- **Integration with system environment**

#### Basic Usage

```python
from nlm import EnvironmentConfig

# Create environment configuration
env_config = EnvironmentConfig()

# Load from environment variables
config = Config()
env_config.loadFromEnvironment(config)

# Access environment variables
width = int(EnvironmentConfig.getEnv("NLM_WORLD_WIDTH", "100"))
height = int(EnvironmentConfig.getEnv("NLM_WORLD_HEIGHT", "100"))
seed = int(EnvironmentConfig.getEnv("NLM_RANDOM_SEED", "42"))

# Check if environment variable exists
if EnvironmentConfig.hasEnv("NLM_EXPERIMENT_NAME"):
    print(f"Experiment name: {EnvironmentConfig.getEnv('NLM_EXPERIMENT_NAME')}")
```

### ValidatedConfig

The `ValidatedConfig` class provides configuration validation.

#### Key Features

- **Rule-based validation** for configuration values
- **Descriptive error messages** for validation failures
- **Automatic validation** with configurable rules
- **Integration with other configuration systems**

#### Basic Usage

```python
from nlm import Config, ValidatedConfig

# Create configuration and validation
config = Config()
config.setInt("world.width", 100)
config.setFloat("energy.decay_rate", 0.01)

validated_config = ValidatedConfig(config)

# Add validation rules
validated_config.addValidationRule("world.width", lambda x: x > 0, "Width must be positive")
validated_config.addValidationRule("world.height", lambda x: x > 0, "Height must be positive")
validated_config.addValidationRule("energy.decay_rate", lambda x: 0 <= x <= 1, "Decay rate must be between 0 and 1")
validated_config.addValidationRule("random.seed", lambda x: x >= 0, "Seed must be non-negative")

# Validate configuration
if validated_config.isValid():
    print("Configuration is valid")
else:
    errors = validated_config.getValidationErrors()
    print(f"Validation errors: {errors}")
```

## Performance Monitoring

### PerformanceProfiler

The `PerformanceProfiler` class provides comprehensive performance profiling.

#### Key Features

- **Multi-threaded monitoring** for real-time performance tracking
- **Memory and CPU profiling** for system resource monitoring
- **Benchmark testing** for performance comparison
- **Report generation** for analysis and documentation
- **Comparison tools** for performance optimization

#### Basic Usage

```python
from nlm import PerformanceProfiler

# Create performance profiler
profiler = PerformanceProfiler()

# Start profiling
profiler.start_profiling()

# Profile a function
@profiler.benchmark_function
def my_function():
    # Simulate work
    time.sleep(0.1)
    return "result"

# Run benchmark
result = my_function()
print(f"Function executed in {result.execution_time:.3f} seconds")
print(f"Memory usage: {result.memory_usage:.2f} MB")
print(f"Success: {result.success}")

# Stop profiling
profiler.stop_profiling()

# Get summary
summary = profiler.get_summary()
print(f"Summary: {summary}")

# Generate report
report = profiler.generate_report()
print(report)
```

#### Advanced Features

```python
# Profile simulation
profiler = PerformanceProfiler()
result = profiler.benchmark_simulation(brain, world, num_steps=100, num_runs=10)

# Run benchmark suite
test_functions = [my_function1, my_function2, my_function3]
suite = profiler.run_suite(test_functions, "my_suite")

# Compare results
comparison = profiler.compare_results(result1, result2)
print(f"Comparison: {comparison}")
```

### MemoryMonitor

The `MemoryMonitor` class provides real-time memory monitoring.

#### Key Features

- **Continuous monitoring** with configurable sampling intervals
- **Memory tracing** for detailed memory analysis
- **Historical data storage** for trend analysis
- **Peak memory tracking** for resource planning
- **Python object tracking** for memory leak detection
- **NumPy array monitoring** for array-heavy applications

#### Basic Usage

```python
from nlm import MemoryMonitor

# Create memory monitor
memory_monitor = MemoryMonitor(sampling_interval=0.1)

# Get current memory
metrics = memory_monitor.get_current_memory()
print(f"Current memory: {metrics.rss_mb:.2f} MB")
print(f"Python objects: {metrics.python_objects}")
print(f"NumPy arrays: {metrics.numpy_arrays}")

# Start monitoring
memory_monitor.start_monitoring()

# Get average memory
avg_metrics = memory_monitor.get_average_memory()
print(f"Average memory: {avg_metrics.rss_mb:.2f} MB")

# Get peak memory
peak_metrics = memory_monitor.get_peak_memory()
print(f"Peak memory: {peak_metrics.rss_mb:.2f} MB")

# Stop monitoring
memory_monitor.stop_monitoring()

# Use as context manager
with MemoryMonitor() as monitor:
    # Monitor memory here
    time.sleep(1)
```

#### Advanced Features

```python
# Save memory history
memory_monitor.save_history("memory_history.json")

# Load memory history
memory_monitor.load_history("memory_history.json")

# Get history size
print(f"History size: {memory_monitor.get_history_size()} samples")
```

### CPUMonitor

The `CPUMonitor` class provides real-time CPU monitoring.

#### Key Features

- **Continuous CPU monitoring** with configurable sampling
- **Load average tracking** for system performance analysis
- **Frequency monitoring** for CPU power management
- **Historical data storage** for performance analysis
- **Integration with system monitoring tools**

#### Basic Usage

```python
from nlm import CPUMonitor

# Create CPU monitor
cpu_monitor = CPUMonitor(sampling_interval=0.1)

# Get current CPU
cpu_metrics = cpu_monitor.get_current_cpu()
print(f"Current CPU: {cpu_metrics.cpu_percent:.1f}%")
print(f"Load 1min: {cpu_metrics.load_1min:.2f}")
print(f"Load 5min: {cpu_metrics.load_5min:.2f}")
print(f"Load 15min: {cpu_metrics.load_15min:.2f}")

# Start monitoring
cpu_monitor.start_monitoring()

# Get average CPU
avg_metrics = cpu_monitor.get_average_cpu()
print(f"Average CPU: {avg_metrics.cpu_percent:.1f}%")

# Stop monitoring
cpu_monitor.stop_monitoring()
```

### SimulationProfiler

The `SimulationProfiler` class provides neural simulation profiling.

#### Key Features

- **Step-by-step profiling** for detailed simulation analysis
- **Memory and CPU tracking** for resource usage monitoring
- **Performance metrics collection** for simulation optimization
- **Historical data storage** for performance analysis
- **Simulation time tracking** for timing analysis

#### Basic Usage

```python
from nlm import SimulationProfiler

# Create simulation profiler
profiler = SimulationProfiler(sampling_interval=0.01)

# Profile a single step
step_metrics = profiler.profile_step(brain, brain.step, SimulationStep(1))

# Profile complete simulation
simulation_results = profiler.profile_simulation(brain, world, num_steps=100)

# Get average metrics
avg_metrics = profiler.get_average_metrics()
print(f"Average firing rate: {avg_metrics.average_firing_rate:.2f}")
print(f"Average excitation/inhibition ratio: {avg_metrics.excitation_inhibition_ratio:.2f}")
print(f"Average memory usage: {avg_metrics.memory_usage_mb:.2f} MB")

# Save simulation results
profiler.save_results("simulation_results.json")

# Load simulation results
profiler.load_results("simulation_results.json")
```

## Experiment Management

### AdvancedExperiment

The `AdvancedExperiment` class provides comprehensive experiment management.

#### Key Features

- **Parameter sweep experiments** for systematic parameter testing
- **Multi-environment experiments** for robust testing across conditions
- **Agent research experiments** for custom agent development
- **Results analysis** for data-driven insights
- **Plot generation** for visualization and reporting
- **Checkpoint management** for experiment recovery

#### Basic Usage

```python
from nlm import AdvancedExperiment

# Create advanced experiment
experiment = AdvancedExperiment("my_experiment")

# Run parameter sweep experiment
def brain_factory(params):
    config = Config()
    config.setInt("world.width", params.get('width', 100))
    config.setInt("world.height", params.get('height', 100))
    return Brain(config)

param_grid = {
    'width': [50, 100, 200],
    'height': [50, 100, 200]
}

results = experiment.run_parameter_sweep(param_grid, brain_factory)

# Save results
experiment.save_results("experiment_results.json")

# Analyze results
analysis = experiment.analyze_results()
print(f"Analysis: {analysis}")

# Plot results
experiment.plot_results("./plots")

# Run experiment
experiment_results = experiment.run()
```

#### Advanced Features

```python
# Multi-environment experiment
environments = [
    {'name': 'small', 'width': 50, 'height': 50, 'steps': 50},
    {'name': 'medium', 'width': 100, 'height': 100, 'steps': 100},
    {'name': 'large', 'width': 200, 'height': 200, 'steps': 200}
]

experiment.run_multi_environment(environments, brain_factory)

# Custom agent experiment
def agent_factory(episode):
    # Create custom agent logic
    return CustomAgent(episode)

experiment.run_custom_agent(agent_factory, num_episodes=1000)

# Load and save experiments
experiment.save_results("experiment.json")
experiment.load_results("experiment.json")
```

### ExperimentSuite

The `ExperimentSuite` class provides experiment suite management.

#### Key Features

- **Multiple experiment management** for coordinated testing
- **Batch processing** for efficient experiment execution
- **Result aggregation** for comprehensive analysis
- **Suite summary generation** for quick overview
- **Selective execution** for targeted testing

#### Basic Usage

```python
from nlm import ExperimentSuite, AdvancedExperiment

# Create experiment suite
suite = ExperimentSuite("research_suite")

# Add experiments
experiment1 = AdvancedExperiment("exp1")
experiment2 = AdvancedExperiment("exp2")
experiment3 = AdvancedExperiment("exp3")

suite.add_experiment(experiment1)
suite.add_experiment(experiment2)
suite.add_experiment(experiment3)

# Run all experiments
results = suite.run_all()

# Get suite summary
summary = suite.get_suite_summary()
print(f"Suite summary: {summary}")

# Run selected experiments
selected_results = suite.run_selected(["exp1", "exp3"])

# Save suite results
suite.save_suite_results("suite_results.json")

# Load suite results
suite.load_suite_results("suite_results.json")
```

## Custom Neuron Populations

The NLM Python API provides advanced tools for custom neuron population creation.

#### Basic Usage

```python
from nlm import NeuronPopulation

# Create neuron population for brain
neuron_pop = NeuronPopulation(brain)

# Create excitatory population
excitatory_neurons = neuron_pop.createExcitatoryPopulation(100, 0.1)

# Create recurrent network
recurrent_network = neuron_pop.createRecurrentNetwork(20, 10, 5)

# Create custom connectivity
neuron_pop.createCustomConnectivity(
    source_neurons=excitatory_neurons[:10],
    target_neurons=recurrent_network,
    weights=[0.5] * 10
)
```

#### Advanced Features

```python
# Create specialized neuron populations for different research tasks
# Vision processing neurons
vision_neurons = neuron_pop.createExcitatoryPopulation(200, 0.2)

# Motor control neurons
motor_neurons = neuron_pop.createExcitatoryPopulation(50, 0.3)

# Memory-related neurons
memory_neurons = neuron_pop.createExcitatoryPopulation(100, 0.15)

# Create complex connectivity patterns
neuron_pop.createRecurrentNetwork(50, 25, 10)
```

## Batch Processing

The NLM Python API provides batch processing utilities for efficient simulation.

#### Basic Usage

```python
from nlm import BatchProcessor

# Create batch processor
batch_processor = BatchProcessor()

# Process batch of percepts
percepts = [Vision(10, 10), Vision(10, 10), Vision(10, 10)]
actions = batch_processor.processBatch(percepts)

# Simulate batch
batch_results = batch_processor.simulateBatch(num_steps=100, batch_size=10)

# Save batch results
batch_processor.saveBatchResults(batch_results, "batch_results.json")
```

#### Advanced Features

```python
# Custom batch processing with parallel execution
# Implement custom processing logic
# Use for large-scale simulations
```

## Advanced Usage Examples

### Parameter Sweep Experiment

This example demonstrates running a parameter sweep experiment to find optimal neural network configurations.

```python
def brain_factory(params):
    config = Config()
    config.setInt("world.width", params.get('width', 100))
    config.setInt("world.height", params.get('height', 100))
    config.setInt("random.seed", params.get('seed', 42))
    config.setFloat("energy.decay_rate", params.get('decay_rate', 0.01))
    return Brain(config)

# Create experiment
experiment = AdvancedExperiment("parameter_sweep")

# Define parameter grid
param_grid = {
    'width': [50, 100, 200],
    'height': [50, 100, 200],
    'seed': [42, 123, 456],
    'decay_rate': [0.005, 0.01, 0.02]
}

# Run parameter sweep
results = experiment.run_parameter_sweep(param_grid, brain_factory)

# Analyze results
for result in results:
    if result['success']:
        final_stats = result['result']['statistics']
        efficiency = final_stats['efficiency']
        print(f"Parameters: {result['parameters']}, Efficiency: {efficiency:.3f}")
```

### Multi-Environment Testing

This example demonstrates testing neural networks across different environment conditions.

```python
def brain_factory(env_config):
    config = Config()
    config.setInt("world.width", env_config['width'])
    config.setInt("world.height", env_config['height'])
    config.setFloat("energy.decay_rate", env_config.get('decay_rate', 0.01))
    return Brain(config)

# Create environments
environments = [
    {'name': 'simple', 'width': 50, 'height': 50, 'decay_rate': 0.01, 'steps': 50},
    {'name': 'moderate', 'width': 100, 'height': 100, 'decay_rate': 0.05, 'steps': 100},
    {'name': 'complex', 'width': 200, 'height': 200, 'decay_rate': 0.1, 'steps': 200}
]

# Create experiment
experiment = AdvancedExperiment("multi_environment")

# Run multi-environment experiment
results = experiment.run_multi_environment(environments, brain_factory)

# Analyze results
for env_result in results:
    if env_result['success']:
        stats = env_result['statistics']
        print(f"Environment: {env_result['environment_config']['name']}")
        print(f"  Efficiency: {stats['efficiency']:.3f}")
        print(f"  Total actions: {stats['total_actions']}")
```

### Agent Research

This example demonstrates custom agent development and testing.

```python
from nlm import CustomAgent

# Create custom agent
agent = CustomAgent()

# Configure agent
agent.set_behavior_policy('exploration')
agent.set_memory_capacity(1000)
agent.set_learning_rate(0.01)

# Create experiment
experiment = AdvancedExperiment("agent_research")

# Run agent research experiment
experiment.run_custom_agent(lambda episode: agent, num_episodes=1000)

# Analyze results
results = experiment.results['custom_agent']
for episode_result in results:
    if episode_result['success']:
        episode_data = episode_result['result']
        actions = episode_data['actions']
        efficiency = sum(1 for a in actions if a.getType() != ActionType.Wait) / len(actions)
        print(f"Episode {episode_result['episode']}: Efficiency = {efficiency:.3f}")
```

### Performance Benchmarking

This example demonstrates performance benchmarking of NLM simulations.

```python
from nlm import PerformanceProfiler

# Create profiler
profiler = PerformanceProfiler()

# Benchmark simulation function
def benchmark_simulation():
    config = Config()
    config.setInt("world.width", 100)
    config.setInt("world.height", 100)
    
    brain = Brain(config)
    world = SimpleWorld()
    world.configure(100, 100, 10, 10)
    
    # Run simulation
    for step in range(100):
        world.update(0.01)
    
    return brain

# Run benchmark
result = profiler.benchmark_function(benchmark_simulation)

# Create simulation benchmark
result = profiler.benchmark_simulation(brain, world, num_steps=100, num_runs=10)

# Generate report
report = profiler.generate_report()
print(report)
```

## Configuration Files

The NLM Python API supports various configuration file formats:

### JSON Configuration

```json
{
    "name": "experiment",
    "description": "Advanced NLM experiment",
    "experiment_type": "parameter_sweep",
    "parameters": {
        "width": [50, 100, 200],
        "height": [50, 100, 200],
        "seed": 42,
        "decay_rate": 0.01
    }
}
```

### YAML Configuration

```yaml
name: experiment
description: Advanced NLM experiment
experiment_type: parameter_sweep
parameters:
  width:
    - 50
    - 100
    - 200
  height:
    - 50
    - 100
    - 200
  seed: 42
  decay_rate: 0.01
```

## Troubleshooting

### Common Issues

1. **Import Errors**: Ensure all required modules are installed (`pip install nlm[advanced]`)

2. **Memory Issues**: Monitor memory usage with `MemoryMonitor` and optimize memory consumption

3. **Performance Issues**: Profile simulations with `PerformanceProfiler` to identify bottlenecks

4. **Configuration Errors**: Validate configuration using `ValidatedConfig` before use

5. **Experiment Failures**: Use checkpoint functionality to resume experiments from previous state

### Debugging Tips

- Use `print()` statements to monitor experiment progress
- Enable debug logging in brain and world components
- Use memory monitors to identify memory leaks
- Profile simulations to find performance bottlenecks
- Check configuration validation to ensure all required parameters are set

### Getting Help

- Consult the NLM documentation for detailed API references
- Check the examples directory for working code samples
- Join the NLM community for support and discussions
- Report issues on the NLM GitHub repository

## Conclusion

The enhanced NLM Python API provides a comprehensive framework for neural simulation research and experimentation. It offers:

- **Pythonic interfaces** for intuitive usage
- **Advanced configuration systems** for flexible experiment setup
- **Performance monitoring** for optimization and analysis
- **Experiment management** for systematic research
- **Custom neuron population** creation for specialized research
- **Batch processing** utilities for efficient simulation

This guide provides the foundation for leveraging these advanced features to conduct sophisticated neural simulation research and development.
