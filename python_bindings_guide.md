# NLM Python Bindings - Advanced Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings. The Python API provides a high-level interface to the NLM neural simulation framework, making it easy to create and interact with neural networks.

## Installation

The NLM Python bindings are built using scikit-build-core. To install from source:

```bash
pip install scikit-build-core pybind11
pip install .
```

Or in development mode:

```bash
pip install -e .
```

## Core API Reference

### Basic Classes

#### Config

The `Config` class manages NLM configuration parameters.

**Methods:**
- `Config()` - Create empty configuration
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save to JSON file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration
- `summary()` - Get configuration summary

**Usage Example:**
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Save configuration
config.saveToFile("my_config.json")
```

#### Brain

The `Brain` class represents a neural network with spiking neurons.

**Key Methods:**
- `Brain(config)` - Create brain with configuration
- `initialize()` - Initialize the brain
- `step(step)` - Perform one simulation step
- `step(step, time)` - Perform simulation step with timestamp
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getFiringNeuronCount()` - Get firing neurons
- `getAverageFiringRate()` - Get average firing rate
- `getTotalSpikeCount()` - Get total spikes
- `produceAction()` - Generate motor action
- `reset()` - Reset brain state

**Usage Example:**
```python
import pynlm

# Create brain with custom parameters
brain = pynlm.createBrainFromParams(
    neuronCount=5000,
    regionCount=3,
    connectionProbability=0.05
)

brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

#### SimpleWorld

The `SimpleWorld` class provides a 2D simulation environment.

**Key Methods:**
- `SimpleWorld()` - Create world
- `configure(width, height, visionWidth, visionHeight)` - Set world dimensions
- `reset()` - Reset world
- `setAgentStart(x, y)` - Set agent start position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, time)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent state

**Usage Example:**
```python
import pynlm

# Create advanced world
world = pynlm.createWorldWithParams(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16,
    maxEnergy=200.0
)

world.reset()
print(f"World size: {world.getWidth()} x {world.getHeight()}")
```

#### AgentBrain

The `AgentBrain` class connects the brain to the world, handling sensory input and motor output.

**Key Methods:**
- `AgentBrain(brain)` - Create agent interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted)` - Apply reward
- `updateDevelopment(timestep)` - Update development

**Advanced Features:**
- `enableRewardModulation(True/False)` - Enable/disable reward learning
- `enableStructuralPlasticity(True/False)` - Enable/disable structural changes
- `enableDevelopment(True/False)` - Enable/disable development
- `enableCuriosity(True/False)` - Enable/disable curiosity

**Usage Example:**
```python
import pynlm

# Create complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for i in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced API Functions

#### Factory Functions

**Utility Functions:**
- `pynlm.createConfigFromFile(filename)` - Load configuration from file
- `pynlm.createBrainFromParams(neuronCount, regionCount, connectionProbability)` - Create brain with parameters
- `pynlm.createWorldWithParams(width, height, visionWidth, visionHeight, maxEnergy)` - Create world with parameters
- `pynlm.createAdvancedAgentBrain(brain, world)` - Create and initialize agent

**Usage Example:**
```python
import pynlm

# Create complete system with custom parameters
config = pynlm.createConfigFromFile("settings.json")
config.set("brain.neuron_count", 10000)

brain = pynlm.createBrainFromParams(
    neuronCount=10000,
    regionCount=5,
    connectionProbability=0.02
)

world = pynlm.createWorldWithParams(
    width=100,
    height=100,
    visionWidth=20,
    visionHeight=20,
    maxEnergy=500.0
)

agent = pynlm.createAdvancedAgentBrain(brain, world)
```

#### Simulation Utilities

**Simulation Functions:**
- `pynlm.runSimulation(brain, world, agent, steps)` - Run complete simulation
- `pynlm.getSystemInfo()` - Get system information

**Usage Example:**
```python
import pynlm

# Create and run complete simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(40, 40, 16, 16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
print(pynlm.getSystemInfo())
pynlm.runSimulation(brain, world, agent, 2000)
```

### Advanced Usage Examples

#### Example 1: Learning Agent with Curiosity

```python
import pynlm
import time

def create_curiosity_agent(world_size=60):
    # Create brain with developmental stages
    config = pynlm.createBrainFromParams(
        neuronCount=8000,
        regionCount=4,
        connectionProbability=0.03
    )
    
    # Enable advanced plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createWorldWithParams(
        width=world_size,
        height=world_size,
        visionWidth=12,
        visionHeight=12,
        maxEnergy=300.0
    )
    world.reset()
    
    # Create advanced agent
    agent = pynlm.createAdvancedAgentBrain(brain, world)
    
    # Configure for exploratory learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)  # Key for exploratory behavior
    agent.enableDevelopment(True)
    
    return brain, world, agent

# Run simulation
brain, world, agent = create_curiosity_agent()

print("Starting curiosity-driven exploration simulation...")
start_time = time.time()

pynlm.runSimulation(brain, world, agent, 5000)

elapsed = time.time() - start_time
print(f"Simulation completed in {elapsed:.2f} seconds")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Example 2: Multi-Agent Coordination

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor
import threading

