#!/usr/bin/env python3
"""
Example: Simple Neural Network Simulation with NLM

This example demonstrates how to create and run a simple neural network simulation
using the NLM (Neural Learning Machine) framework.

The simulation creates a basic agent that interacts with a simple environment,
learning through experience using the NLM's advanced neural and learning systems.
"""

import nlm
import numpy as np
import time

def main():
    """Main simulation loop demonstrating NLM usage."""
    print("=== NLM Neural Network Simulation Example ===\n")
    
    # Initialize logging
    print("Initializing NLM framework...")
    
    # Create and configure brain with NLM's enhanced configuration system
    config = nlm.createDefaultConfig()
    config.set('neuron_count', 500)
    config.set('region_count', 2)
    config.set('simulation_timestep', 0.001)
    config.set('connection_probability', 0.15)
    
    # Enable learning systems
    config.set('plasticity_learning_rate', 0.01)
    config.set('stdp_ltp_weight', 0.02)
    config.set('stdp_ltd_weight', 0.015)
    config.set('dopamine_baseline', 0.1)
    
    print(f"Configuration: {config.getOr('neuron_count', 0)} neurons, "
          f"{config.getOr('region_count', 0)} regions")
    print(f"Plasticity enabled: LTP={config.getOr('stdp_ltp_weight', 0)}, "
          f"LTD={config.getOr('stdp_ltd_weight', 0)}")
    
    # Create brain with enhanced validation
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Verify configuration validity
    if not brain.getConfig().isValid():
        print("Warning: Configuration has validation errors")
        for error in brain.getConfig().getValidationErrors():
            print(f"  - {error}")
    else:
        print("Configuration validation: PASSED")
    
    # Create environment
    print("\nCreating environment...")
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10)
    
    # Create and initialize agent with NLM's advanced agent system
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Agent initialized with learning systems enabled:")
    print("  - Reward modulation: Active")
    print("  - Curiosity learning: Active")
    print("  - Developmental plasticity: Active")
    
    # Show initial brain status
    brain.logStatus()
    
    # Run simulation loop
    print("\nStarting simulation...")
    start_time = time.time()
    
    for step in range(100):
        # Update environment
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent's neural network
        agent.processSensoryInput(percept)
        
        # Advance brain simulation
        brain.step(step)
        
        # Generate and execute action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress every 20 steps
        if step % 20 == 0:
            neurons_active = brain.getActiveNeuronCount()
            neurons_firing = brain.getFiringNeuronCount()
            spikes_this_step = brain.getTotalSpikeCount() - (step * 10)
            
            print(f"  Step {step:3d}: "
                  f"Active={neurons_active:4d}, "
                  f"Firing={neurons_firing:4d}, "
                  f"Spikes={spikes_this_step:6d}")
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Show final results
    print(f"\nSimulation completed in {elapsed:.2f} seconds")
    print(f"Average time per step: {elapsed/100:.4f} seconds")
    
    brain.logStatus()
    
    # Demonstrate advanced commands
    print("\n=== Advanced Command Examples ===")
    print("To run these commands, use the command line interface:")
    print("  nlm --config configs/my_config.cfg --brain-info")
    print("  nlm --config configs/my_config.cfg --memory-info")
    print("  nlm --config configs/my_config.cfg --performance-stats")
    print("  nlm --config configs/my_config.cfg --export-brain my_brain.bin")
    
    # Show configuration summary
    print("\n=== Configuration Summary ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Demonstrate validation
    print("\n=== Configuration Validation ===")
    validation_result = brain.getConfig().validate()
    if validation_result.isValid:
        print("✓ Configuration is valid")
        if validation_result.warnings:
            print("  Warnings:")
            for warning in validation_result.warnings:
                print(f"    - {warning}")
    else:
        print("✗ Configuration has errors:")
        for error in validation_result.errors:
            print(f"    - {error}")
    
    # Demonstrate help system
    print("\n=== Help System ===")
    help_text = brain.getConfig().getHelpText()
    print("Available configuration parameters:")
    print(help_text[:500] + "..." if len(help_text) > 500 else help_text)
    
    print("\n=== Example Complete ===")
    print("This example demonstrates:")
    print("  1. Creating and configuring a brain with NLM")
    print("  2. Setting up an agent with learning systems")
    print("  3. Running a simulation loop")
    print("  4. Using advanced commands and validation")
    print("  5. Analyzing results and configuration")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        exit(1)