NLM - Easy Usage Guide for Beginners

This guide provides an easy-to-follow introduction to using NLM (Neural Learning Machine)
for beginners and intermediate users. It focuses on practical examples and
clear explanations of the fundamental concepts.

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can
learn, remember things, and make decisions - just like how your brain works!

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
agent.enableCuriosity(True)              # Explore new things
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

## Examples in This Repository

This repository includes several examples that demonstrate different aspects of NLM:

### Python Examples (pynlm/examples/)
- `basic_brain_example.py` - Simple brain creation and simulation
- `learning_plasticity_example.py` - Learning and plasticity demonstration

### C++ Examples (src/experiments/)
- `Phase3Demo.cpp` - Phase 3 experimental setup
- `Phase6Demo.cpp` - Phase 6 integration test

### Documentation
- `easy_usage.md` - This beginner-friendly guide
- `HOW_TO_USE.md` - Comprehensive C++ usage instructions
- `docs/ARCHITECTURE.md` - System architecture overview

Each example builds upon the previous ones, allowing you to progressively learn more about NLM's capabilities.

## Quick Start Checklist

### For Beginners
1. ✅ Read this guide
2. ✅ Try basic_brain_example.py
3. ✅ Understand the three components (Brain, World, Agent)
4. ✅ Try agent_world_example.py
5. ✅ Experiment with configurations

### For Intermediate Users
1. ✅ Read HOW_TO_USE.md
2. ✅ Try learning_plasticity_example.py
3. ✅ Explore advanced configurations
4. ✅ Understand memory systems
5. ✅ Try Phase 3 or Phase 6 demos

### For Advanced Users
1. ✅ Read ARCHITECTURE.md
2. ✅ Explore all example scripts
3. ✅ Understand development stages
4. ✅ Explore performance optimization
5. ✅ Contribute to the project

## Getting Help

### Online Resources
- NLM Documentation: https://nlm.readthedocs.io
- GitHub Repository: https://github.com/nlm-project/nlm
- Examples and Tutorials: Available in this repository

### Community
- Discussion forum (if available)
- GitHub issues and pull requests
- Academic papers and publications

### Local Help
- This examples directory
- The HOW_TO_USE.md and easy_usage.md files
- The source code examples in src/experiments/

## Best Practices

### For Beginners
1. **Start simple**: Use the provided examples exactly as written
2. **Understand the basics**: Make sure you understand brain, world, and agent concepts
3. **Experiment**: Change parameters and see what happens
4. **Document**: Keep notes of what you've tried and what works

### For Intermediate Users
1. **Explore systematically**: Try different combinations of configurations
2. **Analyze results**: Understand what the statistics mean
3. **Connect components**: See how brain, world, and agent interact
4. **Customize**: Modify examples to suit your needs

### For Advanced Users
1. **Optimize**: Understand performance trade-offs
2. **Extend**: Add new features and functionality
3. **Integrate**: Combine NLM with other systems
4. **Contribute**: Share your improvements with others

## Summary

This is the `easy_usage.md` guide, providing a beginner-friendly introduction to NLM.

Key points:
- NLM simulates spiking neural networks
- Three main components: Brain, World, Agent
- Focus on practical examples and clear explanations
- Building blocks for understanding more complex concepts

Use this guide as your starting point, then explore the more detailed documentation and examples for deeper understanding.

You're now ready to start using NLM!

---

## Version Information

- **NLM Version**: 0.1.0
- **Phase**: 6 (Final Integration)
- **Documentation**: Generated on November 8, 2026
- **License**: MIT

## Related Documentation

- **HOW_TO_USE.md**: Comprehensive C++ usage instructions
- **easy_usage.md**: This beginner-friendly guide
- **docs/ARCHITECTURE.md**: System architecture overview
- **docs/SCIENCE.md**: Scientific background
- **README.md**: Project overview and build instructions

---

## Conclusion

You've completed the basics of NLM! Here's what you've learned:

1. **Core Concepts**: Brain, World, Agent
2. **Basic Workflow**: Create → Initialize → Simulate → Monitor
3. **Practical Examples**: Multiple working examples
4. **Configuration**: How to customize NLM parameters
5. **Troubleshooting**: Common issues and solutions

You're now ready to:
- Build your own NLM applications
- Explore advanced features
- Contribute to the project
- Share your work with others

Welcome to the NLM community! 🚀