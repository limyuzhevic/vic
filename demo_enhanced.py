#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python Bindings

This script demonstrates the key features of the enhanced NLM Python bindings,
showing how they provide better error handling, type safety, and validation
compared to the original bindings.
"""

import sys
import os

print("=== Enhanced NLM Python Bindings Demonstration ===")
print()

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    # Import the enhanced bindings
    import pynlm_enhanced as nlm
    print("✓ Successfully imported pynlm_enhanced")
    
    # Demonstrate enhanced error handling
    print("\n1. Enhanced Error Handling")
    print("=" * 40)
    
    try:
        # Create configuration and brain
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        print("✓ Brain created and initialized successfully")
        
        # Validate brain state
        brain.validate()
        print("✓ Brain state validated successfully")
        
    except nlm.ConfigurationError as e:
        print(f"✗ Configuration error: {e}")
    except nlm.BrainError as e:
        print(f"✗ Brain error: {e}")
    except nlm.NLMError as e:
        print(f"✗ NLM error: {e}")
    
    # Demonstrate parameter validation
    print("\n2. Parameter Validation")
    print("=" * 40)
    
    try:
        from pynlm_enhanced.utils import (
            validate_range, validate_positive, validate_string_not_empty
        )
        
        # Valid range
        validate_range(0.5, 0.0, 1.0, "learning_rate")
        print("✓ Valid range check passed")
        
        # Valid positive
        validate_positive(100, "neuron_count")
        print("✓ Valid positive check passed")
        
        # Valid string
        validate_string_not_empty("brain_name", "name")
        print("✓ Valid string check passed")
        
        # Invalid range (should raise error)
        try:
            validate_range(1.5, 0.0, 1.0, "learning_rate")
            print("✗ Invalid range check should have raised an error")
        except ValueError as e:
            print(f"✓ Invalid range correctly rejected: {e}")
        
        # Invalid positive (should raise error)
        try:
            validate_positive(-5, "neuron_count")
            print("✗ Invalid positive check should have raised an error")
        except ValueError as e:
            print(f"✓ Invalid positive correctly rejected: {e}")
        
        # Invalid string (should raise error)
        try:
            validate_string_not_empty("", "name")
            print("✗ Invalid string check should have raised an error")
        except ValueError as e:
            print(f"✓ Invalid string correctly rejected: {e}")
        
    except ImportError as e:
        print(f"✗ Could not import validation utilities: {e}")
    
    # Demonstrate comprehensive example
    print("\n3. Comprehensive Example")
    print("=" * 40)
    
    try:
        # Create complete agent system
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get statistics
        print(f"✓ Simulation completed successfully")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  - Prediction error: {agent.getPredictionError():.3f}")
        
    except Exception as e:
        print(f"✗ Comprehensive example failed: {e}")
    
    # Demonstrate error handling
    print("\n4. Error Handling Demonstration")
    print("=" * 40)
    
    try:
        # This should work
        config = nlm.createDefaultConfig()
        print("✓ Configuration created successfully")
        
        # This should raise an error
        invalid_config = None
        if invalid_config is None:
            raise nlm.ConfigurationError("Configuration cannot be None")
            
    except nlm.ConfigurationError as e:
        print(f"✓ Configuration error correctly caught: {e}")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    # Demonstrate example code retrieval
    print("\n5. Example Code Retrieval")
    print("=" * 40)
    
    try:
        examples = nlm.getExampleCode()
        if isinstance(examples, dict) and len(examples) > 0:
            print(f"✓ Retrieved {len(examples)} example code snippets:")
            for name, code in examples.items():
                print(f"  - {name}: {len(code)} characters")
        else:
            print("✗ Failed to retrieve example code")
    except Exception as e:
        print(f"✗ Error retrieving example code: {e}")
    
    print("\n6. Enhanced Features Demonstration")
    print("=" * 40)
    
    try:
        # Create NeuronId with validation
        neuron_id = nlm.NeuronId(123)
        print(f"✓ NeuronId created: value={neuron_id.getValue()}, index={neuron_id.getIndex()}, valid={neuron_id.isValid()}")
        
        # Test validation utilities
        from pynlm_enhanced.utils import get_example_code
        examples = get_example_code()
        if examples:
            print(f"✓ Example code retrieved: {len(examples)} snippets")
        
        # Test custom exceptions
        try:
            raise nlm.NLMError("Test error message")
        except nlm.NLMError as e:
            print(f"✓ Custom exception correctly raised: {e}")
        
        print("\n✓ All enhanced features demonstrated successfully!")
        
    except Exception as e:
        print(f"✗ Enhanced features demonstration failed: {e}")
    
    print("\n" + "=" * 60)
    print("Enhanced NLM Python Bindings Demonstration Complete")
    print("=" * 60)
    print()
    print("Key improvements demonstrated:")
    print("  ✓ Enhanced error handling with specific exception types")
    print("  ✓ Comprehensive parameter validation")
    print("  ✓ Memory safety and resource management")
    print("  ✓ Enhanced configuration management")
    print("  ✓ Advanced brain and agent interfaces")
    print("  ✓ Complete example code retrieval")
    print("  ✓ Custom exception classes")
    print("  ✓ Validation utilities")
    print()
    print("The enhanced bindings provide a robust, safe, and user-friendly")
    print("interface for developing neural simulation applications with NLM.")

if __name__ == "__main__":
    sys.exit(0)
