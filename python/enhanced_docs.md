# NLM Python API - Enhanced Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides a comprehensive interface for brain-inspired spiking neural network simulations. This documentation covers the enhanced features including high-level convenience functions, performance monitoring, error handling, and research utilities.

## Installation

```bash
pip install pynlm
```

For development from source:
```bash
pip install -e .
```

## Quick Start

### Basic Usage

```python
import pynlm

# Create a simple brain
config = pynlm.create_research_config(1000, "balanced")
brain = pynlm.create_brain(config)
brain.initialize()

# Run a few steps
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
```

### Complete Agent Simulation

```python
import pynlm

# Create a complete research agent
brain, world, agent = pynlm.create_research_agent("balanced", 1000)

# Run simulation with detailed logging
results = pynlm.run_research_simulation(brain, world, agent, 500)

# Analyze results
for i, step_data in enumerate(results):
    if i % 50 == 0:  # Print every 50 steps
        print(f"Step {step_data['step']}: "
              f"Firing rate {step_data['firing_rate']:.2f}, "
              f"Neurons firing {step_data['firing_neurons']}, "
              f"Spikes {step_data['total_spikes']}")
```

### Context Manager Usage

```python
import pynlm

# Use context manager for automatic resource management
with pynlm.AgentContext(pynlm.create_research_config()) as agent_ctx:
    results = agent_ctx.run_simulation(1000)
    
    # Access brain components
    brain = agent_ctx.get_brain()
    print(f"Average firing rate: {agent_ctx.get_firing_rate():.2f}")
    print(f"Total spikes: {agent_ctx.get_total_spikes()}")
```

## Core API Reference

### Configuration System

#### Basic Configuration
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.05)

# Load from file
config.loadFromFile("my_config.json")

# Save to file
config.saveToFile("my_config.json")

# Check if key exists
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
```

#### Research Configuration Presets
```python
# Create predefined research configurations
config_balanced = pynlm.create_research_config(1000, "balanced")
config_excitatory = pynlm.create_research_config(500, "excitatory")
config_inhibitory = pynlm.create_research_config(1500, "inhibitory")
```

### Brain Operations

#### Brain Creation and Initialization
```python
# Create and initialize brain
brain = pynlm.create_brain()
brain.initialize()

# Access brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")
```

#### Simulation Control
```python
# Basic simulation
for step in range(1000):
    brain.step(step)

# Simulation with timestamp
for step in range(1000):
    brain.step(step, time=step * 0.1)

# Inject sensory input
from pynlm import Vision
sensory = Vision(64, 64, 3)
sensory.setData([0.5] * 64 * 64 * 3)
brain.receiveSensoryInput(sensory)

# Reset brain
brain.reset()
```

#### Memory and State Management
```python
# Save brain state
brain.save("checkpoint.bin")

# Load brain state
brain.load("checkpoint.bin")

# Get memory usage
memory_stats = pynlm.analyze_brain_memory_usage(brain)
print(f"Memory stats: {memory_stats}")
```

### Agent and World System

#### Agent Setup
```python
# Create brain, world, and agent together
brain, world, agent = pynlm.create_research_agent()

# Configure world
world.configure(30, 30, 8, 8)
world.reset()
world.setAgentStart(15.0, 15.0)

# Initialize agent with world
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

#### Sensory and Motor Processing
```python
# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())

# Get agent state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent energy: {body.energy}")
```

### Advanced Research Features

#### Developmental Brain
```python
# Create a brain with developmental capabilities
brain, world, agent = pynlm.create_developmental_brain(500, 2000)

# Enable developmental systems
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation with development
for step in range(2000):
    agent.updateDevelopment(0.1)
    brain.step(step)
```

#### Curiosity-Driven Agent
```python
# Create an agent focused on exploration
brain, world, agent = pynlm.create_curiosity_agent(800)

# Monitor curiosity levels
for step in range(500):
    # ... simulation code ...
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
```

#### Monte Carlo Simulation
```python
# Run multiple stochastic simulations
brain, world, agent = pynlm.create_research_agent()

# Run Monte Carlo simulation for statistical analysis
firing_rates = []
for run in range(20):
    rate = pynlm.run_monte_carlo_simulation(brain, world, agent, 200)
    firing_rates.append(rate)

print(f"Average firing rate: {sum(firing_rates)/len(firing_rates):.2f}")
print(f"Standard deviation: {((sum(x*x for x in firing_rates)/len(firing_rates)) - 
                             (sum(firing_rates)/len(firing_rates))**2)**0.5:.2f}")
```

