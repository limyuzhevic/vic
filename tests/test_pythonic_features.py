"""Test Pythonic features of enhanced NLM Python bindings."""
import pytest
from pynlm import Brain, Config, AgentBrain, SimpleWorld
import pynlm.utils as utils

class TestPythonicFeatures:
    """Test Pythonic properties and features."""
    
    def test_brain_pythonic_properties(self, brain):
        """Test Brain Pythonic properties."""
        brain.initialize()
        
        # Test Pythonic property access (as per enhanced API documentation)
        # These properties should exist and return expected types
        assert hasattr(brain, 'spike_count')
        assert isinstance(brain.spike_count, int)
        
        assert hasattr(brain, 'synapse_count')
        assert isinstance(brain.synapse_count, int)
        
        assert hasattr(brain, 'active_neurons')
        assert isinstance(brain.active_neurons, int)
        
        assert hasattr(brain, 'firing_neurons')
        assert isinstance(brain.firing_neurons, int)
        
        assert hasattr(brain, 'ei_ratio')
        assert isinstance(brain.ei_ratio, float)
        
        assert hasattr(brain, 'region_names')
        assert isinstance(brain.region_names, list)
        assert len(brain.region_names) > 0
        
        # Test additional Pythonic properties
        assert hasattr(brain, 'average_firing_rate')
        assert isinstance(brain.average_firing_rate, float)
        
        assert hasattr(brain, 'total_neuron_count')
        assert isinstance(brain.total_neuron_count, int)
        
        assert hasattr(brain, 'total_synapse_count')
        assert isinstance(brain.total_synapse_count, int)
        
        assert hasattr(brain, 'active_neuron_count')
        assert isinstance(brain.active_neuron_count, int)
        
        assert hasattr(brain, 'firing_neuron_count')
        assert isinstance(brain.firing_neuron_count, int)
    
    def test_agent_pythonic_properties(self, agent_brain):
        """Test AgentBrain Pythonic properties."""
        agent = agent_brain
        
        # Test Pythonic property access for AgentBrain
        assert hasattr(agent, 'reward_modulation_enabled')
        assert isinstance(agent.reward_modulation_enabled, bool)
        
        assert hasattr(agent, 'curiosity_enabled')
        assert isinstance(agent.curiosity_enabled, bool)
        
        # Test enable methods
        agent.enable_reward_modulation(True)
        assert agent.reward_modulation_enabled is True
        
        agent.enable_curiosity(True)
        assert agent.curiosity_enabled is True
        
        # Test disable methods
        agent.enable_reward_modulation(False)
        assert agent.reward_modulation_enabled is False
        
        agent.enable_curiosity(False)
        assert agent.curiosity_enabled is False
    
    def test_batch_operations(self, brain):
        """Test batch operations (step_batch, process_sensory_batch)."""
        brain.initialize()
        
        # Test step_batch
        steps = 50
        result = brain.step_batch(steps)
        assert result == steps
        assert brain.spike_count == steps
        
        # Test process_sensory_batch on agent
        agent = AgentBrain(brain)
        agent.initialize(SimpleWorld())
        
        # Create mock sensory inputs
        percepts = [i for i in range(10)]
        result = agent.process_sensory_batch(percepts)
        assert isinstance(result, list)
        assert len(result) == len(percepts)
    
    def test_summary_generation(self, brain):
        """Test summary generation."""
        brain.initialize()
        run_steps(brain, 25)
        
        summary = brain.get_summary()
        assert isinstance(summary, dict)
        
        # Verify all expected keys are present
        expected_keys = [
            'initialized', 'spike_count', 'synapse_count', 
            'active_neurons', 'firing_neurons', 'ei_ratio',
            'average_firing_rate', 'total_neuron_count',
            'developmental_stage', 'pending_spike_events',
            'region_names'
        ]
        
        for key in expected_keys:
            assert key in summary, f"Missing key: {key}"
        
        # Verify data types and values
        assert isinstance(summary['initialized'], bool)
        assert isinstance(summary['spike_count'], int)
        assert isinstance(summary['synapse_count'], int)
        assert isinstance(summary['active_neurons'], int)
        assert isinstance(summary['firing_neurons'], int)
        assert isinstance(summary['ei_ratio'], float)
        assert isinstance(summary['average_firing_rate'], float)
        assert isinstance(summary['total_neuron_count'], int)
        assert isinstance(summary['pending_spike_events'], int)
        assert isinstance(summary['region_names'], list)
        
        assert summary['spike_count'] == 25
    
    def test_world_advanced_configuration(self, world):
        """Test world advanced configuration."""
        config = {
            "max_energy": 200.0,
            "energy_decay_rate": 0.005,
            "random_seed": 12345,
            "agent_start_x": 0.0,
            "agent_start_y": 0.0,
            "world_objects": []
        }
        
        result = world.configure_advanced(**config)
        assert result is True
        
        # Verify configuration was applied
        assert world.get_max_energy() == 200.0
        assert world.get_energy_decay_rate() == 0.005
        assert world.get_random_seed() == 12345
    
    def test_file_operations_convenience_wrappers(self, brain, temp_file):
        """Test file operation convenience wrappers."""
        brain.initialize()
        run_steps(brain, 10)
        
        # Test save_to_file convenience wrapper
        assert brain.save_to_file(temp_file) is True
        assert os.path.exists(temp_file)
        
        # Test load_from_file convenience wrapper (stub returns False)
        assert brain.load_from_file(temp_file) is False

class TestUtilityFunctions:
    """Test utility functions and Pythonic API."""
    
    def test_version_info(self):
        """Test get_version_info utility."""
        version_info = utils.get_version_info()
        assert isinstance(version_info, dict)
        assert 'version' in version_info
        assert 'python_version' in version_info
        assert isinstance(version_info['version'], str)
        assert isinstance(version_info['python_version'], str)
    
    def test_validate_brain(self, brain):
        """Test validate_brain utility."""
        brain.initialize()
        result = utils.validate_brain(brain)
        assert isinstance(result, bool)
        # The stub implementation should return True
        assert result is True
    
    def test_get_memory_usage(self):
        """Test get_memory_usage utility."""
        memory_info = utils.get_memory_usage()
        assert isinstance(memory_info, dict)
        assert 'rss_mb' in memory_info
        assert 'vms_mb' in memory_info
        assert isinstance(memory_info['rss_mb'], (int, float))
        assert isinstance(memory_info['vms_mb'], (int, float))
    
    def test_error_handling_context(self):
        """Test ErrorHandlingContext utility."""
        context = utils.ErrorHandlingContext()
        assert context is not None
        assert hasattr(context, 'last_error')
        assert hasattr(context, '__enter__')
        assert hasattr(context, '__exit__')
        
        # Test context manager
        with context:
            assert context.last_error is None