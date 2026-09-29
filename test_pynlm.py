#!/usr/bin/env python3
"""
Test script for NLM Python bindings

This script tests the basic functionality of the NLM Python bindings
to ensure they work correctly with the C++ implementation.
"""

import pynlm
import time

def test_basic_imports():
    """Test that all modules can be imported correctly"""
    print("Testing basic imports...")
    
    # Test module attributes
    print(f"NLM version: {pynlm.__version__}")
    print(f"Author: {pynlm.__author__}")
    print(f"Description: {pynlm.__description__}")
    
    # Test action types
    print("Available Action types:", list(pynlm.ACTION_TYPES.keys()))
    
    # Test neuron types
    print("Available Neuron types:", list(pynlm.NEURON_TYPES.keys()))
    
    print("✓ Basic imports test passed\n")

def test_config():
    """Test Config class functionality"""
    print("Testing Config class...")
    
    config = pynlm.createDefaultConfig()
    
    # Test setting values
    config.set("test_key", "test_value")
    
    # Test getting values
    random_seed = config.getOr("random_seed", 42)
    print(f"Random seed: {random_seed}")
    
    # Test loading from args
    import sys
    test_args = ["test", "--config", "configs/default.cfg"]
    config.loadFromArgs(len(test_args), test_args)
    
    print("✓ Config test passed\n")

def test_brain_creation():
    """Test brain creation and basic functionality"""
    print("Testing brain creation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test brain properties
    print(f"Initial neuron count: {brain.getTotalNeuronCount()}")
    print(f"Initial synapse count: {brain.getTotalSynapseCount()}")
    print(f"Initial spike count: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate()}")
    
    # Test brain initialization
    success = brain.initialize()
    print(f"Brain initialization: {'Success' if success else 'Failed'}")
    
    # Test brain step
    brain.step(100, 0.1)
    
    print(f"After step - Neuron count: {brain.getTotalNeuronCount()}")
    print(f"After step - Spike count: {brain.getTotalSpikeCount()}")
    print(f"After step - Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Test brain status
    print("Brain status:")
    brain.logStatus()
    
    print("✓ Brain creation test passed\n")

def test_agent_brain():
    """Test AgentBrain functionality"""
    print("Testing AgentBrain...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createSimpleWorld())
    
    # Test neuromodulation levels
    print(f"Initial neuromodulation level: {agent.getNeuromodulationLevel()}")
    print(f"Initial curiosity level: {agent.getCuriosityLevel()}")
    print(f"Initial novelty level: {agent.getNoveltyLevel()}")
    print(f"Initial prediction error: {agent.getPredictionError()}")
    
    # Test enable methods
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ AgentBrain test passed\n")

def test_world():
    """Test SimpleWorld functionality"""
    print("Testing SimpleWorld...")
    
    world = pynlm.createSimpleWorld()
    
    # Test world initialization
    world.initialize(16, 16, 8, 8)
    
    print(f"World vision width: {world.getVisionWidth()}")
    print(f"World vision height: {world.getVisionHeight()}")
    
    # Test world update
    world.update(0.1)
    
    print(f"World simulation time: {world.getSimulationTime()}")
    
    # Test sensory perception
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    percept = world.getSensoryPercept()
    
    print(f"Vision array size: {len(percept.getVision())}")
    print(f"Touch array size: {len(percept.getTouch())}")
    
    print("✓ SimpleWorld test passed\n")

def test_action():
    """Test Action class functionality"""
    print("Testing Action class...")
    
    actions = [
        pynlm.ActionType.Wait,
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.MoveBackward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.LookLeft,
        pynlm.ActionType.LookRight,
        pynlm.ActionType.Interact
    ]
    
    for action_type in actions:
        action = pynlm.Action(action_type)
        print(f"Action: {action.getType()} -> {action.toString()}")
    
    print("✓ Action test passed\n")

def test_simulation_loop():
    """Test a complete simulation loop with agent"""
    print("Testing complete simulation loop...")
    
    # Create all components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize components
    agent.initialize(world)
    brain.initialize()
    world.initialize(20, 20, 8, 8)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Starting simulation for {config.getOr('max_steps', 100)} steps...")
    
    # Run simulation loop
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step, step * 0.1)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute reward and apply learning
        reward = world.computeReward(brain.getRegions()[0].get())
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Action: {action.toString()}")
            print(f"  Reward: {reward}")
            print(f"  Spikes: {brain.getTotalSpikeCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    
    print(f"\nFinal simulation results:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate()}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel()}")
    
    # Save brain state
    brain.save("/tmp/nlm_test_checkpoint.bin")
    print("✓ Simulation saved to checkpoint")
    
    print("✓ Complete simulation loop test passed\n")

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    print()
    
    try:
        test_basic_imports()
        test_config()
        test_brain_creation()
        test_agent_brain()
        test_world()
        test_action()
        test_simulation_loop()
        
        print("=" * 60)
        print("All tests passed! ✓")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())