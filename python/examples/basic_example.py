#!/usr/bin/env python3
"""
Basic example script demonstrating NLM Python bindings.

This script demonstrates the improved NLM Python bindings with:
- Comprehensive exception handling
- Input validation
- Configuration management
- CLI interface
- Advanced Python features
"""

import sys
import os
sys.path.insert(0, os.path.dirname(__file__))

from nlm import (
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    NLMConfigurationError, NLMValidationError, NLMError,
    InputValidator, validate_brain_config, validate_world_parameters
)

def example_basic_usage():
    """Demonstrate basic NLM usage."""
    print("=== Basic NLM Usage Example ===")
    
    try:
        # Create configuration
        config = createDefaultConfig()
        print(f"✓ Created default configuration: {config.summary()}")
        
        # Create brain
        brain = createBrain(config)
        print("✓ Created brain")
        
        # Create world
        world = createSimpleWorld()
        print("✓ Created world")
        
        # Create agent brain
        agent_brain = createAgentBrain(brain)
        print("✓ Created agent brain")
        
        # Initialize systems
        brain.initialize()
        print("✓ Brain initialized")
        
        # Print brain statistics
        print(f"✓ Brain stats: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSynapseCount()} synapses")
        
        print("✓ Basic usage example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Basic usage example failed: {e}")
        return False
    
    return True

def example_configuration_management():
    """Demonstrate configuration management."""
    print("=== Configuration Management Example ===")
    
    try:
        from nlm import Config
        
        # Create configuration
        config = Config()
        
        # Set configuration values
        config.set('learning_rate', 0.01)
        config.set('neuron_count', 1000)
        config.set('world_width', 100.0)
        config.set('world_height', 100.0)
        
        # Validate configuration
        config_dict = {
            'learning_rate': 0.01,
            'neuron_count': 1000,
            'world_width': 100.0,
            'world_height': 100.0,
            'region_count': 5
        }
        
        config_errors = validate_brain_config(config_dict)
        if config_errors:
            print(f"✗ Configuration validation failed: {config_errors}")
            return False
        
        # Save configuration
        config.saveToFile("example_config.json")
        print("✓ Configuration saved to example_config.json")
        
        # Load configuration
        new_config = Config()
        new_config.loadFromFile("example_config.json")
        print(f"✓ Configuration loaded: {new_config.summary()}")
        
        # Clean up
        os.remove("example_config.json")
        
        print("✓ Configuration management example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Configuration management example failed: {e}")
        return False
    
    return True

def example_input_validation():
    """Demonstrate input validation."""
    print("=== Input Validation Example ===")
    
    try:
        from nlm.validation import InputValidator, validate_world_parameters
        
        # Create validator
        validator = InputValidator()
        
        # Validate world parameters
        world_params = {
            'width': 100.0,
            'height': 100.0,
            'vision_width': 20.0,
            'vision_height': 20.0
        }
        
        world_errors = validate_world_parameters(**world_params)
        if world_errors:
            print(f"✗ World parameters validation failed: {world_errors}")
            return False
        
        print("✓ World parameters validation passed")
        
        # Validate individual values
        validation_results = validator.validate_all({
            'neuron_count': 1000,
            'learning_rate': 0.01,
            'timestep': 0.01,
            'temperature': 1.0
        })
        
        # Check for errors
        has_errors = any(validation_results.values())
        if has_errors:
            print(f"✗ Input validation failed: {validation_results}")
            return False
        
        print("✓ Input validation passed")
        
        # Demonstrate error recovery
        from nlm.validation import ErrorRecovery
        recovery = ErrorRecovery()
        
        # Try operation that might fail
        def risky_operation():
            raise ValueError("Simulated error")
        
        try:
            result = recovery.try_with_recovery(risky_operation, max_attempts=3)
        except Exception:
            # This is expected in this simple example
            pass
        
        print("✓ Error recovery mechanism demonstrated")
        
        print("✓ Input validation example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Input validation example failed: {e}")
        return False
    
    return True

