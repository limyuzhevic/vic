# NLM - Easy Usage Guide for Beginners

## Overview

NLM (熙然) is a computational brain simulation that creates virtual brains capable of learning, remembering, and making decisions through spiking neural dynamics. Think of it as simulating how biological brains work, but using simplified models for research purposes.

### The Three Core Components

1. **Brain** - The virtual neural system that processes information
2. **World** - The environment the brain interacts with  
3. **Agent** - The interface connecting brain to world

## Getting Started

### Installation

```bash
# Clone and build the project
mkdir nlm-workspace
cd nlm-workspace
git clone <repository-url>
cd nlm

# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### Quick Test

Run the Phase 6 integration demo to see all systems working together:

```bash
./build/nlm_phase6_demo
```

## Basic Python Usage

### Step 1: Create a Simple Brain

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain (always call this first)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Step 2: Run Basic Processing

```python
# Create brain and world for interaction
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(100):
    world.update(0.1)                    # Advance world time
    percept = world.getSensoryPercept()   # Get what agent sees
    agent.processSensoryInput(percept)   # Brain processes sensory input
    brain.step(step)                     # Brain processes one time step
    action = agent.decodeMotorCommand()  # Brain decides on action
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute action

print(f"Simulation complete! Neurons firing: {brain.getFiringNeuronCount()}")
```

## Understanding the Components

### Brain Class

The `Brain` is the core neural processing system implementing Leaky Integrate-and-Fire (LIF) neuron dynamics.

```python
# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Basic brain operations
brain.step(step_number)  # Process one simulation step
brain.reset()           # Reset to initial developmental state

# Get brain statistics
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
firing = brain.getFiringNeuronCount()
rate = brain.getAverageFiringRate()
stage = brain.getDevelopmentalStage()
```

### World Class

The `SimpleWorld` represents the embodied environment where the brain operates.

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)  # Set agent starting position

# Environment operations
world.update(timestep)                    # Advance environment
percept = world.getSensoryPercept()       # Get sensory input
body = world.getAgentBody()               # Get agent body info
action = world.applyMotorCommand(cmd, time)  # Execute action
```

### AgentBrain Class

The `AgentBrain` connects the neural brain to the environment, handling sensory processing and motor decoding.

```python
# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Core agent functions
agent.processSensoryInput(percept)      # Convert world percept to neural input
action = agent.decodeMotorCommand()     # Convert neural activity to motor command
agent.applyRewardModulation(reward, pred)  # Apply learning signals
agent.updateDevelopment(timestep)       # Advance development stage

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Get subsystem states
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
```

## Tutorial Examples

### Tutorial 1: Silent Brain (Basic Neural Activity)

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 10 simulation steps
for i in range(10):
    brain.step(i)

print("Silent brain test complete!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Tutorial 2: Brain Watching a World

```python
import pynlm

# Setup complete brain-world-agent system
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

print(f"Watched world for 30 steps")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### Tutorial 3: Complete Learning Agent

```python
import pynlm

# Setup learning system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 20 steps
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")

print("Learning agent simulation complete!")
```

## Common Patterns

### Pattern 1: Run a Complete Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    """Run a complete brain-world interaction simulation."""
    for step in range(num_steps):
        # Update environment state
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Brain processes one step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Usage example
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Monitor Brain Activity

```python
def monitor_brain(brain, world, agent, interval=10):
    """Monitor brain activity during simulation."""
    for step in range(1000):
        # (simulation loop would go here)
        
        if step % interval == 0:
            print(f"Step {step}:")
            print(f"  Total neurons: {brain.getTotalNeuronCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            if hasattr(agent, 'getCuriosityLevel'):
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
```

### Pattern 3: Enable Learning Features

```python
def enable_learning(agent):
    """Enable all learning subsystems."""
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore novel stimuli
    print("All learning subsystems enabled")

# Usage
enable_learning(agent)
```

## Quick Reference

| Task | Python Code |
|------|-------------|
| Create brain | `pynlm.createBrain(config)` |
| Initialize brain | `brain.initialize()` |
| Run brain step | `brain.step(step_number)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getTotalNeuronCount()` |
| See world percept | `world.getSensoryPercept()` |
| Get motor action | `agent.decodeMotorCommand()` |
| Enable learning | `agent.enableRewardModulation(True)` |
| Get development | `brain.getDevelopmentalStage()` |

## Troubleshooting

### Common Issues

**Issue: Brain doesn't seem to be working**
- **Solution**: Always call `brain.initialize()` before `brain.step()`
- **Tip**: Try increasing the number of simulation steps

**Issue: Agent isn't responding to world**
- **Solution**: Check that `world.update()` is being called before `agent.processSensoryInput()`
- **Tip**: Ensure sensory input is properly connected

**Issue: Everything is zero**
- **Solution**: Brains need time to "warm up" - try more simulation steps
- **Tip**: Ensure sensory input is actually reaching the brain

### Debugging Tips

```python
def debug_simulation(brain, world, agent, step):
    """Debug information during simulation."""
    print(f"Step {step}:")
    print(f"  World time: {world.getSimulationTime():.1f}")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Action available: {agent.decodeMotorCommand() is not None}")
    
    # Check if brain is initialized
    if step == 0:
        print(f"  Brain initialized: {brain is not None}")
        print(f"  Agent initialized: {agent is not None}")
```

## Next Steps

### For Beginner Users
1. Complete the tutorials in this file
2. Try the basic examples
3. Experiment with different brain configurations
4. Read `HOW_TO_USE.md` for more technical details
5. Read `docs/ARCHITECTURE.md` to understand system design

### For Intermediate Users
1. Explore `docs/SCIENCE.md` for biological background
2. Try `docs/EXPERIMENTS.md` experiment protocols
3. Study `docs/ROADMAP.md` for development progress
4. Read `docs/LIMITATIONS.md` to understand system boundaries
5. Test different scaling configurations with `docs/SCALING.md`

### For Advanced Users
1. Understand `docs/PHASE6_FINAL_AUDIT.md` for current integration status
2. Explore `docs/PERFORMANCE.md` for optimization strategies
3. Study `docs/BIOLOGICAL_FIDELITY.md` for biological modeling details
4. Contribute to new features and documentation
5. Run benchmarks to measure system performance

## Performance Tips

### Optimize Simulation Speed
```python
# Use larger timesteps for faster simulation
world.update(0.1)    # 100ms steps instead of 10ms

# Monitor performance during runs
import time
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()
print(f"1000 steps took {end_time - start_time:.2f} seconds")
```

## Community and Further Learning

- **GitHub**: [NLM Project Repository](https://github.com/nlm-project/nlm)
- **Documentation**: Continuous updates as Phase 6 integration progresses
- **Experiments**: Run cognitive experiments to see emergent behaviors
- **Research**: NLM is an active research platform - results may vary

---

**Note**: This guide covers the basic usage patterns. For advanced features, check the comprehensive `HOW_TO_USE.md` documentation or explore the source code directly.

That completes your NLM beginner guide! You're now ready to start experimenting with artificial developmental brains.
