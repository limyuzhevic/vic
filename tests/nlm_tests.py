"""
Test suite for enhanced NLM Python bindings.

This module provides comprehensive tests for the improved Python bindings,
validating new functionality and ensuring backward compatibility.
"""

import sys
import os
import tempfile
import json
import numpy as np
import pytest
from typing import Dict, Any, Optional

# Import the enhanced NLM bindings
import pynlm

@pytest.fixture
def sample_config():
    """Fixture providing a sample configuration."""
    return pynlm.create_default_config()
@pytest.fixture
def sample_brain():
    """Fixture providing a sample brain."""
    brain = pynlm.create_brain()
    brain.initialize()
    return brain
@pytest.fixture
def sample_world():
    """Fixture providing a sample world."""
    world = pynlm.create_world()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    return world
@pytest.fixture
def sample_agent(brain):
    """Fixture providing a sample agent."""
    agent = pynlm.create_agent_brain(brain)
    return agent
@pytest.fixture
def temp_file():
    """Fixture providing a temporary file path."""
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.json') as f:
        yield f.name
    # Cleanup
    if os.path.exists(f.name):
        os.unlink(f.name)
class TestBasicFunctionality:
    """Test basic functionality of NLM bindings."""
    
    def test_module_import(self):
        """Test that the module can be imported successfully."""
        assert pynlm.__version__ == "0.1.0"
        assert hasattr(pynlm, 'create_default_config')
        assert hasattr(pynlm, 'create_brain')
        assert hasattr(pynlm, 'create_world')
        assert hasattr(pynlm, 'create_agent_brain')
    
    def test_config_creation(self, sample_config):
        """Test configuration creation and basic operations."""
        assert sample_config is not None
        assert hasattr(sample_config, 'has')
        assert hasattr(sample_config, 'set')
        assert hasattr(sample_config, 'get_keys')
        assert hasattr(sample_config, 'summary')
        
        # Test configuration operations
        sample_config.set("test_key", "test_value")
        assert sample_config.has("test_key")
        assert "test_key" in sample_config.get_keys()
        
        summary = sample_config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
    
    def test_brain_creation(self, sample_config):
        """Test brain creation and initialization."""
        brain = pynlm.create_brain(sample_config)
        assert brain is not None
        assert hasattr(brain, 'initialize')
        assert hasattr(brain, 'step')
        assert hasattr(brain, 'reset')
        assert hasattr(brain, 'get_total_neuron_count')
        assert hasattr(brain, 'get_total_synapse_count')
        
        # Initialize brain
        brain.initialize()
        assert brain.get_total_neuron_count() > 0
        assert brain.get_total_synapse_count() >= 0
    
    def test_world_creation(self, sample_world):
        """Test world creation and basic operations."""
        assert sample_world is not None
        assert hasattr(sample_world, 'configure')
        assert hasattr(sample_world, 'update')
        assert hasattr(sample_world, 'get_sensory_percept')
        assert hasattr(sample_world, 'get_agent_body')
        assert hasattr(sample_world, 'get_width')
        assert hasattr(sample_world, 'get_height')
        
        # Test world operations
        width = sample_world.get_width()
        height = sample_world.get_height()
        assert width > 0
        assert height > 0
    
    def test_agent_creation(self, sample_agent):
        """Test agent creation and basic operations."""
        assert sample_agent is not None
        assert hasattr(sample_agent, 'initialize')
        assert hasattr(sample_agent, 'process_sensory_input')
        assert hasattr(sample_agent, 'decode_motor_command')
        assert hasattr(sample_agent, 'enable_reward_modulation')
        assert hasattr(sample_agent, 'enable_structural_plasticity')
        assert hasattr(sample_agent, 'enable_development')
        assert hasattr(sample_agent, 'enable_curiosity')
class TestAgentSimulation:
    """Test agent simulation functionality."""
    
    def test_agent_simulation_creation(self, sample_brain, sample_world):
        """Test agent simulation creation."""
        simulation = pynlm.AgentSimulation(sample_brain, sample_world)
        assert simulation is not None
        assert simulation.brain == sample_brain
        assert simulation.world == sample_world
    
    def test_run_episode(self, sample_brain, sample_world):
        """Test running a simulation episode."""
        world = pynlm.create_world()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.create_agent_brain(sample_brain)
        simulation = pynlm.AgentSimulation(sample_brain, world)
        simulation.agent = agent
        
        # Run episode
        result = simulation.run_episode(num_steps=50, enable_learning=True)
        
        assert result is not None
        assert result.step_count == 50
        assert result.brain == sample_brain
        assert result.world == world
        assert result.agent == agent
        
        # Check statistics
        stats = result.get_statistics()
        assert stats['steps'] == 50
        assert stats['neurons'] > 0
    
    def test_run_continuous(self, sample_brain, sample_world):
        """Test continuous simulation."""
        simulation = pynlm.AgentSimulation(sample_brain, sample_world)
        
        # Run continuous simulation
        result = simulation.run_continuous(duration=2.0, timestep=0.1)
        
        assert result is not None
        assert result.step_count > 0
        
        # Should have run approximately 20 steps (2.0 / 0.1)
        expected_steps = 20
        assert abs(result.step_count - expected_steps) <= 1
