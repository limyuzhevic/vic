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

## New Pythonic API Examples

### Example 3: Create a Powerful Demo Brain with One Line

```python
import pynlm

# Create a demo brain with all learning systems enabled
brain = pynlm.create_demo_brain()
brain.initialize()

print("Demo brain created with:", brain.getTotalNeuronCount(), "neurons")
print("Regions:", brain.region_count)
```

### Example 4: Small Brain for Testing

```python
import pynlm

# Create a small brain for quick testing
brain = pynlm.create_small_brain(200)
brain.initialize()

print("Small brain ready for testing!")
print("Stats:", brain.get_brain_summary())
```

### Example 5: Complete Training with Pythonic Interface

```python
import pynlm

# Setup with powerful brain
brain = pynlm.create_demo_brain()
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems (NEW Pythonic API)
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Run simulation with Pythonic interface
print("Starting simulation...")
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
print(brain.get_brain_summary())
```

### Example 6: Validation and Error Handling

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration
is_valid, message = config.validate()
print(f"Config valid: {is_valid}")
print(f"Message: {message}")

# Set safe values
config.set("neuron_count", 1000)
config.set("region_count", 3)
config.set("connection_probability", 0.15)

# Validate again
is_valid, message = config.validate()
print(f"Updated config valid: {is_valid}")
```

### Example 7: Batch Operations and Helper Methods

```python
import pynlm

# Create brain
brain = pynlm.create_demo_brain()
brain.initialize()

# Add regions using Pythonic methods
brain.add_sensory_region("SensoryRegion")
brain.add_motor_region("MotorRegion")
brain.add_internal_region("InternalRegion")

print(f"Brain now has {brain.get_region_count()} regions")

# Enable learning across all synapses
brain.enable_learning(True)

# Run simulation using batch training
print("Running batch training...")
spike_count = pynlm.run_brain_simulation(brain, 500)
print(f"Total spikes after 500 steps: {spike_count}")

# Get brain summary
print(brain.get_brain_summary())
```

### Example 8: Benchmarking and Performance Analysis

```python
import pynlm

# Create a larger brain for benchmarking
brain = pynlm.create_experimental_brain(2000, 5)
brain.initialize()

# Benchmark performance
spikes, firing_rate, duration_ms = pynlm.benchmark_brain(brain, 1000)
print(f"Benchmark results:")
print(f"  Spikes generated: {spikes}")
print(f"  Firing rate: {firing_rate:.2f} Hz")
print(f"  Duration: {duration_ms} ms")
```

---

## Pythonic Property Access

The new API includes Pythonic property access for cleaner code:

```python
import pynlm

brain = pynlm.create_demo_brain()
brain.initialize()

# Pythonic property access (NEW!)
print(f"Brain has {brain.region_count} regions")
print(f"Total neurons: {brain.total_neuron_count}")
print(f"Total synapses: {brain.total_synapse_count}")
print(f"Average firing rate: {brain.average_firing_rate:.2f} Hz")

# Use 'in' operator for key checking
config = pynlm.createDefaultConfig()
print(f"Has 'neuron_count': {'neuron_count' in config}")
print(f"Has 'invalid_key': {'invalid_key' in config}")
```

---

## Complete Mini Projects with New API

### Project 1: Silent Brain with Demo Brain

```python
import pynlm

# One line to create a powerful brain
brain = pynlm.create_demo_brain()
brain.initialize()

# Make it think
brain.step(0)

print("Powerful brain has", brain.getTotalNeuronCount(), "neurons!")
print("With Pythonic stats: region_count =", brain.region_count)
```

### Project 2: Brain Watching a World (Enhanced)

```python
import pynlm

# Setup with enhanced brain
brain = pynlm.create_demo_brain()
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enhanced setup with learning enabled
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Watch the world with Pythonic methods
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.average_firing_rate)
```

### Project 3: Complete Agent with All Learning

```python
import pynlm

# Setup with all learning systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems (NEW simplified API)
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Run complete agent simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Enhanced agent simulation complete!")
print("Summary:", brain.get_brain_summary())
```

---

## Common Patterns with New API

### Pattern 1: Advanced Simulation Runner

```python
def run_advanced_simulation(world, agent, brain, num_steps):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 20 steps
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Use it like this:
run_advanced_simulation(world, agent, brain, 1000)
```

### Pattern 2: Enhanced Brain Statistics

```python
def print_brain_stats(brain):
    # Use Pythonic property access
    print(f"  Region count: {brain.region_count}")
    print(f"  Total neurons: {brain.total_neuron_count}")
    print(f"  Total synapses: {brain.total_synapse_count}")
    print(f"  Average firing rate: {brain.average_firing_rate:.2f} Hz")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Brain summary:\n{brain.get_brain_summary()}")

