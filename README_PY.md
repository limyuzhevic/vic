# NLM Python Package README

## Quick Start

```python
"""NLM (熙然) - Neural Learning Machine

An experimental artificial developmental brain that learns from experience.
"""

import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate thinking
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

## Core Concepts

NLM simulates a brain-like system with:
- **Neurons**: Spiking cells that process information
- **Synapses**: Connections between neurons that strengthen or weaken
- **Brain**: The central neural network
- **Agent**: Interface between brain and environment
- **World**: The simulation environment

## Basic Usage

### 1. Create a Brain
```python
import pynlm

# Method 1: Simplest (recommended for beginners)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Method 2: Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
brain = pynlm.createBrain(config)

brain.initialize()
```

### 2. Run a Simulation
```python
# Simple thinking loop
for step in range(100):
    brain.step(step)

# Log statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 3. Use with World and Agent
```python
import pynlm

# Create the complete system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action
        action = agent.decodeMotorCommand()
        
        # 6. Execute action
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it
run_simulation(brain, world, agent, 1000)
```

## Complete Example: Agent in Environment

```python
import pynlm
import time

def create_and_run_agent(neuron_count=1000, steps=500):
    """Create a complete agent system and run simulation."""
    
    print(f"Creating agent with {neuron_count} neurons...")
    
    # 1. Create brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    # 4. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Starting simulation...")
    start_time = time.time()
    
    # 5. Run main simulation loop
    for step in range(steps):
        # Update environment
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes information
        brain.step(step)
        
        # Get motor command from brain
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0 and step > 0:
            elapsed = time.time() - start_time
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"curiosity={agent.getCuriosityLevel():.2f}, "
                  f"time={elapsed:.1f}s")
    
    # 6. Print final statistics
    elapsed = time.time() - start_time
    print(f"\nSimulation complete! ({elapsed:.1f}s)")
    print(f"Final stats:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
    
    return brain, agent, world

# Run the example
if __name__ == "__main__":
    brain, agent, world = create_and_run_agent()
```

## Common Patterns

### Pattern 1: Echo Simulation
```python
"""Simple brain that responds to its own activity."""
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Brain starts with some initial spikes
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)

# Simulate as brain "thinks"
for step in range(200):
    brain.step(step)
    
    # Brain responds to its own activity
    if brain.getTotalSpikeCount() % 10 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 5.0f)

print(f"Echo simulation complete. Final spikes: {brain.getTotalSpikeCount()}")
```

### Pattern 2: Reward-Based Learning
```python
"""Brain that learns from simulated rewards."""
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create simple world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()
agent.initialize(world)

# Enable reward modulation
agent.enableRewardModulation(True)

# Run with rewards
for step in range(300):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Simulate finding food (resource)
    if step % 50 == 0:
        percept = world.getSensoryPercept()
        agent.applyRewardModulation(1.0, 0.8)  # Positive reward
        
    # Simulate hitting hazard
    if step % 77 == 0:
        percept = world.getSensoryPercept()
        agent.applyRewardModulation(-0.5, 0.3)  # Negative reward

print("Reward learning simulation complete.")
```

### Pattern 3: Development Simulation
```python
"""Simulate brain development over time."""
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable development
agent.enableDevelopment(True)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

print("Development simulation:")
print(f"Initial stage: {brain.getDevelopmentalStage()}")

# Simulate development over time
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getDevelopmentalStage()}, "
              f"plasticity={agent.getNeuromodulationLevel():.2f}")

print(f"Final stage: {brain.getDevelopmentalStage()}")
```

## Advanced Features

### 1. Custom Configuration
```python
# Create custom configuration
def create_development_config(neuron_count=2000):
    config = pynlm.createDefaultConfig()
    
    # Neural parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.15)
    
    # STDP parameters
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    
    # Neuromodulation parameters
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    
    # Development parameters
    config.set("development.initial_plasticity", 1.0)
    config.set("development.plasticity_decay_rate", 0.05)
    
    return config

# Use custom config
config = create_development_config(3000)
brain = pynlm.createBrain(config)
```

### 2. Multiple Brains
```python
# Create multiple independent brains
def create_brain_network(count=5):
    brains = []
    for i in range(count):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500 + i * 200)
        config.set("random_seed", 12345 + i)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    return brains

# Create network
brains = create_brain_network(3)

# Run each brain independently
for i, brain in enumerate(brains):
    for step in range(100):
        brain.step(step)
    print(f"Brain {i}: {brain.getTotalSpikeCount()} spikes")
```

### 3. Save and Load
```python
import pynlm
import os

# Save brain state
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

brain.save("my_brain.bin")
print("Brain saved to my_brain.bin")

# Load brain state
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.load("my_brain.bin")
print("Brain loaded successfully")
print(f"Loaded brain has {new_brain.getTotalNeuronCount()} neurons")

# Clean up
os.remove("my_brain.bin")
```

## Error Handling

### Common Errors and Solutions

1. **Brain not initialized**
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
# ERROR: Always call initialize()
brain.step(0)  # This will cause issues

# CORRECT:
brain.initialize()
brain.step(0)  # Now works
```

2. **Memory usage too high**
```python
# Use smaller configurations for testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Small brain for testing
config.set("brain.synapse_density", 0.05)

# For production, use larger counts
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.1)
```

3. **Performance issues**
```python
# Profile with smaller simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run fewer steps for testing
for step in range(10):
    brain.step(step)

# Then scale up
for step in range(1000):
    brain.step(step)
```

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check neuron parameters in config

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are connected

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check development stage is appropriate

## Next Steps

When you're comfortable:
1. **Read** `HOW_TO_USE.md` for more technical details
2. **Read** `docs/ARCHITECTURE.md` to understand how it all works
3. **Experiment** with different configurations!
4. **Build** custom experiments using the Phase 6 framework
5. **Contribute** to the research by adding new plasticity rules or neuromodulators

That's it! You're now ready to use NLM.
