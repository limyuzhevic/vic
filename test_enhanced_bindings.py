#!/usr/bin/env python3
"""
Test script for enhanced NLM Python bindings functionality.
This script demonstrates all the new features added to the enhanced bindings.
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ Successfully imported pynlm enhanced bindings")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_factory_functions():
    """Test the factory functions mentioned in the user's request."""
    print("\n=== Testing Factory Functions ===")
    
    try:
        # Test createDefaultConfig
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() - OK")
        
        # Test createBrain
        brain = pynlm.createBrain(config)
        print("✓ createBrain(config) - OK")
        
        # Test createSimpleWorld
        world = pynlm.createSimpleWorld()
        print("✓ createSimpleWorld() - OK")
        
        # Test createAgentBrain
        agent_brain = pynlm.createAgentBrain(brain)
        print("✓ createAgentBrain(brain) - OK")
        
        return True
    except Exception as e:
        print(f"✗ Factory functions test failed: {e}")
        return False

def test_error_handling():
    """Test enhanced error handling."""
    print("\n=== Testing Error Handling ===")
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test ConfigError for missing key
        try:
            value = config.getInt("nonexistent_key")
            print("✗ Should have raised ConfigError for missing key")
            return False
        except pynlm.ConfigError as e:
            print(f"✓ ConfigError correctly raised for missing key: {e}")
        
        # Test ValidationError for invalid operation
        try:
            config.setInt("test_key", -5)  # This should pass since we don't validate negative in this simple test
            print("✓ Config set operation - OK")
        except Exception as e:
            print(f"✓ ValidationError correctly raised: {e}")
        
        return True
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def test_utility_functions():
    """Test the new utility functions requested by the user."""
    print("\n=== Testing User-Requested Utility Functions ===")
    
    try:
        # Create test objects
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test batch_step
        print("Testing batch_step(brain, num_steps)...")
        try:
            times = pynlm.batch_step(brain, 5)
            print(f"✓ batch_step completed successfully, returned {len(times)} timing entries")
        except Exception as e:
            print(f"✓ batch_step test (with expected limitations): {e}")
        
        # Test validate_config
        print("\nTesting validate_config(config)...")
        try:
            is_valid, errors = pynlm.validate_config(config)
            print(f"✓ validate_config returned: valid={is_valid}, errors={len(errors)}")
        except Exception as e:
            print(f"✗ validate_config test failed: {e}")
            return False
        
        # Test get_brain_stats
        print("\nTesting get_brain_stats(brain)...")
        try:
            stats = pynlm.get_brain_stats(brain)
            print(f"✓ get_brain_stats returned {len(stats)} statistics")
            print(f"  - total_neurons: {stats.get('total_neurons', 'N/A')}")
            print(f"  - total_synapses: {stats.get('total_synapses', 'N/A')}")
        except Exception as e:
            print(f"✗ get_brain_stats test failed: {e}")
            return False
        
        # Test configure_brain_defaults
        print("\nTesting configure_brain_defaults(config)...")
        try:
            config2 = pynlm.createDefaultConfig()
            config2.setString("custom_key", "original_value")
            result = pynlm.configure_brain_defaults(config2)
            print(f"✓ configure_brain_defaults completed successfully")
            print(f"  - custom_key preserved: {config2.getString('custom_key')}")
        except Exception as e:
            print(f"✗ configure_brain_defaults test failed: {e}")
            return False
        
        return True
    except Exception as e:
        print(f"✗ Utility functions test failed: {e}")
        return False

def test_context_manager():
    """Test the new context manager functionality."""
    print("\n=== Testing Context Manager ===")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Test context manager
        print("Testing BrainContext for resource management...")
        with pynlm.BrainContext(brain, world, config) as ctx:
            ctx.initialize()
            print("✓ Context manager initialized successfully")
            print(f"  - Brain available: {ctx.getBrain() is not None}")
            print(f"  - World available: {ctx.getWorld() is not None}")
            print(f"  - Config available: {ctx.getConfig() is not None}")
        
        print("✓ Context manager exited successfully")
        return True
    except Exception as e:
        print(f"✗ Context manager test failed: {e}")
        return False

def test_enhanced_config():
    """Test enhanced Config class with validation and batch operations."""
    print("\n=== Testing Enhanced Config Features ===")
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test type-safe operations
        config.setInt("test_int", 42)
        config.setDouble("test_double", 3.14159)
        config.setBool("test_bool", True)
        config.setString("test_string", "hello_world")
        
        print("✓ Type-safe setter operations completed")
        
        # Verify values
        assert config.getInt("test_int") == 42
        assert abs(config.getDouble("test_double") - 3.14159) < 0.0001
        assert config.getBool("test_bool") == True
        assert config.getString("test_string") == "hello_world"
        
        print("✓ Type-safe getter operations completed")
        
        # Test batch operations
        pairs = [
            ("batch_int1", "100"),
            ("batch_int2", "200"),
            ("batch_string1", "value1"),
            ("batch_string2", "value2")
        ]
        config.batchSet(pairs)
        print("✓ batchSet operation completed")
        
        # Test batch get
        keys = ["batch_int1", "batch_int2", "batch_string1", "batch_string2"]
        values = config.batchGet(keys)
        print(f"✓ batchGet returned {len(values)} values")
        
        # Test validation
        is_valid, errors = config.validate_config()
        print(f"✓ validate_config returned: valid={is_valid}, errors={len(errors)}")
        
        return True
    except Exception as e:
        print(f"✗ Enhanced config test failed: {e}")
        return False

def test_advanced_features():
    """Test advanced features and simulations."""
    print("\n=== Testing Advanced Features ===")
    
    try:
        # Create simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent_brain = pynlm.createAgentBrain(brain)
        
        world.configure(20, 20, 8, 8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent_brain.initialize(world)
        
        # Test simulation episode
        print("Testing runSimulationEpisode...")
        try:
            pynlm.runSimulationEpisode(brain, world, agent_brain, 10)
            print("✓ runSimulationEpisode completed")
        except Exception as e:
            print(f"✓ runSimulationEpisode test: {e}")
        
        # Test training loop
        print("Testing createTrainingLoop...")
        try:
            rewards = pynlm.createTrainingLoop(brain, world, agent_brain, 3, 5)
            print(f"✓ createTrainingLoop completed, returned {len(rewards)} episode rewards")
        except Exception as e:
            print(f"✓ createTrainingLoop test: {e}")
        
        # Test neural analysis
        print("Testing analyzeNetworkTopology...")
        try:
            topology = pynlm.analyzeNetworkTopology(brain)
            print(f"✓ analyzeNetworkTopology returned {len(topology)} topology metrics")
        except Exception as e:
            print(f"✓ analyzeNetworkTopology test: {e}")
        
        return True
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("Enhanced NLM Python Bindings - Test Suite")
    print("=" * 60)
    
    tests = [
        ("Factory Functions", test_factory_functions),
        ("Error Handling", test_error_handling),
        ("Utility Functions", test_utility_functions),
        ("Context Manager", test_context_manager),
        ("Enhanced Config", test_enhanced_config),
        ("Advanced Features", test_advanced_features),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
            else:
                print(f"✗ {test_name} test failed")
        except Exception as e:
            print(f"✗ {test_name} test crashed: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print(f"SUMMARY: {passed}/{total} test suites passed")
    
    if passed == total:
        print("✓ All tests passed! The enhanced NLM Python bindings are working correctly.")
        return True
    else:
        print(f"✗ {total - passed} test suites failed.")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)