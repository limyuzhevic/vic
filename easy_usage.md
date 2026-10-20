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

### Example 1a: Advanced Brain Creation with Configuration

```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)    # Custom number of neurons
config.set("brain.connection_probability", 0.05)  # Connectivity

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Custom brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Example 1b: Brain Statistics and Monitoring

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    
    # Monitor brain state every 10 steps
    if step % 10 == 0:
        stats = brain.getBrainStats()
        print(f"Step {step}:")
        print(f"  Firing neurons: {stats['firing_neurons']}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
        print(f"  E/I ratio: {stats['e_i_ratio']:.2f}")
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

### Example 2a: Complete Agent with Learning

```python
import pynlm

# Setup with learning capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Run enhanced agent simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

print("Enhanced agent simulation complete!")
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

### Pattern 1a: Enhanced Simulation with Learning

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning based on environment
        reward = extract_reward(world)  # Custom reward function
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)

# Custom reward function example
def extract_reward(world):
    percept = world.getSensoryPercept()
    
    # Check for positive rewards (resources, goals, etc.)
    if percept and percept.getInternal():
        return percept.getInternal()[0]  # First internal signal is reward
    
    return 0.0  # Default no reward
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
stats = brain.getBrainStats()
print("Neurons firing:", stats['firing_neurons'])
print("Total spikes:", stats['total_spikes'])
print("Avg firing rate:", stats['avg_firing_rate'])
print("Development stage:", stats['dev_stage'])

agent_stats = agent.getAgentStats()
print("Curiosity:", agent_stats['curiosity'])
print("Novelty:", agent_stats['novelty'])
print("Neuromodulation:", agent_stats['neuromodulation'])
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Configuration customization
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)           # More neurons
config.set("brain.connection_probability", 0.05) # Connectivity
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

### Project 3: Complete Agent with Learning

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

### Project 4: Advanced Configuration and Monitoring

```python
import pynlm

# Advanced configuration with custom parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 750)           # Custom neuron count
config.set("brain.connection_probability", 0.08)  # More connections
config.set("plasticity.stdp.enable", True)     # Enable STDP
config.set("plasticity.hebbian.enable", True)   # Enable Hebbian learning

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world with enhanced settings
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Enhanced simulation with detailed monitoring
print("Running enhanced agent simulation...")
print("Initial brain:", brain.getBrainStats())

for step in range(200):
    # Update environment
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Action and environment interaction
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learning and development
    reward = extract_custom_reward(world)
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    # Detailed monitoring every 50 steps
    if step % 50 == 0:
        brain_stats = brain.getBrainStats()
        agent_stats = agent.getAgentStats()
        
        print(f"\n--- Step {step} ---")
        print(f"Brain: {brain_stats['firing_neurons']} firing, "
              f"{brain_stats['total_spikes']} total spikes")
        print(f"Agent: Curiosity={agent_stats['curiosity']:.3f}, "
              f"Novelty={agent_stats['novelty']:.3f}, "
              f"Dev={agent_stats['dev_stage']}")
        print(f"Environment: Time={world.getSimulationTime():.1f}s")

print("\nEnhanced simulation complete!")
print("Final brain state:", brain.getBrainStats())
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
| Get brain stats | `brain.getBrainStats()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if your world has sensory input

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent has motor capabilities

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check your configuration parameters

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Covers building the C++ project
   - Shows advanced Python usage
   - Documents all configuration options

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - System architecture overview
   - Component interactions
   - Design patterns

3. **Experiment with different configurations!**
   - Try different neuron counts
   - Test various connection probabilities
   - Experiment with plasticity settings
   - Compare learning algorithms

4. **Explore the advanced features**
   - Callbacks for monitoring
   - Performance benchmarking
   - Integration with Python scientific libraries

5. **Create your own neural network applications**
   - Build custom reward functions
   - Implement specific learning strategies
   - Create specialized environments

That's it! You're now ready to use NLM with the enhanced Python bindings featuring:
- Comprehensive error handling
- Advanced monitoring and statistics
- Flexible configuration options
- Enhanced learning capabilities
- Better integration with Python workflows

**NEW FEATURES IN THIS VERSION:**
- `brain.getBrainStats()` - Comprehensive brain statistics
- `agent.getAgentStats()` - Detailed agent monitoring
- Enhanced parameter validation
- Better error messages and debugging support
- Support for advanced learning configurations
