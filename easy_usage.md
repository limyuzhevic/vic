# NLM - Easy Usage Guide for Beginners

**Phase 6 Enhanced Edition**

## What is NLM?

NLM (Neural Learning Machine) is a brain simulator for computers that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

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

### Example 3: Advanced Configuration

```python
import pynlm

# Advanced configuration with command-line presets
brain = pynlm.createBrain(pynlm.createConfig())
brain.loadFromArgs(["--preset=debug", "--enable-spike-tracing", "--verbose"])
brain.initialize()

# Run experiment
for step in range(1000):
    brain.step(step)

# Analysis
print("Brain Analysis:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

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

### Pattern 3: Enable Advanced Features

```python
# Enable advanced debugging and analysis
brain.enableAdvancedAnalysis()
brain.enableDetailedLogging()
brain.enablePerformanceMonitoring()

# Enable plasticity rules
brain.getSTDP()->enablePlasticity(True)
brain.getHebbian()->enablePlasticity(True)
brain.getStructuralPlasticity()->enablePlasticity(True)

# Enable neuromodulation
brain.getDopamine()->enableModulation()
brain.getCuriosity()->enableModulation()
brain.getNovelty()->enableModulation()

# Enable memory systems
brain.getWorkingMemory()->enableTracking()
brain.getEpisodicMemory()->enableStorage()
brain.getAssociativeMemory()->enableAssociation()
```

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
- Maintains memory systems
- Applies neuromodulation

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `brain.enableAdvancedAnalysis()`
Enables advanced analysis and debugging capabilities.

### `brain.enableDetailedLogging()`
Enables detailed logging for debugging and monitoring.

### `brain.enablePerformanceMonitoring()`
Enables performance monitoring and metrics collection.

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

### Project 3: Complete Agent with Learning

```python
import pynlm

# Setup with enhanced configuration
brain = pynlm.createBrain(pynlm.createConfig())
brain.loadFromArgs(["--preset=debug", "--enable-spike-tracing", "--verbose"])
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
brain.enableAdvancedAnalysis()
brain.getSTDP()->enablePlasticity(True)
brain.getHebbian()->enablePlasticity(True)
brain.getDopamine()->enableModulation()
brain.getCuriosity()->enableModulation()

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Analysis
print("Final Brain Status:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
```

## Advanced Configuration Options

### Command-Line Presets
```bash
# Use preset configurations
./nlm --preset=debug
./nlm --preset=production
./nlm --preset=research

# Custom configuration with advanced options
./nlm --neuron-count=2000 --region-count=4 --plasticity-mode=both \
  --dopamine-level=0.5 --curiosity-level=0.3 --enable-spike-tracing \
  --log-level=Debug --verbose
```

### Configuration File
```python
# Create custom configuration
config = pynlm.createConfig()
config.set("neuron_count", 1000)
config.set("region_count", 1)
config.set("connection_probability", 0.1)
config.set("stdp_ltp_weight", 0.01)
config.set("enable_spike_tracing", True)
config.set("log_level", "Info")

# Save configuration
config.saveToFile("custom_config.json")

# Load configuration
brain = pynlm.createBrain(config)
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
| Enable analysis | `brain.enableAdvancedAnalysis()` |
| Enable logging | `brain.enableDetailedLogging()` |
| Enable monitoring | `brain.enablePerformanceMonitoring()` |

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Enable detailed logging: `brain.enableDetailedLogging()`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Enable spike tracing: `brain.enableAdvancedAnalysis()`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Enable performance monitoring to debug issues

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Advanced C++ programming
   - Configuration options
   - Analysis tools
   - Performance optimization

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - System architecture
   - Component interactions
   - Design patterns

3. **Experiment with different configurations!**
   - Try different preset configurations
   - Customize configuration parameters
   - Use analysis tools to understand behavior

4. **Explore advanced features**
   - Advanced analysis and debugging
   - Performance optimization
   - Custom configuration files
   - Integration with other tools

That's it! You're now ready to use enhanced NLM Phase 6 with all its advanced features and capabilities.
