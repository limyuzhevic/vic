# NLM (Neural Learning Machine) - Quick Start Guide

## What is NLM?

NLM is a brain-inspired spiking neural network simulator that implements Leaky Integrate-and-Fire (LIF) neurons with learning capabilities. It allows you to create virtual brains that can learn, remember, and make decisions.

## Quick Start (3 Steps)

### Step 1: Install and Setup

```bash
# Install the NLM Python library
pip install pynlm

# Or from source (for development)
pip install -e .
```

### Step 2: Create Your First Brain

```python
import pynlm

# Create a brain with default settings
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Step 3: Run a Simple Simulation

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make the brain think for 100 steps
for i in range(100):
    brain.step(i)

print(f"Brain thought {brain.getTotalSpikeCount()} times")
print("Done! 🎉")
```

## Essential Concepts

### 1. Brain, World, and Agent

| Component | Purpose | Typical Usage |
|-----------|---------|---------------|
| **Brain** | Virtual neural network that processes information | `pynlm.createBrain(config)` |
| **World** | Environment where the brain operates | `pynlm.createSimpleWorld()` |
| **Agent** | Interface between brain and world | `pynlm.createAgentBrain(brain)` |

### 2. The Three-Step Process

1. **Get input**: `world.getSensoryPercept()`
2. **Process input**: `agent.processSensoryInput(percept)`
3. **Make output**: `agent.decodeMotorCommand()`

### 3. Learning Mechanisms

NLM supports multiple learning systems:
- **Reward Modulation**: Learn from rewards (dopamine-based)
- **Structural Plasticity**: Grow new connections
- **Development**: Brain maturation over time
- **Curiosity**: Explore novel experiences

## Common Workflows

### Workflow 1: Simple Brain Simulation

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development: {brain.getDevelopmentalStage()}")
```

### Workflow 2: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation():
    # 1. Create everything
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 2. Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 3. Run simulation
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Simulation complete!")

run_agent_simulation()
```

## Configuration

### Basic Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Modify settings
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Save and Load Configurations

```python
# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")

# Save brain state
brain.save("brain_checkpoint.bin")

# Load brain state
brain.load("brain_checkpoint.bin")
```

## Troubleshooting

### "Brain isn't doing anything"
```python
# Common mistake: forgot to initialize
brain = pynlm.createBrain(config)
brain.initialize()  # <-- This is required!

for step in range(100):
    brain.step(step)  # Now this will work
```

### "Agent isn't moving"
```python
# Make sure you're calling world.update()
for step in range(100):
    world.update(0.1)  # Update the world
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### "Everything is 0"
```python
# Brains need time to warm up
for step in range(1000):  # More steps
    brain.step(step)

# Make sure sensory input is connected
agent.processSensoryInput(world.getSensoryPercept())
```

## Advanced Features

### Complex Configuration

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Set multiple parameters
params = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.2,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.001,
    "plasticity.hebbian.enable": True,
    "plasticity.structural.enable": True,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "environment.width": 30,
    "environment.height": 30
}

for key, value in params.items():
    config.set(key, value)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Development Stage

```python
# Check development stage
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Development stages:
# - Initial: Brain just created
# - CriticalPeriod: Sensitive learning period
# - Maturation: Brain development
# - Adult: Full functionality
# - Aging: Declining performance
```

## Performance Tips

1. **For testing**: Start with 1000 steps and fewer neurons
2. **For speed**: Use larger timesteps (0.1 instead of 0.01)
3. **For learning**: Enable curiosity and reward modulation
4. **For debugging**: Add print statements to see progress

## Next Steps

1. **Read the Quick Reference**: Check `NLM_PYTHON_QUICK_REF.md` for detailed API reference
2. **Explore advanced examples**: See `Advanced_NLM_Python_Examples.md` for complex use cases
3. **Understand architecture**: Read `docs/ARCHITECTURE.md` for technical details
4. **Experiment**: Try different configurations and see what works best

## Getting Help

- **Documentation**: All documentation is in the `docs/` directory
- **Examples**: Look in the `examples/` directory
- **Quick Reference**: `NLM_PYTHON_QUICK_REF.md` has all essential commands
- **Community**: Ask questions in the NLM forum or GitHub issues

## Summary

NLM is powerful but simple to start with. Remember:

1. Always call `brain.initialize()` before use
2. Use the three-step process: get → process → make output
3. Start simple and gradually add complexity
4. Use configuration files to save your settings
5. Check documentation for specific features

You're ready to start using NLM! 🎉

For the complete reference documentation, see `HOW_TO_USE.md` (the detailed version) or `NLM_PYTHON_QUICK_REF.md` (the quick reference).
