# NLM Python API Reference
# ========================

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API (`pynlm`). The API enables Python developers to create and interact with neural simulation brains, agents, and world simulations.

## Overview

The NLM Python API provides bindings to the C++ NLM neural simulation framework, offering access to advanced neural simulation capabilities including:

- LIF neuron dynamics with event-driven spike propagation
- Multiple plasticity mechanisms (STDP, Hebbian, reward-modulated)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty detection)
- Prediction and planning systems
- Developmental learning stages
- Advanced visualization and analysis tools

## Installation

```bash
pip install pynlm
```

The package includes the `nlmc` command-line tool and Python bindings for neural simulation.

## Core Classes

### Config

Configuration management for NLM simulations.

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.01)

# Get configuration values
neuron_count = config.get("brain.neuron_count", 1000)
```

**Key Methods:**
- `createDefaultConfig()` - Create a default configuration object
- `set(key, value)` - Set a configuration value using dot notation
- `get(key, default=None)` - Get a configuration value
- `save(filename)` - Save configuration to file
- `load(filename)` - Load configuration from file

### Brain

The core neural simulation class representing an artificial brain.

```python
import pynlm

# Create brain from configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Simulation loop
for step in range(1000):
    brain.step(step)
    # Access brain state and statistics
    firing_count = brain.getFiringNeuronCount()
    firing_rate = brain.getAverageFiringRate()
    dev_stage = brain.getDevelopmentalStage()
```

**Key Methods:**
- `initialize()` - Initialize brain with neurons and connections
- `step(step_index)` - Advance simulation by one time step
- `getTotalNeuronCount()` - Get total number of neurons
- `getTotalSynapseCount()` - Get total number of synapses
- `getFiringNeuronCount()` - Get number of currently firing neurons
- `getAverageFiringRate()` - Get average firing rate across all neurons
- `getDevelopmentalStage()` - Get current developmental stage
- `getSynapticPlasticityRate()` - Get current synaptic plasticity rate

**Attributes:**
- `neuron_count` - Number of neurons in the brain
- `synapse_density` - Probability of connections between neurons
- `plasticity_learning_rate` - Learning rate for plasticity mechanisms

### AgentBrain

Interface between brain and environment for agent behavior.

```python
import pynlm

