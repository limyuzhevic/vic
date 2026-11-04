"""
Test suite for NLM Python bindings.

This comprehensive test suite validates the enhanced Python bindings
for the NLM (Neural Learning Machine) framework.
"""

import pynlm
import tempfile
import os
import sys
from typing import Dict, Any

def test_import_and_basic_creation():
    """Test basic import and object creation."""
    print("Testing basic import and creation...")
    
    # Test import
    import pynlm
    assert hasattr(pynlm, 'createBrain'), "createBrain function not available"
    assert hasattr(pynlm, 'createDefaultConfig'), "createDefaultConfig function not available"
    assert hasattr(pynlm, 'createSimpleWorld'), "createSimpleWorld function not available"
    assert hasattr(pynlm, 'createAgentBrain'), "createAgentBrain function not available"
    
    # Test constants
    assert hasattr(pynlm, 'INVALID_NEURON_ID'), "INVALID_NEURON_ID constant not available"
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID'), "INVALID_SYNAPSE_ID constant not available"
    assert hasattr(pynlm, 'INVALID_REGION_ID'), "INVALID_REGION_ID constant not available"
    assert hasattr(pynlm, 'INVALID_POPULATION_ID'), "INVALID_POPULATION_ID constant not available"
    
    # Test enum types
    assert hasattr(pynlm, 'NeuronType'), "NeuronType enum not available"
    assert hasattr(pynlm, 'ActionType'), "ActionType enum not available"
    assert hasattr(pynlm, 'DevelopmentalStage'), "DevelopmentalStage enum not available"
    assert hasattr(pynlm, 'WorldObjectType'), "WorldObjectType enum not available"
    
    print("✅ Basic import and creation tests passed!")

def test_config_creation_and_usage():
    """Test configuration creation and usage."""
    print("Testing config creation and usage...")
    
    # Create default config
    config = pynlm.createDefaultConfig()
    assert config is not None, "Failed to create default config"
    
    # Test basic config operations
    config.set("test.key", "test_value")
    assert config.has("test.key"), "Config should have the key we just set"
    
    # Test type-safe getters
    config.set("int_value", 42)
    config.set("float_value", 3.14)
    config.set("bool_value", True)
    config.set("string_value", "hello")
    
    # Test typed getters with defaults
    assert config.get_int("int_value", 0) == 42
    assert config.get_int("missing_key", 99) == 99
    assert config.get_float("float_value", 0.0) == 3.14
    assert config.get_bool("bool_value", False) == True
    assert config.get_string("string_value", "") == "hello"
    
    # Test file operations
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
    
    try:
        # Save config
        config.saveToFile(temp_file)
        assert os.path.exists(temp_file), "Config file should exist after save"
        
        # Load config
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(temp_file)
        assert new_config.has("test.key"), "Loaded config should have the test key"
        
    finally:
        # Cleanup
        if os.path.exists(temp_file):
            os.unlink(temp_file)
    
    print("✅ Config creation and usage tests passed!")

def test_brain_creation_and_initialization():
    """Test brain creation and initialization."""
    print("Testing brain creation and initialization...")
    
    # Create config and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None, "Failed to create brain"
    
    # Test initialization
    brain.initialize()
    
    # Test basic brain properties
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    assert brain.getRegionCount() >= 1, "Brain should have at least one region"
    
    # Test brain operations
    brain.step(0)
    brain.reset()
    
    # Test region operations
    region_id = brain.addRegion("test_region")
    region = brain.getRegion(region_id)
    assert region is not None, "Should be able to get region by ID"
    
    print("✅ Brain creation and initialization tests passed!")

