# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**NEW IN VERSION 0.1.0 - Enhanced Python API!**
* Major improvements to the Python interface including JSON configuration support and convenience methods

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

### Example 2: Enhanced Brain with Configuration

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(50):
    brain.step(i)

print("Brain with enhanced configuration created!")
print("Total neurons:", brain.getTotalNeuronCount())
print("Total synapses:", brain.getTotalSynapseCount())
```

**NEW** - Configuration with JSON support:
```python
# Save configuration to JSON
config.saveToFile("my_config.json")

# Load configuration from JSON
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

### Example 3: Complete Agent Simulation (Enhanced Version)

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 1)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features (NEW)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Enhanced agent simulation complete!")
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

# NEW - Get detailed status
status = brain.getStatus()
print("Brain status:\n" + status)
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

### Pattern 2: Get Enhanced Brain Stats (NEW)

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Learning progress:", brain.runSimulation([], 10))  # NEW!
```

### Pattern 3: Enhanced Brain Features

```python
# These are all optional (ENHANCED):
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enableNoveltyDetection(True)      # Detect interesting events
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

**NEW** - Enhanced configuration with JSON/YAML support available.

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

**NEW** - Enhanced with action creation and convenience methods.

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

# NEW - Enhanced statistics
print("Brain status:\n" + brain.getStatus())
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

# NEW - Enhanced monitoring
print("Brain status:\n" + brain.getStatus())
```

### Project 3: Complete Enhanced Agent

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

# Enable enhanced learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableNoveltyDetection(True)

# Run enhanced agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print("Learning progress:", brain.runSimulation([], 10))

print("Enhanced agent simulation complete!")
print("Final brain status:\n" + brain.getStatus())
```

### Project 4: Enhanced Configuration Management (NEW)

```python
import pynlm

# Create enhanced configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)

# Save to JSON
config.saveToFile("my_config.json")

# Load from JSON
config2 = nlm.createDefaultConfig()
if config2.loadFromFile("my_config.json"):
    print("✓ Configuration loaded successfully!")
    print(config2.summary())

# Enhanced validation
def validate_config(config):
    if config.has("neuron_count") and config.getInt("neuron_count", 0) < 100:
        print("⚠ Warning: Small brain size")
    return True

validate_config(config2)
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

**NEW** - Enhanced quick reference:
| Enhanced Feature | Code |
|-----------------|------|
| Enhanced stats | `brain.getStatus()` |
| Configuration | `config.getInt(key, default)` |
| Learning progress | `brain.runSimulation(input, steps)` |
| JSON support | `config.loadFromFile(filepath)` |
| Action creation | `pynlm.createAction(type, params)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that neurons are receiving input

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that the action is valid

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that configuration is valid

**"Configuration file not found"**
- Use `config.loadFromFile()` with error handling
- Try `safe_load_config()` utility (see examples)
- Ensure the file path is correct

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more technical details
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Try the enhanced examples** in `python_api_examples.md`
4. **Experiment with different configurations** using JSON/YAML files
5. **Use the enhanced convenience methods** for easier development

**NEW** - See the comprehensive examples in `PYTHON_API_IMPROVEMENTS.md` and `python_api_examples.md`

That's it! You're now ready to use the **enhanced** NLM with powerful new features!

## Key Enhancements You Get:

✅ **JSON/YAML Configuration Support** - Save and load brain configurations
✅ **Enhanced Convenience Methods** - Easier configuration access and manipulation  
✅ **Advanced Brain Statistics** - Detailed brain status and learning progress
✅ **Improved Error Handling** - Better error messages and validation
✅ **Action Creation Utilities** - Simplified action creation with parameters
✅ **Learning Progress Analysis** - Built-in metrics for learning assessment
✅ **Data Collection Tools** - Utilities for collecting simulation data
✅ **Enhanced Documentation** - Comprehensive examples and tutorials

The enhanced NLM Python API now provides everything you need for neural simulation research and development!
