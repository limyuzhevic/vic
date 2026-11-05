# NLM Python API Documentation

This document describes the improved Python API for the NLM (Neural Learning Machine) framework.

## Overview

The NLM Python bindings provide access to the C++ neural simulation framework through a Python interface. The API has been enhanced with better factory methods, configuration loading options, and improved usability.

## Core Classes

### Config

The `Config` class manages configuration parameters for NLM simulations.

**Constructor:**
```python
config = pynlm.Config()
```

**Loading Configuration:**

Load from file (supports JSON, YAML, and simple key=value formats):
```python
config.loadFromFile("configs/default.cfg")
config.loadFromFile("config.json")
config.loadFromFile("config.yaml")
```

Load from command line arguments:
```python
config.loadFromArgs(["--neurons=500", "--steps=2000", "--checkpoint=./save.bin"])
```

Load from JSON string:
```python
json_str = '{"neuron_count": 1000, "region_count": 2, "connection_probability": 0.1}'
config.loadFromJsonString(json_str)
```

**Value Access Methods:**

Get values with type-specific methods:
```python
# Get integer value
neuron_count = config.getInt("neuron_count")

# Get double/float value  
simulation_timestep = config.getDouble("simulation_timestep")

# Get boolean value
enable_development = config.getBool("enable_development")

# Get string value
environment = config.getString("environment_name")

# Get value with default (auto-detects type)
value = config.get("some_key")  # Returns None if not found
```

**Save Configuration:**
```python
config.saveToFile("output_config.cfg")
```

**Utility Methods:**
```python
keys = config.getKeys()  # Get all configuration keys
config.clear()           # Clear all configuration
info = config.summary()  # Get configuration summary
```

**Factory Methods:**

Create different types of configurations:

```python
# Default configuration (minimal)
config = pynlm.createDefaultConfig()

# Research-grade configuration with enhanced plasticity
config = pynlm.createResearchConfig()

# Debug configuration for development
config = pynlm.createDebugConfig()

# Performance-optimized configuration for large simulations
config = pynlm.createPerformanceConfig()

# Custom configuration with specific parameters
config = pynlm.createStandardConfig(neuronCount=1000, randomSeed=12345)
```

### Brain

The `Brain` class represents a neural simulation system with spiking neurons and plasticity.

**Constructor:**
```python
brain = pynlm.createBrain(config)
```

**Core Methods:**
```python
brain.initialize()           # Initialize the brain with configuration
brain.step(step)             # Perform a simulation step
brain.reset()                # Reset brain state
brain.save("path/to/file")   # Save brain state to file
brain.load("path/to/file")   # Load brain state from file

# Get brain statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getActiveNeuronCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
brain.getTotalSpikeCount()
```

### AgentBrain

The `AgentBrain` class connects the NLM brain to the environment and handles sensory processing and action selection.

**Constructor:**
```python
agent = pynlm.createAgentBrain(brain)
```

**Core Methods:**
```python
agent.initialize(world)               # Initialize with world
agent.processSensoryInput(percept)    # Process sensory input
agent.decodeMotorCommand()            # Decode brain activity to action
agent.applyRewardModulation(reward, predicted)  # Apply reward-based learning
agent.updateDevelopment(timestep)     # Update development system

# Get internal states and levels
agent.getNeuromodulationLevel()
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getDevelopmentalStage()

# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

### SimpleWorld

The `SimpleWorld` class represents a 2D simulation environment.

**Constructor:**
```python
world = pynlm.createSimpleWorld()
```

**Configuration:**
```python
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
```

**Core Methods:**
```python
world.update(timestep)                  # Update environment
world.applyMotorCommand(cmd, time)      # Apply action to world
percept = world.getSensoryPercept()     # Get sensory input
body = world.getAgentBody()             # Get agent state
world.reset()                           # Reset world
world.setRandomSeed(seed)               # Set random seed
```

## Configuration Templates

The NLM framework provides several standard configuration templates for different use cases:

### 1. Default Configuration (`createDefaultConfig()`)

- Minimal viable configuration for basic operation
- 500 neurons, 1 region, basic plasticity
- Suitable for quick testing and validation

### 2. Research Configuration (`createResearchConfig()`)

- Enhanced configuration for research scenarios
- 2000 neurons, 3 regions, advanced plasticity
- Includes neuromodulation, development, and prediction systems
- Optimized for scientific experiments

### 3. Debug Configuration (`createDebugConfig()`)

- Configuration optimized for debugging and development
- 100 neurons for fast iteration
- Detailed logging and checkpointing
- Easier to monitor and troubleshoot

### 4. Performance Configuration (`createPerformanceConfig()`)

- Configuration optimized for large-scale simulations
- 5000 neurons, sparse connectivity
- Minimal neuromodulation and development
- Better performance for benchmarking

## Usage Examples

### Example 1: Basic Simulation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain and environment
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    # Get observation
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step, step * 0.001)
    
    # Get motor command
    action = agent.decodeMotorCommand()
    
    # Apply to world
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    agent.applyRewardModulation(result.reward, 0.0)
    agent.updateDevelopment(0.001)

# Print results
print(f"Total reward: {agent.getNeuromodulationLevel()}")
print(f"Avg firing rate: {brain.getAverageFiringRate()}")
```

