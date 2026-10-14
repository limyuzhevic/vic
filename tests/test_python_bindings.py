#!/usr/bin/env python3
"""
Comprehensive Python binding tests for NLM (Neural Learning Machine)

This test suite validates the enhanced Python API with power user utilities.
Tests all new functionality added to bindings.cpp.
"""

import sys
import os
import tempfile
import json

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm

def test_basic_functionality():
    """Test basic Python API functionality"""
    print("Testing basic functionality...")
    
    # Test basic brain creation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic operations
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    
    # Test brain step
    brain.step(0)
    brain.step(1, 0.001)
    
    print("  ✓ Basic functionality passed")

def test_enhanced_config_api():
    """Test enhanced configuration API"""
    print("Testing enhanced configuration API...")
    
    # Test default config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test getOr methods with defaults
    neurons = config.getOr("brain.neuron_count", 1000)
    assert neurons == 1000  # default value
    
    # Test set methods
    config.set("brain.neuron_count", 2000, pynlm.ConfigSource.Runtime)
    neurons = config.getOr("brain.neuron_count", 1000)
    assert neurons == 2000
    
    # Test type-specific getOr methods
    config.set("test.float_value", 3.14, pynlm.ConfigSource.Runtime)
    config.set("test.string_value", "hello", pynlm.ConfigSource.Runtime)
    config.set("test.bool_value", True, pynlm.ConfigSource.Runtime)
    config.set("test.int_value", 42, pynlm.ConfigSource.Runtime)
    
    assert config.getOrFloat("test.float_value", 0.0) == 3.14
    assert config.getOrString("test.string_value", "") == "hello"
    assert config.getOrBool("test.bool_value", False) == True
    assert config.getOrInt("test.int_value", 0) == 42
    
    # Test config validation
    errors = config.validate()
    assert isinstance(errors, dict)
    
    # Test export/import
    exported = config.export()
    assert isinstance(exported, dict)
    assert "brain.neuron_count" in exported
    
    # Create new config and import
    config2 = pynlm.createDefaultConfig()
    config2.import(exported)
    assert config2.getOr("brain.neuron_count", 0) == 2000
    
    print("  ✓ Enhanced configuration API passed")

