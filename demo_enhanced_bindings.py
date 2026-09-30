#!/usr/bin/env python3
"""
Demo script showcasing the improved NLM Python bindings.

This script demonstrates all the new features and improvements:
1. Modular structure with clear separation
2. Enhanced documentation and examples
3. Improved error handling
4. Convenience methods and factory functions
5. Type safety and validation
6. Advanced APIs for experienced users
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm as nlm
import numpy as np
import math

def demo_basic_usage():
    """Demonstrate basic usage with improved API."""
    print("=" * 60)
    print("DEMO 1: Basic Usage with Improved API")
    print("=" * 60)
    
    # Create a brain with enhanced factory function
    print("1. Creating brain with default configuration...")
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    brain.initialize()
    print("   ✓ Brain created successfully")
    
    # Create a world with improved factory function
    print("2. Creating world with dimensions...")
    world = nlm.createSimpleWorld(100, 100, 10, 10)
    world.setAgentStart(50, 50)
    print("   ✓ World configured successfully")
    
    # Create agent brain interface
    print("3. Creating agent brain interface...")
    agent_brain = nlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    print("   ✓ Agent brain initialized successfully")
    
    # Run simulation with monitoring
    print("4. Running simulation...")
    for step in range(10):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        world.update(0.01)
        
        body = world.getAgentBody()
        print(f"   Step {step+1}: Position ({body.x:.1f}, {body.y:.1f}), "
              f"Energy: {body.energy:.1f}, Health: {body.health:.1f}")
    
    print("   ✓ Simulation completed successfully")
    print()

def demo_enhanced_configuration():
    """Demonstrate enhanced configuration API."""
    print("=" * 60)
    print("DEMO 2: Enhanced Configuration API")
    print("=" * 60)
    
    # Create configuration with type-specific methods
    print("1. Creating configuration with type validation...")
    config = nlm.Config()
    
    # Set values using type-specific methods
    config.setInt("brain.neuron_count", 150)
    config.setDouble("brain.synaptic_weight", 0.15)
    config.setBool("brain.enable_learning", True)
    config.setString("brain.description", "Enhanced learning environment")
    
    # Set vector values
    config.set("brain.position", [1.0, 2.0, 3.0])
    
    print("   ✓ Configuration created with type validation")
    
    # Get values with type safety
    print("2. Retrieving values with type safety...")
    neuron_count = config.getInt("brain.neuron_count")
    synaptic_weight = config.getDouble("brain.synaptic_weight")
    enable_learning = config.getBool("brain.enable_learning")
    description = config.getString("brain.description")
    position = config.get("brain.position")
    
    print(f"   Neuron count: {neuron_count}")
    print(f"   Synaptic weight: {synaptic_weight}")
    print(f"   Enable learning: {enable_learning}")
    print(f"   Description: {description}")
    print(f"   Position: {position}")
    
    # Create brain with enhanced config
    print("3. Creating brain with enhanced configuration...")
    brain = nlm.Brain(config)
    brain.initialize()
    print("   ✓ Brain created with enhanced configuration")
    
    print()

def demo_error_handling():
    """Demonstrate improved error handling."""
    print("=" * 60)
    print("DEMO 3: Improved Error Handling")
    print("=" * 60)
    
    print("1. Testing configuration error handling...")
    config = nlm.Config()
    
    try:
        # This should raise ConfigError
        config.getInt("nonexistent_key")
        print("   ✗ Should have raised ConfigError")
    except nlm.ConfigError as e:
        print(f"   ✓ ConfigError caught: {e}")
    
    print("2. Testing brain error handling...")
    try:
        # Create brain and try to use invalid neuron ID
        config = nlm.Config()
        brain = nlm.Brain(config)
        brain.initialize()
        
        # This should raise NLMError with helpful message
        nlm.validateNeuronId(nlm.INVALID_NEURON_ID)
        print("   ✗ Should have raised NLMError")
    except nlm.NLMError as e:
        print(f"   ✓ NLMError caught: {e}")
    
    print()

def demo_numpy_integration():
    """Demonstrate NumPy integration utilities."""
    print("=" * 60)
    print("DEMO 4: NumPy Integration")
    print("=" * 60)
    
    print("1. Creating vision input and converting to NumPy...")
    vision = nlm.Vision(10, 10, 3)
    
    # Create test data
    data = [float(i % 256) / 255.0 for i in range(10 * 10 * 3)]
    vision.setData(data)
    
    # Convert to NumPy array using enhanced utility
    numpy_array = nlm.visionToNumpy(vision)
    print(f"   ✓ Converted vision to NumPy array: shape {numpy_array.shape}, dtype {numpy_array.dtype}")
    
    # Process with NumPy
    processed = numpy_array * 2.0 - 1.0  # Normalize to [-1, 1]
    print(f"   ✓ Processed data range: [{processed.min():.3f}, {processed.max():.3f}]")
    
    # Create matrix and convert to NumPy
    print("2. Creating matrix and converting to NumPy...")
    matrix_data = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0]]
    numpy_matrix = nlm.to_numpy_matrix(matrix_data)
    print(f"   ✓ Converted matrix to NumPy: shape {numpy_matrix.shape}")
    
    # Perform NumPy operations
    diagonal_sum = numpy_matrix.diagonal().sum()
    print(f"   ✓ Matrix diagonal sum: {diagonal_sum:.1f}")
    
    print()

def demo_batch_operations():
    """Demonstrate batch operations for improved performance."""
    print("=" * 60)
    print("DEMO 5: Batch Operations")
    print("=" * 60)
    
    # Create a simple world
    print("1. Creating world for batch operations...")
    world = nlm.createSimpleWorld(50, 50, 5, 5)
    
    # Multiple timesteps for batch update
    print("2. Performing batch world updates...")
    timesteps = [0.01, 0.01, 0.01, 0.01, 0.01]
    nlm.batchUpdateWorld(world, timesteps)
    print("   ✓ Batch updates completed successfully")
    
    # Check agent safety
    print("3. Checking agent safety...")
    body = world.getAgentBody()
    safety = nlm.checkAgentSafety(world, body)
    print(f"   ✓ Agent safety check completed")
    print(f"   - Valid position: {safety['is_valid_position']}")
    print(f"   - Healthy: {safety['is_healthy']}")
    print(f"   - Safe: {safety['is_safe']}")
    
    print()

def demo_advanced_features():
    """Demonstrate advanced APIs for experienced users."""
    print("=" * 60)
    print("DEMO 6: Advanced Features")
    print("=" * 60)
    
    # Create brain for advanced features
    print("1. Creating brain for advanced features...")
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Get connectivity matrix
    print("2. Getting brain connectivity matrix...")
    connectivity = nlm.getConnectivityMatrix(brain)
    print(f"   ✓ Connectivity matrix shape: {connectivity.shape}")
    print(f"   ✓ Matrix range: [{connectivity.min():.3f}, {connectivity.max():.3f}]")
    
    # Get activity summary
    print("3. Getting brain activity summary...")
    activity = nlm.getActivitySummary(brain)
    print(f"   ✓ Total neurons: {activity['total_neurons']}")
    print(f"   ✓ Active neurons: {activity['active_neurons']}")
    print(f"   ✓ Firing neurons: {activity['firing_neurons']}")
    print(f"   ✓ Average firing rate: {activity['average_firing_rate']:.2f} Hz")
    print(f"   ✓ Developmental stage: {activity['developmental_stage']}")
    
    # Type validation utilities
    print("4. Using type validation utilities...")
    valid_neuron_id = nlm.NeuronId(123)
    nlm.validateNeuronId(valid_neuron_id)
    print("   ✓ Valid neuron ID passed validation")
    
    try:
        nlm.validateNeuronId(nlm.INVALID_NEURON_ID)
        print("   ✗ Invalid neuron ID should have raised error")
    except nlm.NLMError as e:
        print(f"   ✓ Invalid neuron ID correctly rejected: {str(e)[:50]}...")
    
    # Range validation
    nlm.validateRange(5.0, 0.0, 10.0, "test_param")
    print("   ✓ Value within range passed validation")
    
    try:
        nlm.validateRange(15.0, 0.0, 10.0, "test_param")
        print("   ✗ Value outside range should have raised error")
    except nlm.NLMError as e:
        print(f"   ✓ Value outside range correctly rejected: {str(e)[:50]}...")
    
    print()

def demo_backward_compatibility():
    """Demonstrate backward compatibility with existing API."""
    print("=" * 60)
    print("DEMO 7: Backward Compatibility")
    print("=" * 60)
    
    print("1. Using original API methods...")
    config = nlm.Config()
    
    # Original config methods
    config.loadFromFile("nonexistent.json")  # Should handle gracefully
    config.saveToFile("nonexistent.json")   # Should handle gracefully
    
    # Test action creation
    action1 = nlm.Action(nlm.ActionType.MoveForward)
    action2 = nlm.Action(nlm.ActionType.Custom, [0.5, 1.0])
    
    print(f"   ✓ Action 1 type: {action1.getType()}, name: {action1.getName()}")
    print(f"   ✓ Action 2 type: {action2.getType()}, params: {action2.getParameters()}")
    
    # Test world creation
    world = nlm.SimpleWorld()
    world.configure(100, 100, 10, 10)
    world.setAgentStart(50, 50)
    world.reset()
    print("   ✓ World operations work as before")
    
    # Test brain creation
    brain = nlm.Brain(config)
    brain.initialize()
    brain.reset()
    print("   ✓ Brain operations work as before")
    
    print()

def main():
    """Run all demos."""
    print("Enhanced NLM Python Bindings Demo")
    print("=" * 60)
    print()
    
    try:
        demo_basic_usage()
        demo_enhanced_configuration()
        demo_error_handling()
        demo_numpy_integration()
        demo_batch_operations()
        demo_advanced_features()
        demo_backward_compatibility()
        
        print("=" * 60)
        print("All demos completed successfully!")
        print("=" * 60)
        print()
        print("The enhanced NLM Python bindings provide:")
        print("✓ Modular structure with clear separation of concerns")
        print("✓ Comprehensive documentation and examples")
        print("✓ Improved error handling with custom exceptions")
        print("✓ Convenience methods and factory functions")
        print("✓ Type safety and validation utilities")
        print("✓ Advanced APIs for experienced users")
        print("✓ Full backward compatibility")
        print()
        print("For more information, see the documentation and examples.")
        
    except Exception as e:
        print(f"\n✗ Demo failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0
if __name__ == "__main__":
    sys.exit(main())