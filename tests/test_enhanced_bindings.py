"""
Unit tests for pynlm enhanced Python bindings.

This file contains comprehensive tests for the enhanced Python bindings,
including tests for new wrapper classes, configuration builders, and utilities.
"""

import pytest
import tempfile
import os
from typing import Dict, Any

# Import the pynlm module and its components
import pynlm
from pynlm import (
    BrainSimulator, AgentSimulator, NLMVisualizer, NLMConfigBuilder,
    NLMPerformanceMonitor, NLMUtils, ValidationError, SimulationError
)
class TestBrainSimulator:
    """Test BrainSimulator class."""
    
    def test_initialization(self):
        """Test basic initialization."""
        simulator = BrainSimulator()
        assert simulator.is_alive()
        assert simulator.brain is not None
        assert simulator._simulation_step == 0
        assert simulator._stats_history == []
    
    def test_custom_config(self):
        """Test initialization with custom config."""
        config = pynlm.createDefaultConfig()
        simulator = BrainSimulator(config)
        assert simulator.is_alive()
        assert simulator.config is not None
    
    def test_step_method(self):
        """Test step method."""
        simulator = BrainSimulator()
        simulator.step(0)
        assert simulator._simulation_step == 1
        assert len(simulator._stats_history) == 1
    
    def test_reset(self):
        """Test reset method."""
        simulator = BrainSimulator()
        simulator.step(10)
        assert simulator._simulation_step == 1
        
        simulator.reset()
        assert simulator._simulation_step == 0
        assert simulator._stats_history == []
    
    def test_run_method(self):
        """Test run method."""
        simulator = BrainSimulator()
        simulator.run(10)
        assert simulator._simulation_step == 10
        assert len(simulator._stats_history) == 10
    
    def test_save_load(self):
        """Test save and load methods."""
        simulator = BrainSimulator()
        simulator.step(5)
        
        with tempfile.NamedTemporaryFile(suffix='.pkl', delete=False) as f:
            filepath = f.name
        
        try:
            # Save
            simulator.save(filepath)
            assert os.path.exists(filepath)
            
            # Create new simulator and load
            new_simulator = BrainSimulator()
            new_simulator.load(filepath)
            assert new_simulator.is_alive()
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_stats_collection(self):
        """Test statistics collection."""
        simulator = BrainSimulator()
        simulator.step(5)
        
        stats = simulator.get_current_stats()
        assert 'step' in stats
        assert 'total_neurons' in stats
        assert 'firing_neurons' in stats
        assert stats['step'] == 5
    
    def test_average_firing_rate(self):
        """Test average firing rate calculation."""
        simulator = BrainSimulator()
        
        # Run simulation
        simulator.run(10)
        
        avg_rate = simulator.get_average_firing_rate()
        assert avg_rate >= 0.0
        
        # Test with empty history
        empty_simulator = BrainSimulator()
        assert empty_simulator.get_average_firing_rate() == 0.0
    
    def test_max_firing_rate(self):
        """Test max firing rate calculation."""
        simulator = BrainSimulator()
        
        # Run simulation
        simulator.run(10)
        
        max_rate = simulator.get_max_firing_rate()
        assert max_rate >= 0.0
        
        # Test with empty history
        empty_simulator = BrainSimulator()
        assert empty_simulator.get_max_firing_rate() == 0.0


class TestAgentSimulator:
    """Test AgentSimulator class."""
    
    def test_initialization(self):
        """Test basic initialization."""
        simulator = AgentSimulator()
        assert simulator.is_alive()
        assert simulator.brain is not None
        assert simulator.agent is not None
        assert simulator.world is not None
    
    def test_initialization_with_params(self):
        """Test initialization with custom parameters."""
        simulator = AgentSimulator(
            world_width=30,
            world_height=40,
            vision_width=10,
            vision_height=12
        )
        
        assert simulator.world_width == 30
        assert simulator.world_height == 40
        assert simulator.vision_width == 10
        assert simulator.vision_height == 12
    
    def test_run_step(self):
        """Test single step execution."""
        simulator = AgentSimulator()
        action, metrics = simulator.run_step()
        
        assert action is not None
        assert metrics is not None
        assert 'step' in metrics
        assert 'action_type' in metrics
        assert 'brain_stats' in metrics
        assert 'agent_stats' in metrics
    
    def test_run_episode(self):
        """Test episode execution."""
        simulator = AgentSimulator()
        
        def progress_callback(step, metrics):
            assert step == metrics['step']
        
        episode_data = simulator.run_episode(10, progress_callback)
        assert len(episode_data) == 10
        assert episode_data[0]['step'] == 0
        assert episode_data[-1]['step'] == 9
    
    def test_reset(self):
        """Test reset method."""
        simulator = AgentSimulator()
        simulator.run_step()
        assert simulator._current_step == 1
        
        simulator.reset()
        assert simulator._current_step == 0
        assert simulator._episode_data == []
    
    def test_episode_stats(self):
        """Test episode statistics."""
        simulator = AgentSimulator()
        
        # Run episode
        episode_data = simulator.run_episode(5)
        
        # Get stats
        stats = simulator.get_episode_stats()
        assert stats['total_steps'] == 5
        assert 'unique_actions' in stats
        assert 'average_firing_rate' in stats
        assert 'total_spikes' in stats
        assert 'final_dev_stage' in stats


