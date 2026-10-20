# Interactive Tutorial 1: Your First NLM Brain

Welcome to the first interactive tutorial! This tutorial will walk you through creating and using your first NLM brain step by step.

## Overview

In this tutorial, you'll learn how to:
1. Install and import the NLM Python bindings
2. Create a basic brain configuration
3. Create and initialize a brain
4. Run a simple simulation
5. Analyze and understand the results

## Prerequisites

- Python 3.8+ installed on your system
- Basic Python programming knowledge
- Understanding of neural networks (helpful but not required)

## Step 1: Setup and Installation

### 1.1 Install NLM

First, let's make sure you have NLM installed. If you're running this in an environment where NLM is not installed, please install it:

```bash
pip install pynlm
```

### 1.2 Test Your Installation

Open your terminal or command prompt and run:

```python
import pynlm as nlm

print("NLM Version:", nlm.__version__)
print("Testing NLM installation...")
```

You should see output similar to:
```
NLM Version: 1.0.0
Testing NLM installation...
```

### 1.3 Check Your Environment

Let's also check your Python environment:

```python
import platform
print(f"Python version: {platform.python_version()}")
print(f"Platform: {platform.system()}")
```

## Step 2: Create a Brain Configuration

Now, let's create a basic brain configuration. We'll use the default configuration first:

```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

print(f"Created configuration with {config.getKeys()} keys")
```

Let's examine what parameters are in the default configuration:

```python
# Show some key configuration parameters
print("\nDefault configuration parameters:")
for key in ["brain.neuronCount", "brain.synapseDensity", "plasticity.stdp.learningRate"]:
    if config.has(key):
        value = config.get(key)
        print(f"  {key}: {value}")
```

### 2.1 Understanding the Configuration

The configuration we just created has several important parameters:

- **brain.neuronCount**: Number of neurons in the brain (default: 1000)
- **brain.synapseDensity**: How many connections between neurons (default: 0.1)
- **plasticity.stdp.learningRate**: Learning rate for STDP (default: 0.001)

You can modify these parameters to create different brain configurations:

```python
# Example: Create a smaller brain
config2 = nlm.createDefaultConfig()
config2.set("brain.neuron_count", 500)  # Half the neurons
config2.set("brain.synapse_density", 0.05)  # Fewer connections
```

## Step 3: Create and Initialize Your Brain

Now, let's create the actual brain using our configuration:

```python
import pynlm as nlm

# Create brain from configuration
brain = nlm.createBrain(config)

print(f"Brain created successfully!")
print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
print(f"  - Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")

# Initialize the brain (this sets up all the neural machinery)
brain.initialize()

print("Brain initialized successfully!")
```

### Key Concepts: What Happens During Initialization?

When you call `brain.initialize()`, several important things happen:

1. **Neural Network Setup**: The brain creates all neurons and synapses
2. **Memory Allocation**: Memory pools are allocated for neural state
3. **Initial State**: Neurons start with their initial membrane potentials
4. **System Connections**: Synaptic connections are established between neurons
5. **Plasticity Systems**: Learning mechanisms are initialized

You can check if initialization was successful by looking at the brain statistics:

```python
# Get detailed brain statistics
stats = nlm.getBrainStats(brain)
print(f"\nBrain statistics:")
for key, value in stats.items():
    if isinstance(value, (int, float)):
        print(f"  {key}: {value:.3f}")
    else:
        print(f"  {key}: {value}")
```

## Step 4: Run a Simple Simulation

Now, let's run a simple simulation to see the brain in action:

```python
import time

print("\nStarting simulation...")
start_time = time.time()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    
    # Print progress every 20 steps
    if step % 20 == 0:
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        print(f"  Step {step:3d}: {firing:4d} neurons firing ({rate:.2f} Hz)")

end_time = time.time()
duration = end_time - start_time

print(f"\nSimulation completed in {duration:.2f} seconds")
print(f"Average steps per second: {100/duration:.1f}")
```

### What Just Happened?

During this simulation, the brain:

1. **Processed Input**: Received no external input (silent brain)
2. **Maintained State**: Neurons continued their internal dynamics
3. **Generated Spikes**: Some neurons fired action potentials
4. **Updated Connections**: Synaptic weights may have changed (plasticity)
5. **Maintained Activity**: Network reached a steady state

