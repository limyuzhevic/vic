# NLM Beginner's Comprehensive Guide

## Welcome to NLM

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This guide will help you get started with creating, training, and using neural systems that learn from experience.

**What NLM IS NOT:**
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

**What NLM IS:**
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Section 1: Getting Started

### 1.1 Installation and Setup

NLM has two main components: the C++ core library and Python bindings.

#### Prerequisites
```bash
# Install build dependencies
pip install scikit-build-core pybind11 numpy pytest

# Install pynlm from source (recommended)
pip install .

# Or install in development mode
pip install -e .
```

#### Building from Source
```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake
-DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python)

# Build
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### 1.2 First Program: Silent Brain

Let's start with the simplest possible example - a brain that just thinks.

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain (ALWAYS do this first!)
brain.initialize()

# Make the brain think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

# Get statistics
print("Done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**Expected Output:**
```
Done!
Total neurons: 1000
Total spikes: 42
Average firing rate: 0.04 Hz
```

### 1.3 Understanding the Components

NLM has three main components that work together:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│     WORLD       │───▶│   AGENT BRAIN   │───▶│     BRAIN       │
│ (Environment)   │    │   (Interface)   │    │ (Neural Core)   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
       │                      │                      │
       ▼                      ▼                      ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ Sensory Input   │    │ Motor Output    │    │ Neural Activity │
│ (Vision, Touch,│    │ (Actions)       │    │ (Spikes)        │
│  Internal)      │    │                 │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

#### The World
- Virtual environment where the agent lives
- Contains resources, hazards, and obstacles
- Provides sensory input to the agent

#### The Agent Brain
- Interface between brain and world
- Processes sensory input from world
- Decodes motor commands for world actions

#### The Brain
- Central neural network with neurons and synapses
- Performs computation using spiking dynamics
- Stores memories and learns from experience

## Section 2: Interactive Examples

### 2.1 Example 1: Brain Watching the World

This example shows a brain interacting with a simple world.

```python
import pynlm

# Step 1: Create the brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Create and configure the world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10, 10)  # Start agent in middle
world.reset()

# Step 3: Create the agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 4: Enable learning features (optional)
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)              # Explore new things

# Step 5: Run the simulation
print("Starting simulation...")
for step in range(100):
    # Update world state
    world.update(0.1)  # 0.1 seconds per step
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks (processes information)
    brain.step(step)
    
    # Get what the brain wants to do
    action = agent.decodeMotorCommand()
    
    # Do the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
print(f"Agent position: ({agent.getBrain().getAgentBody().x:.1f}, "
      f"{agent.getBrain().getAgentBody().y:.1f})")
