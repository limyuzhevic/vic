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

### NEW: Example 3: Working with Individual Neurons

```python
import pynlm

# Create a brain and initialize it
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get the first region
region = brain.getRegion(brain.getRegionIds()[0])

if region:
    # Get first neuron in region
    population = region.getAllPopulations()[0]
    neuron = population.getNeuron(0)
    
    print(f"Neuron membrane potential: {neuron.membrane_potential}")
    print(f"Neuron type: {neuron.getType()}")
    print(f"Neuron is firing: {neuron.isFiring()}")
    
    # Change neuron properties
    neuron.setThreshold(-45.0)  # Increase threshold
    neuron.setFiringRate(10.0)  # Set firing rate
    
    print(f"Updated threshold: {neuron.getThreshold()}")
    
    # Step with new properties
    brain.step(0)
    print(f"Spike count after step: {brain.getTotalSpikeCount()}")

print("Individual neuron example completed!")
```

### NEW: Example 4: Working with Synapses

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get first region and a synapse
region = brain.getRegion(brain.getRegionIds()[0])
synapse = region.addSynapse(0, 1, 0.5)  # Create synapse from neuron 0 to 1 with weight 0.5

if synapse:
    print(f"Synapse weight: {synapse.weight}")
    print(f"Synapse type: {synapse.getType()}")
    print(f"Is excitatory: {synapse.isExcitatory()}")
    
    # Modify synapse
    synapse.weight = 0.8  # Direct access
    synapse.setType(pynlm.SynapseType.Inhibitory)
    
    print(f"Updated weight: {synapse.weight}")
    print(f"Updated type: {synapse.getType()}")
    print(f"Is now inhibitory: {synapse.isInhibitory()}")

print("Synapse example completed!")
```

### NEW: Example 5: NumPy Integration for Data Analysis

```python
import pynlm

# Create brain and get neuron data
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get membrane potentials from all neurons
membranes = []
for region in brain.getRegions():
    for population in region.getAllPopulations():
        for neuron in population.getNeurons():
            membranes.append(neuron.membrane_potential)

print(f"Collected {len(membranes)} membrane potentials")

# Use NumPy for statistical analysis
mean_potential = sum(membranes) / len(membranes) if membranes else 0.0
print(f"Average membrane potential: {mean_potential:.2f} mV")

# Create a simple 2D matrix for connectivity visualization
connectivity_data = []
for i in range(10):
    row = [i * 0.1] * 10
    connectivity_data.append(row)

print(f"Created {len(connectivity_data)}x{len(connectivity_data[0])} connectivity matrix")

print("NumPy integration example completed!")
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

### NEW: Pattern 4: Advanced Statistics

```python
# Get comprehensive brain statistics
def get_advanced_stats(brain):
    stats = brain.get_stats()  # New convenience function
    
    # Add derived statistics
    firing_ratio = stats["firing_neurons"] / max(stats["total_neurons"], 1)
    stats["firing_ratio"] = firing_ratio
    
    # Get regional statistics
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        region_stats = {
            "neuron_count": region.getTotalNeuronCount(),
            "active_count": region.getActiveNeuronCount(),
            "firing_count": region.getFiringNeuronCount(),
            "average_rate": region.getAverageFiringRate()
        }
        stats[f"region_{region_id}"] = region_stats
    
    return stats

# Usage:
advanced_stats = get_advanced_stats(brain)
print(f"Total neurons: {advanced_stats['total_neurons']}")
print(f"Firing ratio: {advanced_stats['firing_ratio']:.2%}")
```

### NEW: Pattern 5: Validation and Error Handling

```python
# Validate simulation parameters before running
def validate_and_run(brain, world, agent, steps, dt):
    # Validate parameters
    if not pynlm.validate_simulation_parameters(
        brain.getTotalNeuronCount(), steps, dt):
        raise ValueError("Invalid simulation parameters")
    
    # Run with error handling
    try:
        for step in range(steps):
            world.update(dt)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
                
    except Exception as e:
        print(f"Simulation error at step {step}: {e}")
        raise

# Usage with improved error handling
validate_and_run(brain, world, agent, 1000, 0.1)
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

### NEW: Enhanced Functionality

#### Brain Properties (Pythonic API)
```python
# Direct property access (new Pythonic API)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access neuron properties directly
neuron = brain.getRegion(0).getAllPopulations()[0].getNeuron(0)
print(f"Membrane potential: {neuron.membrane_potential}")

