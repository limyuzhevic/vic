#!/usr/bin/env python3
"""
Example 02: Agent Simulation - Complete agent with world interaction

This example demonstrates a complete agent simulation with:
- Brain, AgentBrain, and World interaction
- Sensory processing
- Motor output
- Reward-based learning
- Development features

Author: NLM Project
Date: 2026
Version: Wave 5 Enhanced
"""

import pynlm

def main():
    print("=== NLM Python Bindings - Agent Simulation Example ===")
    print()
    print("This example demonstrates a complete agent simulation with world interaction.")
    print()
    
    # Create brain with configuration
    print("1. Creating brain with configuration...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    print("   ✓ Brain created")
    
    # Create world
    print("2. Creating world...")
    world = pynlm.createSimpleWorld()
    print("   ✓ World created")
    
    # Configure world with reasonable defaults
    print("3. Configuring world...")
    pynlm.configureWorld(world, 20.0, 20.0, 8, 8)
    world.setAgentStart(1.0, 1.0)
    print("   ✓ World configured")
    
    # Create agent brain
    print("4. Creating agent brain...")
    agent = pynlm.createAgentBrain(brain)
    print("   ✓ Agent brain created")
    
    # Initialize agent with world
    print("5. Initializing agent...")
    agent.initialize(world)
    print("   ✓ Agent initialized")
    
    # Enable learning features
    print("6. Enabling learning features...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    print("   ✓ Learning features enabled")
    
    # Run simulation
    print("7. Running simulation...")
    for step in range(100):
        # Update world state
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Generate and apply motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 25 steps
        if step % 25 == 0:
            firing_count = brain.getFiringNeuronCount()
            spike_count = brain.getTotalSpikeCount()
            energy = world.getAgentBody().energy
            print(f"   Step {step:3d}: {firing_count:4d} firing, {spike_count:6d} total spikes, energy: {energy:.2f}")
    
    # Print final statistics
    print("8. Final simulation statistics...")
    final_spikes = brain.getTotalSpikeCount()
    final_firing = brain.getFiringNeuronCount()
    final_energy = world.getAgentBody().energy
    development_stage = agent.getDevelopmentalStage()
    neuromodulation = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    
    print(f"   - Total spikes: {final_spikes}")
    print(f"   - Active firing neurons: {final_firing}")
    print(f"   - Final energy: {final_energy:.2f}")
    print(f"   - Developmental stage: {development_stage}")
    print(f"   - Neuromodulation level: {neuromodulation:.2f}")
    print(f"   - Curiosity level: {curiosity:.2f}")
    
    print()
    print("=== Example Complete ===")
    print()
    print("This agent example demonstrates the complete agent simulation loop:")
    print("- Create brain, world, and agent")
    print("- Initialize agent with world")
    print("- Enable learning features (reward, curiosity, development)")
    print("- Main simulation loop: update world -> process percept -> step brain -> generate action")
    print("- Track performance metrics")
    print()
    print("For advanced features, see:")
    print("- examples/03_advanced_features.py - Advanced configuration and features")

if __name__ == "__main__":
    main()