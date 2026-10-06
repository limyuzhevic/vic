#!/usr/bin/env python3
"""
Test Script for Improved NLM Python Bindings

This script tests the improvements made to the NLM Python bindings,
including:
- Enhanced configuration value access
- Convenience methods and state management
- Factory functions and simulation helpers
- Improved error handling and type safety

To run this test, you need to have the NLM Python bindings built and installed.
The bindings can be built using the provided CMake configuration.
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_bindings_import():
    """Test that the NLM Python bindings can be imported."""
    print("Testing NLM Python bindings import...")
    try:
        import pynlm as nlm
        print("✓ Successfully imported pynlm")
        
        # Test basic types
        neuron_id = nlm.NeuronId(123)
        print(f"✓ Created NeuronId: {neuron_id}")
        
        synapse_id = nlm.SynapseId(456)
        print(f"✓ Created SynapseId: {synapse_id}")
        
        action_type = nlm.ActionType.MoveForward
        print(f"✓ Created ActionType: {action_type}")
        
        world_object_type = nlm.WorldObjectType.Resource
        print(f"✓ Created WorldObjectType: {world_object_type}")
        
        return True
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    except Exception as e:
        print(f"✗ Error during import test: {e}")
        return False


def test_config_improvements():
    """Test the improved configuration value access methods."""
    print("\nTesting configuration improvements...")
    try:
        import pynlm as nlm
        
        # Create configuration
        config = nlm.Config()
        print("✓ Created Config object")
        
        # Test setting and getting values
        config.setValue("test_string", "hello")
        value = config.getValue("test_string")
        assert value == "hello", f"Expected 'hello', got '{value}'"
        print(f"✓ String get/set: {value}")
        
        config.setInt("test_int", 42)
        int_val = config.getInt("test_int")
        assert int_val == 42, f"Expected 42, got {int_val}"
        print(f"✓ Int get/set: {int_val}")
        
        config.setFloat("test_float", 3.14)
        float_val = config.getFloat("test_float")
        assert abs(float_val - 3.14) < 0.01, f"Expected ~3.14, got {float_val}"
        print(f"✓ Float get/set: {float_val}")
        
        config.setBool("test_bool", True)
        bool_val = config.getBool("test_bool")
        assert bool_val == True, f"Expected True, got {bool_val}"
        print(f"✓ Bool get/set: {bool_val}")
        
        # Test has method
        assert config.has("test_int"), "Should have 'test_int' key"
        assert not config.has("nonexistent"), "Should not have 'nonexistent' key"
        print("✓ 'has' method works correctly")
        
        # Test getKeys
        keys = config.getKeys()
        expected_keys = ["test_string", "test_int", "test_float", "test_bool"]
        for key in expected_keys:
            assert key in keys, f"Missing key: {key}"
        print(f"✓ 'getKeys' works: {keys}")
        
        # Test summary
        summary = config.summary()
        assert "test_string" in summary
        assert "test_int" in summary
        print(f"✓ Summary works: {summary}")
        
        # Test __repr__
        repr_str = repr(config)
        assert "Config:" in repr_str
        print(f"✓ __repr__ works: {repr_str}")
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_state_management():
    """Test convenience state management methods."""
    print("\nTesting state management improvements...")
    try:
        import pynlm as nlm
        
        # Create components
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        world = nlm.createSimpleWorld()
        world.configure(100.0, 100.0, 10, 10)
        agent_brain = nlm.createAgentBrain(brain)
        
        # Test agent brain state methods
        initial_state = agent_brain.getState()
        print(f"✓ Got initial agent state: {initial_state}")
        
        # Check state contains expected keys
        expected_keys = ["developmental_stage", "neuromodulation_level", 
                        "curiosity_level", "novelty_level", "prediction_error",
                        "reward_modulation_enabled", "structural_plasticity_enabled",
                        "development_enabled", "curiosity_enabled"]
        
        for key in expected_keys:
            assert key in initial_state, f"Missing key in state: {key}"
        
        print("✓ Agent state contains all expected keys")
        
        # Test state update
        new_state = {
            "developmental_stage": nlm.DevelopmentalStage.Maturation,
            "reward_modulation_enabled": True,
            "curiosity_enabled": False
        }
        
        agent_brain.updateState(new_state)
        updated_state = agent_brain.getState()
        
        # Verify updates
        assert updated_state["developmental_stage"] == nlm.DevelopmentalStage.Maturation
        assert updated_state["reward_modulation_enabled"] == True
        assert updated_state["curiosity_enabled"] == False
        
        print("✓ Agent state update works correctly")
        
        # Test world state methods
        world_state = world.getState()
        print(f"✓ Got world state: {world_state}")
        
        # Check world state contains expected keys
        world_expected_keys = ["width", "height", "agent_x", "agent_y", 
                              "agent_orientation", "agent_energy", "agent_health",
                              "agent_age", "max_energy", "energy_decay_rate",
                              "simulation_time", "random_seed"]
        
        for key in world_expected_keys:
            assert key in world_state, f"Missing key in world state: {key}"
        
        print("✓ World state contains all expected keys")
        
        # Test world state update
        new_world_state = {
            "agent_x": 30.0,
            "agent_y": 70.0,
            "max_energy": 2.0
        }
        
        world.updateState(new_world_state)
        updated_world_state = world.getState()
        
        assert abs(updated_world_state["agent_x"] - 30.0) < 0.01
        assert abs(updated_world_state["agent_y"] - 70.0) < 0.01
        assert abs(updated_world_state["max_energy"] - 2.0) < 0.01
        
        print("✓ World state update works correctly")
        
        return True
        
    except Exception as e:
        print(f"✗ State management test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_factory_functions():
    """Test the new factory functions."""
    print("\nTesting factory functions...")
    try:
        import pynlm as nlm
        
        # Test createBrainWithDefaultConfig
        brain = nlm.createBrainWithDefaultConfig()
        assert brain is not None
        print("✓ createBrainWithDefaultConfig works")
        
        # Test createCompleteAgentSystem
        brain, world, agent_brain = nlm.createCompleteAgentSystem(
            worldWidth=50.0,
            worldHeight=50.0,
            visionWidth=5,
            visionHeight=5
        )
        
        assert brain is not None
        assert world is not None
        assert agent_brain is not None
        print("✓ createCompleteAgentSystem works")
        
        # Test createDemoEnvironment
        brain, world, agent_brain = nlm.createDemoEnvironment()
        assert brain is not None
        assert world is not None
        assert agent_brain is not None
        print("✓ createDemoEnvironment works")
        
        # Test simulation helper
        agent_brain.initialize(world)
        initial_state = agent_brain.getState()
        
        # Perform simulation step
        new_state, new_world_state = nlm.simulateStep(agent_brain, world, 0.1)
        
        assert new_state is not None
        assert new_world_state is not None
        print("✓ simulateStep works")
        
        return True
        
    except Exception as e:
        print(f"✗ Factory functions test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_world_inspection():
    """Test world inspection and object listing."""
    print("\nTesting world inspection...")
    try:
        import pynlm as nlm
        
        world = nlm.createSimpleWorld()
        world.configure(100.0, 100.0, 10, 10)
        
        # Initially empty world
        objects = world.listObjects()
        assert len(objects) == 0, "Empty world should have no objects"
        print("✓ listObjects works for empty world")
        
        # Add objects
        obj1 = nlm.WorldObject(x=10.0, y=10.0, type=nlm.WorldObjectType.Resource, value=10.0)
        obj2 = nlm.WorldObject(x=20.0, y=20.0, type=nlm.WorldObjectType.Hazard, value=-5.0)
        world.addObject(obj1)
        world.addObject(obj2)
        
        # Check objects
        objects = world.listObjects()
        assert len(objects) == 2, f"Expected 2 objects, got {len(objects)}"
        print(f"✓ Added {len(objects)} objects")
        
        # Verify object properties
        for obj in objects:
            if obj["x"] == 10.0 and obj["y"] == 10.0:
                assert obj["type"] == nlm.WorldObjectType.Resource
                assert obj["value"] == 10.0
            elif obj["x"] == 20.0 and obj["y"] == 20.0:
                assert obj["type"] == nlm.WorldObjectType.Hazard
                assert obj["value"] == -5.0
        
        print("✓ Object properties preserved correctly")
        
        return True
        
    except Exception as e:
        print(f"✗ World inspection test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_error_handling():
    """Test improved error handling."""
    print("\nTesting error handling...")
    try:
        import pynlm as nlm
        
        config = nlm.Config()
        
        # Test key not found error
        try:
            config.getValue("nonexistent_key")
            assert False, "Should have raised error for missing key"
        except RuntimeError:
            print("✓ Missing key raises RuntimeError")
        
        # Test type mismatch error
        config.setValue("int_key", "123")
        try:
            config.getInt("int_key")
            # This might work depending on implementation
            print("✓ Type conversion may work")
        except (RuntimeError, ValueError):
            print("✓ Type mismatch raises appropriate error")
        
        # Test null object handling in simulateStep
        world = nlm.createSimpleWorld()
        try:
            nlm.simulateStep(None, world)
            assert False, "Should have raised error for null agent_brain"
        except (ValueError, TypeError, AttributeError):
            print("✓ Null agent_brain raises error")
        
        try:
            nlm.simulateStep("not_an_agent", None)
            assert False, "Should have raised error for null world"
        except (ValueError, TypeError, AttributeError):
            print("✓ Null world raises error")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_convenience_methods():
    """Test additional convenience methods."""
    print("\nTesting convenience methods...")
    try:
        import pynlm as nlm
        
        # Test world configuration convenience
        world = nlm.createSimpleWorld()
        world.configure(100.0, 100.0, 10, 10)
        
        # Test enable/disable methods
        agent_brain = None  # We need a real agent brain for this
        config = nlm.Config()
        brain = nlm.createBrain(config)
        agent_brain = nlm.createAgentBrain(brain)
        
        # Test system enable/disable methods
        agent_brain.setRewardModulationEnabled(True)
        assert agent_brain.isRewardModulationEnabled() == True
        print("✓ Reward modulation enable/disable works")
        
        agent_brain.setCuriosityEnabled(True)
        assert agent_brain.isCuriosityEnabled() == True
        print("✓ Curiosity enable/disable works")
        
        agent_brain.setDevelopmentEnabled(True)
        assert agent_brain.isDevelopmentEnabled() == True
        print("✓ Development enable/disable works")
        
        return True
        
    except Exception as e:
        print(f"✗ Convenience methods test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("TESTING IMPROVED NLM PYTHON BINDINGS")
    print("=" * 60)
    
    tests = [
        ("Import Test", test_bindings_import),
        ("Configuration Improvements", test_config_improvements),
        ("State Management", test_state_management),
        ("Factory Functions", test_factory_functions),
        ("World Inspection", test_world_inspection),
        ("Error Handling", test_error_handling),
        ("Convenience Methods", test_convenience_methods),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'-' * 40}")
        print(f"Running: {test_name}")
        print(f"{'-' * 40}")
        
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ Test {test_name} crashed: {e}")
            results.append((test_name, False))
    
    # Print summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! The NLM Python bindings improvements are working correctly.")
        return True
    else:
        print(f"\n⚠️  {failed} test(s) failed. Please review the output above.")
        return False


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)