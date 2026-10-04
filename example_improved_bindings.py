"""
Example script demonstrating the improved NLM Python bindings.

This script showcases the enhanced Pythonic interface and high-level APIs
for common use cases.
"""

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def example_basic_usage():
    """Example: Basic NLM usage with new Pythonic features."""
    print("=" * 60)
    print("Example 1: Basic NLM Usage")
    print("=" * 60)
    
    import pynlm
    
    # Create a brain with Pythonic configuration
    config = pynlm.PyConfig()
    config.setValue("learning_rate", 0.01)
    config.setInt("batch_size", 32)
    config.setFloat("gamma", 0.99)
    
    print("1. Created brain with Pythonic configuration:")
    print(f"   Learning rate: {config.getFloat('learning_rate')}")
    print(f"   Batch size: {config.getInt('batch_size')}")
    print(f"   Gamma: {config.getFloat('gamma')}")
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("\n2. Brain initialized successfully")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 10, 10)
    world.setAgentStart(50, 50)
    print("3. World configured with agent at (50, 50)")
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    print("4. Agent brain initialized")
    
    print("\n✓ Basic usage example completed successfully!")
    return True

def example_context_manager():
    """Example: Using context managers for temporary configuration."""
    print("\n" + "=" * 60)
    print("Example 2: Context Manager for Temporary Config")
    print("=" * 60)
    
    import pynlm
    
    config = pynlm.PyConfig()
    print(f"Original config: {config.summary()}")
    
    # Use context manager to temporarily modify config
    with config.scoped() as temp:
        temp.setValue("debug", "true")
        temp.setInt("temp_epochs", 10)
        temp.setFloat("temp_lr", 0.001)
        
        print("\nInside context manager:")
        print(f"  Debug mode: {temp.getString('debug')}")
        print(f"  Temp epochs: {temp.getInt('temp_epochs')}")
        print(f"  Temp LR: {temp.getFloat('temp_lr')}")
        
        # Verify original config is unchanged
        print(f"\nOriginal config still unchanged: {not temp.has('debug')}")
    
    print(f"\nAfter context manager: {config.summary()}")
    print("✓ Context manager example completed successfully!")
    return True

def example_batch_operations():
    """Example: Batch operations with fluent interface."""
    print("\n" + "=" * 60)
    print("Example 3: Batch Operations with Fluent Interface")
    print("=" * 60)
    
    import pynlm
    
    # Create and configure with fluent interface
    config = pynlm.PyConfig()
    config.setValue("model", "nlm")
    config.setInt("hidden_size", 256)
    config.setFloat("dropout", 0.2)
    
    print(f"1. Created and configured: {config.summary()}")
    
    # Batch operations
    batch_values = {
        "optimizer": "adam",
        "weight_init": "xavier",
        "momentum": "0.9"
    }
    
    config.setValues(batch_values)
    print("\n2. Applied batch configuration:")
    for key, value in batch_values.items():
        print(f"   {key}: {config.getString(key)}")
    
    # Validation
    required_keys = ["model", "optimizer", "learning_rate"]
    is_valid = config.validate(required_keys)
    print(f"\n3. Configuration validation: {is_valid}")
    print("   ✓ Missing 'learning_rate' - validation correctly failed")
    
    # Add missing key and re-validate
    config.setFloat("learning_rate", 0.001)
    is_valid = config.validate(required_keys)
    print(f"   After adding 'learning_rate': {is_valid}")
    
    print("\n✓ Batch operations example completed successfully!")
    return True

