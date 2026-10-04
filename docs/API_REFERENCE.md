# NLM Python API Reference Guide

## Overview

This document provides complete documentation for the NLM Python API. The Python bindings expose the full NLM computational brain capabilities through a clean, object-oriented interface.

## Module Structure

The main `pynlm` module provides the following components:

- **Brain** - The core neural computation engine
- **Config** - Configuration management system
- **AgentBrain** - Interface layer between brain and world
- **SimpleWorld** - Basic 2D environment simulation
- **WorldObject** - Objects in the simulation world
- **NeuronType** - Enumeration of neuron types
- **DevelopmentalStage** - Enumeration of developmental stages
- **ActionType** - Enumeration of action types
- **WorldObjectType** - Enumeration of world object types

## Brain Class

### Creation and Initialization

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Simulation Control

```python
# Run one simulation step
brain.step(step_number)

# Run step with timestamp (for time-sensitive simulations)
brain.step(step_number, timestamp)

# Reset brain to initial state
brain.reset()

# Process external sensory input
brain.receiveSensoryInput(sensory_input)

# Get motor output from brain activity
action = brain.produceAction()
```

### State Management

```python
# Save brain state to file
brain.save("checkpoint.bin")

# Load brain state from file
brain.load("checkpoint.bin")
```

### Neural Statistics

```python
# Basic statistics
total_neurons = brain.getTotalNeuronCount()
total_synapses = brain.getTotalSynapseCount()
firing_neurons = brain.getFiringNeuronCount()
average_firing_rate = brain.getAverageFiringRate()
max_firing_rate = brain.getMaxFiringRate()
excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()

# Connectivity statistics
total_connections = brain.getTotalConnections()
average_weight = brain.getAverageSynapseWeight()
weight_std = brain.getSynapseWeightStd()

# Activity statistics
total_spikes = brain.getTotalSpikeCount()
spike_frequency = brain.getSpikeFrequency()

# Memory system access
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
concept_memory = brain.getSemanticMemory()
procedural_memory = brain.getProceduralMemory()
```

### Regional Management

```python
# Add a new brain region
region_id = brain.addRegion("cortex")

# Get brain region
region = brain.getRegion(region_id)

# List all regions
regions = brain.getAllRegions()

# Remove a region (if supported)
brain.removeRegion(region_id)
```

### Configuration Access

```python
# Get configuration value
neuron_count = brain.getConfigValue("brain.neuron_count")

# Check if configuration exists
if brain.hasConfig("brain.synapse_density"):
    density = brain.getConfigValue("brain.synapse_density")

# Get all configuration keys
keys = brain.getConfigKeys()
```

## Config Class

### Creation

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create from file
config.loadFromFile("config.json")
```

### Configuration Operations

```python
# Set configuration value
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Get configuration value
value = config.get("brain.neuron_count")

# Check if configuration exists
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Get all configuration keys
keys = config.getKeys()

# Load configuration from file
config.loadFromFile("my_config.json")

# Save configuration to file
config.saveToFile("my_config.json")
```

### Configuration Categories

#### Brain Configuration
```python
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)
config.set("brain.tau_mem", 20.0)
```

#### Plasticity Configuration
```python
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
```

#### Neuromodulation Configuration
```python
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
```

#### Performance Configuration
```python
config.set("performance.optimization_level", 2)
config.set("simulation.max_steps", 10000)
config.set("memory.enable_pooling", True)
```

## AgentBrain Class

### Creation and Initialization

```python
import pynlm

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

### Sensory Processing

```python
# Process sensory input from world
agent.processSensoryInput(percept)
```

### Motor Decoding

```python
# Get motor command from brain activity
motor_cmd = agent.decodeMotorCommand()
```

### Neuromodulation

```python
# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Get neuromodulation level
neuromod_level = agent.getNeuromodulationLevel()
```

### Development

```python
# Update developmental stage
agent.updateDevelopment(timestep)

# Get current developmental stage
stage = agent.getDevelopmentalStage()

# Get developmental age
age = agent.getDevelopmentalAge()
```

### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

# Check if subsystems are enabled
reward_enabled = agent.isRewardModulationEnabled()
curiosity_enabled = agent.isCuriosityEnabled()
```

### Statistics and Monitoring

```python
# Get system statistics
curiosity_level = agent.getCuriosityLevel()
novelty_level = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
neuromodulation_level = agent.getNeuromodulationLevel()
development_progress = agent.getDevelopmentProgress()

# Get system status
is_ready = agent.isReady()
get_subsystem_status = agent.getSubsystemStatus()
```

## SimpleWorld Class

### Creation and Configuration

```python
# Create simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

### World Control

```python
# Reset world to initial state
world.reset()

# Update world by timestep
world.update(timestep=0.1)

# Set random seed for reproducibility
world.setRandomSeed(42)
```