def test_world_creation_and_usage():
    """Test world creation and usage."""
    print("Testing world creation and usage...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    assert world is not None, "Failed to create world"
    
    # Configure world
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Test world properties
    assert world.getWidth() == 10, "World width should be 10"
    assert world.getHeight() == 10, "World height should be 10"
    assert world.getMaxEnergy() > 0, "World should have positive max energy"
    
    # Test world operations
    world.setAgentStart(5.0, 5.0)
    world.setRandomSeed(42)
    world.update(0.1)
    
    # Test object operations
    obj = pynlm.WorldObject(1.0, 1.0, pynlm.WorldObjectType.Resource, 1.0)
    world.addObject(obj)
    world.removeObject(1.0, 1.0)
    
    # Test agent body
    body = world.getAgentBody()
    assert body is not None, "Should be able to get agent body"
    
    # Test sensory percept
    percept = world.getSensoryPercept()
    assert percept is not None, "Should be able to get sensory percept"
    
    print("✅ World creation and usage tests passed!")

def test_agent_creation_and_usage():
    """Test agent creation and usage."""
    print("Testing agent creation and usage...")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "Failed to create agent"
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Test agent properties
    assert agent.getSensoryInputSize() > 0, "Agent should expect sensory input"
    assert agent.getMotorOutputSize() > 0, "Agent should expect motor output"
    
    # Test agent operations
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    motor_cmd = agent.decodeMotorCommand()
    assert motor_cmd is not None, "Should be able to decode motor command"
    
    # Test neuromodulation
    agent.applyRewardModulation(0.5, 0.3)
    
    # Test development
    agent.updateDevelopment(0.1)
    
    # Test feature enable/disable
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(False)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Test feature queries
    assert agent.isRewardModulationEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == False
    assert agent.isDevelopmentEnabled() == True
    assert agent.isCuriosityEnabled() == True
    
    # Test statistics
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    neuromodulation = agent.getNeuromodulationLevel()
    
    assert curiosity is not None
    assert novelty is not None
    assert prediction_error is not None
    assert neuromodulation is not None
    
    # Test developmental stage
    stage = agent.getDevelopmentalStage()
    assert stage is not None
    
    print("✅ Agent creation and usage tests passed!")

def test_complete_simulation():
    """Test a complete simulation with all components."""
    print("Testing complete simulation...")
    
    # Setup configuration
    config = pynlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create and initialize agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    num_steps = 100
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal() and percept.getInternal()[0] > 0:
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 20 steps
        if step % 20 == 0:
            stats = get_brain_stats(brain, agent)
            print(f"Step {step}: {stats['neurons']['firing']} neurons firing, "
                  f"Curiosity: {stats['agent']['curiosity_level']:.2f}, "
                  f"Dev: {stats['neurons']['developmental_stage']}")
    
    # Final statistics
    final_stats = get_brain_stats(brain, agent)
    assert final_stats['neurons']['total'] > 0, "Should have neurons"
    assert final_stats['agent']['curiosity_level'] >= 0, "Curiosity level should be non-negative"
    
    print(f"✅ Complete simulation test passed! Final firing rate: {final_stats['neurons']['average_firing_rate']:.2f}")

def test_error_handling():
    """Test error handling and edge cases."""
    print("Testing error handling...")
    
    # Test that brain initialization is required
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # This should not crash (though behavior may be undefined)
    try:
        brain.step(0)
        print("⚠️  Warning: brain.step() without initialization may not be safe")
    except Exception as e:
        print(f"✅ Expected error when calling brain.step() without initialization: {e}")
    
    # Test config error handling
    config = pynlm.createDefaultConfig()
    config.set("invalid_key", None)  # Should handle gracefully
    
    print("✅ Error handling tests passed!")

def get_brain_stats(brain, agent=None):
    """Get comprehensive statistics about brain and agent state."""
    stats = {
        'neurons': {
            'total': brain.getTotalNeuronCount(),
            'active': brain.getActiveNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'region_count': brain.getRegionCount(),
            'developmental_stage': str(brain.getDevelopmentalStage())
        }
    }
    
    if agent:
        stats['agent'] = {
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'developmental_stage': str(agent.getDevelopmentalStage()),
            'enabled_features': {
                'reward_modulation': agent.isRewardModulationEnabled(),
                'structural_plasticity': agent.isStructuralPlasticityEnabled(),
                'development': agent.isDevelopmentEnabled(),
                'curiosity': agent.isCuriosityEnabled()
            }
        }
    
    return stats

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_import_and_basic_creation,
        test_config_creation_and_usage,
        test_brain_creation_and_initialization,
        test_world_creation_and_usage,
        test_agent_creation_and_usage,
        test_complete_simulation,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            print(f"\n{'='*60}")
            print(f"Running: {test.__name__}")
            print(f"{'='*60}")
            test()
            passed += 1
            print(f"✅ {test.__name__} PASSED")
        except Exception as e:
            failed += 1
            print(f"❌ {test.__name__} FAILED: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("Test Results Summary")
    print("=" * 60)
    print(f"✅ Passed: {passed}")
    print(f"❌ Failed: {failed}")
    print(f"📊 Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! The enhanced Python bindings are working correctly.")
        return True
    else:
        print(f"\n⚠️  {failed} test(s) failed. Please check the output above.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)