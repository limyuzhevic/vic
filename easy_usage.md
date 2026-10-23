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

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
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

### Example 3: Complete Agent with Learning

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

## New Advanced Features!

### Episode-based Simulation

```python
# Run a complete simulation episode and get results
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent and initialize
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation episode
results = pynlm.runEpisode(brain, world, [0.5, 0.8, 0.3, 0.6, 0.7, 0.9, 0.4], 50)

total_spikes, avg_firing_rate, developmental_stage = results
print(f"Simulation complete: {total_spikes} spikes, avg firing: {avg_firing_rate:.3f}, dev stage: {developmental_stage}")
```

### Network Analysis

```python
# Analyze your brain's network structure
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a few steps first
for i in range(10):
    brain.step(i)

# Analyze network
network_stats = pynlm.analyzeNetworkStructure(brain)
connection_density, ei_ratio, clustering, path_length, neurons, synapses = network_stats

print(f"Network Analysis:")
print(f"  Connection density: {connection_density:.4f}")
print(f"  E/I ratio: {ei_ratio:.4f}")
print(f"  Clustering coefficient: {clustering:.4f}")
print(f"  Characteristic path length: {path_length:.4f}")
print(f"  Neurons: {neurons}, Synapses: {synapses}")
```

### Performance Monitoring

```python
# Profile your simulation for performance
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run performance test
performance = pynlm.profileSimulation(brain, 1000)
elapsed_time, avg_spike_rate, neuron_count, synapse_count, current_firing, current_active = performance

print(f"Performance Profile:")
print(f"  Time elapsed: {elapsed_time} ms")
print(f"  Average spike rate: {avg_spike_rate} spikes/step")
print(f"  Neuron count: {neuron_count}")
print(f"  Synapse count: {synapse_count}")
print(f"  Current firing neurons: {current_firing}")
print(f"  Current active neurons: {current_active}")
```

### Advanced Configuration

```python
# Configure brain for specific learning scenarios
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure for reward-based learning
advanced_config = pynlm.configureForLearning(brain, True, True, True, True, 0.01)

print("Brain configured for advanced learning with reward, curiosity, development, and structural plasticity")
```

### Custom Training

```python
# Train your brain with custom reward episodes
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Create training episodes
training_episodes = [
    ([0.1, 0.2, 0.3, 0.4, 0.5], 1.0),  # sensory_pattern, target_reward
    ([0.6, 0.7, 0.8, 0.9, 1.0], 0.5),
    ([0.0, 0.1, 0.2, 0.3, 0.4], 0.8),
]

# Train the brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

training_stats = pynlm.trainWithReward(brain, world, training_episodes, 100)
rewards, spikes, avg_reward, avg_spikes = training_stats

print(f"Training completed!")
print(f"  Average reward: {avg_reward:.3f}")
print(f"  Average spikes: {avg_spikes:.1f}")
print(f"  Episode rewards: {[f'{r:.2f}' for r in rewards]}")
```

### Export Network Data

```python
# Export network information to file
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation first
for i in range(20):
    brain.step(i)

# Export network to file
pynlm.exportNetworkGraph(brain, "network_info.txt")

print("Network exported to network_info.txt")
```

---

## Best Practices

### 1. Always Initialize

```python
brain = pynlm.createBrain(config)
brain.initialize()  # Don't forget this!
```

### 2. Use Appropriate Step Sizes

```python
# Smaller timesteps for better accuracy
for step in range(1000):
    brain.step(step, timestep=0.001)

# Larger timesteps for speed
for step in range(1000):
    brain.step(step, timestep=0.01)
```

### 3. Monitor Learning

```python
# Track learning progress
print(f"Step {step}:")
print(f"  Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"  Development stage: {brain.getDevelopmentalStage()}")
print(f"  Network E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### 4. Reset Between Episodes

```python
# Always reset before new episodes
brain.reset()
world.reset()
agent.reset()
```

### 5. Validate Configuration

```python
# Check your configuration before running
errors = pynlm.validateConfig(config)
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid!")
```

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that learning subsystems are enabled

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that configuration parameters are reasonable

---

## Next Steps

When you're comfortable:
1. **Read `HOW_TO_USE.md` for more detailed documentation**
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!**
4. **Try the advanced features and training examples**

That's it! You're now ready to use NLM with both basic and advanced features.

Enjoy experimenting with your virtual brain!
