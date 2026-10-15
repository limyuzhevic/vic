# Getting Started with NLM - Complete Tutorial for Beginners

This tutorial provides a comprehensive introduction to the NLM (Neural Learning Machine) Python bindings for complete beginners. It covers all the essential concepts and provides step-by-step examples.

## What is NLM?

NLM is a **brain simulator** for computers that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

**Key Concepts:**

| Concept | Description | NLM Equivalent |
|---------|-------------|----------------|
| Brain cells (neurons) | Send signals to each other | `Brain` class |
| Connections between cells | Get stronger or weaker based on experience | `Synapses` and `Plasticity` |
| Remembering patterns | Store experiences and recall them later | `Memory` systems |
| Making decisions | Choose actions based on what's seen | `AgentBrain` and `Actions` |
| Living in an environment | Interact with surroundings | `SimpleWorld` |

Think of it like this: Your brain is the **NLM Brain**, your body is the **NLM World**, and your consciousness is the **NLM Agent**. You see things (sensory input), think about them (neural processing), decide what to do (action selection), and do it (motor output). Your brain learns from the results (reward/prediction error) and changes for next time (plasticity).

---

## What You'll Learn

By the end of this tutorial, you will be able to:

1. ✅ Install and set up the NLM Python bindings
2. ✅ Create a virtual brain with neurons
3. ✅ Set up a simulated world environment
4. ✅ Create an intelligent agent that can think and act
5. ✅ Run a complete simulation with learning
6. ✅ Understand the basic brain-world-agent loop

---

## Setup and Installation

### Prerequisites

You need Python 3.8 or higher:

```bash
python3 --version
# Should show something like "Python 3.8.10"
```

### Installing NLM Python Bindings

**Option 1: Using PyProject.toml (Recommended)**

```bash
cd /workspace/app
pip install -e .  # Install in development mode
```

**Option 2: Using CMake Directly**

```bash
cd /workspace/app
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3
make -j$(nproc)
make install
```

### Verify Installation

```python
import pynlm as nlm
print("✓ NLM Python bindings installed successfully")
print(f"Available classes: Config, Brain, AgentBrain, SimpleWorld, etc.")
```

---

## Part 1: Your First Brain

Let's start with the simplest possible example - creating a brain and making it think.

### The Simplest Possible Example

```python
import pynlm as nlm

print("=== Your First Brain ===\n")

# One line to create a virtual brain with default settings
brain = nlm.createBrain(nlm.createDefaultConfig())

# Initialize it (turns it on)
brain.initialize()

# Make it think for one moment
brain.step(0)

# See how many neurons your brain has
neuron_count = brain.getTotalNeuronCount()
spike_count = brain.getTotalSpikeCount()

print(f"✅ Your brain has {neuron_count} neurons!")
print(f"✅ Your brain fired {spike_count} times!")
print("🎉 Success! You've created your first virtual brain!")
```

**What happened in this example:**

1. `import pynlm as nlm` - Import the library
2. `nlm.createBrain(nlm.createDefaultConfig())` - Create a brain with default settings
3. `brain.initialize()` - Start up the brain (essential step!)
4. `brain.step(0)` - Make the brain process one moment in time
5. Check brain statistics

---

## Part 2: Brain in a Simple World

Now let's add a world where the brain lives and interacts with the environment.

### Step-by-Step: Brain Watching a World

```python
import pynlm as nlm

print("\n=== Brain in a Simple World ===\n")

# Setup
print("1. Setting up the simulation...")
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

print("✅ Simulation setup complete!")
print(f"   World size: {world.getWidth()} x {world.getHeight()} meters")
print(f"   Agent vision: {world.getVisionWidth()} x {world.getVisionHeight()} pixels")

# Watch the world for 30 steps
print("\n2. Watching the world...")
for i in range(30):
    world.update(0.1)  # Update world by 0.1 seconds
    
    # What does the agent see?
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(i)
    
    # Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # Do the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Show progress
    if (i + 1) % 10 == 0:
        firing_rate = brain.getAverageFiringRate()
        print(f"   Step {i + 1}: {firing_rate:.1f} Hz firing rate")

print(f"\n✅ Simulation finished!")
print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
print(f"   Total spikes: {brain.getTotalSpikeCount()}")
print(f"   Average firing rate: {brain.getAverageFiringRate():.1f} Hz")
```

**What happened in this example:**

