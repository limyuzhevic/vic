#!/usr/bin/env python3
"""
Test script to verify current Python bindings functionality
and demonstrate improvements needed.
"""

import sys
import os

# Add the build directory to Python path
sys.path.insert(0, '/workspace/app/build')

import pynlm

def test_current_bindings():
    """Test the current Python bindings API"""
    print("Testing current NLM Python bindings...")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    print(f"Created config: {config}")
    
    # Create a brain
    brain = pynlm.createBrain(config)
    print(f"Created brain: {brain}")
    
    # Initialize the brain
    brain.initialize()
    print(f"Initialized brain with {brain.getTotalNeuronCount()} neurons")
    
    # Get some statistics
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Test world creation
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    print(f"Created world: {world.getWidth()}x{world.getHeight()}")
    
    # Test agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print(f"Created agent")
    
    # Test simulation
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
    print("Current bindings test PASSED")
    
if __name__ == "__main__":
    try:
        test_current_bindings()
    except Exception as e:
        print(f"Test FAILED: {e}")
        sys.exit(1)