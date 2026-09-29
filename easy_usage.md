# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator written in C++20 with Python bindings. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

**Key Features:**
- Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- Synaptic delays and multiple plasticity mechanisms (STDP, Hebbian, structural)
- Neuromodulation for curiosity, novelty, and reward
- Working, episodic, and associative memory systems
- Developmental brain maturation
- Integration with world environments
- Python bindings for easy programming

**Analogy:**
| Real Brain | NLM |
|------------|-----|
| Neurons fire | `brain.step(step)` |
| Memories form | `agent.enableStructuralPlasticity()` |
| You learn from rewards | `agent.applyRewardModulation(reward)` |
| You explore when curious | `agent.getCuriosityLevel()` |

---

## Prerequisites

Before getting started, you'll need:

### Software Requirements
- **Python**: 3.8+ (for the Python bindings)
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+ (for building from source)
- **CMake**: Version 3.16+ (for building)
- **Python Packages**: `pybind11`, `scikit-build-core`, `pytest`, `numpy`

### Installation Options

#### Option 1: Install from PyPI (Easiest)
```bash
pip install pynlm
```

#### Option 2: Build from Source (For Development)
```bash
# Clone the repository
pip install -e .  # Install in development mode
```

#### Option 3: Full Build with All Features
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install .
```

---

## Quick Start (Beginner Friendly)

### Quick Test - Everything Working?

Let's start with the absolute simplest test to verify everything is installed:

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it (always call this first!)
brain.initialize()

# Make it think for a few steps
for i in range(10):
    brain.step(i)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Step-by-Step Setup

Here's the most common setup pattern:

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Create simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# 4. Create agent bridge
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 5. Enable key features (optional but recommended)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# 6. Run simulation
for step in range(100):
    world.update(0.1)
    
    # What does the agent see?
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # Do the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

---

## The 3 Core Concepts

### 1. Brain - The Virtual Thinking Entity
**What it does:** Stores information, learns, makes decisions
**Key Methods:**
- `brain.initialize()` - Start up the brain
- `brain.step(step)` - Make it think one moment
- `brain.getTotalSpikeCount()` - Get number of spikes
- `agent.getCuriosityLevel()` - How curious it is

**Key Properties:**
- Total neurons, synapses
- Firing rate, activity patterns
- Developmental stage

### 2. World - The Environment
**What it does:** Contains the agent's sensory inputs and actions
**Key Methods:**
- `world.configure()` - Set world size and vision
- `world.update(time)` - Advance time
- `world.getSensoryPercept()` - Get what agent sees
- `world.applyMotorCommand(action)` - Do an action

**Key Features:**
- 2D grid world
- Vision sensors (8x8 grayscale)
- Touch sensors for collision
- Internal state signals

### 3. Agent - The Interface Bridge
**What it does:** Connects the brain to the world
**Key Methods:**
- `agent.processSensoryInput(percept)` - Give brain the percept
- `agent.decodeMotorCommand()` - Get action from brain
- `agent.applyRewardModulation(reward)` - Brain learns from rewards
- `agent.updateDevelopment(time)` - Brain matures over time

**Key Features:**
- Sensory processing
- Motor command decoding
- Neuromodulation (curiosity, reward)
- Development tracking

---

## Complete Working Example

Here's a complete, runnable example that demonstrates all core features:

```python
import pynlm
import time

