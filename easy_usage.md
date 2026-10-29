# NLM Python Module Documentation
# Comprehensive API Reference

## Overview

The `pynlm` module provides Python bindings to the NLM (Neural Learning Machine) framework, allowing users to create, configure, and run artificial neural brain simulations from Python.

This documentation provides complete reference for the public API, with examples for both beginners and advanced users.

## Module Structure

The NLM Python module is organized around three main concepts:

1. **Brain** - The neural substrate that thinks and learns
2. **World** - The environment the brain interacts with
3. **Agent** - The interface that connects brain to world

### Key Components

| Python Module | C++ Equivalent | Purpose |
|---------------|----------------|---------|
| `pynlm` | `nlm` | Main module with factory functions |
| `pynlm.Brain` | `Brain` | Neural computation engine |
| `pynlm.World` | `SimpleWorld` | Simulation environment |
| `pynlm.AgentBrain` | `AgentBrain` | Perception-action system |

## Installation

The Python bindings are built as part of the C++ build system using `pybind11`. The `pynlm` module will be installed as a Python package alongside the C++ libraries.

```bash
# After building with CMake:
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Python module should be available in the build directory
export PYTHONPATH=$PYTHONPATH:build/pynlm
```

## Module-Level Functions

### `pynlm.createDefaultConfig()`

Creates a default configuration object for initializing NLM brains.

**Returns:**
- `Config`: A configuration object with sensible defaults for Phase 2

**Example:**
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration if needed
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
```

**Configuration Options:**
- `neuron_count` (int): Number of neurons in the brain
- `region_count` (int): Number of brain regions
- `connection_probability` (float): Probability of synaptic connections
- `simulation_timestep` (float): Time step duration in seconds
- `random_seed` (int): Random seed for reproducible experiments
- `plasticity_learning_rate` (float): Learning rate for plasticity
- `stdp_ltp_weight` (float): STDP LTP weight parameter
- `stdp_ltd_weight` (float): STDP LTD weight parameter

### `pynlm.createBrain(config=None)`

Creates a new NLM brain with the specified configuration.

**Parameters:**
- `config` (Config, optional): Configuration object. If None, creates default config.

**Returns:**
- `Brain`: A new NLM brain instance

**Example:**
```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain()

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
brain = pynlm.createBrain(config)
```

### `pynlm.createSimpleWorld()`

Creates a simple world environment for brain-agent interaction.

**Returns:**
- `World`: A world environment instance

**Example:**
```python
import pynlm

# Create default world
world = pynlm.createSimpleWorld()

# Configure world with specific dimensions
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Reset world for new simulation
world.reset()
```

**World Configuration Options:**
- `width` (int): World width in cells
- `height` (int): World height in cells  
- `visionWidth` (int): Agent's visual field width
- `visionHeight` (int): Agent's visual field height

### `pynlm.createAgentBrain(brain)`

Creates an agent brain that interfaces between a neural brain and a world environment.

**Parameters:**
- `brain` (Brain): The neural brain to control

**Returns:**
- `AgentBrain`: An agent brain instance

**Example:**
```python
import pynlm

# Create neural brain and agent
brain = pynlm.createBrain()
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
world = pynlm.createSimpleWorld()
agent.initialize(world)
```

## Configuration Class

The `Config` class provides type-safe configuration management for NLM brains.

### Methods

#### `config.set(key, value, source=ConfigSource.Runtime)`

Sets a configuration value.

**Parameters:**
- `key` (str): Configuration key
- `value` (various): Configuration value
- `source` (ConfigSource): Configuration source

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("random_seed", 42)
```

#### `config.getOr(key, default_value)`

Gets a configuration value with default fallback.

**Parameters:**
- `key` (str): Configuration key
- `default_value`: Default value if key not found

**Returns:**
- Value of configuration or default

**Example:**
```python
neuron_count = config.getOr("neuron_count", 100)
```

#### `config.has(key)`

Checks if configuration key exists.

**Parameters:**
- `key` (str): Configuration key to check

**Returns:**
- bool: True if key exists

#### `config.summary()`

