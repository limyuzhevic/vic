"""
Demo utilities for NLM.

This module provides demonstration and example utilities for NLM users,
including simple demos and usage examples.
"""

import sys
import time
from typing import Optional

# Import NLM modules
import pynlm

def main():
    """Run NLM demonstration."""
    print("=== NLM Basic Demo ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Run basic simulation
    for step in range(10):
        brain.step(step)
        if step % 3 == 0:
            print(f"Step {step}:")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("\n=== Demo Complete ===")

def run_with_world():
    """Run demo with world interaction."""
    print("=== NLM World Demo ===")
    
    # Create all components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Setup complete - Brain: {brain.getTotalNeuronCount()} neurons, "
          f"World: {world.getWidth()}x{world.getHeight()}")
    
    # Run simulation
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"Step {step}:")
            print(f"  Action: {action}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Brain spikes: {brain.getTotalSpikeCount()}")
    
    print("\n=== World Demo Complete ===")

if __name__ == "__main__":
    try:
        if len(sys.argv) > 1:
            if sys.argv[1] == "--world":
                run_with_world()
            else:
                main()
        else:
            main()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
