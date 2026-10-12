#!/usr/bin/env python3
"""
Quick start guide for NLM Python bindings.

This file provides a minimal working example to get started with the NLM Python bindings.
"""

import pynlm as nlm

def quick_start():
    """Quick start example."""
    print("NLM Python Bindings - Quick Start")
    print("=" * 40)
    
    # Create configuration
    config = nlm.create_simulation_config()
    print(f"✓ Created configuration: {config}")
    
    # Create brain
    brain = nlm.Brain(config)
    brain.initialize()
    print(f"✓ Created and initialized brain")
    
    # Create world
    world = nlm.create_test_environment()
    print(f"✓ Created test world: {world}")
    
    # Create agent brain
    agent_brain = nlm.AgentBrain(brain)
    agent_brain.initialize(world)
    print(f"✓ Created and initialized agent brain")
    
    # Run a simple simulation
    for step in range(3):
        brain.step(step)
        print(f"✓ Simulation step {step + 1}")
    
    print("\n" + "=" * 40)
    print("✓ Quick start completed successfully!")
    print("\nFor more examples, see example_pythonic_api.py")

if __name__ == "__main__":
    quick_start()