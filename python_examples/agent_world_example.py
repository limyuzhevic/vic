#!/usr/bin/env python3
"""
NLM Python Example: Agent in a World

This example shows how to create an agent that interacts with a world,
sensing and acting within an environment.
"""

import pynlm
import time

def agent_world_example():
    """Create an agent that interacts with a world"""
    print("=== NLM Agent-World Example ===")
    
    # Step 1: Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)  # Center of world
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Setup complete:")
    print(f"  World: {world.getWidth()}x{world.getHeight()}m")
    print(f"  Agent start: (10.0, 10.0)")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  Sensory input size: {agent.getSensoryInputSize()}")
    print(f"  Motor output size: {agent.getMotorOutputSize()}")
    
    # Step 2: Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"\nEnabled features:")
    print(f"  Reward modulation: {'enabled' if agent.isRewardModulationEnabled() else 'disabled'}")
    print(f"  Curiosity: {'enabled' if agent.isCuriosityEnabled() else 'disabled'}")
    print(f"  Development: {'enabled' if agent.isDevelopmentEnabled() else 'disabled'}")
    
    # Step 3: Run simulation loop
    print(f"\nRunning simulation...")
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 20 == 0:
            print(f"  Step {step}:")
            print(f"    Agent position: ({agent.getBrain().getConfig().getOr<float>('agent.x', 0.0):.1f}, "
                  f"{agent.getBrain().getConfig().getOr<float>('agent.y', 0.0):.1f})")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Total spikes: {brain.getTotalSpikeCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Prediction error: {agent.getPredictionError():.3f}")
            print(f"    Development stage: {agent.getDevelopmentalStage()}")
    
    # Step 4: Show final results
    print(f"\n=== Final Results ===")
    print(f"Simulation completed: 200 steps")
    print(f"Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"Final energy: {world.getAgentBody().energy:.1f}/{world.getMaxEnergy()}")
    print(f"Final health: {world.getAgentBody().health:.1f}")
    print(f"Final age: {world.getAgentBody().age:.1f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return brain, world, agent

if __name__ == "__main__":
    try:
        brain, world, agent = agent_world_example()
        print(f"\n✓ Agent-World example completed successfully!")
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