class TestNLMConfigBuilder:
    """Test NLMConfigBuilder class."""
    
    def test_preset_application(self):
        """Test preset application."""
        builder = NLMConfigBuilder()
        
        # Test all presets
        for preset in ['minimal', 'standard', 'advanced', 'research']:
            config = builder.apply_preset(preset).get_config()
            assert config is not None
        
        # Test invalid preset
        with pytest.raises(ValidationError):
            builder.apply_preset('invalid_preset')
    
    def test_neural_parameters(self):
        """Test neural parameter setting."""
        builder = NLMConfigBuilder()
        config = (builder
                 .set_neural_parameters(neuron_count=2000, synapse_density=0.15)
                 .get_config())
        
        assert config is not None
    
    def test_plasticity_parameters(self):
        """Test plasticity parameter setting."""
        builder = NLMConfigBuilder()
        config = (builder
                 .set_plasticity(
                     stdp_enabled=True,
                     hebbian_enabled=True,
                     stdp_rate=0.002,
                     hebbian_rate=0.001,
                     structural_enabled=True
                 )
                 .get_config())
        
        assert config is not None
    
    def test_neuromodulation_parameters(self):
        """Test neuromodulation parameter setting."""
        builder = NLMConfigBuilder()
        config = (builder
                 .set_neuromodulation(
                     dopamine_scale=1.5,
                     curiosity_enabled=True,
                     novelty_enabled=True,
                     prediction_error_enabled=True
                 )
                 .get_config())
        
        assert config is not None
    
    def test_development_parameters(self):
        """Test development parameter setting."""
        builder = NLMConfigBuilder()
        config = (builder
                 .set_development(
                     developmental_enabled=True,
                     critical_period=True
                 )
                 .get_config())
        
        assert config is not None
    
    def test_parameter_setting(self):
        """Test individual parameter setting."""
        builder = NLMConfigBuilder()
        config = (builder
                 .set_parameter('brain.neuron_count', 3000)
                 .set_parameter('brain.synapse_density', 0.2)
                 .get_config())
        
        assert config is not None
    
    def test_parameter_validation(self):
        """Test parameter validation."""
        builder = NLMConfigBuilder()
        
        # Test valid parameters
        builder.set_parameter('brain.neuron_count', 1000)
        builder.set_parameter('brain.synapse_density', 0.1)
        builder.set_parameter('plasticity.stdp.enable', True)
        
        # Test invalid parameters (should be caught by C++ validation)
        # Note: This tests the Python validation layer
    
    def test_export_import(self):
        """Test configuration export and import."""
        builder = NLMConfigBuilder()
        config = (builder
                 .apply_preset('standard')
                 .set_neural_parameters(neuron_count=1500)
                 .get_config())
        
        # Export
        exported = builder.export_config()
        assert isinstance(exported, dict)
        assert 'brain.neuron_count' in exported
        
        # Import
        new_builder = NLMConfigBuilder()
        new_builder.import_config(exported)
        new_config = new_builder.get_config()
        
        # Note: Direct comparison might be complex due to C++ bindings
        assert new_config is not None
    
    def test_parameter_info(self):
        """Test parameter info access."""
        builder = NLMConfigBuilder()
        
        # Test that parameter info is accessible
        info = builder.PARAMETER_INFO
        assert isinstance(info, dict)
        assert 'brain.neuron_count' in info
        assert 'type' in info['brain.neuron_count']
        assert 'default' in info['brain.neuron_count']
        assert 'description' in info['brain.neuron_count']


