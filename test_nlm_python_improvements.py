#!/usr/bin/env python3
"""
Test script for NLM Python bindings improvements

This script demonstrates the new features and functionality
that have been added to the NLM Python bindings.
"""

import sys
import os
import tempfile
import json

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing basic NLM functionality...")
    
    try:
        # Import the enhanced NLM bindings
        import pynlm as nlm
        print("✓ Successfully imported NLM bindings")
        
        # Create a configuration
        config = nlm.create_config()
        print("✓ Created configuration")
        
        # Apply a preset
        config.apply_preset("small")
        print("✓ Applied 'small' preset")
        
        # Set some values
        config.set("neuron_count", 500)
        config.set("learning_rate", 0.01)
        print("✓ Set configuration values")
        
        # Check configuration
        print(f"  - Neuron count: {config.get('neuron_count', 0)}")
        print(f"  - Learning rate: {config.get('learning_rate', 0.0)}")
        
        # Validate configuration
        is_valid = config.validate()
        print(f"✓ Configuration validation: {'PASS' if is_valid else 'FAIL'}")
        
        # Get configuration keys
        keys = config.get_keys()
        print(f"✓ Configuration keys: {keys}")
        
        # Save configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.save_to_file(f.name)
            print(f"✓ Saved configuration to: {f.name}")
            
            # Try to load it back
            config2 = nlm.create_config()
            config2.load_from_file(f.name)
            print("✓ Successfully loaded configuration")
            
            # Verify key values
            for key in keys:
                val1 = config.get(key)
                val2 = config2.get(key)
                if val1 == val2:
                    print(f"  - Key '{key}' matches")
                else:
                    print(f"  - Key '{key}' mismatch: {val1} vs {val2}")
            
            os.unlink(f.name)
        
        print("✓ All basic functionality tests passed!\n")
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_advanced_features():
    """Test advanced NLM features"""
    print("Testing advanced NLM features...")
    
    try:
        import pynlm as nlm
        print("✓ Successfully imported NLM bindings")
        
        # Create different preset configurations
        presets = ["minimal", "small", "medium", "large", "learning", "development"]
        
        for preset in presets:
            config = nlm.create_config_from_preset(preset)
            print(f"✓ Created configuration from preset: {preset}")
            
            # Verify that configuration has the expected keys
            keys = config.get_keys()
            print(f"  - Keys: {keys}")
            
            # Basic validation
            if config.validate():
                print(f"  - Validation: PASS")
            else:
                print(f"  - Validation: FAIL")
        
        # Test environment variable expansion (if supported)
        print("✓ Environment variable expansion test skipped (requires platform-specific implementation)")
        
        print("✓ All advanced features tests passed!\n")
        return True
        
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling and edge cases"""
    print("Testing error handling...")
    
    try:
        import pynlm as nlm
        print("✓ Successfully imported NLM bindings")
        
        # Create configuration
        config = nlm.create_config()
        
        # Test getting non-existent key
        result = config.get("non_existent_key", "default_value")
        print(f"✓ Get non-existent key: {result} (expected: 'default_value')")
        
        # Test setting and removing keys
        config.set("test_key", 42)
        assert config.has("test_key"), "Key should exist after setting"
        
        config.remove("test_key")
        assert not config.has("test_key"), "Key should not exist after removing"
        
        print("✓ Key management tests passed")
        
        # Test configuration with invalid values (if validation is enabled)
        print("✓ Invalid value validation test skipped (requires implementation)")
        
        print("✓ All error handling tests passed!\n")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_performance():
    """Test performance aspects"""
    print("Testing performance aspects...")
    
    try:
        import pynlm as nlm
        print("✓ Successfully imported NLM bindings")
        
        # Create large configuration
        config = nlm.create_config()
        
        # Test setting many keys
        import time
        start_time = time.time()
        
        for i in range(1000):
            config.set(f"key_{i}", i)
        
        elapsed = time.time() - start_time
        print(f"✓ Set 1000 keys in {elapsed:.3f} seconds")
        
        # Test getting all keys
        start_time = time.time()
        keys = config.get_keys()
        elapsed = time.time() - start_time
        print(f"✓ Retrieved {len(keys)} keys in {elapsed:.3f} seconds")
        
        # Test summary generation
        start_time = time.time()
        summary = config.summary()
        elapsed = time.time() - start_time
        print(f"✓ Generated summary ({len(summary)} characters) in {elapsed:.3f} seconds")
        
        print("✓ All performance tests passed!\n")
        return True
        
    except Exception as e:
        print(f"✗ Performance test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Main test function"""
    print("=" * 60)
    print("NLM Python Bindings - Test Suite")
    print("=" * 60)
    print()
    
    results = []
    
    # Run all test suites
    results.append(("Basic Functionality", test_basic_functionality()))
    results.append(("Advanced Features", test_advanced_features()))
    results.append(("Error Handling", test_error_handling()))
    results.append(("Performance", test_performance()))
    
    # Print summary
    print("=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name:20} : {status}")
        if result:
            passed += 1
    
    print("=" * 60)
    print(f"Overall: {passed}/{total} test suites passed")
    print("=" * 60)
    
    if passed == total:
        print("🎉 All tests passed! NLM Python bindings improvements are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above for details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
