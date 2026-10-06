#!/usr/bin/env python3
"""
Test script to verify NLM Python bindings after improvements
"""

import sys
import pynlm
import json

def test_basic_bindings():
    print("Testing basic NLM Python bindings...")
    
    # Test Config class
    print("1. Testing Config class...")
    config = pynlm.createDefaultConfig()
    
    # Test basic config methods
    config.setInt("neuron_count", 500)
    config.setFloat("connection_probability", 0.2)
    config.setBool("plasticity.stdp.enable", True)
    
    assert config.has("neuron_count"), "Config.has() should work"
    assert config.getInt("neuron_count") == 500, "getInt() should return correct value"
    assert config.getFloat("connection_probability") == 0.2, "getFloat() should return correct value"
    assert config.getBool("plasticity.stdp.enable") == True, "getBool() should return correct value"
    
    print("   ✓ Config class tests passed")
    
    # Test Brain creation
    print("2. Testing Brain creation...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() == 500, "Should create correct number of neurons"
    assert brain.getRegionCount() == 1, "Should create one region by default"
    
    print("   ✓ Brain creation tests passed")
    
    # Test basic brain operations
    print("3. Testing basic brain operations...")
    brain.step(0)
    
    total_spikes = brain.getTotalSpikeCount()
    firing_neurons = brain.getFiringNeuronCount()
    avg_firing = brain.getAverageFiringRate()
    
    assert isinstance(total_spikes, int), "getTotalSpikeCount() should return int"
    assert isinstance(firing_neurons, int), "getFiringNeuronCount() should return int"
    assert isinstance(avg_firing, float), "getAverageFiringRate() should return float"
    
    print(f"   ✓ Basic operations: {total_spikes} spikes, {firing_neurons} firing neurons")
    
    # Test Action class
    print("4. Testing Action class...")
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action.getType() == pynlm.ActionType.MoveForward, "Action type should be preserved"
    
    action.setType(pynlm.ActionType.TurnLeft)
    assert action.getType() == pynlm.ActionType.TurnLeft, "Action type should be changeable"
    
    action_name = action.getName()
    assert isinstance(action_name, str), "getName() should return string"
    
    print(f"   ✓ Action class: {action_name}")
    
    # Test AgentBrain class
    print("5. Testing AgentBrain class...")
    agent = pynlm.createAgentBrain(brain)
    
    # Test enable/disable methods
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled() == True, "isRewardModulationEnabled() should work"
    assert agent.isCuriosityEnabled() == True, "isCuriosityEnabled() should work"
    
    print("   ✓ AgentBrain enable/disable methods work")
    
    # Test SimpleWorld class
    print("6. Testing SimpleWorld class...")
    world = pynlm.createSimpleWorld()
    
    world.configure(10, 10, 8, 8)
    assert world.getWidth() == 10.0, "getWidth() should return correct value"
    assert world.getHeight() == 10.0, "getHeight() should return correct value"
    
    world.reset()
    world.setAgentStart(5.0, 5.0)
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    assert percept is not None, "getSensoryPercept() should return valid percept"
    
    print("   ✓ SimpleWorld class tests passed")
    
    # Test convenience functions
    print("7. Testing convenience functions...")
    
    # Test run_simulation
    stats = pynlm.run_simulation(brain, 100, verbose=False)
    assert len(stats) == 3, "run_simulation should return tuple of 3 values"
    
    # Test get_brain_stats
    brain_json = pynlm.get_brain_stats(brain)
    stats_dict = json.loads(brain_json)
    assert "total_neurons" in stats_dict, "brain_json should contain total_neurons"
    assert "total_spikes" in stats_dict, "brain_json should contain total_spikes"
    
    # Test create_experiment_config
    exp_config = pynlm.create_experiment_config("test_experiment")
    exp_config.setBool("neuromodulation.reward.enable", True)
    
    print("   ✓ Convenience functions work")
    
    # Test get_version_info
    print("8. Testing version info...")
    version_info = pynlm.get_version_info()
    version_dict = json.loads(version_info)
    
    assert "name" in version_dict, "version_info should contain name"
    assert "version" in version_dict, "version_info should contain version"
    assert "phase" in version_dict, "version_info should contain phase"
    
    print(f"   ✓ Version: {version_dict['name']} {version_dict['version']}")
    
    # Test validation
    print("9. Testing configuration validation...")
    validation = pynlm.validate_config(config)
    validation_dict = json.loads(validation)
    
    assert "valid" in validation_dict, "validation should contain 'valid' field"
    assert "errors" in validation_dict, "validation should contain 'errors' field"
    
    print("   ✓ Configuration validation works")
    
    # Test action methods
    print("10. Testing additional Action methods...")
    action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0, 3.0])
    assert len(action.getParameters()) == 3, "getParameters() should return all parameters"
    
    action.setParameters([0.5, 0.5, 0.5])
    params = action.getParameters()
    assert params[0] == 0.5, "setParameters() should update parameters"
    
    print("   ✓ Additional Action methods work")
    
    print("\n✅ All tests passed! NLM Python bindings are working correctly.")
    return True

if __name__ == "__main__":
    try:
        success = test_basic_bindings()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)