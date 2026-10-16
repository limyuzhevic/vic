"""
Basic Brain Simulation Example
=============================

This example demonstrates the fundamental usage of NLM to create and run
 a simple brain simulation. It's designed for beginners who want to understand
the basic concepts of neural simulation.

## What This Example Shows

1. Creating a brain configuration
2. Initializing and running a brain simulation
3. Monitoring brain activity and statistics
4. Basic simulation loop structure

## When to Use This Example

- You're new to NLM and want to understand the basics
- You need a simple reference implementation
- You want to verify your installation works
- You need to understand the minimal viable brain simulation
"""

import pynlm

def main():
    """Run the basic brain simulation example"""
    print("=== Basic Brain Simulation Example ===")
    print()
    
    # 1. Create brain configuration
    print("Step 1: Creating brain configuration...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    print(f"   Created brain with default settings")
    
    # 2. Initialize the brain
    print("Step 2: Initializing brain...")
    success = brain.initialize()
    if not success:
        print("   ERROR: Brain initialization failed!")
        return
    
    print(f"   Brain initialized successfully")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Regions: {brain.getRegionCount()}")
    
    # 3. Run simulation steps
    print("Step 3: Running simulation steps...")
    print()
    
    for step in range(10):
        brain.step(step)
        
        # Print statistics every 2 steps
        if step % 2 == 0:
            print(f"Step {step:2d}: "
                  f"{brain.getFiringNeuronCount():4d} neurons firing, "
                  f"{brain.getTotalSpikeCount():4d} total spikes")
    
    # 4. Print final statistics
    print()
    print("Step 4: Final statistics...")
    print(f"   Total simulation steps: 10")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes generated: {brain.getTotalSpikeCount()}")
    
    print()
    print("=== Basic Simulation Complete ===")

if __name__ == "__main__":
    main()
