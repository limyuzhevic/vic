# NLM User Guide - Beginner

## Welcome to NLM!

This guide is designed for people who are new to NLM (Neural Learning Machine). It covers the basics and gets you running a simple simulation in just a few minutes.

## What is NLM?

Think of NLM as a **virtual brain** that can learn and adapt. Just like your brain:
- It has **neurons** that send signals
- It has **connections** between neurons
- It can **learn** from experience
- It can **remember** things
- It can make **decisions**

## Quick Start (Copy & Paste)

Here's a complete, working example that you can run immediately:

```python
import pynlm

# Step 1: Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

That's it! You now have a working neural brain that has simulated 100 steps of thinking.

## Understanding the Basics

### The Three Main Parts

NLM has three main components that work together:

1. **Brain** - The thinking part (neurons and connections)
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

### Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns | `agent.applyRewardModulation(reward, predicted)` |

## Your First Complete Example

This example shows a brain exploring a simple world:

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

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)  # Update world
    agent.processSensoryInput(world.getSensoryPercept())  # Brain sees it
    brain.step(i)  # Brain thinks
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

## Common Operations

### 1. Creating and Running a Brain

```python
# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Start it up
brain.initialize()

# Run it
for step in range(1000):
    brain.step(step)
```

### 2. Adding a World (Environment)

```python
# Create world
world = pynlm.createSimpleWorld()

# Set up world size and what the agent can see
world.configure(
    width=20,           # World is 20x20 cells
    height=20,
    visionWidth=8,      # Agent can see 8x8 cells
    visionHeight=8
)

# Reset world to starting state
world.reset()

# Set where agent starts
world.setAgentStart(10.0, 10.0)
```

### 3. Connecting Brain and World

```python
# Create agent (connection between brain and world)
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)

# Enable some basic learning (optional but recommended)
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)         # Explore new things
```

### 4. Running Complete Simulation

```python
# Complete simulation loop
for step in range(500):
    # 1. Update world (time passes)
    world.update(0.1)
    
    # 2. Get what agent sees
    percept = world.getSensoryPercept()
    
    # 3. Tell brain what it sees
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks/decides
    brain.step(step)
    
    # 5. Get action from brain
    action = agent.decodeMotorCommand()
    
    # 6. Do the action in world
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Getting to Know Your Brain

### Basic Statistics

You can always ask your brain how it's doing:

```python
print("Neurons in brain:", brain.getTotalNeuronCount())
print("Connections:", brain.getTotalSynapseCount())
print("Currently firing:", brain.getFiringNeuronCount())
print("Average firing rate:", brain.getAverageFiringRate())
print("Total spikes so far:", brain.getTotalSpikeCount())
```

### Seeing What the Agent Sees

```python
percept = world.getSensoryPercept()

# Get visual data (if available)
if hasattr(percept, 'getVision'):
    vision_data = percept.getVision()
    print(f"Vision size: {len(vision_data)} pixels")
    
# Get internal signals
internal = percept.getInternal()
print(f"Internal signals: {len(internal)}")
```

## Simple Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print("Firing neurons:", brain.getFiringNeuronCount())
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
print("Final curiosity level:", agent.getCuriosityLevel())
```

## Tips and Tricks

### Brains Need Time to Warm Up

```python
# Don't expect much at first
for step in range(100):
    brain.step(step)
    
print("Don't be surprised if firing is low at first!")
print("Brains need time to develop connections.")
```

### Common Mistakes to Avoid

**Mistake 1: Forgetting to initialize**
```python
# WRONG - Don't do this!
brain = pynlm.createBrain(config)
brain.step(0)  # Crashes! Brain not initialized

# RIGHT - Always initialize first
brain = pynlm.createBrain(config)
brain.initialize()  # Required!
brain.step(0)       # Now works fine
```

**Mistake 2: Using uninitialized variables**
```python
# WRONG - Using undefined variables
for step in range(100):
    brain.step(step)  # brain might not be defined

# RIGHT - Define everything clearly
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(100):
    brain.step(step)
```

### Debugging Tips

**If your brain isn't doing anything:**
- Did you call `brain.initialize()`?
- Try running more steps
- Check if there are enough neurons

**If the agent isn't moving:**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**If everything is zero:**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

## What Next?

When you're ready to learn more:

1. **Read the HOW_TO_USE.md** - More detailed explanations
2. **Read docs/ARCHITECTURE.md** - How everything fits together
3. **Try different configurations** - Change the settings in the examples
4. **Look at the example scripts** - See more complex examples

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Get brain stats | `brain.getFiringNeuronCount()` |

## Understanding the Learning Features

### What These Do

```python
# Enable these to make your brain smarter
agent.enableRewardModulation(True)      # Learn from rewards and punishments
agent.enableStructuralPlasticity(True)  # Can grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explores new experiences
```

### How They Help

- **Reward Modulation**: Makes the brain remember what works well
- **Structural Plasticity**: Allows the brain to create new connections
- **Development**: The brain gets more capable over time
- **Curiosity**: Makes the brain want to explore new things

## Final Tips

1. **Start simple**: Use the examples as templates
2. **Add features gradually**: Start with just a brain, then add world, then agent
3. **Print progress**: Use the print statements in examples to see what's happening
4. **Experiment**: Try changing numbers in the examples
5. **Don't rush**: Give brains time to develop

You now have everything you need to start using NLM! The examples are designed to work as-is, so you can get up and running quickly. Enjoy exploring the virtual brain world!

When you're ready for more advanced features:
- Check out `nlm_examples.py` for more complex examples
- Look at `nlm_cli.py` for command-line interface
- Read `HOW_TO_USE.md` for installation and setup
- Explore `docs/` for technical documentation

Happy simulating!
