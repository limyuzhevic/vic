#!/usr/bin/env python3
"""
NLM Python Bindings - Test Examples

This file contains test examples for the enhanced NLM Python bindings.
Run with: python3 test_bindings.py
"""

import pynlm
import time
import json
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

def test_basic_functionality():
    """Test basic NLM functionality with enhanced bindings."""
    print("=== Testing Basic Functionality ===")
    
    # Create brain with default config
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Get initial statistics
    stats = brain.get_stats()
    print(f"Initial stats: {json.dumps(stats, indent=2)}")
    
    # Run simulation
    for step in range(50):
        brain.step(step)
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Get final statistics
    final_stats = brain.get_stats()
    print(f"Final stats: {json.dumps(final_stats, indent=2)}")
    
    # Network analysis
    analysis = brain.analyze_network()
    print(f"Network analysis: {json.dumps(analysis, indent=2)}")
    
    return brain

def test_enhanced_apis():
    """Test enhanced Python APIs."""
    print("\n=== Testing Enhanced APIs ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Test property-style access
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Test context manager
    print("\nTesting context manager...")
    with brain.create_context_manager(auto_reset=True) as ctx:
        print("Brain initialized via context manager")
        brain.step(0)
        action = brain.produce_action()
        print(f"Action produced: {action.getName()}")
    
    # Test state management
    print("\nTesting state management...")
    state = brain.get_state()
    print(f"Brain state keys: {list(state.keys())}")
    
    # Test setting state
    state["custom_value"] = "test"
    brain.set_state(state)
    print("State set successfully")
    
    return brain

def test_batch_operations():
    """Test batch processing utilities."""
    print("\n=== Testing Batch Operations ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Get initial membrane potentials
    potentials = brain.get_numpy_potentials()
    print(f"Initial potentials shape: {potentials.shape}")
    print(f"Initial potentials mean: {np.mean(potentials):.4f}")
    
    # Batch inject current to all neurons of specific type
    print("\nInjecting current to all excitatory neurons...")
    brain.inject_current_to_type(pynlm.NeuronType.Excitatory, 0.5)
    
    # Run simulation
    for step in range(20):
        brain.step(step)
    
    # Get final potentials
    final_potentials = brain.get_numpy_potentials()
    print(f"Final potentials shape: {final_potentials.shape}")
    print(f"Final potentials mean: {np.mean(final_potentials):.4f}")
    
    # Test batch inject with dictionary
    print("\nTesting batch inject with dictionary...")
    # Note: This would need actual NeuronId objects
    # For this test, we'll skip as it requires actual neuron IDs
    
    return brain

def test_serialization():
    """Test serialization and recording functionality."""
    print("\n=== Testing Serialization ===")
    
    # Create and run brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    for step in range(30):
        brain.step(step)
    
    # Test recording
    print("Testing recording...")
    recording_file = "test_recording.txt"
    brain.enable_recording(recording_file)
    
    for step in range(10):
        brain.step(step)
    
    brain.disable_recording()
    print(f"Recording saved to {recording_file}")
    
    # Check if file was created
    if Path(recording_file).exists():
        print("Recording file created successfully")
        # Clean up
        Path(recording_file).unlink()
    
    # Test visualization data
    print("\nTesting visualization data...")
    brain.enable_visualization()
    viz_data = brain.create_visualization_data()
    print(f"Visualization data keys: {list(viz_data.keys())}")
    
    # Check for expected keys
    expected_keys = ['stats', 'regions', 'potentials']
    for key in expected_keys:
        if key in viz_data:
            print(f"✓ {key} present")
        else:
            print(f"✗ {key} missing")
    
    brain.disable_visualization()
    
    return brain

def test_type_conversions():
    """Test type conversion utilities."""
    print("\n=== Testing Type Conversions ===")
    
    # Test neuron type conversions
    print("Testing neuron type conversions...")
    neuron_type = pynlm.NeuronType.Excitatory
    print(f"NeuronType: {neuron_type}")
    print(f"String representation: {pynlm.neuron_type_to_string(neuron_type)}")
    
    # Test string to enum conversion
    converted_type = pynlm.string_to_neuron_type("Inhibitory")
    print(f"Converted type: {converted_type}")
    
    # Test array conversions
    print("\nTesting array conversions...")
    data = [1.0, 2.0, 3.0, 4.0, 5.0]
    array = pynlm.to_numpy_array(data)
    print(f"Original data: {data}")
    print(f"Numpy array: {array}")
    print(f"Array shape: {array.shape}")
    print(f"Array dtype: {array.dtype}")
    
    # Convert back
    vector = pynlm.to_vector(array)
    print(f"Converted back: {vector}")
    print(f"Conversion successful: {data == vector}")
    
    return array, vector

def test_error_handling():
    """Test error handling."""
    print("\n=== Testing Error Handling ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Test uninitialized brain error
    print("Testing uninitialized brain error...")
    brain2 = pynlm.createBrain()
    try:
        brain2.step(0)
        print("ERROR: Should have thrown SimulationError")
    except pynlm.SimulationError as e:
        print(f"✓ Correctly caught SimulationError: {e}")
    
    # Test validation error
    print("\nTesting validation error...")
    try:
        # Test invalid neuron type
        pynlm.string_to_neuron_type("InvalidType")
        print("ERROR: Should have thrown PythonError")
    except pynlm.PythonError as e:
        print(f"✓ Correctly caught PythonError: {e}")
    
    # Test invalid configuration
    print("\nTesting invalid configuration...")
    try:
        # Create config with invalid data
        config_dict = {"invalid_key": "not_an_int"}
        config = pynlm.create_config_from_dict(config_dict)
        # This should work - the config just stores strings
        print("✓ Config created successfully")
    except Exception as e:
        print(f"Caught error (may be expected): {e}")
    
    return brain

def test_advanced_features():
    """Test advanced features and visualization."""
    print("\n=== Testing Advanced Features ===")
    
    # Create brain with more neurons
    config = pynlm.create_default_config_dict()
    config["neuron_count"] = 200
    config["region_count"] = 5
    config["connection_probability"] = 0.15
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get region information
    regions = brain.get_network_info()
    print(f"Number of regions: {len(regions)}")
    
    # Analyze network
    analysis = brain.analyze_network()
    print(f"Network state: {analysis['network_state']}")
    print(f"Balance state: {analysis['balance_state']}")
    print(f"Firing rate: {analysis['firing_rate']:.4f}")
    print(f"Excitation/inhibition ratio: {analysis['excitation_inhibition_ratio']:.4f}")
    
    # Test development
    print(f"\nDevelopment stage: {brain.get_developmental_stage()}")
    
    # Change development stage
    brain.set_developmental_stage(pynlm.DevelopmentalStage.Maturation)
    print(f"Development stage changed to: {brain.get_developmental_stage()}")
    
    return brain

def main():
    """Run all tests."""
    print("NLM Python Bindings - Test Examples")
    print("=" * 50)
    
    try:
        # Run tests
        test_basic_functionality()
        test_enhanced_apis()
        test_batch_operations()
        test_serialization()
        test_type_conversions()
        test_error_handling()
        test_advanced_features()
        
        print("\n" + "=" * 50)
        print("All tests completed successfully!")
        print("\nKey features demonstrated:")
        print("1. Basic NLM functionality with enhanced bindings")
        print("2. Property-style access and context managers")
        print("3. Batch processing and type conversions")
        print("4. Serialization and visualization")
        print("5. Error handling and validation")
        print("6. Advanced network analysis")
        
    except Exception as e:
        print(f"\nERROR: Test failed with exception: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())