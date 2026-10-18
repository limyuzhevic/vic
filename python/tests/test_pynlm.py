"""
Python tests for NLM bindings and utilities.
Comprehensive test suite for the pynlm Python module.
"""

import pytest
import numpy as np
import tempfile
import os
import json
import time
from typing import Dict, Any

import pynlm


class TestBasicFunctionality:
    """Test basic NLM Python functionality."""
    
    def test_version(self):
        """Test that version is properly set."""
        assert hasattr(pynlm, '__version__')
        assert isinstance(pynlm.__version__, str)
        assert pynlm.__version__ == "0.1.0"
    
    def test_module_constants(self):
        """Test that module constants are defined."""
        assert hasattr(pynlm, 'INVALID_NEURON_ID')
        assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
        assert hasattr(pynlm, 'INVALID_REGION_ID')
        assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    def test_basic_creation(self):
        """Test basic brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
    
    def test_brain_step(self):
        """Test brain step functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        for i in range(10):
            brain.step(i)
        
        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes


class TestConfig:
    """Test configuration functionality."""
    
    def test_config_creation(self):
        """Test configuration creation."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Test basic config operations
        config.set("test.int", 42)
        config.set("test.float", 3.14)
        config.set("test.bool", True)
        config.set("test.string", "hello")
        
        # Test getting values
        assert config.get("test.int") == 42
        assert abs(config.get("test.float") - 3.14) < 0.001
        assert config.get("test.bool") is True
        assert config.get("test.string") == "hello"
        
        # Test existence
        assert config.has("test.int")
        assert config.has("test.float")
        
        # Test keys
        keys = config.getKeys()
        assert "test.int" in keys
        assert "test.float" in keys
        assert "test.bool" in keys
        assert "test.string" in keys
    
    def test_config_file_io(self):
        """Test configuration file I/O."""
        config = pynlm.createDefaultConfig()
        
        # Set some test values
        config.set("brain.neuron_count", 2000)
        config.set("learning.rate", 0.001)
        config.set("testing.debug", True)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            temp_file = f.name
        
        try:
            # Load from file
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(temp_file)
            
            # Verify values
            assert config2.get("brain.neuron_count") == 2000
            assert abs(config2.get("learning.rate") - 0.001) < 0.0001
            assert config2.get("testing.debug") is True
            
        finally:
            os.unlink(temp_file)


class TestAgentBrain:
    """Test agent brain functionality."""
    
    def test_agent_creation(self):
        """Test agent brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Test agent methods
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_agent_enable_subsystems(self):
        """Test enabling and disabling agent subsystems."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Verify they are enabled
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
        
        # Disable some
        agent.enableRewardModulation(False)
        agent.enableCuriosity(False)
        
        assert not agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()  # Still enabled
        assert agent.isDevelopmentEnabled()  # Still enabled
        assert not agent.isCuriosityEnabled()


class TestSimpleWorld:
    """Test simple world functionality."""
    
    def test_world_creation(self):
        """Test world creation."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Test configuration
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        
        # Test reset
        world.reset()
        
        # Test update
        world.update(0.1)
        assert world.getSimulationTime() > 0
    
    def test_world_objects(self):
        """Test world object manipulation."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create and add objects
        resource_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
        hazard_obj = pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Hazard, value=0.5)
        
        world.addObject(resource_obj)
        world.addObject(hazard_obj)
        
        # Test removal
        world.removeObject(5.0, 5.0)
        assert not world.isValidPosition(5.0, 5.0)
        
        # Add it back
        world.addObject(resource_obj)
        assert world.isValidPosition(5.0, 5.0)


class TestBrainConfigHelper:
    """Test BrainConfig helper class."""
    
    def test_create_default(self):
        """Test BrainConfig.create_default()."""
        config = pynlm.BrainConfig.create_default()
        assert isinstance(config, pynlm.Config)
    
    def test_with_parameters(self):
        """Test BrainConfig.with_parameters()."""
        config = pynlm.BrainConfig.with_parameters(
            neuron_count=1500,
            synapse_density=0.05,
            learning_rate=0.01,
            custom.param="test"
        )
        
        assert config.get("brain.neuron_count") == 1500
        assert abs(config.get("brain.synapse_density") - 0.05) < 0.0001
        assert abs(config.get("learning.rate") - 0.01) < 0.0001
        assert config.get("custom.param") == "test"
    
    def test_from_file(self):
        """Test BrainConfig.from_file()."""
        # Create and save a config
        config1 = pynlm.BrainConfig.with_parameters(test_value=123)
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            pynlm.BrainConfig.save_to_file(config1, f.name)
            temp_file = f.name
        
        try:
            # Load it back
            config2 = pynlm.BrainConfig.from_file(temp_file)
            assert config2.get("test_value") == 123
        finally:
            os.unlink(temp_file)


class TestSimulationMetrics:
    """Test SimulationMetrics helper class."""
    
    def test_collect_basic(self):
        """Test SimulationMetrics.collect()."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        metrics = pynlm.SimulationMetrics.collect(brain)
        
        assert "neuron_count" in metrics
        assert "synapse_count" in metrics
        assert "firing_neuron_count" in metrics
        assert "total_spike_count" in metrics
        assert "average_firing_rate" in metrics
        assert "excitation_inhibition_ratio" in metrics
        assert "developmental_stage" in metrics
        
        assert metrics["neuron_count"] == brain.getTotalNeuronCount()
        assert metrics["firing_neuron_count"] == brain.getFiringNeuronCount()
    
    def test_collect_with_world(self):
        """Test SimulationMetrics.collect() with world."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        metrics = pynlm.SimulationMetrics.collect(brain, world)
        
        assert "agent_position" in metrics
        assert "agent_orientation" in metrics
        assert "agent_energy" in metrics
        assert "agent_health" in metrics
        assert "agent_age" in metrics
        assert "simulation_time" in metrics
        
        assert isinstance(metrics["agent_position"], tuple)
        assert len(metrics["agent_position"]) == 2
    
    def test_format_metrics(self):
        """Test SimulationMetrics.format()."""
        metrics = {
            "step": 100,
            "neuron_count": 1000,
            "firing_neuron_count": 150,
            "average_firing_rate": 2.5,
            "agent_position": (5.0, 5.0),
            "test_value": 123,
        }
        
        formatted = pynlm.SimulationMetrics.format(metrics, precision=1)
        
        assert "Simulation Metrics:" in formatted
        assert "Step: 100" in formatted
        assert "Neuron Count: 1000" in formatted
        assert "Firing Neuron Count: 150" in formatted
        assert "Average Firing Rate: 2.5" in formatted
        assert "Agent Position: (5.0, 5.0)" in formatted
        assert "Test Value: 123" in formatted


class TestPerformanceMonitor:
    """Test PerformanceMonitor class."""
    
    def test_monitor_creation(self):
        """Test PerformanceMonitor creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        monitor = pynlm.PerformanceMonitor(brain)
        assert monitor is not None
        
        stats = monitor.get_stats()
        assert stats["status"] == "no_data"
    
    def test_monitor_start_stop(self):
        """Test PerformanceMonitor start/stop."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        monitor = pynlm.PerformanceMonitor(brain)
        
        # Start monitoring
        monitor.start()
        
        # Record some steps
        for i in range(5):
            brain.step(i)
            monitor.record_step(0.001 * (i + 1))
        
        # Stop monitoring
        results = monitor.stop()
        
        assert "total_time" in results
        assert "avg_step_time" in results
        assert "steps_per_second" in results
        
        assert results["total_time"] >= 0
        assert results["steps_per_second"] > 0
    
    def test_monitor_stats(self):
        """Test PerformanceMonitor.get_stats()."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        monitor = pynlm.PerformanceMonitor(brain)
        
        # Add some step times
        monitor.record_step(0.01)
        monitor.record_step(0.02)
        monitor.record_step(0.015)
        
        stats = monitor.get_stats()
        
        assert stats["step_count"] == 3
        assert abs(stats["avg_step_time"] - 0.015) < 0.0001
        assert stats["min_step_time"] == 0.01
        assert stats["max_step_time"] == 0.02
        assert abs(stats["std_step_time"] - 0.005) < 0.0001