### Performance Monitoring

#### Basic Performance Tracking
```python
# Run simulation with performance monitoring
brain, world, agent = pynlm.create_research_agent()
profile = pynlm.benchmark_simulation(brain, world, agent, 1000)

print(f"Profile name: {profile.name}")
print(f"Steps completed: {profile.metrics.steps_completed}")
print(f"Total simulation time: {profile.metrics.total_simulation_time:.2f}s")
print(f"Average firing rate: {profile.metrics.average_firing_rate:.2f}")
print(f"Average step time: {profile.average_step_time:.4f}s")
```

#### Performance Analysis
```python
# Get step times for detailed analysis
step_times = profile.get_step_times()
print(f"Step times: {step_times}")

# Detect performance bottlenecks
bottlenecks = pynlm.detect_performance_bottlenecks(profile)
if bottlenecks:
    print(f"Performance bottlenecks: {bottlenecks}")
```

#### Parameter Sweep
```python
# Run parameter sweep for systematic exploration
configurations = [
    (500, "balanced"),
    (1000, "balanced"),
    (1500, "excitatory"),
    (1500, "inhibitory"),
]

profiles = pynlm.benchmark_parameter_space(configurations, 500)

# Analyze results
analysis = pynlm.analyze_performance_comparison(profiles)
print(f"Analysis: {analysis}")
```

### Error Handling

The enhanced Python bindings include comprehensive error handling with custom exception classes:

```python
import pynlm

try:
    # This will raise an error if parameters are invalid
    config = pynlm.create_research_config(0, "balanced")  # neuron_count must be positive
except pynlm.NLMError as e:
    print(f"Error: {e}")

try:
    # Invalid preset name
    brain, world, agent = pynlm.create_research_agent("invalid_preset", 1000)
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
```

### Context Managers

Context managers ensure proper resource cleanup and simplify simulation management:

```python
# Brain context manager
with pynlm.BrainContext(config) as brain_ctx:
    brain = brain_ctx.get_brain()
    for step in range(100):
        brain_ctx.step(step)

# Agent context manager  
with pynlm.AgentContext() as agent_ctx:
    results = agent_ctx.run_simulation(500)
    print(f"Firing rate: {agent_ctx.get_firing_rate():.2f}")
    print(f"Total spikes: {agent_ctx.get_total_spikes()}")
```

## Research Workflows

### 1. Development Study
```python
def study_development():
    """Study how brain development affects learning"""
    results = {}
    
    # Create developmental brain
    brain, world, agent = pynlm.create_developmental_brain(300, 1500)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Track development over time
    development_stages = []
    firing_rates = []
    
    for step in range(2000):
        agent.updateDevelopment(0.1)
        brain.step(step)
        
        stage = brain.getDevelopmentalStage()
        rate = brain.getAverageFiringRate()
        
        development_stages.append(stage)
        firing_rates.append(rate)
        
        # Save metrics periodically
        if step % 200 == 0:
            print(f"Step {step}: {stage}, firing rate {rate:.2f}")
    
    results["development_stages"] = development_stages
    results["firing_rates"] = firing_rates
    return results
```

### 2. Parameter Optimization
```python
def optimize_parameters():
    """Find optimal parameter combinations"""
    configurations = [
        (100, "balanced"),
        (500, "balanced"), 
        (1000, "balanced"),
        (2000, "balanced"),
        (1000, "excitatory"),
        (1000, "inhibitory"),
    ]
    
    results = pynlm.benchmark_parameter_space(configurations, 500)
    
    # Find best configuration
    best_name = None
    best_metric = float('inf')
    
    for name, profile in results.items():
        # Use average firing rate as optimization metric
        metric = abs(profile.metrics.average_firing_rate - 50.0)  # Target firing rate
        if metric < best_metric:
            best_metric = metric
            best_name = name
    
    print(f"Best configuration: {best_name}")
    return results
```

