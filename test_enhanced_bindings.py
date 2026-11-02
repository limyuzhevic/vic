#!/usr/bin/env python3
"""
Test script for verifying enhanced NLM Python bindings
"""

import pynlm
import sys
import traceback

def test_basic_functionality():
    """Test basic functionality of enhanced bindings"""
    print("=== Testing Basic Functionality ===")
    
    try:
        # Test 1: Basic creation
        print("1. Testing basic creation...")
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Test context manager
        with brain:
            brain.step(0)
        
        print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        print("   ✓ Context manager works")
        
    except Exception as e:
        print(f"   ✗ Basic functionality test failed: {e}")
        return False
    
    return True

def test_config_builder():
    """Test ConfigBuilder functionality"""
    print("\n=== Testing ConfigBuilder ===")
    
    try:
        # Test 2: ConfigBuilder with fluent API
        print("2. Testing ConfigBuilder...")
        config = (pynlm.ConfigBuilder()
            .set_neural_parameters(
                region_count=5,
                neurons_per_region=100,
                connection_density=0.1,
                learning_rate=0.01,
                decay_rate=0.995
            )
            .set_world_parameters(
                width=100,
                height=100,
                vision_width=20,
                vision_height=20,
                max_energy=100.0
            )
            .set_agent_parameters(
                max_energy=100.0,
                health=100.0,
                energy_decay_rate=0.1
            )
            .create())
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        print("   ✓ ConfigBuilder creates valid configuration")
        print(f"   ✓ Neural parameters: {config.get('neural.region_count')}")
        
        # Test validation
        config.validate()
        print("   ✓ Configuration validation works")
        
    except Exception as e:
        print(f"   ✗ ConfigBuilder test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def test_operator_overloads():
    """Test operator overloads"""
    print("\n=== Testing Operator Overloads ===")
    
    try:
        # Test 3: Vision operator overloads
        print("3. Testing Vision operator overloads...")
        vision1 = pynlm.Vision(10, 10, 3)
        vision1.set_data(list(range(10*10*3)))
        
        vision2 = pynlm.Vision(10, 10, 3)
        vision2.set_data([x * 0.5 for x in range(10*10*3)])
        
        # Test addition
        combined = vision1 + vision2
        print("   ✓ Vision addition works")
        
        # Test multiplication
        scaled = vision1 * 2.0
        print("   ✓ Vision scalar multiplication works")
        
        # Test normalization
        vision1.normalize(0.0, 1.0)
        print("   ✓ Vision normalization works")
        
        # Test scaling
        vision1.scale(0.5)
        print("   ✓ Vision scaling works")
        
    except Exception as e:
        print(f"   ✗ Operator overloads test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def test_context_managers():
    """Test context managers for all classes"""
    print("\n=== Testing Context Managers ===")
    
    try:
        # Test 4: Context managers
        print("4. Testing context managers...")
        
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        world = pynlm.create_simple_world()
        agent = pynlm.create_agent_brain(brain)
        
        # Initialize
        world.configure(50, 50, 15, 15)
        world.reset()
        agent.initialize(world)
        brain.initialize()
        
        # Use context managers
        with world as world_context:
            with brain as brain_context:
                with agent as agent_context:
                    for step in range(5):
                        world_context.update(0.1)
                        agent_context.process_sensory_input(world_context.get_sensory_percept())
                        brain_context.step(step)
                        world_context.apply_motor_command(
                            agent_context.decode_motor_command(), 
                            world_context.get_simulation_time()
                        )
        
        print("   ✓ All context managers work")
        print(f"   ✓ Simulation completed successfully")
        
    except Exception as e:
        print(f"   ✗ Context managers test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def test_iterator_support():
    """Test iterator support"""
    print("\n=== Testing Iterator Support ===")
    
    try:
        # Test 5: Iterator support
        print("5. Testing iterator support...")
        
        config = pynlm.create_default_config()
        config.set("custom.key1", "value1")
        config.set("custom.key2", "value2")
        config.set("custom.key3", "value3")
        
        # Test iteration over config
        keys = list(config)
        print(f"   ✓ Config iteration works: {len(keys)} keys")
        
        # Test dictionary-style access
        print(f"   ✓ Dictionary access: config['custom.key1'] = {config['custom.key1']}")
        
        # Test has() method
        print(f"   ✓ 'has' method: {config.has('custom.key2')}")
        
        # Test len()
        print(f"   ✓ len() method: {len(config)}")
        
        # Test contains
        print(f"   ✓ 'in' operator: {'custom.key1' in config}")
        
    except Exception as e:
        print(f"   ✗ Iterator support test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def test_error_handling():
    """Test error handling"""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test 6: Error handling
        print("6. Testing error handling...")
        
        # Test ConfigError
        try:
            config = pynlm.create_default_config()
            # This should not raise an error
            config.validate()
            print("   ✓ Config validation works")
        except Exception as e:
            print(f"   ✗ Config validation failed: {e}")
            return False
        
        # Test that Vision addition fails with different dimensions
        vision1 = pynlm.Vision(10, 10, 3)
        vision2 = pynlm.Vision(5, 5, 3)  # Different dimensions
        
        try:
            combined = vision1 + vision2
            print("   ✗ Vision addition should have failed with different dimensions")
            return False
        except Exception:
            print("   ✓ Vision addition correctly rejects mismatched dimensions")
        
    except Exception as e:
        print(f"   ✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def test_utility_functions():
    """Test utility functions"""
    print("\n=== Testing Utility Functions ===")
    
    try:
        # Test 7: Utility functions
        print("7. Testing utility functions...")
        
        # Test version
        version = pynlm.version()
        print(f"   ✓ Version: {version}")
        
        # Test system info
        system_info = pynlm.get_system_info()
        print(f"   ✓ System info retrieved")
        
        # Test current time
        current_time = pynlm.current_time()
        print(f"   ✓ Current time: {current_time}")
        
        # Test normalize
        data = [1.0, 2.0, 3.0, 4.0, 5.0]
        normalized = pynlm.normalize(data, 0.0, 1.0)
        print(f"   ✓ Normalization works: {normalized[:3]}...")
        
        # Test array_to_string
        array_str = pynlm.array_to_string([1.0, 2.5, 3.7])
        print(f"   ✓ Array to string: {array_str}")
        
    except Exception as e:
        print(f"   ✗ Utility functions test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def test_advanced_agent():
    """Test advanced agent simulation"""
    print("\n=== Testing Advanced Agent Simulation ===")
    
    try:
        # Test 8: Advanced agent simulation
        print("8. Testing advanced agent simulation...")
        
        # Use quick_start for easy initialization
        config, brain, world, agent = pynlm.quick_start()
        
        # Enable learning features
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        # Configure world
        world.configure(width=30, height=30, vision_width=10, vision_height=10)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        # Initialize agent
        agent.initialize(world)
        brain.initialize()
        
        # Run simulation with stats
        with world as world_context:
            with brain as brain_context:
                with agent as agent_context:
                    for step in range(20):
                        world_context.update(0.1)
                        
                        # Get statistics
                        if step % 5 == 0:
                            stats = agent_context.get_simulation_stats()
                            print(f"   Step {step}: {stats}")
                        
                        # Normal agent behavior
                        agent_context.process_sensory_input(world_context.get_sensory_percept())
                        brain_context.step(step)
                        world_context.apply_motor_command(
                            agent_context.decode_motor_command(),
                            world_context.get_simulation_time()
                        )
        
        print("   ✓ Advanced agent simulation completed")
        
    except Exception as e:
        print(f"   ✗ Advanced agent test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def test_backward_compatibility():
    """Test backward compatibility with original API"""
    print("\n=== Testing Backward Compatibility ===")
    
    try:
        # Test 9: Backward compatibility
        print("9. Testing backward compatibility...")
        
        # Test that all original methods still exist
        config = pynlm.create_default_config()
        
        # Original methods should still work
        config.has("test.key")
        config.getKeys()
        config.clear()
        config.summary()
        config.loadFromFile("nonexistent.txt")  # Should return False
        config.saveToFile("test_config.txt")   # Should return True if successful
        
        print("   ✓ Original API methods still work")
        
        # Test original class creation
        brain = pynlm.create_brain(config)
        world = pynlm.create_simple_world()
        agent = pynlm.create_agent_brain(brain)
        
        print("   ✓ Original class creation methods work")
        
    except Exception as e:
        print(f"   ✗ Backward compatibility test failed: {e}")
        traceback.print_exc()
        return False
    
    return True

def cleanup():
    """Clean up test files"""
    import os
    
    test_files = ["test_config.txt"]
    for file in test_files:
        if os.path.exists(file):
            os.remove(file)
            print(f"   ✓ Cleaned up {file}")

def main():
    """Run all tests"""
    print("Enhanced NLM Python Bindings - Test Suite")
    print("=" * 50)
    
    tests = [
        test_basic_functionality,
        test_config_builder,
        test_operator_overloads,
        test_context_managers,
        test_iterator_support,
        test_error_handling,
        test_utility_functions,
        test_advanced_agent,
        test_backward_compatibility,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"   ✗ Test {test.__name__} crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        print("\n❌ Test suite failed!")
        return 1
    else:
        print("\n✅ All tests passed!")
        
        # Clean up
        cleanup()
        return 0

if __name__ == "__main__":
    sys.exit(main())