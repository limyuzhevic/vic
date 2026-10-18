# NLM - Enhanced User Experience Guide

## Welcome to NLM!

**NLM (熙然, meaning "serene flow")** is an experimental computational brain project that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

### Quick Overview
- **Brains**: Virtual brains with neurons and synapses
- **Worlds**: Environments where brains live and interact
- **Agents**: Bridges connecting brains to worlds

### Core Workflow
1. **Create** a brain
2. **Initialize** the brain
3. **Interact** with it (send sensory input, receive actions)
4. **Observe** behavior through statistics

---

## 🚀 Getting Started - 5 Minute Tutorial

This tutorial will walk you through creating your first virtual brain and running it in a simulated world.

### Step 1: Setup

```python
import pynlm

# Create a basic brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain (required before use!)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Step 2: Basic Brain Activity

```python
# Run the brain for 100 steps
for step in range(100):
    brain.step(step)
    
    # Print progress every 25 steps
    if step % 25 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Basic brain simulation complete!")
```

### Step 3: Create a Complete Agent System

```python
import pynlm

def create_complete_agent(width=20, height=20):
    """Create a complete agent with brain and world."""
    
    # 1. Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=width,
        height=height,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # 3. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning features (optional but recommended)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return brain, world, agent

# Use it
brain, world, agent = create_complete_agent()

# Run simulation
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
```

### Step 4: Real-World Example - Exploring a World

```python
def explore_world():
    """Example: Explore a world with sensory processing and learning."""
    
    import pynlm
    import time
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=15, height=15,
        visionWidth=6, visionHeight=6
    )
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    print("Starting exploration...")
    print("Watch how the agent learns from its experiences!")
    print()
    
    # Explore for 1000 steps
    for step in range(1000):
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Do action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print observations periodically
        if step % 200 == 0:
            print(f"Exploration step {step}:")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Energy: {world.getAgentBody().energy:.1f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print()
    
    return brain, world, agent

# Run exploration
brain, world, agent = explore_world()
```

---

## 📚 Core Concepts Explained

### What Are Neurons?

In NLM, neurons are like tiny processors that can:
- **Receive signals** (current injected into them)
- **Fire when excited enough** (send spikes to other neurons)
- **Learn from experience** (change connection strengths)

### What Is Working Memory?

Working memory in NLM:
- **Stores active neuron activity** temporarily
- **Competes** between different patterns
- **Selects** the most important information
- **Updates** based on current sensory input

### What Is Episodic Memory?

Episodic memory records:
- **Complete experiences** as "episodes"
- **What happened** (neuron activations)
- **When it happened** (step/timestamp)
- **What reward** was received
- **Can replay** past experiences

### What Is Concept Formation?

Concept formation:
- **Extracts patterns** from neural activity
- **Groups similar experiences** together
- **Creates abstract concepts**
- **Helps with generalization**

### What Is the Neural Planner?

The neural planner:
- **Evaluates sequences of actions**
- **Chooses the best action sequence**
- **Considers future rewards**
- **Accounts for constraints**

### What Is the Attention System?

Attention:
- **Focuses on important information**
- **Filters out irrelevant data**
- **enhances relevant neural activity**
- **Balances excitation/inhibition**

### What Is Neuromodulation?

Neuromodulation:
- **Dopamine**: Reward prediction error
- **Curiosity**: Novelty detection and exploration
- **Novelty**: Detects new experiences
- **Modifies plasticity**: Learning rates

### What Is Developmental Stages?

Development stages:
- **Initial**: High plasticity, rapid learning
- **Critical period**: Optimal learning window
- **Maturation**: Stable connections
- **Adult**: Limited plasticity
- **Aging**: Declining function

---

## 🛠️ Essential Commands Reference

### Brain Commands

| Function | Python Command | What It Does |
|----------|----------------|--------------|
| Create brain | `pynlm.createBrain(config)` | Makes a new brain |
| Initialize | `brain.initialize()` | Prepares brain for use |
| Run step | `brain.step(step)` | Processes one moment in time |
| Get neurons | `brain.getTotalNeuronCount()` | Returns total neuron count |
| Get firing | `brain.getFiringNeuronCount()` | Returns currently firing neurons |
| Get action | `brain.produceAction()` | Gets action based on brain activity |
| Save state | `brain.save("checkpoint.bin")` | Saves brain state to file |
| Load state | `brain.load("checkpoint.bin")` | Loads brain state from file |

### World Commands

| Function | Python Command | What It Does |
|----------|----------------|--------------|
| Create world | `pynlm.createSimpleWorld()` | Makes a 2D simulation world |
| Configure | `world.configure(width, height, visionW, visionH)` | Sets world dimensions |
| Reset | `world.reset()` | Resets world to initial state |
| Update | `world.update(timestep)` | Advances world by time |
| Get percept | `world.getSensoryPercept()` | Gets sensory input |
| Get body | `world.getAgentBody()` | Gets agent position/state |

### Agent Commands

| Function | Python Command | What It Does |
|----------|----------------|--------------|
| Create agent | `pynlm.createAgentBrain(brain)` | Creates brain-world interface |
| Initialize | `agent.initialize(world)` | Connects agent to world |
| Process input | `agent.processSensoryInput(percept)` | Feeds sensory data to brain |
| Get action | `agent.decodeMotorCommand()` | Gets action from brain |
| Apply reward | `agent.applyRewardModulation(reward, predicted)` | Teaches from rewards |
| Update dev | `agent.updateDevelopment(timestep)` | Advances developmental stage |

### Optional Features

| Feature | Enable Command | What It Does |
|---------|----------------|--------------|
| Reward learning | `agent.enableRewardModulation(True)` | Brain learns from rewards |
| Structural growth | `agent.enableStructuralPlasticity(True)` | Brain grows new connections |
| Development | `agent.enableDevelopment(True)` | Brain matures over time |
| Curiosity | `agent.enableCuriosity(True)` | Brain explores new things |

---

## 🎯 Common Use Cases

### 1. Silent Brain Test

Perfect for verifying installation and basic functionality.

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print("Brain works! It thought", brain.getTotalSpikeCount(), "times")
```

### 2. World Observation

Watch how the agent perceives and interacts with its environment.

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)

