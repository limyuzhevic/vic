#!/usr/bin/env python3
"""
Quick Start Guide for NLM Python Bindings

This script demonstrates the most common and useful patterns for using the NLM Python bindings.
Copy and paste this into a Python interpreter or save as a script to run.
"""

import pynlm

print("=" * 60)
print("NLM Python Bindings - Quick Start Guide")
print("=" * 60)

# 1. BASIC SETUP
print("\n1. BASIC SETUP")
print("-" * 40)
brain = pynlm.createBrain()
world = pynlm.createWorld(width=100.0, height=100.0, visionWidth=20, visionHeight=20)
agent = pynlm.createAgent(brain, world)
agent.initialize(world)
print("Created brain, world, and agent successfully")

# 2. BASIC SIMULATION
print("\n2. BASIC SIMULATION")
print("-" * 40)
print("Running 10 simulation steps...")
for i in range(10):
    pynlm.simulateStep(brain, i, i * 0.1)
print("Simulation completed")

# 3. SENSOR DATA EXAMPLE
print("\n3. SENSOR DATA")
print("-" * 40)
# Create vision data (grayscale image)
import numpy as np
vision_data = np.random.rand(20, 20).flatten().astype(float)
vision = pynlm.Vision(20, 20)
vision.setData(vision_data)
print(f"Created vision input with {vision.getWidth()}x{vision.getHeight()} pixels")

# Create audio data
audio_data = np.random.rand(8800).astype(float)  # 0.2 seconds of audio
audio = pynlm.Audio(sampleRate=44100, numSamples=8800)
audio.setData(audio_data)
print(f"Created audio input with {audio.getNumSamples()} samples at {audio.getSampleRate()} Hz")

# Create sensory percept
sensory_percept = pynlm.SensoryPercept()
sensory_percept.setVision(vision_data)
sensory_percept.setAudio(audio_data)
sensory_percept.setTimestamp(0.0)
print("Created sensory percept with vision and audio data")

# 4. MONITORING
print("\n4. MONITORING")
print("-" * 40)
brain_stats = pynlm.getBrainStats(brain)
print(f"Brain stats: {brain_stats['total_neurons']} neurons, {brain_stats['firing_neurons']} firing")

world_stats = pynlm.getWorldStats(world)
print(f"World stats: Agent at ({world_stats['agent_x']:.1f}, {world_stats['agent_y']:.1f})")
print(f"Agent energy: {world_stats['agent_energy']:.1f}, health: {world_stats['agent_health']:.1f}")

# 5. EXPERIMENTATION
print("\n5. EXPERIMENTATION")
print("-" * 40)
experiment = pynlm.Experiment("quick_test")
print(f"Created experiment: {experiment.getName()}")

# Record some metrics
for step in [10, 20, 30]:
    pynlm.recordMetrics(experiment, brain, world, step)
    print(f"Recorded metrics at step {step}")

# Get experiment metrics
metrics = experiment.getMetrics()
print(f"Recorded {len(metrics)} metrics")

# 6. COGNITIVE SYSTEMS
print("\n6. COGNITIVE SYSTEMS")
print("-" * 40)
try:
    working_memory = pynlm.getNeuralWorkingMemory(brain)
    print("✓ Found neural working memory system")
    print(f"  Active memory traces: {working_memory.getActiveTraces()}")
except Exception as e:
    print(f"⚠ Neural working memory not available: {e}")

try:
    prediction_system = pynlm.getPredictionSystem(brain)
    print("✓ Found prediction system")
    print(f"  Prediction error: {prediction_system.getPredictionError():.3f}")
except Exception as e:
    print(f"⚠ Prediction system not available: {e}")

try:
    planner = pynlm.getPlanner(brain)
    print("✓ Found neural planner")
    print(f"  Planning depth: {planner.getPlanningDepth()}")
except Exception as e:
    print(f"⚠ Neural planner not available: {e}")

# 7. ACTION AND REWARD
print("\n7. ACTION AND REWARD")
print("-" * 40)
# Add reward for an action
pynlm.addRewardForAction(experiment, pynlm.ActionType.MoveForward, 1.0)
print("Added reward of 1.0 for MoveForward action")

# 8. VISUALIZATION
print("\n8. VISUALIZATION")
print("-" * 40)
try:
    nlm.plotBrainActivity(brain)
    print("✓ Brain activity plot completed")
except Exception as e:
    print(f"⚠ Visualization not available: {e}")

# 9. STATE PERSISTENCE
print("\n9. STATE PERSISTENCE")
print("-" * 40)
# Save brain state
brain.save("quick_test_brain.nlm")
print(f"✓ Saved brain state to 'quick_test_brain.nlm'")

# Reset everything
pynlm.resetAll(brain, world)
print("✓ Reset brain and world")

print("\n" + "=" * 60)
print("Quick start complete!")
print("\nFor more detailed examples, see the README.md file.")
print("For advanced usage, explore the cognitive system functions.")
print("=" * 60)
