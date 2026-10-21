# NLM - Easy Usage Guide for Beginners

**Version:** 0.1.0 | **Author:** NLM Research Team | **License:** MIT

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

**NLM Architecture:** All systems are now integrated into a complete artificial brain!
- 🧠 **Brain** - The virtual brain that thinks (spiking neural network)
- 🌍 **World** - The environment the brain lives in (simulation space)
- 🤖 **Agent** - The bridge connecting brain to world (sensory-motor interface)

**New in Version 0.1.0:** Enhanced Python bindings with better error handling, convenience functions, and comprehensive integration!

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

# Enable learning features (NEW!)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

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
print("Total spikes:", brain.getTotalSpikeCount())
```

### Example 3: Using Convenience Functions (NEW!)

```python
import pynlm

# Quick brain creation with convenience function
brain = pynlm.quick_brain(200, "configs/default.cfg")
brain.initialize()

# Run complete simulation with statistics
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with learning and statistics
stats, final_stats = pynlm.run_agent_simulation(agent, world, 200, enable_learning=True)

print("Simulation completed!")
print("Final spike count:", final_stats["final_spike_count"])
print("Average firing rate:", final_stats["average_firing_rate"])
```

### Example 4: Predefined Presets (NEW!)

```python
import pynlm

# Create different types of brains with presets
simple_brain = pynlm.create_brain_with_preset("simple", 100)
complex_brain = pynlm.create_brain_with_preset("complex", 500)
optimized_brain = pynlm.create_brain_with_preset("optimized", 200)

# Initialize and use
simple_brain.initialize()
for i in range(50):
    simple_brain.step(i)

print(f"Simple brain: {simple_brain.getTotalNeuronCount()} neurons")
```

### Example 5: Error Handling (NEW!)

```python
import pynlm

# Safe configuration with error handling
try:
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    
    # Validate configuration
    if pynlm.validate_config(config):
        print("Configuration is valid!")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Save configuration safely
    pynlm.save_config_to_file(config, "my_config.json")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Example 6: Quick Experiments (NEW!)

```python
import pynlm

# Run quick predefined experiments
results = pynlm.quick_experiment("memory", 100)
print(f"Memory experiment: {results['total_spikes']} total spikes")

results = pynlm.quick_experiment("learning", 100)
print(f"Learning experiment: {results['total_spikes']} total spikes")

results = pynlm.quick_experiment("plasticity", 100)
print(f"Plasticity experiment: {results['total_spikes']} total spikes")
```

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

### Pattern 3: Enable Brain Features (NEW!)

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

### New Functions Available:

#### `pynlm.quick_brain(neuron_count, config_file="")`
Quick brain creation with optional config file.

#### `pynlm.run_agent_simulation(agent, world, num_steps, enable_learning=False)`
Run complete agent simulation with statistics.

#### `pynlm.create_brain_with_preset(preset_name, neuron_count)`
Create brains with predefined configurations ("simple", "complex", "optimized").

#### `pynlm.create_agent_with_preset(preset_name, brain)`
Create agent brains with behavior presets ("explorer", "learner", "methodical").

#### `pynlm.save_config_to_file(config, filepath)`
Save configuration with error handling.

#### `pynlm.load_config_from_file(filepath)`
Load configuration with error handling.

#### `pynlm.validate_config(config)`
Validate configuration and throw ConfigurationError on issues.

#### `pynlm.quick_experiment(experiment_type, steps)`
Run quick predefined experiments ("memory", "learning", "plasticity").

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

### Project 3: Complete Agent (NEW!)

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

**NEW:**
| What you want | Code |
|--------------|------|
| Quick brain | `pynlm.quick_brain(neuron_count, filepath)` |
| Agent simulation | `pynlm.run_agent_simulation(agent, world, steps)` |
| Brain presets | `pynlm.create_brain_with_preset(preset, count)` |
| Agent presets | `pynlm.create_agent_with_preset(preset, brain)` |
| Save config | `pynlm.save_config_to_file(config, "file.json")` |
| Load config | `pynlm.load_config_from_file("file.json")` |
| Validate config | `pynlm.validate_config(config)` |
| Quick experiment | `pynlm.quick_experiment(type, steps)` |

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

**"Configuration errors"**
- Use `pynlm.validate_config()` to check for issues
- Check that required keys exist and have valid values

**"Unknown experiment type"**
- Available types: "memory", "learning", "plasticity"

---

## Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Use the new convenience functions to simplify your code

**Pro Tips:**
- Use presets for different brain types
- Run quick experiments to test functionality
- Use error handling to make your code robust
- Leverage statistics collection in agent simulations

That's it! You're now ready to use the enhanced NLM with all its new features and improved error handling.

**Version 0.1.0 Features:** Enhanced Python bindings, convenience functions, better error handling, integrated systems, comprehensive documentation.

The easy usage guide has been updated to reflect all the improvements made to the NLM project!