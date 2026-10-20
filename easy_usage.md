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

## Advanced Features

### Pattern 1: Run a Simulation with Error Handling

```python
import pynlm
import traceback

def run_simulation(brain, world, agent, num_steps):
    try:
        for step in range(num_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    except Exception as e:
        print(f"Simulation error: {e}")
        traceback.print_exc()
        return False
    return True
```

### Pattern 2: Get Comprehensive Brain Statistics

```python
# Just add these inside your loop:
stats = pynlm.getBrainStatistics(brain)
print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

### Pattern 3: Batch Processing

```python
import pynlm

# Process multiple sensory inputs in batch
percepts = [world.getSensoryPercept() for _ in range(10)]
pynlm.batchProcessSensoryInput(agent, percepts)

# Decode multiple motor commands in batch
commands = pynlm.batchDecodeMotorCommands(agent, 10)
for cmd in commands:
    print(f"Command: {cmd}")
```

### Pattern 4: Save and Load Checkpoints

```python
import pynlm

# Save brain checkpoint
if pynlm.saveBrainCheckpoint(brain, "my_brain_checkpoint.bin"):
    print("Brain checkpoint saved successfully!")

# Load brain checkpoint  
if pynlm.loadBrainCheckpoint(brain, "my_brain_checkpoint.bin"):
    print("Brain checkpoint loaded successfully!")
```

### Pattern 5: Configuration Management

```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()

# Add validation rules (advanced feature)
config.addValidationRule("brain.neuron_count", "Neuron count validation", 
                         lambda x: x > 0 and x <= 10000, "Neuron count must be 1-10000")

# Validate configuration
if pynlm.validateConfig(config):
    print("Configuration is valid!")

# Get configuration summary with validation
print(pynlm.getConfigSummary(config))
```

---

## Common Problems and Solutions

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### "Checkpoint save/load failed"
- Check if you have write permissions to the directory
- Try using absolute paths: "C:/checkpoints/brain.bin"

### "Configuration validation failed"
- Check the validation error messages with `getConfigSummary()`
- Ensure required keys are present and have valid values

---

## Next Steps

When you're comfortable:
1. **Read `HOW_TO_USE.md` for more details** - Covers advanced configuration options
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into the brain architecture
3. **Experiment with different configurations** - Try changing neuron_count, connection_probability, etc.
4. **Try the Phase 6 demo** - Run `./nlm_phase6_demo` to see the integrated brain in action

## Advanced Usage Tips

### Performance Tips:
- Use batch processing for multiple sensory inputs: `pynlm.batchProcessSensoryInput()`
- Run longer simulations efficiently with error handling
- Use checkpointing to save progress and resume later

### Debugging Tips:
- Use `getBrainStatistics()` to monitor brain activity
- Check configuration validation messages for detailed errors
- Monitor firing rates and spike counts to understand brain dynamics

### Best Practices:
- Always call `initialize()` before using any component
- Use `reset()` to start fresh simulations
- Save checkpoints frequently during long-running simulations
- Validate configurations before creating brains

That's it! You're now ready to use NLM at both beginner and advanced levels.

---

## New Advanced Features Added in Phase 6:

1. **Configuration Validation**: Comprehensive validation with detailed error messages
2. **Batch Processing**: Efficient processing of multiple inputs and outputs
3. **Enhanced Error Handling**: Better error messages and exception handling
4. **Advanced Simulation Utilities**: Helper functions for complex simulations
5. **Checkpoint Management**: Improved save/load utilities with validation
6. **Performance Monitoring**: Statistics and analysis helpers
7. **Memory Management**: Better resource handling and cleanup

These features make NLM more robust, user-friendly, and suitable for both research and production use.
