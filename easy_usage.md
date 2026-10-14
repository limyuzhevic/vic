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

## Quick Start (Copy & Paste) - Enhanced Version

### Example 1: Simplest Brain with Validation

```python
import pynlm

# Step 1: Make a brain (enhanced version)
config = pynlm.createDefaultConfig()  # Enhanced with sensible defaults
brain = pynlm.createBrain(config)     # Enhanced with validation

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Brain stats:", brain.get_simulation_stats())
```

### Example 2: Brain in a Simple World with Enhanced Features

```python
import pynlm
import random

# Create everything with enhanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features (enhanced)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run for 50 steps with enhanced logging
for step in range(50):
    world.update(0.1)  # Update world
    
    # Get what the agent sees (enhanced percept)
    percept = world.getSensoryPercept()
    
    # Check if percept is valid
    if percept.is_valid():
        print(f"Step {step}: Valid percept with {percept.get_sensory_count()} signals")
    
    # Process sensory input in brain (enhanced)
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain (enhanced)
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Enhanced progress tracking
    if step % 10 == 0:
        print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, " +
              f"Curiosity: {agent.getCuriosityLevel():.3f}")

print("Simulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Agent stats: {agent.get_agent_stats()}")
print(f"Brain state: {brain}")
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

# One line to create a virtual brain (enhanced)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("Brain created successfully:", brain)
```

---

## Common Patterns (Enhanced)

### Pattern 1: Run a Simulation with Validation

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
    """Run a complete enhanced simulation with validation."""
    
    # Validate initial state
    if not brain.validate_state():
        print("Warning: Initial brain state is invalid!")
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Validate percept before processing
        if not percept.is_valid():
            print(f"Warning: Invalid percept at step {step}")
            continue
        
        # 4. Tell the brain
        agent.processSensoryInput(percept)
        
        # 5. Brain thinks
        brain.step(step)
        
        # 6. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 7. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Enhanced progress tracking
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # Validate final state
    if brain.validate_state():
        print("Simulation completed successfully!")
    else:
        print("Warning: Final brain state is invalid!")
    
    return brain.get_simulation_stats()
```

### Pattern 2: Get Brain Stats (Enhanced)

```python
# Just add these inside your loop with enhanced logging:
def enhanced_status_monitor(brain, agent):
    print(f"Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Avg firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"Prediction error: {agent.getPredictionError():.3f}")
    print(f"Agent stats: {agent.get_agent_stats()}")
```

### Pattern 3: Enable Brain Features (Enhanced)

```python
# These are all optional with enhanced descriptions:
def create_enhanced_agent(config):
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize with world
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    
    # Enable learning features with descriptions
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    return brain, agent, world

# Use it like this:
brain, agent, world = create_enhanced_agent(config)
stats = run_enhanced_simulation(brain, world, agent, 1000)
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses (enhanced with validation).
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain! (Enhanced with validation)

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain (enhanced with validation).

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take (enhanced with validation).

---

## Mini Projects (Enhanced)

### Project 1: Silent Brain (Just Neurons) - Enhanced

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Final brain state: {brain}")
print(f"Final stats: {brain.get_simulation_stats()}")
```

### Project 2: Brain Watching a World - Enhanced

```python
import pynlm

# Setup with enhanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world for 30 steps with enhanced logging
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    # Enhanced logging
    if i % 10 == 0:
        print(f"Step {i}: Firing rate: {brain.getAverageFiringRate():.3f} Hz")
        print(f"  Agent stats: {agent.get_agent_stats()}")

print("Watched world for 30 steps")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

### Project 3: Complete Agent - Enhanced

```python
import pynlm
import random

def run_enhanced_agent_simulation(num_steps=100):
    """Run a complete enhanced agent simulation."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Add some interesting world objects
    for _ in range(5):
        x = random.uniform(5, 10)
        y = random.uniform(5, 10)
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=random.uniform(1, 5))
        world.addObject(obj)
    
    # Run agent
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Enhanced progress tracking
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, " +
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world

# Run the enhanced simulation
brain, agent, world = run_enhanced_agent_simulation(100)
print("Agent simulation complete!")
print(f"Final results: {brain.get_simulation_stats()}")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `brain = pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Validate config | `pynlm.validate_config(config)` |
| Get agent stats | `agent.get_agent_stats()` |
| Check if percept valid | `percept.is_valid()` |

---

## Troubleshooting (Enhanced)

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if configuration is valid: `pynlm.validate_config(config)`

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Validate agent's configuration: `agent.validate_configuration()`

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check percept validity: `percept.is_valid()`
- Validate world configuration: `world.configure()` with proper dimensions

### New: Common Validation Errors

**Configuration validation errors:**
```python
if not pynlm.validate_config(config):
    print("Configuration is invalid!")
    # In a real implementation, you might get error details:
    # errors = config.get_validation_errors()
```

**Brain state validation errors:**
```python
if not brain.validate_state():
    print("Brain state is invalid!")
    brain.logStatus()  # Log detailed error information
```

**Agent configuration validation errors:**
```python
if not agent.validate_configuration():
    print("Agent configuration is invalid!")
```

**World configuration errors:**
```python
# If world configuration seems wrong, check:
print(f"World dimensions: {world.getWidth()}x{world.getHeight()}")
print(f"Vision dimensions: {world.getVisionWidth()}x{world.getVisionHeight()}")
```

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - This comprehensive guide includes advanced features
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into the system architecture
3. **Experiment with different configurations!** - Try different brain sizes, connection probabilities, and learning rates
4. **Check the Advanced User Guide** - For power users and research applications

**You're now ready to use NLM with enhanced features!** The improved bindings provide better validation, error handling, and comprehensive statistics to help you create better simulations and understand your brain's behavior.

---

*Enhanced version includes: Configuration validation, state validation, comprehensive statistics, error handling, and improved documentation.*