### 3. Comparative Study
```python
def compare_simulation_types():
    """Compare different simulation approaches"""
    
    def balanced_workflow():
        brain, world, agent = pynlm.create_research_agent("balanced", 1000)
        return pynlm.run_monte_carlo_simulation(brain, world, agent, 200)
    
    def excitatory_workflow():
        brain, world, agent = pynlm.create_research_agent("excitatory", 1000)
        return pynlm.run_monte_carlo_simulation(brain, world, agent, 200)
    
    def inhibitory_workflow():
        brain, world, agent = pynlm.create_research_agent("inhibitory", 1000)
        return pynlm.run_monte_carlo_simulation(brain, world, agent, 200)
    
    # Profile different workflows
    workflows = [
        ("Balanced", balanced_workflow),
        ("Excitatory", excitatory_workflow),
        ("Inhibitory", inhibitory_workflow),
    ]
    
    profiles = pynlm.profile_research_workflows(
        [wf[1] for wf in workflows],
        200,
        [wf[0] for wf in workflows]
    )
    
    return profiles
```

## API Summary

### High-Level Convenience Functions

| Function | Description | Returns |
|----------|-------------|---------|
| `create_research_config()` | Create research configuration with preset | Config |
| `create_brain()` | Create and initialize brain | Brain |
| `create_research_agent()` | Create complete agent setup | tuple(Brain, World, AgentBrain) |
| `run_research_simulation()` | Run simulation with logging | list(dict) |
| `run_parameter_sweep()` | Run multiple simulations | list(list(dict)) |
| `create_developmental_brain()` | Create brain with development | tuple(Brain, World, AgentBrain) |
| `create_curiosity_agent()` | Create curiosity-driven agent | tuple(Brain, World, AgentBrain) |

### Performance Tools

| Function | Description | Returns |
|----------|-------------|---------|
| `benchmark_simulation()` | Run with performance profiling | PerformanceProfile |
| `benchmark_parameter_space()` | Compare multiple configurations | dict(PerformanceProfile) |
| `analyze_performance_comparison()` | Analyze benchmark results | dict |
| `profile_research_workflows()` | Profile multiple workflows | list(PerformanceProfile) |
| `monitor_memory_usage()` | Monitor memory usage | dict |
| `detect_performance_bottlenecks()` | Detect performance issues | list(str) |

### Context Managers

| Class | Purpose | Usage |
|-------|---------|-------|
| `BrainContext` | Temporary brain simulations | `with BrainContext(config):` |
| `AgentContext` | Complete agent simulations | `with AgentContext(config):` |

### Error Types

| Exception | Description |
|-----------|-------------|
| `NLMError` | Base NLM error |
| `ConfigError` | Configuration-related error |
| `BrainError` | Brain simulation error |
| `WorldError` | World simulation error |
| `AgentError` | Agent-related error |

## Examples Directory

The examples directory contains additional examples:

- `basic_usage.py` - Basic simulation setup
- `agent_simulation.py` - Complete agent workflow
- `development_study.py` - Developmental brain research
- `parameter_optimization.py` - Systematic parameter search
- `performance_benchmarking.py` - Performance analysis

These examples demonstrate the enhanced features of the NLM Python bindings for research workflows.

## Migration Guide

### From Basic to Enhanced API

The enhanced API maintains backward compatibility while adding new features:

**Old pattern:**
```python
import pynlm
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
```

**New enhanced pattern:**
```python
import pynlm
config = pynlm.create_research_config(1000, "balanced")
brain = pynlm.create_brain(config)
```

The new pattern provides better defaults, error handling, and validation while maintaining the same core functionality.

## Performance Tips

1. **Use context managers** for proper resource cleanup
2. **Batch operations** when running multiple simulations
3. **Monitor performance** to identify bottlenecks
4. **Use parameter sweeps** for systematic exploration
5. **Leverage development features** for studying brain maturation

## Troubleshooting

### Common Issues

**Issue:** "Your brain isn't doing anything"
**Solution:** Ensure `brain.initialize()` is called before `brain.step()`

**Issue:** "ImportError: No module named 'pynlm'"
**Solution:** Install the package with `pip install pynlm` or `pip install -e .`

**Issue:** Segmentation faults
**Solution:** Check that all required files are present and use `brain.reset()` between simulations

**Issue:** Poor performance
**Solution:** Use `benchmark_simulation()` to identify bottlenecks, or try `create_research_config()` for optimized settings

### Getting Help

- Check the examples in the examples directory
- Use `pynlm.benchmark_simulation()` for performance profiling
- Use `pynlm.detect_performance_bottlenecks()` to identify issues
- Refer to the C++ documentation for technical details