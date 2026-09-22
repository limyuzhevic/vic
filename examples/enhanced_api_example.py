#!/usr/bin/env python3
"""
Example script demonstrating the enhanced NLM Python API.
This script shows how to use the new convenience methods and features.
"""

import pynlm

def main():
    print("=== Enhanced NLM Python API Examples ===\n")
    
    # Create brain and configure for enhanced usage
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("1. Basic Brain Setup")
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   Firing rate: {brain.getAverageFiringRate():.3f}")
    print()
    
    # Example with world and agent
    print("2. Agent with Enhanced Percept API")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("   Features enabled:")
    print("   - Reward modulation")
    print("   - Curiosity-driven exploration")
    print("   - Development")
    print()
    
    # Run simulation with enhanced percept access
    print("3. Running Enhanced Simulation (20 steps)")
    for step in range(20):
        world.update(0.1)
        
        # Get percept using enhanced methods
        percept = world.getSensoryPercept()
        
        # Use new convenience methods
        vision = percept.getVisionAsTensor()
        touch = percept.getTouchAsArray()
        
        print(f"   Step {step:2d}: Vision range [{min(vision):.2f}, {max(vision):.2f}], "
              f"Touch signals: {sum(touch):.1f}, Novel: {percept.isNovel()}")
        
        # Convert to dictionary for analysis
        if step == 5:  # Analyze at step 5
            percept_dict = percept.toDict()
            print(f"     Analysis at step 5: {len(percept_dict['vision'])} vision pixels")
        
        # Process and act
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print()
    print("4. Agent Statistics")
    print(f"   Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Developmental stage: {agent.getDevelopmentalStage().name}")
    print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Prediction error: {agent.getPredictionError():.3f}")
    
    print()
    print("=== Enhanced API Features Demonstrated ===")
    print("✓ Enhanced percept access with convenience methods")
    print("✓ Novelty detection for exploration")
    print("✓ Dictionary conversion for data analysis")
    print("✓ Multiple neuromodulation systems")
    print("✓ Developmental progression")
    print("✓ Curiosity-driven behavior")

if __name__ == "__main__":
    main()