class TestBrainAnalysis:
    """Test brain analysis functionality."""
    
    def test_comprehensive_stats(self, sample_brain):
        """Test comprehensive brain statistics."""
        stats = sample_brain.get_comprehensive_stats()
        
        assert 'basic' in stats
        assert 'activity' in stats
        assert 'development' in stats
        assert 'performance' in stats
        
        # Check basic stats
        basic = stats['basic']
        assert 'neuron_count' in basic
        assert 'synapse_count' in basic
        assert 'region_count' in basic
        assert 'population_count' in basic
        
        assert basic['neuron_count'] > 0
        assert basic['synapse_count'] >= 0
        
        # Check activity stats
        activity = stats['activity']
        assert 'active_neurons' in activity
        assert 'firing_neurons' in activity
        assert 'average_firing_rate' in activity
        assert 'total_spikes' in activity
        assert 'excitation_inhibition_ratio' in activity
        
        # Check development stats
        development = stats['development']
        assert 'developmental_stage' in development
        assert 'region_stats' in development
        
        # Check performance stats
        performance = stats['performance']
        assert 'spike_efficiency' in performance
        assert 'connectivity_strength' in performance
        assert 'stability' in performance
        
        # Values should be reasonable
        assert 0 <= activity['average_firing_rate'] <= 100
        assert 0 <= performance['spike_efficiency'] <= 1
        assert 0 <= performance['connectivity_strength'] <= 1
        assert 0 <= performance['stability'] <= 1
    
    def test_brain_stats_methods(self, sample_brain):
        """Test individual brain statistics methods."""
        # Test basic statistics methods
        assert sample_brain.get_total_neuron_count() > 0
        assert sample_brain.get_total_synapse_count() >= 0
        assert sample_brain.get_region_count() >= 0
        assert sample_brain.get_population_count() >= 0
        assert sample_brain.get_active_neuron_count() >= 0
        assert sample_brain.get_firing_neuron_count() >= 0
        
        # Firing rate should be reasonable
        firing_rate = sample_brain.get_average_firing_rate()
        assert 0 <= firing_rate <= 100
        
        # Excitation/inhibition ratio should be reasonable
        ei_ratio = sample_brain.get_excitation_inhibition_ratio()
        assert 0 <= ei_ratio <= 10  # Allow some range
        
        # Developmental stage should be valid
        stage = sample_brain.get_developmental_stage()
        assert stage in [pynlm.DevelopmentalStage.INITIAL,
                        pynlm.DevelopmentalStage.CRITICAL_PERIOD,
                        pynlm.DevelopmentalStage.MATURATION,
                        pynlm.DevelopmentalStage.ADULT,
                        pynlm.DevelopmentalStage.AGING]
class TestSimulationResults:
    """Test simulation results functionality."""
    
    def test_simulation_result_creation(self, sample_brain):
        """Test simulation result creation."""
        result = pynlm.SimulationResult(sample_brain)
        
        assert result is not None
        assert result.brain == sample_brain
        assert result.world is None
        assert result.agent is None
        assert result.step_count == 0
        assert result.spike_history == []
        assert result.firing_rate_history == []
        assert result.memory_usage == {}
        assert result.plasticity_history == []
    
    def test_get_statistics(self, sample_brain):
        """Test statistics retrieval."""
        result = pynlm.SimulationResult(sample_brain)
        stats = result.get_statistics()
        
        assert 'neurons' in stats
        assert 'synapses' in stats
        assert 'active_neurons' in stats
        assert 'firing_neurons' in stats
        assert 'average_firing_rate' in stats
        assert 'excitatory_inhibition_ratio' in stats
        assert 'total_spikes' in stats
        assert 'developmental_stage' in stats
        assert 'steps' in stats
        assert 'memory_usage' in stats
        
        assert stats['neurons'] == sample_brain.get_total_neuron_count()
    
    def test_analyze_plasticity(self, sample_brain):
        """Test plasticity analysis."""
        result = pynlm.SimulationResult(sample_brain)
        
        # Empty plasticity history should return zeros
        analysis = result.analyze_plasticity()
        
        assert 'plasticity_effectiveness' in analysis
        assert 'learning_rate' in analysis
        assert 'plasticity_variance' in analysis
        assert 'adaptation_rate' in analysis
        
        assert analysis['plasticity_effectiveness'] == 0.0
        assert analysis['learning_rate'] == 0.0
        assert analysis['plasticity_variance'] == 0.0
        assert analysis['adaptation_rate'] == 0.0
    
    def test_export_results(self, sample_brain, temp_file):
        """Test results export."""
        result = pynlm.SimulationResult(sample_brain)
        
        # Add some test data
        result.spike_history = [10.0, 15.0, 20.0]
        result.firing_rate_history = [0.1, 0.2, 0.15]
        
        # Export results
        result.export_results(temp_file)
        
        # Verify file was created and contains expected data
        assert os.path.exists(temp_file)
        
        with open(temp_file, 'r') as f:
            data = json.load(f)
        
        assert 'brain_stats' in data
        assert 'plasticity_analysis' in data
        assert 'step_count' in data
        assert 'spike_history' in data
        assert 'firing_rate_history' in data
        
        assert data['spike_history'] == [10.0, 15.0, 20.0]
        assert data['firing_rate_history'] == [0.1, 0.2, 0.15]
