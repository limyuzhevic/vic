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

### Project 5: Advanced Configuration

```python
import pynlm
import json

def create_custom_brain_config():
    """Create a custom brain configuration with advanced parameters."""
    
    # Start with default config
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.set("brain.neuron_count", 5000)  # Larger network
    config.set("brain.synapse_density", 0.12)
    config.set("brain.connection_probability", 0.08)
    config.set("brain.initial_weight_mean", 0.6)
    config.set("brain.initial_weight_std", 0.15)
    config.set("brain.v_thresh", -45.0)  # Lower threshold for higher excitability
    config.set("brain.v_rest", -70.0)
    config.set("brain.v_reset", -75.0)
    config.set("brain.tau_mem", 15.0)  # Faster dynamics
    config.set("brain.tau_ref", 3.0)   # Shorter refractory period
    
    # Advanced plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)  # Higher learning rate
    config.set("plasticity.stdp.tau_plus", 25.0)      # Longer LTP window
    config.set("plasticity.stdp.tau_minus", 30.0)     # Longer LTD window
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.003)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.00015)
    config.set("plasticity.structural.pruning_rate", 0.000015)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.2)  # Stronger dopamine effect
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Developmental configuration
    config.set("development.initial_plasticity", 1.5)
    config.set("development.critical_period", 200.0)  # Longer critical period
    config.set("development.maturation_rate", 0.005)
    
    return config

def save_and_load_config():
    """Demonstrate saving and loading brain configurations."""
    
    # Create custom config
    config = create_custom_brain_config()
    
    # Save to file
    config.saveToFile("custom_config.json")
    print("Configuration saved to custom_config.json")
    
    # Load from file
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile("custom_config.json")
    print("Configuration loaded from custom_config.json")
    
    # Verify they are the same
    print("Verifying configuration consistency:")
    
    # Compare some key parameters
    original_neuron_count = config.getOr<int64_t>("brain.neuron_count", 1000)
    loaded_neuron_count = config2.getOr<int64_t>("brain.neuron_count", 1000)
    
    print(f"  Neuron count: {original_neuron_count} == {loaded_neuron_count}")
    print(f"  Match: {'✓' if original_neuron_count == loaded_neuron_count else '✗'}")
    
    return config2

def create_specialized_brain_types():
    """Create different brain types for specialized tasks."""
    
    brains = {}
    
    # 1. High-performance brain for quick learning
    high_perf_config = pynlm.createDefaultConfig()
    high_perf_config.set("brain.neuron_count", 2000)
    high_perf_config.set("brain.tau_mem", 10.0)
    high_perf_config.set("plasticity.stdp.learning_rate", 0.005)
    high_perf_config.set("neuromod.curiosity.enable", True)
    brains["high_performance"] = pynlm.createBrain(high_perf_config)
    
    # 2. Stable brain for consistent behavior
    stable_config = pynlm.createDefaultConfig()
    stable_config.set("brain.neuron_count", 1000)
    stable_config.set("brain.tau_mem", 30.0)
    stable_config.set("plasticity.stdp.learning_rate", 0.0005)
    stable_config.set("neuromod.curiosity.enable", False)
    stable_config.set("development.initial_plasticity", 0.8)
    brains["stable"] = pynlm.createBrain(stable_config)
    
    # 3. Creative brain with high curiosity
    creative_config = pynlm.createDefaultConfig()
    creative_config.set("brain.neuron_count", 3000)
    creative_config.set("plasticity.structural.enable", True)
    creative_config.set("neuromod.curiosity.enable", True)
    creative_config.set("neuromod.novelty.enable", True)
    creative_config.set("development.critical_period", 300.0)
    brains["creative"] = pynlm.createBrain(creative_config)
    
    # Initialize all brains
    for brain_name, brain in brains.items():
        brain.initialize()
        print(f"Created {brain_name} brain with {brain.getTotalNeuronCount()} neurons")
    
    return brains

# Run advanced configuration examples
print("=== Advanced Configuration Examples ===\n")

print("1. Creating custom brain configuration...")
custom_config = create_custom_brain_config()
print(f"   Custom config created with {custom_config.getOr<int64_t>('brain.neuron_count', 1000)} neurons\n")

print("2. Demonstrating save/load functionality...")
saved_config = save_and_load_config()
print()

print("3. Creating specialized brain types...")
specialized_brains = create_specialized_brain_types()
print()

print("=== Advanced configuration examples complete ===")
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

That's it! You're now ready to use NLM.
