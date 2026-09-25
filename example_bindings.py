#!/usr/bin/env python3
"""
Example script demonstrating the new NLM Python bindings features.

This script shows how to use the high-level convenience functions
added to the NLM Python bindings to make development easier.
"""

import pynlm

print("NLM Python Bindings - Example Scripts")
print("=" * 50)

# Example 1: Basic Usage (Original functionality)
print("\n1. Basic Usage - Creating a simple brain")
print("-" * 40)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Example 2: Complete Agent with World
print("\n2. Complete Agent with World")
print("-" * 40)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Created complete agent setup")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Agent enabled reward modulation: {agent.isRewardModulationEnabled()}")
print(f"Agent enabled curiosity: {agent.isCuriosityEnabled()}")

# Example 3: High-Level Convenience Functions
print("\n3. High-Level Convenience Functions")
print("-" * 40)

# Create agent with learning features
brain, world, agent = pynlm.createLearningAgent(
    width=15, height=15,
    enableStructuralPlasticity=True,
    enableDevelopment=True,
    enableNovelty=True
)

print("Created learning agent with all features enabled")

# Run simulation with statistics
stats = pynlm.runSimulationWithStats(brain, world, agent, 100)
print(f"Simulation completed: {stats.totalSteps} steps")
print(f"Final spike count: {stats.finalSpikeCount}")
print(f"Average firing rate: {stats.averageFiringRate:.2f}")

# Example 4: Simulation Runner with Progress Tracking
print("\n4. Simulation Runner with Progress Tracking")
print("-" * 40)

runner = pynlm.createSimulationRunner(width=10, height=10)

def progress_callback(progress):
    if progress % 0.1 < 0.01:  # Every 10%
        print(f"  Progress: {progress * 100:.0f}%")

print("Running simulation with progress tracking...")
runner.runWithProgress(200, progressCallback=progress_callback)

brain = runner.getBrain()
print(f"Simulation complete!")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Firing: {brain.getFiringNeuronCount()}")
print(f"  Spikes: {brain.getTotalSpikeCount()}")

# Example 5: World Creation Utilities
print("\n5. World Creation Utilities")
print("-" * 40)

# Create world with obstacles
obstacle_world = pynlm.createWorldWithObstacles(
    width=30, height=30,
    obstacles=[(10, 10), (15, 15), (20, 20), (25, 25)]
)

print(f"Created obstacle world with {len(obstacle_world.getAgentBody())} objects")

# Create world with resources
resource_world = pynlm.createWorldWithResources(
    width=25, height=25,
    resources=[(5, 5), (8, 8), (12, 12), (18, 18)]
)

print(f"Created resource world with {len(resource_world.getAgentBody())} resources")

# Example 6: Simple Agent Controller
print("\n6. Simple Agent Controller")
print("-" * 40)

controller = pynlm.createSimpleAgentController(width=12, height=12)
print("Created simple agent controller")

# Run controller
controller.run()

print(f"Controller results:")
print(f"  Average firing rate: {controller.getAverageFiringRate():.2f}")
print(f"  Total spikes: {controller.getTotalSpikeCount()}")
print(f"  Region count: {controller.getRegionCount()}")

# Example 7: Training and Checkpointing
print("\n7. Training and Checkpointing")
print("-" * 40)

import tempfile
import os

# Create temporary file for trained brain
with tempfile.NamedTemporaryFile(suffix='.pkl', delete=False) as temp_file:
    temp_path = temp_file.name

# Train a brain
print(f"Training brain and saving to {temp_path}...")
brain = pynlm.trainAndSaveBrain(
    filepath=temp_path,
    width=10, height=10,
    trainingSteps=150
)

print(f"Training complete! Brain saved with {brain.getTotalNeuronCount()} neurons")

# Load the trained brain
loaded_brain = pynlm.loadTrainedBrain(temp_path)
loaded_brain.initialize()

print("Brain loaded successfully!")
print(f"Loaded brain has {loaded_brain.getTotalNeuronCount()} neurons")

# Clean up
os.unlink(temp_path)

# Example 8: Debug and Diagnostic Tools
print("\n8. Debug and Diagnostic Tools")
print("-" * 40)

# Create demo environment
demo_brain, demo_world, demo_agent = pynlm.createDemoEnvironment()

print("Created demo environment with all features enabled")
print(f"  Reward modulation: {demo_agent.isRewardModulationEnabled()}")
print(f"  Curiosity: {demo_agent.isCuriosityEnabled()}")

# Run a few steps
for step in range(20):
    demo_world.update(0.1)
    demo_agent.processSensoryInput(demo_world.getSensoryPercept())
    demo_brain.step(step)
    action = demo_agent.decodeMotorCommand()
    demo_world.applyMotorCommand(action, demo_world.getSimulationTime())

print("Demo simulation complete!")
print(f"  Firing neurons: {demo_brain.getFiringNeuronCount()}")
print(f"  Total spikes: {demo_brain.getTotalSpikeCount()}")

# Example 9: Exploration Simulation
print("\n9. Exploration Simulation")
print("-" * 40)

brain, world, agent = pynlm.runExplorationSimulation(
    worldWidth=20, worldHeight=20,
    steps=100,
    enableCuriosity=True
)

print("Exploration simulation complete!")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")

# Example 10: Available Features and Information
print("\n10. Available Features and Information")
print("-" * 40)

print(f"Version: {pynlm.getVersion()}")

print("\nAvailable features:")
features = pynlm.getFeatures()
for feature in features:
    print(f"  • {feature}")

print("\nExample patterns available:")
examples = pynlm.getExamplePatterns()
for name, example in examples.items():
    print(f"\n  {name.upper()}:")
    # Show first line of each example
    first_line = example.split('\n')[0]
    print(f"    {first_line}...")

print("\n" + "=" * 50)
print("All examples completed successfully!")
print("The NLM Python bindings now include powerful high-level")
print("convenience functions that make development easier and")
print("more intuitive for users at all skill levels.")
print("=" * 50)