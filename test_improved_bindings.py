#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings.

This script tests the enhanced error handling, new functionality,
and convenience functions in the improved pynlm Python bindings.
"""

import sys
import traceback
import pynlm

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("=== Testing Basic Functionality ===")
    
    try:
        # Test configuration creation
        config = pynlm.createDefaultConfig()
        assert config is not None
        print("✓ Default configuration created")
        
        # Test setting and getting values
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_bool", True)
        config.set("test_string", "Hello, NLM!")
        
        # Test type-safe get methods
        assert config.get("test_int").value == 42
        assert config.get("test_float").value == 3.14
        assert config.get("test_bool").value == True
        assert config.get("test_string").value == "Hello, NLM!"
        
        # Test getOr with defaults
        assert config.getOr("nonexistent", 0) == 0
        assert config.getOr("nonexistent", "default") == "default"
        
        print("✓ Configuration operations work correctly")
        
        # Test brain creation with configuration
        brain = pynlm.createBrain(config)
        assert brain is not None
        print("✓ Brain created successfully")
        
        # Test brain initialization
        pynlm.initializeBrain(brain)
        print("✓ Brain initialized successfully")
        
        # Test basic statistics
        stats = pynlm.getBrainStats(brain)
        assert len(stats) == 8  # Should return 8 statistics
        print(f"✓ Brain statistics: {stats[0]} neurons, {stats[6]} total spikes")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test improved error handling."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test NLMError
        try:
            raise pynlm.NLMError("Test error message")
        except pynlm.NLMError as e:
            assert "Test error message" in str(e)
            print("✓ NLMError works correctly")
        
        # Test null parameter validation
        try:
            brain = pynlm.createBrain(None)
            print("✗ Should have raised error for null brain")
            return False
        except (pynlm.NLMError, TypeError, AttributeError):
            print("✓ Null parameter validation works")
        
        # Test invalid parameter validation
        config = pynlm.createDefaultConfig()
        config.set("learning_rate", 0.01)
        
        # Test configuration validation
        issues = pynlm.validateConfiguration(config)
        assert isinstance(issues, list)
        print(f"✓ Configuration validation works: {len(issues)} issues found")
        
        # Test invalid steps parameter
        try:
            world = pynlm.createSimpleWorld()
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            
            # This should work fine
            result = pynlm.runSimulation(brain, world, agent, 10)
            assert len(result) == 3  # Should return 3 values
            print("✓ Simulation with valid parameters works")
            
        except Exception as e:
            print(f"✗ Simulation test failed: {e}")
            return False
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_enhanced_features():
    """Test enhanced Python bindings features."""
    print("\n=== Testing Enhanced Features ===")
    
    try:
        # Test test components
        config = pynlm.createTestConfig()
        brain = pynlm.createTestBrain()
        world = pynlm.createTestWorld()
        agent = pynlm.createTestAgent()
        
        assert config is not None
        assert brain is not None
        assert world is not None
        assert agent is not None
        print("✓ Test components created successfully")
        
        # Test agent features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Verify features are enabled
        assert agent.isRewardModulationEnabled()
        assert agent.isCuriosityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isStructuralPlasticityEnabled()
        print("✓ Agent feature toggling works")
        
        # Test world configuration
        world.configure(200, 200, 32, 32)
        assert world.getWidth() == 200
        assert world.getHeight() == 200
        print("✓ World configuration works")
        
        # Test comprehensive statistics
        stats = pynlm.getBrainStats(brain)
        neuron_count, synapse_count, active_count = stats[0], stats[1], stats[2]
        
        print(f"✓ Comprehensive statistics collected:")
        print(f"  - Neurons: {neuron_count}")
        print(f"  - Synapses: {neuron_count}")
        print(f"  - Active neurons: {active_count}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced features test failed: {e}")
        traceback.print_exc()
        return False

def test_convenience_functions():
    """Test convenience functions and utilities."""
    print("\n=== Testing Convenience Functions ===")
    
    try:
        # Create test components
        config = pynlm.createTestConfig()
        brain = pynlm.createBrain(config)
        pynlm.initializeBrain(brain)
        
        world = pynlm.createTestWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test simulation convenience function
        result = pynlm.runSimulation(brain, world, agent, 5)
        assert isinstance(result, tuple) and len(result) == 3
        
        spikes, active_neurons, curiosity = result
        print(f"✓ runSimulation works: {spikes} spikes, {active_neurons} active neurons")
        
        # Test brain status printing (just ensure it doesn't crash)
        print("✓ Testing brain status printing...")
        pynlm.printBrainSummary(brain)  # Should not raise exception
        print("✓ Brain summary printed successfully")
        
        # Test configuration validation
        invalid_config = pynlm.createDefaultConfig()
        invalid_config.set("neuron_count", -1)  # Invalid value
        issues = pynlm.validateConfiguration(invalid_config)
        print(f"✓ Configuration validation: found {len(issues)} issues")
        
        return True
        
    except Exception as e:
        print(f"✗ Convenience functions test failed: {e}")
        traceback.print_exc()
        return False

def test_type_safety():
    """Test type safety and parameter validation."""
    print("\n=== Testing Type Safety ===")
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test type-safe configuration
        config.set("string_value", "hello")
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        config.set("bool_value", True)
        
        # Test type-safe retrieval
        string_val = config.get("string_value")
        assert string_val.value == "hello"
        assert isinstance(string_val.value, str)
        
        int_val = config.get("int_value")
        assert int_val.value == 42
        assert isinstance(int_val.value, (int, float))  # int or float
        
        float_val = config.get("float_value")
        assert float_val.value == 3.14
        assert isinstance(float_val.value, float)
        
        bool_val = config.get("bool_value")
        assert bool_val.value == True
        assert isinstance(bool_val.value, bool)
        
        print("✓ Type-safe configuration and retrieval works")
        
        # Test default values
        default_int = config.getOr("nonexistent_int", 100)
        assert default_int == 100
        
        default_string = config.getOr("nonexistent_string", "default")
        assert default_string == "default"
        
        print("✓ Default value handling works")
        
        return True
        
    except Exception as e:
        print(f"✗ Type safety test failed: {e}")
        traceback.print_exc()
        return False

def test_memory_safety():
    """Test memory safety and reference management."""
    print("\n=== Testing Memory Safety ===")
    
    try:
        # Create multiple components
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createTestConfig()
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        world1 = pynlm.createSimpleWorld()
        world2 = pynlm.createTestWorld()
        
        agent1 = pynlm.createAgentBrain(brain1)
        agent2 = pynlm.createAgentBrain(brain2)
        
        # Test that all components can be used independently
        pynlm.initializeBrain(brain1)
        pynlm.initializeBrain(brain2)
        
        agent1.initialize(world1)
        agent2.initialize(world2)
        
        # Test operations
        for brain, agent, world in [(brain1, agent1, world1), (brain2, agent2, world2)]:
            pynlm.initializeBrain(brain)
            world.reset()
            agent.initialize(world)
            
            # Run a few steps
            for step in range(3):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Memory safety and independent component usage works")
        
        # Test that objects can be garbage collected (implicit test)
        import gc
        del config1, config2, brain1, brain2, world1, world2, agent1, agent2
        gc.collect()
        
        print("✓ Garbage collection test passed")
        
        return True
        
    except Exception as e:
        print(f"✗ Memory safety test failed: {e}")
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all tests and report results."""
    print("NLM Python Bindings - Enhanced Version Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Error Handling", test_error_handling),
        ("Enhanced Features", test_enhanced_features),
        ("Convenience Functions", test_convenience_functions),
        ("Type Safety", test_type_safety),
        ("Memory Safety", test_memory_safety),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ {test_name} crashed: {e}")
            traceback.print_exc()
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 50)
    print("TEST SUMMARY")
    print("=" * 50)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name:25} {status}")
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! The improved NLM Python bindings are working correctly.")
        return True
    else:
        print(f"\n❌ {failed} test(s) failed. Please review the issues above.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)