# Call it
print_brain_stats(brain)
```

### Pattern 3: Feature Comparison

```python
def compare_two_brains(brain1, brain2, steps):
    # Use the powerful comparison function
    spikes1, spikes2, ratio, rate_ratio = pynlm.compare_brains(brain1, brain2, steps)
    
    print(f"Comparison after {steps} steps:")
    print(f"  Brain 1 spikes: {spikes1}")
    print(f"  Brain 2 spikes: {spikes2}")
    print(f"  Spike ratio (B1/B2): {ratio:.2f}")
    print(f"  Firing rate ratio (B1/B2): {rate_ratio:.2f}")

# Use it
compare_two_brains(brain1, brain2, 200)
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### Enhanced `pynlm.create_demo_brain()`
Creates a brain with ALL learning systems enabled (equivalent to setting up with STDP, reward modulation, structural plasticity, etc.)

### Enhanced `pynlm.create_small_brain(neuronCount)`
Creates a compact brain optimized for testing and quick iteration.

### Enhanced `pynlm.create_experimental_brain(neuronCount, regionCount)`
Creates a multi-region brain for experimental research.

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `brain.get_brain_summary()`
Gets a comprehensive text summary of brain state (NEW!)

### `brain.enable_learning(True)`
Enables all plasticity systems (Hebbian + STDP + Reward-modulated) across all synapses (NEW!)

### `pynlm.run_brain_simulation(brain, steps)`
Runs a complete simulation with automatic initialization (NEW!)

### `pynlm.benchmark_brain(brain, steps)`
Benchmarks brain performance and returns spike count, firing rate, and duration (NEW!)

### `pynlm.compare_brains(brain1, brain2, steps)`
Compares two brains and returns spike counts, firing rates, and ratios (NEW!)

### `pynlm.validate_brain_config(config)`
Validates configuration and returns (is_valid, error_message) (NEW!)

---

## Mini Projects with Enhanced Features

### Project 1: Learning Brain

```python
import pynlm

# Create a learning brain
brain = pynlm.create_demo_brain()
brain.initialize()

# Enable all learning systems
brain.enable_learning(True)

# Run with sensor input
for step in range(200):
    brain.step(step)

print("Learning brain completed!")
print("Stats:", brain.get_brain_summary())
```

### Project 2: Performance Testing

```python
import pynlm

# Create experimental brain
brain = pynlm.create_experimental_brain(5000, 10)
brain.initialize()

# Benchmark performance
spikes, firing_rate, duration = pynlm.benchmark_brain(brain, 1000)

print(f"Performance: {firing_rate:.2f} Hz in {duration}ms")
print(f"Generated {spikes} spikes")
```

### Project 3: Feature Comparison

```python
import pynlm

# Create two different brains
brain1 = pynlm.create_demo_brain()
brain2 = pynlm.create_small_brain(500)

brain1.initialize()
brain2.initialize()

# Compare them
spikes1, spikes2, ratio, rate_ratio = pynlm.compare_brains(brain1, brain2, 300)

print(f"Comparison results:")
print(f"  Brain1: {spikes1} spikes, rate ratio {rate_ratio:.2f}")
```

---

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create basic brain | `pynlm.createBrain(config)` |
| Create powerful demo brain | `pynlm.create_demo_brain()` |
| Create small brain for testing | `pynlm.create_small_brain(neuronCount)` |
| Create experimental brain | `pynlm.create_experimental_brain(neuronCount, regionCount)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Run complete simulation | `pynlm.run_brain_simulation(brain, steps)` |
| Get brain stats | `brain.get_brain_summary()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Enable all learning | `brain.enable_learning(True)` |
| Benchmark brain | `pynlm.benchmark_brain(brain, steps)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try enabling learning: `brain.enable_learning(True)`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"Configuration errors"**
- Use `pynlm.validate_brain_config(config)` to check your settings

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the enhanced features like `create_demo_brain()`, `run_brain_simulation()`, and benchmarking!

That's it! You're now ready to use the enhanced NLM with all its powerful Pythonic features.

**New Features Added:**
- Pythonic property access (`brain.region_count`, `brain.total_neuron_count`)
- Factory methods for common use cases (`create_demo_brain`, `create_small_brain`)
- Simulation helpers (`run_brain_simulation`, `benchmark_brain`)
- Configuration validation (`validate_brain_config`)
- Feature comparison (`compare_brains`)
- Enhanced learning control (`enable_learning`, `enable_feature`)
- Comprehensive brain summary (`get_brain_summary`) 

These features make NLM much more powerful and easier to use than the original version!