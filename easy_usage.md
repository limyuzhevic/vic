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
print("Development stage:", brain.get_developmental_stage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enable_reward_modulation(True)      # Learn from rewards
agent.enable_structural_plasticity(True)  # Grow new connections
agent.enable_development(True)           # Brain matures over time
agent.enable_curiosity(True)             # Explore new things
```

## New Pythonic Features

### Pattern 4: Use Pythonic Properties

```python
# Get brain statistics using properties
stats = brain.get_stats()
print(f"Neurons: {stats['neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']}")

# Use world dictionary for quick access
world_dict = world.get_world_dict()
print(f"World size: {world_dict['width']}x{world_dict['height']}")
print(f"Max energy: {world_dict['max_energy']}")

# Use agent dictionary for comprehensive stats
agent_stats = agent.get_agent_stats()
print(f"Developmental stage: {agent_stats['developmental_stage']}")
print(f"Neuromodulation level: {agent_stats['neuromodulation_level']}")
```

### Pattern 5: Context Managers

```python
# Use context manager for safe config operations
with agent.enable_all_features():
    # All features enabled in this block
    for step in range(100):
        # Your simulation code here
        brain.step(step)
        action = agent.decode_motor_command()

# Context manager for simulation state
with brain.simulation_state_context():
    brain.step(0)
    brain.step(1)
    # Brain state is preserved
```

### Pattern 6: Advanced Configuration

```python
# Create config from dictionary
config = pynlm.create_config_from_dict({
    "max_neurons": 1000,
    "max_synapses": 5000,
    "learning_rate": 0.01,
    "plasticity_flags": {"hebbian": True, "stdp": True, "reward_modulated": True}
})

# Validate config against required keys
required_keys = ["max_neurons", "learning_rate"]
valid, errors = pynlm.validate_config(config, required_keys)
if valid:
    brain = pynlm.create_brain(config)
else:
    print(f"Config validation errors: {errors}")
```

### Pattern 7: Batch Operations

```python
# Batch set multiple configuration values
config = pynlm.create_default_config()
pynlm.batch_set_config(config, {
    "learning_rate": 0.001,
    "plasticity_strength": 0.8,
    "curiosity_threshold": 0.5
})

# Batch process neurons
neuron_ids = [brain.get_all_neuron_ids()[:10]]  # Get first 10 neuron IDs
results = pynlm.batch_process_neurons(brain, neuron_ids, lambda id: brain.get_neuron_activity(id))
print(f"Processed {len(results)} neurons")
```

## New Advanced Features

### Feature 1: Enhanced Brain Management

```python
# Create brain with advanced configuration
brain_config = {
    "max_neurons": 2000,
    "max_synapses": 10000,
    "learning_rate": 0.001,
    "plasticity_flags": {"hebbian": True, "stdp": True, "structural": True}
}
brain = pynlm.create_brain_from_dict(brain_config)
brain.initialize()

# Network topology manipulation
pynlm.add_inter_region_connection(brain, pynlm.INVALID_REGION_ID, pynlm.INVALID_REGION_ID, weight=0.5)

# Set custom plasticity rules
flags = pynlm.PlasticityFlags()
flags.hebbian = True
flags.stdp = True
flags.reward_modulated = True
pynlm.set_plasticity_rule(brain, flags)
```

### Feature 2: Performance Monitoring

```python
# Get comprehensive brain statistics
stats = brain.get_performance_stats()
print(f"Performance: {stats}")

# Network topology analysis
analysis = pynlm.analyze_network_topology(brain)
print(f"Regions: {analysis['total_regions']}")
print(f"Total neurons: {analysis['total_neurons']}")
print(f"Total synapses: {analysis['total_synapses']}")
```

### Feature 3: Visualization and Analysis

```python
# Create visualization interface
viz = pynlm.create_visualization(brain, "brain_network.png")

# Get network analysis
network_analysis = pynlm.analyze_network_topology(brain)
print(f"Network health: {network_analysis['total_regions']} regions, {network_analysis['total_neurons']} neurons")

# Performance monitoring
performance = pynlm.get_brain_performance_stats(brain)
print(f"Performance metrics: {performance}")
```

### Feature 4: Error Recovery and Validation

```python
# Validate brain state
if pynlm.validate_brain_state(brain):
    print("Brain state is valid")
else:
    print("Brain state validation failed")

# Auto-recovery
if pynlm.auto_recover_brain(brain):
    print("Brain recovered successfully")
```

### Feature 5: Agent Enhancement

```python
# Create agent with all features enabled
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable all features at once
agent.enable_all_features()

# Get agent statistics
agent_stats = agent.get_agent_stats()
print(f"Agent development stage: {agent_stats['developmental_stage']}")
print(f"Neuromodulation level: {agent_stats['neuromodulation_level']}")
print(f"Curiosity level: {agent_stats['curiosity_level']}")
```

### Feature 6: Custom Learning Algorithms

```python
# Set custom neuromodulation level
pynlm.set_neuromodulation_level(brain, 0.7)

# Set custom developmental stage
pynlm.set_development_stage(brain, pynlm.DevelopmentalStage.Maturation)

# Custom plasticity rules
flags = pynlm.PlasticityFlags()
flags.hebbian = True
flags.stdp = True
flags.structural = True
pynlm.set_plasticity_rule(brain, flags)
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

That's it! You're now ready to use NLM.
