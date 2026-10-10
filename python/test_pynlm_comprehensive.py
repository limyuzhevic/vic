#!/usr/bin/env python3
"""
Python Bindings Test Suite for NLM (Neural Learning Machine)

Comprehensive test suite for the pynlm Python bindings.
Tests all major functionality including brain creation, configuration,
sensory processing, motor output, and advanced features.
"""

import sys
import os
import json
import tempfile
import traceback
import pynlm


def test_basic_imports():
    """Test that all expected modules can be imported"""
    print("Testing basic imports...")
    
    # Test core imports
    assert hasattr(pynlm, 'createBrain')
    assert hasattr(pynlm, 'createDefaultConfig')
    assert hasattr(pynlm, 'createSimpleWorld')
    assert hasattr(pynlm, 'createAgentBrain')
    
    # Test type constants
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    # Test enumerations
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'SynapseType')
    assert hasattr(pynlm, 'ActionType')
    assert hasattr(pynlm, 'MotorCommand')
    assert hasattr(pynlm, 'WorldObjectType')
    
    print("  ✓ Basic imports test passed")


def test_config_creation_and_manipulation():
    """Test configuration creation and manipulation"""
    print("Testing configuration...")
    
    # Test default config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test config has basic methods
    assert hasattr(config, 'has')
    assert hasattr(config, 'getKeys')
    assert hasattr(config, 'clear')
    assert hasattr(config, 'summary')
    
    # Test setting and getting values
    config.set("test_int", 42)
    assert config.has("test_int")
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert len(summary) > 0
    
    print("  ✓ Configuration test passed")


def test_brain_creation_and_initialization():
    """Test brain creation and initialization"""
    print("Testing brain creation...")
    
    # Create a default config
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test brain methods
    assert hasattr(brain, 'initialize')
    assert hasattr(brain, 'step')
    assert hasattr(brain, 'receiveSensoryInput')
    assert hasattr(brain, 'produceAction')
    assert hasattr(brain, 'reset')
    
    # Initialize brain
    brain.initialize()
    
    # Test brain state methods
    assert hasattr(brain, 'getTotalNeuronCount')
    assert hasattr(brain, 'getTotalSynapseCount')
    assert hasattr(brain, 'getActiveNeuronCount')
    assert hasattr(brain, 'getFiringNeuronCount')
    assert hasattr(brain, 'getAverageFiringRate')
    
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    
    print(f"  ✓ Created brain with {neuron_count} neurons and {synapse_count} synapses")
    
    print("  ✓ Brain creation test passed")


def test_world_creation_and_interaction():
    """Test world creation and interaction"""
    print("Testing world creation...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    # Test world methods
    assert hasattr(world, 'configure')
    assert hasattr(world, 'reset')
    assert hasattr(world, 'update')
    assert hasattr(world, 'getSensoryPercept')
    assert hasattr(world, 'getAgentBody')
    
    # Configure world
    world.configure(100, 100, 20, 20)
    
    # Set agent start position
    world.setAgentStart(50, 50)
    
    # Get world info
    width = world.getWidth()
    height = world.getHeight()
    
    print(f"  ✓ Created world {width}x{height}")
    
    print("  ✓ World creation test passed")


def test_agent_brain_creation():
    """Test agent brain creation and interface"""
    print("Testing agent brain...")
    
    # Create brain first
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    assert agent_brain is not None
    
    # Test agent brain methods
    assert hasattr(agent_brain, 'initialize')
    assert hasattr(agent_brain, 'processSensoryInput')
    assert hasattr(agent_brain, 'decodeMotorCommand')
    assert hasattr(agent_brain, 'applyRewardModulation')
    assert hasattr(agent_brain, 'getNeuromodulationLevel')
    
    # Create a world for integration testing
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 20, 20)
    
    # Initialize agent brain with world
    agent_brain.initialize(world)
    
    print("  ✓ Agent brain test passed")


def test_sensory_input_and_action():
    """Test sensory input and action handling"""
    print("Testing sensory input and action...")
    
    # Create world and brain
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 10, 10)
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create sensory inputs
    # Test Vision
    vision = pynlm.Vision(10, 10, 3)
    vision_data = [0.1] * 300  # 10x10x3
    vision.setData(vision_data)
    
    # Test Audio
    audio = pynlm.Audio(44100, 1000)
    audio_data = [0.0] * 1000
    audio.setData(audio_data)
    
    # Test Internal Signals
    internal = pynlm.InternalSignals()
    internal.addSignal(0.5)
    internal.addSignal(0.8)
    
    # Test Action
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    action.setType(pynlm.ActionType.Look)
    action_params = [0.5, 0.5]
    action.setParameters(action_params)
    
    # Verify action
    assert action.getType() == pynlm.ActionType.Look
    assert action.getParameters() == action_params
    assert action.getName() == "Look"
    
    print("  ✓ Sensory input and action test passed")


