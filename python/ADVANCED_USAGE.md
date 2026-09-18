# Copyright 2026 NLM Authors
# MIT License

# This file contains the main Python bindings for NLM
# It provides a comprehensive Python interface for the NLM neural simulation framework
# and includes extensive documentation and examples.

# The NLM (Neural Learning Machine) Python bindings
# Phase 6: Final Integration

# Basic Usage Examples

# Example 1: Create and run a simple brain
import pynlm

brain = pynlm.createQuickBrain()
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSpikeCount()} spikes")

# Example 2: Create a learning agent
brain, world, agent = pynlm.createQuickAgent(20, 20, 8, 8)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Agent simulation complete. Curiosity: {agent.getCuriosityLevel():.3f}")

# Example 3: Save and load brain states
brain.save("my_checkpoint.bin")
print("Brain saved to checkpoint")

brain.load("my_checkpoint.bin")
print("Brain loaded from checkpoint")

# Example 4: Get system information
info = pynlm.getSystemInfo()
print(info)

# Advanced Usage

# Create custom configurations
config = pynlm.loadConfig("my_config.json")
brain = pynlm.createBrain(config)

# Run complex simulations
metrics = run_complex_experiment(10000)

# Get all available types
types = pynlm.getNeuronTypes()
actions = pynlm.getActionTypes()

# Print help
pynlm.printHelp()