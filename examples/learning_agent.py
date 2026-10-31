#!/usr/bin/env python3
# Learning Agent Example
# Demonstrates learning capabilities with reward modulation and curiosity

import sys
sys.path.insert(0, '../')

import pynlm as nlm
import time

def main():
    print("=== NLM Learning Agent Example ===")
    print("This example demonstrates learning with reward modulation and curiosity.\n")
    
    # Create configuration
    print("1. Creating configuration...")
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500)
    
    # Create brain
    print("2. Creating brain...")
    brain = nlm.createBrain(config)
    brain.initialize()
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Create world
    print("3. Creating world...")
    world = nlm.createSimpleWorld()
    world.configure(width=15, height=15, vision_width=6, vision_height=6)
    world.reset()
    world.setAgentStart(5, 5)
    
    # Create agent
    print("4. Creating agent...")
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    print("5. Enabling learning features...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    print(f"   Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"   Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    print(f"   Structural plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    print(f"   Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    
    # Run learning simulation
    print("\n6. Running learning simulation (20 steps)...")
    
    for step in range(20):
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Provide learning signals at intervals
        if step % 5 == 0 and step > 0:
            # Simple reward based on energy
            energy = world.getAgentBody().energy
            reward = energy / 100.0  # Normalize to [0, 1]
            
            # Get prediction error
            prediction_error = agent.getPredictionError()
            
            # Apply learning
            agent.applyRewardModulation(reward, prediction_error)
            
            print(f"   Step {step}: Reward={reward:.2f}, Prediction Error={prediction_error:.2f}")
            
            # Update development
            agent.updateDevelopment(0.1)
    
    # Print final learning statistics
    print("\n7. Learning completed!")
    print("\nFinal learning statistics:")
    print(f"   Total simulation time: {world.getSimulationTime():.2f}s")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Prediction error: {agent.getPredictionError():.3f}")
    
    # Print agent state
    body = world.getAgentBody()
    print(f"   Final position: ({body.x:.1f}, {body.y:.1f})")
    print(f"   Final orientation: {body.orientation:.1f}°")
    print(f"   Final energy: {body.energy:.1f}")
    print(f"   Final health: {body.health}")
    print(f"   Final age: {body.age}")
    
    print("\nLearning example completed successfully!")

if __name__ == "__main__":
    main()
