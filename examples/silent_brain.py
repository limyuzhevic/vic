#!/usr/bin/env python3
"""
Example 1: Silent Brain Simulation

This example shows the simplest possible brain simulation.
No world or agent needed - just a brain thinking by itself.
"""

import pynlm
import time

# Create a brain with default configuration
print("Creating a virtual brain...")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make the brain think for 10 steps
print("Running brain simulation...")
start_time = time.time()

for i in range(10):
    brain.step(i)
    print(f"  Step {i}: {brain.getFiringNeuronCount()} neurons firing")

end_time = time.time()
duration = end_time - start_time

# Show results
print(f"\nSimulation complete!")
print(f"  Duration: {duration:.3f} seconds")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  Development stage: {brain.getDevelopmentalStage().name}")

print("\nThis is the absolute minimum - just a brain thinking!")