Gets human-readable configuration summary.

**Returns:**
- str: Configuration summary

## Brain Class

The `Brain` class represents the artificial neural substrate that performs computations.

### Core Methods

#### `brain.initialize()`

Initializes the brain with current configuration.

**Example:**
```python
brain = pynlm.createBrain()
brain.initialize()  # Must be called before use
```

#### `brain.step(step_number)`

Executes one simulation step.

**Parameters:**
- `step_number` (int): Current step number

**Example:**
```python
for i in range(1000):
    brain.step(i)  # One step of neural computation
```

#### `brain.reset()`

Resets brain to initial state.

**Example:**
```python
brain.reset()  # Resets to initial configuration
brain.initialize()  # Re-initializes
```

### State Access Methods

#### `brain.getTotalNeuronCount()`

Gets total number of neurons in the brain.

**Returns:**
- int: Total neuron count

#### `brain.getFiringNeuronCount()`

Gets number of currently firing neurons.

**Returns:**
- int: Number of firing neurons

#### `brain.getTotalSpikeCount()`

Gets total number of spikes generated.

**Returns:**
- int: Total spike count

#### `brain.getAverageFiringRate()`

Gets average firing rate across all neurons.

**Returns:**
- float: Average firing rate in Hz

#### `brain.getDevelopmentalStage()`

Gets current developmental stage.

**Returns:**
- str: Developmental stage ("Initial", "CriticalPeriod", etc.)

### Advanced Features

#### `brain.enableRewardModulation(enabled)`

Enables or disables reward-modulated plasticity.

**Parameters:**
- `enabled` (bool): Whether to enable reward modulation

#### `brain.enableStructuralPlasticity(enabled)`

Enables or disables structural plasticity (synaptogenesis/pruning).

**Parameters:**
- `enabled` (bool): Whether to enable structural plasticity

#### `brain.enableDevelopment(enabled)`

Enables or disables developmental processes.

**Parameters:**
- `enabled` (bool): Whether to enable development

#### `brain.enableCuriosity(enabled)`

Enables or disables curiosity-driven exploration.

**Parameters:**
- `enabled` (bool): Whether to enable curiosity

## World Class

The `World` class represents the simulation environment.

### Core Methods

#### `world.update(dt)`

Updates the world state by time dt.

**Parameters:**
- `dt` (float): Time step duration in seconds

#### `world.getSensoryPercept()`

Gets the current sensory perception from the agent's perspective.

**Returns:**
- `SensoryPercept`: Current sensory input

#### `world.applyMotorCommand(action, timestamp)`

Applies a motor command to the world.

**Parameters:**
- `action` (Action): Motor action to apply
- `timestamp` (float): Simulation time

#### `world.getSimulationTime()`

Gets current simulation time.

**Returns:**
- float: Current simulation time

## AgentBrain Class

The `AgentBrain` class provides the perception-action interface between brain and world.

### Core Methods

#### `agent.initialize(world)`

Initializes agent with world environment.

**Parameters:**
- `world` (World): World environment to interact with

#### `agent.processSensoryInput(percept)`

Processes sensory input and updates brain's internal state.

**Parameters:**
- `percept` (SensoryPercept): Sensory input to process

#### `agent.decodeMotorCommand()`

Decodes current brain state into motor action.

**Returns:**
- `Action`: Selected motor action

#### `agent.applyRewardModulation(reward, predicted)`

Applies reward-based learning modulation.

**Parameters:**
- `reward` (float): Received reward
- `predicted` (float): Predicted reward

## Advanced Usage Patterns

### Complete Simulation Loop

This pattern shows the full brain-world-agent interaction cycle:

```python
def run_simulation(brain, world, agent, num_steps):
    # Initialize simulation
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Main simulation loop
    for step in range(num_steps):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input through agent
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Spikes={brain.getTotalSpikeCount()}")
    
    return brain

# Run complete simulation
brain = run_simulation(brain, world, agent, 1000)
```

### Learning Experiment

This pattern demonstrates setting up a learning experiment:

