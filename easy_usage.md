# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an **experimental artificial developmental brain** system. It's a research-grade neural network simulator that models brain-like computation using spiking neural networks with Leaky Integrate-and-Fire (LIF) neurons.

Based on the project structure (Phase 6 integration complete), NLM implements:

### Core Features
- **Spiking Neural Networks**: LIF neurons with event-driven dynamics
- **Multiple Plasticity Mechanisms**: STDP (Spike-Timing-Dependent Plasticity) and Hebbian learning
- **Structural Plasticity**: Dynamic synapse formation and pruning
- **Multi-Stage Development**: Brain maturation from primitive to adult states
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Memory Systems**: Working, episodic, and associative memory
- **Prediction & Planning**: Temporal sequence learning and action selection
- **Social Learning**: Observation and imitation capabilities

Think of it like this:
- **Neurons** = Brain cells that send electrical signals
- **Synapses** = Connections between brain cells that strengthen/weaken
- **Spikes** = Neural events representing communication
- **Learning** = When connections get stronger or weaker based on timing and experience
- **Memory** = The brain's experience storage and recall mechanisms
- **Development** = How brains mature and adapt over time

NLM simulates all of these components working together as a unified cognitive system.

## Project Overview (Phase 6 Complete)

NLM has evolved through 6 phases:

1. **Phase 1**: Core systems (types, config, brain interfaces)
2. **Phase 2**: Neural dynamics (LIF, spikes, STDP, Hebbian, structural plasticity)
3. **Phase 3**: World interaction (sensory-motor loop, reward prediction, development)
4. **Phase 4**: Emerging cognition (memory, prediction, planning, attention, self-model)
5. **Phase 5**: Performance (optimization, parallel processing, checkpointing)
6. **Phase 6**: Final integration (all systems working together)

## Quick Start (Copy & Paste Examples)

### Example 1: Simplest Brain

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run the brain for 100 time steps
for i in range(100):
    brain.step(i)  # Process neural dynamics

print(f"Done! Your brain fired {brain.getTotalSpikeCount()} spikes")
print(f"Neurons: {brain.getTotalNeuronCount()}, Firing: {brain.getFiringNeuronCount()}")
```

### Example 2: Complete Agent in Environment

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create and configure a simple world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow/new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore novel things

# Run simulation
for step in range(100):
    world.update(0.1)  # Update world dynamics
    percept = world.getSensoryPercept()  # What does agent see?
    agent.processSensoryInput(percept)  # Brain processes sensory input
    brain.step(step)  # Brain performs neural computation
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Take action

print("Agent simulation complete!")
```

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

## Step-by-Step Guide

### 1. Get Started with Python

```bash
# Install NLM (requires scikit-build-core and pybind11)
pip install pynlm

# Verify installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### 2. Create Your First Brain

```python
import pynlm

# Step 1: Create brain configuration
config = pynlm.createDefaultConfig()
# Optional: Customize brain properties
config.set("brain.neuron_count", 500)

# Step 2: Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Step 3: Check initial status
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Synaptic connections: {brain.getTotalSynapseCount()}")
```

### 3. Run Neural Simulation

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation - this is where neurons process information
for step in range(1000):
    brain.step(step)
    
    # Monitor progress
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### 4. Working with Environments

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create and configure environment
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()

# Initialize agent with environment
agent.initialize(world)

# Configure agent features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run episode
for step in range(500):
    world.update(0.1)  # Advance time
    percept = world.getSensoryPercept()  # Get sensory input
    agent.processSensoryInput(percept)  # Brain sees world
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides
    world.applyMotorCommand(action, world.getSimulationTime())  # Act
```

## Project Architecture Explained

### Core Components

| Component | Purpose | Key Features |
|-----------|---------|--------------|
| **Brain** | Neural computation core | LIF neurons, spiking dynamics, STDP, Hebbian learning |
| **Agent** | Interface to environment | Sensory processing, motor decoding, neuromodulation |
| **World** | Simulation environment | 2D grid, perception, actions, rewards |
| **Memory** | Experience storage | Working, episodic, associative memory |
| **Development** | Brain maturation | Stages from primitive to adult, structural changes |

