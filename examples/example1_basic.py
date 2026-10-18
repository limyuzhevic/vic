#!/usr/bin/env python3
"""
NLM Example 1: Basic Brain Simulation
=====================================

This is the simplest possible example that demonstrates the basic usage
of NLM (Neural Learning Machine). It creates a virtual brain and runs
it through some basic steps to show neural activity.

This example is perfect for beginners who want to understand the
fundamental concepts of NLM.
"""

import pynlm
import time

def main():
    print("=== NLM Example 1: Basic Brain Simulation ===\n")
    
    # Step 1: Create a configuration
    print("Step 1: Creating default configuration...")
    config = pynlm.createDefaultConfig()
    
    # Step 2: Create and initialize the brain
    print("Step 2: Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"  Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"  Brain initialized with {brain.getTotalSynapseCount()} synapses")
    
    # Step 3: Run basic simulation steps
    print("\nStep 3: Running basic simulation...")
    print("  Running 100 simulation steps...")
    
    for i in range(100):
        brain.step(i)
        
        if i % 20 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            print(f"    Step {i:3d}: {firing} firing neurons, {rate:6.2f} Hz firing rate")
    
    # Step 4: Analyze results
    print("\nStep 4: Analyzing results...")
    
    total_spikes = brain.getTotalSpikeCount()
    active_neurons = brain.getActiveNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    avg_rate = brain.getAverageFiringRate()
    ei_ratio = brain.getExcitationInhibitionRatio()
    
    print(f"  Total spikes generated: {total_spikes}")
    print(f"  Active neurons: {active_neurons}")
    print(f"  Currently firing: {firing_neurons}")
    print(f"  Average firing rate: {avg_rate:.2f} Hz")
    print(f"  Excitation/Inhibition ratio: {ei_ratio:.2f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Step 5: Demonstrate learning
    print("\nStep 5: Demonstrating learning capabilities...")
    
    # Create a world and agent for learning demonstration
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run an episode
    print("  Running agent episode...")
    
    for step in range(50):
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        if step % 10 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"    Step {step:2d}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
    
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Step 6: Save results
    print("\nStep 6: Saving results...")
    
    results_file = "./example1_results.txt"
    with open(results_file, 'w') as f:
        f.write(f"NLM Example 1: Basic Brain Simulation\n")
        f.write(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        f.write(f"Brain Statistics:\n")
        f.write(f"  Total Neurons: {brain.getTotalNeuronCount()}\n")
        f.write(f"  Total Synapses: {brain.getTotalSynapseCount()}\n")
        f.write(f"  Total Spikes: {total_spikes}\n")
        f.write(f"  Firing Neurons: {firing_neurons}\n")
        f.write(f"  Average Firing Rate: {avg_rate:.2f} Hz\n")
        f.write(f"  E/I Ratio: {ei_ratio:.2f}\n")
        f.write(f"  Development Stage: {brain.getDevelopmentalStage()}\n\n")
        f.write(f"Agent Statistics:\n")
        f.write(f"  Final Curiosity Level: {agent.getCuriosityLevel():.3f}\n")
        f.write(f"  Final Novelty Level: {agent.getNoveltyLevel():.3f}\n")
        f.write(f"  World Size: {world.getWidth()}x{world.getHeight()}\n")
    
    print(f"  Results saved to: {results_file}")
    
    print("\n=== Example 1 Complete ===")
    print("This demonstrates the basic usage of NLM with a simple brain simulation.")
    print("Key concepts shown:")
    print("  1. Creating and initializing a brain")
    print("  2. Running simulation steps")
    print("  3. Monitoring neural activity")
    print("  4. Creating agents and worlds")
    print("  5. Learning through reward and curiosity")
    
    return 0

if __name__ == "__main__":
    try:
        exit(main())
    except Exception as e:
        print(f"Error: {e}")
        print("Make sure pynlm is properly installed.")
        exit(1)