# Basic NLM Usage Examples

This file contains basic usage examples for the NLM Python API. These examples are designed to be simple and easy to understand for users new to the NLM framework.

## Basic Examples

### 1. Simplest Possible Brain

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### 2. Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### 3. Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### 4. Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Configuration Examples

### Creating and Using Configurations

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
print(f"Config keys: {config.getKeys()}")

# Set some values
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)

# Check if key exists
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    print(f"Brain has {neurons} neurons")

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Get config from brain
brain_config = brain.getConfig()
print(f"Brain config: {brain_config.summary()}")
```

### Configuration File I/O

```python
import pynlm
from pathlib import Path

# Save configuration to file
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

config.saveToFile("my_config.cfg")
print("Configuration saved to my_config.cfg")

# Load configuration from file
config2 = pynlm.createConfigFromFile("my_config.cfg")
print(f"Loaded configuration has {config2.get('brain.neuron_count')} neurons")

# Clean up
Path("my_config.cfg").unlink(missing_ok=True)
```

## World Examples

### Basic World Configuration

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Set agent start position
world.setAgentStart(10.0, 10.0)
world.reset()

# Check world state
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
```

### World Object Management

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)
world.reset()

# Add objects to the world
world.addObject(pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=10.0))
world.addObject(pynlm.WorldObject(x=25.0, y=25.0, type=pynlm.WorldObjectType.Resource, value=15.0))
world.addObject(pynlm.WorldObject(x=15.0, y=15.0, type=pynlm.WorldObjectType.Hazard, value=5.0))

# Check if position is valid
if world.isValidPosition(12.0, 12.0):
    print("Position (12, 12) is valid")
else:
    print("Position (12, 12) is not valid")

# Get agent body
agent_body = world.getAgentBody()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")
```

## Agent Examples

### Basic Agent Usage

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable some features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Get agent information
print(f"Sensory input size: {agent.getSensoryInputSize()}")
print(f"Motor output size: {agent.getMotorOutputSize()}")
print(f"Initial curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Agent Statistics

```python
import pynlm

# Create a complete simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()
agent.initialize(world)

# Run simulation and collect statistics
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Print agent statistics
print("=== Agent Statistics ===")
print(f"Brain neurons: {brain.getTotalNeuronCount()}")
print(f"Brain synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

## Complete Simulation Example

Here's a complete example that combines all the basic concepts:

```python
import pynlm

def run_complete_simulation():
    """Run a complete simulation with brain, world, and agent"""
    
    # 1. Create and configure the brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created and initialized")
    
    # 2. Create and configure the world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setMaxEnergy(100.0)
    world.setAgentStart(10.0, 10.0)
    world.reset()
    print("✓ World configured")
    
    # 3. Create and initialize the agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    print("✓ Agent configured and initialized")
    
    # 4. Run the simulation
    total_reward = 0.0
    num_steps = 100
    
    print(f"\nRunning simulation for {num_steps} steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get internal reward
        internal = percept.getInternal()
        if internal:
            total_reward += internal[0]
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}: neurons_firing={brain.getFiringNeuronCount()}, "
                  f"reward={internal[0] if internal else 0:.2f}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}")
    
    # 5. Print final statistics
    print(f"\n=== Simulation Complete ===")
    print(f"Total steps: {num_steps}")
    print(f"Total reward: {total_reward:.2f}")
    print(f"Final neurons firing: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world, total_reward

# Run the simulation
if __name__ == "__main__":
    brain, agent, world, reward = run_complete_simulation()
    print("\nSimulation completed successfully!")
```

## Quick Reference

| What you want | Code |
|---------------|------|
| Create brain | `brain = pynlm.createBrain()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getTotalNeuronCount()` |
| See world | `percept = world.getSensoryPercept()` |
| Make action | `action = agent.decodeMotorCommand()` |
| Do action | `world.applyMotorCommand(action, time)` |

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### Error Messages

- **ImportError: No module named 'pynlm'**: Run `pip install .` to install the NLM package
- **AttributeError: 'Brain' object has no attribute 'some_method'**: Check the API documentation for available methods
- **RuntimeError: Simulation failed**: Check your configuration parameters and simulation logic

## Next Steps

After you're comfortable with the basics:

1. **Read `HOW_TO_USE.md`** for more detailed explanations
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Try the advanced examples** in `examples/advanced_usage.py`
4. **Explore the C++ API** if you need more control
5. **Check the documentation** for information about all available features

That's it! You're now ready to use NLM. Have fun creating and experimenting with your virtual brains!
