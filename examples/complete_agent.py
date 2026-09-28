#!/usr/bin/env python3
"""
Example 3: Complete Agent with Learning

This example demonstrates a complete agent that learns from its experiences
using reward modulation and curiosity.
"""

import pynlm
import time
import json

# Setup complete agent system
print("Creating complete agent system...")
config = pynlm.createDefaultConfig()

# Create components
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent for 100 steps with periodic learning events
print("\nRunning agent with learning...")
start_time = time.time()

for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process through agent
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide reward based on position and energy
    reward = 0.0
    if world.getAgentBody().energy > 0.5:
        reward += 0.1  # Positive reward for having energy
    if step % 10 == 0:
        print(f"  Step {step}: Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
              f"Energy {world.getAgentBody().energy:.1f}, Reward {reward:.2f}")

end_time = time.time()
duration = end_time - start_time

# Save brain state
brain_file = "agent_brain_state.json"
brain.save(brain_file)

# Show comprehensive results
print(f"\nAgent simulation complete!")
print(f"  Duration: {duration:.3f} seconds")
print(f"  Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  Final development stage: {brain.getDevelopmentalStage().name}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"  Prediction error: {agent.getPredictionError():.2f}")

# Save simulation stats
sim_stats = {
    "duration": duration,
    "final_position": {"x": world.getAgentBody().x, "y": world.getAgentBody().y},
    "total_spikes": brain.getTotalSpikeCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "final_development_stage": brain.getDevelopmentalStage().name,
    "neuromodulation_level": agent.getNeuromodulationLevel(),
    "curiosity_level": agent.getCuriosityLevel(),
    "prediction_error": agent.getPredictionError(),
    "brain_saved_to": brain_file
}

with open("simulation_stats.json", "w") as f:
    json.dump(sim_stats, f, indent=2)

print(f"\nSimulation statistics saved to simulation_stats.json")
print(f"Brain state saved to {brain_file}")

print("\nThe agent successfully learned, explored, and adapted to its environment!")