### Agent Interaction

```python
# Set agent starting position
world.setAgentStart(x, y)

# Apply motor command to world
world.applyMotorCommand(motor_cmd, current_time)
```

### State Access

```python
# Get sensory percept from world
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
```

### World Object Management

```python
# Add object to world
world.addObject(
    pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
)

# Remove object from world
world.removeObject(x, y)

# Check if position is valid
is_valid = world.isValidPosition(x, y)
```

### World Properties

```python
# Get world dimensions
width = world.getWidth()
height = world.getHeight()

# Get world properties
vision_width = world.getVisionWidth()
vision_height = world.getVisionHeight()
simulation_time = world.getSimulationTime()
max_energy = world.getMaxEnergy()

# Set world properties
world.setMaxEnergy(100.0)
```

## Enumerations

### NeuronType

```python
import pynlm

# Neuron types
neuron_type = pynlm.NeuronType.Excitatory
neuron_type = pynlm.NeuronType.Inhibitory
neuron_type = pynlm.NeuronType.Sensory
neuron_type = pynlm.NeuronType.Motor
neuron_type = pynlm.NeuronType.Modulatory
neuron_type = pynlm.NeuronType.Internal
```

### DevelopmentalStage

```python
development_stage = pynlm.DevelopmentalStage.Initial
development_stage = pynlm.DevelopmentalStage.CriticalPeriod
development_stage = pynlm.DevelopmentalStage.Maturation
development_stage = pynlm.DevelopmentalStage.Adult
development_stage = pynlm.DevelopmentalStage.Aging
```

### ActionType

```python
action_type = pynlm.ActionType.MoveForward
action_type = pynlm.ActionType.TurnLeft
action_type = pynlm.ActionType.Interact
action_type = pynlm.ActionType.Eat
# ... and more action types
```

### WorldObjectType

```python
object_type = pynlm.WorldObjectType.Empty
object_type = pynlm.WorldObjectType.Resource
object_type = pynlm.WorldObjectType.Hazard
object_type = pynlm.WorldObjectType.Wall
object_type = pynlm.WorldObjectType.Marker
```

## Error Handling

### Common Exceptions

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Your simulation code here
    
except pynlm.NLMRuntimeError as e:
    print(f"NLM runtime error: {e}")
    
except pynlm.NLMBadConfigurationError as e:
    print(f"Configuration error: {e}")
    
