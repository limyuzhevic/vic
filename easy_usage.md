# NLM - Getting Started: Your First Brain Simulation

**An Experimental Artificial Developmental Brain**

## Welcome to NLM!

NLM (熙然, meaning "serene flow") is a brain simulator that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it as giving a computer program a brain that:
- Reacts to its environment
- Learns from experience  
- Can remember things
- Makes decisions based on what it has learned

## Quick Start: Your First Simulation

### The Three Essential Components

Every NLM simulation needs three main parts:

1. **Brain** - The virtual brain that thinks and processes information
2. **World** - The environment where the brain lives and interacts with
3. **Agent** - The bridge that connects the brain to the world

### Complete Working Example

Copy and paste this into a Python file and run it:

```python
import pynlm

# Step 1: Create a simple brain
print("Creating brain...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Create the world environment
print("Setting up world...")
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Step 3: Connect brain to world
print("Connecting brain to world...")
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable some basic learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Step 4: Run the simulation
print("Running simulation...")
for step in range(50):
    # Update the world (pass time)
    world.update(0.1)
    
    # Get what the agent sees (sensory input)
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Let the brain think
    brain.step(step)
    
    # Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # Do the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())

print("✅ Simulation complete!")
print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
print(f"   - Neurons firing: {brain.getFiringNeuronCount()}")
print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"   - Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### What This Example Shows

This simulation demonstrates:

1. **Brain Initialization**: Creating a working brain with thousands of neurons
2. **World Creation**: Setting up a 2D environment with vision
3. **Sensory Processing**: The brain receiving visual input
4. **Motor Control**: The brain producing actions based on its internal state
5. **Learning**: The brain adapting based on rewards and curiosity

## Step-by-Step Guide

### Step 1: Import and Setup

```python
import pynlm

# Create configuration (settings for the brain)
config = pynlm.createDefaultConfig()

# Create the brain
brain = pynlm.createBrain(config)
brain.initialize()
```

**Key Points:**
- Always call `initialize()` before using the brain
- The brain needs time to set up its neural networks
- Use `createDefaultConfig()` for simple starting points

### Step 2: Create the Environment

```python
# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure the world
world.configure(
    width=10,        # World width in cells
    height=10,       # World height in cells  
    visionWidth=8,   # How much the agent can see
    visionHeight=8   # How much the agent can see
)

# Reset to initial state
world.reset()
```

**Key Points:**
- Worlds have width/height in cells
- Vision determines sensory input range
- Always call `reset()` to start fresh

### Step 3: Connect Brain to World

```python
# Create agent that bridges brain and world
agent = pynlm.createAgentBrain(brain)

# Initialize the agent with the world
agent.initialize(world)
```

**Key Points:**
- The agent handles the communication between brain and world
- It processes sensory input and produces motor commands

### Step 4: Enable Learning (Optional)

```python
# Turn on learning features
agent.enableRewardModulation(True)    # Learn from rewards
get.enableCuriosity(True)              # Explore new things
agent.enableDevelopment(True)         # Brain matures over time
agent.enableStructuralPlasticity(True) # Grow new connections
```

**Key Points:**
- These features make the brain more interesting and adaptive
- They add computational cost but enable complex behavior
- Start with just reward modulation for basic learning

### Step 5: Run the Simulation Loop

```python
# The main simulation loop
for step in range(100):
    # Update the world (pass time forward)
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Send to brain
    agent.processSensoryInput(percept)
    
    # Let brain think
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: Monitor progress
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Understanding the Components

### Brain Class (`brain`)

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization (always needed first)
brain.initialize()

# Main operations
brain.step(step)           # Process one time step
brain.receiveSensoryInput(percept)  # Get sensory input
action = brain.produceAction()      # Get motor action

# Get statistics
brain.getTotalNeuronCount()      # How many neurons
brain.getTotalSpikeCount()       # How many spikes fired
brain.getAverageFiringRate()      # Average firing rate
brain.getDevelopmentalStage()    # Current development stage
```

### World Class (`world`)

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()                    # Start over
world.update(timestep)           # Pass time

# Interaction
world.applyMotorCommand(action, time)

# Perception
percept = world.getSensoryPercept()
world.getAgentBody()             # Get agent's position
```