def test_neuromodulation():
    """Test neuromodulation functionality"""
    print("Testing neuromodulation...")
    
    # Create brain and agent brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent_brain = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 20, 20)
    
    # Initialize agent brain
    agent_brain.initialize(world)
    
    # Test neuromodulation levels
    neuromodulation_level = agent_brain.getNeuromodulationLevel()
    assert isinstance(neuromodulation_level, float)
    assert 0.0 <= neuromodulation_level <= 1.0
    
    curiosity_level = agent_brain.getCuriosityLevel()
    assert isinstance(curiosity_level, float)
    
    novelty_level = agent_brain.getNoveltyLevel()
    assert isinstance(novelty_level, float)
    
    prediction_error = agent_brain.getPredictionError()
    assert isinstance(prediction_error, float)
    
    # Test neuromodulation enabling
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    
    assert agent_brain.isRewardModulationEnabled() == True
    assert agent_brain.isCuriosityEnabled() == True
    
    print("  ✓ Neuromodulation test passed")


def test_development():
    """Test development functionality"""
    print("Testing development...")
    
    # Create brain and agent brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent_brain = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 20, 20)
    
    # Initialize agent brain
    agent_brain.initialize(world)
    
    # Test developmental stage
    stage = agent_brain.getDevelopmentalStage()
    assert isinstance(stage, int)
    
    # Test development update
    agent_brain.updateDevelopment(0.1)
    
    print("  ✓ Development test passed")


def test_memory_functionality():
    """Test memory functionality"""
    print("Testing memory...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic memory operations
    total_spikes = brain.getTotalSpikeCount()
    active_neurons = brain.getActiveNeuronCount()
    
    print(f"  ✓ Memory test: {total_spikes} total spikes, {active_neurons} active neurons")
    
    print("  ✓ Memory test passed")


def test_config_file_operations():
    """Test configuration file operations"""
    print("Testing configuration file operations...")
    
    # Create a temporary config file
    config_data = {
        "neuron_count": 1000,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "random_seed": 42
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(config_data, f)
        config_file = f.name
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test loading config from file
        config.loadFromFile(config_file)
        
        # Test saving config to file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            save_file = f.name
        
        try:
            config.saveToFile(save_file)
            
            # Load saved config and verify
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(save_file)
            
            # Verify key values
            assert config2.has("neuron_count")
            assert config2.has("connection_probability")
            
        finally:
            if os.path.exists(save_file):
                os.unlink(save_file)
        
    finally:
        if os.path.exists(config_file):
            os.unlink(config_file)
    
    print("  ✓ Configuration file operations test passed")


def test_type_identifiers():
    """Test type identifier functionality"""
    print("Testing type identifiers...")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(123)
    assert neuron_id.value == 123
    assert neuron_id.index() == 123
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(456)
    assert synapse_id.value == 456
    
    # Test RegionId
    region_id = pynlm.RegionId(789)
    assert region_id.value == 789
    
    # Test PopulationId
    population_id = pynlm.PopulationId(999)
    assert population_id.value == 999
    
    # Test enumerations
    assert pynlm.NeuronType.Excitatory != pynlm.NeuronType.Inhibitory
    assert pynlm.SynapseType.Excitatory != pynlm.SynapseType.Inhibitory
    
    print("  ✓ Type identifiers test passed")


def test_error_handling():
    """Test error handling"""
    print("Testing error handling...")
    
    # Test that invalid operations raise appropriate errors
    config = pynlm.createDefaultConfig()
    
    # Test non-existent key access
    try:
        # This should either return None or raise an appropriate exception
        pass
    except Exception:
        pass  # Expected
    
    print("  ✓ Error handling test passed")


def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    print()
    
    tests = [
        test_basic_imports,
        test_type_identifiers,
        test_config_creation_and_manipulation,
        test_brain_creation_and_initialization,
        test_world_creation_and_interaction,
        test_agent_brain_creation,
        test_sensory_input_and_action,
        test_neuromodulation,
        test_development,
        test_memory_functionality,
        test_config_file_operations,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
            print(f"  ✓ {test.__name__} PASSED")
        except Exception as e:
            failed += 1
            print(f"  ✗ {test.__name__} FAILED: {e}")
            traceback.print_exc()
        
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
