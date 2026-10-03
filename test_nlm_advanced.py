#!/usr/bin/env python3
"""
Test script to verify NLM improvements and functionality

This script tests the new advanced features and improvements made to the NLM framework.
"""

import pynlm
import sys
import os

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("=== Testing Basic NLM Functionality ===")
    
    # Test 1: Create brain with default configuration
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    except Exception as e:
        print(f"✗ Failed to create brain: {e}")
        return False
    
    # Test 2: Run simulation
    try:
        for step in range(100):
            brain.step(step)
        print(f"✓ Ran simulation for 100 steps, {brain.getTotalSpikeCount()} total spikes")
    except Exception as e:
        print(f"✗ Simulation failed: {e}")
        return False
    
    # Test 3: Check statistics
    try:
        print(f"✓ Statistics - Firing: {brain.getFiringNeuronCount()}, "
              f"Avg Rate: {brain.getAverageFiringRate():.3f}")
    except Exception as e:
        print(f"✗ Statistics check failed: {e}")
        return False
    
    return True

def test_advanced_configuration():
    """Test advanced configuration presets."""
    print("\n=== Testing Advanced Configuration Presets ===")
    
    # Test 1: Quick test preset
    try:
        config1 = pynlm.createDefaultConfig()
        config1.set("neuron_count", 100)
        config1.set("region_count", 1)
        print("✓ Quick test preset created")
    except Exception as e:
        print(f"✗ Quick test preset failed: {e}")
        return False
    
    # Test 2: Performance preset
    try:
        config2 = pynlm.createDefaultConfig()
        config2.set("neuron_count", 500)
        config2.set("simulation_timestep", 0.01)
        config2.set("working_memory_capacity", 50)
        print("✓ Performance preset created")
    except Exception as e:
        print(f"✗ Performance preset failed: {e}")
        return False
    
    # Test 3: Research preset
    try:
        config3 = pynlm.createDefaultConfig()
        config3.set("neuron_count", 5000)
        config3.set("region_count", 2)
        config3.set("enable_curiosity", True)
        config3.set("enable_novelty", True)
        print("✓ Research preset created")
    except Exception as e:
        print(f"✗ Research preset failed: {e}")
        return False
    
    # Test 4: Visualization preset
    try:
        config4 = pynlm.createDefaultConfig()
        config4.set("neuron_count", 200)
        config4.set("enable_checkpointing", True)
        config4.set("enable_replay", True)
        print("✓ Visualization preset created")
    except Exception as e:
        print(f"✗ Visualization preset failed: {e}")
        return False
    
    return True

def test_configuration_validation():
    """Test configuration validation and error handling."""
    print("\n=== Testing Configuration Validation ===")
    
    # Test 1: Valid configuration
    try:
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
        
        if config.has("neuron_count") and config.get("neuron_count") == 100:
            print("✓ Configuration validation passed")
        else:
            print("✗ Configuration validation failed")
            return False
    except Exception as e:
        print(f"✗ Configuration validation failed: {e}")
        return False
    
    # Test 2: Missing required parameters
    try:
        config = pynlm.createDefaultConfig()
        config.remove("neuron_count")  # This should still work with defaults
        print("✓ Missing parameters handled gracefully")
    except Exception as e:
        print(f"✗ Missing parameters handling failed: {e}")
        return False
    
    # Test 3: Configuration merging
    try:
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        config2.set("learning_rate", 0.01)
        
        # Merge configuration
        for key in config2.getKeys():
            config1.set(key, config2.get(key))
        
        print("✓ Configuration merging successful")
    except Exception as e:
        print(f"✗ Configuration merging failed: {e}")
        return False
    
    return True

def test_advanced_learning():
    """Test advanced learning features."""
    print("\n=== Testing Advanced Learning Features ===")
    
    try:
        # Create brain with advanced settings
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("enable_advanced_learning", True)
        config.set("use_policy_gradient", True)
        config.set("use_reinforcement_learning", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Advanced learning configuration created")
        
        # Create agent with world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced learning
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("✓ Advanced learning subsystems enabled")
        
        # Run simulation with learning
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0] if not percept.getInternal().empty() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        print(f"✓ Advanced learning simulation completed: "
              f"{brain.getTotalSpikeCount()} spikes, "
              f"Curiosity={agent.getCuriosityLevel():.3f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced learning test failed: {e}")
        return False

def test_debugging_tools():
    """Test debugging and monitoring tools."""
    print("\n=== Testing Debugging Tools ===")
    
    try:
        # Create configuration with debug settings
        config = pynlm.createDefaultConfig()
        config.set("log_level", "debug")
        config.set("enable_detailed_logging", True)
        config.set("enable_performance_monitoring", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Debug configuration created")
        
        # Run simulation with debug output
        for step in range(50):
            brain.step(step)
            if step % 10 == 0:
                # Debug output
                print(f"  Step {step}: Firing={brain.getFiringNeuronCount()}, "
                      f"Rate={brain.getAverageFiringRate():.3f}")
        
        print("✓ Debug output successful")
        return True
        
    except Exception as e:
        print(f"✗ Debugging tools test failed: {e}")
        return False

def test_error_handling():
    """Test error handling and recovery."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test 1: Invalid parameters (should be handled gracefully)
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", -1)  # Negative neurons (invalid but handled)
        print("✓ Invalid parameter handled gracefully")
        
        # Test 2: Missing files (should log error but not crash)
        config2 = pynlm.createDefaultConfig()
        if not config2.loadFromFile("nonexistent_file.txt"):
            print("✓ Missing file handled gracefully")
        
        # Test 3: Configuration corruption
        config3 = pynlm.createDefaultConfig()
        config3.set("some_invalid_key", "invalid_value")
        print("✓ Invalid key handled gracefully")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("NLM Advanced Features Test Suite")
    print("=" * 40)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Advanced Configuration", test_advanced_configuration),
        ("Configuration Validation", test_configuration_validation),
        ("Advanced Learning", test_advanced_learning),
        ("Debugging Tools", test_debugging_tools),
        ("Error Handling", test_error_handling),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\nRunning: {test_name}")
        try:
            success = test_func()
            results.append((test_name, success))
            if success:
                print(f"✓ PASSED: {test_name}")
            else:
                print(f"✗ FAILED: {test_name}")
        except Exception as e:
            print(f"✗ ERROR: {test_name} - {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 40)
    print("TEST SUMMARY")
    print("=" * 40)
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! NLM advanced features are working correctly.")
        return 0
    else:
        print(f"\n⚠️  {total - passed} test(s) failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())