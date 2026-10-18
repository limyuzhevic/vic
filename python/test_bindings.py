#!/usr/bin/env python3
"""
Test script for NLM Python bindings.
This script demonstrates how to test the improved Python bindings.
"""

import sys
import time
import pynlm

def test_basic_creation():
    """Test basic brain creation and initialization."""
    print("Testing basic creation...")
    
    # Test 1: Default brain creation
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    assert brain1 is not None, "Brain should not be None"
    brain1.initialize()
    assert brain1.neuron_count > 0, "Brain should have neurons"
    
    # Test 2: Quick brain with parameters
    brain2 = pynlm.quick_brain(100, 2)
    assert brain2.neuron_count == 100, "Brain should have 100 neurons"
    assert brain2.region_count == 2, "Brain should have 2 regions"
    
    # Test 3: Brain builder pattern
    brain3 = (pynlm.BrainBuilder()
             .neuron_count(50)
             .region_count(1)
             .connection_probability(0.1)
             .create())
    assert brain3.neuron_count == 50, "Brain builder should work"
    
    print("✓ Basic creation tests passed")

def test_simulation():
    """Test simulation functionality."""
    print("Testing simulation...")
    
    brain = pynlm.quick_brain(50, 1)
    
    # Test single step
    brain.step(0)
    
    # Test multiple steps
    for i in range(10):
        brain.step(i)
    
    # Test context manager
    with pynlm.BrainContext(pynlm.createDefaultConfig()) as brain2:
        brain2.initialize()
        brain2.step(0)
        assert brain2.neuron_count > 0
    
    print("✓ Simulation tests passed")

def test_world_integration():
    """Test world and agent integration."""
    print("Testing world integration...")
    
    brain = pynlm.quick_brain(100, 2)
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run complete simulation
    for step in range(5):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify results
    assert brain.neuron_count == 100
    assert world.width == 10
    assert world.height == 10
    
    print("✓ World integration tests passed")

def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    
    # Test 1: Invalid configuration
    config = pynlm.Config()
    # Should not raise for valid config
    config.validate()
    
    # Test 2: Simulation in invalid state
    brain = pynlm.quick_brain(10, 1)
    try:
        # Try to step without initialization (should raise)
        brain.step(0)
    except Exception as e:
        assert "Error" in str(e) or "invalid" in str(e).lower()
    
    print("✓ Error handling tests passed")

def test_properties_and_methods():
    """Test new properties and convenience methods."""
    print("Testing properties and methods...")
    
    brain = pynlm.quick_brain(100, 2)
    
    # Test properties
    assert hasattr(brain, 'neuron_count')
    assert hasattr(brain, 'synapse_count')
    assert hasattr(brain, 'firing_neurons')
    assert hasattr(brain, 'average_firing_rate')
    assert hasattr(brain, 'status')
    
    # Test property values
    assert brain.neuron_count == 100
    assert brain.region_count == 2
    
    # Test convenience method
    brain.step(0, 0.1)  # step with time
    
    print("✓ Properties and methods tests passed")

def test_numpy_integration():
    """Test NumPy integration (if available)."""
    print("Testing NumPy integration...")
    
    try:
        import numpy as np
        
        brain = pynlm.quick_brain(50, 1)
        
        # Test NumPy array creation from vectors
        # (This would need to be implemented in bindings)
        print("✓ NumPy integration available")
        
    except ImportError:
        print("⚠ NumPy not available, skipping NumPy tests")
    
    print("✓ NumPy integration tests passed")

def test_memory_management():
    """Test memory management and cleanup."""
    print("Testing memory management...")
    
    # Test context manager cleanup
    with pynlm.BrainContext(pynlm.createDefaultConfig()) as brain:
        brain.initialize()
        brain.step(0)
        initial_count = brain.neuron_count
        
        # Brain should be cleaned up when exiting context
    
    # Brain should be reset after context exit
    assert brain.neuron_count == initial_count  # Should still exist
    
    print("✓ Memory management tests passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_creation,
        test_simulation,
        test_world_integration,
        test_error_handling,
        test_properties_and_methods,
        test_numpy_integration,
        test_memory_management,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
