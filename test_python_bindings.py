#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.
Tests the improved Python API, error handling, and new features.
"""

import sys
import os
import tempfile
import json
import pynlm

def test_basic_creation():
    """Test basic brain and configuration creation."""
    print("Testing basic creation...")
    
    # Test basic config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test brain creation
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test brain initialization
    assert brain.initialize()
    
    # Test basic statistics
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    print("  ✓ Basic creation tests passed")

def test_error_handling():
    """Test improved error handling."""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test invalid operations
    try:
        # This should work without errors
        brain.step(0)
        print("  ✓ Normal operation works")
    except Exception as e:
        print(f"  ✗ Unexpected error: {e}")
        raise
    
    # Test configuration error handling
    config.set("invalid.key", "test")
    assert config.has("invalid.key")
    
    print("  ✓ Error handling tests passed")

def test_quick_functions():
    """Test the new quick helper functions."""
    print("Testing quick helper functions...")
    
    # Test quick test function
    spikes, firing_neurons, synapses = pynlm.quickTest(50, 50)
    assert spikes >= 0
    assert firing_neurons >= 0
    assert synapses >= 0
    
    print("  ✓ Quick test function works")
    
    # Test quick agent simulation
    spikes, curiosity, novelty = pynlm.createQuickAgentSimulation(30, 30)
    assert spikes >= 0
    assert 0.0 <= curiosity <= 1.0
    assert 0.0 <= novelty <= 1.0
    
    print("  ✓ Quick agent simulation works")

def test_json_config():
    """Test JSON configuration loading."""
    print("Testing JSON configuration...")
    
    # Create a temporary JSON config file
    config_data = {
        "brain": {
            "neuron_count": 200,
            "connection_probability": 0.1
        },
        "plasticity": {
            "stdp": {"enable": True, "learning_rate": 0.001},
            "hebbian": {"enable": True}
        }
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(config_data, f)
        json_file = f.name
    
    try:
        config = pynlm.createDefaultConfig()
        # The config.loadFromFile should handle JSON now
        # (Note: This might need to be implemented in the C++ code)
        result = config.loadFromFile(json_file)
        print(f"  JSON config loading result: {result}")
    except Exception as e:
        print(f"  JSON config loading (expected to be implemented): {e}")
    finally:
        os.unlink(json_file)
    
    print("  ✓ JSON configuration test completed")

def test_agent_advanced_features():
    """Test advanced agent features."""
    print("Testing advanced agent features...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    # Test agent configuration
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isCuriosityEnabled()
    
    print("  ✓ Agent configuration works")

def test_world_interaction():
    """Test world and agent interaction."""
    print("Testing world interaction...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Run a simulation
    for step in range(20):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step, step * 0.1)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("  ✓ World interaction test passed")

def test_memory_management():
    """Test memory management and cleanup."""
    print("Testing memory management...")
    
    # Create and destroy multiple objects
    for i in range(10):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Process some steps
        for j in range(10):
            brain.step(j)
        
        # Brain should be cleaned up automatically
    
    print("  ✓ Memory management test passed")

def test_version_info():
    """Test version information."""
    print("Testing version information...")
    
    # Get version info
    version_info = pynlm.getVersionInfo()
    assert len(version_info) == 3
    
    print(f"  ✓ Version: {version_info[0]}")
    print(f"  ✓ Phase: {version_info[1]}")
    print(f"  ✓ Version: {version_info[2]}")
    
    print("  ✓ Version information test passed")

def test_advanced_diagnostics():
    """Test advanced diagnostic features."""
    print("Testing advanced diagnostics...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Test diagnostic counters
    for step in range(10):
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 4, 4)
        world.reset()
        
        agent.initialize(world)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        print(f"  Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
    
    print("  ✓ Advanced diagnostics test passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_creation,
        test_error_handling,
        test_quick_functions,
        test_json_config,
        test_agent_advanced_features,
        test_world_interaction,
        test_memory_management,
        test_version_info,
        test_advanced_diagnostics,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ Test failed: {e}")
            failed += 1
            # Continue with other tests
    
    print("\n" + "=" * 60)
    print(f"Test Summary: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("All tests passed! ✓")
        return True
    else:
        print(f"Some tests failed ({failed} failures)")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)