### Example 2: Custom Configuration with JSON

```python
import pynlm
import json

# Define custom configuration
config = pynlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.001)

# Or load from JSON string
json_config = '''
{
    "neuron_count": 1500,
    "region_count": 3,
    "connection_probability": 0.2,
    "simulation_timestep": 0.0005,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.018,
    "enable_reward_modulation": true,
    "enable_curiosity": true
}
'''
config.loadFromJsonString(json_config)

# Use configuration
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
brain.initialize()
```

### Example 3: Multi-Agent Simulation

```python
import pynlm

# Create separate configurations for each agent
config1 = pynlm.createResearchConfig()
config1.set("random_seed", 42)

config2 = pynlm.createResearchConfig()  
config2.set("random_seed", 123)

# Create brains and worlds for each agent
brain1 = pynlm.createBrain(config1)
brain2 = pynlm.createBrain(config2)
world1 = pynlm.createSimpleWorld()
world2 = pynlm.createSimpleWorld()

# Configure worlds
world1.configure(20, 20, 16, 16)
world2.configure(20, 20, 16, 16)

# Set different starting positions
world1.setAgentStart(2, 2)
world2.setAgentStart(18, 18)

# Create agents
agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)

# Initialize
agent1.initialize(world1)
agent2.initialize(world2)

# Enable different capabilities
agent1.enableCuriosity(True)
agent2.enableRewardModulation(True)

# Run simulation with multiple agents
for step in range(500):
    # Update both worlds
    world1.update(0.1)
    world2.update(0.1)
    
    # Get observations and process
    percept1 = world1.getSensoryPercept()
    percept2 = world2.getSensoryPercept()
    
    agent1.processSensoryInput(percept1)
    agent2.processSensoryInput(percept2)
    
    # Brain steps
    brain1.step(step, step * 0.001)
    brain2.step(step, step * 0.001)
    
    # Decode actions
    action1 = agent1.decodeMotorCommand()
    action2 = agent2.decodeMotorCommand()
    
    # Apply to respective worlds
    world1.applyMotorCommand(action1, world1.getSimulationTime())
    world2.applyMotorCommand(action2, world2.getSimulationTime())
```

## Configuration File Formats

The configuration system supports multiple file formats:

### 1. Simple Key=Value Format

```
# Simple configuration file
neuron_count = 500
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
```

### 2. JSON Format

```json
{
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "log_level": "INFO"
}
```

### 3. YAML Format

```yaml
# YAML configuration
neuron_count: 500
region_count: 2
connection_probability: 0.1
simulation_timestep: 0.001
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
log_level: INFO
```

## Building and Installation

The NLM Python package can be installed using pip:

```bash
pip install .
```

For development, install with additional dependencies:

```bash
pip install .[dev]
```

The package uses scikit-build-core for building the C++ extensions.

## Troubleshooting

### Common Issues:

1. **Module not found**: Ensure the package is installed correctly
   ```bash
   pip install -e .  # Install in development mode
   ```

2. **Compilation errors**: Install build dependencies
   ```bash
   pip install scikit-build-core pybind11
   ```

3. **Configuration loading errors**: Check file format
   - Simple format: `key = value`
   - JSON/YAML: Use proper syntax for your chosen format

4. **Performance issues**: For large simulations, use `createPerformanceConfig()`

## Version Information

- **Python API Version**: 0.1.0
- **C++ Core Version**: Phase 6
- **Supported Python versions**: 3.8, 3.9, 3.10, 3.11, 3.12

## License

MIT License. See LICENSE file for details.
