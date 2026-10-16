# NLM Advanced Usage Guide

# NLM Advanced Usage Guide

## Overview

This guide covers advanced usage patterns and features of the NLM (Neural Learning Machine) project, including Python bindings, runtime configuration, performance tuning, and logging.

## Integration with Python Bindings

### Prerequisites

1. **Python Installation**: Python 3.8 or higher
2. **Required Packages**: `pybind11`, `scikit-build-core`, `numpy`
3. **C++ Build**: NLM must be built with Python bindings enabled

### Installing Python Bindings

#### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core numpy

# Build and install Python module
pip install .

# Or install in development mode (recommended for development)
pip install -e .
```

#### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Basic Python API Usage

#### Creating and Initializing a Brain

```python
import pynlm
import time

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally modify configuration
# config.set("brain.neuron_count", 2000)
# config.set("plasticity.stdp.enable", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### Running Advanced Simulations

```python
import pynlm
import numpy as np

def run_advanced_simulation(config_path=None, steps=5000):
    """Run an advanced simulation with multiple components."""
    
    # Create configuration from file or default
    if config_path:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_path)
    else:
        config = pynlm.createDefaultConfig()
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world interface
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_firing_rate = brain.getAverageFiringRate()
    
    # Run simulation loop
    for step in range(steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress periodically
        if step % 500 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount() - initial_spikes}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_advanced_simulation(steps=5000)
```

### Key Python Classes

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### Python Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more
```

## Runtime Configuration Modification

### Overview

The NLM runtime configuration system allows dynamic modification of simulation parameters during execution. This is particularly useful for:

- Adaptive experimentation
- Real-time debugging
- Performance tuning during runtime
- A/B testing of different parameter sets

### Runtime Configuration Modes

NLM supports three runtime configuration modes:

1. **Runtime Config Test**: Automated testing of configuration changes
2. **Runtime Config Demo**: Interactive demonstration of configuration effects
3. **Runtime Config CLI**: Command-line interface for runtime configuration

### Accessing Runtime Configuration

```python
import pynlm

# Create brain and get its configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
config = brain.getConfig()

# Modify configuration at runtime
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("neuromod.curiosity.weight", 0.2)

# Get current values
current_ltp_rate = config.getOrFloat("plasticity.stdp.learning_rate", 0.001)
curiosity_enabled = config.getOrBool("neuromod.curiosity.enable", True)
```

### Using Runtime Configuration with Command Line

```bash
# Enable runtime configuration CLI mode
./nlm --mode runtime_config_cli

# Common runtime commands:
# "set plasticity.stdp.learning_rate 0.005"
# "get plasticity.stdp.learning_rate"
# "enable plasticity.hebbian"
# "disable novelty.detection"
# "show config"
# "reset config"
```

### Runtime Configuration API

```python
# Configuration modification methods
config.set(key, value)                    # Set a configuration value
config.getOr(key, default)               # Get value or return default
config.has(key)                          # Check if key exists
config.clear()                           # Clear all configuration
config.saveToFile(filename)             # Save configuration to file

# Common configuration keys
brain.neuron_count
brain.region_count
plasticity.stdp.enable
plasticity.stdp.learning_rate
neuromod.dopamine.scale
neuromod.curiosity.enable
rl.curiosity_weight
environment.width
environment.height
```

## Performance Tuning Guide

### Overview

Performance tuning is crucial for efficient NLM simulations, especially for large-scale experiments. This guide covers key optimization strategies.

### Key Performance Parameters

#### Neural Network Size
```ini
# For small simulations (< 1000 neurons)
neuron_count = 500
region_count = 2

# For medium simulations (1000-5000 neurons)
neuron_count = 2000
region_count = 5

