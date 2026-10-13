#!/usr/bin/env python3
"""
Test script to validate NLM Python bindings improvements.

This script tests the new convenience functions and error handling
capabilities added to the NLM Python bindings.
"""

import sys
import os
import pynlm

def test_basic_functionality():
    """Test basic NLM functionality still works."""
    print("Testing basic NLM functionality...")
    
    # Create basic brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test basic operations
    brain.step(0)
    neurons = brain.getTotalNeuronCount()
    spikes = brain.getTotalSpikeCount()
    
    print(f"✓ Basic brain created: {neurons} neurons, {spikes} spikes")
    return True

def test_new_convenience_functions():
    """Test new convenience functions."""
    print("\nTesting new convenience functions...")
    
    try:
        # Test createStandardConfig
        config = pynlm.createStandardConfig()
        print("✓ createStandardConfig works")
        
        # Test getBrainStatistics
        brain = pynlm.createBrain(config)
        brain.initialize()
        for i in range(10):
            brain.step(i)
        
        stats = pynlm.getBrainStatistics(brain)
        print("✓ getBrainStatistics works")
        
        # Test validateConfig
        validation_result = pynlm.validateConfig(config)
        print("✓ validateConfig works")
        
        # Test safeBrainInitialize
        brain2 = pynlm.createBrain(config)
        pynlm.safeBrainInitialize(brain2)
        print("✓ safeBrainInitialize works")
        
        # Test createTrainingConfig
        training_config = pynlm.createTrainingConfig(numSteps=100)
        print("✓ createTrainingConfig works")
        
        # Test createDevelopmentConfig
        dev_config = pynlm.createDevelopmentConfig("critical")
        print("✓ createDevelopmentConfig works")
        
        # Test createTrainingEnvironment
        world, brain3, agent = pynlm.createTrainingEnvironment(10, 10, 4, 4)
        print("✓ createTrainingEnvironment works")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in convenience functions: {e}")
        return False

def test_error_handling():
    """Test custom error handling."""
    print("\nTesting custom error handling...")
    
    try:
        # Test configuration validation with invalid config
        config = pynlm.createDefaultConfig()
        validation_result = pynlm.validateConfig(config)
        
        if "valid": in validation_result:
            print("✓ Configuration validation works")
        else:
            print("✗ Configuration validation failed")
            return False
            
        return True
        
    except Exception as e:
        print(f"✗ Error in error handling: {e}")
        return False

def test_batch_operations():
    """Test batch processing capabilities."""
    print("\nTesting batch operations...")
    
    try:
        # Test createTrainingEnvironment multiple times
        for i in range(3):
            world, brain, agent = pynlm.createTrainingEnvironment(8, 8, 3, 3)
            print(f"✓ Batch environment {i+1} created")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in batch operations: {e}")
        return False

def test_progress_tracking():
    """Test progress tracking functionality."""
    print("\nTesting progress tracking...")
    
    try:
        world, brain, agent = pynlm.createTrainingEnvironment(15, 15, 5, 5)
        
        # Define progress callback
        def progress_callback(completed, progress):
            if completed % 10 == 0:
                print(f"  Progress: {progress:.1f}% ({completed} steps)")
        
        # Run simulation with progress tracking
        result = pynlm.runSimulationWithProgress(
            brain, world, agent, 50, progress_callback
        )
        
        completed, duration = result
        print(f"✓ Progress tracking completed: {completed} steps in {duration}s")
        return True
        
    except Exception as e:
        print(f"✗ Error in progress tracking: {e}")
        return False

def test_configuration_management():
    """Test configuration management utilities."""
    print("\nTesting configuration management...")
    
    try:
        # Test createAndValidateConfig
        config = pynlm.loadAndValidateConfig()
        print("✓ loadAndValidateConfig works")
        
        # Test createStandardConfig
        config2 = pynlm.createStandardConfig()
        print("✓ createStandardConfig works")
        
        # Test validateConfig with standard config
        validation_result = pynlm.validateConfig(config2)
        print("✓ validateConfig with standard config works")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in configuration management: {e}")
        return False

def test_documentation_examples():
    """Test examples from documentation."""
    print("\nTesting documentation examples...")
    
    try:
        # Test Example 1: Simplest Brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        for i in range(10):
            brain.step(i)
        print("✓ Documentation Example 1 (Simplest Brain) works")
        
        # Test Example 2: Brain in Simple World
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        agent = pynlm.createAgentBrain(brain2)
        agent.initialize(world)
        
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain2.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Documentation Example 2 (Brain in Simple World) works")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in documentation examples: {e}")
        return False

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Python Bindings - Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("New Convenience Functions", test_new_convenience_functions),
        ("Error Handling", test_error_handling),
        ("Batch Operations", test_batch_operations),
        ("Progress Tracking", test_progress_tracking),
        ("Configuration Management", test_configuration_management),
        ("Documentation Examples", test_documentation_examples),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ {test_name} failed with exception: {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name:30} {status}")
        if success:
            passed += 1
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! NLM Python bindings improvements are working correctly.")
        return True
    else:
        print(f"❌ {total - passed} tests failed. Please check the implementation.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)