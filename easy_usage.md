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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

## Learning Path

### Phase 1: Basic Usage (You are here)
- Create simple brains and run simulations
- Understand the brain-world-agent relationship
- Use basic examples from this guide

### Phase 2: Advanced Usage
- Enable learning systems (reward, curiosity, development)
- Create complex world environments
- Save and load brain states
- Work with different brain configurations

### Phase 3: Expert Usage
- Create custom worlds with obstacles and rewards
- Implement reinforcement learning scenarios
- Use advanced memory systems
- Performance optimization and debugging

## Error Handling

Even simple NLM code can encounter issues. Here's how to handle common problems:

```python
import pynlm
import traceback

def safe_run_simulation(brain_config, steps=100):
    try:
        # Create brain
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
            
            # Check for issues
            if brain.getFiringNeuronCount() == 0:
                print(f"Warning: No neurons firing at step {step}")
                
        return brain
        
    except Exception as e:
        print(f"Error during simulation: {e}")
        traceback.print_exc()
        return None

# Usage
config = pynlm.createDefaultConfig()
brain = safe_run_simulation(config, 500)
if brain:
    print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

## Common Troubleshooting

**"My brain isn't doing anything"
- Did you call `brain.initialize()`? (This is crucial!)
- Check that neuron parameters are set correctly
- Enable development to see changes over time

**"The simulation crashes with segmentation fault"
- Ensure `brain.initialize()` is called before `brain.step()`
- Check that you're not passing null/empty percepts to agent
- Use smaller brain sizes for debugging

**"Learning doesn't seem to work"
- Check that learning systems are enabled with `agent.enableRewardModulation(True)`
- Verify reward signals are being applied
- Ensure curiosity and development systems are working

**"Memory usage is too high"
- Consider reducing brain size
- Use checkpointing to save state frequently
- Monitor brain statistics regularly

## Performance Tips

```python
import pynlm
import time

def measure_performance(config, steps=1000):
    """Measure simulation performance"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    end_time = time.time()
    
    duration = end_time - start_time
    steps_per_second = steps / duration
    
    print(f"Simulation: {steps} steps in {duration:.2f}s")
    print(f"Performance: {steps_per_second:.1f} steps/second")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain, duration

# Use smaller brains for testing
config = pynlm.createDefaultConfig()
# config.set("brain.neuron_count", 100)  # Uncomment to customize
brain, duration = measure_performance(config)
```

## Advanced Configuration

```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()

# Modify brain parameters
config.set("brain.neuron_count", 500)           # Number of neurons
config.set("brain.synapse_density", 0.2)        # Connectivity
config.set("brain.v_thresh", -50.0)             # Threshold
config.set("brain.v_rest", -70.0)               # Resting potential

# Enable plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Enable development
config.set("development.enable", True)
config.set("development.initial_plasticity_rate", 0.1)

print("Configuration:")
print(config.summary())

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

## State Management

```python
import pynlm
import tempfile
import os

def save_load_demo(config, steps=100):
    """Demonstrate brain save/load functionality"""
    print("=== Brain Save/Load Demo ===")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        if step % 50 == 0:
            print(f"Step {step}")
    
    # Save brain state
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
        save_path = f.name
    
    if brain.save(save_path):
        print(f"Brain saved to: {save_path}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Neurons firing: {brain.getFiringNeuronCount()}")
        
        # Load brain state
        loaded_brain = pynlm.createBrain(config)
        if loaded_brain.load(save_path):
            print(f"Brain loaded successfully!")
            print(f"Loaded brain spikes: {loaded_brain.getTotalSpikeCount()}")
            
            # Continue simulation
            for step in range(steps, steps * 2):
                loaded_brain.step(step)
            
            print(f"After loading and continuing: {loaded_brain.getTotalSpikeCount()} total spikes")
        
        # Clean up
        os.unlink(save_path)
    else:
        print("Failed to save brain")
    
    return brain

# Run demo
config = pynlm.createDefaultConfig()
save_load_demo(config)
```

## Complete Example: Learning Agent

```python
import pynlm
import random

def create_learning_agent(config=None, steps=1000):
    """Create a complete learning agent with all systems enabled"""
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Setup brain with configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Add some resources to the world for the agent to discover
    for i in range(5):
        x = random.uniform(2, 18)
        y = random.uniform(2, 18)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
    
    # Run simulation with learning
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = 0.0
        if world.getAgentBody().energy > 50:
            reward = 1.0  # Positive reward for survival
        if random.random() < 0.1:  # Random resource discovery
            reward += 2.0
            
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
    
    print(f"\nFinal Statistics:")
    print(f"Total simulation steps: {steps}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

# Run complete learning agent
config = pynlm.createDefaultConfig()
# Customize config
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

brain, agent, world = create_learning_agent(config, 2000)
```

That's it! You're now ready to use NLM at an expert level with comprehensive error handling, performance monitoring, configuration management, and complete learning agents!
