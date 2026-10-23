"""
Example 2: Complete Agent-Environment Interaction
Demonstrates a full embodied intelligence example with world interaction.
"""

import sys
sys.path.insert(0, 'python')

import pynlm
import time
import random

def agent_environment_simulation():
    """Complete example showing agent interacting with world"""
    print("=== NLM Agent-Environment Simulation Example ===\n")
    
    # Create configuration optimized for agent training
    config = pynlm.create_default_config()
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.12)
    
    print("Creating brain with agent interfaces...")
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Create agent and world interfaces
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.create_simple_world()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Set agent starting position
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Enable important subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Agent initialized")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print()
    
    # Run simulation
    num_steps = 500
    print(f"Running simulation for {num_steps} steps...")
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world (time progresses)
        world.update(timestep=0.1)
        
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step, step * 0.1)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation based on world feedback
        reward = 0.0
        if percept.getInternal() and not percept.getInternal().empty():
            reward = percept.getInternal()[0]
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Energy: {world.getAgentBody().energy:.1f}/{world.getMaxEnergy()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nSimulation completed in {duration:.2f} seconds")
    print(f"\nFinal statistics:")
    print(f"  Total time: {world.getSimulationTime():.1f}")
    print(f"  Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Energy: {world.getAgentBody().energy:.1f}/{world.getMaxEnergy()}")
    print(f"  Firing neurons (avg): {brain.getAverageFiringRate():.3f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print()
    
    return brain, agent, world

if __name__ == "__main__":
    brain, agent, world = agent_environment_simulation()
    print("Agent-environment simulation completed successfully!")
