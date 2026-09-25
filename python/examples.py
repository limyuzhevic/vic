#!/usr/bin/env python3
"""
NLM Python Bindings Example Scripts

This file provides comprehensive examples of how to use the NLM Python bindings
for neural network simulation, learning, and experimentation.

Usage:
    python3 examples.py

Prerequisites:
    - pynlm module installed (pip install pynlm)
    - NLM C++ library built and installed
"""

import pynlm
import numpy as np

def example_basic_usage():
    """Example 1: Basic brain creation and initialization"""
    print("=== Example 1: Basic Brain Usage ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Optionally configure specific parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a simulation
    for step in range(100):
        brain.step(step)
        
    print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
    print()

def example_with_world():
    """Example 2: Brain interacting with a simulated world"""
    print("=== Example 2: Brain in a Simulated World ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    num_steps = 200
    for step in range(num_steps):
        world.update(0.1)  # Update world state
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = 0.0
        if world.getSensoryPercept().getInternal():
            reward = world.getSensoryPercept().getInternal()[0]
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"World simulation completed. Final spikes: {brain.getTotalSpikeCount()}")
    print()

def example_learning_experiment():
    """Example 3: Learning experiment with plasticity"""
    print("=== Example 3: Learning Experiment ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable different plasticity rules
    if auto* region = brain.getRegion(pynlm.RegionId(1)) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Hebbian and STDP
        }
    }
    
    # Run learning experiment
    num_steps = 500
    for step in range(num_steps):
        # Inject input pattern
        for i in range(10):
            brain.injectCurrentToNeurons(pynlm.NeuronType::Sensory, 30.0f);
        
        brain.step(step)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Learning step {step}: {brain.getTotalSpikeCount()} spikes")
    
    # Check final statistics
    print(f"Learning experiment completed:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print()

def example_serialization():
    """Example 4: Saving and loading brain state"""
    print("=== Example 4: Brain State Serialization ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation
    for step in range(100):
        brain.step(step)
    
    # Save brain state
    save_file = "brain_checkpoint.bin"
    if brain.save(save_file) {
        print(f"Brain state saved to {save_file}")
        print(f"Checkpoint size: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    } else {
        print("Failed to save brain state")
        return
    }
    
    // Create new brain and load state
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    if brain2.load(save_file) {
        print("Brain state loaded successfully")
        
        // Verify loaded state
        print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
        print(f"Loaded brain has {brain2.getTotalSynapseCount()} synapses")
        print(f"Loaded brain total spikes: {brain2.getTotalSpikeCount()}")
    } else {
        print("Failed to load brain state")
    }
    
    // Clean up
    import os
    if os.path.exists(save_file) {
        os.remove(save_file)
    }
    
    print()

def example_advanced_features():
    """Example 5: Advanced features and optimizations"""
    print("=== Example 5: Advanced Features ===")
    
    config = pynlm.createDefaultConfig()
    
    // Configure for specific use case
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 4)
    config.set("plasticity.stdp.enable", true)
    config.set("plasticity.hebbian.enable", true)
    config.set("plasticity.structural.enable", true)
    config.set("neuromod.dopamine.scale", 1.5f)
    config.set("neuromod.curiosity.enable", true)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    // Create agent with advanced capabilities
    agent = pynlm.createAgentBrain(brain)
    
    // Enable all subsystems
    agent.enableRewardModulation(true)
    agent.enableStructuralPlasticity(true)
    agent.enableDevelopment(true)
    agent.enableCuriosity(true)
    
    // Run advanced simulation
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    for step in range(300) {
        world.update(0.05)
        
        // Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        // Process with brain
        brain.step(step)
        
        // Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        // Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        // Update development
        agent.updateDevelopment(0.05)
        
        if step % 100 == 0:
            print(f"Advanced simulation step {step}:")
            print(f"  Network complexity: {brain.getTotalSynapseCount()} synapses")
            print(f"  Learning activity: {brain.getFiringNeuronCount()} neurons firing")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Advanced simulation completed successfully")
    print()

def main():
    """Run all examples"""
    print("NLM Python Bindings - Example Scripts")
    print("=====================================")
    print()
    
    try:
        example_basic_usage()
        example_with_world()
        example_learning_experiment()
        example_serialization()
        example_advanced_features()
        
        print("All examples completed successfully!")
        print()
        print("Key takeaways:")
        print("1. Basic usage: create, initialize, and simulate a brain")
        print("2. World integration: brain-agent-world interaction loop")
        print("3. Learning: plasticity rules enable adaptation")
        print("4. Persistence: save/load brain state")
        print("5. Advanced: configure complex learning scenarios")
        
    except Exception as e:
        print(f"Example failed with error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()