# Create brain and agent interface
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Configure world interaction
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent-world interaction
for step in range(500):
    # Update world
    world.update(timestep=0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
```

**Key Methods:**
- `initialize(world)` - Initialize agent with world interface
- `processSensoryInput(percept)` - Process sensory input from world
- `decodeMotorCommand()` - Convert brain state to motor commands
- `applyRewardModulation(reward, confidence)` - Apply reward-based learning
- `updateDevelopment(delta_time)` - Update developmental state
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity system

### SimpleWorld

Environment simulation for agent interaction.

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8,
    numTargets=5,
    targetUpdateRate=10
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Update world simulation
time_step = 0.1
for step in range(100):
    world.update(timestep=time_step)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Apply motor command
    motor_cmd = pynlm.Action(pynlm.MotorCommandType.MOVE_FORWARD, 1.0, 0.0)
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight, ...)` - Configure world parameters
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Advance world simulation
- `getSensoryPercept()` - Get current sensory percept
- `applyMotorCommand(motor_cmd, time)` - Apply motor command to world
- `getSimulationTime()` - Get current simulation time

### SensoryInput

Represents sensory input to the agent from the environment.

```python
import pynlm

# Create sensory input
percept = pynlm.SensoryPercept()

# Add different sensory modalities
percept.addVisualFeature("wall", True, 0.9)
percept.addVisualFeature("target", False, 0.0)
percept.addVisualFeature("agent", True, 0.7)

percept.addEpisodicMemory("visited_location", (5.0, 5.0), 0.8)
percept.addEpisodicMemory("reward_location", (15.0, 15.0), 0.9)

percept.addInternalState("energy", 0.8)
percept.addInternalState("curiosity", 0.3)

percept.addSocialSignal("conspecific_presence", True, 0.6)
```

**Key Methods:**
- `addVisualFeature(feature, present, confidence)` - Add visual feature
- `addEpisodicMemory(key, value, strength)` - Add episodic memory
- `addInternalState(key, value, confidence)` - Add internal state
- `addSocialSignal(signal, present, confidence)` - Add social signal
- `getVisualFeatures()` - Get all visual features
- `getEpisodicMemory()` - Get episodic memories
- `getInternalStates()` - Get internal states
- `getSocialSignals()` - Get social signals

### Action

Represents motor commands output by the agent.

```python
import pynlm

# Create different types of actions
action1 = pynlm.Action(pynlm.MotorCommandType.MOVE_FORWARD, 1.0, 0.0)
action2 = pynlm.Action(pynlm.MotorCommandType.TURN_LEFT, 0.0, 45.0)
action3 = pynlm.Action(pynlm.MotorCommandType.GRAB, 1.0, 0.0)
action4 = pynlm.Action(pynlm.MotorCommandType.RELEASE, 1.0, 0.0)

# Action attributes
print(f"Command type: {action1.command_type}")
print(f"Strength: {action1.strength}")
print(f"Direction: {action1.direction}")
print(f"Confidence: {action1.confidence}")
```

**Key Methods:**
- `Action(command_type, strength, direction)` - Create action
- `MotorCommandType` - Enum: MOVE_FORWARD, TURN_LEFT, TURN_RIGHT, GRAB, RELEASE, SHOOT

## Utility Functions

### pynlm.createDefaultConfig()
```python
import pynlm

config = pynlm.createDefaultConfig()
```

### pynlm.createBrain(config)
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

### pynlm.createAgentBrain(brain)
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
```

### pynlm.createSimpleWorld()
```python
import pynlm

world = pynlm.createSimpleWorld()
```

## Configuration Key Paths

The configuration system uses dot notation for nested keys. Common paths include:

- `brain.neuron_count` - Number of neurons
- `brain.synapse_density` - Connection probability
- `plasticity.stdp.learning_rate` - STDP learning rate
- `plasticity.stdp.ltp_weight` - LTP weight
- `plasticity.stdp.ltd_weight` - LTD weight
- `development.initial_plasticity` - Initial plasticity level
- `development.critical_period_start` - Critical period start
- `development.critical_period_end` - Critical period end
- `neuromodulation.dopamine_baseline` - Dopamine baseline
- `reward.discount_factor` - Reward discount factor
- `environment.name` - Environment name
- `environment.width` - Environment width
- `environment.height` - Environment height

## Error Handling

The NLM Python API uses standard Python exceptions for error handling:

- `RuntimeError` - General runtime errors
- `ConfigurationError` - Configuration-related errors
- `InitializationError` - Initialization errors

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -100)  # Invalid value
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.InitializationError as e:
    print(f"Initialization error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Integration Examples

### Basic Brain Creation and Simulation

```python
import pynlm
import time

def basic_brain_example():
    """Create a basic brain and run a simple simulation."""
    print("=== Basic Brain Creation Example ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Optionally customize the configuration
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a simulation
    for step in range(100):
        brain.step(step)
        
        # Log statistics periodically
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("Simulation completed successfully!")
    return brain

if __name__ == "__main__":
    basic_brain_example()
```

### Complete Agent Simulation

```python
import pynlm
import time

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    print("=== Complete Agent Simulation Example ===")
    
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
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation completed!")
    return brain, agent, world

if __name__ == "__main__":
    run_agent_simulation(500)
```

## Performance Considerations

### Memory Usage
- Brain memory scales approximately as O(neuron_count^2) due to synaptic connections
- Use smaller neuron counts (300-1000) for quick testing
- Use configuration templates (lightweight.cfg) for memory-constrained environments

### Simulation Speed
- Sim steps scale approximately as O(neuron_count * steps)
- Use SIMD vectorization and parallelization for large-scale simulations
- Consider using GPU acceleration for very large networks (500+ neurons)

### Configuration Optimization
- For performance: use high_perf.cfg
- For development: use development.cfg
- For quick testing: use lightweight.cfg

## Advanced API Usage

### Custom Configuration Creation
```python
import pynlm
import json

# Create custom configuration
config_dict = {
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.2
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.005,
            "ltp_weight": 0.008,
            "ltd_weight": 0.009
        }
    },
    "development": {
        "initial_plasticity": 1.0,
        "critical_period_start": 100,
        "critical_period_end": 500
    },
    "environment": {
        "name": "GridWorld",
        "width": 30,
        "height": 30
    }
}

