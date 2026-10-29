"""
Basic NLM Python tests for core functionality.

Tests for brain creation, initialization, and basic simulation steps.
"""

import pytest
import pynlm


class TestBrainCreation:
    """Test brain creation and initialization."""
    
    def test_brain_creation(self):
        """Test basic brain creation."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
    
    def test_default_brain_config(self):
        """Test brain with default configuration."""
        config = pynlm.createDefaultConfig()
        assert config.has("random_seed")
        assert config.has("simulation_timestep")
        assert config.has("neuron_count")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() == config.getInt("neuron_count", 0)


class TestBrainSimulation:
    """Test brain simulation operations."""
    
    def test_brain_step(self):
        """Test brain simulation steps."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        initial_firing = brain.getFiringNeuronCount()
        
        # Run simulation
        brain.step(10)
        
        final_spikes = brain.getTotalSpikeCount()
        final_firing = brain.getFiringNeuronCount()
        
        # Spike count should be non-decreasing
        assert final_spikes >= initial_spikes
        # Firing count should be reasonable
        assert 0 <= final_firing <= brain.getTotalNeuronCount()
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run some steps
        for i in range(10):
            brain.step(i)
        
        initial_spikes = brain.getTotalSpikeCount()
        assert initial_spikes > 0
        
        # Reset brain
        brain.reset()
        brain.initialize()
        
        # After reset, spike count should be reset (or at least reasonable)
        reset_spikes = brain.getTotalSpikeCount()
        assert reset_spikes >= 0
    
    def test_multiple_brain_steps(self):
        """Test running multiple simulation steps."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run many steps
        for step in range(100):
            brain.step(step)
        
        final_spikes = brain.getTotalSpikeCount()
        
        # Should have made progress
        assert final_spikes >= initial_spikes
    
    def test_step_with_time(self):
        """Test brain step with explicit time."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Step with specific time
        brain.step(5, 0.05)
        
        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes


class TestBrainStatistics:
    """Test brain statistics and metrics."""
    
    def test_brain_statistics(self):
        """Test comprehensive brain statistics."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Collect statistics
        stats = {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage(),
        }
        
        # Validate statistics
        assert stats['total_neurons'] > 0
        assert stats['total_synapses'] > 0
        assert 0 <= stats['active_neurons'] <= stats['total_neurons']
        assert 0 <= stats['firing_neurons'] <= stats['total_neurons']
        assert stats['total_spikes'] >= 0
        assert 0.0 <= stats['average_firing_rate'] <= 1.0
        assert 0.0 <= stats['e_i_ratio'] <= 10.0
        assert stats['developmental_stage'] is not None
    
    def test_brain_performance_stats(self):
        """Test brain performance statistics."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        for i in range(20):
            brain.step(i)
        
        # Get performance stats
        perf_stats = brain.getPerformanceStats()
        
        assert 'totalNeurons' in perf_stats
        assert 'totalSynapses' in perf_stats
        assert 'activeNeurons' in perf_stats
        assert 'firingNeurons' in perf_stats
        assert 'totalSpikes' in perf_stats
        assert 'averageFiringRate' in perf_stats
        assert 'eI_ratio' in perf_stats
        
        # Validate values
        assert perf_stats['totalNeurons'] > 0
        assert perf_stats['totalSynapses'] > 0
        assert 0 <= perf_stats['activeNeurons'] <= perf_stats['totalNeurons']
        assert 0 <= perf_stats['firingNeurons'] <= perf_stats['totalNeurons']
        assert perf_stats['totalSpikes'] >= 0
        assert 0.0 <= perf_stats['averageFiringRate'] <= 1.0
        assert 0.0 <= perf_stats['eI_ratio'] <= 10.0


class TestConfiguration:
    """Test configuration system."""
    
    def test_default_config_creation(self):
        """Test default configuration creation."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Check essential keys
        assert config.has("random_seed")
        assert config.has("simulation_timestep")
        assert config.has("neuron_count")
        assert config.has("region_count")
        assert config.has("connection_probability")
    
    def test_config_set_get_string(self):
        """Test string configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Set and get string value
        config.setString("test.key", "test_value")
        assert config.getString("test.key", "") == "test_value"
        
        # Test default value
        assert config.getString("nonexistent.key", "default") == "default"
    
    def test_config_set_get_int(self):
        """Test integer configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Set and get integer value
        config.setInt("test.int_key", 42)
        assert config.getInt("test.int_key", 0) == 42
        
        # Test default value
        assert config.getInt("nonexistent.key", 99) == 99
    
    def test_config_set_get_float(self):
        """Test float configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Set and get float value
        config.setFloat("test.float_key", 3.14)
        assert config.getFloat("test.float_key", 0.0) == 3.14
        
        # Test default value
        assert config.getFloat("nonexistent.key", 0.0) == 0.0
    
    def test_config_set_get_bool(self):
        """Test boolean configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Set and get boolean value
        config.setBool("test.bool_key", True)
        assert config.getBool("test.bool_key", False) == True
        
        config.setBool("test.bool_key2", False)
        assert config.getBool("test.bool_key2", True) == False
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Test with valid required keys
        valid_keys = ["random_seed", "simulation_timestep"]
        assert config.validate(valid_keys)
        
        # Test with invalid required key
        with pytest.raises(py.value_error):
            config.validate(["random_seed", "nonexistent.key"])
    
    def test_config_merge(self):
        """Test configuration merging."""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        # Modify config2
        config2.setString("extra.key", "extra_value")
        
        # Merge config2 into config1
        config1.mergeConfig(config2)
        
        # Check that config1 has config2's values
        assert config1.getString("extra.key", "") == "extra_value"
        assert config1.has("random_seed")  # From original config1


class TestConfigurationTemplates:
    """Test configuration templates and presets."""
    
    def test_brain_config_template(self):
        """Test brain configuration template."""
        config = pynlm.createDefaultConfig()
        config.createBrainConfig(neuronCount=1000, regionCount=2, connectionProb=0.1)
        
        assert config.getInt("brain.neuron_count", 0) == 1000
        assert config.getInt("brain.region_count", 0) == 2
        assert config.getFloat("brain.connection_probability", 0.0) == 0.1
    
    def test_agent_config_template(self):
        """Test agent configuration template."""
        config = pynlm.createDefaultConfig()
        config.createAgentConfig()
        
        assert config.getBool("agent.reward_modulation", False) == True
        assert config.getBool("agent.structural_plasticity", False) == True
        assert config.getBool("agent.development", False) == True
        assert config.getBool("agent.curiosity", False) == True
        assert config.getBool("agent.planning", False) == True
    
    def test_world_config_template(self):
        """Test world configuration template."""
        config = pynlm.createDefaultConfig()
        config.createWorldConfig(width=30, height=30, visionWidth=16, visionHeight=16)
        
        assert config.getInt("world.width", 0) == 30
        assert config.getInt("world.height", 0) == 30
        assert config.getInt("world.vision_width", 0) == 16
        assert config.getInt("world.vision_height", 0) == 16


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
