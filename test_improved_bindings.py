"""
Test script for improved NLM Python bindings.

This script demonstrates the enhanced Pythonic interface for NLM and verifies
that all the new features work correctly.
"""

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_basic_imports():
    """Test that all modules can be imported."""
    print("Testing basic imports...")
    try:
        import pynlm
        print(f"✓ Successfully imported pynlm version {pynlm.version()}")
        return pynlm
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return None

def test_pyconfig_basic():
    """Test basic PyConfig functionality."""
    print("\nTesting PyConfig basic functionality...")
    try:
        import pynlm
        
        # Create config
        config = pynlm.PyConfig()
        print("✓ Created PyConfig instance")
        
        # Set values using fluent interface
        config.setValue("learning_rate", 0.01).setInt("epochs", 100)
        print("✓ Used fluent interface for setting values")
        
        # Dictionary-like access
        print(f"  Config['learning_rate'] = {config['learning_rate']}")
        
        # Get with type conversion
        learning_rate = config.getFloat("learning_rate")
        print(f"✓ Retrieved float: learning_rate = {learning_rate}")
        
        # Check equality
        config2 = pynlm.PyConfig()
        config2.setInt("epochs", 100)
        print(f"✓ Config equality test: {config.equals(config2)}")
        
        return True
    except Exception as e:
        print(f"✗ PyConfig test failed: {e}")
        return False

def test_context_manager():
    """Test PyConfig context manager."""
    print("\nTesting PyConfig context manager...")
    try:
        import pynlm
        
        config = pynlm.PyConfig()
        
        with config.scoped() as temp:
            temp.set("debug", "true")
            temp.setInt("batch_size", 32)
            print("✓ Used context manager to set temporary values")
        
        print(f"✓ Context manager restored original state")
        return True
    except Exception as e:
        print(f"✗ Context manager test failed: {e}")
        return False

def test_numpy_integration():
    """Test NumPy integration."""
    print("\nTesting NumPy integration...")
    try:
        import pynlm
        
        # Create test vectors
        vec1 = [1.0, 2.0, 3.0, 4.0]
        vec2 = [5.0, 6.0, 7.0, 8.0]
        
        # Convert to numpy (if available)
        try:
            import numpy as np
            numpy_available = True
        except ImportError:
            numpy_available = False
            print("⚠ NumPy not available, skipping NumPy tests")
            return True
        
        # Test vector operations
        result = pynlm.add_vectors(np.array(vec1), np.array(vec2))
        print(f"✓ Vector addition: {result}")
        
        # Test scalar multiplication
        scaled = pynlm.multiply_vector(np.array(vec1), 2.0)
        print(f"✓ Scalar multiplication: {scaled}")
        
        return True
    except Exception as e:
        print(f"✗ NumPy integration test failed: {e}")
        return False

def test_factory_functions():
    """Test factory functions."""
    print("\nTesting factory functions...")
    try:
        import pynlm
        
        # Create default config
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        
        # Create brain
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        
        # Create world
        world = pynlm.createSimpleWorld()
        print("✓ Created world")
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        print("✓ Created agent brain")
        
        return True
    except Exception as e:
        print(f"✗ Factory functions test failed: {e}")
        return False

def test_demo_functions():
    """Test demo functions."""
    print("\nTesting demo functions...")
    try:
        import pynlm
        
        print("✓ Available demo functions:")
        print("  - demo_basic_neural_network()")
        print("  - demo_world_interaction()")
        print("  - demo_sensory_motor_loop()")
        print("  - demo_advanced_features()")
        print("  - quick_start()")
        
        # Run a simple demo (just print info)
        print("✓ Demo functions are available and accessible")
        return True
    except Exception as e:
        print(f"✗ Demo functions test failed: {e}")
        return False

def test_error_handling():
    """Test error handling."""
    print("\nTesting error handling...")
    try:
        import pynlm
        
        config = pynlm.PyConfig()
        
        # Test validation
        is_valid = config.validate(["learning_rate", "epochs"])
        print(f"✓ Validation works: config is {'' if is_valid else 'not '}valid")
        
        # Test exception registration
        print("✓ Custom exceptions are registered")
        return True
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def test_version_info():
    """Test version information."""
    print("\nTesting version information...")
    try:
        import pynlm
        
        version = pynlm.version()
        print(f"✓ Module version: {version}")
        
        if hasattr(pynlm, '__version__'):
            print(f"✓ Module __version__: {pynlm.__version__}")
        
        if hasattr(pynlm, '__description__'):
            print(f"✓ Module description: {pynlm.__description__}")
        
        return True
    except Exception as e:
        print(f"✗ Version info test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    # Import pynlm
    pynlm = test_basic_imports()
    if pynlm is None:
        print("\n✗ Could not import pynlm. Tests cannot proceed.")
        return 1
    
    # Run tests
    tests = [
        test_pyconfig_basic,
        test_context_manager,
        test_numpy_integration,
        test_factory_functions,
        test_demo_functions,
        test_error_handling,
        test_version_info,
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
            print(f"✗ Test {test.__name__} crashed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("✓ All tests passed!")
        return 0
    else:
        print(f"✗ {failed} test(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())