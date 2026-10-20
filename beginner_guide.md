# NLM (熙然) - Beginner-Friendly User Guide

## Welcome to NLM!

**NLM (熙然)** is a brain-inspired spiking neural network simulator that lets you create virtual brains and watch them learn, think, and behave just like real neural systems!

Think of NLM as building an artificial brain from the ground up, using millions of simulated neurons that communicate through electrical signals and learn from experience.

## What You Need to Know

Before we start, here are the three main concepts you need to understand:

### 1. **Brain** - Your virtual brain
- Made up of thousands of neurons and connections (synapses)
- Can learn, remember, and make decisions
- Think of it like a thinking machine!

### 2. **World** - The environment
- Where your brain lives and interacts
- Has things like objects, goals, and challenges
- Think of it like the brain's "body" and surroundings

### 3. **Agent** - The bridge between brain and world
- Connects your neural brain to the external world
- Takes sensory input (what the brain sees/hears)
- Sends motor output (what the brain decides to do)
- Think of it like the brain's "senses and motor system"

## Getting Started: Your First Virtual Brain

Let's start with the simplest possible example:

### Step 1: Install NLM
```bash
# Install from source (recommended for development)
pip install .

# Or install system dependencies
sudo apt-get install libyaml-cpp-dev libjsoncpp-dev

# Build
mkdir build
cd build
cmake ..
make -j4
```

### Step 2: Create Your First Brain
Open a Python interpreter or create a new file:

```python
import pynlm

# Step 1: Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize the brain (turn it on)
brain.initialize()

# Step 3: Make the brain think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Average firing rate:", brain.getAverageFiringRate())
```

### What Just Happened?
1. **Created Brain**: 1000 neurons were born
2. **Initialized**: The brain "woke up" and set up its internal state
3. **Thought**: For 100 "moments" in time, neurons sent signals to each other
4. **Learned**: Connections between neurons got stronger/weaker based on activity

## Your First Agent in a World

Now let's make your brain interact with a world:

```python
import pynlm

# Step 1: Create and initialize the brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Create a simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)  # Start at middle of world

# Step 3: Create the agent (brain-world connection)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 4: Run the simulation
for step in range(500):
    # 1. Update world state
    world.update(0.1)
    
    # 2. Get what the agent sees (sensory input)
    percept = world.getSensoryPercept()
    
    # 3. Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks
    brain.step(step)
    
    # 5. Brain decides what to do (action)
    action = agent.decodeMotorCommand()
    
    # 6. Do the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 7. Brain learns from the result (reward/modulation)
    agent.applyRewardModulation(0.1, 0.0)  # Small reward
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")

print("\n=== Simulation Complete! ===")
print(f"Total steps: {step + 1}")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
```

## Understanding the Loop

Every interaction between your brain and world follows this pattern:

```
World Update → Sensory Input → Brain Processing → Motor Output → Action → Result
```

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

## Common Brain Features

### 1. **Memory Systems**
Your brain has several memory systems:
- **Working Memory**: Short-term memory for current information
- **Episodic Memory**: Stores experiences as "episodes"
- **Associative Memory**: Links related experiences

### 2. **Learning Mechanisms**
- **STDP**: Spike-Timing-Dependent Plasticity ( strengthens connections based on timing )
- **Hebbian Learning**: "Neurons that fire together, wire together"
- **Reward Modulation**: Learn from rewards and punishments
- **Curiosity**: Explore new things to learn more

### 3. **Development**
Your brain matures over time:
- **Critical Period**: When learning is most rapid
- **Maturation**: Brain becomes more efficient
- **Aging**: Some abilities decline, others improve

## Experiment Examples

### Example 1: Silent Brain (Just Neurons)
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Brain Watching a World
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

### Example 3: Complete Agent with Learning
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
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Give reward for staying alive
    agent.applyRewardModulation(0.01, 0.0)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Common Problems and Solutions

### Problem: "My brain isn't doing anything"
**Solution:**
- Did you call `brain.initialize()`? Always call this first!
- Try increasing the number of steps
- Check that sensory input is connected to your brain
- Add some current injection to neurons

### Problem: "The agent isn't moving"
**Solution:**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that `world.getSensoryPercept()` returns valid data

### Problem: "Everything is 0"
**Solution:**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that motor commands are being generated properly

## Getting Help

### Basic Commands
```python
# Get brain statistics
print("Neurons:", brain.getTotalNeuronCount())
print("Synapses:", brain.getTotalSynapseCount())
print("Firing:", brain.getFiringNeuronCount())
print("Spikes:", brain.getTotalSpikeCount())
print("Firing rate:", brain.getAverageFiringRate())
print("Development:", brain.getDevelopmentalStage())

# Get agent statistics
print("Curiosity:", agent.getCuriosityLevel())
print("Novelty:", agent.getNoveltyLevel())
print("Neuromodulation:", agent.getNeuromodulationLevel())
```

### Configuration Options
```python
# Change brain size
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)

# Enable/disable features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Change world properties
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
```

### Saving and Loading
```python
# Save brain state
brain.save("my_brain.bin")

# Load brain state
brain.load("my_brain.bin")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")
```

## Next Steps

When you're comfortable:

1. **Read the documentation**: Check out `HOW_TO_USE.md` for more technical details
2. **Explore the architecture**: Read `docs/ARCHITECTURE.md` to understand how everything works
3. **Try the experiments**: Look at `src/experiments/` for different use cases
4. **Experiment with configurations**: Try different parameters in your configurations
5. **Learn about learning**: Understand how neuromodulation and plasticity work

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

## Frequently Asked Questions

**Q: Do I need to understand neural networks to use NLM?**
**A:** No! NLM is designed to be intuitive, even for beginners. You can start with simple examples and gradually learn as you go.

**Q: How long should simulations run?**
**A:** Start with 100-1000 steps for testing, then increase to 10,000+ for real experiments.

**Q: What are good parameter values?**
**A:** Use the default config as a starting point, then experiment with values that make sense for your use case.

**Q: Can I use NLM for machine learning?**
**A:** Yes! NLM is a research project that explores brain-inspired learning. It's different from traditional ML but very powerful for certain applications.

## Happy Brain Building!

You've now completed the beginner guide! You're ready to start exploring the fascinating world of artificial neural systems. Remember:

1. **Start simple** - Use the examples as your foundation
2. **Learn gradually** - Understand one concept at a time
3. **Experiment** - Try different parameters and configurations
4. **Ask questions** - The NLM community is here to help

Your virtual brain is waiting to learn, grow, and explore the world!

---
**Need help?** Check out the NLM documentation or community forums for more advanced tutorials and examples.

**Resources:**
- README.md: Project overview and build instructions
- HOW_TO_USE.md: Detailed technical documentation
- easy_usage.md: This beginner-friendly guide
- docs/ARCHITECTURE.md: System architecture details
- configs/: Example configuration files
- src/experiments/: Real experiment examples

---