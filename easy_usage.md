# NLM - Easy Usage Guide for Beginners (Enhanced)

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

### Example 2: Brain in a Simple World (Enhanced)

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

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

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

### NEW: Example 3: Complete Simulation with Statistics

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.create_standard_config()  # NEW: factory function
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run complete simulation with statistics
num_steps = 1000
for step in range(num_steps):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward-based learning
    agent.applyRewardModulation(0.1, 0.0)  # Simple reward
    agent.updateDevelopment(0.1)
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Complete simulation finished!")

# Get comprehensive statistics
brain_stats = brain.get_statistics()  # NEW: convenience method
agent_status = agent.get_agent_status()  # NEW: convenience method
world_state = world.get_world_state()  # NEW: convenience method

print("Brain Statistics:", brain_stats)
print("Agent Status:", agent_status)
print("World State:", world_state)
```

---

## New Features (Enhanced API)

### 1. Enhanced Error Handling

The NLM Python bindings now provide comprehensive error handling with specific exception types:

```python
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # This will throw an NLMError if initialization fails
    for i in range(1000):
        brain.step(i)
        
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
except pynlm.ConfigError as e:
    print(f"Configuration Error: {e}")
except pynlm.BrainError as e:
    print(f"Brain Error: {e}")
except pynlm.WorldError as e:
    print(f"World Error: {e}")
```

### 2. Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.create_standard_config()  # Pre-configured with typical values

# Validate configuration
try:
    config.validate()  # NEW: validates for common issues
    print("Configuration is valid!")
except pynlm.ConfigError as e:
    print(f"Invalid configuration: {e}")

# Type-safe configuration setting
config.set_safe("brain.neuron_count", "2000")  # With validation
config.set_int("brain.neuron_count", 3000)      # Type checking
config.set_float("learning.rate", 0.001)        # Type checking
```

### 3. Convenience Methods

```python
import pynlm

# Create and run simulation in one call
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Run complete simulation
brain.run_for_steps(500)  # NEW: run brain for specific steps
world.run_episode(500)    # NEW: run world simulation
agent.run_episode(world, 500)  # NEW: run complete agent episode
agent.learn_from_episode(world, 500, reward=0.5)  # NEW: with learning

# Get comprehensive statistics
stats = brain.get_statistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.3f}")

# Agent status
status = agent.get_agent_status()
print(f"Curiosity level: {status['curiosity_level']:.3f}")
print(f"Development stage: {status['developmental_stage']}")

# World state
world_state = world.get_world_state()
print(f"Agent position: ({world_state['agent_body']['x']:.1f}, {world_state['agent_body']['y']:.1f})")
```

### 4. Performance Monitoring

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run performance benchmark
results = brain.benchmark_performance(1000)  # NEW: performance testing
print(f"Performance Results:")
print(f"  Steps completed: {results['steps_completed']}")
print(f"  Total time (ms): {results['total_time_ms']:.1f}")
print(f"  Steps per second: {results['steps_per_second']:.1f}")
print(f"  Average time per step (ms): {results['average_time_per_step_ms']:.3f}")
```

### 5. Enhanced Parameter Validation

```python
import pynlm

# This will throw validation errors if parameters are invalid
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create world with validation
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.1)

# Validate world configuration
try:
    world.validate_configuration()
    print("World configuration is valid!")
except pynlm.WorldError as e:
    print(f"Invalid world configuration: {e}")
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

## Common Patterns (Enhanced)

### Pattern 1: Run a Simulation (Enhanced)

```python
def run_simulation(brain, world, agent, num_steps):
    """Run a complete simulation with statistics."""
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
        
        # 7. Apply learning
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Comprehensive Statistics

```python
# Inside your loop or after simulation:
print("=== Brain Statistics ===")
stats = brain.get_statistics()
for key, value in stats.items():
    print(f"  {key}: {value}")

print("\n=== Agent Status ===")
status = agent.get_agent_status()
for key, value in status.items():
    print(f"  {key}: {value}")

print("\n=== World State ===")
state = world.get_world_state()
for key, value in state.items():
    if key == "agent_body":
        print(f"  agent_body:")
        for k, v in value.items():
            print(f"    {k}: {v}")
    else:
        print(f"  {key}: {value}")
```

### Pattern 3: Enable Brain Features (Enhanced)

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Save brain state with validation
brain.save("my_brain.pkl")

# Load brain state with error handling
try:
    brain.load("my_brain.pkl")
    print("Brain loaded successfully!")
except pynlm.BrainError as e:
    print(f"Failed to load brain: {e}")
```

---

## What Each Part Does

### Enhanced API Methods

#### `pynlm.create_standard_config()`
Creates a standard configuration with typical values for common use cases.

#### `pynlm.run_complete_simulation(brain, world, agent, num_steps)`
Run a complete simulation with integrated brain-world-agent interactions.

#### `brain.get_statistics()`
Get comprehensive brain statistics as a dictionary.

#### `agent.get_agent_status()`
Get comprehensive agent status as a dictionary.

#### `world.get_world_state()`
Get comprehensive world state as a dictionary.

#### `brain.benchmark_performance(num_steps)`
Run performance benchmark and return timing statistics.

#### `config.validate()`
Validate configuration for common issues.

#### `world.validate_configuration()`
Validate world configuration for consistency.

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

### Project 3: Complete Agent (Enhanced)

```python
import pynlm

# Setup with enhanced configuration
config = pynlm.create_standard_config()  # Better defaults
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
agent.enableDevelopment(True)

# Run agent with statistics
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    agent.applyRewardModulation(0.1, 0.0)  # Simple learning
    agent.updateDevelopment(0.1)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")

print("Enhanced agent simulation complete!")
```

---

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.get_statistics()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Validate config | `config.validate()` |
| Run benchmark | `brain.benchmark_performance(steps)` |
| Save/load brain | `brain.save(filepath)` / `brain.load(filepath)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check for validation errors in configuration

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Enable curiosity to encourage exploration

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration validation

**"Configuration validation failed"**
- Use `config.validate()` to see specific issues
- Common problems: invalid neuron counts, impossible parameter ranges

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Learn about advanced configuration options
   - Understand all the neural mechanisms

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - Deep dive into the neural architecture
   - Learn about memory systems and plasticity

3. **Experiment with different configurations!**
   - Try different neuron counts and connection densities
   - Experiment with neuromodulation parameters
   - Create your own simulation presets

4. **Performance optimization**
   - Use `brain.benchmark_performance()` to measure efficiency
   - Experiment with different brain configurations for optimal performance

5. **Save and load brain states**
   - Use `brain.save()` and `brain.load()` to preserve trained brains
   - Create brain checkpoints during long simulations

That's it! You're now ready to use the **enhanced NLM** with comprehensive error handling, validation, and convenience methods. Happy simulating!