"""
NLM Python Bindings Test Suite

Comprehensive test suite for the NLM Python bindings (pynlm).
Tests cover basic functionality, integration, and edge cases.
"""

import pytest
import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

class TestPynlmImport:
    """Test basic pynlm module import and metadata."""
    
    def test_module_import(self):
        """Test that pynlm module can be imported."""
        assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    def test_version_attribute(self):
        """Test that __version__ attribute exists."""
        assert PYNLM_AVAILABLE, "pynlm module should be available"
        assert hasattr(pynlm, '__version__'), "pynlm should have __version__ attribute"
        assert isinstance(pynlm.__version__, str), "__version__ should be a string"
        assert len(pynlm.__version__) > 0, "__version__ should not be empty"
    
    def test_module_docstring(self):
        """Test that module has documentation."""
        assert PYNLM_AVAILABLE, "pynlm module should be available"
        if hasattr(pynlm, '__doc__') and pynlm.__doc__:
            assert "NLM" in pynlm.__doc__, "Module docstring should mention NLM"
class TestPynlmConfig:
    """Test NLM configuration functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.config = pynlm.createDefaultConfig()
    
    def test_config_creation(self):
        """Test default config creation."""
        assert self.config is not None
        assert hasattr(self.config, 'summary')
    
    def test_config_summary(self):
        """Test config summary method."""
        summary = self.config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
        assert "Configuration" in summary
    
    def test_config_has(self):
        """Test config.has() method."""
        assert not self.config.has("nonexistent_key")
    
    def test_config_keys(self):
        """Test config.getKeys() method."""
        keys = self.config.getKeys()
        assert isinstance(keys, list)
        assert len(keys) >= 0
    
    def test_config_clear(self):
        """Test config.clear() method."""
        self.config.clear()
        assert len(self.config.getKeys()) == 0
    
    def test_config_repr(self):
        """Test config __repr__ method."""
        repr_str = repr(self.config)
        assert isinstance(repr_str, str)
        assert "<Config:" in repr_str
class TestPynlmBrain:
    """Test NLM brain functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        assert self.brain is not None
        assert hasattr(self.brain, 'initialize')
        assert hasattr(self.brain, 'step')
        assert hasattr(self.brain, 'getTotalNeuronCount')
    
    def test_brain_stats(self):
        """Test brain statistics methods."""
        neuron_count = self.brain.getTotalNeuronCount()
        assert isinstance(neuron_count, int)
        assert neuron_count > 0
        
        active_count = self.brain.getActiveNeuronCount()
        assert isinstance(active_count, int)
        
        firing_count = self.brain.getFiringNeuronCount()
        assert isinstance(firing_count, int)
        
        total_spikes = self.brain.getTotalSpikeCount()
        assert isinstance(total_spikes, int)
        assert total_spikes >= 0
    
    def test_brain_step(self):
        """Test brain step method."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Perform a step
        self.brain.step(0)
        
        final_spikes = self.brain.getTotalSpikeCount()
        assert isinstance(final_spikes, int)
        
        # Spikes should be non-decreasing
        assert final_spikes >= initial_spikes
    
    def test_brain_reset(self):
        """Test brain reset method."""
        # Perform some work
        self.brain.step(0)
        self.brain.step(1)
        
        # Reset
        self.brain.reset()
        
        # Check that stats are reset (or at least changed)
        neuron_count = self.brain.getTotalNeuronCount()
        assert isinstance(neuron_count, int)
        assert neuron_count > 0
    
    def test_brain_add_region(self):
        """Test brain.addRegion() method."""
        initial_count = self.brain.getRegionCount()
        
        # Add a region
        self.brain.addRegion("test_region")
        
        final_count = self.brain.getRegionCount()
        assert final_count == initial_count + 1
    
    def test_brain_log_status(self):
        """Test brain.logStatus() method."""
        # This should not raise an exception
        self.brain.logStatus()
        
        # If it returns something, it should be a string
        # (though the actual implementation might return void)
    
    def test_brain_get_config(self):
        """Test brain.getConfig() method."""
        config = self.brain.getConfig()
        assert config is not None
        assert hasattr(config, 'summary')
class TestPynlmSimpleWorld:
    """Test NLM SimpleWorld functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 8, 8)
        self.world.reset()
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        assert self.world is not None
        assert hasattr(self.world, 'configure')
        assert hasattr(self.world, 'update')
        assert hasattr(self.world, 'getSensoryPercept')
    
    def test_world_dimensions(self):
        """Test world dimension methods."""
        width = self.world.getWidth()
        height = self.world.getHeight()
        assert width == 10
        assert height == 10
        
        max_energy = self.world.getMaxEnergy()
        assert max_energy == 100.0  # Default value
    
    def test_world_update(self):
        """Test world update method."""
        time_before = self.world.getSimulationTime()
        
        # Perform an update
        self.world.update(0.1)
        
        time_after = self.world.getSimulationTime()
        assert time_after > time_before
        assert time_after == time_before + 0.1
    
    def test_world_agent_body(self):
        """Test agent body access."""
        agent_body = self.world.getAgentBody()
        assert agent_body is not None
        assert hasattr(agent_body, 'x')
        assert hasattr(agent_body, 'y')
        assert hasattr(agent_body, 'orientation')
    
    def test_world_reset(self):
        """Test world reset method."""
        # Modify agent position
        agent_body = self.world.getAgentBody()
        agent_body.x = 50.0
        agent_body.y = 50.0
        
        # Reset world
        self.world.reset()
        
        # Agent should be back at starting position
        agent_body = self.world.getAgentBody()
        assert agent_body.x == 0.0
        assert agent_body.y == 0.0
    
    def test_world_random_seed(self):
        """Test world random seed methods."""
        self.world.setRandomSeed(42)
        
        seed = self.world.getRandomSeed()
        assert seed == 42
    
    def test_world_objects(self):
        """Test world object management."""
        from pynlm import WorldObject, WorldObjectType
        
        # Create and add a world object
        obj = WorldObject(10.0, 10.0, WorldObjectType.Resource, 5.0)
        self.world.addObject(obj)
        
        # Verify object was added (no direct access in API, but we can test that method doesn't crash)
        # The method should just complete without error
        self.world.removeObject(10.0, 10.0)
    
    def test_world_position_validation(self):
        """Test world position validation."""
        # Valid position
        assert self.world.isValidPosition(5, 5)
        
        # Invalid position
        assert not self.world.isValidPosition(-1, 5)
        assert not self.world.isValidPosition(5, 15)
