#!/usr/bin/env python3
"""
Basic example showing the simplicity and power of NLM Python API
Works with both basic and enhanced versions
"""

import pynlm

print("NLM Python API - Basic Example")
print("=" * 40)

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run simple simulation
print("\nRunning simulation...")
for i in range(10):
    brain.step(i)
    if i % 5 == 0:
        print(f"  Step {i}: {brain.getFiringNeuronCount()} firing neurons")

print(f"\nSimulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")

print("\n✓ Basic example completed successfully!")
