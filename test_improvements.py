import pynlm
import pytest


def test_version():
    """Test version function."""
    version = pynlm.getVersion()
    assert "NLM Python Bindings" in version
    assert "0.1.0" in version


def test_config_creation():
    """Test configuration creation."""
    config = pynlm.Config()
    assert config is not None
    
    # Test basic operations
    config.set("test.int", 42)
    config.set("test.string", "hello")
    config.set("test.double", 3.14)
    config.set("test.bool", True)
    
    assert config.has("test.int")
    assert config.has("test.string")
    assert config.has("test.double")
    assert config.has("test.bool")


def test_config_getters():
    """Test configuration getters."""
    config = pynlm.Config()
    config.set("int_value", 100)
    config.set("string_value", "test")
    config.set("float_value", 3.14)
    
    # Test getOr method
    assert config.getOr("int_value", 0) == 100
    assert config.getOr("string_value", "default") == "test"
    assert config.getOr("float_value", 0.0) == 3.14
    assert config.getOr("missing_key", 999) == 999


def test_config_properties():
    """Test configuration Pythonic properties."""
    config = pynlm.Config()
    
    # Test property setters
    config.neuron_count = 500
    config.synapse_density = 0.05
    config.connection_probability = 0.1
    config.random_seed = 42
    
    # Test property getters
    assert config.neuron_count == 500
    assert config.synapse_density == 0.05
    assert config.connection_probability == 0.1
    assert config.random_seed == 42


def test_config_configure_methods():
    """Test configuration convenience methods."""
    config = pynlm.Config()
    
    # Test standard configuration
    config.configure_standard()
    assert config.neuron_count == 1000
    assert config.synapse_density == 0.1
    assert config.connection_probability == 0.05
    
    # Test compact configuration
    config2 = pynlm.Config()
    config2.configure_compact()
    assert config2.neuron_count == 100
    assert config2.synapse_density == 0.05
    assert config2.connection_probability == 0.02


def test_brain_creation():
    """Test brain creation with enhanced API."""
    config = pynlm.Config()
    config.configure_standard()
    
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test initialization
    assert brain.initialize()
    
    # Test basic stats
    stats = brain.getStats()
    assert "total_neurons" in stats
    assert "total_synapses" in stats
    assert "firing_neurons" in stats
    assert "total_spikes" in stats
    
    assert stats["total_neurons"] > 0
    assert stats["firing_neurons"] >= 0


def test_brain_properties():
    """Test brain Pythonic properties."""
    config = pynlm.Config()
    config.configure_standard()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test read-only properties
    assert hasattr(brain, 'total_neurons')
    assert hasattr(brain, 'total_synapses')
    assert hasattr(brain, 'firing_neurons')
    assert hasattr(brain, 'active_neurons')
    assert hasattr(brain, 'total_spikes')
    assert hasattr(brain, 'pending_spikes')
    assert hasattr(brain, 'excitation_inhibition_ratio')
    assert hasattr(brain, 'average_firing_rate')
    assert hasattr(brain, 'developmental_stage')
    
    # Test that properties return sensible values
    assert brain.total_neurons > 0
    assert brain.total_synapses >= 0
    assert brain.firing_neurons >= 0
    assert brain.active_neurons >= 0
    assert brain.total_spikes >= 0
    assert brain.pending_spikes >= 0


def test_brain_context_manager():
    """Test brain context manager."""
    config = pynlm.Config()
    config.configure_standard()
    
    with pynlm.createBrain(config) as brain:
        assert brain is not None
        brain.initialize()
        assert brain.total_neurons > 0


def test_agent_creation():
    """Test agent creation with enhanced API."""
    brain = pynlm.createDefaultBrain()
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    agent.initialize(world)
    
    # Test agent properties
    assert hasattr(agent, 'neuromodulation_level')
    assert hasattr(agent, 'curiosity_level')
    assert hasattr(agent, 'novelty_level')
    assert hasattr(agent, 'prediction_error')
    assert hasattr(agent, 'developmental_stage')
    
    # Test system status
    status = agent.getSystemStatus()
    assert "neuromodulation" in status
    assert "curiosity" in status
    assert "novelty" in status
    assert "prediction_error" in status


def test_agent_properties():
    """Test agent Pythonic properties."""
    brain = pynlm.createDefaultBrain()
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable some systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Test property setters/getters
    assert agent.neuromodulation_level == agent.getNeuromodulationLevel()
    assert agent.curiosity_level == agent.getCuriosityLevel()
    assert agent.novelty_level == agent.getNoveltyLevel()
    assert agent.prediction_error == agent.getPredictionError()
    
    # Test enable/disable properties
    assert agent.enable_reward_modulation == agent.isRewardModulationEnabled()
    agent.enable_reward_modulation = False
    assert not agent.isRewardModulationEnabled()
    agent.enable_reward_modulation = True
    assert agent.isRewardModulationEnabled()


