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
- `config` = settings for the brain (use `create_default_config()` for simple setup)

### `pynlm.create_default_config()`
Creates a default configuration with sensible settings for most use cases.

### `pynlm.create_optimized_config(neuron_count, connection_probability)`
Creates an optimized configuration for performance testing with higher neuron counts and optimized parameters.

### `pynlm.create_simple_config()`
Creates a minimal configuration with fewer neurons for development and testing.

### `pynlm.create_config_with_settings(...)`
Creates a custom configuration with all settings that can be configured.

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

### `pynlm.BrainSimulation(brain, steps)`
Context manager for brain simulations. Automatically handles initialization and cleanup.

### `pynlm.AgentSimulation(brain, world, agent, steps)`
Context manager for agent simulations. Automatically handles initialization and cleanup.

### `pynlm.run_simple_simulation(brain, steps, **kwargs)`
Convenience function for running simple brain simulations.

### `pynlm.run_agent_simulation(brain, world, agent, steps, **kwargs)`
Convenience function for running complete agent simulations with automatic resource management.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.create_simple_config())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.create_default_config())
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

### Project 3: Complete Agent with Enhanced Features

```python
import pynlm

# Setup with enhanced configuration
config = pynlm.create_optimized_config(neuron_count=2000)
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Use context manager for production code
with pynlm.AgentSimulation(brain, world, agent, steps=100) as simulation:
    simulation.start()
    results = simulation.run()
    
    # Print results
    print("Agent simulation complete!")
    print(f"Total reward: {results.get('total_reward', 0):.2f}")
    print(f"Final curiosity: {results.get('final_curiosity', 0):.3f}")
    print(f"Final novelty: {results.get('final_novelty', 0):.3f}")
```

### Project 4: Advanced Configuration Management

```python
import pynlm

# Create a custom configuration
custom_config = pynlm.create_config_with_settings(
    neuron_count=1500,
    region_count=2,
    connection_probability=0.18,
    initial_weight_mean=0.7,
    initial_weight_std=0.08,
    v_threshold=-50.0,
    random_seed=42
)

# Modify configuration
brain = pynlm.createBrain(custom_config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 25 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Custom configuration experiment completed!")
```

---

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
| Create simple config | `pynlm.create_simple_config()` |
| Create optimized config | `pynlm.create_optimized_config(neuron_count=5000)` |
| Create custom config | `pynlm.create_config_with_settings(**kwargs)` |
| Run simulation | `pynlm.run_agent_simulation(brain, world, agent, steps)` |
| Brain simulation | `with pynlm.BrainSimulation(brain, steps): ...` |
| Agent simulation | `with pynlm.AgentSimulation(brain, world, agent, steps): ...` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if you're using the correct configuration

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Make sure the agent has a motor command to execute

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if you're using a suitable configuration

**"Context manager errors"**
- Ensure all context managers are used properly (enter/exit pairs)
- Check that the brain/world/agent objects are properly initialized
- Make sure you're using the correct context manager for your use case

**"Configuration errors"**
- Check that configuration parameters are valid
- Ensure required parameters are set
- Use the factory methods (`create_simple_config()`, `create_optimized_config()`) for common use cases

**"Simulation performance issues"**
- For larger simulations, use `create_optimized_config()`
- Monitor system resources during long simulations
- Consider using fewer steps for testing

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Explore the Phase 6 integration experiments for advanced usage
5. Check out the performance optimization features for large-scale simulations
6. Learn about the new context managers for production code
7. Try the advanced configuration factory methods
8. Explore the convenience functions for complex simulations

That's it! You're now ready to use NLM with the enhanced Python API!

---

## Version Information

This documentation covers the enhanced NLM Python API with:
- Factory methods for configuration management
- Context managers for resource management
- Convenience functions for common operations
- Enhanced error handling
- Better documentation and examples

For more detailed documentation, see `HOW_TO_USE.md` and the project documentation in `docs/`.
