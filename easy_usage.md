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

## Advanced Usage and New Features

### Pattern 4: Enhanced Brain Statistics and Monitoring

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
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
        
        # 7. Enhanced monitoring and statistics
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  - Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
            print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
            print(f"  - Prediction error: {agent.getPredictionError():.2f}")
        
        # 8. Enable learning features
        agent.applyRewardModulation(1.0, 0.5)  # Apply reward modulation
        agent.updateDevelopment(0.1)  # Update development

# Use it like this:
run_enhanced_simulation(brain, world, agent, 500)
```

### Pattern 5: Advanced Configuration Management

```python
# Create and configure a brain with advanced settings
config = nlm.createDefaultConfig()

# Set various configuration parameters
config.set("neuron_count", 500)  # Large brain
config.set("region_count", 5)    # Multiple regions
config.set("connection_probability", 0.15)
config.set("max_energy", 200.0)
config.set("energy_decay_rate", 0.01)

# Create brain with custom configuration
brain = nlm.createBrain(config)
brain.initialize()

print(f"Configured brain: {brain.getTotalNeuronCount()} neurons")
print(f"Regions: {brain.getRegionCount()}")

# Save and load configurations
brain.save("my_brain_config.json")
# ... later ...
brain.load("my_brain_config.json")
```

### Pattern 6: Complex World Environments

```python
# Create a more complex world configuration
world = nlm.createSimpleWorld()
world.configure(
    width=50,                    # Larger world
    height=50,
    visionWidth=12,              # Better vision range
    visionHeight=12
)
world.reset()
world.setAgentStart(25, 25)     # Center start position

# Add various world objects
# Resources (positive value)
resource1 = nlm.WorldObject(10, 10, nlm.WorldObjectType.Resource, 50.0)
resource2 = nlm.WorldObject(40, 30, nlm.WorldObjectType.Resource, 25.0)

# Hazards (negative value or special type)
hazard = nlm.WorldObject(30, 20, nlm.WorldObjectType.Hazard, -10.0)

# Walls (boundaries)
wall1 = nlm.WorldObject(0, 25, nlm.WorldObjectType.Wall)
wall2 = nlm.WorldObject(50, 25, nlm.WorldObjectType.Wall)

# Add objects to world
for obj in [resource1, resource2, hazard, wall1, wall2]:
    world.addObject(obj)

print(f"World ready: {world.getWidth()}x{world.getHeight()} with {len([obj for obj in world.getAllObjects() if obj.active])} objects")
```

### Pattern 7: Advanced Action Creation

```python
# Create sophisticated actions with parameters
# Movement actions with parameters
move_forward = nlm.Action(nlm.ActionType.MoveForward, [0.8])  # Speed 0.8
move_backward = nlm.Action(nlm.ActionType.MoveBackward, [0.5])

# Turn actions with speed parameters
turn_left = nlm.Action(nlm.ActionType.TurnLeft, [0.3])    # Slow turn
turn_right = nlm.Action(nlm.ActionType.TurnRight, [1.0])  # Fast turn

# Interaction actions
interact = nlm.Action(nlm.ActionType.Interact, [0.5])    # Interaction strength
eat = nlm.Action(nlm.ActionType.Eat, [0.7])             # Eating action

# Custom actions
custom_action = nlm.Action(nlm.ActionType.Custom, [0.1, 0.2, 0.3, 0.4, 0.5])

# Clone and modify actions
action_copy = custom_action.clone()
action_copy.setType(nlm.ActionType.TurnLeft)
action_copy.setParameters([0.5])

print(f"Original action type: {custom_action.getType()}")
print(f"Cloned action type: {action_copy.getType()}")
print(f"Original params: {custom_action.getParameters()}")
print(f"Cloned params: {action_copy.getParameters()}")
```

### Pattern 8: Agent with Learning and Development

```python
# Create agent with advanced learning capabilities
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)   # Grow new connections
agent.enableDevelopment(True)            # Brain matures over time
agent.enableCuriosity(True)              # Explore new things

print("Agent configured with advanced learning capabilities")
print(f"Input size: {agent.getSensoryInputSize()}, Output size: {agent.getMotorOutputSize()}")

# Run advanced simulation with learning
for step in range(100):
    world.update(0.05)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Agent decides action
    action = agent.decodeMotorCommand()
    
    # Execute action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning based on experience
    agent.applyRewardModulation(1.0, 0.3)  # Small reward, smaller expectation
    agent.updateDevelopment(0.05)        # Small development step
    
    # Monitor learning progress
    if step % 25 == 0:
        neuromod = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        print(f"  Step {step}: Neuromod={neuromod:.2f}, Curiosity={curiosity:.2f}")
```

### Pattern 9: Error Handling and Validation

```python
# Advanced error handling for robust simulations
import traceback

def safe_simulation_step(brain, world, agent, step):
    """Safe simulation step with error handling."""
    try:
        # Validate agent state
        if not agent.getBrain():
            raise ValueError("Agent not properly initialized")
        
        # Validate brain state
        if brain.getTotalNeuronCount() == 0:
            raise RuntimeError("Brain has no neurons")
        
        # Validate world state
        if not world.getAgentBody():
            raise RuntimeError("World has no agent")
        
        # Execute simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Monitor and log
        print(f"Step {step} successful")
        return True
        
    except ValueError as ve:
        print(f"Configuration error: {ve}")
        return False
    except RuntimeError as re:
        print(f"Runtime error: {re}")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        traceback.print_exc()
        return False

# Run safe simulation
for step in range(50):
    if not safe_simulation_step(brain, world, agent, step):
        print(f"Simulation halted at step {step}")
        break
```

### Pattern 10: Complete Advanced Brain System

```python
def create_advanced_brain_system(config_file=None):
    """Create a complete advanced brain system with all features."""
    
    # Create configuration
    if config_file:
        config = nlm.createDefaultConfig()
        config.load(config_file)
    else:
        config = nlm.createDefaultConfig()
        # Custom advanced configuration
        config.set("neuron_count", 1000)
        config.set("region_count", 8)
        config.set("connection_probability", 0.12)
        config.set("max_energy", 500.0)
        config.set("development_stage", "Adult")
    
    # Create and initialize brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
    world.reset()
    
    # Create agent with all learning capabilities
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure agent learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, world, agent

# Use the advanced system
brain, world, agent = create_advanced_brain_system()

print("Advanced brain system created!")
print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getRegionCount()} regions")
print(f"World: {world.getWidth()}x{world.getHeight()} grid")
print(f"Agent: Input size {agent.getSensoryInputSize()}, Output size {agent.getMotorOutputSize()}")
print(f"Learning enabled: Reward={agent.isRewardModulationEnabled()}, Curiosity={agent.isCuriosityEnabled()}")

# Run advanced simulation
for step in range(200):
    world.update(0.05)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(0.5, 0.2)  # Learning
    agent.updateDevelopment(0.01)        # Development
    
    if step % 50 == 0:
        print(f"Progress: Step {step}, Spikes: {brain.getTotalSpikeCount()}, Firing: {brain.getFiringNeuronCount()}")
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

**"Learning not working"**
- Make sure to enable learning features: `agent.enableRewardModulation(True)`
- Check that reward modulation is being applied: `agent.applyRewardModulation(reward, expected)`

**"Development not progressing"**
- Enable development: `agent.enableDevelopment(True)`
- Call development updates: `agent.updateDevelopment(timestep)`

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Test the new advanced patterns and Pythonic interfaces
5. Use the `test_new_features.py` script to verify all improvements

That's it! You're now ready to use NLM with all the latest improvements and features!
