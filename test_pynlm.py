# Python Bindings Test Suite

# Comprehensive test suite for pynlm Python bindings

import sys
import os
import tempfile
import shutil
import json
import traceback
from pathlib import Path

# Add the module to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_version():
    """Test module version information"""
    print("Testing version information...")
    assert hasattr(pynlm, '__version__'), "Module should have __version__ attribute"
    assert hasattr(pynlm, '__author__'), "Module should have __author__ attribute"
    assert hasattr(pynlm, '__license__'), "Module should have __license__ attribute"
    assert hasattr(pynlm, '__description__'), "Module should have __description__ attribute"
    
    print(f"  Version: {pynlm.__version__}")
    print(f"  Author: {pynlm.__author__}")
    print(f"  License: {pynlm.__license__}")
    print(f"  Description: {pynlm.__description__}")
    print("✓ Version test passed\n")

def test_config():
    """Test configuration class"""
    print("Testing configuration class...")
    
    # Test creating default config
    config = pynlm.createDefaultConfig()
    assert config is not None, "Should create default config"
    
    # Test basic config operations
    config.set("test.key", "test_value", pynlm.ConfigSource.Runtime)
    assert config.has("test.key"), "Should have the key we just set"
    
    # Test getting values
    value = config.get("test.key")
    assert value == "test_value", f"Expected 'test_value', got {value}"
    
    # Test get or with default
    default_value = config.getOr("nonexistent.key", "default_value")
    assert default_value == "default_value", f"Expected 'default_value', got {default_value}"
    
    # Test getting all keys
    keys = config.getKeys()
    assert "test.key" in keys, "Should contain the key we set"
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str), "Summary should be a string"
    assert len(summary) > 0, "Summary should not be empty"
    
    # Test file operations
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        
        # Load from file
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(f.name)
        assert config2.has("test.key"), "Should load key from file"
        
        # Clean up
        os.unlink(f.name)
    
    # Test clearing
    config.clear()
    assert not config.has("test.key"), "Should clear all keys"
    
    print("✓ Configuration test passed\n")

def test_brain_basic():
    """Test basic brain functionality"""
    print("Testing basic brain functionality...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic properties
    assert brain.getTotalNeuronCount() > 0, "Should have neurons"
    assert brain.getTotalSynapseCount() >= 0, "Should have synapses"
    assert brain.getFiringNeuronCount() >= 0, "Should have firing neurons"
    assert brain.getActiveNeuronCount() >= 0, "Should have active neurons"
    assert brain.getAverageFiringRate() >= 0.0, "Should have firing rate"
    assert brain.getExcitationInhibitionRatio() >= 0.0, "Should have E/I ratio"
    assert brain.getTotalSpikeCount() >= 0, "Should have total spikes"
    
    # Test developmental stage
    assert brain.getDevelopmentalStage() in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ], "Should have valid developmental stage"
    
    # Test simulation steps
    for step in range(10):
        brain.step(step)
        assert brain.getTotalNeuronCount() == 100, "Should maintain neuron count"
    
    # Test reset
    brain.reset()
    assert brain.getTotalNeuronCount() == 100, "Should maintain neuron count after reset"
    
    print("✓ Basic brain test passed\n")

def test_brain_advanced():
    """Test advanced brain functionality"""
    print("Testing advanced brain functionality...")
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500, pynlm.ConfigSource.Default)
    config.set("brain.synapse_density", 0.2, pynlm.ConfigSource.Default)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test advanced properties
    assert brain.getTotalNeuronCount() == 500, "Should have custom neuron count"
    assert brain.getTotalSynapseCount() > 0, "Should have synapses"
    
    # Test region management
    region_id = brain.addRegion("cortex")
    assert region_id is not None, "Should create region"
    
    region = brain.getRegion(region_id)
    assert region is not None, "Should get region"
    
    region_ids = brain.getRegionIds()
    assert region_id in region_ids, "Should contain created region"
    
    # Test region count
    assert brain.getRegionCount() == 1, "Should have one region"
    
    # Test getting all regions
    regions = brain.getRegions()
    assert len(regions) == 1, "Should have one region in list"
    
    # Test developmental stage changes
    original_stage = brain.getDevelopmentalStage()
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
    assert brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod, "Should change stage"
    
    # Test save/load
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        filepath = f.name
    
    try:
        success = brain.save(filepath)
        assert success, "Should save brain state"
        assert os.path.getsize(filepath) > 0, "Saved file should not be empty"
        
        # Create new brain and load
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        success = brain2.load(filepath)
        assert success, "Should load brain state"
        
        assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount(), "Should preserve neuron count"
        
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    print("✓ Advanced brain test passed\n")

