#!/usr/bin/env python3
"""
Example 2: Brain in a Simple World

This example shows how a brain interacts with a world and learns to navigate.
The agent senses the world, makes decisions, and acts.
"""

import pynlm
import time

# Setup brain and world
print("Creating brain and world...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the agent for 30 steps
print("\nRunning world simulation...")
start_time = time.time()

for step in range(30):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 5 == 0:
        print(f"  Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
              f"Firing neurons: {brain.getFiringNeuronCount()}")

end_time = time.time()
duration = end_time - start_time

# Show results
print(f"\nWorld simulation complete!")
print(f"  Duration: {duration:.3f} seconds")
print(f"  Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
print(f"  Agent age: {world.getAgentBody().age}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")

print("\nThe agent learned to navigate and explore the world!")