```python
def run_learning_experiment(brain, world, agent, num_steps):
    # Configure for learning
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("plasticity_learning_rate", 0.01)
    
    # Re-initialize with learning config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run learning simulation
    for step in range(num_steps):
        # Reset world each episode for reproducibility
        world.reset()
        
        # Run episode
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward for learning
            reward = world.getCurrentReward()
            predicted = agent.getPredictedReward()
            agent.applyRewardModulation(reward, predicted)
        
        # Log learning progress
        if step % 500 == 0:
            print(f"Episode {step}: "
                  f"Total Reward: {agent.getTotalReward():.2f}")
    
    return brain, agent.getLifetimeResults()
```

### Performance Monitoring

Track brain performance over time:

```python
class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.metrics = {
            'spike_count': [],
            'firing_rate': [],
            'neuron_count': [],
            'learning_efficiency': []
        }
    
    def record_step(self, step):
        self.metrics['spike_count'].append(self.brain.getTotalSpikeCount())
        self.metrics['firing_rate'].append(self.brain.getAverageFiringRate())
        self.metrics['neuron_count'].append(self.brain.getTotalNeuronCount())
    
    def get_summary(self):
        return {
            'total_spikes': self.metrics['spike_count'][-1],
            'avg_firing_rate': sum(self.metrics['firing_rate']) / len(self.metrics['firing_rate']),
            'peak_firing': max(self.metrics['firing_rate']),
            'final_neuron_count': self.metrics['neuron_count'][-1]
        }

# Use performance monitor
monitor = PerformanceMonitor(brain)
for i in range(1000):
    brain.step(i)
    monitor.record_step(i)

print("Performance Summary:", monitor.get_summary())
```

## Error Handling

The NLM Python module provides comprehensive error handling:

### Common Exceptions

| Exception | Cause | Handling |
|-----------|-------|----------|
| `ValueError` | Invalid parameter values | Check parameter ranges |
| `RuntimeError` | Brain not initialized | Call `brain.initialize()` first |
| `KeyError` | Invalid configuration key | Use `config.has(key)` to check |

### Best Practices

```python
import pynlm

try:
    # Create and initialize brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
        
except Exception as e:
    print(f"Error in simulation: {e}")
    # Attempt recovery
    brain.reset()
    brain.initialize()
```

## Configuration Files

The NLM supports configuration files in simple key=value format:

### Example Config File

```ini
# NLM Configuration File
# brain simulation parameters

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Loading Configuration Files

```python
import pynlm

# Create brain and load configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.cfg")

brain = pynlm.createBrain(config)
brain.initialize()
```

## Performance Optimization

### Tips for Faster Simulations

1. **Reduce Neuron Count**: Start with smaller networks for development
2. **Increase Time Step**: Use larger dt for faster simulation
3. **Disable Learning**: Turn off plasticity for basic simulations
4. **Batch Operations**: Group operations where possible

### Memory Management

```python
import pynlm
import gc

# Enable garbage collection
import tracemalloc
tracemalloc.start()

# Run simulation
brain = pynlm.createBrain()
brain.initialize()

for i in range(1000):
    brain.step(i)
    
    # Check memory usage periodically
    if i % 100 == 0:
        current, peak = tracemalloc.get_traced_memory()
        print(f"Step {i}: Current memory usage = {current / 1024 / 1024:.2f} MB")

tracemalloc.stop()
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ NLM to Python bindings:

**C++** → **Python**
```cpp
// C++
Config config;
config.set("neuron_count", 1000);
brain.initialize();
```

```python
# Python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
```

### Version Notes

- **Version 0.1.0**: Initial Python bindings release
- **Phase 2**: Basic neural computation support
- **Future**: Enhanced visualization, advanced learning algorithms

## Troubleshooting

### Common Issues and Solutions

1. **"Brain not initialized" error**
   ```python
   # Problem: Forgetting to initialize brain
   brain = pynlm.createBrain()
   brain.step(0)  # Error: brain not initialized
   
   # Solution
   brain = pynlm.createBrain()
   brain.initialize()
   brain.step(0)
   ```

