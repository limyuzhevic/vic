# NLM - Enhanced Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this, and now with ENHANCED features!

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

### Example 3: Enhanced Brain with Learning

```python
import pynlm

# Create advanced configuration with enhanced features
config = pynlm.createDefaultConfig()

# Enable advanced learning features
config.set("brain.neuron_count", 2000)  # More neurons
config.set("plasticity.stdp.enable", True)  # Enable STDP learning
config.set("plasticity.hebbian.enable", True)  # Enable Hebbian learning
config.set("neuromod.dopamine.scale", 1.2)  # Enhanced dopamine effects
config.set("neuromod.curiosity.enable", True)  # Enable curiosity-driven exploration
config.set("neuromod.novelty.enable", True)  # Enable novelty detection

# Create and initialize brain with advanced capabilities
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)              # Explore new things

# Run enhanced simulation
print("Running enhanced simulation...")
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Track learning progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Enhanced simulation complete!")
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

### Pattern 1: Run a Complete Simulation

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with brain and world."""
    
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
        
        # 7. Optional: Track progress
        if step % 100 == 0:
            print(f"Progress: Step {step}/{num_steps}")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Learning rate: {agent.getNeuromodulationLevel():.3f}")

# Use it like this:
run_enhanced_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Advanced Brain Statistics

```python
# Add these inside your loop for advanced monitoring:
print("=== Advanced Brain Statistics ===")
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("E/I ratio:", brain.getExcitationInhibitionRatio())
print("Development stage:", brain.getDevelopmentalStage())

# Memory system stats
working_memory = brain.getWorkingMemory()
if working_memory:
    print("Working memory traces:", working_memory.getActiveTraces())

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print("Episodic episodes:", episodic_memory.getEpisodeCount())

# Neuromodulation stats
dopamine = brain.getDopamine()
if dopamine:
    print("Dopamine level:", dopamine.getLevel())

curiosity = brain.getCuriosity()
if curiosity:
    print("Curiosity level:", curiosity.getLevel())

novelty = brain.getNovelty()
if novelty:
    print("Novelty level:", novelty.getLevel())
```

### Pattern 3: Enable Advanced Brain Features

```python
# These are all the advanced optional features:

# 1. Reward-based learning (dopamine system)
agent.enableRewardModulation(True)

# 2. Structural plasticity (grow new connections)
agent.enableStructuralPlasticity(True)

# 3. Developmental adaptation
agent.enableDevelopment(True)

# 4. Curiosity-driven exploration
agent.enableCuriosity(True)

# 5. Novelty detection
agent.enableNoveltyDetection(True)

# 6. Prediction system
agent.enablePrediction(True)

# 7. Social learning
agent.enableSocialLearning(True)
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
- Stores experiences in memory
- Applies neuromodulation effects
- Makes adaptations based on development

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
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup with enhanced features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
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
print("Curiosity level:", agent.getCuriosityLevel())
```

### Project 3: Complete Agent with Learning

```python
import pynlm

# Advanced setup with all learning systems
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run sophisticated simulation
print("Running sophisticated learning simulation...")
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")

print("Sophisticated learning simulation complete!")
print(f"Final performance metrics:")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"  Total memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create basic brain | `pynlm.createBrain(config)` |
| Create enhanced brain | `pynlm.createBrain(config)` with advanced settings |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Get advanced stats | Use the statistics functions in Pattern 2 |

---

## Troubleshooting

### "My brain isn't learning enough"
- Did you call `agent.enableRewardModulation(True)`?
- Did you call `agent.enableStructuralPlasticity(True)`?
- Try increasing the number of steps to 1000+

### "The agent isn't exploring enough"
- Enable curiosity: `agent.enableCuriosity(True)`
- Enable novelty: `agent.enableNoveltyDetection(True)`

### "Everything is 0"
- Brains need time to "warm up" - try more steps (1000+)
- Some neurons need input to fire - make sure sensory input is connected
- Enable learning systems

### "Performance is slow"
- Try reducing neuron count for testing: `config.set("brain.neuron_count", 500)`
- Reduce simulation steps
- Enable performance optimizations

### "Memory systems not working"
- Make sure `agent.enableDevelopment(True)` is called
- Check if `brain.getWorkingMemory()` returns null
- Try running more simulation steps for memory formation

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Covers Python API in detail
   - Includes configuration examples
   - Advanced usage patterns

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - Deep dive into brain architecture
   - Technical implementation details
   - Advanced configuration options

3. **Experiment with different configurations**
   - Try different neuron counts
   - Test various plasticity settings
   - Explore neuromodulation parameters

4. **Try the enhanced demos**
   - Run `./nlm demo` for basic demo
   - Run `./nlm benchmark` for performance testing
   - Run `./nlm experiment` for full integration test

5. **Use the enhanced command line**
   - `nlm --help` for complete help
   - `nlm --version` for version info
   - `nlm demo --verbose` for detailed output

That's it! You're now ready to use NLM with all its enhanced features!

---

*Version: Enhanced Edition*
*Phase 6: Advanced Features*
*For complete documentation, see ENHANCED_DOCUMENTATION.md*
