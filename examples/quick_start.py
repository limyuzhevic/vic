#!/usr/bin/env python3
"""
NLM Python Bindings - Quick Start Guide

This is a minimal example showing how to get started with the enhanced NLM Python bindings.
Copy this code and run it to see the enhanced functionality in action.

Author: NLM Team
Version: Enhanced Edition
"""

import pynlm

print("Welcome to NLM Python Bindings - Enhanced Version")
print("=" * 50)

# Create everything with enhanced convenience functions
config, brain, world, agent = pynlm.create_experimental_setup()

# Initialize agent
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
print("Running simulation...")
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Show results
print("\nSimulation Complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

print("\nEnhanced features demonstrated:")
print("✓ Enhanced configuration builder")
print("✓ Experimental setup helper")
print("✓ Agent initialization with world")
print("✓ Multiple learning subsystems enabled")
print("✓ Complete simulation loop")