def test_brain_enhancements():
    """Test enhanced Brain class methods"""
    print("Testing enhanced Brain class methods...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test new Brain methods
    spike_system = brain.getSpikeSystem()
    assert spike_system is not None
    
    stdp = brain.getSTDP()
    assert stdp is not None
    
    hebbian = brain.getHebbian()
    assert hebbian is not None
    
    structural = brain.getStructuralPlasticity()
    assert structural is not None
    
    # Test plasticity control methods
    brain.updatePlasticity()
    brain.develop()
    
    # Test development stage
    stage = brain.getDevelopmentalStage()
    assert stage in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
    assert brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Adult
    
    # Test statistics
    stats = {
        "neuron_count": brain.getTotalNeuronCount(),
        "synapse_count": brain.getTotalSynapseCount(),
        "active_count": brain.getActiveNeuronCount(),
        "firing_count": brain.getFiringNeuronCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "total_spikes": brain.getTotalSpikeCount(),
        "pending_spike_events": brain.getPendingSpikeEventCount()
    }
    
    assert stats["neuron_count"] > 0
    assert stats["total_spikes"] >= 0
    assert 0.0 <= stats["average_firing_rate"] <= 1000.0
    assert 0.0 <= stats["excitation_inhibition_ratio"] <= 100.0
    
    print("  ✓ Enhanced Brain class methods passed")

def test_agentbrain_enhancements():
    """Test enhanced AgentBrain class methods"""
    print("Testing enhanced AgentBrain class methods...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test new AgentBrain methods
    sensory_input_size = agent.getSensoryInputSize()
    assert sensory_input_size > 0
    
    motor_output_size = agent.getMotorOutputSize()
    assert motor_output_size > 0
    
    # Test system activation
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Verify activation
    assert agent.isRewardModulationEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == True
    assert agent.isDevelopmentEnabled() == True
    assert agent.isCuriosityEnabled() == True
    
    # Test advanced methods
    metrics = agent.getAllMetrics()
    assert isinstance(metrics, dict)
    assert "neuromodulation" in metrics
    assert "curiosity" in metrics
    assert "novelty" in metrics
    assert "prediction_error" in metrics
    assert "development_stage" in metrics
    
    # Test world interaction methods
    brain_state = agent.getWorldSensors(world)
    assert isinstance(brain_state, dict)
    
    agent_state = agent.getWorldInfo(world)
    assert isinstance(agent_state, dict)
    assert "x" in agent_state
    assert "y" in agent_state
    assert "energy" in agent_state
    
    # Test development
    agent.updateDevelopment(0.1)
    dev_stage = agent.getDevelopmentalStage()
    assert dev_stage in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    print("  ✓ Enhanced AgentBrain class methods passed")

def test_world_enhancements():
    """Test enhanced SimpleWorld class methods"""
    print("Testing enhanced SimpleWorld class methods...")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Test new world methods
    width = world.getWidth()
    height = world.getHeight()
    assert width == 30.0
    assert height == 30.0
    
    # Test energy settings
    max_energy = world.getMaxEnergy()
    assert max_energy > 0.0
    
    world.setMaxEnergy(200.0)
    assert world.getMaxEnergy() == 200.0
    
    world.setEnergyDecayRate(0.5)
    assert world.getEnergyDecayRate() == 0.5
    
    # Test random seed
    seed = world.getRandomSeed()
    assert seed >= 0
    
    world.setRandomSeed(12345)
    assert world.getRandomSeed() == 12345
    
    # Test advanced world methods
    objects = world.getWorldObjects()
    assert isinstance(objects, list)
    
    resources = world.getObjectsByType(pynlm.WorldObjectType.Resource)
    assert isinstance(resources, list)
    
    count = world.countObjects()
    assert count >= 0
    
    objects_in_range = world.getObjectsInRange(15.0, 15.0, 10.0)
    assert isinstance(objects_in_range, list)
    
    # Test export/import
    exported = world.exportObjects()
    assert isinstance(exported, list)
    
    world2 = pynlm.createSimpleWorld()
    world2.importObjects(exported)
    # Should have same number of objects
    assert world2.countObjects() == world.countObjects()
    
    # Test advanced data access
    all_sensors = world.getAllSensoryData()
    assert isinstance(all_sensors, dict)
    assert "vision" in all_sensors
    assert "touch" in all_sensors
    
    agent_state = world.getAgentState()
    assert isinstance(agent_state, dict)
    assert "x" in agent_state
    assert "y" in agent_state
    
    # Test world statistics
    stats = world.getWorldStats()
    assert isinstance(stats, dict)
    assert "width" in stats
    assert "height" in stats
    assert "max_energy" in stats
    
    print("  ✓ Enhanced SimpleWorld class methods passed")

def test_module_utilities():
    """Test module-level utility functions"""
    print("Testing module-level utility functions...")
    
    # Test complex config creation
    complex_config = pynlm.createComplexConfig()
    assert complex_config is not None
    
    neurons = complex_config.getOr("brain.neuron_count", 1000)
    assert neurons == 2000  # value set by complex config
    
    # Test agent creation with world
    config = pynlm.createDefaultConfig()
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    brain, agent, world_obj = pynlm.createCompleteSetup(config, world)
    
    assert brain is not None
    assert agent is not None
    assert world_obj is not None
    
    # Verify everything is connected
    assert agent.getBrain() is brain
    assert brain.getConfig() is not None
    
    # Test validation
    errors = pynlm.validateConfig(config)
    assert isinstance(errors, dict)
    
    # Test serialization helpers
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
        filepath = f.name
    
    try:
        # Save brain
        success = pynlm.serializeBrain(brain, filepath)
        assert success == True
        
        # Load brain
        loaded_brain = pynlm.deserializeBrain(filepath)
        assert loaded_brain is not None
        assert loaded_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
        
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    # Test build info
    build_info = pynlm.getBuildInfo()
    assert isinstance(build_info, dict)
    assert "version" in build_info
    assert "api_enhanced" in build_info
    assert build_info["api_enhanced"] == "true"
    
    print("  ✓ Module-level utility functions passed")

def test_integration():
    """Test complete integration scenario"""
    print("Testing complete integration...")
    
    # Create complex setup
    config = pynlm.createComplexConfig()
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Create complete agent setup
    brain, agent, world_obj = pynlm.createCompleteSetup(config, world)
    
    # Run simulation
    for step in range(100):
        world_obj.update(0.1)
        
        # Get percept and process
        percept = world_obj.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and apply
        motor_cmd = agent.decodeMotorCommand()
        world_obj.applyMotorCommand(motor_cmd, world_obj.getSimulationTime())
        
        # Apply reward
        reward = 1.0 if step % 10 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Check for completion
        if step % 20 == 0:
            print(f"    Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # Verify final state
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    assert brain.getTotalSpikeCount() >= 0
    
    # Get metrics
    metrics = agent.getAllMetrics()
    assert isinstance(metrics, dict)
    
    # Get world info
    world_info = agent.getWorldInfo(world_obj)
    assert isinstance(world_info, dict)
    
    print("  ✓ Complete integration test passed")

def test_error_handling():
    """Test error handling and edge cases"""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test that methods don't crash on edge cases
    try:
        # These should not crash even if called multiple times
        brain.step(0)
        brain.step(1, 0.001)
        brain.step(2, 0.002)
        
        # Test validation with empty config
        empty_config = pynlm.createDefaultConfig()
        empty_config.clear()
        errors = empty_config.validate()
        assert isinstance(errors, dict)
        
        # Test world with no objects
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        world_state = world.getWorldStats()
        assert isinstance(world_state, dict)
        
    except Exception as e:
        print(f"  ✗ Error handling test failed: {e}")
        raise
    
    print("  ✓ Error handling passed")

def run_all_tests():
    """Run all test suites"""
    print("=" * 60)
    print("NLM Python Binding Tests - Enhanced API")
    print("=" * 60)
    
    test_functions = [
        test_basic_functionality,
        test_enhanced_config_api,
        test_brain_enhancements,
        test_agentbrain_enhancements,
        test_world_enhancements,
        test_module_utilities,
        test_integration,
        test_error_handling
    ]
    
    passed = 0
    failed = 0
    
    for test_func in test_functions:
        try:
            test_func()
            passed += 1
        except Exception as e:
            print(f"  ✗ {test_func.__name__} failed: {e}")
            failed += 1
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print(f"Test Summary: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("✓ All tests passed!")
        return True
    else:
        print(f"✗ {failed} test(s) failed")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)