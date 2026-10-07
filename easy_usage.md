# NLM - Enhanced Usage Guide for Beginners and Power Users

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns
- **Development** = The brain maturing and changing over time

That's it! NLM simulates all of this with real neural computation.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks and learns
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

### Example 2: Enhanced Brain with Testing

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)  # More neurons
config.set("simulation_timestep", 0.0005)  # Faster simulation

brain = pynlm.createBrain(config)
brain.initialize()

print("Brain initialized with", brain.getTotalNeuronCount(), "neurons")

# Run neural computation for many steps
for i in range(1000):
    brain.step(i)

print("Simulation complete! Total spikes:", brain.getTotalSpikeCount())
print("Average firing rate:", brain.getAverageFiringRate())
```

### Example 3: Brain in a Simple World with Enhanced Features

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore new things
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

# Run for 100 steps
for step in range(100):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Average firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
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
| Your brain develops | Brain naturally matures over time |

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
print("Memory usage:", brain.getMemoryUsage())
print("Learning rate:", brain.getLearningRate())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore new things
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enablePrediction(True)           # Make predictions
agent.enableSocialLearning(True)        # Learn from others
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
- Learns from experience
- Stores memories

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `brain.getTotalSpikeCount()`
Get total number of spikes (brain activity)

### `brain.getAverageFiringRate()`
Get average firing rate (how active the brain is)

### `brain.getDevelopmentalStage()`
Get current developmental stage (how mature the brain is)

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
print("Total spikes:", brain.getTotalSpikeCount())
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

### Project 4: Enhanced Test Mode

```python
import pynlm
import subprocess

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run enhanced test with command line options
subprocess.run([
    "python3", "test_runner.py",
    "--verbose",
    "--neurons", "2000",
    "--steps", "2000",
    "--trials", "100"
])
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
| Get learning stats | `brain.getLearningRate()` |
| Get development stage | `brain.getDevelopmentalStage()` |

---

## Advanced Configuration Options

### Creating Custom Configurations

```python
import pynlm

# Create custom config
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("neuron_count", 2000)  # More neurons
config.set("simulation_timestep", 0.0005)  # Faster
config.set("connection_probability", 0.2)  # More connections
config.set("random_seed", 12345)  # Deterministic results

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Command Line Options for Advanced Users

```bash
# Enhanced command line options
nlm_phase2 [options]

Basic Options:
  -v, --verbose           Enable verbose logging (DEBUG level)
  -q, --quiet             Quiet mode (ERROR level only)
  -h, --help              Show this help message

Configuration:
  --config=FILE           Load configuration from FILE
                        (default: configs/default.cfg)
  --output=FILE           Save test results to FILE

Test Control:
  --no-connectivity       Skip basic connectivity test
  --no-plasticity         Skip plasticity learning test
  --no-stdp               Skip STDP verification test
  --no-all                Skip all tests

Test Parameters:
  --neurons=N             Set neuron count (default: 500)
  --steps=N               Set simulation steps for plasticity test (default: 1000)
  --trials=N              Set STDP trials for verification (default: 50)
  --step=VALUE            Set simulation timestep (default: 0.001)

Output Options:
  --save-results          Save detailed results to file
  --generate-report       Generate comprehensive test report
  --format=FORMAT         Report format: text, json, xml (default: text)

Examples:
  nlm_phase2 --verbose
  nlm_phase2 --neurons=1000 --steps=2000 --trials=100
  nlm_phase2 --no-stdp --generate-report --output=results.txt
  nlm_phase2 --config=my_config.cfg --save-results
```

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that you have enough neurons

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Make sure motor neurons are connected

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"Performance is slow"**
- Reduce neuron count
- Increase simulation timestep
- Skip unnecessary features

**"Results aren't reproducible"**
- Set a specific random_seed in config
- Use the same configuration file

---

## Next Steps

When you're comfortable:

1. **Read HOW_TO_USE.md** for more detailed documentation
2. **Read docs/ARCHITECTURE.md** to understand how it all works
3. **Experiment with different configurations!**
4. **Create your own mini projects**
5. **Use the enhanced command-line interface**

That's it! You're now ready to use NLM at both beginner and advanced levels.

## Enhanced Features

This version includes:
- **Improved error handling** - Better crash recovery
- **Enhanced logging** - More detailed output
- **Flexible configuration** - Command-line and config file support
- **Advanced testing** - Separate test suites with better reporting
- **Memory safety** - Proper null checks and validation
- **Performance optimizations** - More efficient test execution
- **Better documentation** - Comprehensive guides for all users

You now have access to a robust, well-tested neural simulation system that's easy to use for beginners but powerful enough for advanced research and experimentation!
