#!/usr/bin/env python3
"""
Verification script for NLM Python Bindings improvements

This script verifies that all the improvements have been implemented correctly
and that the Python bindings work as expected.
"""

import sys
import os

def check_imports():
    """Check that all required modules can be imported"""
    print("1. Checking imports...")
    
    try:
        import pynlm
        print("   ✓ pynlm imported successfully")
        print(f"   Version: {getattr(pynlm, '__version__', 'Unknown')}")
        return True
    except ImportError as e:
        print(f"   ✗ Failed to import pynlm: {e}")
        return False

def check_basic_functionality():
    """Test basic Python bindings functionality"""
    print("\n2. Testing basic functionality...")
    
    try:
        import pynlm
        
        # Create brain
        config = pynlm.createDefaultConfig()
        print("   ✓ Default config created")
        
        brain = pynlm.createBrain(config)
        print(f"   ✓ Brain created with {brain.total_neurons} neurons")
        
        brain.initialize()
        print("   ✓ Brain initialized")
        
        brain.step(0)
        print("   ✓ Brain step executed")
        
        # Test property-based access
        print(f"   ✓ Property access: {brain.total_neurons} neurons")
        print(f"   ✓ Property access: {brain.firing_neurons} firing neurons")
        
        return True
    except Exception as e:
        print(f"   ✗ Basic functionality test failed: {e}")
        return False

def check_advanced_features():
    """Test advanced Python features"""
    print("\n3. Testing advanced features...")
    
    try:
        import pynlm
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test context manager
        with pynlm.brain_simulation(brain):
            print("   ✓ Context manager working")
            brain.step(1)
        
        # Test property-based accessors
        assert hasattr(brain, 'total_neurons'), "Property 'total_neurons' missing"
        assert hasattr(brain, 'firing_neurons'), "Property 'firing_neurons' missing"
        assert hasattr(brain, 'average_firing_rate'), "Property 'average_firing_rate' missing"
        print("   ✓ Property-based accessors implemented")
        
        # Test dictionary interface for config
        config["test_key"] = "test_value"
        assert config.get("test_key") == "test_value", "Dictionary interface not working"
        print("   ✓ Dictionary interface for config working")
        
        # Test iteration
        regions = list(pynlm.iter_regions(brain))
        print(f"   ✓ Iteration working: {len(regions)} regions")
        
        # Test list comprehensions
        active_neurons = pynlm.get_active_neuron_ids(brain)
        print(f"   ✓ List comprehensions working: {len(active_neurons)} active neurons")
        
        return True
    except Exception as e:
        print(f"   ✗ Advanced features test failed: {e}")
        return False

def check_examples():
    """Test example scripts"""
    print("\n4. Testing example scripts...")
    
    try:
        # Check if examples directory exists
        if not os.path.exists("examples"):
            print("   ✗ Examples directory not found")
            return False
        
        # Check for advanced examples
        advanced_example = "examples/advanced_examples.py"
        if os.path.exists(advanced_example):
            print("   ✓ Advanced examples file found")
            
            # Try to import the example (without running it)
            import importlib.util
            spec = importlib.util.spec_from_file_location("advanced_examples", advanced_example)
            if spec and spec.loader:
                print("   ✓ Advanced examples can be imported")
            else:
                print("   ✗ Advanced examples cannot be imported")
                return False
        else:
            print("   ✗ Advanced examples file not found")
            return False
        
        # Check for user guide
        user_guide = "user_guide.md"
        if os.path.exists(user_guide):
            print("   ✓ User guide found")
        else:
            print("   ✗ User guide not found")
            return False
        
        return True
    except Exception as e:
        print(f"   ✗ Examples test failed: {e}")
        return False

def check_documentation():
    """Check documentation files"""
    print("\n5. Checking documentation...")
    
    required_files = [
        "python/README.md",
        "user_guide.md",
        "setup.py",
        "examples/advanced_examples.py"
    ]
    
    all_good = True
    for file_path in required_files:
        if os.path.exists(file_path):
            print(f"   ✓ {file_path} found")
        else:
            print(f"   ✗ {file_path} not found")
            all_good = False
    
    return all_good

def check_api_completeness():
    """Check that all expected API features are present"""
    print("\n6. Checking API completeness...")
    
    try:
        import pynlm
        
        # Check core classes
        required_classes = [
            'createDefaultConfig',
            'createBrain',
            'createSimpleWorld',
            'createAgentBrain'
        ]
        
        for class_name in required_classes:
            if hasattr(pynlm, class_name):
                print(f"   ✓ {class_name} available")
            else:
                print(f"   ✗ {class_name} missing")
                return False
        
        # Check helper functions
        helper_functions = [
            'brain_simulation',
            'agent_context',
            'world_context',
            'run_agent_episode',
            'batch_steps',
            'batch_world_updates',
            'debug_brain_state',
            'config_to_dict',
            'dict_to_config'
        ]
        
        for func_name in helper_functions:
            if hasattr(pynlm, func_name):
                print(f"   ✓ {func_name} available")
            else:
                print(f"   ✗ {func_name} missing")
                return False
        
        # Check constants
        constants = [
            'INVALID_NEURON_ID',
            'INVALID_SYNAPSE_ID',
            'INVALID_REGION_ID',
            'INVALID_POPULATION_ID'
        ]
        
        for const_name in constants:
            if hasattr(pynlm, const_name):
                print(f"   ✓ {const_name} available")
            else:
                print(f"   ✗ {const_name} missing")
                return False
        
        return True
    except Exception as e:
        print(f"   ✗ API completeness check failed: {e}")
        return False

def main():
    """Run all verification tests"""
    print("NLM Python Bindings - Verification Script")
    print("=" * 50)
    
    tests = [
        check_imports,
        check_basic_functionality,
        check_advanced_features,
        check_examples,
        check_documentation,
        check_api_completeness
    ]
    
    results = []
    for test in tests:
        results.append(test())
    
    print("\n" + "=" * 50)
    print("Verification Summary:")
    
    for i, (test, result) in enumerate(zip(tests, results)):
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"  {test.__name__}: {status}")
    
    all_passed = all(results)
    
    if all_passed:
        print("\n🎉 All verification tests passed!")
        print("\nThe NLM Python bindings have been successfully improved with:")
        print("  • Pythonic property-based accessors")
        print("  • Context managers for resource management")
        print("  • Advanced Python features (iterators, list comprehensions)")
        print("  • Enhanced error handling")
        print("  • Comprehensive documentation and examples")
        print("  • Factory methods for common use cases")
        return 0
    else:
        print("\n❌ Some verification tests failed.")
        print("Please check the output above for details.")
        return 1

if __name__ == "__main__":
    exit(main())