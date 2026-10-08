#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings (pynlm)
Tests all functionality of the refactored modular bindings
"""

import pynlm
import sys
import os

def test_version():
    """Test version function"""
    print("Testing version...")
    version = pynlm.version()
    assert isinstance(version, str), f"Version should be string, got {type(version)}"
    assert version == "0.1.0", f"Expected version 0.1.0, got {version}"
    print("✓ Version test passed")

def test_quick_start():
    """Test quick_start guide"""
    print("Testing quick_start guide...")
    guide = pynlm.quick_start()
    assert isinstance(guide, str), f"Guide should be string, got {type(guide)}"
    assert "Quick Start Guide" in guide, "Guide should contain 'Quick Start Guide'"
    print("✓ Quick start guide test passed")

def test_id_types():
    """Test ID type bindings (eliminates boilerplate)"""
    print("Testing ID type bindings...")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(123)
    assert neuron_id.value == 123
    assert neuron_id.index() == 123
    assert repr(neuron_id) == "<NeuronId: 123>"
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(456)
    assert synapse_id.value == 456
    assert synapse_id.index() == 456
    assert repr(synapse_id) == "<SynapseId: 456>"
    
    # Test RegionId
    region_id = pynlm.RegionId(789)
    assert region_id.value == 789
    assert region_id.index() == 789
    assert repr(region_id) == "<RegionId: 789>"
    
    # Test PopulationId
    population_id = pynlm.PopulationId(999)
    assert population_id.value == 999
    assert population_id.index() == 999
    assert repr(population_id) == "<PopulationId: 999>"
    
    # Test invalid IDs
    assert pynlm.INVALID_NEURON_ID.value == 18446744073709551615
    assert pynlm.INVALID_SYNAPSE_ID.value == 18446744073709551615
    assert pynlm.INVALID_REGION_ID.value == 18446744073709551615
    assert pynlm.INVALID_POPULATION_ID.value == 18446744073709551615
    
    # Test equality operators
    assert neuron_id == pynlm.NeuronId(123)
    assert neuron_id != pynlm.NeuronId(456)
    assert neuron_id != synapse_id
    
    print("✓ ID type bindings test passed")

def test_enums():
    """Test enum bindings"""
    print("Testing enum bindings...")
    
    # Test NeuronType enum
    assert pynlm.NeuronType.Excitatory.value == 0
    assert pynlm.NeuronType.Inhibitory.value == 1
    assert pynlm.NeuronType.Sensory.value == 4
    
    # Test SynapseType enum
    assert pynlm.SynapseType.Excitatory.value == 0
    assert pynlm.SynapseType.Electrical.value == 3
    
    # Test DevelopmentalStage enum
    assert pynlm.DevelopmentalStage.Initial.value == 0
    assert pynlm.DevelopmentalStage.Aging.value == 4
    
    # Test FiringState enum
    assert pynlm.FiringState.Resting.value == 0
    assert pynlm.FiringState.Inhibited.value == 3
    
    # Test ActionType enum
    assert pynlm.ActionType.MoveForward.value == 0
    assert pynlm.ActionType.Interact.value == 9
    assert pynlm.ActionType.Wait.value == 13
    
    # Test MotorCommand enum
    assert pynlm.MotorCommand.MoveForward.value == 0
    assert pynlm.MotorCommand.Interact.value == 6
    
    # Test WorldObjectType enum
    assert pynlm.WorldObjectType.Empty.value == 0
    assert pynlm.WorldObjectType.Resource.value == 1
    assert pynlm.WorldObjectType.Hazard.value == 2
    
    print("✓ Enum bindings test passed")

def test_config_class():
    """Test Config class"""
    print("Testing Config class...")
    
    # Test createDefaultConfig
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test default values
    assert not config.has("brain.neuron_count")
    assert not config.has("plasticity.stdp.enable")
    
    # Test getting keys
    keys = config.getKeys()
    assert isinstance(keys, list)
    assert "random_seed" in keys
    
    # Test setting values
    config.set("brain.neuron_count", 2000)
    assert config.has("brain.neuron_count")
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert "Configuration" in summary
    
    # Test repr
    repr_str = repr(config)
    assert "<Config:" in repr_str
    assert "Configuration" in repr_str
    
    print("✓ Config class test passed")

def test_action_class():
    """Test Action class"""
    print("Testing Action class...")
    
    # Test default constructor
    action = pynlm.Action()
    assert action.getType() is None
    
    # Test constructor with type
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action.getType() == pynlm.ActionType.MoveForward
    
    # Test constructor with type and parameters
    params = [0.5, 0.3, 0.8]
    action = pynlm.Action(pynlm.ActionType.Eat, params)
    assert action.getType() == pynlm.ActionType.Eat
    assert action.getParameters() == params
    
    # Test set/get methods
    action.setType(pynlm.ActionType.TurnLeft)
    assert action.getType() == pynlm.ActionType.TurnLeft
    
    new_params = [0.1, 0.2]
    action.setParameters(new_params)
    assert action.getParameters() == new_params
    
    # Test getName method
    assert action.getName() == "TurnLeft"
    
    # Test clone
    cloned = action.clone()
    assert cloned.getType() == action.getType()
    assert cloned.getParameters() == action.getParameters()
    assert cloned is not action  # Different objects
    
    print("✓ Action class test passed")

def test_sensory_classes():
    """Test sensory input classes"""
    print("Testing sensory classes...")
    
    # Test SensoryInput base class
    # Note: SensoryInput is abstract, can't instantiate directly
    # We'll test its subclasses instead
    
    # Test Vision
    vision = pynlm.Vision(100, 80, 3)
    assert vision.getWidth() == 100
    assert vision.getHeight() == 80
    assert vision.getChannels() == 3
    assert vision.getType() is not None
    
    vision.setData([0.1, 0.2, 0.3])
    assert vision.getData() == [0.1, 0.2, 0.3]
    
    # Test Audio
    audio = pynlm.Audio(44100, 1024)
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 1024
    
    audio.setData([0.5] * 1024)
    assert audio.getData() == [0.5] * 1024
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    internal.addSignal(1.0)
    internal.addSignal(2.5)
    assert len(internal.getData()) == 2
    
    signals = internal.getData()
    assert signals[0] == 1.0
    assert signals[1] == 2.5
    
    print("✓ Sensory classes test passed")

def test_world_classes():
    """Test world and agent state classes"""
    print("Testing world classes...")
    
    # Test WorldObject
    obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 1.5)
    assert obj.x == 10.0
    assert obj.y == 20.0
    assert obj.type == pynlm.WorldObjectType.Resource
    assert obj.value == 1.5
    assert obj.active is True
    
    # Test AgentBody
    body = pynlm.AgentBody()
    assert body.x == 0.0
    assert body.y == 0.0
    assert body.orientation == 0.0
    assert body.energy == 100.0  # Default value
    
    # Test ActionResult
    result = pynlm.ActionResult(1.0, True, "Success")
    assert result.reward == 1.0
    assert result.success is True
    assert result.message == "Success"
    
    # Test SimpleWorld
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    world.configure(50, 50, 20, 20)
    assert world.getWidth() == 50
    assert world.getHeight() == 50
    
    world.reset()
    assert world.getSimulationTime() == 0.0
    
    world.setAgentStart(25.0, 25.0)
    
    # Test agent body
    body = world.getAgentBody()
    assert body.x == 25.0
    assert body.y == 25.0
    
    print("✓ World classes test passed")

def test_brain_classes():
    """Test brain and agent interface classes"""
    print("Testing brain classes...")
    
    # Test createBrain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test initialize
    brain.initialize()
    assert brain.getTotalNeuronCount() == 1000  # Default neuron count
    
    # Test basic brain operations
    brain.step(0)
    brain.step(1, 0.001)  # With timestamp
    
    # Test statistics
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    
    # Test developmental stage
    assert brain.getDevelopmentalStage() in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    # Test AgentBrain
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    # Test createSimpleWorld for agent
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    
    # Test agent statistics
    assert agent.getCuriosityLevel() >= 0.0
    assert agent.getNoveltyLevel() >= 0.0
    assert agent.getNeuromodulationLevel() >= 0.0
    
    # Test enable/disable functions
    agent.enableRewardModulation(True)
    assert agent.isRewardModulationEnabled() is True
    
    agent.enableStructuralPlasticity(False)
    assert agent.isStructuralPlasticityEnabled() is False
    
    agent.enableDevelopment(True)
    assert agent.isDevelopmentEnabled() is True
    
    agent.enableCuriosity(True)
    assert agent.isCuriosityEnabled() is True
    
    print("✓ Brain classes test passed")

def test_factory_functions():
    """Test factory functions"""
    print("Testing factory functions...")
    
    # Test createDefaultConfig
    config = pynlm.createDefaultConfig()
    assert config is not None
    assert isinstance(config, pynlm.Config)
    
    # Test createBrain
    brain = pynlm.createBrain(config)
    assert brain is not None
    assert isinstance(brain, pynlm.Brain)
    
    # Test createSimpleWorld
    world = pynlm.createSimpleWorld()
    assert world is not None
    assert isinstance(world, pynlm.SimpleWorld)
    
    # Test createAgentBrain
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    assert isinstance(agent, pynlm.AgentBrain)
    
    print("✓ Factory functions test passed")

def test_basic_usage_pattern():
    """Test the basic usage pattern from easy_usage.md"""
    print("Testing basic usage pattern...")
    
    # Test Simplest Possible Example from easy_usage.md
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.step(0)
    
    assert brain.getTotalNeuronCount() > 0
    print(f"✓ Simplest example: {brain.getTotalNeuronCount()} neurons created")
    
    # Test example from easy_usage.md
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    print(f"✓ Basic simulation: {brain.getTotalSpikeCount()} spikes")
    
    # Test complete agent example from easy_usage.md
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    assert brain.getFiringNeuronCount() >= 0
    print(f"✓ Complete agent example: {brain.getFiringNeuronCount()} neurons firing")
    
    print("✓ Basic usage pattern test passed")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_version,
        test_quick_start,
        test_id_types,
        test_enums,
        test_config_class,
        test_action_class,
        test_sensory_classes,
        test_world_classes,
        test_brain_classes,
        test_factory_functions,
        test_basic_usage_pattern,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("Some tests failed. Please check the output above.")
        return False
    else:
        print("All tests passed! ✓")
        return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)