### Agent Class (`agent`)

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Core operations
agent.processSensoryInput(percept)    # Receive input
action = agent.decodeMotorCommand()   # Produce output
agent.applyRewardModulation(reward)   # Learn from rewards

# Learning control
agent.enableRewardModulation(True/False)
agent.enableCuriosity(True/False)
agent.enableDevelopment(True/False)
agent.enableStructuralPlasticity(True/False)

# Statistics
agent.getCuriosityLevel()           # How curious the agent is
agent.getNoveltyLevel()             # How novel the world is
agent.getPredictionError()          # Learning progress
```

## Common Patterns

### Pattern 1: Basic Simulation
Use this when you just want to see the brain working:

```python
def basic_simulation(num_steps):
    # Setup (same as above)
    # Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Pattern 2: Monitoring and Logging
Use this to track what's happening:

```python
def monitored_simulation(num_steps):
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log important metrics
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Reward: {agent.getNeuromodulationLevel():.3f}")
```

### Pattern 3: Episodic Recording
Use this to save interesting episodes:

```python
def recording_simulation(num_steps):
    episodes = []
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record episode at intervals
        if step % 100 == 0:
            episode = {
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'curiosity': agent.getCuriosityLevel(),
                'action': action
            }
            episodes.append(episode)
    
    return episodes
```

## Mini Projects

### Project 1: Silent Brain
Just watch the brain think without any interaction:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Project 2: Brain Watching the World
Let the brain observe but not act:

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

# Watch the world
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Project 3: Learning Agent
Complete simulation with learning capabilities:

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

# Run with learning
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

## Quick Reference Guide

| What You Want | Code Example | Description |
|---------------|-------------|-------------|
| Create brain | `pynlm.createBrain(config)` | Create a virtual brain |
| Start brain | `brain.initialize()` | Initialize neural networks |
| Make brain think | `brain.step(step)` | Process one time step |
| Create world | `pynlm.createSimpleWorld()` | Create environment |
| Configure world | `world.configure(width, height, ...)` | Set world parameters |
| Create agent | `pynlm.createAgentBrain(brain)` | Bridge brain to world |
| Initialize agent | `agent.initialize(world)` | Connect agent to world |
| Get brain stats | `brain.getFiringNeuronCount()` | Monitor neural activity |
| See world | `world.getSensoryPercept()` | Get sensory input |
| Get action | `agent.decodeMotorCommand()` | Get motor command |
| Apply action | `world.applyMotorCommand(action, time)` | Execute action in world |

## Tips and Tricks

### 1. Start Simple
Begin with the simplest possible example and gradually add complexity. If something doesn't work, check that you're calling `initialize()` before `step()`.

### 2. Monitor Progress
Always check brain statistics, especially firing rates. If they're too low, try increasing simulation steps or adding sensory input.

### 3. Experiment with Parameters
Try different world sizes, brain configurations, and learning settings to see what produces interesting behavior.

### 4. Save Checkpoints
Use `brain.save()` to save interesting brain states and `brain.load()` to restore them later.

### 5. Handle Errors
Always wrap simulations in try-catch blocks to handle unexpected errors gracefully.

## Troubleshooting

### "My brain isn't doing anything"
- **Solution**: Did you call `brain.initialize()`?
- **Solution**: Try running more simulation steps
- **Solution**: Check that sensory input is connected

### "The agent isn't moving"
- **Solution**: Did you call `world.applyMotorCommand()`?
- **Solution**: Make sure `world.update()` is being called
- **Solution**: Check that `agent.decodeMotorCommand()` is returning valid actions

### "Everything is 0"
- **Solution**: Brains need time to "warm up" - try more steps
- **Solution**: Some neurons need input to fire - ensure sensory input is connected

## Next Steps

When you're comfortable with the basics:

1. **Read `HOW_TO_USE.md`** - For advanced usage and API documentation
2. **Explore `docs/ARCHITECTURE.md`** - To understand the technical architecture
3. **Build from Source** - To customize and extend the system
4. **Experiment with Configurations** - Try different brain and world settings

That's it! You're now ready to use NLM. Happy brain simulation! 🧠