2. **Configuration key errors**
   ```python
   # Problem: Invalid configuration key
   config = pynlm.createDefaultConfig()
   config.set("invalid_key", 123)  # May fail
   
   # Solution: Use getOr with defaults
   value = config.getOr("valid_key", "default_value")
   ```

3. **Memory issues**
   ```python
   # Problem: Memory leak in long simulations
   # Solution: Monitor memory usage and reset periodically
   if step % 1000 == 0:
       brain.reset()
       brain.initialize()
   ```

## Examples

### Basic Brain Simulation

```python
import pynlm

# Create simple brain
brain = pynlm.createBrain()
brain.initialize()

# Run basic simulation
print("Running basic simulation...")
for i in range(100):
    brain.step(i)

print(f"Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Complete Agent Simulation

```python
import pynlm

# Setup complete agent system
brain = pynlm.createBrain()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure environment
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)

# Initialize systems
brain.initialize()
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent in world
print("Running agent simulation...")
for step in range(200):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Advanced Usage

For advanced users who want to get the most out of NLM, this section covers:

### 1. Complex Environment Setup

```python
import pynlm

# Create custom world with specific properties
world = pynlm.createSimpleWorld()
world.configure(
    width=50,              # Larger world for complex tasks
    height=50,
    visionWidth=12,        # Better sensory resolution
    visionHeight=12,
    # Additional properties if available
    cellSize=1.0,          # Size of each world cell
    updateRate=30.0        # World update frequency
)

# Add environmental features
world.setFoodDistribution("gaussian", center=(25, 25), radius=5.0)
world.setObstacles(["wall", "pit"], positions=[(10, 10), (40, 40)])
```

### 2. Brain Development and Evolution

```python
import pynlm

# Create brain with developmental plasticity
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("development_synaptogenesis_rate", 0.002)
config.set("development_pruning_rate", 0.0002)

brain = pynlm.createBrain(config)
brain.enableDevelopment(True)
brain.enableCuriosity(True)
brain.enableStructuralPlasticity(True)

# Monitor development
development_stages = []

for day in range(1, 101):
    for step in range(20):  # Simulate each day
        brain.step(step)
    
    stage = brain.getDevelopmentalStage()
    firing_rate = brain.getAverageFiringRate()
    neuron_count = brain.getTotalNeuronCount()
    
    development_stages.append({
        'day': day,
        'stage': stage,
        'firing_rate': firing_rate,
        'neuron_count': neuron_count,
        'complexity': len(brain.getRegions()) if hasattr(brain, 'getRegions') else 1
    })
    
    print(f"Day {day}: {stage}, FR={firing_rate:.2f} Hz, Neurons={neuron_count}")

# Analyze development patterns
final_stage = development_stages[-1]['stage']
peak_firing_day = max(development_stages, key=lambda x: x['firing_rate'])['day']

print(f"\nDevelopment Analysis:")
print(f"Final developmental stage: {final_stage}")
print(f"Peak firing day: {peak_firing_day}")
print(f"Neuron count growth: {development_stages[-1]['neuron_count'] - development_stages[0]['neuron_count']}")
```

### 3. Learning Experiment with Analysis

[Complete advanced learning experiment code would go here...]

### 4. Performance Optimization and Profiling

[Complete performance profiling example would go here...]

### 5. Advanced Configuration Management

[Complete configuration management example would go here...]

### 6. Advanced Visualization and Analysis

[Complete visualization example would go here...]

### 7. Scientific Experiment Management

[Complete scientific experiment example would go here...]

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

## Learning Path

For those new to NLM, we recommend this learning progression:

1. **Start with Basic Examples**: Use `easy_usage.md` to get familiar with core concepts
2. **Try Advanced Examples**: Work through `advanced_examples.py` to understand advanced usage
3. **Create Custom Experiments**: Use the provided templates to design your own experiments
4. **Analyze Results**: Use the visualization and analysis tools to understand your results
5. **Publish Findings**: Use the scientific experiment management tools to document your work

The NLM framework provides a comprehensive platform for neural network research and experimentation, with tools for everything from simple demonstrations to complex scientific studies.