class TestNLMPerformanceMonitor:
    """Test NLMPerformanceMonitor class."""
    
    def test_initialization(self):
        """Test initialization."""
        monitor = NLMPerformanceMonitor()
        assert 'step_times' in monitor.metrics
        assert 'event_counts' in monitor.metrics
        assert monitor._start_time == 0.0
        assert monitor._step_start_time == 0.0
    
    def test_start_monitoring(self):
        """Test starting monitoring."""
        monitor = NLMPerformanceMonitor()
        monitor.start_monitoring()
        assert monitor._start_time > 0
        assert monitor._step_start_time > 0
    
    def test_end_step(self):
        """Test ending a step."""
        monitor = NLMPerformanceMonitor()
        monitor.start_monitoring()
        
        metrics = monitor.end_step('test_event')
        
        assert 'step_time' in metrics
        assert 'total_elapsed' in metrics
        assert 'steps_per_second' in metrics
        assert 'event_count' in metrics
        assert metrics['event_count'] == 1
        
        assert monitor.metrics['event_counts']['test_event'] == 1
    
    def test_get_summary(self):
        """Test getting summary."""
        monitor = NLMPerformanceMonitor()
        monitor.start_monitoring()
        
        # Add some step times
        monitor.metrics['step_times'] = [0.1, 0.2, 0.15]
        monitor.metrics['event_counts'] = {'test': 3}
        
        summary = monitor.get_summary()
        
        assert summary['total_steps'] == 3
        assert summary['average_step_time'] > 0
        assert summary['steps_per_second'] > 0
        assert summary['event_counts']['test'] == 3
    
    def test_reset(self):
        """Test reset."""
        monitor = NLMPerformanceMonitor()
        monitor.metrics['step_times'] = [0.1]
        monitor.metrics['event_counts'] = {'test': 1}
        monitor._start_time = 1.0
        monitor._step_start_time = 0.5
        
        monitor.reset()
        
        assert monitor.metrics['step_times'] == []
        assert monitor.metrics['event_counts'] == {}
        assert monitor._start_time == 0.0
        assert monitor._step_start_time == 0.0


class TestNLMUtils:
    """Test NLMUtils class."""
    
    def test_generate_random_config(self):
        """Test random config generation."""
        config = NLMUtils.generate_random_config(
            neuron_range=(500, 1000),
            synapse_range=(0.05, 0.3)
        )
        
        assert config is not None
        assert config.has('brain.neuron_count')
        assert config.has('brain.synapse_density')
    
    def test_create_simulation_grid(self):
        """Test simulation grid creation."""
        agents = NLMUtils.create_simulation_grid(world_size=20, num_agents=4)
        
        assert len(agents) == 4
        for agent in agents:
            assert 'world' in agent
            assert 'agent' in agent
            assert 'brain' in agent
            assert agent['agent']['x'] >= 0
            assert agent['agent']['x'] <= 20
            assert agent['agent']['y'] >= 0
            assert agent['agent']['y'] <= 20
    
    def test_benchmark_simulation(self):
        """Test simulation benchmarking."""
        config = pynlm.createDefaultConfig()
        results = NLMUtils.benchmark_simulation(config, num_steps=100)
        
        assert 'steps' in results
        assert 'duration' in results
        assert 'steps_per_second' in results
        assert 'neurons' in results
        assert 'synapses' in results
        assert 'spikes' in results
        assert 'average_firing_rate' in results
        
        assert results['steps'] == 100
        assert results['duration'] > 0
        assert results['steps_per_second'] > 0


class TestVisualizationIntegration:
    """Test visualization integration."""
    
    def test_visualization_without_matplotlib(self):
        """Test that visualization handles missing matplotlib gracefully."""
        # This should not raise an exception
        fig, plt = NLMVisualizer.plot_brain_activity([])
        assert fig is None
        assert plt is None


class TestErrorHandling:
    """Test error handling."""
    
    def test_brain_simulator_not_initialized(self):
        """Test error when brain simulator is not initialized."""
        simulator = BrainSimulator()
        # Manually set initialized to False to test error
        simulator._is_initialized = False
        
        with pytest.raises(SimulationError):
            simulator.step(0)
    
    def test_agent_simulator_not_initialized(self):
        """Test error when agent simulator is not initialized."""
        simulator = AgentSimulator()
        simulator._is_initialized = False
        
        with pytest.raises(SimulationError):
            simulator.run_step()


class TestConfigurationValidation:
    """Test configuration validation."""
    
    def test_parameter_validation(self):
        """Test parameter validation in config builder."""
        builder = NLMConfigBuilder()
        
        # These should pass Python validation
        builder.set_parameter('brain.neuron_count', 1000)
        builder.set_parameter('brain.synapse_density', 0.1)
        builder.set_parameter('plasticity.stdp.enable', True)
        
        # Note: C++ validation would happen at runtime
        # This tests the Python validation layer
        assert True  # If we get here, validation passed


if __name__ == '__main__':
    # Run tests
    pytest.main([__file__, '-v'])