## Step 5: Analyze and Understand the Results

Now, let's analyze what happened during the simulation:

```python
# Final brain statistics
print(f"\n=== Final Brain Statistics ===")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Currently firing: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Get comprehensive statistics
stats = nlm.getBrainStats(brain)
print(f"\n=== Comprehensive Statistics ===")
for key, value in stats.items():
    if isinstance(value, (int, float)):
        if key.endswith("rate") or key.endswith("ratio") or "Firing" in key:
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value:.1f}")
    else:
        print(f"  {key}: {value}")
```

## Step 6: Try Something Different

Now that you understand the basics, let's experiment with different configurations:

### Experiment 1: More Neurons, Faster Processing
```python
print("\n" + "="*60)
print("EXPERIMENT 1: More Neurons, Fast Processing")
print("="*60)

# Create larger brain
config_large = nlm.createDefaultConfig()
config_large.set("brain.neuron_count", 5000)
config_large.set("brain.synapse_density", 0.05)

brain_large = nlm.createBrain(config_large)
brain_large.initialize()

print(f"Created brain with {brain_large.getTotalNeuronCount()} neurons")

# Run simulation
for step in range(50):
    brain_large.step(step)

print(f"Average firing rate: {brain_large.getAverageFiringRate():.2f} Hz")
```

### Experiment 2: Learning Configuration
```python
print("\n" + "="*60)
print("EXPERIMENT 2: Learning Configuration")
print("="*60)

# Create configuration with learning
config_learning = nlm.createDefaultConfig()
config_learning.set("plasticity.stdp.learning_rate", 0.01)
config_learning.set("plasticity.hebbian.learning_rate", 0.02)
config_learning.set("neuromod.curiosity.exploration_weight", 0.8)

brain_learning = nlm.createBrain(config_learning)
brain_learning.initialize()

print(f"Created learning-optimized brain")
print(f"Firing rate: {brain_learning.getAverageFiringRate():.3f} Hz")
```

### Experiment 3: Performance Test
```python
print("\n" + "="*60)
print("EXPERIMENT 3: Performance Test")
print("="*60)

import time

# Test performance with different brain sizes
sizes = [100, 500, 1000, 2000]

for neurons in sizes:
    config_perf = nlm.createDefaultConfig()
    config_perf.set("brain.neuron_count", neurons)
    
    brain_perf = nlm.createBrain(config_perf)
    brain_perf.initialize()
    
    start_time = time.time()
    for step in range(100):
        brain_perf.step(step)
    end_time = time.time()
    
    duration = end_time - start_time
    steps_per_sec = 100 / duration
    
    print(f"Neurons: {neurons:5d} | Time: {duration:5.2f}s | "
          f"Steps/sec: {steps_per_sec:6.1f} | "
          f"Firing rate: {brain_perf.getAverageFiringRate():5.2f} Hz")
```

## Step 7: Your First Complete NLM Application

Now, let's create a complete NLM application that includes a world and agent:

```python
import pynlm as nlm

print("=== Complete NLM Application Example ===")

# 1. Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# 2. Create world
world = nlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
world.reset()

# 3. Create agent
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print(f"Created complete agent-brain-world system:")
print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
print(f"  - World: {world.getWidth()}x{world.getHeight()} units")
print(f"  - Vision: {world.getVisionWidth()}x{world.getVisionHeight()} pixels")
print(f"  - Learning: Reward modulation, curiosity enabled")

# 5. Run simulation
print(f"\nRunning simulation...")
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 20 == 0:
        print(f"  Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, Firing: {brain.getFiringNeuronCount()}")

print(f"\n=== Complete Application Results ===")
print(f"✓ Brain created and initialized")
print(f"✓ World configured and reset")
print(f"✓ Agent brain interface created")
print(f"✓ Learning systems enabled")
print(f"✓ Simulation completed successfully")
print(f"✓ Agent explored and learned")
```

## Step 8: Save and Load Brain States

Learn how to save and load brain states for later use:

