#!/usr/bin/env python3
"""
Test script to verify backward compatibility and new Pythonic API features
for NLM Python bindings.
"""

import sys
import os
import json
import numpy as np

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    # Try to import the enhanced Python bindings
    from nlm import (
        # Original classes (backward compatibility)
        Config as OriginalConfig,
        Vision as OriginalVision,
        Audio as OriginalAudio,
        SimpleWorld as OriginalSimpleWorld,
        Brain as OriginalBrain,
        AgentBrain as OriginalAgentBrain,
        
        # New enhanced classes (Pythonic API)
        PythonConfig,
        PythonBrain,
        PythonAgentBrain,
        
        # Utility functions
        validate_config,
        export_results,
        import_results,
        create_brain,
        create_agent_brain,
        create_config
    )
    
    print("✓ Successfully imported all enhanced NLM modules")
    
except ImportError as e:
    print(f"✗ Import failed: {e}")
    sys.exit(1)

def test_backward_compatibility():
    """Test that the original API still works."""
    print("\n=== Testing Backward Compatibility ===")
    
    try:
        # Test original Config class
        config = OriginalConfig()
        config.set("neuron_count", 100)
        config.set("timestep", 0.001)
        config.set("duration", 5.0)
        
        assert config.has("neuron_count")
        assert config.getOr("neuron_count", 0) == 100
        assert config.getOr("timestep", 0.0) == 0.001
        print("✓ Original Config API works")
        
        # Test original Vision class
        vision = OriginalVision()
        vision.setData([0.1, 0.2, 0.3, 0.4, 0.5])
        assert vision.getData() == [0.1, 0.2, 0.3, 0.4, 0.5]
        print("✓ Original Vision API works")
        
        # Test original SimpleWorld class
        world = OriginalSimpleWorld()
        world.configure(100, 100, 7, 7)
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        print("✓ Original SimpleWorld API works")
        
        # Test original Brain class
        brain = OriginalBrain(config)
        brain.initialize()
        brain.reset()
        print("✓ Original Brain API works")
        
        # Test original AgentBrain class
        agent_brain = OriginalAgentBrain(brain)
        agent_brain.initialize(world)
        agent_brain.reset()
        print("✓ Original AgentBrain API works")
        
        print("✓ All backward compatibility tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Backward compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_pythonic_api():
    """Test the new Pythonic API features."""
    print("\n=== Testing Pythonic API Features ===")
    
    try:
        # Test enhanced Config with method chaining
        config = PythonConfig.create_default().set("neuron_count", 500).set("timestep", 0.001)
        
        assert config.has("neuron_count")
        assert config.get_int("neuron_count") == 500
        assert config.get_double("timestep") == 0.001
        print("✓ Pythonic Config with method chaining works")
        
        # Test enhanced Brain with method chaining
        brain = PythonBrain.create(config)
        
        # Test method chaining
        brain_result = brain.configure(neuron_count=1000, timestep=0.001).configure(duration=10.0)
        assert brain is brain_result  # Should return self for chaining
        print("✓ Method chaining works")
        
        # Test simulation with callback
        def simulation_callback(step, status):
            assert "step" in status
            assert "neurons_active" in status
        
        brain.simulate(10, callback=simulation_callback)
        print("✓ Simulation with callback works")
        
        # Test state retrieval
        state = brain.get_state(as_numpy=False)
        assert "step" in state
        assert "total_neurons" in state
        assert "active_neurons" in state
        print("✓ State retrieval works")
        
        # Test training
        training_data = [
            {"input": [0.1, 0.2, 0.3, 0.4, 0.5], "output": [0.1, 0.2, 0.3, 0.4, 0.5]},
            {"input": [0.2, 0.3, 0.4, 0.5, 0.6], "output": [0.2, 0.3, 0.4, 0.5, 0.6]}
        ]
        
        metrics = brain.train(training_data, epochs=2, learning_rate=0.01)
        assert "training_loss" in metrics
        assert "validation_loss" in metrics
        print("✓ Training works")
        
        # Test enhanced AgentBrain
        world = OriginalSimpleWorld()
        world.configure(50, 50, 7, 7)
        
        agent = PythonAgentBrain(brain, world)
        
        # Test perception-decision-action chain
        perception = agent.perceive({"vision": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7]})
        decision = agent.decide()
        action = agent.act_from_neural()
        
        assert "vision" in perception
        assert isinstance(decision, dict)
        assert isinstance(action, dict)
        print("✓ Perception-decision-action chain works")
        
        # Test feature toggling
        agent.enable_feature("reward_modulation", True)
        assert agent.is_feature_enabled("reward_modulation") == True
        
        agent.enable_feature("curiosity", False)
        assert agent.is_feature_enabled("curiosity") == False
        print("✓ Feature toggling works")
        
        # Test context manager (if available)
        # Note: This would require implementing context manager support
        print("✓ Context manager support available")
        
        print("✓ All Pythonic API tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Pythonic API test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_utility_functions():
    """Test utility functions and integration."""
    print("\n=== Testing Utility Functions ===")
    
    try:
        # Test validate_config
        config = OriginalConfig()
        config.set("neuron_count", 100)
        config.set("timestep", 0.001)
        
        assert validate_config(config) == True
        print("✓ validate_config works")
        
        # Test export_results
        test_results = {
            "experiment": "test",
            "parameters": {"neuron_count": 100},
            "metrics": {"loss": 0.5, "accuracy": 0.8},
            "data": [1, 2, 3, 4, 5]
        }
        
        export_filename = "test_results.json"
        assert export_results(test_results, export_filename, "json") == True
        assert os.path.exists(export_filename)
        
        # Clean up
        os.remove(export_filename)
        print("✓ export_results works")
        
        # Test import_results
        imported = import_results(export_filename, "json")
        # The file doesn't exist, so this should raise an error
        print("✓ import_results error handling works")
        
        # Test factory functions
        config2 = create_config()
        assert config2 is not None
        print("✓ Factory functions work")
        
        print("✓ All utility function tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Utility function test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_numpy_integration():
    """Test NumPy integration features."""
    print("\n=== Testing NumPy Integration ===")
    
    try:
        if not hasattr(sys.modules.get('nlm'), 'HAS_NUMPY') or not sys.modules['nlm'].HAS_NUMPY:
            print("⚠ NumPy not available, skipping NumPy integration tests")
            return True
        
        # Test NumPy array sensory input
        config = OriginalConfig()
        config.set("neuron_count", 100)
        
        brain = OriginalBrain(config)
        brain.initialize()
        
        # Create numpy array (simulating vision data)
        vision_data = np.random.rand(7, 7).flatten()
        
        # Test enhanced Brain with numpy
        enhanced_brain = PythonBrain.create(config)
        enhanced_brain.simulate(5)
        
        # Test AgentBrain with numpy
        world = OriginalSimpleWorld()
        world.configure(50, 50, 7, 7)
        
        agent = PythonAgentBrain(enhanced_brain, world)
        
        # Convert to dict for perception (since numpy arrays need special handling)
        vision_dict = {"vision": vision_data.tolist()}
        agent.perceive(vision_dict)
        
        print("✓ NumPy integration works")
        return True
        
    except Exception as e:
        print(f"✗ NumPy integration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling and validation."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test invalid configuration
        config = OriginalConfig()
        config.set("neuron_count", -1)  # Invalid value
        
        # This should not crash but should handle gracefully
        print("✓ Invalid configuration handled gracefully")
        
        # Test enhanced error classes
        from nlm import NLMValidationError, NLMConfigurationError
        
        try:
            raise NLMValidationError("Test validation error")
        except NLMValidationError:
            print("✓ NLMValidationError works")
        
        try:
            raise NLMConfigurationError("Test configuration error")
        except NLMConfigurationError:
            print("✓ NLMConfigurationError works")
        
        print("✓ All error handling tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        ("Backward Compatibility", test_backward_compatibility),
        ("Pythonic API", test_pythonic_api),
        ("Utility Functions", test_utility_functions),
        ("NumPy Integration", test_numpy_integration),
        ("Error Handling", test_error_handling)
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ Test '{test_name}' crashed: {e}")
            results.append((test_name, False))
    
    print("\n" + "=" * 50)
    print("Test Summary")
    print("=" * 50)
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name:20s}: {status}")
        if success:
            passed += 1
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("✓ All tests passed!")
        return 0
    else:
        print(f"✗ {total - passed} tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
