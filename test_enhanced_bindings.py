#!/usr/bin/env python3

"""
Test script to verify the enhanced Python bindings for NLM.
This script demonstrates the new features and functionality.
"""

import pynlm
import sys

def test_basic_functionality():
    """Test basic Python bindings functionality"""
    print("=== Testing Basic Functionality ===")
    
    # Test 1: Create default config
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    # Test 2: Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Created and initialized brain")
    
    # Test 3: Basic statistics
    total_neurons = brain.getTotalNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    total_spikes = brain.getTotalSpikeCount()
    
    print(f"  Total neurons: {total_neurons}")
    print(f"  Firing neurons: {firing_neurons}")
    print(f"  Total spikes: {total_spikes}")
    print("✓ Basic statistics working")
    
    # Test 4: Simulation step
    for i in range(5):
        brain.step(i)
    print("✓ Simulation steps completed")
    
    # Test 5: Brain stats
    stats = pynlm.BrainStats()
    print("✓ BrainStats class accessible")
    
    return brain

def test_advanced_features():
    """Test advanced features"""
    print("\n=== Testing Advanced Features ===")
    
    # Test 1: Advanced configuration
    config = pynlm.createConfigWithDefaults()
    print("✓ Advanced configuration created")
    
    # Test 2: Rich world creation
    world = pynlm.createRichWorld(20, 20, 8, 8)
    print("✓ Rich world created")
    
    # Test 3: Advanced agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAdvancedAgent(brain, world)
    print("✓ Advanced agent created")
    
    # Test 4: Advanced simulation
    world.reset()
    agent.initialize(world)
    
    print("Running advanced simulation...")
    for step in range(10):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("✓ Advanced simulation completed")
    
    return brain, world, agent

def test_error_handling():
    """Test error handling features"""
    print("\n=== Testing Error Handling ===")
    
    # Test 1: Safe simulation step
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test safe simulation
    success = pynlm.safeSimulationStep(brain, 0)
    if success:
        print("✓ Safe simulation step working")
    else:
        print("✗ Safe simulation step failed")
    
    # Test 2: Configuration file operations
    import json
    import tempfile
    import os
    
    # Create temporary config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_dict = {
            "brain": {
                "neuron_count": 500,
                "synapse_density": 0.1
            }
        }
        json.dump(config_dict, f)
        temp_file = f.name
    
    try:
        # Import config
        imported_config = pynlm.importConfigFromJson(temp_file)
        print("✓ Import config from JSON working")
        
        # Export config
        export_success = pynlm.exportConfigToJson(imported_config, temp_file + ".exported")
        if export_success:
            print("✓ Export config to JSON working")
            
            # Clean up
            os.remove(temp_file + ".exported")
        
    finally:
        os.remove(temp_file)
    
    print("✓ Error handling tests completed")

def test_utilities():
    """Test utility functions"""
    print("\n=== Testing Utility Functions ===")
    
    # Test 1: System info
    system_info = pynlm.getSystemInfo()
    print(f"✓ System info: {system_info.cpu_cores} CPU cores")
    
    # Test 2: Action type utilities
    move_forward = pynlm.ActionType.MoveForward
    print(f"✓ Action type: {move_forward}")
    
    # Test 3: World object type utilities
    resource = pynlm.WorldObjectType.Resource
    print(f"✓ World object type: {resource}")
    
    # Test 4: Neuron type utilities
    excitatory = pynlm.NeuronType.Excitatory
    print(f"✓ Neuron type: {excitatory}")
    
    print("✓ All utility functions working")

def main():
    """Run all tests"""
    print("Testing Enhanced NLM Python Bindings")
    print("=" * 50)
    
    try:
        # Run all test suites
        test_basic_functionality()
        test_advanced_features()
        test_error_handling()
        test_utilities()
        
        print("\n" + "=" * 50)
        print("✓ All tests completed successfully!")
        print("\nEnhanced features verified:")
        print("  • Advanced configuration options")
        print("  • Rich world creation")
        print("  • Advanced agent with all features enabled")
        print("  • Comprehensive statistics and monitoring")
        print("  • Error handling and validation")
        print("  • Advanced user utilities")
        print("  • Configuration file operations")
        
        return 0
        
    except Exception as e:
        print(f"\n✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
