#!/usr/bin/env python3
"""
Quick Start Guide for NLM Python Bindings

This is the simplest possible example to get you started with NLM (Neural Learning Machine).
Copy and paste this into a Python file and run it to see your first virtual brain!

What you'll learn:
- How to create a virtual brain
- How to make it "think"
- How to see what your brain is doing

This is adapted from the official NLM documentation and easy_usage.md
"""

import pynlm

print("=== NLM Quick Start ===\n")

# Step 1: Create a virtual brain with default configuration
print("Step 1: Creating a virtual brain...")
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize the brain (essential before use)
print("Step 2: Initializing the brain...")
brain.initialize()

# Step 3: Make the brain think for 100 steps
print("Step 3: Making the brain think for 100 steps...")
print("(This simulates 100 moments in time)\n")

for i in range(100):
    brain.step(i)  # One step of thinking
    if i % 25 == 0:
        firing = brain.getFiringNeuronCount()
        print(f"  Step {i:3d}: {firing} neurons firing")

# Step 4: See the results
print("\nStep 4: Results!")
print(f"  Total neurons in brain: {brain.getTotalNeuronCount()}")
print(f"  Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  Developmental stage: {brain.getDevelopmentalStage().name}")

print("\n=== Your Brain is Now Thinking! ===")
print("\nThe brain has created and processed 100 time steps.")
print("Your virtual brain is working and learning through neural dynamics!")
print("\nNext steps:")
print("1. Check out advanced_usage.py for more complex examples")
print("2. Try creating a world and agent in the advanced examples")
print("3. Experiment with different configurations")

print("\n=== Quick Start Complete! ===")