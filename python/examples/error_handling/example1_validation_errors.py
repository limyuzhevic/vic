# Error Handling Example
"""
Example demonstrating enhanced error handling and validation.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Error Handling Example ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test different types of error handling
    print("1. Testing parameter validation:")
    
    # Test invalid config parameter
    try:
        # This should trigger validation error
        brain.set_config_value("invalid_key", "value")
        print("   WARNING: Should have raised validation error")
    except pynlm.pynlm.ValidationError as e:
        print(f"   ✓ Caught ValidationError: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    # Test type validation
    try:
        # Try to use invalid type
        result = brain.step("invalid_step_number")
        print("   WARNING: Should have raised type error")
    except pynlm.pynlm.TypeError as e:
        print(f"   ✓ Caught TypeError: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    # Test range validation
    print("\n2. Testing range validation:")
    
    # Test negative timestep
    try:
        world = pynlm.createSimpleWorld()
        world.update(-0.1)  # Should trigger range error
        print("   WARNING: Should have raised range error")
    except pynlm.pynlm.ValidationError as e:
        print(f"   ✓ Caught range validation error: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    # Test state validation
    print("\n3. Testing state validation:")
    
    # Try to use brain before initialization
    uninitialized_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    try:
        uninitialized_brain.step(0)
        print("   WARNING: Should have raised state error")
    except pynlm.pynlm.StateError as e:
        print(f"   ✓ Caught StateError: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    # Test configuration validation
    print("\n4. Testing configuration validation:")
    
    # Create config with enhanced validation
    config = pynlm.EnhancedConfig()
    
    try:
        # Test valid configuration
        config.set_value("brain.neuron_count", 1000)
        config.set_value("brain.synapse_density", 0.1)
        config.validate("brain.neuron_count", "int")
        config.validate("brain.synapse_density", "float")
        print("   ✓ Configuration validation passed")
    except pynlm.pynlm.ValidationError as e:
        print(f"   ✗ Validation error: {e}")
    
    try:
        # Test invalid configuration
        config.validate("brain.neuron_count", "string")
        print("   WARNING: Should have raised type error")
    except pynlm.pynlm.ValidationError as e:
        print(f"   ✓ Caught type validation error: {e}")
    
    print("\n=== Error Handling Example Complete ===")

if __name__ == "__main__":
    main()