except pynlm.NLMMemoryError as e:
    print(f"Memory error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Error Codes

| Error Code | Description |
|------------|-------------|
| `NLM_INVALID_STEP` | Step number is invalid |
| `NLM_CONFIG_ERROR` | Configuration error |
| `NLM_MEMORY_ERROR` | Memory allocation error |
| `NLM_FILE_ERROR` | File I/O error |
| `NLM_INITIALIZATION_ERROR` | Brain initialization error |

## Performance Tips

### Memory Management

```python
# Use configuration to optimize memory usage
config.set("performance.optimization_level", 3)
config.set("memory.enable_pooling", True)

# Create brain with custom memory pool size
config.set("memory.pool_size", 1024)
```

### Simulation Optimization

```python
# Configure for high-performance simulation
config.set("simulation.max_steps", 10000)
config.set("plasticity.enable_stdp", True)
config.set("plasticity.enable_hebbian", False)
```

### Monitoring Performance

```python
# Monitor simulation performance
start_time = time.time()

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        step_time = time.time() - start_time
        firing_rate = brain.getAverageFiringRate()
        print(f"Step {step}: {step_time:.3f}s, {firing_rate:.2f} Hz")

end_time = time.time()
total_time = end_time - start_time
print(f"Total simulation time: {total_time:.2f}s")
```

## Advanced Usage Examples

### Complex Brain Configuration

```python
def create_advanced_config():
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.05)
    config.set("brain.v_thresh", -45.0)
    
    # Advanced plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.0005)
    config.set("plasticity.stdp.tau_plus", 30.0)
    config.set("plasticity.stdp.tau_minus", 30.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.rate", 0.01)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.1)
    
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.8)
    
    # Advanced performance configuration
    config.set("performance.optimization_level", 3)
    config.set("performance.enable_parallel", True)
    config.set("performance.enable_vectorization", True)
    
    return config

advanced_config = create_advanced_config()
brain = pynlm.createBrain(advanced_config)
```

### Complete Simulation Pipeline

```python
def run_complete_simulation(config, world_config, num_steps):
    # Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with comprehensive monitoring
    simulation_data = {
        'steps': [],
        'rewards': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Calculate reward
        reward = calculate_reward(percept, world)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        simulation_data['steps'].append(step)
        simulation_data['rewards'].append(reward)
        simulation_data['firing_rates'].append(brain.getAverageFiringRate())
        simulation_data['curiosity_levels'].append(agent.getCuriosityLevel())
        simulation_data['novelty_levels'].append(agent.getNoveltyLevel())
    
    return simulation_data

def calculate_reward(percept, world):
    # Custom reward function - can be modified based on experiment goals
    internal = percept.getInternal() if percept.getInternal() else []
    reward = internal[0] if internal else 0.0
    
    # Add bonus for proximity to goals
    agent_body = world.getAgentBody()
    for obj in world.getObjects():
        if obj.getType() == pynlm.WorldObjectType.Resource:
            distance = calculate_distance(agent_body.getPosition(), obj.getPosition())
            if distance < 2.0:
                reward += 1.0
    
    return reward

def calculate_distance(pos1, pos2):
    return ((pos1[0] - pos2[0])**2 + (pos1[1] - pos2[1])**2)**0.5

# Run complete simulation
config = create_advanced_config()
world_config = {
    'width': 20, 'height': 20,
    'visionWidth': 8, 'visionHeight': 8
}

simulation_data = run_complete_simulation(config, world_config, 1000)
```

### Brain State Persistence

```python
import pynlm

# Save brain state for later analysis
brain.save("simulation_checkpoint.bin")

# Load brain state
loaded_brain = pynlm.createBrain(config)
loaded_brain.load("simulation_checkpoint.bin")
loaded_brain.initialize()

# Compare brain states
def compare_brains(brain1, brain2):
    neuron_diff = abs(brain1.getTotalNeuronCount() - brain2.getTotalNeuronCount())
    firing_diff = abs(brain1.getFiringNeuronCount() - brain2.getFiringNeuronCount())
    weight_diff = abs(brain1.getAverageSynapseWeight() - brain2.getAverageSynapseWeight())
    
    return {
        'neuron_count_difference': neuron_diff,
        'firing_neuron_difference': firing_diff,
        'synapse_weight_difference': weight_diff,
        'similarity_score': 1.0 - (neuron_diff + firing_diff + weight_diff) / 3.0
    }

comparison = compare_brains(brain, loaded_brain)
print(f"Brain similarity: {comparison['similarity_score']:.3f}")
```

## Module Information

### Version

```python
import pynlm
print(f"NLM Python bindings version: {pynlm.__version__}")
```

### Platform Information

```python
import pynlm

platform_info = {
    'platform': pynlm.getPlatform(),
    'python_version': pynlm.getPythonVersion(),
    'compiler': pynlm.getCompilerInfo(),
    'build_date': pynlm.getBuildDate()
}

print("Platform information:", platform_info)
```

### Dependencies

```python
import pynlm

dependencies = pynlm.getDependencies()
print("Required dependencies:", dependencies)
```

## Migration Guide

### From C++ to Python

This section provides guidance for transitioning from the C++ API to the Python API.

#### Basic Equivalents

| C++ | Python |
|-----|--------|
| `Brain brain(config)` | `brain = pynlm.createBrain(config)` |
| `brain.initialize()` | `brain.initialize()` |
| `brain.step(step)` | `brain.step(step)` |
| `brain.save("file")` | `brain.save("file")` |
| `brain.load("file")` | `brain.load("file")` |

#### Advanced Features

| C++ | Python |
|-----|--------|
| `Brain::getWorkingMemory()` | `brain.getWorkingMemory()` |
| `AgentBrain agent(brain)` | `agent = pynlm.createAgentBrain(brain)` |
| `SimpleWorld world` | `world = pynlm.createSimpleWorld()` |

### Common Patterns

#### Configuration Management

```python
# C++
config->set("brain.neuron_count", 1000);

# Python
config.set("brain.neuron_count", 1000)
```

#### Error Handling

```python
// C++
if (!brain.step(step)) {
    throw std::runtime_error("Step failed");
}

// Python
try:
    brain.step(step)
except pynlm.NLMRuntimeError:
    print("Step failed")
```

## Troubleshooting

### Import Errors

**Error:** `ImportError: No module named 'pynlm'`

**Solution:**
```bash
pip install pynlm
```

### Runtime Errors

**Error:** Brain not initialized

**Solution:**
```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()  # Always call initialize first
    brain.step(0)
except pynlm.NLMRuntimeError as e:
    print(f"Brain error: {e}")
```

### Performance Issues

**Problem:** Simulation is too slow

**Solution:**
```python
# Increase optimization level
config.set("performance.optimization_level", 3)

# Reduce simulation complexity
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
```

## Further Reading

- [Configuration Guide](CONFIGURATION_GUIDE.md)
- [Command-Line Interface Guide](COMMAND_LINE_GUIDE.md)
- [Performance Guide](PERFORMANCE_GUIDE.md)
- [Experiment Guide](EXPERIMENT_GUIDE.md)
- [Troubleshooting Guide](TROUBLESHOOTING_GUIDE.md)