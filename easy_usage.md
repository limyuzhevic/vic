# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

### Example 3: Advanced Agent with Learning

```python
import pynlm

# Setup with custom configuration
config = pynlm.createDefaultConfig()

# Configure for learning
config.set("plasticity.stdp.enable", True)      # Enable STDP learning
config.set("plasticity.hebbian.enable", True)    # Enable Hebbian learning
config.set("plasticity.structural.enable", True) # Enable structural plasticity
config.set("neuromod.curiosity.enable", True)    # Enable curiosity-driven exploration
config.set("neuromod.novelty.enable", True)      # Enable novelty detection

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()

# Configure world with larger map
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()
world.setRandomSeed(42)  # For reproducible experiments

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Advanced simulation with learning
for step in range(500):
    # Update environment
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # Brain thinks and learns
    brain.step(step)
    
    # Get action from brain's motor system
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward for successful actions
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development system
    agent.updateDevelopment(0.1)
    
    # Print advanced statistics every 50 steps
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")

print("Advanced agent simulation complete!")
```

### Example 4: Multiple Agents Competition

```python
import pynlm

# Create environment
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()

# Create multiple brains for competition
brains = []
agents = []
configs = []

for i in range(3):
    config = pynlm.createDefaultConfig()
    # Each brain has slightly different parameters
    config.set("brain.neuron_count", 500 + i * 100)
    config.set("random_seed", 42 + i)
    
    brains.append(pynlm.createBrain(config))
    brains[-1].initialize()
    
    agents.append(pynlm.createAgentBrain(brains[-1]))
    agents[-1].initialize(world)
    
    configs.append(config)

# Competition simulation
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Each agent processes the same input but may respond differently
    for agent_idx, agent in enumerate(agents):
        agent.processSensoryInput(percept)
        brain_step = step + agent_idx  # Offset steps for different behaviors
        agents[agent_idx].getBrain()->step(brain_step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Each agent learns from its own experience
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Reset world for next round
    world.reset()
    
    # Print competition status
    if step % 50 == 0:
        print(f"Competition step {step}:")
        for i, agent in enumerate(agents):
            print(f"  Agent {i+1}: {agent.getBrain()->getFiringNeuronCount()} firing, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"spikes={agent.getBrain()->getTotalSpikeCount()}")

print("Competition simulation complete!")
```

### Example 5: Learning and Memory Experiment

```python
import pynlm
import numpy as np

# Setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world with resources
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

# Place resources in world
world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, value=5.0))
world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, value=-5.0))

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Memory experiment: track exploration vs exploitation
exploration_history = []
exploitation_history = []
memory_strength_history = []

for step in range(300):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Record curiosity (exploration tendency)
    curiosity = agent.getCuriosityLevel()
    exploration_history.append(curiosity)
    
    # Record memory indicators (synaptic strength)
    if brain.getTotalSynapseCount() > 0:
        memory_strength = brain.getAverageFiringRate() * brain.getTotalSpikeCount()
        memory_strength_history.append(memory_strength)
    
    # Agent acts
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward from world interaction
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    
    # Apply reward modulation (learning)
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Record exploitation (goal-directed behavior)
    exploitation = 1.0 - curiosity
    exploitation_history.append(exploitation)

print("Memory experiment complete!")
print(f"Final curiosity (exploration): {np.mean(exploration_history):.3f}")
print(f"Final exploitation tendency: {np.mean(exploitation_history):.3f}")
print(f"Average memory strength: {np.mean(memory_strength_history):.3f}")
print(f"Total learning: {brain.getTotalSpikeCount()} spikes recorded")
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

### Project 3: Complete Agent with Advanced Learning

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

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent with detailed monitoring
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

## Advanced Features

### Additional Configuration Options

```python
# Advanced brain configuration
config.set("brain.neuron_count", 2000)                    # More neurons for complexity
config.set("brain.synapse_density", 0.2)                   # More connections
config.set("plasticity.stdp.learning_rate", 0.01)         # Faster learning
config.set("plasticity.stdp.tau_plus", 30.0)              # Longer potentiation
config.set("plasticity.stdp.tau_minus", 30.0)             # Longer depression
config.set("neuromod.curiosity.scale", 2.0)                # Stronger curiosity
config.set("neuromod.novelty.threshold", 0.1)              # More sensitive to novelty
```

### Advanced Agent Methods

```python
# Get detailed brain statistics
print(f"Brain regions: {brain.getRegionCount()}")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Get agent's internal state
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Developmental stage: {agent.getDevelopmentalStage()}")

# Check which subsystems are enabled
print(f"Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"Development: {agent.isDevelopmentEnabled()}")
print(f"Curiosity: {agent.isCuriosityEnabled()}")
```

### Multi-Agent Configuration Helper

```python
# Create specialized agents for different tasks
def create_explorer_agent(world):
    """Agent optimized for exploration and discovery"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("neuromod.curiosity.scale", 3.0)  # High curiosity
    config.set("neuromod.novelty.threshold", 0.05)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    agent.enableRewardModulation(True)
    return agent

def create_exploiter_agent(world):
    """Agent optimized for efficient goal pursuit"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("neuromod.curiosity.scale", 0.5)  # Low curiosity
    config.set("neuromod.novelty.threshold", 0.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(False)
    agent.enableNovelty(False)
    agent.enableRewardModulation(True)
    return agent
```

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

**"My learning isn't working"**
- Check that `agent.enableRewardModulation(True)` is called
- Verify reward values are being calculated correctly
- Ensure the brain has enough time to learn

**"My simulation is too slow"**
- Reduce the number of neurons in config
- Decrease simulation time steps
- Disable structural plasticity if not needed
- Reduce curiosity levels to limit exploration

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Add custom neuron types and plasticity rules
5. Create multi-agent environments
6. Implement reinforcement learning scenarios
7. Build complex task-solving behaviors

That's it! You're now ready to use NLM with advanced capabilities for both beginners and expert users.