# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, "serene flow") is a **brain simulator** that creates virtual brains capable of learning, remembering, and making decisions through neural dynamics.

### Key Concepts

**Neurons** - Brain cells that send electrical signals to each other through **synapses** (connections)

**Learning** - Synapses strengthen or weaken based on experience (Hebbian learning, STDP)

**Memory** - Patterns stored in neural networks (working, episodic, associative memory)

**Development** - Brains mature through developmental stages from Initial to Aging

## Getting Started: Your Brain Components

NLM uses three main components to simulate intelligent behavior:

1. **Brain** (`pynlm.createBrain()`) - The neural network that thinks and processes information
2. **World** (`pynlm.createSimpleWorld()`) - The environment with agents, objects, and rules
3. **Agent** (`pynlm.createAgentBrain(brain)`) - The interface connecting brain to world

## Quick Start: Two Essential Examples

### Example 1: Silent Brain (Basic Testing)

This creates a brain and runs it without any environment:

```python
import pynlm

# Create and initialize the brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for 100 simulation steps
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
print(f"Brain stats: {brain.getFiringNeuronCount()} neurons firing")
```

### Example 2: Complete Agent (Brain in World)

This demonstrates the full brain-world interaction loop:

```python
import pynlm

# Setup brain and environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features (optional but recommended)
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)        # Explore new things
agent.enableDevelopment(True)      # Brain matures over time

# Run the complete simulation loop
for step in range(200):
    # Update environment
    world.update(0.1)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process input in brain
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Agent simulation complete!")
print(f"Development stage: {brain.getDevelopmentalStage()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

## The Core Loop: How NLM Works

Think of NLM as a continuous loop where a brain interacts with its world:

```
[World]  →  [Sensory Input]  →  [Brain Processing]  →  [Motor Output]  →  [World Update]
     ↑                                                        ↓
 [Rewards/Errors] ← [Neuromodulation] ← [Learning/P plastic] ← [Development]
```

### Each Step in Detail:

1. **World Update** (`world.update(timestep)`) - Advances time, updates agent position, processes events
2. **Sensory Input** (`world.getSensoryPercept()`) - What the agent sees/hears/feels
3. **Brain Processing** (`agent.processSensoryInput(percept)`) - Brain receives and encodes input
4. **Brain Computation** (`brain.step(step)`) - Neural dynamics, spikes, learning
5. **Motor Output** (`agent.decodeMotorCommand()`) - Brain decides what to do
6. **World Action** (`world.applyMotorCommand()`) - Agent acts in the world

## Monitoring Your Brain

Add these stats to understand what's happening:

```python
# Inside your simulation loop:
print(f"Step {step}: {brain.getFiringNeuronCount()} neurons active")
print(f"Total spikes so far: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

## Configuration Options

### Simple Setup (Everything Works)

```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)  # Uses sensible defaults
```

### Advanced Configuration

```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)           # More neurons
config.set("brain.synapse_density", 0.05)        # More connections
config.set("plasticity.stdp.enable", True)       # STDP learning
config.set("neuromod.dopamine.scale", 1.0)      # Reward strength
config.set("neuromod.curiosity.enable", True)   # Exploration

brain = pynlm.createBrain(config)
```

## Common Use Cases

### 1. Testing Neural Dynamics

```python
# Quick test of basic brain functionality
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Inject some current to make neurons fire
brain.injectCurrentToNeurons("Excitatory", 1.0)

for i in range(50):
    brain.step(i)

print(f"Test complete. Firing neurons: {brain.getFiringNeuronCount()}")
```

### 2. Agent Training

```python
# Train an agent to navigate a world
def train_agent(world_size=30, episodes=100):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for episode in range(episodes):
        world.reset()
        agent.reset()
        
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain, agent
```

### 3. Development Studies

```python
# Study how brains develop over time
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for age in range(100):
    agent = pynlm.createAgentBrain(brain)
    agent.updateDevelopment(1.0)  # Age by 1 developmental unit
    
    print(f"Age {age}: Stage {brain.getDevelopmentalStage()}")
    print(f"  Plasticity: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
```

## Troubleshooting

### "My brain isn't doing anything"

**Check these issues:**
- Did you call `brain.initialize()` before `brain.step()`?
- Are neurons receiving input? Try injecting current: `brain.injectCurrentToNeurons("Excitatory", 1.0)`
- Did you call `agent.processSensoryInput()`?

### "The agent isn't moving"

**Check these issues:**
- Is `world.applyMotorCommand()` being called?
- Did you call `world.update()` to advance the simulation?
- Does the agent have energy? Use `world.setMaxEnergy()` to start

### "Everything is 0"

**Brains need time to "warm up":**
- Try more simulation steps (100+)
- Add sensory input to activate neurons
- Check that connections exist (neuron count > 0)

## Next Steps

When you're ready to dive deeper:

1. **Read `HOW_TO_USE.md`** - Technical details and advanced features
2. **Check `docs/ARCHITECTURE.md`** - Deep dive into NLM's design
3. **Try `nlm_phase6_demo`** - See all systems working together
4. **Experiment with configurations** - Change neuron counts, learning rates, etc.

That's it! You're now ready to create and experiment with virtual brains using NLM.

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

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

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
