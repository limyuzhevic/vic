# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API (`pynlm`). NLM is a brain-inspired spiking neural network simulator that implements a computational model of artificial development.

## Quick Start

For those new to NLM, here are the essential concepts and commands:

### Core Concepts

1. **Brain**: The central neural network with neurons and synapses
2. **World**: The environment where the brain interacts  
3. **Agent**: The interface connecting brain to world
4. **Sensory Input**: Information from the world to the brain
5. **Motor Commands**: Actions the brain produces to affect the world

### Basic Usage

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## API Reference

### Core Classes

#### Brain Class

**Creation**: `brain = pynlm.createBrain(config)`

**Methods**:
- `brain.initialize()`: Initialize the brain with configuration
- `brain.step(step_number)`: Run one simulation step
- `brain.step(step_number, time)`: Run step with timestamp
- `brain.receiveSensoryInput(input)`: Inject sensory input into brain
- `brain.produceAction()`: Get motor action based on neural activity
- `brain.reset()`: Reset brain state
- `brain.save(filepath)`: Save brain state to file
- `brain.load(filepath)`: Load brain state from file

**Properties**:
- `brain.getTotalNeuronCount()`: Total neurons in brain
- `brain.getTotalSynapseCount()`: Total synapses in brain
- `brain.getFiringNeuronCount()`: Currently firing neurons
- `brain.getAverageFiringRate()`: Average firing rate across neurons
- `brain.getTotalSpikeCount()`: Total spikes generated
- `brain.getDevelopmentalStage()`: Current developmental stage

#### Config Class

**Creation**: `config = pynlm.createDefaultConfig()`

**Methods**:
- `config.loadFromFile(filepath)`: Load configuration from file
- `config.saveToFile(filepath)`: Save configuration to file
- `config.has(key)`: Check if configuration key exists
- `config.set(key, value)`: Set configuration value
- `config.getKeys()`: Get all configuration keys
- `config.clear()`: Clear all configuration

**Common Configuration Keys**:
- `brain.neuron_count`: Total number of neurons
- `brain.synapse_density`: Synapse connectivity density
- `plasticity.stdp.enable`: Enable spike-timing-dependent plasticity
- `plasticity.hebbian.enable`: Enable Hebbian learning
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration

#### SimpleWorld Class

**Creation**: `world = pynlm.createSimpleWorld()`

**Methods**:
- `world.configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `world.reset()`: Reset world to initial state
- `world.setAgentStart(x, y)`: Set agent starting position
- `world.update(timestep)`: Update world simulation
- `world.applyMotorCommand(cmd, time)`: Apply motor command to world
- `world.getSensoryPercept()`: Get sensory input from world
- `world.getAgentBody()`: Get agent body state

#### AgentBrain Class

**Creation**: `agent = pynlm.createAgentBrain(brain)`

**Methods**:
- `agent.initialize(world)`: Initialize with world
- `agent.processSensoryInput(percept)`: Process sensory input
- `agent.decodeMotorCommand()`: Decode motor command from brain
- `agent.applyRewardModulation(reward, predictedReward)`: Apply reward-based neuromodulation
- `agent.updateDevelopment(timestep)`: Update development system

**Enable/Disable Methods**:
- `agent.enableRewardModulation(True/False)`
- `agent.enableStructuralPlasticity(True/False)`
- `agent.enableDevelopment(True/False)`
- `agent.enableCuriosity(True/False)`

### Advanced API Functions

#### Batch Operations

```python
# Batch operations for efficiency
pynlm.batchBrainSteps(brain, [0, 1, 2, 3, 4])
pynlm.batchSensoryInput(agent, [percept1, percept2, percept3])
```

#### Configuration Utilities

```python
# Advanced configuration creation
config = pynlm.createConfigFromDict({
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": True,
    "neuromod.curiosity.enable": True
})

