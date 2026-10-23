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

### Pattern 4: Advanced Configuration and Fine-tuning

```python
# Advanced agent setup with fine-tuned parameters
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure advanced parameters for specific behavior
agent.setCuriosityThreshold(0.2)        # More exploratory
agent.setExplorationMaxChance(0.5)     # Higher random exploration
agent.setMotorActivationThreshold(0.3)  # Lower threshold for action
agent.setVisionInputScale(3.0)          # Stronger vision signals
agent.setCuriosityNoveltyFactor(3.0)    # Higher curiosity from novelty
agent.setPlasticityFactor(1.5)          # Enhanced plasticity
agent.setBasePlasticity(0.7)            # Higher base plasticity

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Print agent configuration
print("=== Agent Configuration ===")
print(f"Curiosity Level: {agent.getCuriosityLevel():.3f}")
print(f"Motor Threshold: {agent.getMotorActivationThreshold():.3f}")
print(f"Plasticity Factor: {agent.getPlasticityFactor():.3f}")
print(f"Development Stage: {agent.getDevelopmentalStage().name}")
print(f"Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
```

### Pattern 5: Development Monitoring and Control

```python
# Monitor and control brain development
agent.updateDevelopment(10.0)  # Fast-forward development

# Check development stages and plan accordingly
stage = agent.getDevelopmentalStage()
if stage.name == "Initial":
    print("Brain is in high plasticity phase - perfect for learning")
elif stage.name == "Adult":
    print("Brain is mature - stable performance")
else:
    print(f"Brain is in {stage.name} phase")

# Adjust plasticity based on performance
if agent.getAverageFiringRate() < 0.5:
    agent.setPlasticityFactor(2.0)  # Increase learning
else:
    agent.setPlasticityFactor(0.8)  # Maintain stability
```

### Pattern 6: Advanced Neuromodulation Control

```python
# Fine-tune neuromodulation for specific tasks
agent.applyRewardModulation(1.0, 0.5)   # Positive prediction error
agent.applyRewardModulation(-0.5, 0.0)  # Negative prediction error

# Monitor neuromodulation effects
print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Expected reward: {agent.getPredictionError():.3f}")
```

---

## Advanced Features Reference

### Configuration Parameters

| Parameter | Description | Default | Range | Unit |
|-----------|-------------|---------|--------|------|
| `curiosityThreshold` | Curiosity level to trigger exploration | 0.3 | [0.0, 1.0] | - |
| `explorationMaxChance` | Maximum exploration probability | 0.3 | [0.0, 1.0] | - |
| `rewardDecayRate` | Reward decay rate for expected reward | 0.05 | (0.0, 1.0] | - |
| `motorActivationThreshold` | Neural activity threshold for action | 0.5 | [0.0, 1.0] | - |
| `visionInputScale` | Vision signal scaling factor | 5.0 | [0.0, ∞) | - |
| `touchInputScale` | Touch signal scaling factor | 8.0 | [0.0, ∞) | - |
| `plasticityFactor` | Neuromodulation plasticity multiplier | 1.0 | [0.0, ∞) | - |
| `basePlasticity` | Base plasticity value | 0.5 | [0.0, ∞) | - |
| `curiosityNoveltyFactor` | Novelty contribution to curiosity | 2.0 | [0.0, ∞) | - |
| `curiosityPredictionFactor` | Prediction error contribution to curiosity | 0.5 | [0.0, ∞) | - |

### Development Stages

- **Initial**: High plasticity (value = 1.0)
- **Critical Period**: Medium-high plasticity (value = 0.8)
- **Maturation**: Medium plasticity (value = 0.5)
- **Adult**: Low plasticity (value = 0.2)
- **Aging**: Very low plasticity (value continues decreasing)

### Useful Methods

| Method | Purpose | Returns |
|--------|---------|---------|
| `getCuriosityLevel()` | Current curiosity state | float [0.0, 1.0] |
| `getNoveltyLevel()` | Recent environmental novelty | float [0.0, 1.0] |
| `getPredictionError()` | Difference between expected and actual reward | float |
| `getNeuromodulationLevel()` | Current neuromodulation signal | float [-1.0, 1.0] |
| `getDevelopmentalStage()` | Current developmental stage | DevelopmentalStage enum |
| `getAverageFiringRate()` | Overall neural activity | float |

### Configuration Examples

```python
# Example 1: Aggressive learner (for rapid skill acquisition)
agent.setPlasticityFactor(2.0)
agent.setCuriosityThreshold(0.1)
agent.setMotorActivationThreshold(0.2)
agent.enableDevelopment(True)

# Example 2: Stable performer (for maintenance)
agent.setPlasticityFactor(0.5)
agent.setCuriosityThreshold(0.5)
agent.setMotorActivationThreshold(0.8)
agent.enableDevelopment(False)

# Example 3: Balanced explorer (for general purpose)
agent.setPlasticityFactor(1.0)
agent.setCuriosityThreshold(0.3)
agent.setMotorActivationThreshold(0.5)
agent.enableDevelopment(True)
```

---

## Next Steps

When you're comfortable with the basic functionality:

1. **Read `docs/ARCHITECTURE.md`** to understand how it all works
2. **Read `HOW_TO_USE.md`** for more advanced usage patterns
3. **Experiment with different configurations** using the new parameters!
4. **Create custom development scenarios** using the fine-tuning options
5. **Monitor brain state** using the new getter methods for debugging

That's it! You're now ready to use NLM with advanced configuration and fine-tuning capabilities.

