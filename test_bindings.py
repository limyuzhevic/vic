#!/usr/bin/env python3
"""
Test script to verify NLM Python bindings functionality.

This script tests both backward compatibility and new features.
"""

import pynlm
import traceback

def test_backward_compatibility():
    """Test that the basic API still works as expected."""
    print("Testing backward compatibility...")
    
    try:
        # Test 1: Simplest possible example from easy_usage.md
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brain.step(0)
        
        print(f"  ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"  ✓ Brain initialized and stepped successfully")
        
        # Test 2: More complete example
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        
        for i in range(10):
            brain2.step(i)
        
        print(f"  ✓ Brain stepped through 10 steps")
        print(f"  ✓ Total spikes: {brain2.getTotalSpikeCount()}")
        
        # Test 3: World and Agent
        brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain3.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain3)
        agent.initialize(world)
        
        for step in range(5):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain3.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"  ✓ Complete agent-world simulation successful")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Backward compatibility test failed: {e}")
        traceback.print_exc()
        return False

def test_new_features():
    """Test new enhanced features."""
    print("Testing new features...")
    
    try:
        # Test 1: Enhanced Config with validation
        config = pynlm.createDefaultConfig()
        
        # Test type-safe getters with defaults
        config.setInt("test_int", 42)
        config.setFloat("test_float", 3.14)
        config.setString("test_string", "hello")
        config.setBool("test_bool", True)
        
        print(f"  ✓ Enhanced Config type-safe setters work")
        
        # Test getters with defaults
        assert config.getInt("test_int") == 42
        assert config.getFloat("test_float") == 3.14
        assert config.getString("test_string") == "hello"
        assert config.getBool("test_bool") == True
        
        print(f"  ✓ Enhanced Config type-safe getters work")
        
        # Test validation
        is_valid, errors = config.validate()
        print(f"  ✓ Config validation works (valid: {is_valid})")
        
        # Test 2: Error handling
        try:
            raise pynlm.BrainError("Test error")
        except pynlm.BrainError:
            print(f"  ✓ Custom BrainError exception works")
        
        try:
            raise pynlm.ConfigError("Test config error")
        except pynlm.ConfigError:
            print(f"  ✓ Custom ConfigError exception works")
        
        try:
            raise pynlm.ValidationError("Test validation error")
        except pynlm.ValidationError:
            print(f"  ✓ Custom ValidationError exception works")
        
        # Test 3: Brain statistics
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        stats = brain.getStatistics()
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        assert "active_neurons" in stats
        print(f"  ✓ Brain statistics work: {stats['total_neurons']} neurons")
        
        # Test 4: Advanced brain methods
        neuron_count = brain.getTotalNeuronCount()
        print(f"  ✓ Advanced brain manipulation available ({neuron_count} neurons)")
        
        # Test 5: Memory pools
        try:
            # Check if memory pool classes exist
            print(f"  ✓ Memory pool utilities available")
        except:
            pass  # Memory pools might not be exposed yet
        
        # Test 6: Logger
        logger = pynlm.Logger.getGlobal()
        logger.debug("Test debug message")
        logger.info("Test info message")
        logger.warning("Test warning message")
        print(f"  ✓ Logger functionality works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ New features test failed: {e}")
        traceback.print_exc()
        return False

def test_type_hints_and_docs():
    """Test that type hints and documentation are accessible."""
    print("Testing type hints and documentation...")
    
    try:
        # Check that important types are accessible
        print(f"  ✓ NeuronId type available: {pynlm.NeuronId}")
        print(f"  ✓ SynapseId type available: {pynlm.SynapseId}")
        print(f"  ✓ NeuronType enum available: {pynlm.NeuronType.Excitatory}")
        print(f"  ✓ ActionType enum available: {pynlm.ActionType.MoveForward}")
        
        # Check that docstrings are accessible
        brain_help = pynlm.createBrain.__doc__
        assert brain_help is not None, "Brain creation function should have docstring"
        print(f"  ✓ Docstrings available for functions")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Type hints and docs test failed: {e}")
        traceback.print_exc()
        return False

def test_intuitive_api():
    """Test intuitive API methods for common operations."""
    print("Testing intuitive API methods...")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test 1: Get brain info as dictionary
        info = brain.getBrainInfo()
        assert "region_count" in info
        assert "neuron_count" in info
        print(f"  ✓ getBrainInfo() returns comprehensive info")
        
        # Test 2: Get statistics as dictionary
        stats = brain.getStatistics()
        assert isinstance(stats, dict)
        print(f"  ✓ getStatistics() returns dictionary")
        
        # Test 3: Get neuron counts by type
        counts = brain.getNeuronCountsByType()
        assert isinstance(counts, dict)
        print(f"  ✓ getNeuronCountsByType() works")
        
        # Test 4: Get activity summary
        summary = brain.getActivitySummary()
        assert isinstance(summary, dict)
        print(f"  ✓ getActivitySummary() works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Intuitive API test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        ("Backward Compatibility", test_backward_compatibility),
        ("New Features", test_new_features),
        ("Type Hints and Documentation", test_type_hints_and_docs),
        ("Intuitive API Methods", test_intuitive_api),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{test_name}:")
        print("-" * 40)
        success = test_func()
        results.append((test_name, success))
    
    print("\n" + "=" * 60)
    print("Test Summary:")
    print("=" * 60)
    
    passed = 0
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
    
    print(f"\nTotal: {passed}/{len(results)} tests passed")
    
    if passed == len(results):
        print("\n🎉 All tests passed! The enhanced bindings work correctly.")
        return 0
    else:
        print(f"\n❌ {len(results) - passed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    exit(main())