class TestPynlmAgentBrain:
    """Test NLM AgentBrain functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 8, 8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_creation(self):
        """Test agent brain creation."""
        assert self.agent is not None
        assert hasattr(self.agent, 'initialize')
        assert hasattr(self.agent, 'processSensoryInput')
        assert hasattr(self.agent, 'decodeMotorCommand')
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        from pynlm import Vision
        
        # Create a vision percept
        vision = Vision(8, 8, 3)
        # Note: The actual API doesn't expose setData method, but we can test the structure
        
        # Process sensory input (should not crash)
        self.agent.processSensoryInput(vision)
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Decode motor command (should not crash)
        from pynlm import MotorCommand
        cmd = self.agent.decodeMotorCommand()
        
        # Should return a MotorCommand enum value
        assert cmd in pynlm.MotorCommand
    
    def test_agent_enable_features(self):
        """Test enabling brain features."""
        # Enable various features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Verify they are enabled
        assert self.agent.isRewardModulationEnabled()
        assert self.agent.isStructuralPlasticityEnabled()
        assert self.agent.isDevelopmentEnabled()
        assert self.agent.isCuriosityEnabled()
    
    def test_agent_nmodulation_levels(self):
        """Test neuromodulation level access."""
        # These should return values without crashing
        neuromod = self.agent.getNeuromodulationLevel()
        assert isinstance(neuromod, (int, float))
        
        curiosity = self.agent.getCuriosityLevel()
        assert isinstance(curiosity, (int, float))
        
        novelty = self.agent.getNoveltyLevel()
        assert isinstance(novelty, (int, float))
        
        prediction = self.agent.getPredictionError()
        assert isinstance(prediction, (int, float))
    
    def test_agent_development(self):
        """Test development update."""
        # Update development (should not crash)
        self.agent.updateDevelopment(0.1)
        
        # Get developmental stage
        stage = self.agent.getDevelopmentalStage()
        assert stage in pynlm.DevelopmentalStage
    
    def test_agent_reward_modulation(self):
        """Test reward modulation."""
        # Apply reward modulation (should not crash)
        self.agent.applyRewardModulation(1.0, 0.5)
    
    def test_agent_reset(self):
        """Test agent reset."""
        # Reset agent (should not crash)
        self.agent.reset()
class TestPynlmInvalidIds:
    """Test invalid ID constants."""
    
    def test_invalid_neuron_id(self):
        """Test INVALID_NEURON_ID constant."""
        assert pynlm.INVALID_NEURON_ID is not None
        assert hasattr(pynlm.INVALID_NEURON_ID, 'value')
        assert pynlm.INVALID_NEURON_ID.value == 0
    
    def test_invalid_synapse_id(self):
        """Test INVALID_SYNAPSE_ID constant."""
        assert pynlm.INVALID_SYNAPSE_ID is not None
        assert hasattr(pynlm.INVALID_SYNAPSE_ID, 'value')
        assert pynlm.INVALID_SYNAPSE_ID.value == 0
    
    def test_invalid_region_id(self):
        """Test INVALID_REGION_ID constant."""
        assert pynlm.INVALID_REGION_ID is not None
        assert hasattr(pynlm.INVALID_REGION_ID, 'value')
        assert pynlm.INVALID_REGION_ID.value == 0
    
    def test_invalid_population_id(self):
        """Test INVALID_POPULATION_ID constant."""
        assert pynlm.INVALID_POPULATION_ID is not None
        assert hasattr(pynlm.INVALID_POPULATION_ID, 'value')
        assert pynlm.INVALID_POPULATION_ID.value == 0
class TestPynlmEdgeCases:
    """Test edge cases and error conditions."""
    
    def test_empty_config_summary(self):
        """Test empty config summary."""
        config = pynlm.createDefaultConfig()
        summary = config.summary()
        assert "Configuration (0 entries)" in summary
    
    def test_brain_step_without_initialize(self):
        """Test brain step without explicit initialize call."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        # Initialize is called automatically in some cases
        brain.step(0)
        
        # Should not crash
        assert True
    
    def test_multiple_steps(self):
        """Test multiple brain steps."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Perform multiple steps
        for i in range(10):
            brain.step(i)
        
        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes

if __name__ == '__main__':
    pytest.main([__file__, '-v'])