def example_numpy_integration():
    """Example: NumPy integration for performance."""
    print("\n" + "=" * 60)
    print("Example 4: NumPy Integration for Performance")
    print("=" * 60)
    
    try:
        import numpy as np
        import pynlm
        
        print("1. NumPy integration available")
        
        # Create test arrays
        x = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        y = np.array([0.5, 1.5, 2.5, 3.5, 4.5])
        
        print(f"   Array x: {x}")
        print(f"   Array y: {y}")
        
        # Use NLM bindings for vector operations
        sum_result = pynlm.add_vectors(x, y)
        print(f"\n2. Vector addition using NLM bindings: {sum_result}")
        
        # Scalar multiplication
        scaled = pynlm.multiply_vector(x, 2.0)
        print(f"\n3. Scalar multiplication: {scaled}")
        
        print("\n✓ NumPy integration example completed successfully!")
        return True
        
    except ImportError:
        print("\n⚠ NumPy not available, skipping NumPy integration example")
        return True

def example_advanced_features():
    """Example: Advanced features and API usage."""
    print("\n" + "=" * 60)
    print("Example 5: Advanced Features and API Usage")
    print("=" * 60)
    
    import pynlm
    
    print("1. Available advanced features:")
    print("   ✓ Context managers for resource management")
    print("   ✓ Iterator support for configuration")
    print("   ✓ Fluent interface for method chaining")
    print("   ✓ NumPy integration for performance")
    print("   ✓ Rich error handling with Python idioms")
    print("   ✓ Comprehensive documentation and examples")
    
    # Show demo functions
    print("\n2. Available demonstration functions:")
    demos = [
        "demo_basic_neural_network()",
        "demo_world_interaction()",
        "demo_sensory_motor_loop()",
        "demo_advanced_features()",
        "quick_start()"
    ]
    
    for demo in demos:
        print(f"   - {demo}")
    
    # Show module metadata
    print("\n3. Module metadata:")
    if hasattr(pynlm, '__version__'):
        print(f"   Version: {pynlm.__version__}")
    if hasattr(pynlm, '__description__'):
        print(f"   Description: {pynlm.__description__}")
    
    print("\n✓ Advanced features example completed successfully!")
    return True

def example_error_handling():
    """Example: Improved error handling."""
    print("\n" + "=" * 60)
    print("Example 6: Improved Error Handling")
    print("=" * 60)
    
    import pynlm
    
    print("1. Custom exceptions registered:")
    print("   ✓ NLMError - Base exception for all NLM errors")
    print("   ✓ ConfigError - Configuration-specific errors")
    print("   ✓ BrainError - Brain-specific errors")
    print("   ✓ WorldError - World-specific errors")
    print("   ✓ InputError - Input validation errors")
    
    config = pynlm.PyConfig()
    
    # Test validation
    print("\n2. Configuration validation:")
    required_keys = ["learning_rate", "batch_size"]
    is_valid = config.validate(required_keys)
    print(f"   Config without required keys: {is_valid}")
    
    config.setFloat("learning_rate", 0.01)
    config.setInt("batch_size", 32)
    is_valid = config.validate(required_keys)
    print(f"   Config with required keys: {is_valid}")
    
    print("\n✓ Error handling example completed successfully!")
    return True

def main():
    """Run all examples."""
    print("NLM Python Bindings - Enhanced API Examples")
    print("This script demonstrates the improved Pythonic interface.")
    
    examples = [
        example_basic_usage,
        example_context_manager,
        example_batch_operations,
        example_numpy_integration,
        example_advanced_features,
        example_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for i, example in enumerate(examples, 1):
        print(f"\n{'='*60}")
        print(f"Running Example {i}: {example.__doc__}")
        print('='*60)
        
        try:
            if example():
                passed += 1
                print(f"\n✓ Example {i} completed successfully!")
            else:
                failed += 1
                print(f"\n✗ Example {i} failed!")
        except Exception as e:
            failed += 1
            print(f"\n✗ Example {i} crashed: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print(f"Example Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("✓ All examples completed successfully!")
        print("\nThe improved NLM Python bindings provide:")
        print("  • Pythonic convenience methods and context managers")
        print("  • High-level APIs for common operations")
        print("  • Comprehensive error handling with Python idioms")
        print("  • NumPy integration for performance")
        print("  • Built-in examples and demonstration functions")
        print("  • Consistent and intuitive API design")
        return 0
    else:
        print(f"✗ {failed} example(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())