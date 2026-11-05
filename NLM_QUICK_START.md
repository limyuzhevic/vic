# NLM - Quick Start Guide

## Getting Started with NLM

NLM (Neural Learning Machine) is a brain simulation framework that lets you create artificial brains that can learn, remember, and make decisions. This guide will help you get started quickly with the Python API.

## Prerequisites

- Python 3.8+
- pip (Python package installer)

## Quick Installation

```bash
pip install pynlm
```

## Your First Brain

### Example 1: Silent Brain

This is the simplest possible NLM brain - it just creates neurons and lets them "think".

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Make the brain think for 100 steps
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
```

### Example 2: Complete Agent Simulation

This example shows how to create a complete agent that interacts with a simulated world.

```python
import pynlm

def run_complete_simulation(steps=1000):
    """Run a complete NLM simulation with agent and world."""
    
    # 1. Setup the brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # 2. Create the agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure the world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # 4. Initialize the agent with the world
    agent.initialize(world)
    
    # 5. Enable learning features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableCuriosity(True)             # Explore new things
    agent.enableDevelopment(True)           # Brain matures over time
    
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
    print(f"Agent enabled features: RewardModulation, Curiosity, Development")
    
    # 6. Run the simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Brain decides action
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    print(f"\nSimulation complete!")
    print(f"Final stats: {brain.getTotalSpikeCount()} total spikes")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation(500)
```

## Configuration Examples

### Quick Start Configuration

Create a configuration optimized for beginners:

```python
import pynlm

# Create a quick-start config (500 neurons, simple setup)
config = pynlm.createQuickStartConfig()

# Modify parameters if needed
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Create brain with this config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Custom Configuration

Create a configuration with specific parameters:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Customize for your use case
config.set("brain.neuron_count", 2000)              # More neurons = more complex
config.set("brain.synapse_density", 0.05)           # Connectivity
config.set("plasticity.stdp.learning_rate", 0.002) # Learning speed
config.set("neuromod.dopamine.scale", 1.0)         # Reward sensitivity
config.set("neuromod.curiosity.enable", True)      # Enable exploration

# Save for later use
config.saveToFile("my_config.json")
```

## Simulation Modes

NLM supports different simulation modes optimized for different use cases:

### Fast Mode
- Optimized for speed
- Suitable for testing and prototyping
- Limited feature set

```python
agent = pynlm.createAgentBrain(brain)
agent.configureForSimulation("fast")
```

### Accurate Mode
- Maximum precision
- All features enabled
- Slower but more detailed

```python
agent = pynlm.createAgentBrain(brain)
agent.configureForSimulation("accurate")
```

### Balanced Mode
- Good trade-off between speed and features
- Recommended for most use cases

```python
agent = pynlm.createAgentBrain(brain)
agent.configureForSimulation("balanced")
```

### Minimal Mode
- Only essential features
- Maximum speed
- Good for simple simulations

```python
agent = pynlm.createAgentBrain(brain)
agent.configureForSimulation("minimal")
```

## Common Patterns

### Pattern 1: Agent with Reward Learning

```python
def run_learning_episode(agent, world, steps=200):
    """Run an episode where the agent learns from rewards."""
    agent.enableRewardModulation(True)
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # This brain activity will be influenced by rewards
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Agent learns from the outcome
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
```

### Pattern 2: Development Simulation

```python
def run_development_simulation(agent, world, steps=1000):
    """Simulate brain development over time."""
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Development updates based on experience
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"Development stage: {agent.getDevelopmentalStage()}")
```

### Pattern 3: Curiosity-Driven Exploration

```python
def run_curiosity_driven_simulation(agent, world, steps=500):
    """Simulate exploration driven by curiosity."""
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    novelty_count = 0
    total_spikes = 0
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        total_spikes = brain.getTotalSpikeCount()
        
        # Check for novel experiences
        if agent.getNoveltyLevel() > 0.1:
            novelty_count += 1
            print(f"Novel experience at step {step} (curiosity: {agent.getCuriosityLevel():.3f})")
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    print(f"\nExploration summary:")
    print(f"  Novel experiences: {novelty_count}")
    print(f"  Total spikes: {total_spikes}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
```

## Getting Help

### Built-in Diagnostics

```python
# Get comprehensive status information
status = agent.getFeatureStatus()
print(status)

# Get list of available features
features = agent.getAvailableFeatures()
print("Available features:", features)

# Validate setup
is_valid, errors = validate_brain_setup(brain, agent, world)
if not is_valid:
    print("Setup errors:", errors)
```

### Simulation Status

```python
# Get real-time simulation status
status = get_simulation_status(brain, agent, world)
for key, value in status.items():
    print(f"{key}: {value}")
```

## Saving and Loading

### Save Brain State

```python
# Save brain state to file
brain.save("my_brain_checkpoint.bin")

# Save configuration
config.saveToFile("my_config.json")
```

### Load Brain State

```python
# Load brain state
brain.load("my_brain_checkpoint.bin")

# Load configuration
config.loadFromFile("my_config.json")
```

## Troubleshooting

### Common Issues and Solutions

1. **"Brain isn't doing anything"**
   ```python
   # Ensure brain is initialized before stepping
   brain.initialize()
   for step in range(100):
       brain.step(step)
   ```

2. **"Agent isn't moving"**
   ```python
   # Make sure world updates are called
   world.update(0.1)
   action = agent.decodeMotorCommand()
   world.applyMotorCommand(action, world.getSimulationTime())
   ```

3. **"Everything is 0"**
   ```python
   # Brains need time to "warm up"
   for step in range(1000):  # More steps
       brain.step(step)
   ```

4. **Import errors**
   ```bash
   pip install --force-reinstall pynlm
   ```

## Next Steps

When you're comfortable with the basics, explore:

1. **Advanced Configuration** - Read `HOW_TO_USE.md` for more details
2. **Architecture Understanding** - Read `docs/ARCHITECTURE.md`
3. **Scientific Background** - Read `docs/SCIENCE.md`
4. **Experiment Framework** - Explore the experiment API
5. **Performance Optimization** - Use the benchmark tools
6. **Community** - Join the NLM community for support and examples

## Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Examples**: https://github.com/nlm-project/nlm-examples
- **Community**: https://discord.gg/nlm
- **API Reference**: Available in the pynlm module documentation

## License

MIT License - NLM is open source and free to use!
