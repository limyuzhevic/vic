#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python Bindings

This script demonstrates the key features of the enhanced NLM Python bindings,
including type safety, error handling, and Pythonic interfaces.
"""

import sys
import os

# Add the build directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

try:
    from nlm_enhanced import *
    print("Successfully imported nlm_enhanced bindings")
except ImportError as e:
    print(f"Error importing nlm_enhanced: {e}")
    print("Make sure to build the enhanced bindings first:")
    print("  mkdir build")
    print("  cd build")
    print("  cmake ..")
    print("  make -j4")
    sys.exit(1)

def demonstrate_basic_functionality():
    """Demonstrate basic functionality of enhanced bindings"""
    print("\n=== Basic Functionality Demo ===")
    
    # Create enhanced configuration
    print("\n1. Creating enhanced configuration...")
    config = createDefaultConfig()
    print(f"   Created configuration: {config.toString()}")
    
    # Set configuration values with automatic type conversion
    print("\n2. Setting configuration values...")
    config.set("learning_rate", 0.01)
    config.set("batch_size", 32)
    config.set("use_gpu", True)
    config.set("model_name", "neural_network")
    
    # Get values with automatic type conversion
    print("\n3. Getting configuration values...")
    print(f"   Learning rate: {config['learning_rate']} (type: {type(config['learning_rate']).__name__})")
    print(f"   Batch size: {config['batch_size']} (type: {type(config['batch_size']).__name__})")
    print(f"   Use GPU: {config['use_gpu']} (type: {type(config['use_gpu']).__name__})")
    print(f"   Model name: {config['model_name']} (type: {type(config['model_name']).__name__})")
    
    # Demonstrate iteration
    print("\n4. Iterating over configuration...")
    for key, value in config.items():
        print(f"   {key}: {value}")
    
    # Demonstrate validation
    print("\n5. Validating configuration...")
    if config.validate():
        print("   Configuration is valid!")
    else:
        print("   Configuration is invalid!")
    
    return config

def demonstrate_error_handling():
    """Demonstrate enhanced error handling"""
    print("\n=== Error Handling Demo ===")
    
    # Create configuration with validation issues
    print("\n1. Testing configuration validation...")
    config = createDefaultConfig()
    config.set("invalid_param", "value")
    
    if not config.validate():
        print("   Configuration validation failed (as expected)")
    
    # Demonstrate exception handling
    print("\n2. Testing exception handling...")
    try:
        # This should raise an exception
        value = config["invalid_param"]
        print(f"   Got value: {value}")
    except ConfigurationError as e:
        print(f"   Caught ConfigurationError: {e}")
        print(f"   Error type: {e.type()}")
        print(f"   Error details: {get_error_details(e)}")
    
    # Demonstrate type conversion errors
    print("\n3. Testing type conversion...")
    try:
        # Try to access invalid key
        value = config["nonexistent_key"]
        print(f"   Got value: {value}")
    except ConfigurationError as e:
        print(f"   Caught ConfigurationError: {e}")
    
    return config

def demonstrate_enhanced_brain():
    """Demonstrate enhanced brain functionality"""
    print("\n=== Enhanced Brain Demo ===")
    
    # Create enhanced brain
    print("\n1. Creating enhanced brain...")
    config = createDefaultConfig()
    brain = createBrain(config)
    print(f"   Created brain: {brain.toString()}")
    
    # Initialize brain with error handling
    print("\n2. Initializing brain...")
    try:
        brain.initialize()
        print("   Brain initialized successfully")
    except BrainError as e:
        print(f"   Error initializing brain: {e}")
        return None
    
    # Demonstrate simulation with error handling
    print("\n3. Running simulation...")
    for step in range(10):
        time = step * 0.01
        try:
            brain.step(step, time)
            if (step + 1) % 5 == 0:
                print(f"   Step {step + 1}/10 completed")
        except BrainError as e:
            print(f"   Error at step {step}: {e}")
            break
    
    # Get brain statistics
    print("\n4. Getting brain statistics...")
    try:
        stats = brain.getStatistics()
        print("   Brain statistics:")
        for key, value in stats.items():
            print(f"     {key}: {value}")
    except BrainError as e:
        print(f"   Error getting statistics: {e}")
    
    return brain

def demonstrate_enhanced_agent_brain():
    """Demonstrate enhanced agent brain functionality"""
    print("\n=== Enhanced Agent Brain Demo ===")
    
    # Create enhanced agent brain
    print("\n1. Creating enhanced agent brain...")
    config = createDefaultConfig()
    brain = createBrain(config)
    agent_brain = createAgentBrain(brain)
    print(f"   Created agent brain")
    
    # Demonstrate feature control
    print("\n2. Testing feature control...")
    agent_brain.enableFeature("curiosity")
    agent_brain.disableFeature("structural_plasticity")
    
    feature_status = agent_brain.getFeatureStatus()
    print("   Feature status:")
    for feature, enabled in feature_status.items():
        print(f"     {feature}: {enabled}")
    
    # Demonstrate neuromodulation
    print("\n3. Testing neuromodulation...")
    agent_brain.applyRewardModulation(reward=1.0, predicted_reward=0.8)
    print("   Reward neuromodulation applied")
    
    # Demonstrate development update
    print("\n4. Testing development update...")
    agent_brain.updateDevelopment(timestep=0.1)
    print("   Development updated")
    
    return agent_brain

def demonstrate_type_system():
    """Demonstrate enhanced type system"""
    print("\n=== Type System Demo ===")
    
    # Demonstrate NLMType usage
    print("\n1. Testing NLMType...")
    
    # Create NLMType values
    null_value = NLMType()
    int_value = NLMType(42)
    float_value = NLMType(3.14)
    string_value = NLMType("hello")
    bool_value = NLMType(True)
    
    print(f"   Null value: {null_value} (is null: {null_value.is_null()})")
    print(f"   Int value: {int_value} (type: {int_value.type()})")
    print(f"   Float value: {float_value} (type: {float_value.type()})")
    print(f"   String value: {string_value} (type: {string_value.type()})")
    print(f"   Bool value: {bool_value} (type: {bool_value.type()})")
    
    # Demonstrate type conversion
    print("\n2. Testing type conversion...")
    try:
        converted_int = int_value.as_int()
        print(f"   Converted int: {converted_int}")
    except ConfigurationError as e:
        print(f"   Error converting int: {e}")
    
    try:
        converted_float = int_value.as_float()
        print(f"   Converted float: {converted_float}")
    except ConfigurationError as e:
        print(f"   Error converting float: {e}")
    
    # Demonstrate type checking
    print("\n3. Testing type checking...")
    if int_value.is_int():
        print(f"   int_value is int: {int_value.as_int()}")
    
    if float_value.is_float():
        print(f"   float_value is float: {float_value.as_float()}")
    
    if string_value.is_string():
        print(f"   string_value is string: {string_value.as_string()}")
    
    return [null_value, int_value, float_value, string_value, bool_value]

def demonstrate_utility_functions():
    """Demonstrate utility functions"""
    print("\n=== Utility Functions Demo ===")
    
    # Demonstrate type conversion utilities
    print("\n1. Testing type conversion utilities...")
    
    # Convert Python to NLMType
    py_list = [1, 2, 3, 4, 5]
    nlm_list = py_to_nlm(py_list)
    print(f"   Python list: {py_list}")
    print(f"   Converted NLMType: {nlm_list}")
    
    # Convert NLMType to Python
    back_to_py = nlm_to_py(nlm_list)
    print(f"   Back to Python: {back_to_py}")
    
    # Demonstrate validation utilities
    print("\n2. Testing validation utilities...")
    
    # Validate configuration
    config = createDefaultConfig()
    config.set("learning_rate", 0.01)
    config.set("batch_size", 32)
    
    if validate_config(config):
        print("   Configuration validation: OK")
    else:
        print("   Configuration validation: FAILED")
    
    # Demonstrate string utilities
    print("\n3. Testing string utilities...")
    
    test_string = "  HELLO WORLD  "
    trimmed = trim(test_string)
    print(f"   Original: '{test_string}'")
    print(f"   Trimmed: '{trimmed}'")
    
    print(f"   Starts with 'HELLO': {starts_with(trimmed, 'HELLO')}")
    print(f"   To lowercase: {to_lower(trimmed)}")
    
    # Demonstrate statistical utilities
    print("\n4. Testing statistical utilities...")
    
    values = [1.0, 2.0, 3.0, 4.0, 5.0]
    mean_val = mean(values)
    stddev_val = stddev(values)
    median_val = median(values)
    
    print(f"   Values: {values}")
    print(f"   Mean: {mean_val}")
    print(f"   Standard deviation: {stddev_val}")
    print(f"   Median: {median_val}")
    
    # Demonstrate file utilities
    print("\n5. Testing file utilities...")
    
    # Note: These utilities check the current environment
    current_dir = get_current_directory()
    print(f"   Current directory: {current_dir}")
    
    # Demonstrate error utilities
    print("\n6. Testing error utilities...")
    
    # Create a sample error
    try:
        raise ConfigurationError("Test configuration error")
    except ConfigurationError as e:
        error_type = get_error_type(e)
        error_details = get_error_details(e)
        print(f"   Error type: {error_type}")
        print(f"   Error details: {error_details}")
        
        # Log the error
        log_error(e)
        print(f"   Error logged successfully")
    
    return [py_list, config, values, current_dir, error_details]

def main():
    """Main demonstration function"""
    print("Enhanced NLM Python Bindings Demonstration")
    print("=" * 50)
    
    try:
        # Demonstrate basic functionality
        config = demonstrate_basic_functionality()
        
        # Demonstrate error handling
        demonstrate_error_handling()
        
    # Demonstrate enhanced brain
    brain = demonstrate_enhanced_brain()
    if brain is not None:
        print("\n   Brain simulation completed successfully!")
        
        # Demonstrate enhanced agent brain
        agent_brain = demonstrate_enhanced_agent_brain()
        
        # Demonstrate type system
        type_values = demonstrate_type_system()
        
        # Demonstrate utility functions
        demonstrate_utility_functions()
        
        print("\n" + "=" * 50)
        print("All demonstrations completed successfully!")
        print("\nEnhanced NLM Python bindings provide:")
        print("  ✓ Better type safety with automatic type conversion")
        print("  ✓ Enhanced error handling with custom exceptions")
        print("  ✓ Pythonic interfaces with context managers")
        print("  ✓ Comprehensive validation and parameter checking")
        print("  ✓ Convenience methods and utility functions")
        print("  ✓ Backward compatibility with existing pynlm")
        
    except Exception as e:\n        print(f"\nError during demonstration: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())