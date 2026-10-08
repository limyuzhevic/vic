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

### Example 3: Working with Configuration Files (NEW!)

```python
import pynlm
import json

# Create brain with custom configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)  # More neurons
config.set("plasticity.stdp.enable", True)  # Enable STDP learning
config.set("neuromod.dopamine.scale", 1.5)  # Stronger dopamine signals

brain = pynlm.createBrain(config)
brain.initialize()

print("Custom configuration:")
print(brain.getConfig().summary())
```

### Example 4: Saving and Loading Configuration (NEW!)

```python
import pynlm

# Create and configure a brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

# Save configuration to file (auto-detects JSON/YAML)
config.saveToFile("my_brain_config.json")  # or ".yaml" or ".cfg"
print("Configuration saved to my_brain_config.json")

# Create a new brain and load the saved config
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_brain_config.json")

new_brain = pynlm.createBrain(new_config)
new_brain.initialize()

print("Loaded configuration:")
print(new_brain.getConfig().summary())
```

### Example 5: Working with Vector Configuration (NEW!)

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Set vector configuration values
brain.getConfig().set("learning.rate_list", [0.001, 0.01, 0.1])
brain.getConfig().set("noise.levels", [0.1, 0.05, 0.01])
brain.getConfig().set("temperature.values", [25.0, 27.5, 30.0])

# Get vector configuration values
rate_list = brain.getConfig().get("learning.rate_list")
print("Learning rates:", rate_list.value if rate_list else "Not found")

noise_levels = brain.getConfig().get("noise.levels")
print("Noise levels:", noise_levels.value if noise_levels else "Not found")

# Check all configuration keys
keys = brain.getConfig().getKeys()
print("All configuration keys:", keys)
```

### Example 6: Enhanced Agent with Learning Features (UPDATED!)

```python
import pynlm

# Setup with enhanced learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent with learning
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Enhanced agent simulation complete!")
```

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

### Project 3: Complete Agent with Configuration

```python
import pynlm

# Setup with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller brain for testing
config.set("world.width", 15)
config.set("world.height", 15)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
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
print("Configuration used:", brain.getConfig().summary())
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
| Set config value | `config.set("key", value)` |
| Get config value | `config.get("key")` |
| Load from file | `config.loadFromFile("file.json")` |
| Save to file | `config.saveToFile("file.json")` |

---

## Troubleshooting

**"Configuration file not found"**
- Ensure the file path is correct
- Check that the file exists
- Files can be .json, .yaml, .yml, or .cfg format

**"Invalid configuration format"**
- JSON files must be valid JSON
- YAML files must be valid YAML
- .cfg files must use key=value format

**"Vector type mismatch"**
- Configuration arrays are automatically typed based on content
- All elements in an array must be of the same type

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more advanced documentation
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configuration formats!
4. Try saving and loading complex brain states

That's it! You're now ready to use NLM with advanced configuration features!

---

## New Configuration Features Summary

✅ **JSON Support**: Load/save configurations in JSON format
✅ **YAML Support**: Load/save configurations in YAML format  
✅ **Vector Support**: Work with integer, double, and string arrays
✅ **Type Safety**: Automatic type detection and validation
✅ **Backward Compatibility**: Existing .cfg files still work
✅ **Error Handling**: Robust error handling with detailed messages
✅ **Enhanced Python API**: More configuration options exposed to Python