def test_agent():
    """Test agent functionality"""
    print("Testing agent functionality...")
    
    # Create agent setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent properties
    assert agent.getSensoryInputSize() > 0, "Should have sensory input size"
    assert agent.getMotorOutputSize() > 0, "Should have motor output size"
    
    # Test enable/disable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled(), "Should enable reward modulation"
    assert agent.isStructuralPlasticityEnabled(), "Should enable structural plasticity"
    assert agent.isDevelopmentEnabled(), "Should enable development"
    assert agent.isCuriosityEnabled(), "Should enable curiosity"
    
    # Test neuromodulation levels
    agent.applyRewardModulation(0.5, 0.3)
    assert agent.getNeuromodulationLevel() >= 0.0, "Should have neuromodulation level"
    assert agent.getCuriosityLevel() >= 0.0, "Should have curiosity level"
    assert agent.getNoveltyLevel() >= 0.0, "Should have novelty level"
    assert agent.getPredictionError() >= 0.0, "Should have prediction error"
    
    # Test development
    agent.updateDevelopment(0.1)
    stage = agent.getDevelopmentalStage()
    assert stage in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ], "Should have valid developmental stage"
    
    # Test process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test decode motor command
    action = agent.decodeMotorCommand()
    assert action is not None, "Should decode motor command"
    
    # Test reset
    agent.reset()
    assert agent.getCuriosityLevel() == 0.0, "Should reset curiosity level"
    assert agent.getNoveltyLevel() == 0.0, "Should reset novelty level"
    assert agent.getPredictionError() == 0.0, "Should reset prediction error"
    
    print("✓ Agent test passed\n")

def test_world():
    """Test world functionality"""
    print("Testing world functionality...")
    
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 16, 16)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Test world properties
    assert world.getWidth() == 30.0, "Should have correct width"
    assert world.getHeight() == 30.0, "Should have correct height"
    assert world.getVisionWidth() == 16, "Should have correct vision width"
    assert world.getVisionHeight() == 16, "Should have correct vision height"
    assert world.getSimulationTime() >= 0.0, "Should have simulation time"
    
    # Test world updates
    for i in range(10):
        world.update(0.1)
        assert world.getSimulationTime() > 0.0, "Should increase simulation time"
    
    # Test agent body
    body = world.getAgentBody()
    assert body is not None, "Should have agent body"
    assert body.x >= 0.0 && body.x < 30.0, "Should have valid x position"
    assert body.y >= 0.0 && body.y < 30.0, "Should have valid y position"
    assert body.energy >= 0.0, "Should have energy"
    assert body.health >= 0.0, "Should have health"
    assert body.age >= 0.0, "Should have age"
    
    # Test world objects
    objects = []
    for x in range(0, 30, 5):
        for y in range(0, 30, 5) {
            obj = world.getObjectAt(x, y)
            if obj is not None: {
                objects.append(obj)
            }
        }
    }
    
    # Test valid positions
    assert world.isValidPosition(5.0, 5.0), "Should be valid position"
    assert not world.isValidPosition(-1.0, 5.0), "Should not be valid position (out of bounds)"
    
    # Test adding and removing objects
    test_obj = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, 10.0, 0.5)
    world.addObject(test_obj)
    
    retrieved_obj = world.getObjectAt(10.0, 10.0)
    assert retrieved_obj is not None, "Should retrieve added object"
    assert retrieved_obj.type == pynlm.WorldObjectType.Resource, "Should have correct type"
    assert retrieved_obj.value == 10.0, "Should have correct value"
    
    world.removeObject(10.0, 10.0)
    retrieved_obj = world.getObjectAt(10.0, 10.0)
    assert retrieved_obj is None, "Should remove object"
    
    print("✓ World test passed\n")

def test_utilities():
    """Test utility functions"""
    print("Testing utility functions...")
    
    # Test createSimpleTest
    brain = pynlm.createSimpleTest()
    assert brain is not None, "Should create simple test brain"
    assert brain.getTotalNeuronCount() == 100, "Should have 100 neurons"
    brain.initialize()
    
    # Test createCompleteTestSetup
    brain, world, agent = pynlm.createCompleteTestSetup()
    assert brain is not None, "Should create brain"
    assert world is not None, "Should create world"
    assert agent is not None, "Should create agent"
    
    # Test runQuickSimulation
    result = pynlm.runQuickSimulation(50)
    assert result["steps_completed"] == 50, "Should complete 50 steps"
    assert result["total_spikes"] >= 0, "Should have spikes"
    assert result["firing_neurons"] >= 0, "Should have firing neurons"
    assert result["average_firing_rate"] >= 0.0, "Should have firing rate"
    
    # Test createAgentWithFeatures
    agent = pynlm.createAgentWithFeatures("test_agent")
    assert agent is not None, "Should create agent"
    assert agent.isRewardModulationEnabled(), "Should enable reward modulation"
    assert agent.isStructuralPlasticityEnabled(), "Should enable structural plasticity"
    assert agent.isDevelopmentEnabled(), "Should enable development"
    assert agent.isCuriosityEnabled(), "Should enable curiosity"
    
    print("✓ Utilities test passed\n")

