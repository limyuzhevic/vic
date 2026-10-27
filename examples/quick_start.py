#!/usr/bin/env python3
"""
NLM Python API - Quick Start Guide

This is the simplest possible example to get you started with NLM.
Copy this code and run it to see your first neural brain in action!

Step 1: Create a brain
Step 2: Initialize it
Step 3: Run it
Step 4: See the results!
"""

import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
print("Your brain has made", brain.getTotalSpikeCount(), "spikes so far!")

print("\nThat's it! Your brain is working!")