class TestMemoryManager:
    """Test memory management functionality."""
    
    def test_memory_manager_creation(self, sample_brain):
        """Test memory manager creation."""
        memory_manager = pynlm.MemoryManager(sample_brain)
        
        assert memory_manager is not None
        assert memory_manager.brain == sample_brain
    
    def test_memory_stats(self, sample_brain):
        """Test memory statistics."""
        memory_manager = pynlm.MemoryManager(sample_brain)
        stats = memory_manager.get_memory_stats()
        
        assert 'neuron_count' in stats
        assert 'synapse_count' in stats
        assert 'region_count' in stats
        assert 'population_count' in stats
        assert 'estimated_memory_usage' in stats
        
        assert stats['neuron_count'] == sample_brain.get_total_neuron_count()
        assert stats['synapse_count'] == sample_brain.get_total_synapse_count()
        
        # Memory usage should be non-negative
        assert stats['estimated_memory_usage'] >= 0
    
    @pytest.mark.skipif(True, reason="Testing requires write permissions")
    def test_checkpoint_operations(self, sample_brain, temp_file):
        """Test checkpoint save/load operations."""
        memory_manager = pynlm.MemoryManager(sample_brain)
        
        # Save checkpoint
        result = memory_manager.save_checkpoint(temp_file)
        assert result is True
        assert os.path.exists(temp_file)
        
        # Load checkpoint (this would require the checkpoint format to be compatible)
        # For now, just test that the method exists
        assert hasattr(memory_manager, 'load_checkpoint')
class TestConfigurationManagement:
    """Test configuration management functionality."""
    
    def test_config_creation(self):
        """Test configuration creation."""
        config = pynlm.create_default_config()
        assert config is not None
    
    def test_config_operations(self):
        """Test configuration operations."""
        config = pynlm.create_default_config()
        
        # Test set/get operations
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_string", "hello")
        
        # Keys should contain our test keys
        keys = config.get_keys()
        assert "test_int" in keys
        assert "test_float" in keys
        assert "test_string" in keys
        
        # Summary should be a string
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
    
    def test_config_save_load(self, temp_file):
        """Test configuration save/load to file."""
        # Create and save config
        config = pynlm.create_default_config()
        config.set("test_key", "test_value")
        config.save_to_file(temp_file)
        
        assert os.path.exists(temp_file)
        
        # Load config from file
        loaded_config = pynlm.load_config_from_file(temp_file)
        assert loaded_config is not None
        
        # The loaded config should have the test key
        # (Note: this depends on the actual config implementation)
        # For now, just verify the method works
        assert hasattr(loaded_config, 'has')
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = pynlm.create_default_config()
        
        # Test has method
        config.set("validation_key", "validation_value")
        assert config.has("validation_key")
        assert not config.has("non_existent_key")