class TestRegistrationSystem:
    """Test brain registration system."""
    
    def test_brain_registration(self):
        """Test brain registration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Register brain
        pynlm.register_brain("test_brain", brain)
        
        # Get brain
        retrieved = pynlm.get_brain("test_brain")
        assert retrieved is brain
        
        # List brains
        brains = pynlm.list_brains()
        assert "test_brain" in brains
        
        # Unregister brain
        pynlm.unregister_brain("test_brain")
        
        # Verify unregistered
        assert "test_brain" not in pynlm.list_brains()
        
        # Verify getting unregistered brain raises exception
        with pytest.raises(pynlm.NLMException):
            pynlm.get_brain("test_brain")
    
    def test_multiple_brains(self):
        """Test registering multiple brains."""
        config = pynlm.createDefaultConfig()
        
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        pynlm.register_brain("brain1", brain1)
        pynlm.register_brain("brain2", brain2)
        
        assert len(pynlm.list_brains()) == 2
        assert pynlm.get_brain("brain1") is brain1
        assert pynlm.get_brain("brain2") is brain2
        
        pynlm.unregister_brain("brain1")
        assert "brain1" not in pynlm.list_brains()
        assert "brain2" in pynlm.list_brains()


class TestExperimentHelper:
    """Test experiment helper function."""
    
    def test_run_experiment(self):
        """Test run_experiment function."""
        config = pynlm.createDefaultConfig()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Run experiment
        results = pynlm.run_experiment(
            config=config,
            world=world,
            num_steps=50,
            enable_learning=False,  # Disable to speed up test
            enable_development=False,
            enable_curiosity=False,
            log_interval=25
        )
        
        # Verify results structure
        assert "metrics" in results
        assert "final_brain" in results
        assert "final_agent" in results
        assert "final_world" in results
        
        assert len(results["metrics"]) == 2  # Two logs (steps 0 and 25, 50)
        
        # Verify metrics structure
        first_metric = results["metrics"][0]
        assert "step" in first_metric
        assert "neuron_count" in first_metric
        assert "firing_neuron_count" in first_metric


class TestBenchmarkHelpers:
    """Test benchmark helper functions."""
    
    def test_benchmark_brain_creation(self):
        """Test benchmark_brain_creation function."""
        results = pynlm.benchmark_brain_creation(
            num_brains=5,
            neuron_count=500
        )
        
        assert "creation_time" in results
        assert "brains_per_second" in results
        assert "avg_creation_time" in results
        
        assert results["creation_time"] >= 0
        assert results["brains_per_second"] > 0
        assert results["avg_creation_time"] > 0
        
        assert abs(results["avg_creation_time"] * 5 - results["creation_time"]) < 0.1
    
    def test_benchmark_simulation(self):
        """Test benchmark_simulation function."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        results = pynlm.benchmark_simulation(brain, world, num_steps=100)
        
        assert "simulation_time" in results
        assert "steps_per_second" in results
        assert "avg_step_time" in results
        
        assert results["simulation_time"] >= 0
        assert results["steps_per_second"] > 0
        assert results["avg_step_time"] > 0


class TestNLMException:
    """Test NLMException class."""
    
    def test_exception_creation(self):
        """Test NLMException creation."""
        exc = pynlm.NLMException("Test error", 42)
        
        assert "NLM Error 42" in str(exc)
        assert exc.error_code == 42
        assert exc.message == "Test error"
    
    def test_exception_default_code(self):
        """Test NLMException with default error code."""
        exc = pynlm.NLMException("Test error")
        
        assert exc.error_code == 1
        assert "NLM Error 1" in str(exc)


class TestRegistrationCleanup:
    """Test registration cleanup."""
    
    def test_cleanup_function(self):
        """Test that cleanup() function exists."""
        assert hasattr(pynlm, 'cleanup')
        assert callable(pynlm.cleanup)
    
    def test_cleanup_registers(self):
        """Test cleanup clears registered brains."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        pynlm.register_brain("test", brain)
        assert "test" in pynlm.list_brains()
        
        pynlm.cleanup()
        assert "test" not in pynlm.list_brains()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
