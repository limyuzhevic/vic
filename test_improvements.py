#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings improvements.

This test file validates all the improvements made to the NLM Python bindings,
including enhanced error handling, validation, convenience functions, and
advanced features for experienced users.
"""

import sys
import json
import tempfile
import traceback
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent / "python"))

import pynlm

def test_basic_functionality():
    """Test basic NLM functionality with enhanced bindings."""
    print("Testing basic functionality...")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    assert config is not None, "Failed to create default configuration"
    
    # Create a brain
    brain = pynlm.createBrain(config)
    assert brain is not None, "Failed to create brain"
    
    # Initialize the brain
    success = brain.initialize()
    assert success, "Brain initialization failed"
    
    # Run a few simulation steps
    for step in range(10):
        brain.step(step)
    
    # Check brain statistics
    stats = brain.getStats()
    assert stats["total_neurons"] > 0, "Brain should have neurons"
    assert stats["total_synapses"] >= 0, "Brain should have synapses"
    assert stats["firing_neurons"] >= 0, "Brain should have firing neurons"
    
    print("✓ Basic functionality test passed")

def test_enhanced_config():
    """Test enhanced configuration functionality."""
    print("Testing enhanced configuration...")
    
    # Create configuration from dictionary
    config_dict = {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.08,
        "v_thresh": -45.0
    }
    
    config = pynlm.createConfigFromDict(config_dict)
    assert config is not None, "Failed to create config from dictionary"
    
    # Test configuration properties
    assert config.hasKey("neuron_count"), "Config should have neuron_count"
    assert config.hasKey("region_count"), "Config should have region_count"
    assert config.hasKey("v_thresh"), "Config should have v_thresh"
    
    # Test get methods
    neuron_count = config.getValue("neuron_count")
    assert neuron_count is not None, "Should get neuron_count"
    assert neuron_count == 2000, f"Expected 2000, got {neuron_count}"
    
    region_count = config.getValue("region_count")
    assert region_count is not None, "Should get region_count"
    assert region_count == 3, f"Expected 3, got {region_count}"
    
    # Test configuration as dictionary
    config_as_dict = config.toDict()
    assert "neuron_count" in config_as_dict, "Dictionary should contain neuron_count"
    assert "region_count" in config_as_dict, "Dictionary should contain region_count"
    
    # Test configuration filtering
    neuron_keys = config.getKeys("neuron")
    assert len(neuron_keys) >= 1, "Should have at least one neuron key"
    
    # Test configuration pairs
    config_pairs = config.getKeyValuePairs()
    assert isinstance(config_pairs, dict), "Config pairs should be a dictionary"
    assert "neuron_count" in config_pairs, "Config pairs should contain neuron_count"
    
    print("✓ Enhanced configuration test passed")

def test_enhanced_world():
    """Test enhanced world functionality."""
    print("Testing enhanced world...")
    
    # Create world with enhanced configuration
    world = pynlm.createSimpleWorld(20, 20, 8, 8)
    assert world is not None, "Failed to create world"
    
    # Set agent start position
    world.setAgentStart(10.0, 10.0)
    
    # Update world
    world.update(0.1)
    
    # Get world statistics
    stats = world.getStats()
    assert "width" in stats, "World stats should contain width"
    assert "height" in stats, "World stats should contain height"
    assert "simulation_time" in stats, "World stats should contain simulation_time"
    assert stats["width"] == 20, f"Expected width 20, got {stats['width']}"
    assert stats["height"] == 20, f"Expected height 20, got {stats['height']}"
    
    # Test world object operations
    world_obj = pynlm.createWorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
    assert world_obj is not None, "Failed to create world object"
    assert world_obj.x == 5.0, f"Expected x 5.0, got {world_obj.x}"
    assert world_obj.y == 5.0, f"Expected y 5.0, got {world_obj.y}"
    assert world_obj.type == pynlm.WorldObjectType.Resource, "Should be Resource type"
    
    # Add object to world
    world.addObject(world_obj)
    
    # Get object at position
    retrieved_obj = world.getObjectAt(5.0, 5.0)
    assert retrieved_obj is not None, "Should retrieve object at position"
    
    # Remove object
    world.removeObject(5.0, 5.0)
    retrieved_obj = world.getObjectAt(5.0, 5.0)
    assert retrieved_obj is None, "Object should be removed"
    
    # Test position validation
    assert world.isValidPosition(10.0, 10.0), "Agent start position should be valid"
    assert not world.isValidPosition(25.0, 25.0), "Out of bounds position should be invalid"
    
    print("✓ Enhanced world test passed")

def test_enhanced_agent():
    """Test enhanced agent brain functionality."""
    print("Testing enhanced agent...")
    
    # Create complete agent setup
    config_dict = {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.05
    }
    
    brain, agent, world = pynlm.createCompleteEnvironment(config_dict)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable various features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Check feature flags
    assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
    assert agent.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
    assert agent.isDevelopmentEnabled(), "Development should be enabled"
    assert agent.isCuriosityEnabled(), "Curiosity should be enabled"
    
    # Run simulation with sensory input
    for step in range(20):
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.1 if step % 10 == 0 else 0.0
        agent.applyRewardModulation(reward, reward * 0.9)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    # Check agent statistics
    stats = agent.getStats()
    assert "neuromodulation_level" in stats, "Agent stats should contain neuromodulation level"
    assert "curiosity_level" in stats, "Agent stats should contain curiosity level"
    assert "novelty_level" in stats, "Agent stats should contain novelty level"
    assert "prediction_error" in stats, "Agent stats should contain prediction error"
    
    print("✓ Enhanced agent test passed")

def test_enhanced_sensory():
    """Test enhanced sensory input functionality."""
    print("Testing enhanced sensory...")
    
    # Create vision input
    vision_data = []
    for i in range(64):  # 8x8x1
        vision_data.append(float(i % 256) / 255.0)
    
    vision = pynlm.createVision(8, 8, 1, vision_data)
    assert vision is not None, "Failed to create vision"
    assert vision.getWidth() == 8, "Width should be 8"
    assert vision.getHeight() == 8, "Height should be 8"
    assert vision.getChannels() == 1, "Channels should be 1"
    
    # Create audio input
    audio_data = [0.1, 0.2, 0.3, 0.4, 0.5]
    audio = pynlm.createAudio(44100, 1, audio_data)
    assert audio is not None, "Failed to create audio"
    assert audio.getSampleRate() == 44100, "Sample rate should be 44100"
    assert audio.getNumSamples() == 1, "Num samples should be 1 (simplified)"
    
    # Create internal signals
    internal_signals = pynlm.createInternalSignals([0.1, 0.2, 0.3, 0.4, 0.5])
    assert internal_signals is not None, "Failed to create internal signals"
    
    # Create action
    action = pynlm.createAction(pynlm.ActionType.MoveForward)
    assert action is not None, "Failed to create action"
    assert action.getType() == pynlm.ActionType.MoveForward, "Action type should be MoveForward"
    
    # Create action with parameters
    action_with_params = pynlm.createAction(pynlm.ActionType.MoveForward, [0.5, 0.5, 0.5])
    assert action_with_params is not None, "Failed to create action with parameters"
    assert len(action_with_params.getParameters()) == 3, "Should have 3 parameters"
    
    # Create sensory percept
    percept = pynlm.createSensoryPercept(
        vision=vision,
        internal=internal_signals,
        audio=audio
    )
    assert percept is not None, "Failed to create sensory percept"
    assert percept.hasVision(), "Percept should have vision"
    assert percept.hasInternal(), "Percept should have internal signals"
    assert percept.hasAudio(), "Percept should have audio"
    
    print("✓ Enhanced sensory test passed")

def test_enhanced_error_handling():
    """Test enhanced error handling and validation."""
    print("Testing enhanced error handling...")
    
    # Test invalid configuration
    invalid_config = pynlm.createDefaultConfig()
    invalid_config.set("neuron_count", -100)  # Invalid value
    
    try:
        brain = pynlm.createBrain(invalid_config)
        brain.initialize()
        assert False, "Should have failed with negative neuron count"
    except Exception as e:
        assert "Invalid argument" in str(e), f"Should be validation error, got: {e}"
    
    # Test invalid world configuration
    try:
        world = pynlm.createSimpleWorld(-10, 10, 8, 8)  # Invalid width
        assert False, "Should have failed with negative width"
    except Exception as e:
        assert "Invalid argument" in str(e), f"Should be validation error, got: {e}"
    
    # Test invalid step parameters
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        # This should not raise an exception due to validation
        brain.step(-5)  # This should raise validation error
        assert False, "Should have failed with negative step"
    except Exception as e:
        assert "Invalid argument" in str(e), f"Should be validation error, got: {e}"
    
    print("✓ Enhanced error handling test passed")

def test_enhanced_serialization():
    """Test enhanced serialization functionality."""
    print("Testing enhanced serialization...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps
    for step in range(20):
        brain.step(step)
    
    # Save brain to temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tmp_file:
        filepath = tmp_file.name
    
    try:
        # Save brain state
        success = brain.save(filepath)
        assert success, "Failed to save brain state"
        
        # Create new brain and load state
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        
        # Load brain state
        success = brain2.load(filepath)
        assert success, "Failed to load brain state"
        
        # Both brains should have same number of neurons
        assert brain.getTotalNeuronCount() == brain2.getTotalNeuronCount(), \
            "Loaded brain should have same neuron count"
        
        # Save config to file
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as config_file:
            config_path = config_file.name
        
        try:
            # Save config
            success = config.saveToFile(config_path)
            assert success, "Failed to save config"
            
            # Create new config and load
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromFile(config_path)
            assert success, "Failed to load config"
            
            # Configs should have same values
            assert config.getValue("neuron_count") == config2.getValue("neuron_count"), \
                "Loaded config should have same neuron count"
            
        finally:
            Path(config_path).unlink(missing_ok=True)
    
    finally:
        Path(filepath).unlink(missing_ok=True)
    
    print("✓ Enhanced serialization test passed")

def test_enhanced_statistics():
    """Test enhanced statistics and monitoring."""
    print("Testing enhanced statistics...")
    
    # Create complete environment
    brain, agent, world = pynlm.createCompleteEnvironment()
    agent.initialize(world)
    
    # Run simulation
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get comprehensive statistics
    stats = pynlm.getComprehensiveStats(brain, agent, world)
    
    # Check brain statistics
    assert "brain" in stats, "Stats should contain brain"
    assert "total_neurons" in stats["brain"], "Brain stats should contain total_neurons"
    assert "total_synapses" in stats["brain"], "Brain stats should contain total_synapses"
    assert "firing_neurons" in stats["brain"], "Brain stats should contain firing_neurons"
    
    # Check agent statistics
    assert "agent" in stats, "Stats should contain agent"
    assert "neuromodulation_level" in stats["agent"], "Agent stats should contain neuromodulation_level"
    assert "curiosity_level" in stats["agent"], "Agent stats should contain curiosity_level"
    assert "novelty_level" in stats["agent"], "Agent stats should contain novelty_level"
    assert "prediction_error" in stats["agent"], "Agent stats should contain prediction_error"
    
    # Check world statistics
    assert "world" in stats, "Stats should contain world"
    assert "width" in stats["world"], "World stats should contain width"
    assert "height" in stats["world"], "World stats should contain height"
    
    # Get simulation summary
    summary = pynlm.getSimulationSummary(brain, agent, world, 29)
    assert "Step:" in summary, "Summary should contain step information"
    assert "Brain Neurons:" in summary, "Summary should contain neuron count"
    assert "Agent Curiosity:" in summary, "Summary should contain curiosity level"
    
    print("✓ Enhanced statistics test passed")

def test_advanced_features():
    """Test advanced features for experienced users."""
    print("Testing advanced features...")
    
    # Test simulation environment
    env = pynlm.createSimulationEnvironment()
    assert env.brain is not None, "Environment should have brain"
    assert env.agent is not None, "Environment should have agent"
    assert env.world is not None, "Environment should have world"
    
    # Run simulation using environment
    env.runSimulation(50, verbose=True)
    
    # Test custom configuration
    custom_config = {
        "neuron_count": 1500,
        "region_count": 4,
        "connection_probability": 0.03,
        "v_thresh": -55.0,
        "v_rest": -75.0
    }
    
    brain, agent, world = pynlm.createCompleteEnvironment(custom_config)
    agent.initialize(world)
    
    # Verify custom configuration
    assert brain.getTotalNeuronCount() == 1500, "Should have 1500 neurons"
    assert brain.getTotalSynapseCount() >= 0, "Should have synapses"
    
    # Test advanced action creation
    action1 = pynlm.createAction(pynlm.ActionType.MoveForward, [0.5, 0.5, 0.5, 0.5])
    action2 = pynlm.createAction(pynlm.ActionType.TurnLeft)
    
    assert action1.getName() == "MoveForward", "Action name should be MoveForward"
    assert action2.getName() == "TurnLeft", "Action name should be TurnLeft"
    
    # Test advanced sensory creation
    vision1 = pynlm.createVision(16, 16, 3)
    vision2 = pynlm.createVisionFromData(8, 8, 1, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9])
    
    audio = pynlm.createAudioFromData(22050, 2, [0.1, 0.2, 0.3, 0.4])
    internal = pynlm.createInternalSignalsFromList([0.1, 0.2, 0.3, 0.4, 0.5])
    
    # Create complex sensory percept
    percept = pynlm.createSensoryPercept(
        vision=vision1,
        touch=vision2,  # Using vision2 as touch for testing
        internal=internal,
        audio=audio
    )
    
    assert percept.hasVision(), "Percept should have vision"
    assert percept.hasInternal(), "Percept should have internal"
    assert percept.hasAudio(), "Percept should have audio"
    
    print("✓ Advanced features test passed")

def test_example_functions():
    """Test example functions provided in the bindings."""
    print("Testing example functions...")
    
    # Test minimal example
    result = pynlm.example_minimal()
    assert "Minimal example completed successfully" in result, "Minimal example should succeed"
    
    # Test complete agent example
    result = pynlm.example_complete_agent()
    assert "Complete agent example completed successfully" in result, "Complete agent example should succeed"
    
    # Test sensory input example
    result = pynlm.example_with_sensory_input()
    assert "Example with sensory input completed successfully" in result, "Sensory example should succeed"
    
    print("✓ Example functions test passed")

def test_utility_functions():
    """Test utility functions and type conversion."""
    print("Testing utility functions...")
    
    # Test version
    version = pynlm.getVersion()
    assert version.startswith("NLM Python Bindings v"), f"Version should start with 'NLM Python Bindings v', got: {version}"
    
    # Test system ready
    assert pynlm.isSystemReady(), "System should be ready"
    
    # Test type conversion
    test_vector = [0.1, 0.2, 0.3, 0.4, 0.5]
    python_list = pynlm.toPythonList(test_vector)
    assert isinstance(python_list, list), "Should convert to Python list"
    assert len(python_list) == 5, "Should have 5 elements"
    
    # Test reverse conversion
    from_list = pynlm.toStdVector(python_list)
    assert isinstance(from_list, list), "Should convert back to list"
    assert len(from_list) == 5, "Should have 5 elements"
    
    # Test ID creation
    neuron_id = pynlm.neuronId(12345)
    assert neuron_id.value == 12345, "Neuron ID should be 12345"
    
    synapse_id = pynlm.synapseId(67890)
    assert synapse_id.value == 67890, "Synapse ID should be 67890"
    
    population_id = pynlm.populationId(54321)
    assert population_id.value == 54321, "Population ID should be 54321"
    
    region_id = pynlm.regionId(98765)
    assert region_id.value == 98765, "Region ID should be 98765"
    
    # Test region ID to string
    region_str = pynlm.regionIdToString(region_id)
    assert "RegionId" in region_str, "String representation should contain 'RegionId'"
    assert "98765" in region_str, "String representation should contain '98765'"
    
    print("✓ Utility functions test passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings - Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_enhanced_config,
        test_enhanced_world,
        test_enhanced_agent,
        test_enhanced_sensory,
        test_enhanced_error_handling,
        test_enhanced_serialization,
        test_enhanced_statistics,
        test_advanced_features,
        test_example_functions,
        test_utility_functions,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"✗ Test {test.__name__} failed: {e}")
            print(f"  Error details: {traceback.format_exc()}")
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print(f"FAILED: {failed} test(s) failed")
        return False
    else:
        print("SUCCESS: All tests passed!")
        return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)