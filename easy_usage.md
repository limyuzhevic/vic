# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然) is an experimental artificial developmental brain system. It's a neural simulation framework that creates virtual brains capable of learning, remembering, and making decisions through interaction with an environment.

**What makes NLM special:**
- Brain-inspired architecture with real spiking neurons
- Event-driven dynamics and synaptic plasticity
- Multiple learning mechanisms (STDP, Hebbian, structural)
- Neuromodulation for reward, curiosity, and novelty
- Memory systems (working, episodic, associative)

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Create a default configuration
config = pynlm.createDefaultConfig()

# Step 2: Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Step 3: Run simulation for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent that connects brain to world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation loop
for step in range(50):
    # Update environment
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input in brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get motor action
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

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

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createDefaultConfig()`
Creates a configuration with sensible defaults for the brain.

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses based on the configuration.

### `brain.initialize()`
Sets up the brain with initial conditions and connections.

### `brain.step(step_number)`
Runs one simulation step: neurons process inputs, propagate spikes, update synapses.

### `world.update(time)`
Advances the simulation environment by the specified time.

### `world.getSensoryPercept()`
Gets the current sensory information (vision, touch, etc.) from the world.

### `pynlm.createSimpleWorld()`
Creates a simple 2D world for simulation.

### `world.configure(width, height, visionWidth, visionHeight)`
Sets up world dimensions and agent sensing capabilities.

### `pynlm.createAgentBrain(brain)`
Creates an agent interface that connects brain to world for sensory processing and motor actions.

### `agent.initialize(world)`
Sets up the agent to work with the world.

### `agent.processSensoryInput(percept)`
Passes sensory information to the brain for processing.

### `agent.decodeMotorCommand()`
Reads brain activity and produces motor commands for the world.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

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

### Project 3: Complete Agent

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

---

## Quick Reference

### Core Functions

| What you want | Code |
|--------------|------|
| Create config | `config = pynlm.createDefaultConfig()` |
| Create brain | `brain = pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Configure world | `world.configure(width, height, visionWidth, visionHeight)` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()`, `brain.getTotalSpikeCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

### Configuration Keys

| Key | Type | Default | Description |
|----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |

---

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? Always call it before using the brain!
- Try increasing the number of steps to let the brain "warm up"
- Check if neurons have enough current to fire

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`? Actions need to be applied to the world
- Make sure `world.update()` is being called in your simulation loop
- Check if the agent's motor neurons are active

**"Everything is 0"**
- Brains need time to "warm up" - try more steps (100+ recommended)
- Some neurons need input to fire - make sure sensory input is connected
- Check if you're injecting current into sensory neurons

**"Python import error"**
- Make sure you have all dependencies installed: `pip install pynlm`
- The package might not be built yet - run `pip install .` from the project root

**"Configuration errors"**
- Use `pynlm.createDefaultConfig()` for a working starting point
- Check that configuration keys are valid (no spaces, use dots for nesting)

### Debug Tips

```python
# Add debugging to your simulation loop:
print(f"Step {step}:")
print(f"  Sensory input size: {len(percept.getVision())}")
print(f"  Active neurons: {brain.getActiveNeuronCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Synaptic connections: {brain.getTotalSynapseCount()}")

# Check agent status:
print(f"  Agent curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Agent novelty: {agent.getNoveltyLevel():.3f}")
print(f"  Agent prediction error: {agent.getPredictionError():.3f}")
```

---

## Next Steps

### For Beginners:
1. **Read `HOW_TO_USE.md` for more details** - Comprehensive C++ build instructions and Python API reference
2. **Try the example projects** in this guide to get hands-on experience
3. **Experiment with different configurations** to see how they affect brain behavior

### For Intermediate Users:
1. **Understand the brain architecture** - Read `docs/ARCHITECTURE.md`
2. **Learn about scientific principles** - Read `docs/SCIENCE.md`
3. **Explore experiment examples** - Check out `docs/EXPERIMENTS.md`
4. **Configure advanced settings** - Modify config files for specific use cases

### For Advanced Users:
1. **Study the source code** in the `src/` directory
2. **Implement custom learning rules** in the plasticity modules
3. **Create custom world environments** or add new sensory modalities
4. **Contribute to the project** - Report issues and suggest improvements

### Configuration Examples

```python
import pynlm

# Basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # More neurons
config.set("brain.synapse_density", 0.2)  # More connections
config.set("plasticity.stdp.enable", True)  # Enable STDP
config.set("plasticity.hebbian.enable", True)  # Enable Hebbian

brain = pynlm.createBrain(config)
```

That's it! You're now ready to use NLM. The guide covers everything from getting started to advanced usage.

**Need help?** Check the troubleshooting section or explore the documentation in the `docs/` folder for deeper insights into the NLM system.
