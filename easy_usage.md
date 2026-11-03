# NLM - Enhanced Easy Usage Guide for Beginners

## What is NLM?

NLM (Neural Learning Machine) is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

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

### Example 2: Brain in a Simple World (Enhanced)

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

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

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
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
```

### Example 3: Using Advanced Configuration (NEW!)

```python
import pynlm

# Create an advanced configuration with custom parameters
config = pynlm.createAdvancedConfig(
    neuronCount=2000,
    connectionProbability=0.08
)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with advanced features
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward based on agent's state
    agent.applyRewardModulation(world.getAgentBody().energy * 0.01, 0.0)

print("Advanced simulation complete!")
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

### Pattern 1: Run a Simulation (Enhanced)

```python
def run_simulation(brain, world, agent, num_steps, enable_learning=True):
    """Run a complete simulation with optional learning features."""
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
        
        # 7. Apply learning (if enabled)
        if enable_learning and step % 10 == 0:
            body = world.getAgentBody()
            reward = min(body.energy / 100.0, 1.0)  # Normalized reward
            agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development system
        agent.updateDevelopment(0.1)

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Comprehensive Brain Stats

```python
# Enhanced statistics gathering
def log_simulation_stats(brain, agent, step):
    """Log comprehensive statistics during simulation."""
    print(f"Step {step}:")
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")

# Add these inside your loop:
if step % 50 == 0:
    log_simulation_stats(brain, agent, step)
```

### Pattern 3: Enable Advanced Brain Features

```python
# These are all optional and can be enabled at different times:
def setupAdvancedFeatures(agent, config):
    """Enable all advanced features for complex simulations."""
    # Core learning features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableCuriosity(True)             # Explore new things
    
    # Structural and developmental features
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    
    # Additional config optimizations
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("development.enabled", True)
    
    return agent, config

# Use it like this:
agent, config = setupAdvancedFeatures(agent, config)
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

### `pynlm.createAdvancedConfig(neuronCount, connectionProbability)`
**NEW!** Creates an advanced configuration with custom parameters.
- Optimized for simulations that need more control over setup

### `pynlm.runSimpleEpisode(brain, world, agent, numSteps)`
**NEW!** Runs a complete simulation episode automatically.
- Handles initialization and cleanup
- Applies simple reward based on agent state
- Returns total spike count for quick feedback

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `agent.applyRewardModulation(reward, predicted_reward)`
Tells the brain how well it did (for learning).

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
print(f"Neurons created: {brain.getTotalNeuronCount()}")
```

### Project 2: Brain Watching a World (Enhanced)

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
print("Active neurons:", brain.getFiringNeuronCount())
```

### Project 3: Complete Advanced Agent (NEW!)

```python
import pynlm

# Setup
config = pynlm.createAdvancedConfig(
    neuronCount=1500,
    connectionProbability=0.06
)
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

# Run agent with advanced features
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Advanced reward based on energy
    body = world.getAgentBody()
    reward = max(0.0, (body.energy - 20.0) / 80.0)  # Threshold at 20 energy
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")

print("Advanced agent simulation complete!")
print(f"Final preference: {brain.getLearnedPreference():.3f}")
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
| Create advanced config | `pynlm.createAdvancedConfig(neurons, density)` |
| Run complete episode | `pynlm.runSimpleEpisode(brain, world, agent, steps)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neurons have enough input

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent's brain is decoding actions correctly

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if reward modulation is enabled for learning

**"Python import fails"**
- Ensure NLM is installed: `pip install pynlm`
- Check C++ build: `python setup.py build_ext --inplace` or `pip install -e .`

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more detailed explanations
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Try creating your own custom configurations
4. Experiment with different parameter combinations!
5. Explore the Phase 6 integration examples in the C++ demos

That's it! You're now ready to use NLM with enhanced functionality and advanced features!

---

## Advanced Usage Tips

### Performance Optimization
- Use `createAdvancedConfig()` for better performance with larger networks
- Enable only necessary features to reduce overhead
- Monitor firing rates to detect network saturation

### Debugging
- Use the new `exportToDict()` method on Config for debugging
- Check system info with `pynlm.getSystemInfo()`
- Monitor curiosity and novelty levels for exploration behavior

### Experimentation
- Try different neuron counts (500-5000+)
- Vary connection probabilities (0.01-0.2)
- Experiment with different timestep values
- Combine different plasticity rules

Enjoy exploring the neural learning machine! 🚀