#!/usr/bin/env python3
"""
Example usage of the enhanced NLM Python bindings.

This script demonstrates how to use the new features of the NLM Python bindings.
Run with: python3 example_usage.py
"""

import pynlm
import json
import time

def example_basic_usage():
    """Example 1: Basic usage of NLM."""
    print("Example 1: Basic Usage")
    print("-" * 30)
    
    # Create a simple brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run some steps
    for i in range(10):
        brain.step(i)
        if i % 5 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Final: {brain.getTotalSpikeCount()} total spikes")
    print()

def example_enhanced_features():
    """Example 2: Using enhanced Python features."""
    print("Example 2: Enhanced Python Features")
    print("-" * 30)
    
    # Create brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Get network statistics
    stats = brain.get_stats()
    print("Network statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Analyze network
    analysis = brain.analyze_network()
    print(f"\nNetwork state: {analysis['network_state']}")
    print(f"Balance state: {analysis['balance_state']}")
    
    # Use context manager
    print("\nUsing context manager:")
    with brain.create_context_manager(auto_reset=True):
        brain.step(0)
        action = brain.produce_action()
        print(f"Produced action: {action.getName()}")
    
    print()

def example_batch_operations():
    """Example 3: Batch operations."""
    print("Example 3: Batch Operations")
    print("-" * 30)
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Get initial membrane potentials
    potentials = brain.get_numpy_potentials()
    print(f"Initial potentials shape: {potentials.shape}")
    print(f"Initial potentials mean: {potentials.mean():.4f}")
    
    # Batch inject current to all excitatory neurons
    print("\nInjecting current to all excitatory neurons...")
    brain.inject_current_to_type(pynlm.NeuronType.Excitatory, 0.5)
    
    # Run simulation
    for i in range(5):
        brain.step(i)
    
    # Get final potentials
    final_potentials = brain.get_numpy_potentials()
    print(f"Final potentials shape: {final_potentials.shape}")
    print(f"Final potentials mean: {final_potentials.mean():.4f}")
    print(f"Potentials changed by: {(final_potentials - potentials).mean():.4f}")
    print()

def example_serialization():
    """Example 4: Serialization and recording."""
    print("Example 4: Serialization and Recording")
    print("-" * 30)
    
    # Create and run brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    for i in range(5):
        brain.step(i)
    
    # Test recording
    recording_file = "test_recording.json"
    print(f"Recording to {recording_file}...")
    brain.enable_recording(recording_file)
    
    for i in range(5):
        brain.step(i)
    
    brain.disable_recording()
    
    # Check recording file
    if os.path.exists(recording_file):
        with open(recording_file, 'r') as f:
            data = json.load(f)
        print(f"Recording file size: {len(data)} bytes")
        os.remove(recording_file)
    
    # Test visualization data
    print("\nCreating visualization data...")
    brain.enable_visualization()
    viz_data = brain.create_visualization_data()
    print(f"Visualization data contains:")
    for key in viz_data.keys():
        print(f"  - {key}")
    
    brain.disable_visualization()
    print()

def example_type_conversions():
    """Example 5: Type conversions."""
    print("Example 5: Type Conversions")
    print("-" * 30)
    
    # Test enum conversions
    print("Enum conversions:")
    for neuron_type in pynlm.NeuronType:
        print(f"  {neuron_type} -> {pynlm.neuron_type_to_string(neuron_type)}")
    
    # Test string to enum
    print(f"\nString to enum: 'Inhibitory' -> {pynlm.string_to_neuron_type('Inhibitory')}")
    
    # Test array conversions
    print("\nArray conversions:")
    data = [1.0, 2.0, 3.0, 4.0, 5.0]
    array = pynlm.to_numpy_array(data)
    print(f"Python list -> Numpy array: {data} -> shape {array.shape}")
    
    vector = pynlm.to_vector(array)
    print(f"Numpy array -> Python list: {array} -> {vector}")
    print()

def example_error_handling():
    """Example 6: Error handling."""
    print("Example 6: Error Handling")
    print("-" * 30)
    
    # Test with uninitialized brain
    brain = pynlm.createBrain()
    try:
        brain.step(0)
        print("ERROR: Should have thrown SimulationError")
    except pynlm.SimulationError as e:
        print(f"✓ Caught SimulationError: {e}")
    
    # Test invalid configuration
    print("\nTesting config validation...")
    config_dict = {"neuron_count": 100}
    config = pynlm.create_config_from_dict(config_dict)
    print(f"Config created with {len(config.getKeys())} keys")
    
    # Test with invalid config data
    invalid_config = {"invalid_type": "should_be_number"}
    config2 = pynlm.create_config_from_dict(invalid_config)
    print(f"Invalid config stored successfully (keys: {config2.getKeys()})")
    
    print()

def example_advanced_analysis():
    """Example 7: Advanced network analysis."""
    print("Example 7: Advanced Network Analysis")
    print("-" * 30)
    
    # Create larger brain for better analysis
    config = pynlm.create_default_config_dict()
    config["neuron_count"] = 500
    config["region_count"] = 10
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    # Get detailed statistics
    stats = brain.get_stats()
    print("Network statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Get region information
    regions = brain.get_network_info()
    print(f"\nNumber of regions: {len(regions)}")
    
    # Analyze network
    analysis = brain.analyze_network()
    print(f"\nNetwork analysis:")
    for key, value in analysis.items():
        print(f"  {key}: {value}")
    
    # Test development
    print(f"\nDevelopment stage: {brain.get_developmental_stage()}")
    
    # Change development stage
    brain.set_developmental_stage(pynlm.DevelopmentalStage.Maturation)
    print(f"Development stage changed to: {brain.get_developmental_stage()}")
    
    # Check if development affects network
    brain.step(50)
    analysis2 = brain.analyze_network()
    print(f"\nAfter development change:")
    print(f"  Firing rate: {analysis2['firing_rate']:.4f}")
    print(f"  Network state: {analysis2['network_state']}")
    
    print()

def main():
    """Run all examples."""
    print("NLM Python Bindings - Example Usage")
    print("=" * 40)
    print()
    
    try:
        example_basic_usage()
        example_enhanced_features()
        example_batch_operations()
        example_serialization()
        example_type_conversions()
        example_error_handling()
        example_advanced_analysis()
        
        print("=" * 40)
        print("All examples completed successfully!")
        print("\nThe enhanced NLM Python bindings provide:")
        print("• Enhanced type conversions (Python ↔ C++)")
        print("• Pythonic APIs with property access")
        print("• Context managers for simulation control")
        print("• Batch processing utilities")
        print("• Serialization and recording support")
        print("• Advanced network analysis tools")
        print("• Comprehensive error handling")
        
    except Exception as e:
        print(f"\nERROR: Example failed: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    # Import os for file handling
    import os
    exit(main())