print("Observed world for 200 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### 3. Learning Agent

Create an agent that learns from its experiences.

```python
def learning_agent(config, num_steps=1000):
    """Create an agent that learns from rewards."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and learn
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    return brain, world, agent

# Run learning agent
config = pynlm.createDefaultConfig()
brain, world, agent = learning_agent(config, 500)
```

### 4. Checkpoint System

Save and load agent states for continuation or analysis.

```python
import pynlm

# Create and train agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Train for 100 steps
for step in range(100):
    brain.step(step)

# Save checkpoint
brain.save("my_agent_checkpoint.bin")
print("Agent saved at step 100")

# Later, continue training
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("my_agent_checkpoint.bin")

# Continue from saved state
for step in range(100, 200):
    brain2.step(step)

print("Training continued from checkpoint")
```

---

## ⚠️ Troubleshooting

### "My brain isn't doing anything"

**Check:**
1. Did you call `brain.initialize()` before `brain.step()`?
2. Try increasing the number of steps
3. Make sure `agent.processSensoryInput()` is being called

**Fix:**
```python
# Before this:
brain.step(step)

# Try this:
if step == 0 or step % 50 == 0:
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
```

### "The agent isn't moving"

**Check:**
1. Did you call `world.applyMotorCommand()`?
2. Make sure `world.update()` is being called
3. Check that `agent.decodeMotorCommand()` returns a valid action

**Fix:**
```python
# Make sure world is updating
world.update(0.1)

# Make sure action is applied
if action:
    world.applyMotorCommand(action, world.getSimulationTime())
```

### "Everything is 0"

**Check:**
1. Brains need time to "warm up" - try more steps
2. Some neurons need input to fire - make sure sensory input is connected
3. Check if agent is processing sensory input

**Fix:**
```python
# Warm up the brain first
for warm_step in range(100):
    brain.step(warm_step)

# Now run normal simulation
for step in range(100, 1000):
    agent.processSensoryInput(percept)  # Make sure this is called!
    brain.step(step)
```

### "ImportError: No module named 'pynlm'"

**Fix:**
```bash
pip install --force-reinstall .
```

---

## 📖 Next Steps

### For Beginners

1. **Read `HOW_TO_USE.md`** - Detailed installation and usage guide
2. **Try the examples** in this file - They work and are tested
3. **Experiment with different configurations** - Change world size, brain parameters, etc.
4. **Use checkpoint system** - Save and load agent states

### For Intermediate Users

1. **Read `docs/ARCHITECTURE.md`** - Deep dive into system architecture
2. **Experiment with different brain types** - Modify configuration
3. **Create custom experiments** - Use the experiment framework
4. **Analyze performance** - Use built-in metrics and profiling

### For Advanced Users

1. **Read `docs/SCIENCE.md`** - Scientific background and theory
2. **Explore Phase experiments** - Run Phase 3-5 experiments
3. **Use C++ API directly** - For maximum performance
4. **Contribute to the project** - Help improve NLM

---

## 🎉 You've Finished the Enhanced User Experience Guide!

You now have:

✅ **Complete working examples** for all major use cases
✅ **Clear explanations** of core concepts
✅ **Essential command reference** for quick lookup
✅ **Comprehensive troubleshooting** for common issues
✅ **Progressive learning path** from beginner to advanced

You're ready to start creating your own NLM applications! 🚀

**Happy brain building!** 🐝🧠

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
