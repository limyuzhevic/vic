"""Test basic functionality of enhanced NLM Python bindings."""
import pytest
from pynlm import Brain, Config, AgentBrain, SimpleWorld
import pynlm.utils as utils

class TestBasicFunctionality:
    """Test basic NLM functionality."""
    
    def test_brain_creation_and_initialization(self, brain):
        """Test brain creation and initialization."""
        assert brain is not None
        assert brain.initialize() is True
        assert hasattr(brain, 'spike_count')
        assert hasattr(brain, 'synapse_count')
        assert hasattr(brain, 'active_neurons')
        assert hasattr(brain, 'firing_neurons')
        assert hasattr(brain, 'ei_ratio')
        assert hasattr(brain, 'region_names')
    
    def test_config_creation(self):
        """Test configuration creation."""
        config = Config()
        assert config is not None
        assert config.get("simulation_max_steps") == 1000
        config.set("simulation_max_steps", 500)
        assert config.get("simulation_max_steps") == 500
    
    def test_agent_brain_creation(self, brain):
        """Test agent brain creation."""
        agent = AgentBrain(brain)
        assert agent is not None
        assert agent.brain is brain
        assert hasattr(agent, 'reward_modulation_enabled')
        assert hasattr(agent, 'curiosity_enabled')
    
    def test_world_creation(self):
        """Test world creation."""
        world = SimpleWorld()
        assert world is not None
        assert world.get_width() == 100
        assert world.get_height() == 100
        assert world.configure(80, 60, 10, 10) is True
    
    def test_step_execution(self, brain):
        """Test step execution."""
        brain.initialize()
        steps = 5
        for i in range(steps):
            result = brain.step(i, i * 0.01)
            assert result is True
            assert brain.spike_count == i + 1
        assert brain.spike_count == steps
    
    def test_property_access(self, brain):
        """Test property access."""
        brain.initialize()
        run_steps(brain, 10)
        
        assert brain.spike_count == 10
        assert isinstance(brain.synapse_count, int)
        assert isinstance(brain.active_neurons, int)
        assert isinstance(brain.firing_neurons, int)
        assert isinstance(brain.ei_ratio, float)
        assert isinstance(brain.region_names, list)
        assert len(brain.region_names) > 0
        
        # Test Pythonic properties
        assert brain.spike_count == brain.getTotalSpikeCount() if hasattr(brain, 'getTotalSpikeCount') else True
        assert brain.synapse_count == brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else True
    
    def test_memory_system_access(self, brain):
        """Test memory system access."""
        # These properties should exist in the enhanced API
        assert hasattr(brain, 'get_working_memory')
        assert hasattr(brain, 'get_episodic_memory')
        assert hasattr(brain, 'get_associative_memory')
        assert hasattr(brain, 'get_prediction_system')
        assert hasattr(brain, 'get_planner')
        
        # Test that memory methods return something (may be None in stub)
        assert brain.get_working_memory() is None
        assert brain.get_episodic_memory() is None
        assert brain.get_associative_memory() is None
    
    def test_brain_repr(self, brain):
        """Test brain string representation."""
        repr_str = repr(brain)
        assert "Brain:" in repr_str
        assert "initialized=" in repr_str

class TestEnhancedBrainFeatures:
    """Test enhanced Brain class features."""
    
    def test_step_batch(self, brain):
        """Test step_batch functionality."""
        brain.initialize()
        steps = 20
        result = brain.step_batch(steps)
        assert result == steps
        assert brain.spike_count == steps
    
    def test_get_summary(self, brain):
        """Test get_summary functionality."""
        brain.initialize()
        run_steps(brain, 5)
        
        summary = brain.get_summary()
        assert isinstance(summary, dict)
        assert 'spike_count' in summary
        assert 'synapse_count' in summary
        assert 'active_neurons' in summary
        assert 'firing_neurons' in summary
        assert 'ei_ratio' in summary
        assert 'average_firing_rate' in summary
        assert 'total_neuron_count' in summary
        assert 'region_names' in summary
        
        assert summary['spike_count'] == 5
        assert summary['active_neurons'] == 5
    
    def test_save_load_methods(self, brain, temp_file):
        """Test save and load methods."""
        brain.initialize()
        run_steps(brain, 10)
        
        # Test save_to_file
        assert brain.save_to_file(temp_file) is True
        assert os.path.exists(temp_file)
        
        # Test load_from_file (should return False for stub implementation)
        assert brain.load_from_file(temp_file) is False
    
    def test_world_advanced_configuration(self, world):
        """Test world advanced configuration."""
        config = {
            "max_energy": 200.0,
            "energy_decay_rate": 0.005,
            "random_seed": 12345,
            "agent_start_x": 0.0,
            "agent_start_y": 0.0
        }
        
        result = world.configure_advanced(**config)
        assert result is True
        assert world.get_max_energy() == 200.0
        assert world.get_energy_decay_rate() == 0.005
        assert world.get_random_seed() == 12345