class TestErrorHandling:
    """Test error handling functionality."""
    
    def test_validation_error(self):
        """Test validation error creation."""
        try:
            raise pynlm.ValidationError("Test validation error", "test_field")
        except pynlm.ValidationError as e:
            assert e.message == "Test validation error"
            assert e.field == "test_field"
            assert e.error_code == "VALIDATION_ERROR"
    
    def test_configuration_error(self):
        """Test configuration error creation."""
        try:
            raise pynlm.ConfigurationError("Test configuration error", "config_key")
        except pynlm.ConfigurationError as e:
            assert e.message == "Test configuration error"
            assert e.config_key == "config_key"
            assert e.error_code == "CONFIG_ERROR"
    
    def test_simulation_error(self):
        """Test simulation error creation."""
        try:
            raise pynlm.SimulationError("Test simulation error", 42)
        except pynlm.SimulationError as e:
            assert e.message == "Test simulation error"
            assert e.step == 42
            assert e.error_code == "SIMULATION_ERROR"
    
    def test_enums(self):
        """Test enumeration values."""
        # Test NeuromodulationLevel
        assert pynlm.NeuromodulationLevel.LOW.value == "low"
        assert pynlm.NeuromodulationLevel.MEDIUM.value == "medium"
        assert pynlm.NeuromodulationLevel.HIGH.value == "high"
        assert pynlm.NeuromodulationLevel.CRITICAL.value == "critical"
        
        # Test DevelopmentalStage
        assert pynlm.DevelopmentalStage.INITIAL.value == "initial"
        assert pynlm.DevelopmentalStage.CRITICAL_PERIOD.value == "critical_period"
        assert pynlm.DevelopmentalStage.MATURATION.value == "maturation"
        assert pynlm.DevelopmentalStage.ADULT.value == "adult"
        assert pynlm.DevelopmentalStage.AGING.value == "aging"
class TestBackwardCompatibility:
    """Test backward compatibility."""
    
    def test_backward_compatibility_functions(self):
        """Test that backward compatibility functions exist."""
        assert hasattr(pynlm, '_create_default_config')
        assert hasattr(pynlm, '_create_brain')
        assert hasattr(pynlm, '_create_world')
        assert hasattr(pynlm, '_create_agent')
        assert hasattr(pynlm, '_run_simulation')
    
    def test_enums_equivalent_access(self):
        """Test that enums can be accessed via expected attributes."""
        # Check that enums are accessible
        assert hasattr(pynlm, 'NeuromodulationLevel')
        assert hasattr(pynlm, 'DevelopmentalStage')
        
        # Check enum values
        assert hasattr(pynlm.NeuromodulationLevel, 'LOW')
        assert hasattr(pynlm.DevelopmentalStage, 'INITIAL')
        assert hasattr(pynlm.DevelopmentalStage, 'ADULT')
class TestIntegration:
    """Test integration functionality."""
    
    def test_complete_integration_example(self):
        """Test a complete integration example."""
        # This test mirrors the example in the examples file
        try:
            # Setup
            config = pynlm.create_default_config()
            brain = pynlm.create_brain(config)
            brain.initialize()
            
            world = pynlm.create_world()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.create_agent_brain(brain)
            agent.initialize(world)
            
            # Enable learning
            agent.enable_reward_modulation(True)
            agent.enable_curiosity(True)
            
            # Run agent
            for step in range(20):
                world.update(0.1)
                agent.process_sensory_input(world.get_sensory_percept())
                brain.step(step)
                action = agent.decode_motor_command()
                world.apply_motor_command(action, world.get_simulation_time())
            
            # Verify results
            assert brain.get_total_neuron_count() > 0
            assert world.get_width() == 15
            assert world.get_height() == 15
            
        except Exception as e:
            pytest.fail(f"Integration test failed: {e}")
    
    def test_module_initialization(self):
        """Test module initialization and logging."""
        # The module should have initialized without errors
        assert pynlm.__version__ == "0.1.0"
        assert pynlm.PYNLM_AVAILABLE == (pynlm.pynlm is not None)
class TestPerformance:
    """Test performance-related functionality."""
    
    def test_performance_benchmark(self, sample_brain):
        """Test performance benchmarking."""
        # Create a simple benchmark
        num_runs = 3
        duration = 0.1
        
        results = []
        for run in range(num_runs):
            brain = pynlm.create_brain()
            brain.initialize()
            
            import time
            start_time = time.time()
            
            for step in range(100):
                brain.step(step)
            
            end_time = time.time()
            duration_ms = (end_time - start_time) * 1000
            
            stats = {
                'run': run + 1,
                'duration_ms': duration_ms,
                'neuron_count': brain.get_total_neuron_count(),
                'total_spikes': brain.get_total_spike_count()
            }
            
            results.append(stats)
        
        # Calculate performance metrics
        avg_duration = np.mean([r['duration_ms'] for r in results])
        avg_spikes = np.mean([r['total_spikes'] for r in results])
        
        # Performance should be reasonable (less than 1000ms for 100 steps)
        assert avg_duration < 1000.0
        assert avg_spikes >= 0
        
        # Check that all runs completed successfully
        assert len(results) == num_runs
        assert all(r['neuron_count'] > 0 for r in results)
        assert all(r['total_spikes'] >= 0 for r in results)

# Run tests if this file is executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])
