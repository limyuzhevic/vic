#!/usr/bin/env python3
"""
Example: Basic NLM Python Bindings Usage

This example demonstrates the improved API for working with the NLM
(Neural Learning Machine) neural simulation framework.
"""

import pynlm as nlm

def create_simple_brain_simulation():
    """Create a simple brain simulation using the improved bindings."""
    
    print("Creating NLM brain simulation...")
    print("=" * 40)
    
    # Create a default configuration
    config = nlm.create_default_config(
        neuron_count=100,
        region_count=2,
        connection_probability=0.1
    )
    
    print("✓ Created configuration with 100 neurons and 2 regions")
    
    # Create brain with the configuration
    brain = nlm.createBrain(config)
    print("✓ Created brain")
    
    # Initialize the brain
    success = brain.initialize()
    if not success:
        print("✗ Failed to initialize brain")
        return
    
    print("✓ Brain initialized successfully")
    
    # Create a world with agent
    world = nlm.create_world_with_agent(
        width=200.0,
        height=200.0,
        vision_width=64.0,
        vision_height=64.0
    )
    
    print("✓ Created world with agent (200x200, 64x64 vision)")
    
    # Create agent brain interface
    agent_brain = nlm.createAgentBrain(brain)
    print("✓ Created agent brain interface")
    
    # Get some basic statistics
    metrics = nlm.collect_brain_metrics(brain)
    print(f"✓ Brain statistics:")
    for key, value in metrics.items():
        print(f"    {key}: {value}")
    
    # Create context manager for simulation
    print("\nStarting simulation using context manager...")
    with nlm.BrainSimulation(brain, world) as sim:
        # Run a simple episode
        print("✓ Running simulation episode (10 steps)...")
        nlm.run_simple_episode(brain, world, max_steps=10)
        
        # Get final state
        final_metrics = nlm.collect_brain_metrics(brain)
        print(f"✓ Final simulation statistics:")
        for key, value in final_metrics.items():
            print(f"    {key}: {value}")
    
    print("\n✓ Simulation completed successfully!")
    print("=" * 40)
    print("Example completed successfully!")

if __name__ == "__main__":
    create_simple_brain_simulation()