# Create config from dictionary
config = pynlm.createDefaultConfig()
for key, value in config_dict.items():
    if isinstance(value, dict):
        for subkey, subvalue in value.items():
            config.set(f"{key}.{subkey}", subvalue)
    else:
        config.set(key, value)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Multi-Agent System
```python
import pynlm

def create_multi_agent_system(num_agents, config):
    """Create a multi-agent system with shared resources."""
    brains = []
    agents = []
    
    for i in range(num_agents):
        # Create brain for each agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    
    # Initialize all agents
    for i, agent in enumerate(agents):
        # Place agents at different starting positions
        agent_x = (i * 5) % 45 + 2.5
        agent_y = ((i * 7) % 45) + 2.5
        world.setAgentStart(agent_x, agent_y)
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    return brains, agents, world
```

### Experiment Management
```python
import pynlm
import json
import os
import time

def run_parameter_sweep(base_config, parameter_grid, output_dir):
    """Run parameter sweep experiment."""
    os.makedirs(output_dir, exist_ok=True)
    
    results = []
    
    for param_name, param_values in parameter_grid.items():
        for param_value in param_values:
            # Create modified config
            config = pynlm.createDefaultConfig()
            
            # Copy base config values
            config.set("brain.neuron_count", base_config.get("brain.neuron_count", 1000))
            config.set("plasticity.stdp.learning_rate", base_config.get("plasticity.stdp.learning_rate", 0.001))
            
            # Set parameter
            config.set(f"brain.{param_name}", param_value)
            
            # Create and run brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(100):
                brain.step(step)
            
            # Record results
            result = {
                "parameters": {param_name: param_value},
                "final_neuron_count": brain.getTotalNeuronCount(),
                "final_synapse_count": brain.getTotalSynapseCount(),
                "final_firing_rate": brain.getAverageFiringRate(),
                "steps_completed": 100
            }
            results.append(result)
            
            # Save intermediate result
            with open(f"{output_dir}/result_{param_name}_{param_value}.json", 'w') as f:
                json.dump(result, f, indent=2)
    
    # Save complete results
    with open(f"{output_dir}/parameter_sweep_results.json", 'w') as f:
        json.dump(results, f, indent=2)
    
    return results
```

## Troubleshooting

### Common Issues and Solutions

1. **Brain initialization fails**
   - Check configuration values (neuron_count must be positive)
   - Ensure required configuration keys are present
   - Check for memory constraints

2. **Simulation runs slowly**
   - Reduce neuron_count for testing
   - Use lightweight.cfg for quick runs
   - Consider using development.cfg for optimized performance

3. **Configuration parsing errors**
   - Check configuration file syntax
   - Ensure all required configuration keys are present
   - Use valid configuration templates

4. **Memory issues**
   - Monitor memory usage with large neuron counts
   - Use smaller configuration values for memory-constrained environments
   - Consider using high_perf.cfg for optimized memory usage

### Debugging Tips

```python
import pynlm
import logging

# Enable verbose logging
logging.basicConfig(level=logging.DEBUG)

# Create brain with verbose configuration
config = pynlm.createDefaultConfig()
config.set("log_level", "DEBUG")
config.set("log_to_file", True)
config.set("log_filename", "debug_brain.log")

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(50):
    brain.step(step)
```

## Version Information

This API documentation applies to pynlm version 1.0, part of the NLM (Neural Learning Machine) project.

For more information about NLM, visit the NLM documentation or join the NLM community forums.
