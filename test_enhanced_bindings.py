#!/usr/bin/env python3
"""
Test script demonstrating the enhanced NLM Python bindings.

This script showcases the new convenience methods, error handling,
and advanced features added to the NLM Python bindings.
"""

import pynlm as nlm
import sys

def test_convenience_functions():
    """Test the new convenience functions for easy API usage."""
    print("=== Testing Convenience Functions ===")
    
    # Test 1: Default configuration
    print("\n1. Testing create_default_config()...")
    try:
        config = nlm.create_default_config()
        print(f"   ✓ Default config created: {config}")
        
        # Test preset access
        presets = nlm.get_preset_configs()
        print(f"   ✓ Available presets: {list(presets.keys())}")
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False
    
    # Test 2: Default brain creation
    print("\n2. Testing create_default_brain()...")
    try:
        brain = nlm.create_default_brain()
        print(f"   ✓ Default brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"   ✓ State: {brain}")
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False
    
    # Test 3: World creation
    print("\n3. Testing create_world()...")
    try:
        world = nlm.create_world()
        print(f"   ✓ World created: {world.getWidth()}x{world.getHeight()}")
        print(f"   ✓ Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False
    
    # Test 4: Grid world creation
    print("\n4. Testing create_grid_world()...")
    try:
        world = nlm.create_grid_world(50, 40, 64, 32)
        print(f"   ✓ Custom grid world: {world.getWidth()}x{world.getHeight()}")
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False
    
    # Test 5: Agent creation
    print("\n5. Testing create_example_agent()...")
    try:
        brain, agent_brain, world = nlm.create_example_agent()
        print(f"   ✓ Example agent created:")
        print(f"     - Brain: {brain.getTotalNeuronCount()} neurons")
        print(f"     - AgentBrain: initialized with world")
        print(f"     - World: {world.getWidth()}x{world.getHeight()}")
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False
    
    return True

def test_error_handling():
    """Test the enhanced error handling and validation."""
    print("\n\n=== Testing Error Handling ===")
    
    # Test 1: Invalid configuration validation
    print("\n1. Testing configuration validation...")
    try:
        invalid_config = {"neuron_count": "invalid"}
        valid, errors = nlm.validate_config(invalid_config)
        print(f"   ✓ Validation result: valid={valid}, errors='{errors}'")
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False
    
    # Test 2: Number formatting
    print("\n2. Testing number formatting...")
    try:
        formatted = nlm.format_number(1234567)
        print(f"   ✓ Number formatted: {formatted}")
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False
    
    # Test 3: Duration formatting
    print("\n3. Testing duration formatting...")
    try:
        duration = nlm.format_duration(3661.5)
        print(f"   ✓ Duration formatted: {duration}")
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False
    
    return True

def test_api_version():
    """Test API version information."""
    print("\n\n=== Testing API Version ===")
    
    try:
        version = nlm.version()
        print(f"   ✓ API version: {version}")
        return True
    except Exception as e:
        print(f"   ✗ Failed: {e}")
        return False

def main():
    """Run all tests."""
    print("=== NLM Enhanced Python Bindings Test Suite ===")
    print(f"Python bindings version: {nlm.version()}")
    
    tests = [
        test_convenience_functions,
        test_error_handling,
        test_api_version,
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                print(f"   Test failed: {test.__name__}")
        except Exception as e:
            print(f"   ✗ Test exception: {e}")
    
    print(f"\n\n=== Test Results ===")
    print(f"Passed: {passed}/{total}")
    print(f"Success Rate: {passed/total*100:.1f}%")
    
    if passed == total:
        print("🎉 All tests passed! Enhanced Python bindings are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