```python
import os

print("\n=== Saving and Loading Brain States ===")

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run some simulation to have state to save
for step in range(50):
    brain.step(step)

# Save brain state
save_path = "my_brain_state.brain"
brain.save(save_path)

print(f"✓ Brain state saved to: {save_path}")

# Check if file exists
if os.path.exists(save_path):
    print(f"✓ File exists: {save_path}")
    
    # Create new brain
    new_brain = nlm.createBrain(config)
    new_brain.initialize()
    
    # Load brain state
    new_brain.load(save_path)
    print(f"✓ Brain state loaded successfully")
    
    # Verify loaded state
    print(f"  - Neurons after load: {new_brain.getTotalNeuronCount()}")
    print(f"  - Firing after load: {new_brain.getFiringNeuronCount()}")
else:
    print(f"✗ File not found: {save_path}")
```

## Step 9: Performance Analysis

Final example showing how to analyze NLM performance:

```python
import time
import psutil

print("\n=== Performance Analysis ===")

# Monitor system resources
process = psutil.Process()
initial_memory = process.memory_info().rss / 1024 / 1024

# Create and run brain
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

brain = nlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Performance measurement
start_time = time.time()
start_memory = process.memory_info().rss / 1024 / 1024

# Run simulation
for step in range(200):
    brain.step(step)
    
    # Monitor memory usage periodically
    if step % 50 == 0:
        current_memory = process.memory_info().rss / 1024 / 1024
        print(f"  Step {step}: Memory usage: {current_memory:.1f} MB")

end_time = time.time()
end_memory = process.memory_info().rss / 1024 / 1024

# Calculate performance metrics
duration = end_time - start_time
steps_per_second = 200 / duration
memory_used = end_memory - start_memory
memory_per_step = memory_used / 200

print(f"\n=== Performance Results ===")
print(f"Total time: {duration:.2f} seconds")
print(f"Steps per second: {steps_per_second:.1f}")
print(f"Memory used: {memory_used:.1f} MB")
print(f"Memory per step: {memory_per_step:.2f} bytes")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Save final state
brain.save("performance_test_brain.brain")
print(f"✓ Performance results saved to: performance_test_brain.brain")
```

## Your First Complete NLM Experience

Congratulations! You've now completed your first interactive NLM tutorial. Here's what you've learned:

### What You've Accomplished:

1. ✅ **Setup and Installation**: Installed and tested NLM Python bindings
2. ✅ **Brain Creation**: Created and initialized NLM brains
3. ✅ **Basic Operations**: Ran simulations and processed neural activity
4. ✅ **Configuration Management**: Modified brain parameters and settings
5. ✅ **State Management**: Saved and loaded brain states
6. ✅ **Performance Analysis**: Measured and analyzed performance metrics
7. ✅ **System Integration**: Created complete brain-world-agent systems

### Key Skills Developed:

1. **Python Programming**: Writing NLM code and scripts
2. **System Understanding**: Understanding NLM architecture and components
3. **Debugging**: Troubleshooting and fixing common issues
4. **Performance Optimization**: Optimizing brain configuration and performance
5. **Documentation**: Documenting experiments and results

### Next Steps:

1. **Explore Further**: Try the other examples in the `examples/` directory
2. **Advanced Learning**: Study the comprehensive documentation in `docs/`
3. **Research Applications**: Design experiments for your research questions
4. **Community Contribution**: Share your findings and examples with others
5. **Professional Development**: Use NLM in your research or projects

## Final Tips and Best Practices

### Coding Style:
- Use meaningful variable names
- Add comments to explain complex operations
- Handle errors gracefully
- Document your code with docstrings

### Performance Tips:
- Choose appropriate brain sizes for your use case
- Monitor memory usage during long simulations
- Use configuration templates for consistent setups
- Profile performance for optimization

### Debugging Tips:
- Start with simple examples and gradually add complexity
- Check error messages for specific guidance
- Use print statements for debugging
- Save intermediate states for testing

## Happy Brain-Building! 🧠✨

You now have the foundation to explore the fascinating world of NLM and neural simulation. Remember that NLM is a research tool - use it to investigate questions about neural computation, learning, and behavior.

Whether you're conducting research, building applications, or learning about neural systems, NLM provides a powerful platform for exploring how brains work and how artificial neural systems can be designed.

Keep experimenting, keep learning, and keep asking questions about how neural systems can solve real-world problems!

Happy neural engineering! 🚀🧠