def example_exception_handling():
    """Demonstrate exception handling."""
    print("=== Exception Handling Example ===")
    
    try:
        from nlm.exceptions import (
            NLMError, NLMConfigurationError, NLMValidationError,
            NLMMemoryError, NLMTypeError, raise_config_error,
            raise_validation_error, create_exception_from_cpp
        )
        
        # Demonstrate different exception types
        try:
            raise_config_error("Invalid configuration file path")
        except NLMConfigurationError as e:
            print(f"✓ Caught NLMConfigurationError: {e}")
            print(f"  - Error code: {e.error_code}")
            print(f"  - Context: {e.context}")
        
        try:
            raise_validation_error("Invalid input value", field="learning_rate", value=2.0)
        except NLMValidationError as e:
            print(f"✓ Caught NLMValidationError: {e}")
            print(f"  - Field: {e.field}")
            print(f"  - Value: {e.value}")
            print(f"  - Constraints: {e.constraints}")
        
        # Demonstrate exception conversion
        cpp_exception = ValueError("C++ style exception")
        py_exception = create_exception_from_cpp(cpp_exception, {"operation": "test"})
        print(f"✓ Converted C++ exception: {type(py_exception).__name__}: {py_exception}")
        
        print("✓ Exception handling example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Exception handling example failed: {e}")
        return False
    
    return True

def example_advanced_features():
    """Demonstrate advanced Python features."""
    print("=== Advanced Features Example ===")
    
    try:
        # Demonstrate module-level constants and configuration
        from nlm import (
            NLM_PERFORMANCE_MODE, NLM_PARALLEL_PROCESSING, NLM_MEMORY_POOLING,
            DEFAULT_SIMULATION_DT, DEFAULT_MAX_STEP_COUNT, DEFAULT_CHECKPOINT_INTERVAL
        )
        
        print(f"✓ Performance mode: {NLM_PERFORMANCE_MODE}")
        print(f"✓ Parallel processing: {NLM_PARALLEL_PROCESSING}")
        print(f"✓ Memory pooling: {NLM_MEMORY_POOLING}")
        print(f"✓ Default simulation dt: {DEFAULT_SIMULATION_DT}")
        print(f"✓ Default max steps: {DEFAULT_MAX_STEP_COUNT}")
        print(f"✓ Default checkpoint interval: {DEFAULT_CHECKPOINT_INTERVAL}")
        
        # Demonstrate submodules
        from nlm import config, brain, world, agent, sensory
        
        print("✓ Submodules accessible:")
        print(f"  - Config module: {config}")
        print(f"  - Brain module: {brain}")
        print(f"  - World module: {world}")
        print(f"  - Agent module: {agent}")
        print(f"  - Sensory module: {sensory}")
        
        # Demonstrate performance settings
        print("✓ Performance settings:")
        print(f"  - Log level: {config.DEFAULT_LOG_LEVEL}")
        print(f"  - Error recovery: {config.DEFAULT_ERROR_RECOVERY}")
        print(f"  - Validation strict: {config.DEFAULT_VALIDATION_STRICT}")
        
        print("✓ Advanced features example completed successfully\n")
        
    except Exception as e:
        print(f"✗ Advanced features example failed: {e}")
        return False
    
    return True

def main():
    """Run all examples."""
    print("NLM Python Bindings Examples")
    print("=" * 50)
    
    examples = [
        ("Basic Usage", example_basic_usage),
        ("Configuration Management", example_configuration_management),
        ("Input Validation", example_input_validation),
        ("Exception Handling", example_exception_handling),
        ("Advanced Features", example_advanced_features),
    ]
    
    passed = 0
    failed = 0
    
    for name, example_func in examples:
        print(f"\nRunning: {name}")
        print("-" * 30)
        
        try:
            if example_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Example failed with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Summary: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("✓ All examples completed successfully!")
        return 0
    else:
        print("✗ Some examples failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())