# Modify neuron properties directly
neuron.threshold = -40.0  # Direct access
neuron.firing_state = pynlm.FiringState.Active  # Enum access
```

#### Convenience Functions
```python
# High-level convenience functions (new)
config = pynlm.create_default_config_with_validation()
brain = pynlm.create_brain_from_config(config)

# Run simulation with statistics
sim_stats = pynlm.run_simulation_steps(brain, 1000, 0.1)
print(f"Total spikes: {sim_stats}")

# Get comprehensive stats
stats = pynlm.get_brain_stats(brain)
print(f"Neurons: {stats['total_neurons']}, Firing: {stats['firing_neurons']}")
```

#### NumPy Integration
```python
# NumPy data interchange (new)
import numpy as np

# Convert vectors to NumPy arrays
membrane_array = pynlm.vector_to_numpy(membrane_vector)

# Create NumPy matrices for visualization
connectivity_matrix = pynlm.create_numpy_matrix(connectivity_data)

# Validate NumPy arrays
pynlm.check_numpy_array(membrane_array, expected_size=len(membrane_vector))
```

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

### NEW: Project 3: Advanced Neuron Manipulation

```python
import pynlm

# Create a more complex brain
config = pynlm.create_default_config_with_validation()
config.set("brain.neuron_count", 200)

brain = pynlm.createBrain(config)
brain.initialize()

# Create neuron populations
region = brain.getRegion(brain.getRegionIds()[0])

# Create multiple populations with different neuron types
population_exc = region.addPopulation(50, pynlm.NeuronType.Excitatory)
population_inh = region.addPopulation(30, pynlm.NeuronType.Inhibitory)

# Connect neurons (convenience function)
pynlm.connect_neurons(
    population_exc.getNeuron(0),
    population_inh.getNeuron(0),
    0.5,
    pynlm.SynapseType.Excitatory
)

# Modify neuron properties
for population in [population_exc, population_inh]:
    for i in range(min(5, population.getSize())):
        neuron = population.getNeuron(i)
        neuron.setFiringRate(5.0 + i * 0.5)  # Different firing rates

# Run simulation
for step in range(100):
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Advanced neuron manipulation completed!")
```

### NEW: Project 4: NumPy-Based Analysis

```python
import pynlm

# Setup brain simulation
brain = pynlm.createBrain(pynlm.create_default_config_with_validation())
brain.initialize()

# Run simulation to collect data
for step in range(100):
    brain.step(step)

# Collect neural activity data
all_membranes = []
all_rates = []

for region in brain.getRegions():
    for population in region.getAllPopulations():
        for neuron in population.getNeurons():
            all_membranes.append(neuron.membrane_potential)
            all_rates.append(neuron.getFiringRate())

# Use NumPy for analysis (after NumPy integration)
mean_membrane = sum(all_membranes) / len(all_membranes)
mean_rate = sum(all_rates) / len(all_rates)

print(f"Statistical Analysis:")
print(f"  Mean membrane potential: {mean_membrane:.2f} mV")
print(f"  Mean firing rate: {mean_rate:.2f} Hz")
print(f"  Data points collected: {len(all_membranes)}")

# Create 2D visualization data
heatmap_data = []
for i in range(10):
    row = [all_membranes[i * len(all_membranes) // 10 + j] for j in range(min(10, len(all_membranes) - i * len(all_membranes) // 10))]
    heatmap_data.append(row)

print(f"Created {len(heatmap_data)}x{len(heatmap_data[0])} heatmap data matrix")
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

## New Quick Reference

| What you want | New Code |
|--------------|-----------|
| Access neuron properties | `neuron.membrane_potential`, `neuron.threshold` |
| Create brain with validation | `pynlm.create_default_config_with_validation()` |
| Run simulation with stats | `pynlm.run_simulation_steps(brain, steps, dt)` |
| Convert to NumPy | `pynlm.vector_to_numpy(vector)` |
| Get comprehensive stats | `pynlm.get_brain_stats(brain)` |
| Validate parameters | `pynlm.validate_simulation_parameters()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if there are any active neurons

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"Neuron properties not working"**
- Did you enable access to the core neural classes?
- Make sure the brain was properly initialized

**"NumPy integration not available"**
- NumPy integration is now included in the core bindings
- Check that the latest version was compiled with NumPy support

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the new NumPy integration examples
5. Explore the advanced neuron manipulation examples

That's it! You're now ready to use NLM with the improved Python API!
