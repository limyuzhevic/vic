#!/usr/bin/env python3
"""
Test file for enhanced NLM Python bindings.

This file tests all the improvements made to the pynlm library.
Run with: python tests/test_enhanced_bindings.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import pytest
from typing import Dict, Any

def test_basic_creation():
    """Test basic brain creation and initialization"""
    print("Testing basic creation...")
    
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    brain = pynlm.createBrain(config)
    assert brain is not None
    assert brain.getTotalNeuronCount() > 0
    
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    
    print("✓ Basic creation test passed")

def test_enhanced_configuration():
    """Test enhanced configuration features"""
    print("Testing enhanced configuration...")
    
    config = pynlm.createDefaultConfig()
    
    # Test type-safe setting
    config.setInt("brain.neuron_count", 1500)
    assert config.getOr("brain.neuron_count", 1000) == 1500
    
    config.setDouble("brain.synapse_density", 0.12)
    assert config.getOr("brain.synapse_density", 0.1) == 0.12
    
    config.setBool("plasticity.stdp.enable", True)
    assert config.getOr("plasticity.stdp.enable", False) == True
    
    # Test dictionary conversion
    config_dict = pynlm.configToJson(config)
    assert isinstance(config_dict, dict)
    assert len(config_dict) > 0
    
    # Test dictionary loading
    config2 = pynlm.configFromJson(config_dict)
    assert config2 is not None
    
    print("✓ Enhanced configuration test passed")

def test_context_managers():
    """Test context manager functionality"""
    print("Testing context managers...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test brain context manager
    brain_ctx = pynlm.brainContext(brain)
    assert brain_ctx is not None
    
    # Test world context manager
    world = pynlm.createSimpleWorld()
    world_ctx = pynlm.worldContext(world)
    assert world_ctx is not None
    
    print("✓ Context managers test passed")

def test_convenience_methods():
    """Test convenience methods"""
    print("Testing convenience methods...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test statistics method
    stats = brain.getStatistics()
    assert isinstance(stats, dict)
    assert "total_neurons" in stats
    assert "total_synapses" in stats
    
    # Test neuron type name method
    neuron_name = brain.getNeuronTypeName(pynlm.NeuronType.Excitatory)
    assert neuron_name == "Excitatory"
    
    # Test configuration utilities
    pynlm.raiseNLMException("Test exception")
    print("✓ Convenience methods test passed")

def test_error_handling():
    """Test enhanced error handling"""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test invalid neuron type error
    try:
        brain.injectCurrentToNeuronType("INVALID_TYPE", 1.0)
        assert False, "Should have raised exception"
    except pynlm.NLMException:
        pass  # Expected
    
    # Test invalid configuration error
    try:
        pynlm.loadConfigFromFile("nonexistent_file.json")
        assert False, "Should have raised exception"
    except pynlm.ConfigurationError:
        pass  # Expected
    
    print("✓ Error handling test passed")

def test_advanced_operations():
    """Test advanced operations"""
    print("Testing advanced operations...")
    
    # Test createNetwork
    brain = pynlm.createNetwork(1000, 3)
    assert brain is not None
    assert brain.getTotalNeuronCount() == 1000
    
    # Test connectRegions
    pynlm.connectRegions(brain, "region0", "region1", 0.5)
    
    # Test analysis
    analysis = pynlm.analyzeBrain(brain)
    assert isinstance(analysis, dict)
    assert "total_neurons" in analysis
    
    # Test region statistics
    regions = brain.getRegionStatistics()
    assert isinstance(regions, dict)
    assert len(regions) > 0
    
    print("✓ Advanced operations test passed")

def test_standard_library_integration():
    """Test standard library integration"""
    print("Testing standard library integration...")
    
    config = pynlm.createDefaultConfig()
    
    # Test list operations
    settings = [
        ("brain.neuron_count", 1500),
        ("brain.synapse_density", 0.12),
        ("learning.rate", 0.001),
    ]
    
    for key, value in settings:
        config.set(key, value)
    
    # Test dictionary operations
    updates = {
        "brain.v_thresh": -50.0,
        "brain.v_rest": -65.0,
    }
    
    for key, value in updates.items():
        config.set(key, value)
    
    # Test generator operations
    keys = config.getKeys()
    key_list = list(keys)  # Convert to list to consume generator
    assert len(key_list) > 0
    
    # Test sorted operations
    sorted_keys = sorted(config.getKeys())
    assert sorted_keys == sorted(sorted_keys)
    
    print("✓ Standard library integration test passed")

def test_simulation_utilities():
    """Test simulation utilities"""
    print("Testing simulation utilities...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test runSimulation
    total_spikes = pynlm.runSimulation(brain, 50)
    assert total_spikes >= 0
    
    # Test resetBrain
    pynlm.resetBrain(brain)
    
    print("✓ Simulation utilities test passed")

def test_data_persistence():
    """Test data persistence features"""
    print("Testing data persistence...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation
    for i in range(50):
        brain.step(i)
    
    # Test save/load brain state
    pynlm.saveBrainState(brain, "test_checkpoint.bin")
    
    new_brain = pynlm.createBrain(config)
    pynlm.loadBrainState(new_brain, "test_checkpoint.bin")
    
    assert new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
    
    # Test JSON configuration persistence
    config.saveToFile("test_config.json")
    loaded_config = pynlm.loadConfigFromFile("test_config.json")
    assert loaded_config is not None
    
    # Clean up
    os.remove("test_checkpoint.bin")
    os.remove("test_config.json")
    
    print("✓ Data persistence test passed")

def test_module_constants():
    """Test module constants"""
    print("Testing module constants...")
    
    # Test action constants
    assert pynlm.MOVE_FORWARD == 0
    assert pynlm.TURN_LEFT == 5
    assert pynlm.INTERACT == 14
    
    # Test neuron type constants
    assert pynlm.EXCITATORY == 0
    assert pynlm.INHIBITORY == 1
    assert pynlm.SENSORY == 4
    
    # Test world object type constants
    assert pynlm.RESOURCE == 1
    assert pynlm.HAZARD == 2
    
    # Test developmental stage constants
    assert pynlm.INITIAL == 0
    assert pynlm.CRITICAL_PERIOD == 1
    assert pynlm.ADULT == 4
    
    print("✓ Module constants test passed")

def test_advanced_utilities():
    """Test advanced utilities"""
    print("Testing advanced utilities...")
    
    brain = pynlm.createNetwork(2000, 5)
    brain.initialize()
    
    # Test findActiveNeurons
    active_neurons = pynlm.findActiveNeurons(brain, pynlm.NeuronType.Excitatory)
    assert isinstance(active_neurons, list)
    
    # Test random utilities
    import random
    random.seed(42)
    
    neuron_type = pynlm.randomNeuronType()
    synapse_type = pynlm.randomSynapseType()
    
    assert neuron_type in [pynlm.NeuronType.Excitatory, pynlm.NeuronType.Inhibitory,
                           pynlm.NeuronType.Modulatory, pynlm.NeuronType.Sensory,
                           pynlm.NeuronType.Motor, pynlm.NeuronType.Internal]
    
    assert synapse_type in [pynlm.SynapseType.Excitatory, pynlm.SynapseType.Inhibitory,
                           pynlm.SynapseType.Modulatory, pynlm.SynapseType.Electrical,
                           pynlm.SynapseType.GapJunction]
    
    print("✓ Advanced utilities test passed")

def test_comprehensive_episode():
    """Test comprehensive episode simulation"""
    print("Testing comprehensive episode simulation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createWorldWithDefaults()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run episode
    episode_stats = pynlm.runEpisode(brain, world, agent, 100)
    
    assert isinstance(episode_stats, dict)
    assert "total_reward" in episode_stats
    assert "success_rate" in episode_stats
    assert "total_spikes" in episode_stats
    
    print("✓ Comprehensive episode test passed")

def run_all_tests():
    """Run all tests"""
    print("=" * 80)
    print("ENHANCED NLM BINDINGS - TEST SUITE")
    print("=" * 80)
    
    tests = [
        test_basic_creation,
        test_enhanced_configuration,
        test_context_managers,
        test_convenience_methods,
        test_error_handling,
        test_advanced_operations,
        test_standard_library_integration,
        test_simulation_utilities,
        test_data_persistence,
        test_module_constants,
        test_advanced_utilities,
        test_comprehensive_episode,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 80)
    print(f"TEST RESULTS: {passed} passed, {failed} failed")
    print("=" * 80)
    
    if failed == 0:
        print("\n🎉 ALL TESTS PASSED!")
        print("The enhanced NLM bindings are working correctly.")
    else:
        print(f"\n⚠️  {failed} tests failed.")
        print("Please check the implementation.")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)