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

## New! Advanced Features

The latest version includes many new convenience functions that make advanced NLM usage easier:

### 1. Create Standard Configuration

```python
import pynlm

# Create a standard configuration for general NLM use
config = pynlm.createStandardConfig()

# Use it with brain creation
brain = pynlm.createBrain(config)
brain.initialize()
print("Standard config created with optimal brain settings")
```

### 2. Get Comprehensive Brain Statistics

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get comprehensive statistics as JSON
stats = pynlm.getBrainStatistics(brain)
print("Brain Statistics:", stats)
```

### 3. Validate Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration
try:
    validation_result = pynlm.validateConfig(config)
    if validation_result != '{"valid": true}':
        print("Configuration validation failed:", validation_result)
    else:
        print("Configuration is valid!")
except Exception as e:
    print("Configuration error:", str(e))
```

### 4. Safe Brain Operations

```python
import pynlm

# Create and initialize brain safely
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Safe initialization with error checking
try:
    pynlm.safeBrainInitialize(brain)
    print("Brain initialized successfully!")
    
    # Verify brain is in valid state
    neuron_count = brain.getTotalNeuronCount()
    print(f"Brain has {neuron_count} neurons")
    
except pynlm.BrainError as e:
    print(f"Brain initialization failed: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 5. Create Training Environment

```python
import pynlm

# Create a complete training environment with optimal settings
world, brain, agent = pynlm.createTrainingEnvironment(
    width=30, 
    height=30, 
    visionWidth=6, 
    visionHeight=6,
    learningRate=0.01,
    explorationRate=0.1
)

print("Training environment created!")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")

# Run training for 100 steps
for step in range(100):
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinking
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Training complete!")
```

### 6. Create Development Stage Configuration

```python
import pynlm

# Create different development stage configurations
stages = ["initial", "critical", "maturation", "adult"]

for stage in stages:
    config = pynlm.createDevelopmentConfig(stage)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created {stage} stage brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Clean up
    brain.reset()
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

### New Project 4: Advanced Agent with Error Handling

```python
import pynlm

# Setup advanced agent with comprehensive error handling
try:
    # Create training configuration
    config = pynlm.createTrainingConfig(
        numSteps=500,
        enableRewardModulation=True,
        enableCuriosity=True,
        enableDevelopment=True
    )
    
    # Create advanced training environment
    world, brain, agent = pynlm.createTrainingEnvironment(
        width=20,
        height=20,
        visionWidth=6,
        visionHeight=6,
        learningRate=0.01,
        explorationRate=0.15
    )
    
    print("Advanced agent setup complete!")
    
    # Run advanced simulation with progress tracking
    def progress_callback(completed, progress):
        if completed % 50 == 0:
            print(f"Progress: {progress:.1f}% - "
                  f"Neurons: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}")
    
    result = pynlm.runSimulationWithProgress(
        brain, world, agent, 500, progress_callback
    )
    
    completed, duration = result
    print(f"Advanced simulation completed in {duration} seconds")
    print(f"Successfully processed {completed} steps")
    
    # Save brain checkpoint for later analysis
    pynlm.checkpointBrain(brain, "advanced_brain_checkpoint.json")
    print("Brain checkpoint saved")
    
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.WorldError as e:
    print(f"World error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
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

## New Troubleshooting Section

### Common Advanced Issues

**"Configuration validation failed"**
- Check that all required configuration parameters are set
- Use `pynlm.validateConfig()` to get detailed error messages
- Try using `pynlm.createStandardConfig()` for a known-good configuration

**"Brain initialization errors"**
- Ensure brain configuration is valid using `pynlm.validateConfig()`
- Try `pynlm.createDevelopmentConfig()` for specific developmental stages
- Use `pynlm.safeBrainInitialize()` for error-checked initialization

**"Training environment issues"**
- Check world configuration with `world.configure()`
- Verify agent initialization with `agent.initialize(world)`
- Try `pynlm.createTrainingEnvironment()` for optimized setup

**"Performance problems"**
- Reduce simulation steps for debugging
- Use `pynlm.getBrainStatistics()` to identify bottlenecks
- Try `pynlm.createDevelopmentConfig("adult")` for simpler behavior

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Covers advanced configuration options
   - Details for performance optimization
   - Troubleshooting guide for complex scenarios

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - Deep dive into NLM architecture
   - Scientific background and theory
   - Mathematical models and algorithms

3. **Experiment with different configurations!**
   - Try different developmental stages with `pynlm.createDevelopmentConfig()`
   - Use `pynlm.createTrainingConfig()` for specialized training
   - Test `pynlm.createStandardConfig()` for general-purpose brains

4. **Explore the new convenience functions**
   - `pynlm.getConvenienceFunctions()` to see all available functions
   - `pynlm.validateConfig()` for configuration validation
   - `pynlm.safeBrainInitialize()` for robust initialization

5. **Join the NLM community**
   - Share your discoveries and experiments
   - Get help with advanced usage
   - Contribute to the NLM ecosystem

That's it! You're now ready to use NLM with all the advanced features and convenience functions!

## Version Information

- **NLM Python Bindings Version:** 1.0.0
- **Build Date:** $(date +%Y-%m-%d)
- **API Version:** 1.0

## Key Improvements in This Version

This version includes significant improvements:

1. **Enhanced Error Handling**: Custom exception classes (ConfigurationError, BrainError, WorldError, etc.)
2. **Convenience Functions**: 20+ new functions for common use cases
3. **Advanced Documentation**: Comprehensive examples and usage patterns
4. **Configuration Management**: Validation, loading, and saving utilities
5. **Training Support**: Dedicated training configuration and environment setup
6. **Progress Tracking**: Real-time feedback for long-running simulations
7. **Batch Processing**: Tools for processing multiple brains efficiently
8. **Development Stages**: Configuration for different developmental phases
9. **Type Safety**: Better input validation and error checking
10. **Performance**: Optimized functions and batch operations

These improvements make NLM more powerful, user-friendly, and suitable for both beginners and advanced users!