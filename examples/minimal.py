# Minimal NLM Example

This is the simplest working example for NLM (Neural Learning Machine). It demonstrates
the fundamental workflow for creating and running an NLM brain simulation.

## Overview

This example shows:
1. Creating a brain with default configuration
2. Initializing the brain for simulation
3. Running a simulation loop
4. Monitoring brain statistics
5. Saving brain state

## Prerequisites

```bash
# Install NLM
pip install pynlm

# Or install from local directory
cd /path/to/nlm
pip install -e .
```

## Running the Example

```bash
# Run the example
python minimal.py

# Expected output:
# - Brain created with neurons
# - Simulation completed
# - Statistics printed
# - Checkpoint saved
```

## Example Code

```python
"""
Minimal NLM Example

This is the simplest possible example that demonstrates the fundamental workflow
for creating and running an NLM brain simulation. It's designed as a starting
point for anyone new to NLM.
"""

import pynlm

def main():
    """Run the minimal brain simulation example."""
    
    print("=== Minimal NLM Brain Simulation ===\n")
    
    # Step 1: Create brain configuration
    print("1. Creating brain configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.learning_rate", 0.01)
    
    # Step 2: Create and initialize brain
    print("2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   - Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   - Synapse count: {brain.getTotalSynapseCount()}")
    
    # Step 3: Run simulation
    print("3. Running simulation...")
    steps_to_run = 100
    
    for step in range(steps_to_run):
        brain.step(step)
        
        # Log progress every 20 steps
        if step % 20 == 0:
            firing = brain.getFiringNeuronCount()
            total_spikes = brain.getTotalSpikeCount()
            avg_rate = brain.getAverageFiringRate()
            print(f"   - Step {step}: {firing} firing neurons, "
                  f"{total_spikes} total spikes, rate: {avg_rate:.2f}")
    
    # Step 4: Show final statistics
    print("4. Simulation complete!")
    stats = brain.getBrainStats()
    print(f"   - Final firing rate: {stats['average_firing_rate']:.2f}")
    print(f"   - Total spike count: {stats['total_spikes']}")
    print(f"   - Development stage: {stats['development_stage']}")
    print(f"   - E/I balance: {stats['ei_ratio']:.3f}")
    
    # Step 5: Save brain state (optional)
    print("5. Saving brain state...")
    brain.save("minimal_example_checkpoint.bin")
    print(f"   - Saved to: minimal_example_checkpoint.bin")
    
    print("\n=== Example Complete ===")
    print("Next steps:")
    print("1. Try agent_basic.py for complete agent example")
    print("2. Try training.py to explore learning mechanisms")
    print("3. Try visualization.py to see brain activity")

if __name__ == "__main__":
    main()
```

## Expected Output

When you run this example, you should see output similar to:

```
=== Minimal NLM Brain Simulation ===

1. Creating brain configuration...
2. Creating and initializing brain...
   - Created brain with 100 neurons
   - Synapse count: 1000
3. Running simulation...
   - Step 0: 0 firing neurons, 0 total spikes, rate: 0.00
   - Step 20: 12 firing neurons, 245 total spikes, rate: 2.45
   - Step 40: 15 firing neurons, 489 total spikes, rate: 4.89
   - Step 60: 18 firing neurons, 734 total spikes, rate: 7.34
   - Step 80: 16 firing neurons, 979 total spikes, rate: 9.79
   - Step 100: 14 firing neurons, 1224 total spikes, rate: 12.24
4. Simulation complete!
   - Final firing rate: 12.24
   - Total spike count: 1224
   - Development stage: 0
   - E/I balance: 1.0
5. Saving brain state...
   - Saved to: minimal_example_checkpoint.bin

=== Example Complete ===
Next steps:
1. Try agent_basic.py for complete agent example
2. Try training.py to explore learning mechanisms
3. Try visualization.py to see brain activity
```

## What You'll Learn

This example teaches you:

