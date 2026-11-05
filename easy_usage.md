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

## Two Ways to Use NLM

NLM supports two main ways to use it:

1. **Python API** (via pynlm module) - For easy scripting and research
2. **Command-line demos** - For testing and demonstrations

**Important**: The Python bindings (pynlm) are built by default when using pip install, but they're still experimental and may have bugs.

---

## Quick Start with Python API

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

### Example 2: Brain in a Simple World (Ready-to-use pattern)

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

## Quick Start with Command-Line Interface

### Using the Unified Demo System

The unified demo system provides command-line access to all NLM phases:

**Basic usage:**
```bash
./nlm_demo --phase 6
```

**With custom parameters:**
```bash
./nlm_demo --phase 3 --episodes 10 --steps-per-episode 500
```

**Enable advanced features:**
```bash
./nlm_demo --phase 4 --enable-dev --enable-curiosity --enable-plasticity
```

**Run benchmark:**
```bash
./nlm_demo --benchmark --neuron-count 2000
```

**Show help:**
```bash
./nlm_demo --help
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

### Python API Functions:

**`pynlm.createBrain(config)`**
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

**`brain.initialize()`**
Starts up the brain. Always call this before using the brain!

**`brain.step(step_number)`**
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

**`world.update(time)`**
Updates the virtual world by `time` seconds.

**`agent.processSensoryInput(percept)`**
Gives sensory information (vision, touch, etc.) to the brain.

**`agent.decodeMotorCommand()`**
Reads the brain's motor neurons to decide what action to take.

### Command-Line Options:

**`--phase N`**: Run phase 2-6 (2=basic neural computation, 3=world interaction, etc.)

**`--episodes N`**: Number of episodes (for phases 3-6)

**`--steps-per-episode N`**: Steps per episode

**`--neuron-count N`**: Total neurons to simulate

**`--enable-dev`**: Enable developmental features

**`--enable-reward`**: Enable reward modulation

**`--enable-curiosity`**: Enable curiosity-driven exploration

**`--enable-plasticity`**: Enable synaptic plasticity

**`--enable-structural`**: Enable structural plasticity (grow/prune synapses)

**`--benchmark`**: Run performance benchmark instead of demo

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

| What you want | Python Code | Command-Line |
|--------------|-------------|--------------|
| Create brain | `pynlm.createBrain(config)` | `--phase N` |
| Start brain | `brain.initialize()` | (auto) |
| Make brain think | `brain.step(step)` | (auto) |
| Create world | `pynlm.createSimpleWorld()` | (built-in) |
| Create agent | `pynlm.createAgentBrain(brain)` | (built-in) |
| Get brain stats | `brain.getFiringNeuronCount()` | `--benchmark` |
| See world | `world.getSensoryPercept()` | (built-in) |
| Make action | `agent.decodeMotorCommand()` | (built-in) |

---

## Next Steps

When you're comfortable:

1. **For research**: Use the Python API (`pynlm`) to create custom experiments
2. **For understanding**: Read `HOW_TO_USE.md` for detailed explanations
3. **For architecture**: Read `docs/ARCHITECTURE.md` to understand how it all works
4. **For testing**: Use the command-line demos with different phases
5. **For development**: Modify the source code to add new features

That's it! You're now ready to use NLM.

## Python API Notes

The Python bindings expose the following main classes:

- **Brain**: Central neural simulation engine
- **Config**: Configuration for brain parameters  
- **AgentBrain**: Interface connecting brain to world
- **SimpleWorld**: 2D simulation world
- **Vision, Audio, InternalSignals**: Different sensory modalities
- **Action**: Motor commands for interacting with the world

**Tip**: For research and experimentation, start with the simple patterns in this guide, then build more complex simulations by combining the basic building blocks.

**Important**: The Python bindings are experimental and may change. For the most stable features, use the command-line demos or C++ API directly.