1. Create brain, world, and agent
2. Configure the world (size, vision)
3. Initialize the agent in the world
4. Run simulation loop:
   - Update world state
   - Get sensory percept (what agent sees)
   - Tell brain what it sees
   - Brain thinks (processes information)
   - Brain decides what to do
   - Do action in world
5. Collect statistics

---

## Part 3: Complete Agent with Learning

Now let's create a complete agent that can learn from experience using the full brain-world-agent loop.

### Complete Agent Simulation

```python
import pynlm as nlm

print("\n=== Complete Agent with Learning ===\n")

# Setup
print("1. Setting up complete agent...")
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)         # Explore new things
agent.enableStructuralPlasticity(True)  # Grow new connections

print("✅ Agent setup complete!")
print("   Learning features enabled:")
print("   - Reward modulation: YES")
print("   - Curiosity: YES")
print("   - Structural plasticity: YES")

# Run agent
print("\n2. Running agent simulation...")
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Show progress every 20 steps
    if step % 20 == 0:
        firing_rate = brain.getFiringNeuronCount()
        print(f"   Step {step}: {firing_rate} neurons firing")

print(f"\n✅ Agent simulation complete!")
print(f"   Total steps: {step + 1}")
print(f"   Final firing rate: {brain.getAverageFiringRate():.1f} Hz")
print(f"   Development stage: {brain.getDevelopmentalStage()}")
```

**What happened in this example:**

1. Create complete setup with config, brain, world, and agent
2. Enable learning features (neuromodulation, curiosity, plasticity)
3. Run full simulation loop
4. Show progress at intervals

---

## Part 4: Understanding Each Component

Let's break down what each component does in the brain-world-agent loop.

### The Brain-World-Agent Analogy

| Real World | NLM Code | What it does |
|------------|----------|--------------|
| You see something | `percept = world.getSensoryPercept()` | Get sensory input |
| Your brain processes it | `agent.processSensoryInput(percept)` | Send to brain |
| You decide to act | `action = agent.decodeMotorCommand()` | Choose action |
| You do the action | `world.applyMotorCommand(action, time)` | Execute action |
| Your brain learns | `agent.applyRewardModulation(reward, predicted)` | Learn from results |

### Common Patterns

#### Pattern 1: Run a Simulation

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

#### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

#### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## Part 5: Mini Projects

Let's try some mini projects to practice what you've learned.

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm as nlm

print("\n=== Project 1: Silent Brain ===\n")

brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

print("Running silent brain test...")
for i in range(10):
    brain.step(i)

print("✅ Silent brain test done!")
print(f"   Total neurons: {brain.getTotalNeuronCount()}")
print(f"   Total spikes: {brain.getTotalSpikeCount()}")
```

### Project 2: Brain Watching a World

```python
import pynlm as nlm

print("\n=== Project 2: Brain Watching a World ===\n")

# Setup
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
print("Watching world for 30 steps...")
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print("✅ Watched world for 30 steps")
print(f"   Firing rate: {brain.getAverageFiringRate():.1f} Hz")
```

### Project 3: Complete Agent

```python
import pynlm as nlm

print("\n=== Project 3: Complete Agent ===\n")

# Setup
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
print("Running complete agent simulation...")
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("✅ Agent simulation complete!")
```

---

## Part 6: Quick Reference

Here's a quick reference of the most common commands:

| What you want | Code |
|--------------|------|
| Create brain | `brain = nlm.createBrain(nlm.createDefaultConfig())` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step_number)` |
| Create world | `world = nlm.createSimpleWorld()` |
| Create agent | `agent = nlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `percept = world.getSensoryPercept()` |
| Make action | `action = agent.decodeMotorCommand()` |

---

## Part 7: Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? (This is essential!)
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Part 8: Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the advanced tutorials
5. Create your own mini-projects

---

## Summary

Congratulations! You've completed the "Getting Started with NLM" tutorial. You now know:

1. ✅ How to install and set up NLM Python bindings
2. ✅ How to create a virtual brain with neurons
3. ✅ How to set up a simulated world environment
4. ✅ How to create an intelligent agent that can think and act
5. ✅ How to run a complete simulation with learning
6. ✅ The basic brain-world-agent loop
7. ✅ Common patterns and mini-projects

You're now ready to use NLM! The sky's the limit for what you can create with this experimental brain simulator.

**Enjoy your journey into the world of neural computation!** 🚀
