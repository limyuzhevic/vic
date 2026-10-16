#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings
Tests new Pythonic features and improvements
"""

import sys
import traceback
import pynlm as nlm

def test_basic_functionality():
    """Test basic functionality to ensure bindings work"""
    print("=== Testing Basic Functionality ===")
    
    # Test 1: Create default config
    print("Test 1: Creating default config...")
    config = nlm.createDefaultConfig()
    print(f"   Created config: {config}")
    
    # Test 2: Create brain
    print("Test 2: Creating brain...")
    brain = nlm.createBrain(config)
    print(f"   Created brain: {brain}")
    
    # Test 3: Initialize brain
    print("Test 3: Initializing brain...")
    brain.initialize()
    print("   Brain initialized successfully")
    
    # Test 4: Get statistics
    print("Test 4: Getting brain statistics...")
    stats = brain.getStatistics()
    print(f"   Statistics: {stats}")
    
    # Test 5: Create simple world
    print("Test 5: Creating simple world...")
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    print(f"   World configured: {world.getWidth()}x{world.getHeight()}")
    
    # Test 6: Create agent brain
    print("Test 6: Creating agent brain...")
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    print("   Agent brain initialized")
    
    return brain, world, agent

def test_pythonic_features():
    """Test new Pythonic features added to bindings"""
    print("\n=== Testing Pythonic Features ===")
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Test 1: Dictionary conversion
    print("Test 1: Dictionary conversion...")
    stats = brain.getStatistics()
    print(f"   Statistics dict: {stats}")
    assert "total_neurons" in stats
    assert "total_synapses" in stats
    print("   ✓ Dictionary conversion works")
    
    # Test 2: Enum support
    print("Test 2: Enum support...")
    print(f"   NeuronType.Excitatory: {nlm.NeuronType.Excitatory}")
    print(f"   SynapseType.Excitatory: {nlm.SynapseType.Excitatory}")
    print(f"   ActionType.MoveForward: {nlm.ActionType.MoveForward}")
    print("   ✓ Enum support works")
    
    # Test 3: Object creation from dict
    print("Test 3: Object creation from dictionary...")
    # Test Config from dict
    config_dict = {"neuron_count": 100, "region_count": 2}
    config = nlm.create_brain_from_dict(config_dict)
    assert config.has("neuron_count")
    assert config.has("region_count")
    print("   ✓ Config creation from dict works")
    
    # Test Action from dict
    action_dict = {"type": nlm.ActionType.MoveForward, "parameters": [1.0, 2.0]}
    action = nlm.Action.from_dict(action_dict)
    assert action.getType() == nlm.ActionType.MoveForward
    print("   ✓ Action creation from dict works")
    
    # Test WorldObject from dict
    world_obj_dict = {"x": 1.0, "y": 2.0, "type": nlm.WorldObjectType.Resource}
    world_obj = nlm.WorldObject.from_dict(world_obj_dict)
    assert world_obj.x == 1.0
    assert world_obj.y == 2.0
    assert world_obj.type == nlm.WorldObjectType.Resource
    print("   ✓ WorldObject creation from dict works")
    
    # Test 4: Helper functions
    print("Test 4: Helper functions...")
    assert nlm.validate_config(config)
    print("   ✓ validate_config works")
    
    report = nlm.generate_report(brain)
    assert "=== NLM Brain Report ===" in report
    print("   ✓ generate_report works")
    
    # Test 5: Simulation runner
    print("Test 5: Simulation runner...")
    brain2 = nlm.createBrain(nlm.createDefaultConfig())
    brain2.initialize()
    
    nlm.run_brain_simulation(brain2, None, None, 10, 0.1)
    print("   ✓ run_brain_simulation works")
    
    return brain

def test_error_handling():
    """Test error handling and exceptions"""
    print("\n=== Testing Error Handling ===")
    
    # Test 1: Invalid configuration values
    print("Test 1: Invalid configuration values...")
    config = nlm.createDefaultConfig()
    
    # This should work
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_bool", True)
    config.set("test_string", "hello")
    config.set("test_list", [1.0, 2.0, 3.0])
    
    # This should raise ConfigurationError
    try:
        config.set("test_invalid", None)
        print("   ✗ Should have raised ConfigurationError for None value")
        return False
    except nlm.ConfigurationError:
        print("   ✓ ConfigurationError raised for None value")
    
    # Test 2: KeyError for missing keys
    print("Test 2: KeyError for missing keys...")
    try:
        _ = config["nonexistent_key"]
        print("   ✗ Should have raised KeyError for missing key")
        return False
    except KeyError:
        print("   ✓ KeyError raised for missing key")
    
    # Test 3: Type errors
    print("Test 3: Type errors...")
    try:
        config.set("test_complex", complex(1, 2))
        print("   ✗ Should have raised ConfigurationError for unsupported type")
        return False
    except nlm.ConfigurationError:
        print("   ✓ ConfigurationError raised for unsupported type")
    
    return True

def test_advanced_features():
    """Test advanced Pythonic features"""
    print("\n=== Testing Advanced Features ===")
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Test 1: Brain statistics
    print("Test 1: Brain statistics...")
    stats = brain.getStatistics()
    print(f"   Total neurons: {stats.get('total_neurons')}")
    print(f"   Total synapses: {stats.get('total_synapses')}")
    print(f"   Firing neurons: {stats.get('firing_neurons')}")
    print(f"   Average firing rate: {stats.get('average_firing_rate')}")
    
    # Test 2: Agent features
    print("Test 2: Agent features...")
    world = nlm.createSimpleWorld()
    world.configure(10, 10, 5, 5)
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    features = agent.getFeatures()
    print(f"   Neuromodulation level: {features.get('neuromodulation_level')}")
    print(f"   Curiosity level: {features.get('curiosity_level')}")
    print(f"   Novelty level: {features.get('novelty_level')}")
    print(f"   Prediction error: {features.get('prediction_error')}")
    
    # Test 3: World operations
    print("Test 3: World operations...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    pos = world.get_agent_position()
    print(f"   Agent position: {pos}")
    
    world.set_agent_position(5.0, 5.0)
    new_pos = world.get_agent_position()
    print(f"   New agent position: {new_pos}")
    
    # Test 4: Action utilities
    print("Test 4: Action utilities...")
    move_action = nlm.Action(nlm.ActionType.MoveForward)
    print(f"   Move action name: {move_action.getName()}")
    
    action_dict = move_action.to_dict()
    print(f"   Action as dict: {action_dict}")
    
    reconstructed_action = nlm.Action.from_dict(action_dict)
    assert reconstructed_action.getType() == nlm.ActionType.MoveForward
    print("   ✓ Action round-trip conversion works")
    
    return True

def test_batching_operations():
    """Test batch operations and performance utilities"""
    print("\n=== Testing Batch Operations ===")
    
    # Create multiple brains
    brains = []
    for i in range(3):
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        brains.append(brain)
    
    print(f"Created {len(brains)} brains")
    
    # Test batch statistics collection
    print("Test 1: Batch statistics...")
    for i, brain in enumerate(brains):
        stats = brain.getStatistics()
        print(f"   Brain {i}: {stats.get('total_neurons')} neurons, {stats.get('total_synapses')} synapses")
    
    # Test simulation with multiple steps
    print("Test 2: Multi-step simulation...")
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(5):
        brain.step(step)
    
    print(f"   Simulated 5 steps, total spikes: {brain.getTotalSpikeCount()}")
    
    return True

def main():
    """Run all tests"""
    print("Testing Improved NLM Python Bindings")
    print("=" * 50)
    
    try:
        # Test basic functionality
        brain, world, agent = test_basic_functionality()
        
        # Test Pythonic features
        test_pythonic_features()
        
        # Test error handling
        if not test_error_handling():
            print("\n✗ Error handling tests failed")
            return 1
        
        # Test advanced features
        test_advanced_features()
        
        # Test batch operations
        test_batching_operations()
        
        print("\n" + "=" * 50)
        print("✅ All tests passed!")
        print("\nSummary of improvements:")
        print("1. Better enum support with descriptive documentation")
        print("2. Pythonic properties and convenience methods")
        print("3. Dictionary serialization/deserialization")
        print("4. Improved error handling with custom exception types")
        print("5. Helper functions for common operations")
        print("6. Better configuration management")
        print("7. Enhanced agent and brain feature access")
        print("8. Batch operations support")
        
        return 0
        
    except Exception as e:
        print(f"\n✗ Test failed with exception: {e}")
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())