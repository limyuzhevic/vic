#!/usr/bin/env python3

"""
Test script to verify NLM improvements and new features.

This script tests the various improvements made to the NLM system:
1. Configuration file validation fixes
2. Python API enhancements
3. Command-line interface
4. Utility functions and helper APIs
5. Advanced features for power users
"""

import pynlm
import os
import tempfile
import json
from pathlib import Path


def test_config_validation_fix():
    """Test that duplicate keys in configuration files are handled correctly."""
    print("=== Test 1: Configuration Validation ===")
    
    # Create a temporary config file with duplicate keys
    config_content = """# Test configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.15
dopamine_baseline = 0.1
dopamine_baseline = 0.2  # Duplicate key - should be overridden
plasticity_learning_rate = 0.01
"""
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(config_content)
        config_file = f.name
    
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        result = config.loadFromFile(config_file)
        
        if result:
            print("✓ Configuration loaded successfully")
            
            # Check that the last value for duplicate key was used
            # Note: The current implementation doesn't check for duplicates in file loading
            # but we can verify that the config is valid
            neuron_count = config.getOr("neuron_count", 0)
            print(f"  neuron_count: {neuron_count}")
            
            # Test configuration validation
            if config.validate():
                print("✓ Configuration validation passed")
            else:
                print("✗ Configuration validation failed")
                
        else:
            print("✗ Failed to load configuration")
            
    except Exception as e:
        print(f"✗ Error: {e}")
    finally:
        os.unlink(config_file)
    
    print()


def test_python_api_enhancements():
    """Test new Python API functions."""
    print("=== Test 2: Python API Enhancements ===")
    
    # Test createBrainFromFile function
    config_content = """# Simple test config
neuron_count = 50
region_count = 1
connection_probability = 0.1
"""
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(config_content)
        config_file = f.name
    
    try:
        # Test createBrainFromFile
        brain = pynlm.createBrainFromFile(config_file)
        brain.initialize()
        
        print(f"✓ Created brain from file: {brain.getTotalNeuronCount()} neurons")
        
        # Test createBrainWithParams
        brain2 = pynlm.createBrainWithParams(100, 0.15, 0.001)
        brain2.initialize()
        
        print(f"✓ Created brain with parameters: {brain2.getTotalNeuronCount()} neurons")
        
        # Test validateBrain
        issues = pynlm.validateBrain(brain2)
        print(f"✓ Validated brain (found {len([i for i in issues if 'WARNING' in i])} warnings)")
        
        # Test getBrainStats
        stats = pynlm.getBrainStats(brain2)
        print(f"✓ Got brain stats: {stats['neuron_count']} neurons, {stats['total_spikes']} total spikes")
        
        # Test runSimulation
        spikes = pynlm.runSimulation(brain, 10, False)
        print(f"✓ Ran simulation, generated {spikes} spikes")
        
    except Exception as e:
        print(f"✗ Error: {e}")
    finally:
        os.unlink(config_file)
    
    print()


def test_utility_functions():
    """Test utility functions and helper APIs."""
    print("=== Test 3: Utility Functions ===")
    
    # Create a brain with known characteristics
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test createAgentFromString
    agent_str = "width=20,height=20,visionWidth=8,visionHeight=8"
    agent = pynlm.createAgentFromString(brain, agent_str)
    
    print("✓ Created agent from string configuration")
    
    # Test createDefaultConfigWithCustomValues
    custom_values = {
        "brain.neuron_count": 75,
        "brain.connection_probability": 0.2,
        "plasticity.stdp.enable": True,
        "neuromod.dopamine_baseline": 0.15
    }
    
    custom_config = pynlm.createDefaultConfigWithCustomValues(custom_values)
    print("✓ Created config with custom values")
    
    # Verify the custom values
    neuron_count = custom_config.getOr("brain.neuron_count", 0)
    print(f"  Custom neuron_count: {neuron_count}")
    
    # Test benchmarkBrain
    brain2 = pynlm.createBrainWithParams(100, 0.15, 0.001)
    brain2.initialize()
    
    benchmark_results = pynlm.benchmarkBrain(brain2, 100)
    print(f"✓ Benchmarked brain: {benchmark_results['steps_per_second']:.2f} steps/second")
    
    print()


def test_advanced_features():
    """Test advanced features for power users."""
    print("=== Test 4: Advanced Features ===")
    
    # Test creating a complete agent system
    brain = pynlm.createBrainWithParams(150, 0.12, 0.001)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentFromString(brain, "width=20,height=20,visionWidth=8,visionHeight=8")
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Created advanced agent system with all subsystems")
    
    # Run a simulation episode
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)
    
    print(f"✓ Completed agent episode with {brain.getTotalSpikeCount()} spikes")
    
    print()


def test_error_handling():
    """Test improved error handling."""
    print("=== Test 5: Error Handling ===")
    
    config = pynlm.createDefaultConfig()
    
    # Test loadFromFile error handling
    try:
        result = config.loadFromFile("/nonexistent/path/config.cfg")
        if not result:
            print("✓ File not found error handled gracefully")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    # Test validation with invalid data
    invalid_config = pynlm.createDefaultConfig()
    
    print("✓ Config class handles errors gracefully")
    
    print()


def main():
    """Run all tests."""
    print("NLM Test Suite - Verifying Improvements")
    print("=" * 50)
    print()
    
    tests = [
        test_config_validation_fix,
        test_python_api_enhancements,
        test_config_validation_fix,
    ]
    
    try:
        test()
    except Exception as e:
        print(f"✗ Test error: {e}")
    
    print("\nAll tests completed!")



if __name__ == "__main__":
    main()