### How They Work Together

```
WORLD (Environment)
  ↓ (percepts)    ACTION (commands)
SENSORY INPUT → AGENT (brain interface) → BRAIN (neural processing) → MOTOR OUTPUT
  ↑                                     ↑
 REWARD/PUNISHMENT (neuromodulation)     ↑
  ↓                                     ↓
PLASTICITY (learning) ← NEUROMODULATION ← DEVELOPMENT
  ↓                                     ↓
MEMORY (storage) ← WORKING (short-term) ← ACTION (planning)
  ↓
BEHAVIOR (adapted)
```

## Python Examples

### Example A: Silent Brain (Neural Computation)

```python
import pynlm

print("=== Silent Brain Example ===")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(50):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Current firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example B: Learning Agent

```python
import pynlm

print("=== Learning Agent Example ===")

# Setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Learning phase
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Development stage: {brain.getDevelopmentalStage()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Example C: Development & Plasticity

```python
import pynlm

print("=== Development & Plasticity Example ===")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Developmental progression simulation
stages = []
for day in range(10):
    # Simulate daily development
    for hour in range(24):
        for step in range(100):
            brain.step(step)
    
    stage = brain.getDevelopmentalStage()
    firing = brain.getFiringNeuronCount()
    stages.append((day, stage, firing))

# Print development summary
print("Development progress:")
for day, stage, firing in stages:
    print(f"  Day {day}: {stage} stage, {firing} neurons firing")
```

## Common Usage Patterns

### Pattern 1: Simulation Loop

```python
def run_simulation(brain, world=None, agent=None, steps=1000):
    """Standard simulation pattern with optional world interaction."""
    for step in range(steps):
        if world and agent:
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        
        brain.step(step)
        
        if agent:
            action = agent.decodeMotorCommand()
            if world:
                world.applyMotorCommand(action, world.getSimulationTime())
```

### Pattern 2: Brain Statistics

```python
def print_brain_stats(brain, agent=None, step=None):
    """Print comprehensive brain statistics."""
    if step is not None:
        print(f"\n=== Step {step} ===")
    
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Firing: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"Development: {brain.getDevelopmentalStage()}")
    
    if agent:
        print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"Prediction error: {agent.getPredictionError():.3f}")
```

### Pattern 3: Advanced Configuration

```python
def create_advanced_brain(neuron_count=2000, enable_curiosity=True, enable_stdp=True):
    """Create a brain with specific configuration."""
    config = pynlm.createDefaultConfig()
    
    # Basic configuration
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.15)
    
    # Plasticity settings
    if enable_stdp:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Neuromodulation
    config.set("neuromod.curiosity.enable", enable_curiosity)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    # Development
    config.set("development.enable", True)
    config.set("development.initial_stage", "Initial")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain
```

## Mini Projects

### Mini Project 1: Brain Visualization

```python
import pynlm
import matplotlib.pyplot as plt

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

spike_counts = []
for step in range(500):
    brain.step(step)
    spike_counts.append(brain.getTotalSpikeCount())

plt.plot(spike_counts)
plt.title("Neural Spike Activity")
plt.xlabel("Simulation Steps")
plt.ylabel("Total Spikes")
plt.show()
print("Brain visualization complete!")
```

### Mini Project 2: Memory Development

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

memories = []
for episode in range(10):
    for step in range(100):
        brain.step(step)
    
    memories.append({
        'episode': episode,
        'stage': brain.getDevelopmentalStage(),
        'firing_rate': brain.getAverageFiringRate(),
        'spike_count': brain.getTotalSpikeCount()
    })

print("Memory development summary:")
for m in memories:
    print(f"  Episode {m['episode']}: {m['stage']}, "
          f"{m['firing_rate']:.2f} Hz firing")
```

### Mini Project 3: Agent Training

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Agent training started...")
for episode in range(5):
    score = 0
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"  Episode {episode}, Step {step}: "
                  f"{agent.getCuriosityLevel():.3f} curiosity")
    
    print(f"Episode {episode} complete")

print("Agent training complete!")
```

## Quick Reference Guide

### Essential Commands

| Action | Python Code | Description |
|--------|-------------|-------------|
| Create brain | `pynlm.createBrain(config)` | Creates neural network with neurons/synapses |
| Initialize brain | `brain.initialize()` | Sets up brain state before simulation |
| Run simulation | `brain.step(step)` | Processes one time step of neural dynamics |
| Create world | `pynlm.createSimpleWorld()` | Creates 2D simulation environment |
| Create agent | `pynlm.createAgentBrain(brain)` | Connects brain to environment |
| Get stats | `brain.getTotalNeuronCount()` | Various brain statistics |
| See world | `world.getSensoryPercept()` | Gets agent's sensory input |
| Get action | `agent.decodeMotorCommand()` | Reads motor commands from brain |

### Key Methods Summary

**Brain Methods:**
- `initialize()` - Setup neural system
- `step(time)` - Process neural dynamics
- `reset()` - Clear state
- `receiveSensoryInput()` - Inject sensory data
- `produceAction()` - Output motor commands
- `save/load()` - Persistent storage

**Agent Methods:**
- `initialize(world)` - Setup interface
- `processSensoryInput()` - Convert percepts to neural input
- `decodeMotorCommand()` - Convert neural activity to actions
- `applyRewardModulation()` - Apply learning signals
- `updateDevelopment()` - Progress brain maturation

**World Methods:**
- `configure()` - Setup environment parameters
- `update(time)` - Advance simulation
- `applyMotorCommand()` - Execute actions
- `getSensoryPercept()` - Get observations
- `getAgentBody()` - Access agent state

## Troubleshooting Guide

### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try running more simulation steps
- Check if neurons have enough input to fire

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Verify that `world.update()` is in your loop
- Check that `agent.decodeMotorCommand()` returns valid actions

**"Everything is 0 or very slow"**
- Brains need time to "warm up" - try more steps
- Ensure sensory input is connected properly
- Check that neuromodulation is enabled for learning

**"Simulation crashes or segfault"**
- Always call `brain.initialize()` before `brain.step()`
- Make sure `reset()` is called before re-running
- Check configuration values for valid ranges

**"Learning isn't happening"**
- Verify that `agent.enableRewardModulation(True)` is called
- Check that plasticity rules are enabled
- Ensure world provides reward/punishment signals

**"Development not progressing"**
- Call `agent.updateDevelopment(time)` in your loop
- Check developmental stage boundaries
- Verify that development parameters are properly configured

### Installation Issues

**Import Error: No module named 'pynlm'**
```bash
# Reinstall the module
pip install --force-reinstall pynlm

# Or install in development mode
pip install -e .
```

**Build Errors**
```bash
# Clean build cache
rm -rf build dist *.egg-info
pip install --force-reinstall pynlm
```

**Missing Dependencies**
```bash
pip install pybind11 scikit-build-core numpy
```

## Next Steps

When you're comfortable with the basics:

1. **Explore HOW_TO_USE.md** - Read about advanced C++ features and building
2. **Check ARCHITECTURE.md** - Deep dive into the technical architecture
3. **Read SCIENCE.md** - Understand the scientific background
4. **Try Phase 6 Demos** - See the complete integrated system in action
5. **Experiment with Configurations** - Modify settings to see different behaviors
6. **Contribute** - Help improve the documentation or codebase!

You now have everything you need to start building with NLM! The system is designed to be accessible for beginners while providing depth for advanced users. Happy coding! 

## Resources

- [HOW_TO_USE.md](HOW_TO_USE.md) - Advanced usage and building instructions
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [README.md](README.md) - Project overview and Phase 6 achievements
- [pyproject.toml](pyproject.toml) - Python package configuration
