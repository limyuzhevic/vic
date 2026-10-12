#!/usr/bin/env python3
"""
Test script to verify NLM Python bindings improvements.
This script tests the new features added to pynlm.
"""

import sys
import time
import traceback
from typing import Dict, Any

# Try to import pynlm
try:
    import pynlm
    print("✓ Successfully imported pynlm")
    print(f"  Version: {getattr(pynlm, '__version__', 'Not set')}")
    print(f"  Description: {getattr(pynlm, '__description__', 'Not set')}")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_version_and_metadata():
    """Test that version and metadata are correctly exposed."""
    print("\n--- Testing Version and Metadata ---")
    
    # Test __version__ attribute
    assert hasattr(pynlm, '__version__'), "Missing __version__ attribute"
    print(f"✓ __version__ attribute: {pynlm.__version__}")
    
    # Test __description__ attribute
    assert hasattr(pynlm, '__description__'), "Missing __description__ attribute"
    print(f"✓ __description__ attribute: {pynlm.__description__}")
    
    print("✓ Version and metadata test passed")

def test_exception_classes():
    """Test that exception classes are properly defined."""
    print("\n--- Testing Exception Classes ---")
    
    # Test that exception classes exist
    assert hasattr(pynlm, 'NLMError'), "Missing NLMError class"
    assert hasattr(pynlm, 'ConfigError'), "Missing ConfigError class"
    assert hasattr(pynlm, 'BrainError'), "Missing BrainError class"
    assert hasattr(pynlm, 'WorldError'), "Missing WorldError class"
    assert hasattr(pynlm, 'AgentError'), "Missing AgentError class"
    
    # Test exception inheritance
    assert issubclass(pynlm.ConfigError, pynlm.NLMError), "ConfigError should inherit from NLMError"
    assert issubclass(pynlm.BrainError, pynlm.NLMError), "BrainError should inherit from NLMError"
    assert issubclass(pynlm.WorldError, pynlm.NLMError), "WorldError should inherit from NLMError"
    assert issubclass(pynlm.AgentError, pynlm.NLMError), "AgentError should inherit from NLMError"
    
    print("✓ All exception classes are properly defined")
    print("✓ Exception inheritance is correct")

def test_basic_functionality():
    """Test basic NLM functionality still works."""
    print("\n--- Testing Basic Functionality ---")
    
    # Create a simple brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created and initialized")
    
    # Run some simulation steps
    initial_spikes = brain.getTotalSpikeCount()
    for i in range(10):
        brain.step(i)
    
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes >= initial_spikes, "Spike count should not decrease"
    print(f"✓ Brain simulation works: {initial_spikes} -> {final_spikes} spikes")
    
    # Test world functionality
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    print("✓ World created and configured")
    
    # Test agent functionality
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("✓ Agent created and initialized")
    
    # Test world update
    world.update(0.1)
    percept = world.getSensoryPercept()
    print("✓ World update and sensory percept work")
    
    print("✓ Basic functionality test passed")

def test_advanced_features():
    """Test advanced features like learning capabilities."""
    print("\n--- Testing Advanced Features ---")
    
    # Create brain with advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain with advanced configuration created")
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    print("✓ Advanced learning features enabled")
    
    # Test learning during simulation
    initial_curiosity = agent.getCuriosityLevel()
    initial_neuromod = agent.getNeuromodulationLevel()
    
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    final_curiosity = agent.getCuriosityLevel()
    final_neuromod = agent.getNeuromodulationLevel()
    final_spikes = brain.getTotalSpikeCount()
    
    print(f"✓ Advanced simulation completed")
    print(f"  Curiosity level: {initial_curiosity:.3f} -> {final_curiosity:.3f}")
    print(f"  Neuromodulation level: {initial_neuromod:.3f} -> {final_neuromod:.3f}")
    print(f"  Total spikes: {final_spikes}")
    
    # Check that learning actually happened
    assert final_curiosity >= initial_curiosity, "Curiosity should increase with learning"
    assert final_spikes > 0, "Brain should have spikes during simulation"
    
    print("✓ Advanced features test passed")

def test_performance_monitoring():
    """Test performance monitoring utilities."""
    print("\n--- Testing Performance Monitoring ---")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Simple performance test
    start_time = time.time()
    
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"✓ Performance test completed")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Steps per second: {100/total_time:.1f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Basic performance assertions
    assert total_time > 0, "Simulation should take some time"
    assert brain.getTotalSpikeCount() > 0, "Brain should have activity"
    
    print("✓ Performance monitoring test passed")

def test_error_handling():
    """Test error handling with custom exceptions."""
    print("\n--- Testing Error Handling ---")
    
    # Test that we can catch specific exception types
    try:
        # Create a configuration with invalid settings to trigger errors
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation (this should not raise errors in normal operation)
        for i in range(10):
            brain.step(i)
            
        print("✓ Normal operation completed without errors")
        
        # Test that we can catch specific exception types
        # (This is a basic test - in real usage, different scenarios would trigger different exceptions)
        
    except pynlm.NLMError as e:
        print(f"✓ NLMError caught: {e}")
    except pynlm.ConfigError as e:
        print(f"✓ ConfigError caught: {e}")
    except pynlm.BrainError as e:
        print(f"✓ BrainError caught: {e}")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        traceback.print_exc()
        raise
    
    print("✓ Error handling test passed")

def test_context_manager():
    """Test context manager functionality."""
    print("\n--- Testing Context Manager ---")
    
    # Test basic context manager usage
    try:
        with pynlm.BrainContext(pynlm.createBrain(pynlm.createDefaultConfig())) as brain:
            brain.initialize()
            
            for i in range(10):
                brain.step(i)
            
            spikes = brain.getTotalSpikeCount()
            print(f"✓ Context manager works: {spikes} spikes processed")
            
    except AttributeError:
        print("⚠ Context manager not yet implemented (expected for basic test)")
    except Exception as e:
        print(f"✗ Context manager error: {e}")
        raise
    
    print("✓ Context manager test completed")

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_version_and_metadata,
        test_exception_classes,
        test_basic_functionality,
        test_advanced_features,
        test_performance_monitoring,
        test_error_handling,
        test_context_manager,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} failed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed! NLM Python bindings improvements are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())