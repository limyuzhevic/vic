#!/usr/bin/env python3
"""
Simple demonstration script for NLM Python API.
"""

import pynlm

def main():
    print("=== NLM Enhanced Python API Examples ===\n")
    
    # Create a simple brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("1. Basic Brain Setup")
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   Current firing rate: {brain.getAverageFiringRate():.3f}")
    print()
    
    # Example with world and agent
    print("2. Complete Agent Simulation")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("   Features enabled:")
    print("   - Reward-based learning")
    print("   - Curiosity-driven exploration")
    print()
    
    # Run enhanced simulation
    print("3. Enhanced Simulation (30 steps)")
    for step in range(30):
        # World updates
        world.update(0.1)
        
        # Get percept with enhanced API
        percept = world.getSensoryPercept()
        
        # Enhanced percept methods
        vision_tensor = percept.getVisionAsTensor()
        touch_array = percept.getTouchAsArray()
        internal_array = percept.getInternalAsArray()
        
        # Novelty detection
        is_novel = percept.isNovel()
        
        # Get as dictionary for analysis
        if step == 10:
            data_dict = percept.toDict()
            print(f"   Step 10 analysis: Vision shape {len(data_dict['vision'])} elements")
        
        print(f"   Step {step:2d}: Vision max={max(vision_tensor):.2f}, "
              f"Novelty={is_novel}, Action={agent.decodeMotorCommand().name}")
        
        # Agent processing
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print()
    print("4. Final Agent Statistics")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Developmental stage: {agent.getDevelopmentalStage().name}")
    print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Prediction error: {agent.getPredictionError():.3f}")

if __name__ == "__main__":
    main()