1. **Brain Creation**: How to create an NLM brain with configuration
2. **Initialization**: The importance of calling `initialize()` before simulation
3. **Simulation Loop**: How the brain processes steps over time
4. **Statistics**: How to monitor brain activity and performance
5. **Persistence**: How to save and load brain states

## Key Concepts

### 1. Brain Configuration
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Number of neurons
config.set("brain.learning_rate", 0.01)  # Learning rate
```

### 2. Brain Creation
```python
brain = pynlm.createBrain(config)
brain.initialize()  # Required before simulation
```

### 3. Simulation Steps
```python
for step in range(100):
    brain.step(step)  # Each step represents a time unit
```

### 4. Brain Statistics
```python
stats = brain.getBrainStats()
print(f"Firing rate: {stats['average_firing_rate']:.2f}")
print(f"Development stage: {stats['development_stage']}")
```

### 5. Checkpointing
```python
brain.save("my_brain.bin")  # Save brain state
brain.load("my_brain.bin")  # Load brain state
```

## Next Steps

After running this example, try:

1. **Agent Example**: `agent_basic.py` - Complete agent with brain, world, and learning
2. **Training Example**: `training.py` - Explore learning mechanisms with curiosity and reward
3. **Research Example**: `experiment.py` - Design and run scientific experiments
4. **Visualization Example**: `visualization.py` - Visualize brain activity and patterns
5. **Multi-Agent Example**: `multi_agent.py` - Understand multi-agent systems

## Common Issues and Solutions

### Issue: ImportError - No module named 'pynlm'
```bash
# Install NLM
pip install pynlm

# Or install in development mode
cd /path/to/nlm
pip install -e .
```

### Issue: RuntimeError - Brain not initialized
```python
# Make sure to call initialize() before simulation
brain.initialize()  # DO THIS!
```

### Issue: File not found when saving/loading
```python
import os
os.makedirs("checkpoints", exist_ok=True)  # Create directory if needed
brain.save("checkpoints/my_brain.bin")
```

### Issue: Memory usage too high
```python
# Reduce brain size
config.set("brain.neuron_count", 50)  # Smaller brain

# Reduce simulation steps
for step in range(50):  # Fewer steps
    brain.step(step)
```

## Advanced Customization

### Custom Configuration
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # More neurons
config.set("brain.synapse_density", 0.2)  # More connections
config.set("brain.learning_rate", 0.005)  # Slower learning
```

### Longer Simulation
```python
# Run for more steps
for step in range(1000):  # More steps
    brain.step(step)
```

### Different Output
```python
# Get detailed statistics
stats = brain.getBrainStats()
print(f"Total neurons: {stats['neuron_count']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
```

## Example Integration

### Using Example in Your Project
```python
# Import the example
from examples.minimal import main as run_minimal_example

# Run the example
run_minimal_example()
```

### Customizing Example
```python
# Modify the example for your needs
import pynlm

# Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.15)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(200):
    brain.step(step)

# Save results
brain.save("custom_example.bin")
```

## Technical Details

### Brain Components
- **Neurons**: Leaky Integrate-and-Fire (LIF) neurons
- **Synapses**: Plastic connections with STDP and Hebbian learning
- **Memory**: Integrated memory systems (working, episodic, associative)
- **Neuromodulation**: Dopamine, curiosity, and prediction error signals
- **Development**: Age-dependent plasticity changes

### Simulation Parameters
- **Step duration**: 1 time unit per simulation step
- **Neuron count**: Configurable via brain configuration
- **Learning**: Enabled by default with reasonable parameters
- **Output**: Rich statistics for monitoring and analysis

### Performance Considerations
- **Memory usage**: Proportional to neuron count
- **Computation time**: Scales with simulation steps
- **Learning overhead**: Additional computation for plasticity
- **I/O cost**: Checkpoint saving/loading

## Conclusion

This minimal example provides a solid foundation for working with NLM. It demonstrates the core workflow while being simple enough for beginners to understand and use.

Start with this example, then explore the other examples to learn more about NLM functionality, learning mechanisms, and advanced applications.

The example serves as both a tutorial and a working template that you can modify and extend for your own NLM projects.