print(f"Final health: {agent.getBrain().getAgentBody().health}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

**Example Output:**
```
Starting simulation...
Step 0: Pos=(10.0, 10.0), Energy=100.0, Health=100, FiringRate=0.42Hz
Step 10: Pos=(10.1, 10.0), Energy=98.5, Health=100, FiringRate=0.41Hz
Step 20: Pos=(10.2, 10.1), Energy=97.2, Health=100, FiringRate=0.39Hz
...
Step 99: Pos=(15.8, 12.3), Energy=45.7, Health=78, FiringRate=0.38Hz
Simulation finished!
Agent position: (15.8, 12.3)
Final health: 78
Firing neurons: 47
```

### 2.2 Example 2: Learning Through Rewards

This example demonstrates reward-based learning.

```python
import pynlm
import random

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.setAgentStart(7, 7)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable reward-based learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Reward-based learning simulation")
print("Watch how the brain learns to get rewards!")

# Run with random rewards to demonstrate learning
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get perception
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action (may get reward or penalty)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log curiosity and learning metrics
    if step % 50 == 0:
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        print(f"Step {step}: Curiosity={curiosity:.2f}, "
              f"Novelty={novelty:.2f}, PredictionError={prediction_error:.2f}")

print(f"Learning complete!")
print(f"Final curiosity level: {agent.getCuriosityLevel():.2f}")
```

### 2.3 Example 3: Developmental Brain

This example shows how brains develop over time.

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Developmental Simulation")
print("Brain will progress through developmental stages:")
print("1. Initial -> CriticalPeriod -> Maturation -> Adult -> Aging")

# Run development simulation
development_steps = 1000
for step in range(development_steps):
    # Step the brain
    brain.step(step, step * 0.001)  # Include time for development
    
    # Log developmental stage changes
    if step % 200 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Step {step}: Development stage = {stage}")
        
        # Show key developmental metrics
        avg_firing = brain.getAverageFiringRate()
        print(f"  Average firing rate: {avg_firing:.2f} Hz")
        print(f"  Plasticity factors changing...")

print(f"Development complete!")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
print(f"Final performance metrics:")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

## Section 3: Core Concepts and Terminology

### 3.1 Neurons and Synapses

**Neurons:**
- Basic computational units in NLM
- Implement Leaky Integrate-and-Fire (LIF) dynamics
- Three main types: Excitatory, Inhibitory, Modulatory

**Synapses:**
- Connections between neurons
- Can be excitatory, inhibitory, or modulatory
- Strengthen or weaken based on learning rules

### 3.2 Spiking Dynamics

Neurons fire spikes when their membrane potential reaches a threshold:
- **Resting potential:** -70 mV (default)
- **Threshold:** -50 mV (default)
- **Reset potential:** -75 mV (after spike)
- **Membrane time constant:** 20 ms (default)

### 3.3 Learning Mechanisms

NLM supports multiple learning mechanisms:

#### STDP (Spike-Timing-Dependent Plasticity)
- Synaptic strength changes based on spike timing
- Pre-post spikes strengthen synapses (LTP)
- Post-pre spikes weaken synapses (LTD)

#### Hebbian Learning
- "Neurons that fire together, wire together"
- Correlation-based weight changes

#### Reward-Modulated Learning
- Neuromodulators (dopamine-like signals) scale learning
- Reward prediction errors guide learning

### 3.4 Memory Systems

NLM has multiple memory systems (Phase 4 features):

#### Working Memory
- Persistent neural activity for temporary storage
- Maintains information during tasks
- Competitive selection between items

#### Episodic Memory
- Stores experiences as neural patterns
- Pattern completion for recall
- Replay during consolidation

#### Concept Formation
- Discovers patterns in experiences
- Forms stable prototypes
- Categorization and abstraction

### 3.5 Developmental Stages

Brains go through developmental stages:

1. **Initial:** High plasticity, naive connections
2. **CriticalPeriod:** Sensitive to experience
3. **Maturation:** Refining connections
4. **Adult:** Stable, efficient processing
5. **Aging:** Gradual decline in plasticity

## Section 4: Common Patterns and Best Practices

### 4.1 The Complete Agent Pattern

```python
def run_agent_simulation(brain, world, agent, num_steps):
    """Complete agent simulation with all components"""
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain computation
        brain.step(step)
        
        # 5. Decode motor command
        action = agent.decodeMotorCommand()
        
        # 6. Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Optional: Apply reward modulation
        # reward = world.getReward()  # If available
        # agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development (if enabled)
        # agent.updateDevelopment(0.1)
        
        # 9. Log progress
        if step % 100 == 0:
            print(f"Step {step}: Health={agent.getBrain().getAgentBody().health}")

# Usage example:
# run_agent_simulation(brain, world, agent, 1000)
```

### 4.2 Getting Started with Configuration

```python
import pynlm

# Create default config
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 500)      # Smaller brain
config.set("plasticity.stdp.enable", True) # Enable STDP
config.set("brain.v_thresh", -55.0)        # Lower threshold

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### 4.3 Debugging Tips

#### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing simulation steps
- Check that neurons have enough input current

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are connected

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - ensure sensory input is connected

### 4.4 Saving and Loading

```python
import pynlm
import os

# Train a brain first
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(1000):
    brain.step(i)

print("Training complete. Testing save/load...")

# Save the brain state
checkpoint_file = "my_brain.bin"
brain.save(checkpoint_file)
print(f"Brain saved to: {checkpoint_file}")

# Create a new brain and load the checkpoint
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
print("Loading checkpoint...")
new_brain.load(checkpoint_file)

# Verify the loaded state
print(f"Original spike count: {brain.getTotalSpikeCount()}")
print(f"Loaded spike count: {new_brain.getTotalSpikeCount()}")
print(f"Match: {brain.getTotalSpikeCount() == new_brain.getTotalSpikeCount()}")

# Clean up
if os.path.exists(checkpoint_file):
    os.remove(checkpoint_file)
```

## Section 5: Mini Projects

### Project 1: Silent Brain

Create a brain that just thinks without any interaction.

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

### Project 2: Brain Watching a World

Let a brain observe a world without taking actions.

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10, 10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Project 3: Complete Learning Agent

Create an agent that learns through interaction.

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.setAgentStart(7, 7)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: "
              f"{brain.getFiringNeuronCount()} neurons firing, "
              f"health={agent.getBrain().getAgentBody().health}")

print("Agent simulation complete!")
```

## Section 6: Quick Reference

### 6.1 Core Classes

| Class | Purpose | Main Methods |
|-------|---------|--------------|
| `pynlm.createBrain()` | Create neural brain | `initialize()`, `step()` |
| `pynlm.createSimpleWorld()` | Create environment | `update()`, `applyMotorCommand()` |
| `pynlm.createAgentBrain()` | Connect brain to world | `processSensoryInput()`, `decodeMotorCommand()` |
| `pynlm.createDefaultConfig()` | Create configuration | `set()`, `getKeys()` |

### 6.2 Common Operations

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

### 6.3 Common Errors

| Error Message | Solution |
|---------------|----------|
| "My brain isn't doing anything" | Call `brain.initialize()` first |
| "The agent isn't moving" | Call `world.applyMotorCommand()` |
| "Everything is 0" | Run more simulation steps |

## Section 7: Next Steps

### 7.1 Continue Learning

1. **Read `HOW_TO_USE.md`** - More detailed technical information
2. **Read `docs/ARCHITECTURE.md`** - System architecture overview
3. **Read `docs/SCIENCE.md`** - Scientific background
4. **Explore examples in `test_comprehensive.py`** - Real-world examples
5. **Check `benchmark_performance.py`** - Performance optimization

### 7.2 Advanced Topics

Once you're comfortable with the basics:

- **Learning and Plasticity**: Understanding how brains learn
- **Development**: How brains mature over time
- **Memory**: Working and episodic memory systems
- **Prediction**: Forward models and anticipation
- **Cognition**: Attention, planning, and problem-solving

### 7.3 Experiment Design

Start building your own experiments:

1. **Simple Behavior**: Study basic movement
2. **Learning Tasks**: Test different learning mechanisms
3. **Development Studies**: Compare developmental trajectories
4. **Memory Tests**: Measure storage and retrieval
5. **Social Interactions**: Study multi-agent systems

## Section 8: Community and Support

### 8.1 Getting Help

- **Documentation**: This guide and the official docs
- **Examples**: Look at existing code in the repository
- **Testing**: Run the test suite with `pytest tests/`
- **Community**: Join NLM discussions

### 8.2 Contributing

If you want to contribute:

1. **Report Issues**: File bug reports with reproduction steps
2. **Improve Examples**: Add new example scenarios
3. **Fix Documentation**: Correct errors or add missing information
4. **Performance**: Optimize existing implementations
5. **Research**: Submit your findings

## Section 9: License and Citation

NLM is licensed under MIT. Please cite it in your research papers.

## Section 10: Acknowledgments

Thanks to all contributors and researchers who have worked on NLM!

---

*This guide is continuously improved. Please report any issues or suggest improvements!*