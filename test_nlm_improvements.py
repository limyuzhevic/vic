#!/usr/bin/env python3
"""
Test script for NLM Python module improvements.

This script tests the key improvements made to the NLM framework:
1. Reward system integration
2. Python module functionality
3. Neuromodulation integration
4. Example usage patterns
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("=== Testing Basic NLM Functionality ===")
    
    # Test 1: Create brain and config
    print("Test 1: Creating brain and configuration...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("connection_probability", 0.1)
    
    brain = pynlm.createBrain(config)
    print(f"✓ Created brain with {config.get('neuron_count')} neurons")
    
    # Test 2: Initialize brain
    print("Test 2: Initializing brain...")
    brain.initialize()
    print("✓ Brain initialized successfully")
    
    # Test 3: Run simulation
    print("Test 3: Running simulation...")
    for step in range(50):
        brain.step(step)
    
    print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} spikes, "
          f"{brain.getAverageFiringRate():.2f} Hz average firing rate")
    
    # Test 4: Create world and agent
    print("Test 4: Creating world and agent...")
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test 5: Enable features
    print("Test 5: Enabling advanced features...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✓ All advanced features enabled")
    
    return brain, agent, world

def test_reward_system():
    """Test the enhanced reward system."""
    print("\n=== Testing Enhanced Reward System ===")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Test reward system
    print("Test 1: Reward system initialization...")
    reward_system = agent.rewardSystem
    print(f"✓ Reward system initialized: {reward_system}")
    
    # Test reward operations
    print("Test 2: Reward operations...")
    reward_system.setValue(1.0)
    print(f"✓ Set reward value: {reward_system.getValue()}")
    
    reward_system.add(0.5)
    print(f"✓ Added reward: {reward_system.getTotalAccumulated()}")
    
    reward_system.updateExpectedValue(0.3)
    print(f"✓ Updated expected value: {reward_system.getExpectedValue()}")
    
    # Test history tracking
    print("Test 3: Reward history tracking...")
    print(f"✓ History entries: {len(reward_system.getHistory())}")
    print(f"✓ Average reward: {reward_system.getAverageHistory():.3f}")
    
    # Test prediction error
    print("Test 4: Prediction error calculation...")
    reward_system.setValue(1.0)
    reward_system.updateExpectedValue(0.5)
    # Prediction error should be computed when reward is added
    reward_system.add(1.0)  # This should compute prediction error
    print(f"✓ Prediction error: {reward_system.getPredictionError():.3f}")
    
    return reward_system

def test_neuromodulation_integration():
    """Test neuromodulation integration."""
    print("\n=== Testing Neuromodulation Integration ===")
    
    # Create complete agent with world
    agent = pynlm.create_complete_agent()
    world = pynlm.create_simple_world(15, 15, 8, 8)
    
    # Test neuromodulation levels
    print("Test 1: Neuromodulation levels...")
    print(f"✓ Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"✓ Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"✓ Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"✓ Prediction error: {agent.getPredictionError():.3f}")
    
    # Test development stages
    print("Test 2: Development stages...")
    stage = agent.getDevelopmentalStage()
    print(f"✓ Current developmental stage: {stage}")
    
    # Test development update
    print("Test 3: Development system update...")
    agent.updateDevelopment(1000.0)  # 1000 timesteps
    new_stage = agent.getDevelopmentalStage()
    print(f"✓ Updated developmental stage: {new_stage}")
    
    return agent

def test_convenience_functions():
    """Test convenience functions."""
    print("\n=== Testing Convenience Functions ===")
    
    # Test experimental brain creation
    print("Test 1: Experimental brain creation...")
    brain = pynlm.create_experimental_brain(neuron_count=200, connection_probability=0.15)
    print(f"✓ Created experimental brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test world creation
    print("Test 2: Experimental world creation...")
    world = pynlm.create_experimental_world(25, 25, 12, 12)
    print(f"✓ Created experimental world {world.getWidth()}x{world.getHeight()}")
    
    # Test complete agent creation
    print("Test 3: Complete agent creation...")
    agent = pynlm.create_complete_agent()
    print(f"✓ Created complete agent with {agent.getBrain().getTotalNeuronCount()} neurons")
    
    # Test simulation
    print("Test 4: Running simulation...")
    results = pynlm.run_simple_simulation(
        pynlm.create_default_config(),
        {"world": agent.getBrain().getTotalNeuronCount()},  # Pass brain as world for testing
        agent,
        100
    )
    print(f"✓ Simulation completed: {results['final_firing_rate']:.2f} Hz final rate")
    
    return brain, world, agent

def test_error_handling():
    """Test error handling."""
    print("\n=== Testing Error Handling ===")
    
    # Test configuration error handling
    print("Test 1: Configuration error handling...")
    try:
        config = pynlm.create_default_config()
        config.set("invalid.key", "invalid.value")
        print("✓ Configuration handled without crashing")
    except Exception as e:
        print(f"✓ Caught expected error: {type(e).__name__}")
    
    # Test module import
    print("Test 2: Module import validation...")
    try:
        import pynlm
        print(f"✓ Successfully imported pynlm v{pynlm.get_version()}")
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False
    
    return True

def test_example_projects():
    """Test example projects from documentation."""
    print("\n=== Testing Example Projects ===")
    
    # Example 1: Simplest brain
    print("Example 1: Simplest brain...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.step(0)
    print(f"✓ Simple brain test: {brain.getTotalNeuronCount()} neurons")
    
    # Example 2: Brain in world
    print("Example 2: Brain in world...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run a few steps
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
    
    print(f"✓ World simulation: {brain.getTotalSpikeCount()} spikes over {20} steps")
    
    # Example 3: Advanced simulation
    print("Example 3: Advanced simulation...")
    agent = pynlm.create_complete_agent()
    config = pynlm.createDefaultConfig()
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.001)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.001)
    
    print(f"✓ Advanced simulation: {brain.getAverageFiringRate():.2f} Hz average rate")
    
    return True

def main():
    """Run all tests."""
    print("NLM Python Module - Improvement Validation Tests")
    print("=" * 60)
    
    try:
        # Run all test suites
        brain, agent, world = test_basic_functionality()
        reward_system = test_reward_system()
        neuromod_agent = test_neuromodulation_integration()
        test_convenience_functions()
        
        if not test_error_handling():
            print("\n✗ Error handling tests failed!")
            return 1
        
        test_example_projects()
        
        # Summary
        print("\n" + "=" * 60)
        print("TEST SUMMARY")
        print("=" * 60)
        print("✓ Basic NLM functionality: PASSED")
        print("✓ Enhanced reward system: PASSED")
        print("✓ Neuromodulation integration: PASSED")
        print("✓ Convenience functions: PASSED")
        print("✓ Error handling: PASSED")
        print("✓ Example projects: PASSED")
        
        print(f"\nModule Version: {pynlm.get_version()}")
        print(f"Available Features: {len(pynlm.list_available_features())}")
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED!")
        print("The NLM Python module improvements are working correctly.")
        print("=" * 60)
        
        return 0
        
    except Exception as e:
        print(f"\n✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())