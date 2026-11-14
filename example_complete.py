#!/usr/bin/env python3
# NLM Python Example: Complete Agent Simulation
# Demonstrates the full workflow of using NLM with Python

import pynlm
import numpy as np
import time

def run_complete_example():
    """Run a complete example showing the full NLM workflow"""
    print("=== NLM Python Complete Example ===\n")
    
    # 1. Create configuration
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("random_seed", 42)
    
    # 2. Create brain
    print("2. Creating brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    
    # 3. Create agent interface
    print("3. Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create world
    print("4. Creating world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    print("5. Initializing agent with world...")
    agent.initialize(world)
    
    # 6. Enable all subsystems
    print("6. Enabling subsystems...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    print("7. Running simulation loop...")
    num_steps = 500
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step, step * 0.1)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        action_result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = action_result.reward
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"   Step {step:3d}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"     Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"     Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"     Reward: {reward:.3f}")
    
    # 8. Print final results
    print("\n=== Simulation Results ===")
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"Total simulation time: {total_time:.2f} seconds")
    print(f"Steps per second: {num_steps/total_time:.1f}")
    print(f"Total reward: {sum(action_result.reward for _ in range(num_steps))/num_steps:.3f}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # 9. Test checkpoint/save
    print("\n9. Testing checkpoint functionality...")
    checkpoint_file = "nlm_checkpoint_test.bin"
    if brain.save(checkpoint_file):
        print(f"   Checkpoint saved: {checkpoint_file}")
        
        # Load checkpoint
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        if new_brain.load(checkpoint_file):
            print("   Checkpoint loaded successfully")
            print(f"   Loaded brain has {new_brain.getTotalNeuronCount()} neurons")
        else:
            print("   ERROR: Failed to load checkpoint")
    else:
        print("   ERROR: Failed to save checkpoint")
    
    print("\n=== Example Complete ===")
    return True

if __name__ == "__main__":
    try:
        run_complete_example()
        print("\nExample completed successfully!")
    except Exception as e:
        print(f"\nError running example: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
