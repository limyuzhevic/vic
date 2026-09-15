#!/usr/bin/env python3
"""Test suite for enhanced NLM Python bindings."""

import pynlm
import tempfile
import os
import json

def test_enhanced_python_bindings():
    """Test all new enhanced Python binding methods."""
    print("Testing Enhanced Python Bindings...")
    
    # Create a basic brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test 1: getNeuronStates
    print("  Testing getNeuronStates...")
    neuron_states = pynlm.getBrainStates(brain)
    assert isinstance(neuron_states, list)
    print(f"    Found {len(neuron_states)} neurons")
    
    # Test 2: getSynapseStates  
    print("  Testing getSynapseStates...")
    synapse_states = pynlm.getSynapseStates(brain)
    assert isinstance(synapse_states, list)
    print(f"    Found {len(synapse_states)} synapses")
    
    # Test 3: getMemoryStats
    print("  Testing getMemoryStats...")
    memory_stats = pynlm.getMemoryStats(brain)
    assert isinstance(memory_stats, dict)
    print(f"    Memory stats keys: {list(memory_stats.keys())}")
    
    # Test 4: getPlasticityStats
    print("  Testing getPlasticityStats...")
    plasticity_stats = pynlm.getPlasticityStats(brain)
    assert isinstance(plasticity_stats, dict)
    print(f"    Plasticity stats keys: {list(plasticity_stats.keys())}")
    
    # Test 5: getDevelopmentStats
    print("  Testing getDevelopmentStats...")
    development_stats = pynlm.getDevelopmentStats(brain)
    assert isinstance(development_stats, dict)
    print(f"    Development stats keys: {list(development_stats.keys())}")
    
    # Test 6: getPredictionStats
    print("  Testing getPredictionStats...")
    prediction_stats = pynlm.getPredictionStats(brain)
    assert isinstance(prediction_stats, dict)
    print(f"    Prediction stats keys: {list(prediction_stats.keys())}")
    
    # Test 7: getCognitionStats
    print("  Testing getCognitionStats...")
    cognition_stats = pynlm.getCognitionStats(brain)
    assert isinstance(cognition_stats, dict)
    print(f"    Cognition stats keys: {list(cognition_stats.keys())}")
    
    # Test 8: getSelfModel
    print("  Testing getSelfModel...")
    self_model = pynlm.getSelfModel(brain)
    assert isinstance(self_model, dict)
    print(f"    Self model keys: {list(self_model.keys())}")
    
    # Test 9: getSocialInfo
    print("  Testing getSocialInfo...")
    social_info = pynlm.getSocialInfo(brain)
    assert isinstance(social_info, dict)
    print(f"    Social info keys: {list(social_info.keys())}")
    
    # Test 10: analyzeBehavior
    print("  Testing analyzeBehavior...")
    behavior_analysis = pynlm.analyzeBehavior(brain)
    assert isinstance(behavior_analysis, dict)
    print(f"    Behavior analysis keys: {list(behavior_analysis.keys())}")
    
    # Test 11: getCheckpointInfo
    print("  Testing getCheckpointInfo...")
    checkpoint_info = pynlm.getCheckpointInfo(brain)
    assert isinstance(checkpoint_info, dict)
    print(f"    Checkpoint info keys: {list(checkpoint_info.keys())}")
    
    # Test 12: triggerCheckpoint
    print("  Testing triggerCheckpoint...")
    checkpoint_result = pynlm.triggerCheckpoint(brain)
    assert isinstance(checkpoint_result, bool)
    print(f"    Checkpoint trigger result: {checkpoint_result}")
    
    # Test 13: compareBrains
    print("  Testing compareBrains...")
    other_brain = pynlm.createBrain(config)
    other_brain.initialize()
    comparison = pynlm.compareBrains(brain, other_brain)
    assert isinstance(comparison, dict)
    print(f"    Comparison keys: {list(comparison.keys())}")
    
    print("✓ All enhanced Python binding tests passed!")