# For large simulations (> 5000 neurons)
neuron_count = 10000
region_count = 10-20
```

#### Plasticity Settings
```ini
# Reduce plasticity overhead for performance
plasticity.stdp.enable = false  # If STDP not critical
plasticity.hebbian.enable = false  # If Hebbian not needed
plasticity.structural.enable = false  # If structural plasticity not required
```

#### Simulation Timestep
```ini
# Trade-off between accuracy and performance
simulation_timestep = 0.001  # High accuracy, slower
simulation_timestep = 0.01   # Medium accuracy, better performance
simulation_timestep = 0.1    # Low accuracy, fastest
s```

#### Memory Management
```ini
# Optimize memory usage
log.level = WARN      # Reduce log data volume
log.to_file = false   # Disable file logging
log.max_size = 10     # Small log files
```

### Performance Monitoring

#### Built-in Statistics
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor simulation performance
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

#### Benchmarking

```bash
# Run performance benchmark
./nlm --mode benchmark --neurons 1000 --steps 1000

# Scaling benchmark
./nlm --mode benchmark --benchmark-scale --scaling-levels 100,500,1000,2000
```

### Optimization Techniques

1. **Vectorization**: Use SIMD instructions for neuron computations
2. **Memory Pools**: Leverage memory pools for efficient object allocation
3. **Event-Driven Updates**: Only process neurons that need updates
4. **Lazy Plasticity**: Apply plasticity less frequently
5. **Spatio-Temporal Compression**: Use sparse representations for inactive neurons

### Recommended Configurations for Different Scales

#### Small Scale (Exploration/Debugging)
```ini
neuron_count = 100
region_count = 1
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
neuromod.curiosity.enable = true
log.level = DEBUG
simulation.max_steps = 1000
```

#### Medium Scale (Standard Research)
```ini
neuron_count = 1000
region_count = 3
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
plasticity.structural.enable = true
neuromod.dopamine.scale = 0.5
neuromod.curiosity.enable = true
log.level = INFO
simulation.max_steps = 10000
```

#### Large Scale (Production/High-Performance)
```ini
neuron_count = 5000
region_count = 10
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
log.level = WARN
log.to_file = true
simulation.max_steps = 50000
```

## Logging and Statistics Guide

### Overview

NLM provides comprehensive logging and statistics for monitoring simulation progress and analyzing results. This guide covers logging configuration and statistics analysis.

### Logging System

#### Log Levels

| Level | Description | Use Case |
|-------|-------------|----------|
| `TRACE` | Detailed debug information | Deep debugging |
| `DEBUG` | Normal debugging information | Development |
| `INFO` | Standard operational information | Production |
| `WARN` | Warning messages | Monitoring |
| `ERROR` | Error conditions | Error tracking |
| `FATAL` | Critical failures | System crashes |

#### Logging Configuration
```ini
log.level = INFO
log.to_file = true
log.filename = nlm_simulation.log
log.max_size = 100
log.max_files = 5
```

#### Log Messages

NLM generates logs for:
- **Initialization**: Brain setup, configuration loading
- **Simulation Steps**: Progress tracking, statistics
- **Events**: Plasticity changes, spikes, learning events
- **Errors**: Failures, warnings, edge cases
- **Metrics**: Performance statistics, system health

### Statistics Collection

#### Built-in Statistics
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Basic statistics
brain.getTotalNeuronCount()           # Total number of neurons
brain.getTotalSynapseCount()          # Total number of synapses
brain.getFiringNeuronCount()         # Currently firing neurons
brain.getAverageFiringRate()          # Average firing rate (Hz)
brain.getExcitationInhibitionRatio() # E/I balance ratio
brain.getTotalSpikeCount()           # Total spike count
brain.getActiveNeuronCount()         # Count of active neurons

# Development statistics
brain.getDevelopmentalStage()        # Current developmental stage

# Memory statistics (if available)
brain.getMemoryUsage()               # Memory consumption
brain.getPeakMemoryUsage()           # Peak memory usage
```

#### Custom Statistics