def run_experiment(experiment_name="Default"):
    """Run a complete NLM experiment with all features."""
    
    print(f"=== Starting {experiment_name} ===")
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create world with sensory capabilities
    world = pynlm.createSimpleWorld()
    world.configure(
        width=15,
        height=15,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # 4. Create agent bridge
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 5. Enable all subsystems for learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation
    print(f"Running {500} simulation steps...")
    start_time = time.time()
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning signals
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # Calculate and display final statistics
    end_time = time.time()
    
    print(f"\n=== {experiment_name} Results ===")
    print(f"Total runtime: {end_time - start_time:.2f} seconds")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"E/I balance: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Memory working memory: {'Integrated' if agent.getNeuromodulationLevel() > 0 else 'Not Connected'}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    # Save brain state (optional)
    # brain.save("experiment_brain.bin")
    
    return brain, agent, world

# Run the experiment
if __name__ == "__main__":
    experiment_brain, experiment_agent, experiment_world = run_experiment("Demo Experiment")
    
    print("\n=== Experiment Complete ===")
    print("Your NLM brain is now ready for more complex tasks!")
```

---

## Common Patterns and Best Practices

### Pattern 1: Basic Learning Loop
```python
# Pattern for learning agents
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

for step in range(1000):
    world.update(0.1)
    
    # Percept -> Brain -> Action -> World -> Reward
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(calculate_reward(), 0.0)
```

### Pattern 2: Developmental Learning
```python
# Pattern for developing brains over time
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableDevelopment(True)  # Enable brain maturation

# Brain develops faster at first, then slows down
for age in range(0, 100, 1):
    # Age in developmental years (0-100)
    agent.updateDevelopment(0.1)  # 0.1 years per step
    
    # Get current developmental stage
    stage = agent.getDevelopmentalStage()
    print(f"Developmental stage: {stage}")
```

### Pattern 3: Curiosity-Driven Exploration
```python
# Pattern for curious agents that explore when uncertain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)  # Enable exploration

for step in range(500):
    world.update(0.1)
    
    # Get current curiosity level
    curiosity = agent.getCuriosityLevel()
    
    if curiosity > 0.5:  # High curiosity = more exploration
        # Sometimes choose random actions
        if random.random() < 0.3:  # 30% chance of exploration
            action = pynlm.MotorCommand.Interact  # Or any other command
    
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

---

## Quick Reference Guide

### Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| **Brain** | Virtual neural system | `initialize()`, `step()`, `save()` |
| **AgentBrain** | Interface to world | `processSensoryInput()`, `decodeMotorCommand()` |
| **SimpleWorld** | Environment | `configure()`, `update()`, `getSensoryPercept()` |
| **Config** | Configuration | `set()`, `get()`, `saveToFile()` |

### Essential Workflow

```python
# 1. Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# 3. Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# 5. Main loop
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Common Commands

| What you want | Python Code |
|--------------|-------------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Brain thinks | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get stats | `brain.getTotalNeuronCount()` |
| Get percept | `world.getSensoryPercept()` |
| Get action | `agent.decodeMotorCommand()` |

---

## Mini Projects

### Project 1: Silent Brain Test
```python
import pynlm

# Just test that neurons fire
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(20):
    brain.step(i)

print(f"Test passed! {brain.getTotalSpikeCount()} spikes generated.")
```

### Project 2: Brain-Controlled Movement
```python
import pynlm

# Setup learning system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Run with reward for moving forward
for step in range(100):
    world.update(0.1)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward from world
    percept = world.getSensoryPercept()
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    
    # Brain learns from reward
    agent.applyRewardModulation(reward, 0.0)

print("Movement learning complete!")
```

### Project 3: Memory Experiment
```python
import pynlm

# Create brain with memory capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Run exploration to form memories
print("Exploring world to form memories...")
for step in range(200):
    world.update(0.1)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward for learning
    reward = agent.getNeuromodulationLevel()
    agent.applyRewardModulation(reward, 0.0)

print(f"Memory formation complete!")
print(f"Total episodes stored: {brain.getTotalSpikeCount() // 100}")
```

---

## Troubleshooting

### "My brain isn't doing anything"
```python
# Problem: Brain not firing
# Solution: Check initialization and add more time
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # Always call this!

# Add more simulation steps
for i in range(1000):  # More than 100
    brain.step(i)
```

### "The agent isn't moving"
```python
# Problem: No movement
# Solution: Check world update and action
world.update(0.1)  # Always update world first!

action = agent.decodeMotorCommand()
if action == pynlm.MotorCommand.Wait:
    print("Brain chose to wait - maybe not enough input?")
else:
    world.applyMotorCommand(action, world.getSimulationTime())
```

### "Everything is 0"
```python
# Problem: No activity
# Solution: Brains need time to "warm up"
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run many steps for neural network to stabilize
for i in range(500):  # Warm-up steps
    brain.step(i)

# Now add interesting patterns
for i in range(100):  # Main experiment
    brain.step(i + 500)
```

### "ImportError: No module named 'pynlm'"
```bash
# Install the module
pip install pynlm

# Or install in development mode
pip install -e .
```

---

## Next Steps

### For Beginners:
1. **Try the examples** - Run all the code in this guide
2. **Read HOW_TO_USE.md** - For detailed technical documentation
3. **Explore the API** - Try different combinations of brain/world/agent

### For Intermediate Users:
1. **Study ARCHITECTURE.md** - Understand the system design
2. **Build from source** - Learn about the C++ backend
3. **Create custom worlds** - Modify the SimpleWorld for specific tasks

### For Advanced Users:
1. **Read SCIENCE.md** - Scientific background and theory
2. **Study EXPERIMENTS.md** - Detailed experiment descriptions
3. **Contribute to the project** - Help improve NLM

---

## Success Metrics

When you're finished with this guide, you should be able to:

- [ ] Import pynlm and create a brain
- [ ] Initialize and run simulations
- [ ] Create worlds and agents
- [ ] Enable learning subsystems
- [ ] Run complete learning experiments
- [ ] Understand brain statistics and metrics
- [ ] Troubleshoot common problems
- [ ] Create custom mini-projects

That's it! You're now ready to use NLM for serious neural network research and applications.

**Happy brain-building!** 🤖✨