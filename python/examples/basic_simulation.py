#!/usr/bin/env python3
"""
Basic Neural Network Simulation
Example 1: Basic NLM usage for simple neural simulations

This example demonstrates the fundamental usage of the enhanced Python bindings
for creating and running a basic neural network simulation.
"""

import sys
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_0a0a4035-c27c-4d0f-b5e0-c56865fd20dd/python')

import nlm_py
import time
from nlm_utils import SimulationRunner, ConfigBuilder, MetricsCollector

def main():
    print("=== Basic NLM Simulation Example ===")
    
    # Create configuration
    config = ConfigBuilder.create_default_config()
    config.max_steps = 100
    config.temporal_decay = 0.9
    config.spatial_decay = 0.7
    
    print(f"Configuration:")
    print(f"  Max steps: {config.max_steps}")
    print(f"  Timestep: {config.timestep}")
    print(f"  STDP enabled: {config.enable_stdp}")
    print(f"  Hebbian enabled: {config.enable_hebbian}")
    
    # Create brain and world
    brain = nlm_py.createBrain(config)
    world = nlm_py.createSimpleWorld()
    
    # Initialize world
    world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)
    world.setAgentStart(50.0, 50.0)
    
    # Initialize brain
    brain.initialize()
    
    print("\nStarting simulation...")
    start_time = time.time()
    
    # Run simulation
    for step in range(100):
        # Step the brain
        brain.step(step)
        
        # Update world
        world.update(step * 0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        brain.receiveSensoryInput(percept)
        
        # Get action
        action = brain.produceAction()
        world.applyMotorCommand(action, step * 0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"  Step {step}: Active neurons: {brain.getActiveNeuronCount()}, "
                  f"Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"\nSimulation completed!")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Final metrics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Save results
    brain.save("basic_simulation_final_state.nlm")
    print(f"Final state saved to 'basic_simulation_final_state.nlm'")
    
    print("\n=== Example completed successfully ===")

if __name__ == "__main__":
    main()