```python
import pynlm
import time
import statistics

def collect_statistics(brain, duration_steps=1000):
    """Collect comprehensive statistics during simulation."""
    
    stats = {
        'step': [],
        'firing_rate': [],
        'e_i_ratio': [],
        'total_spikes': [],
        'neurons_active': [],
        'timestamps': []
    }
    
    start_time = time.time()
    
    for step in range(duration_steps):
        brain.step(step)
        
        # Record statistics
        stats['step'].append(step)
        stats['firing_rate'].append(brain.getAverageFiringRate())
        stats['e_i_ratio'].append(brain.getExcitationInhibitionRatio())
        stats['total_spikes'].append(brain.getTotalSpikeCount())
        stats['neurons_active'].append(brain.getFiringNeuronCount())
        stats['timestamps'].append(time.time() - start_time)
        
        # Log periodically
        if step % 100 == 0:
            print(f"Step {step}: {brain.getAverageFiringRate():.2f} Hz")
    
    # Calculate statistics
    stats['avg_firing_rate'] = statistics.mean(stats['firing_rate'])
    stats['std_firing_rate'] = statistics.stdev(stats['firing_rate']) if len(stats['firing_rate']) > 1 else 0.0
    stats['avg_e_i_ratio'] = statistics.mean(stats['e_i_ratio'])
    stats['max_firing_rate'] = max(stats['firing_rate'])
    stats['total_simulation_time'] = time.time() - start_time
    
    return stats

# Run statistics collection
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
stats = collect_statistics(brain, duration_steps=1000)

# Analyze results
print(f"Average firing rate: {stats['avg_firing_rate']:.2f} ± {stats['std_firing_rate']:.2f} Hz")
print(f"E/I ratio: {stats['avg_e_i_ratio']:.3f}")
print(f"Max firing rate: {stats['max_firing_rate']:.2f} Hz")
print(f"Total simulation time: {stats['total_simulation_time']:.2f} seconds")
```

### Statistics Analysis

#### Key Metrics to Monitor

1. **Firing Rate**: Average and standard deviation indicate network activity levels
2. **E/I Ratio**: Balance between excitation and inhibition
3. **Spike Statistics**: Total spike count and distribution
4. **Development Metrics**: Changes in developmental stage over time
5. **Performance Metrics**: Simulation speed, memory usage

#### Visualization of Statistics

```python
import matplotlib.pyplot as plt
import pynlm

# Run simulation and collect statistics
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

steps = 1000
firing_rates = []
spike_counts = []

for step in range(steps):
    brain.step(step)
    firing_rates.append(brain.getAverageFiringRate())
    spike_counts.append(brain.getTotalSpikeCount())

# Plot firing rate over time
plt.figure(figsize=(12, 8))

plt.subplot(2, 1, 1)
plt.plot(range(steps), firing_rates)
plt.xlabel('Simulation Step')
plt.ylabel('Firing Rate (Hz)')
plt.title('Firing Rate Over Time')
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(range(steps), spike_counts)
plt.xlabel('Simulation Step')
plt.ylabel('Total Spike Count')
plt.title('Spike Count Over Time')
plt.grid(True)

plt.tight_layout()
plt.savefig('simulation_statistics.png')
plt.show()
```

### Logging Best Practices

1. **Level Selection**: Use INFO level for production, DEBUG for development
2. **File Logging**: Enable file logging for long-running simulations
3. **Log Rotation**: Configure appropriate file sizes and rotation counts
4. **Structured Logging**: Use consistent log message formats
5. **Performance Impact**: Monitor log volume to avoid performance degradation

### Common Issues and Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| High memory usage | Large neuron count, verbose logging | Reduce neurons, increase log level |
| Slow simulation | Complex plasticity rules | Disable unnecessary plasticity |
| Incomplete statistics | Statistics not calculated periodically | Implement custom statistics collection |
| Visualization lag | High FPS setting with large networks | Reduce FPS or decrease network size |