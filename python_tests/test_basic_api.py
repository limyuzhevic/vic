import pytest
import pynlm
import time
import statistics

class TestBasicAPI:
    """Test basic NLM API functionality"""

    def test_config_creation(self):
        """Test default configuration creation"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("brain.neuron_count")
        assert config.get("brain.neuron_count") == 1000

    def test_brain_creation(self, default_config):
        """Test brain creation with configuration"""
        brain = pynlm.createBrain(default_config)
        assert brain is not None
        
        brain.initialize()
        assert brain.getTotalNeuronCount() == 1000
        assert brain.getRegionCount() == 1
        assert brain.getTotalSynapseCount() > 0

    def test_brain_step(self, small_brain):
        """Test basic brain stepping"""
        initial_spikes = small_brain.getTotalSpikeCount()
        
        for step in range(10):
            small_brain.step(step)
        
        final_spikes = small_brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes

    def test_agent_creation(self, small_brain, test_world):
        """Test agent brain creation"""
        agent = pynlm.createAgentBrain(small_brain)
        assert agent is not None
        
        agent.initialize(test_world)
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0

    def test_world_interaction(self, small_brain, test_world, test_agent):
        """Test brain-world-agent interaction"""
        # Initial state
        assert test_world.getAgentBody().x == 0.0
        assert test_world.getAgentBody().y == 0.0
        
        # Run one simulation step
        test_world.update(0.1)
        percept = test_world.getSensoryPercept()
        test_agent.processSensoryInput(percept)
        small_brain.step(0)
        
        # Motor command
        action = test_agent.decodeMotorCommand()
        assert action is not None
        
        # Apply action
        test_world.applyMotorCommand(action, test_world.getSimulationTime())
        
        # Check that world state changed
        assert test_world.getAgentBody().x != 0.0 or test_world.getAgentBody().y != 0.0

    def test_error_handling(self):
        """Test error handling for invalid operations"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test uninitialized brain step
        uninitialized_brain = pynlm.createBrain(config)
        # Should raise exception when trying to step
        with pytest.raises(Exception):
            uninitialized_brain.step(0)

    def test_version_info(self):
        """Test version information access"""
        # Check that pynlm module has expected attributes
        assert hasattr(pynlm, '__version__')
        assert hasattr(pynlm, 'createDefaultConfig')
        assert hasattr(pynlm, 'createBrain')
        assert hasattr(pynlm, 'createSimpleWorld')
        assert hasattr(pynlm, 'createAgentBrain')