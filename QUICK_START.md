# Quick Start Guide

## What is NLM?

NLM (Neural Learning Machine) is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

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

## What's New?

This version includes enhanced features:

### 🚀 Enhanced Package Structure
- Organized `pynlm/` Python package with modules
- Type hints and comprehensive documentation
- Better error handling with custom exceptions

### 📊 Performance Monitoring
- Progress tracking for long simulations
- Built-in benchmarking tools
- Statistics collection and reporting

### 💾 Checkpoint Management
- Save simulation state with metadata
- Restore from checkpoints
- Automatic checkpoint utilities

### ✅ Validation
- Simulation setup validation
- Configuration checking
- Error prevention

### 📚 Better Documentation
- Unified documentation hub
- Quick reference guides
- Example patterns
- Interactive utilities

## Installation

### Prerequisites
- Python 3.8+
- C++ compiler and CMake (for building)

### Install
```bash
pip install -e .  # Install in development mode
# or
pip install pynlm  # Install from package
```

## Basic Usage

### Simple Brain
```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Complete Agent
```python
import pynlm

# Setup complete agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Try These Examples

### Example 1: Get Started
Copy and run this to see your first brain in action:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Agent with Learning
Copy and run this to see an agent that learns from its experiences:

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Run agent
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### Example 3: Progress Tracking
Copy and run this to see progress reporting for long simulations:

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

# Run with progress tracking
stats = pynlm.run_with_progress(brain, world, agent, num_steps=1000, progress_interval=100)

print(f"\nSimulation complete!")
print(f"Total time: {stats['elapsed_time']:.2f} seconds")
print(f"Average firing rate: {stats['average_firing_rate']:.3f} Hz")
```

## What to Do Next

1. **Run the Examples**: Try the examples above to see NLM in action
2. **Explore the Documentation**: Check `NLM_Documentation.md` for comprehensive guides
3. **Try Advanced Features**: Use `pynlm.benchmark_simulation()` and `pynlm.save_checkpoint()`
4. **Create Your Own**: Experiment with different brain configurations and behaviors
5. **Join the Community**: Learn from examples and contribute to the project

## Need Help?

### Quick Reference
- **Core Documentation**: `NLM_Documentation.md`
- **API Reference**: The `pynlm/` package modules
- **Examples**: In the `pynlm/examples/` directory
- **Migration Guide**: `MIGRATION_GUIDE.md` (if upgrading from older versions)

### Common Issues
- **"My brain isn't doing anything"**: Did you call `brain.initialize()`?
- **"The agent isn't moving"**: Did you call `world.applyMotorCommand()`?
- **"Everything is 0"**: Brains need time to "warm up" - try more steps

## Build from Source

If you want to build from source:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..
pip install -e .
```

## Happy Brain Simulation!

You've just started your journey into creating artificial developmental brains! NLM is a powerful tool for exploring how neural systems can learn, remember, and adapt through interaction with environments.

Remember: The key to success is experimentation! Try different configurations, enable different learning subsystems, and see what your brains can achieve.

Good luck, and enjoy the journey of discovery!