# Performance statistics
stats = pynlm.brainPerformanceStats(brain)
print(f"Total neurons: {stats['total_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']}")
```

#### Advanced Input Creation

```python
# Create advanced sensory inputs
vision = pynlm.createVisionInput(width=8, height=8, channels=3)
audio = pynlm.createAudioInput(sampleRate=44100, numSamples=1024)

# Create custom actions
custom_action = pynlm.createCustomAction(
    pynlm.ActionType.Interact, 
    [0.5, 0.8, 1.2]
)
```

#### Export/Import Utilities

```python
# Export and import brain states
success = pynlm.exportBrainState(brain, "brain_state.json")
imported_brain = pynlm.importBrainState("saved_brain.json")

# Configuration backup/restore
pynlm.backupConfig(config, "config_backup.json")
restored_config = pynlm.restoreConfig("config_backup.json")
```

## Common Patterns

### Pattern 1: Simple Brain Simulation

```python
def run_simple_simulation(steps=1000):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain
```

### Pattern 2: Agent in World

```python
def run_agent_simulation(brain, world, steps=500):
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain
```

### Pattern 3: Learning with Reward

```python
def run_learning_simulation(brain, world, steps=1000):
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    for step in range(steps):
        # Provide random rewards
        reward = 0.0
        if step % 50 == 0:
            reward = 1.0
        
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(reward, 0.0)
    
    return brain
```

## Troubleshooting

### Common Issues and Solutions

#### "ImportError: No module named 'pynlm'"

**Solution**: Install the package:
```bash
pip install .
```

#### "Segmentation fault" or "RuntimeError"

**Solution**: Check that you call `initialize()` before any other operations:
```python
brain = pynlm.createBrain(config)
brain.initialize()  # Critical step
brain.step(0)       # Now safe to use
```

#### "My brain isn't doing anything"

**Solution**:
1. Call `brain.initialize()`
2. Run more simulation steps
3. Check if sensory input is connected
4. Enable appropriate plasticity mechanisms

#### "The agent isn't moving"

**Solution**:
1. Call `world.applyMotorCommand()` in your simulation loop
2. Ensure `world.update()` is being called
3. Check agent motor command decoding

#### "Everything is 0"

**Solution**:
1. Brains need time to "warm up" - try more steps
2. Some neurons need input to fire - ensure sensory input is connected
3. Check initial configuration parameters

## Performance Tips

### For Large-Scale Simulations

1. **Use Batch Operations**: Process multiple steps or sensory inputs at once
2. **Optimize Configuration**: Set appropriate neuron counts for your use case
3. **Enable Parallel Processing**: Use the advanced API for parallel operations
4. **Monitor Memory Usage**: Use `brainPerformanceStats()` to track resource usage

### Configuration Best Practices

```python
# Optimized configuration for typical use
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Start smaller, scale up
config.set("brain.synapse_density", 0.05)  # Moderate connectivity
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)
```

## Examples Directory

For complete working examples, see the `examples/` directory:

- `simple_brain.py`: Basic brain simulation
- `agent_world.py`: Complete agent-world interaction
- `learning_demo.py`: Demonstrates plasticity and learning
- `performance_test.py`: Performance benchmarking
- `configuration_examples.py`: Various configuration patterns

## Extending NLM

### Custom Neuron Types

For advanced users interested in extending NLM, the C++ API allows custom neuron types and plasticity rules. See the `docs/ARCHITECTURE.md` for more details.

### Plugin System

The advanced Python API includes hooks for custom simulation logic:
```python
def custom_simulation_logic(brain, world, agent, step):
    # Your custom logic here
    pass

# Use in custom simulation loop
pynlm.runCustomSimulationLoop(brain, world, agent, 
                             lambda s: s < 1000, 
                             custom_simulation_logic)
```

## License and Attribution

NLM is an experimental research project. Please cite:
- "NLM: An Experimental Artificial Developmental Brain" (2024)
- The NLM project documentation available at https://nlm.readthedocs.io

For scientific use, please acknowledge that NLM is not a biologically accurate model but a computational framework for exploring artificial developmental principles.