def test_enums():
    """Test enumeration values"""
    print("Testing enumeration values...")
    
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.NeuronType.Inhibitory is not None
    assert pynlm.NeuronType.Modulatory is not None
    assert pynlm.NeuronType.Sensory is not None
    assert pynlm.NeuronType.Motor is not None
    assert pynlm.NeuronType.Internal is not None
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory is not None
    assert pynlm.SynapseType.Inhibitory is not None
    assert pynlm.SynapseType.Modulatory is not None
    assert pynlm.SynapseType.Electrical is not None
    assert pynlm.SynapseType.GapJunction is not None
    
    # Test DevelopmentalStage
    assert pynlm.DevelopmentalStage.Initial is not None
    assert pynlm.DevelopmentalStage.CriticalPeriod is not None
    assert pynlm.DevelopmentalStage.Maturation is not None
    assert pynlm.DevelopmentalStage.Adult is not None
    assert pynlm.DevelopmentalStage.Aging is not None
    
    # Test FiringState
    assert pynlm.FiringState.Resting is not None
    assert pynlm.FiringState.Active is not None
    assert pynlm.FiringState.Refractory is not None
    assert pynlm.FiringState.Inhibited is not None
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward is not None
    assert pynlm.ActionType.MoveBackward is not None
    assert pynlm.ActionType.MoveLeft is not None
    assert pynlm.ActionType.MoveRight is not None
    assert pynlm.ActionType.TurnLeft is not None
    assert pynlm.ActionType.TurnRight is not None
    assert pynlm.ActionType.Look is not None
    assert pynlm.ActionType.LookUp is not None
    assert pynlm.ActionType.LookDown is not None
    assert pynlm.ActionType.Interact is not None
    assert pynlm.ActionType.Eat is not None
    assert pynlm.ActionType.Drink is not None
    assert pynlm.ActionType.Rest is not None
    assert pynlm.ActionType.Wait is not None
    assert pynlm.ActionType.Custom is not None
    
    # Test MotorCommand
    assert pynlm.MotorCommand.MoveForward is not None
    assert pynlm.MotorCommand.MoveBackward is not None
    assert pynlm.MotorCommand.TurnLeft is not None
    assert pynlm.MotorCommand.TurnRight is not None
    assert pynlm.MotorCommand.LookLeft is not None
    assert pynlm.MotorCommand.LookRight is not None
    assert pynlm.MotorCommand.Interact is not None
    assert pynlm.MotorCommand.Wait is not None
    
    # Test WorldObjectType
    assert pynlm.WorldObjectType.Empty is not None
    assert pynlm.WorldObjectType.Resource is not None
    assert pynlm.WorldObjectType.Hazard is not None
    assert pynlm.WorldObjectType.Wall is not None
    assert pynlm.WorldObjectType.Marker is not None
    
    print("✓ Enum test passed\n")

def test_ids():
    """Test ID types"""
    print("Testing ID types...")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(123)
    assert neuron_id.value == 123, "Should have correct value"
    assert neuron_id.index() == 123, "Should have correct index"
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(456)
    assert synapse_id.value == 456, "Should have correct value"
    assert synapse_id.index() == 456, "Should have correct index"
    
    # Test RegionId
    region_id = pynlm.RegionId(789)
    assert region_id.value == 789, "Should have correct value"
    assert region_id.index() == 789, "Should have correct index"
    
    # Test PopulationId
    population_id = pynlm.PopulationId(999)
    assert population_id.value == 999, "Should have correct value"
    assert population_id.index() == 999, "Should have correct index"
    
    # Test invalid IDs
    assert pynlm.INVALID_NEURON_ID.value == 18446744073709551615, "Should have invalid neuron ID"
    assert pynlm.INVALID_SYNAPSE_ID.value == 18446744073709551615, "Should have invalid synapse ID"
    assert pynlm.INVALID_REGION_ID.value == 18446744073709551615, "Should have invalid region ID"
    assert pynlm.INVALID_POPULATION_ID.value == 18446744073709551615, "Should have invalid population ID"
    
    print("✓ ID test passed\n")

def test_errors():
    """Test error handling"""
    print("Testing error handling...")
    
    # Test invalid initialization
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to step without proper initialization
        brain.step(0)
        print("✓ Brain stepping should not raise error when properly initialized")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        raise
    
    # Test configuration validation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100, pynlm.ConfigSource.Default)
    
    # Should not raise error
    result = pynlm.validation.validateConfig(config)
    assert result.valid, "Configuration should be valid"
    
    print("✓ Error handling test passed\n")

def test_logging():
    """Test logging functionality"""
    print("Testing logging functionality...")
    
    # Test basic logger
    logger = pynlm.log
    assert logger is not None, "Should have logger"
    
    # Test log methods
    logger.info("Test info message")
    logger.warn("Test warning message")
    logger.error("Test error message")
    logger.debug("Test debug message")
    
    print("✓ Logging test passed\n")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("PYNLM PYTHON BINDINGS TEST SUITE")
    print("=" * 60)
    print()
    
    tests = [
        test_version,
        test_config,
        test_brain_basic,
        test_brain_advanced,
        test_agent,
        test_world,
        test_utilities,
        test_enums,
        test_ids,
        test_errors,
        test_logging,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ Test failed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("=" * 60)
    print(f"TEST RESULTS: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("Some tests failed. Please check the output above.")
        return False
    else:
        print("All tests passed!")
        return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)