class MultiAgentSystem:
    def __init__(self, num_agents, world_size):
        self.world_size = world_size
        self.agents = []
        self.world = None
        
        # Create shared world
        self.world = pynlm.createWorldWithParams(
            width=world_size,
            height=world_size,
            visionWidth=8,
            visionHeight=8,
            maxEnergy=100.0
        )
        self.world.reset()
        
        # Create agents
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 2000 + i * 500)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            agent.enableCuriosity(True)
            agent.enableRewardModulation(True)
            
            self.agents.append(agent)
    
    def run_parallel_steps(self, num_steps):
        with ThreadPoolExecutor(max_workers=len(self.agents)) as executor:
            futures = []
            
            for step in range(num_steps):
                # Each agent processes sensory input and acts
                for agent in self.agents:
                    future = executor.submit(self._agent_step, agent, step)
                    futures.append(future)
                
                # Wait for all agents to complete step
                for future in futures:
                    future.result()
                
                # Advance world time
                self.world.update(0.1)
    
    def _agent_step(self, agent, step):
        # Process sensory input
        percept = self.world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)

# Example usage
system = MultiAgentSystem(num_agents=5, world_size=80)
print("Multi-agent system created with 5 agents")
system.run_parallel_steps(100)
print("Parallel simulation completed")
```

#### Example 3: Parameter Search and Optimization

```python
import pynlm
import random

def evaluate_brain_config(config_params, world_size=50, duration=200):
    """Evaluate brain configuration and return fitness score"""
    
    # Create brain with given parameters
    brain = pynlm.createBrainFromParams(
        neuronCount=config_params['neurons'],
        regionCount=config_params['regions'],
        connectionProbability=config_params['connectivity']
    )
    
    brain.initialize()
    
    # Create world
    world = pynlm.createWorldWithParams(
        width=world_size,
        height=world_size,
        visionWidth=10,
        visionHeight=10,
        maxEnergy=200.0
    )
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure based on parameters
    agent.enableRewardModulation(config_params['reward'])
    agent.enableCuriosity(config_params['curiosity'])
    
    # Run simulation and collect metrics
    total_spikes = 0
    max_firing = 0
    curiosity_sum = 0
    
    for step in range(duration):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(random.random() * 10.0, 0.0)
        agent.updateDevelopment(0.1)
        
        total_spikes += brain.getTotalSpikeCount()
        firing = brain.getFiringNeuronCount()
        if firing > max_firing:
            max_firing = firing
        curiosity_sum += agent.getCuriosityLevel()
    
    # Calculate fitness score
    avg_firing = total_spikes / duration
    avg_curiosity = curiosity_sum / duration
    
    fitness = (
        avg_firing * 0.4 +
        avg_curiosity * 0.3 +
        max_firing * 0.2 +
        config_params['neurons'] * 0.1
    )
    
    return fitness, {
        'total_spikes': total_spikes,
        'avg_firing': avg_firing,
        'max_firing': max_firing,
        'avg_curiosity': avg_curiosity,
        'final_brain_state': brain
    }

# Parameter search
best_fitness = 0
best_params = None
best_results = None

print("Running parameter search...")
for generation in range(10):
    for trial in range(20):
        params = {
            'neurons': random.randint(1000, 5000),
            'regions': random.randint(1, 6),
            'connectivity': random.uniform(0.01, 0.1),
            'reward': random.choice([True, False]),
            'curiosity': random.choice([True, False])
        }
        
        fitness, results = evaluate_brain_config(params)
        
        if fitness > best_fitness:
            best_fitness = fitness
            best_params = params
            best_results = results
    
    print(f"Generation {generation + 1}: Best fitness = {best_fitness:.3f}")

print(f"\nBest configuration: {best_params}")
print(f"Best fitness: {best_fitness:.3f}")
if best_results:
    print(f"Total spikes: {best_results['total_spikes']}")
    print(f"Average firing: {best_results['avg_firing']:.1f}")
```

### Troubleshooting

#### Common Issues

**Issue: "ImportError: No module named 'pynlm'"**

**Solution:**
```bash
pip install --force-reinstall .
```

**Issue: "ModuleNotFoundError: No module named 'pybind11'"**

**Solution:**
```bash
pip install pybind11
```

**Issue: "ImportError: libpynlm.so: cannot open shared object file"**

**Solution:**
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/pynlm/lib
```

**Issue: "Segmentation fault"**

**Solution:**
1. Ensure `brain.initialize()` is called before `brain.step()`
2. Check for null pointers in Python bindings
3. Verify world configuration before simulation

#### Debugging Tips

**Enable Python Tracing:**
```python
import pynlm
import traceback

try:
    # Your code here
    pass
except Exception as e:
    print(f"Error type: {type(e).__name__}")
    print(f"Error message: {str(e)}")
    traceback.print_exc()
```

**Check Brain State:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created: {brain.getTotalNeuronCount()} neurons")
print(f"Regions: {brain.getRegionCount()}")
```

**World Configuration Check:**
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

print(f"World configured: {world.getWidth()} x {world.getHeight()}")
print(f"Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
```

### Version Information

**Current Version:** 0.1.0
**API Stability:** Experimental (v0.x)
**Python Support:** 3.8+

### License

MIT License

For more information, see the NLM documentation and architecture files in the `docs/` directory.