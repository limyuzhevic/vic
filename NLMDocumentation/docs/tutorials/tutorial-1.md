# Tutorial 1: Silent Brain

**Learning Objectives**: Create and run a basic neural simulation without external input.

**Prerequisites**: None - this is the absolute beginning!

**Difficulty**: Beginner

**Estimated Time**: 15 minutes

### Overview

In this tutorial, you'll create the simplest possible NLM brain and run it. This brain won't receive any external input, so it will just run its internal dynamics and show you how neurons fire and interact.

### What You'll Learn

- How to create a NLM brain
- How to initialize and run a simulation
- Basic brain statistics and metrics
- How to interpret simulation results

### Step 1: Create Your First Brain

Let's start by creating the simplest possible brain:

```python
import pynlm

print("=== Tutorial 1: Silent Brain ===")

# Create a brain with default configuration
print("1. Creating brain with default configuration...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
print("2. Initializing brain...")
brain.initialize()

# Check brain properties
print(f"3. Brain created successfully!")
print(f"   Total neurons: {brain.getTotalNeuronCount()}")
print(f"   Total synapses: {brain.getTotalSynapseCount()}")
print(f"   Refractory period: {brain.getConfig().getFloat('brain.tau_ref')} ms")

# Run the simulation
print("4. Running simulation for 50 steps...")
for step in range(50):
    brain.step(step)
    
    # Show some statistics every 10 steps
    if step % 10 == 0:
        print(f"   Step {step}:")
        print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"     Active neurons: {brain.getActiveNeuronCount()}")
        print(f"     Total spikes: {brain.getTotalSpikeCount()}")
        print(f"     Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**Run this code now!** Save it as `tutorial_1_silent_brain.py` and execute it:

```bash
python tutorial_1_silent_brain.py
```

### Step 2: Explore Your Brain

Let's modify the tutorial to learn more about your brain:

```python
import pynlm

print("=== Extended Tutorial 1: Brain Exploration ===")

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain properties:")
print(f"  Neuron count: {brain.getTotalNeuronCount()}")
print(f"  Synapse count: {brain.getTotalSynapseCount()}")
print(f"  Firing threshold: {config.getFloat('brain.v_thresh')} mV")
print(f"  Resting potential: {config.getFloat('brain.v_rest')} mV")
print(f"  Reset potential: {config.getFloat('brain.v_reset')} mV")
print(f"  Membrane time constant: {config.getFloat('brain.tau_mem')} ms")
print(f"  Refractory period: {config.getFloat('brain.tau_ref')} ms")

# Run simulation and track changes
print(f"\nSimulation dynamics:")
for step in range(20):
    brain.step(step)
    
    if step % 5 == 0:
        firing_rate = brain.getAverageFiringRate()
        spike_count = brain.getTotalSpikeCount()
        print(f"  Step {step:2d}: {firing_rate:6.2f} Hz, {spike_count:4d} total spikes")
```

### Step 3: Try Different Brain Sizes

Now let's explore how brain size affects behavior:

```python
import pynlm

print("=== Tutorial 1: Brain Size Exploration ===")

# Test different brain sizes
sizes = [10, 100, 500]

for size in sizes:
    print(f"\n--- Brain with {size} neurons ---")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", size)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(30):
        brain.step(step)
    
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Firing efficiency: {brain.getTotalSpikeCount() / size:.2f} spikes/neuron")
    print(f"  Activity level: {brain.getFiringNeuronCount() / size * 100:.1f}% of neurons")
```

### Step 4: Save and Load Your Brain

Learn how to save and load brain states:

```python
import pynlm

print("=== Tutorial 1: Save and Load ===")

# Create and run a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps
for step in range(50):
    brain.step(step)

# Save the brain state
brain.save("tutorial_brain_state.bin")
print(f"Brain saved to 'tutorial_brain_state.bin'")

# Create a new brain and load the saved state
new_brain = pynlm.createBrain(config)
new_brain.initialize()
new_brain.load("tutorial_brain_state.bin")

print(f"Brain loaded successfully!")
print(f"Loaded neurons: {new_brain.getTotalNeuronCount()}")
print(f"Loaded spikes: {new_brain.getTotalSpikeCount()}")
```

### Step 5: Your First Exercise

**Exercise**: Modify the silent brain tutorial to explore:

1. **Different brain sizes**: Try brains with 5, 50, 500, and 5000 neurons
2. **Different thresholds**: Change the firing threshold (`v_thresh`)
3. **Different time constants**: Change the membrane time constant (`tau_mem`)
4. **Longer simulations**: Run simulations for 500 or 1000 steps

**What to observe**:
- How does brain size affect firing rate?
- What happens when you change the threshold?
- How do time constants affect dynamics?
- Can you detect patterns in the firing?

**Write down your observations** in a notebook or document them in comments in your code.

### Key Concepts

#### Neurons and Firing

- **Neurons** are the basic processing units in NLM
- They have **membrane potential** that changes over time
- When membrane potential reaches **threshold**, neurons **fire spikes**
- After firing, neurons enter a **refractory period** where they cannot fire again
- The **firing rate** measures how often neurons fire

#### Brain Statistics

- **Total neurons**: Number of neurons in the brain
- **Total synapses**: Number of connections between neurons
- **Firing neurons**: Neurons currently firing in the simulation
- **Active neurons**: Neurons currently processing (not just firing)
- **Total spikes**: Cumulative number of spikes fired
- **Firing rate**: Average spikes per second per neuron

#### Simulation Steps

- Each **step** represents a moment in time
- In each step, all neurons update their membrane potentials
- Spikes propagate through synapses
- Learning rules may update synaptic weights

### Common Issues and Solutions

**Problem 1: Brain doesn't seem to be doing anything**
- **Solution**: Run more steps (neurons need time to "warm up")
- **Solution**: Check that you're calling `brain.initialize()` before `brain.step()`
- **Solution**: Try different brain sizes or parameters

**Problem 2: Too many spikes too quickly**
- **Solution**: Lower the firing threshold (`v_thresh`)
- **Solution**: Increase the refractory period (`tau_ref`)
- **Solution**: Use a smaller brain

**Problem 3: Brain memory issues**
- **Solution**: Try a smaller brain for testing
- **Solution**: Monitor memory usage in longer simulations

**Problem 4: Inconsistent results**
- **Solution**: Set a random seed for reproducible results
- **Solution**: Save and load brain states for comparison

### What's Next?

You've completed Tutorial 1! Now you're ready for Tutorial 2:

## Tutorial 2: Brain in a World

In Tutorial 2, you'll create a more complete setup with a world and an agent. This will teach you about:
- Creating and configuring worlds
- Setting up agent interfaces
- Running complete agent-world simulations
- Understanding sensory and motor processing

Ready to continue? Move on to Tutorial 2 when you're ready!

---

*Tutorial Version*: 1.0
*Last Updated*: 2026-10-09
*Difficulty*: Beginner

**Need Help?** 
- Check the Troubleshooting Guide for common issues
- Look at the Python API Reference for function details
- Search for specific errors in the documentation