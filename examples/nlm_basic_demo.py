#!/usr/bin/env python3
"""
NLM Basic Demo - Simple demonstration of NLM Python API

This is a minimal example showing how to use the NLM Python bindings to create
and simulate a spiking neural network.

Run with: python3 nlm_basic_demo.py
"""

import pynlm
import time

def main():
    print("=== NLM Basic Demo ===")
    print("Demonstrating NLM Python API capabilities")
    print()
    
    # 1. Create a default configuration
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    config.set("random_seed", 12345)
    
    # 2. Create and initialize the brain
    print("2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   ✓ Created {brain.getTotalSynapseCount()} synapses")
    
    # 3. Create a world for agent interaction
    print("3. Creating world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Create agent brain interface
    print("4. Creating agent...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("   ✓ Agent initialized with reward, development, and curiosity")
    
    # 5. Run simulation loop
    print("5. Running simulation...")
    print("   (Press Ctrl+C to stop early)")
    print()
    
    try:
        for step in range(500):
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Step the brain
            brain.step(step, step * 0.001)
            
            # Decode motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply to world
            world.applyMotorCommand(motor_cmd, step * 0.001)
            
            # Apply reward based on activity
            reward = brain.getAverageFiringRate() / 10.0
            agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            agent.updateDevelopment(0.001)
            
            # Print progress every 50 steps
            if step % 50 == 0:
                print(f"   Step {step:3d}: {brain.getFiringNeuronCount():3d} firing, "
                      f"{agent.getCuriosityLevel():.2f} curiosity, "
                      f"{agent.getNoveltyLevel():.2f} novelty")
        
        print()
        print("✓ Simulation completed successfully!")
        
    except KeyboardInterrupt:
        print("\n   Simulation interrupted by user")
    
    # 6. Show final statistics
    print("\n6. Final Statistics:")
    print(f"   Total steps: 500")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    if brain.getWorkingMemory():
        print(f"   Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    
    if brain.getEpisodicMemory():
        print(f"   Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    print("\n=== Demo Complete ===")
    print("This demonstrates the basic NLM Python API functionality.")
    print("For more advanced examples, see:")
    print("  - examples/nlm_demo.py (comprehensive demo)")
    print("  - python/bindings.cpp (API documentation)")

if __name__ == "__main__":
    main()
