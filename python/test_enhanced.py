#!/usr/bin/env python3
"""
Simple test to verify the enhanced NLM bindings can be imported
and demonstrate basic functionality.
"""

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(__file__))

def test_import():
    """Test that enhanced bindings can be imported"""
    print("Testing import of enhanced NLM bindings...")
    
    try:
        from nlm_enhanced import *
        print("✓ Successfully imported nlm_enhanced")
        
        # Test basic imports
        from nlm_enhanced import EnhancedConfig, EnhancedBrain, EnhancedAgentBrain, NLMType
        print("✓ Successfully imported core classes")
        
        # Test exception types
        from nlm_enhanced import NLMException, ConfigurationError, BrainError, AgentError, WorldError
        print("✓ Successfully imported exception types")
        
        # Test utility functions
        from nlm_enhanced import py_to_nlm, nlm_to_py, validate_config, validate_brain
        print("✓ Successfully imported utility functions")
        
        print("\nAll imports successful!")
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        print("This is expected if the enhanced bindings haven't been built yet.")
        print("To build the enhanced bindings:")
        print("  1. mkdir build")
        print("  2. cd build")
        print("  3. cmake ..")
        print("  4. make -j4")
        return False

def test_basic_functionality():
    """Test basic functionality if imports were successful"""
    print("\nTesting basic functionality...")
    
    try:
        from nlm_enhanced import *
        
        # Test configuration
        config = createDefaultConfig()
        print(f"✓ Created default configuration: {config.toString()}")
        
        # Test setting values
        config.set("test_param", 42)
        config.set("test_string", "hello")
        config.set("test_float", 3.14)
        config.set("test_bool", True)
        
        print(f"✓ Set configuration values")
        
        # Test getting values
        assert config["test_param"] == 42
        assert config["test_string"] == "hello"
        assert config["test_float"] == 3.14
        assert config["test_bool"] is True
        
        print("✓ Configuration values correctly retrieved")
        
        # Test validation
        is_valid = config.validate()
        print(f"✓ Configuration validation: {'Valid' if is_valid else 'Invalid'}")
        
        # Test iteration
        count = 0
        for key, value in config.items():
            count += 1
        print(f"✓ Configuration has {count} entries")
        
        # Test NLMType
        nlm_value = NLMType(42)
        print(f"✓ Created NLMType value: {nlm_value}")
        
        if nlm_value.is_int():
            print(f"✓ NLMType correctly identified as int: {nlm_value.as_int()}")
        
        print("\nAll basic functionality tests passed!")
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Main test function"""
    print("Enhanced NLM Bindings Test")
    print("=" * 40)
    
    # Test import
    import_success = test_import()
    
    if import_success:
        # Test basic functionality
        functionality_success = test_basic_functionality()
        
        if functionality_success:
            print("\n" + "=" * 40)
            print("✓ All tests passed!")
            print("\nThe enhanced NLM bindings are working correctly.")
            return 0
        else:
            print("\n" + "=" * 40)
            print("✗ Basic functionality tests failed")
            return 1
    else:
        print("\n" + "=" * 40)
        print("✗ Import tests failed")
        print("Please build the enhanced bindings first:")
        print("  1. mkdir build")
        print("  2. cd build")
        print("  3. cmake ..")
        print("  4. make -j4")
        return 1

if __name__ == "__main__":
    sys.exit(main())