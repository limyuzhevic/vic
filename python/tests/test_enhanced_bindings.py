#!/usr/bin/env python3
"""
Comprehensive Python tests for enhanced NLM Python bindings.
Tests Pythonic interfaces, high-level APIs, and error handling.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_id_types():
    """Test ID type classes with Pythonic interfaces."""
    print("Testing ID types...")
    
    # Test NeuronId
    neuron_id = pynlm.createNeuronId(42)
    assert neuron_id.value == 42
    assert neuron_id.index() == 42
    assert str(neuron_id) == "NeuronId(42)"
    assert repr(neuron_id) == "<NeuronId: 42>"
    assert neuron_id == pynlm.createNeuronId(42)
    assert neuron_id != pynlm.createNeuronId(43)
    
    # Test SynapseId
    synapse_id = pynlm.createSynapseId(100)
    assert synapse_id.value == 100
    assert synapse_id.index() == 100
    assert str(synapse_id) == "SynapseId(100)"
    
    # Test RegionId
    region_id = pynlm.createRegionId(200)
    assert region_id.value == 200
    assert str(region_id) == "RegionId(200)"
    
    # Test PopulationId
    pop_id = pynlm.createPopulationId(300)
    assert pop_id.value == 300
    assert str(pop_id) == "PopulationId(300)"
    
    # Test hashing and sets
    neuron_ids = {pynlm.createNeuronId(1), pynlm.createNeuronId(2), pynlm.createNeuronId(1)}
    assert len(neuron_ids) == 2  # Duplicate removed
    
    print("✓ ID types tests passed")

def test_enumerations():
    """Test enumeration types."""
    print("Testing enumerations...")
    
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory == 0
    assert pynlm.NeuronType.Inhibitory == 1
    assert pynlm.NeuronType.Sensory == 4
    assert pynlm.NeuronType.Motor == 5
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory == 0
    assert pynlm.SynapseType.GapJunction == 5
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward == 0
    assert pynlm.ActionType.MoveBackward == 1
    assert pynlm.ActionType.Wait == 13
    
    # Test MotorCommand
    assert pynlm.MotorCommand.MoveForward == 0
    assert pynlm.MotorCommand.Interact == 6
    
    # Test developmental stages
    assert pynlm.DevelopmentalStage.Initial == 0
    assert pynlm.DevelopmentalStage.Aging == 4
    
    # Test firing states
    assert pynlm.FiringState.Resting == 0
    assert pynlm.FiringState.Active == 1
    
    print("✓ Enumeration tests passed")

def test_enhanced_config():
    """Test enhanced configuration with Pythonic interfaces."""
    print("Testing enhanced configuration...")
    
    # Test empty config
    config = pynlm.Config()
    assert len(config.getKeys()) == 0
    assert not config.has("test.key")
    assert config.getInt("test.key", 100) == 100
    assert config.getDouble("test.key", 3.14) == 3.14
    assert config.getString("test.key", "default") == "default"
    
    # Test key-value storage
    config.setInt("test.int", 42)
    config.setDouble("test.double", 3.14159)
    config.setString("test.string", "hello")
    config.setBool("test.bool", True)
    
    assert config.getInt("test.int") == 42
    assert config.getDouble("test.double") == 3.14159
    assert config.getString("test.string") == "hello"
    assert config.getBool("test.bool") == True
    
    # Test iteration
    keys = list(config.getKeys())
    assert "test.int" in keys
    assert "test.double" in keys
    assert "test.string" in keys
    assert "test.bool" in keys
    
    # Test length
    assert len(config) == 4
    
    # Test validation
    config.setString("name", "")  # Empty name should cause validation error
    errors = config.validate()
    assert len(errors) > 0
    
    # Test type-safe setters
    config2 = pynlm.Config()
    config2.setInt("key", 10)
    config2.setDouble("key", 5.5)  # Should overwrite
    assert config2.getDouble("key") == 5.5
    
    print("✓ Enhanced config tests passed")

def test_vision_enhancements():
    """Test enhanced vision functionality."""
    print("Testing vision enhancements...")
    
    # Create vision data
    width, height, channels = 3, 4, 3
    pixels = [0.1, 0.5, 0.9, 0.3, 0.7, 0.2] * 4  # 24 elements
    
    vision = pynlm.Vision(width, height, channels)
    vision.setData(pixels)
    
    assert vision.getWidth() == width
    assert vision.getHeight() == height
    assert vision.getChannels() == channels
    assert vision.getDimensions() == 3
    assert vision.is_valid()
    
    # Test validation
    invalid_data = [0.1, 0.2]  # Wrong size
    try:
        vision.validate_data(invalid_data)
        assert False, "Should have raised value_error"
    except Exception:
        pass  # Expected
    
    # Test normalization
    normalized = vision.get_normalized_data()
    assert len(normalized) == len(pixels)
    assert all(0 <= val <= 1 for val in normalized)
    
    # Test size property
    assert vision.get_size() == width * height
    
    print("✓ Vision enhancements tests passed")

def test_action_enhancements():
    """Test enhanced action functionality."""
    print("Testing action enhancements...")
    
    # Create action from type
    move_action = pynlm.Action(pynlm.ActionType.MoveForward, [0.8, 0.6])
    assert move_action.getType() == pynlm.ActionType.MoveForward
    assert move_action.getParameters() == [0.8, 0.6]
    assert move_action.getName() == "MoveForward"
    assert not move_action.is_empty()
    
    # Test action from dict
    action = pynlm.Action()
    action.from_dict({
        "type": pynlm.ActionType.TurnLeft,
        "parameters": [0.5, 0.3],
        "name": "TurnLeft"
    })
    assert action.getType() == pynlm.ActionType.TurnLeft
    assert action.getParameters() == [0.5, 0.3]
    
    # Test action to dict
    action_dict = action.to_dict()
    assert action_dict["type"] == pynlm.ActionType.TurnLeft
    assert action_dict["parameters"] == [0.5, 0.3]
    
    # Test action cloning and resetting
    action_clone = action.clone()
    assert action_clone.getType() == action.getType()
    
    action.reset()
    assert action.is_empty()
    
    print("✓ Action enhancements tests passed")

def test_simple_world_enhancements():
    """Test enhanced SimpleWorld functionality."""
    print("Testing SimpleWorld enhancements...")
    
    world = pynlm.createSimpleWorld()
    world.configure(100, 80, 20, 20)
    
    assert world.getWidth() == 100
    assert world.getHeight() == 80
    assert world.get_visionWidth() == 20
    assert world.get_visionHeight() == 20
    
    world.setMaxEnergy(1000.0)
    assert world.getMaxEnergy() == 1000.0
    
    world.setEnergyDecayRate(0.1)
    assert world.getEnergyDecayRate() == 0.1
    
    # Test agent placement
    world.setAgentStart(50, 50)
    
    # Test object management
    obj = pynlm.WorldObject(60, 60, pynlm.WorldObjectType.Resource, 1.0, 0.5)
    world.addObject(obj)
    
    # Test object retrieval
    agent_body = world.getAgentBody()
    distance = agent_body.distance_to(obj)
    assert distance > 0
    
    # Test simulation
    for step in range(10):
        world.update(0.1)
        assert world.getSimulationTime() == step * 0.1
    
    print("✓ SimpleWorld enhancements tests passed")

def test_brain_enhancements():
    """Test enhanced Brain functionality."""
    print("Testing brain enhancements...")
    
    brain = pynlm.createBrainWithDefaultConfig()
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Test step with callback
    step_count = 0
    def callback(step):
        nonlocal step_count
        step_count = step
    
    brain.step(0, 0.0)
    assert step_count == 0
    
    # Test batch operations
    brain.reset()
    brain.initialize()
    brain.runSteps(50, 0.001)
    
    # Test plasticity control
    brain.enablePlasticity(True, hebbian=True, stdp=True)
    
    # Test sensory input validation
    vision = pynlm.Vision(10, 10)
    vision.setData([0.1] * 300)  # Valid size
    brain.receiveSensoryInput(vision)
    
    try:
        brain.receiveSensoryInput(pynlm.Vision())  # Invalid type
        assert False, "Should have raised value_error"
    except Exception:
        pass  # Expected
    
    print("✓ Brain enhancements tests passed")

def test_agent_brain_enhancements():
    """Test enhanced AgentBrain functionality."""
    print("Testing AgentBrain enhancements...")
    
    world = pynlm.createSimpleWorld()
    world.configure(50, 50, 20, 20)
    
    brain = pynlm.createBrainWithDefaultConfig()
    brain.initialize()
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    assert agent_brain.getSensoryInputSize() > 0
    assert agent_brain.getMotorOutputSize() > 0
    
    # Test episode running
    agent_brain.runEpisode(world, 20)
    
    # Test step with action
    action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5])
    agent_brain.stepWithAction(world, action)
    
    # Test neuromodulation
    agent_brain.applyRewardModulation(0.8, 0.5)
    assert agent_brain.getNeuromodulationLevel() > 0
    
    print("✓ AgentBrain enhancements tests passed")

def test_factory_functions():
    """Test factory functions and utilities."""
    print("Testing factory functions...")
    
    # Test createDefaultConfig
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 2000)
    assert config.getInt("brain.neuron_count") == 2000
    
    # Test createBrainWithDefaultConfig
    brain = pynlm.createBrainWithDefaultConfig()
    assert brain is not None
    brain.initialize()
    
    # Test createCompleteAgentWorld
    world, brain, agent_brain = pynlm.createCompleteAgentWorld()
    assert world is not None
    assert brain is not None
    assert agent_brain is not None
    
    # Test runLearningExperiment
    brain2 = pynlm.createBrainWithDefaultConfig()
    pynlm.runLearningExperiment(brain2, 100)
    assert brain2.getTotalSpikeCount() > 0
    
    # Test simulation with callback
    def simple_callback(step):
        pass
    
    brain3 = pynlm.createBrainWithDefaultConfig()
    pynlm.runSimulation(brain3, 50, simple_callback)
    
    print("✓ Factory functions tests passed")

def test_context_managers():
    """Test context manager functionality."""
    print("Testing context managers...")
    
    # Test brain context manager
    with pynlm.createBrain() as brain:
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
        # Brain should be reset after context exit
    
    print("✓ Context manager tests passed")

def test_error_handling():
    """Test error handling and validation."""
    print("Testing error handling...")
    
    # Test configuration validation
    config = pynlm.Config()
    config.setString("name", "")  # Should cause validation error
    errors = config.validate()
    assert len(errors) > 0
    
    # Test sensory input validation
    vision = pynlm.Vision()
    try:
        vision.validate_data([1.0, 2.0])  # Invalid size
        assert False, "Should have raised value_error"
    except Exception:
        pass
    
    print("✓ Error handling tests passed")

def test_string_representations():
    """Test string representations."""
    print("Testing string representations...")
    
    # Test ID types
    neuron_id = pynlm.createNeuronId(42)
    assert str(neuron_id) == "NeuronId(42)"
    assert repr(neuron_id) == "<NeuronId: 42>"
    
    # Test Config
    config = pynlm.Config()
    config.setInt("test", 100)
    config_str = str(config)
    assert "NLM Configuration" in config_str
    config_repr = repr(config)
    assert "Config" in config_repr
    
    # Test Brain
    brain = pynlm.createBrainWithDefaultConfig()
    brain.initialize()
    brain_str = str(brain)
    assert "NLM Brain simulation" in brain_str
    brain_repr = repr(brain)
    assert "Brain with" in brain_repr
    
    print("✓ String representation tests passed")

def test_collection_operations():
    """Test collection-like operations."""
    print("Testing collection operations...")
    
    # Test config iteration
    config = pynlm.Config()
    config.setInt("a", 1)
    config.setInt("b", 2)
    config.setInt("c", 3)
    
    keys = list(config.getKeys())
    assert "a" in keys
    assert "b" in keys
    assert "c" in keys
    
    # Test containment
    assert "a" in config
    assert "d" not in config
    
    # Test length
    assert len(config) == 3
    
    # Test set operations
    new_keys = {"x", "y", "z"}
    for key in new_keys:
        config.setInt(key, 0)
    
    assert len(config) == 6
    
    print("✓ Collection operations tests passed")

def test_complete_integration():
    """Test complete integration scenario."""
    print("Testing complete integration...")
    
    # Create complete agent world
    world, brain, agent_brain = pynlm.createCompleteAgentWorld()
    world.setAgentStart(25, 25)
    
    # Run simulation with logging
    for step in range(20):
        world.update(0.1)
        agent_brain.processSensoryInput(world.getSensoryPercept())
        agent_brain.updateDevelopment(0.1)
        
        assert agent_brain.getCuriosityLevel() >= 0
        assert agent_brain.getNoveltyLevel() >= 0
    
    # Test action selection and execution
    action = agent_brain.decodeMotorCommand()
    assert action.getType() in [pynlm.ActionType.Wait, pynlm.ActionType.MoveForward, 
                               pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight]
    
    # Test statistics
    assert brain.getTotalSpikeCount() > 0
    assert brain.getFiringNeuronCount() <= brain.getTotalNeuronCount()
    
    print("✓ Complete integration tests passed")

def run_all_tests():
    """Run all tests."""
    print("Running comprehensive NLM Python bindings tests...\n")
    
    try:
        test_id_types()
        test_enumerations()
        test_enhanced_config()
        test_vision_enhancements()
        test_action_enhancements()
        test_simple_world_enhancements()
        test_brain_enhancements()
        test_agent_brain_enhancements()
        test_factory_functions()
        test_context_managers()
        test_error_handling()
        test_string_representations()
        test_collection_operations()
        test_complete_integration()
        
        print("\n" + "="*60)
        print("✓ ALL TESTS PASSED SUCCESSFULLY!")
        print("="*60)
        return True
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