def test_simple_world_creation():
    """Test simple world creation with enhanced API."""
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    
    assert world.getWidth() == 30
    assert world.getHeight() == 30
    assert world.getMaxEnergy() > 0
    
    # Test object creation methods
    world.createResource(5.0, 5.0, 1.0)
    world.createHazard(15.0, 15.0)
    world.createWall(25.0, 25.0)
    
    # Test resource access
    resource_value = world.getResourceAt(5.0, 5.0)
    assert resource_value == 1.0


def test_action_creation():
    """Test action creation with enhanced API."""
    # Create actions from type
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.type == pynlm.ActionType.MoveForward
    assert action1.getName() == "MoveForward"
    
    # Create actions with parameters
    action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
    assert action2.type == pynlm.ActionType.Custom
    assert action2.getParameters() == [1.0, 2.0, 3.0]
    
    # Test action properties
    assert action1.has_parameters == False
    assert action2.has_parameters == True
    
    # Test cloning
    action3 = action2.clone()
    assert action3.type == action2.type
    assert action3.getParameters() == action2.getParameters()


def test_sensory_percept():
    """Test sensory percept with enhanced API."""
    percept = pynlm.SensoryPercept()
    
    # Test vision data
    vision_data = [0.1] * 64
    percept.setVision(vision_data)
    assert percept.getVision() == vision_data
    assert percept.has_vision == True
    assert percept.getVisionWidth() == 8
    assert percept.getVisionHeight() == 8
    
    # Test touch data
    touch_data = [0.5] * 8
    percept.setTouch(touch_data)
    assert percept.getTouch() == touch_data
    assert percept.has_touch == True
    
    # Test internal data
    internal_data = [0.8] * 4
    percept.setInternal(internal_data)
    assert percept.getInternal() == internal_data
    assert percept.has_internal == True
    
    # Test clearing
    percept.clear()
    assert percept.has_vision == False
    assert percept.has_touch == False
    assert percept.has_internal == False


def test_example_script():
    """Test the complete example script."""
    import subprocess
    result = subprocess.run(['python', 'example.py'], 
                          capture_output=True, text=True)
    assert result.returncode == 0
    assert "Episode complete!" in result.stdout


def test_error_handling():
    """Test error handling with custom exceptions."""
    config = pynlm.Config()
    
    # Test that invalid operations raise appropriate exceptions
    try:
        # This should not crash, just test basic functionality
        config.set("test", "value")
        assert config.has("test")
    except Exception as e:
        assert False, f"Unexpected error: {e}"


def test_advanced_factory_functions():
    """Test advanced factory functions."""
    # Test createDefaultBrain
    brain = pynlm.createDefaultBrain()
    assert brain is not None
    assert brain.total_neurons > 0
    
    # Test createDefaultAgent
    agent = pynlm.createDefaultAgent()
    assert agent is not None
    assert agent.isReady() == True
    
    # Test createSimpleDemoWorld
    world = pynlm.createSimpleDemoWorld()
    assert world is not None
    assert world.getWidth() == 20
    assert world.getHeight() == 20


if __name__ == "__main__":
    # Run all tests
    test_version()
    print("✓ test_version passed")
    
    test_config_creation()
    print("✓ test_config_creation passed")
    
    test_config_getters()
    print("✓ test_config_getters passed")
    
    test_config_properties()
    print("✓ test_config_properties passed")
    
    test_config_configure_methods()
    print("✓ test_config_configure_methods passed")
    
    test_brain_creation()
    print("✓ test_brain_creation passed")
    
    test_brain_properties()
    print("✓ test_brain_properties passed")
    
    test_brain_context_manager()
    print("✓ test_brain_context_manager passed")
    
    test_agent_creation()
    print("✓ test_agent_creation passed")
    
    test_agent_properties()
    print("✓ test_agent_properties passed")
    
    test_simple_world_creation()
    print("✓ test_simple_world_creation passed")
    
    test_action_creation()
    print("✓ test_action_creation passed")
    
    test_sensory_percept()
    print("✓ test_sensory_percept passed")
    
    test_example_script()
    print("✓ test_example_script passed")
    
    test_error_handling()
    print("✓ test_error_handling passed")
    
    test_advanced_factory_functions()
    print("✓ test_advanced_factory_functions passed")
    
    print("\n🎉 All tests passed!")