def test_enhanced_config_system():
    """Test enhanced Config system features."""
    print("\nTesting Enhanced Config System...")
    
    # Test JSON format detection
    print("  Testing format detection...")
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump({"neuron_count": 100, "region_count": 1}, f)
        json_file = f.name
    
    try:
        detected_format = pynlm.Config.detectFormat(json_file)
        assert detected_format == "json"
        print(f"    Detected JSON format: {detected_format}")
        
        # Test loading with format
        config = pynlm.Config.loadFromFile(json_file, "json")
        assert config is not None
        print(f"    Loaded config with {len(config.getKeys())} entries")
    finally:
        os.unlink(json_file)
    
    # Test JSON export/import
    print("  Testing JSON export/import...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("connection_probability", 0.15)
    
    json_str = config.exportToString("json")
    assert isinstance(json_str, str)
    assert len(json_str) > 0
    
    new_config = pynlm.Config.importFromString(json_str, "json")
    assert new_config is not None
    assert new_config.getOr<int>("neuron_count", 0) == 200
    print("    JSON export/import successful")
    
    # Test configuration merging
    print("  Testing configuration merging...")
    config1 = pynlm.createDefaultConfig()
    config1.set("neuron_count", 100)
    config1.set("custom_param", "value1")
    
    config2 = pynlm.createDefaultConfig()
    config2.set("region_count", 2)
    config2.set("custom_param", "value2")  # Will override
    
    config1.mergeFrom(config2)
    assert config1.getOr<int>("neuron_count", 0) == 100
    assert config1.getOr<int>("region_count", 0) == 2
    assert config1.getOr<std::string>("custom_param", "") == "value2"
    print("    Configuration merging successful")
    
    print("✓ All enhanced Config system tests passed!")

def test_integration():
    """Test integration between Python bindings and C++ core."""
    print("\nTesting Integration...")
    
    # Create brain and agent brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize()
    
    # Test the full pipeline
    print("  Testing full pipeline...")
    # Create a simple world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Run a simulation step
    for step in range(5):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        brain.step(step)
        action = agent_brain.decodeMotorCommand()
        
    # Test that agent brain can apply reward modulation
    reward_result = agent_brain.applyRewardModulation(1.0, 0.5)
    assert isinstance(reward_result, bool)
    
    # Test development
    agent_brain.updateDevelopment(0.1)
    
    print("    Integration test successful")
    
    # Test new Python helper functions
    print("  Testing Python helper functions...")
    
    # Test getBrainStates helper
    neuron_states = pynlm.getBrainStates(brain)
    assert isinstance(neuron_states, list)
    
    # Test getSynapseStates helper  
    synapse_states = pynlm.getSynapseStates(brain)
    assert isinstance(synapse_states, list)
    
    # Test getMemoryStats helper
    memory_stats = pynlm.getMemoryStats(brain)
    assert isinstance(memory_stats, dict)
    
    print("    Python helper functions successful")
    
    print("✓ All integration tests passed!")

def test_error_conditions():
    """Test error conditions and edge cases."""
    print("\nTesting Error Conditions...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test behavior analysis with small window
    print("  Testing behavior analysis with small window...")
    behavior = pynlm.analyzeBehavior(brain, 1)
    assert isinstance(behavior, dict)
    
    # Test brain comparison with identical brains
    print("  Testing brain comparison...")
    other_brain = pynlm.createBrain(config)
    comparison = pynlm.compareBrains(brain, other_brain)
    assert isinstance(comparison, dict)
    assert "neuron_similarity" in comparison
    assert "behavior_similarity" in comparison
    
    # Test checkpoint operations
    print("  Testing checkpoint operations...")
    checkpoint_info = pynlm.getCheckpointInfo(brain)
    assert isinstance(checkpoint_info, dict)
    
    # Test memory stats on uninitialized brain
    print("  Testing memory stats on brain...")
    # This should work even if some systems aren't fully initialized
    try:
        memory_stats = pynlm.getMemoryStats(brain)
        print(f"    Memory stats retrieved: {type(memory_stats)}")
    except Exception as e:
        print(f"    Expected error for uninitialized systems: {type(e).__name__}")
    
    print("✓ All error condition tests passed!")

def main():
    """Run all tests."""
    print("=" * 60)
    print("Enhanced NLM Python Bindings Test Suite")
    print("=" * 60)
    
    try:
        test_enhanced_python_bindings()
        test_enhanced_config_system()
        test_integration()
        test_error_conditions()
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED SUCCESSFULLY!")
        print("=" * 60)
        return 0
        
    except Exception as e:
        print(f"\n✗ TEST FAILED: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit(main())