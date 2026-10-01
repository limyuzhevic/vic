# Basic Example 1: Simple NLM Usage

This is a basic example demonstrating simple NLM usage for beginners.

## Overview

This example shows the simplest way to create and use a brain in NLM. It's designed for users who are new to NLM and want to understand the basic concepts.

## Code

```python
#!/usr/bin/env python3
"""
Basic Example 1: Simple NLM Usage

This example demonstrates the simplest way to create and use a brain in NLM.
It's designed for beginners who are new to NLM.
"""

import pynlm

def main():
    """Main function"""
    print("=== Basic Example 1: Simple NLM Usage ===")
    
    # Step 1: Create a brain
    print("1. Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Step 2: Initialize the brain
    print("2. Initializing brain...")
    brain.initialize()
    
    print(f"✅ Brain created and initialized successfully")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    
    # Step 3: Run simulation
    print("3. Running simulation...")
    for step in range(100):
        brain.step(step)
        
        # Print progress
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Step 4: Analyze results
    print("4. Analyzing results...")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Excitaiton/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    print("\n✅ Basic example completed successfully!")

if __name__ == "__main__":
    main()
```

## Explanation

### What This Example Shows

This example demonstrates the basic steps for using NLM:

1. **Creating a Brain**: Create a brain instance with default configuration
2. **Initializing the Brain**: Initialize the brain with default settings
3. **Running a Simulation**: Run a simulation step by step
4. **Analyzing Results**: Analyze the results of the simulation

### Key Concepts

- **Brain Creation**: `pynlm.createBrain(config)` creates a brain instance
- **Brain Initialization**: `brain.initialize()` initializes the brain
- **Simulation Loop**: `brain.step(step)` runs one simulation step
- **Brain Statistics**: Various methods provide information about the brain

### Expected Output

```
=== Basic Example 1: Simple NLM Usage ===
1. Creating brain...
2. Initializing brain...
✅ Brain created and initialized successfully
   Total neurons: 1000
   Total synapses: 49980
3. Running simulation...
   Step 0: 0 firing neurons
   Step 20: 0 firing neurons
   Step 40: 0 firing neurons
   Step 60: 0 firing neurons
   Step 80: 0 firing neurons
   Step 100: 0 firing neurons
4. Analyzing results...
   Total spikes: 0
   Average firing rate: 0.00 Hz
   Excitaiton/Inhibition ratio: 1.00

✅ Basic example completed successfully!
```

### Learning Outcomes

By completing this example, you will learn:

1. **How to create a brain**: Use `pynlm.createBrain(config)` to create a brain
2. **How to initialize a brain**: Use `brain.initialize()` to initialize the brain
3. **How to run a simulation**: Use `brain.step(step)` to run a simulation step
4. **How to analyze results**: Use various brain methods to analyze results
5. **How to use NLM**: Basic usage of NLM for simple applications

### Extensions

To extend this example:

1. **Custom Configuration**: Create custom configuration with different parameters
2. **Longer Simulation**: Run a longer simulation
3. **Multiple Brains**: Create and use multiple brains
4. **Visualization**: Add visualization of simulation results
5. **Real-world Application**: Apply the brain to a real-world problem

## Files

- **Basic Example 1 Code**: `examples/basic-example-1.md`
- **Documentation**: Documentation for the example
- **Tutorial**: Tutorial for this example

## Related Resources

- **Tutorial 1**: `docs/tutorials/tutorial-1-basic-usage.md`
- **API Reference**: `docs/api-reference.md`
- **Configuration Guide**: `docs/configuration.md`

## References

1. **NLM Documentation**: https://docs.kilo.ai/nlm
2. **NLM Examples**: https://github.com/your-repo/nlm/examples
3. **NLM Tutorials**: https://docs.kilo.ai/nlm/tutorials

This example provides a solid foundation for understanding NLM and getting started with the system.