#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python API improvements
Tests convenience methods, error handling, and performance monitoring
"""

import pynlm
import pytest
import tempfile
import os
import json
import time
import statistics

class TestNMConvenienceMethods:
    """Test NLM convenience methods"""
    
    def setup_method(self):
        """Setup before each test"""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100)
        self.config.set("brain.synapse_density", 0.1)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        """Cleanup after each test"""
        if self.brain:
            self.brain.reset()
        self.brain = None
    
    def test_brain_stats_basic(self):
        """Test basic brain statistics functionality"""
        
        for i in range(10):
            self.brain.step(i)
        
        stats = self.brain.getBrainStats()
        
        # Verify all expected keys exist
        expected_keys = {
            "total_neurons", "total_synapses", "firing_neurons",
            "average_firing_rate", "spike_count", "excitatory_inhibition_ratio",
            "developmental_stage", "neurons_by_type", "regions"
        }
        
        assert set(stats.keys()) == expected_keys
        
        # Verify values match brain methods
        assert stats["total_neurons"] == self.brain.getTotalNeuronCount()
        assert stats["firing_neurons"] == self.brain.getFiringNeuronCount()
        assert stats["average_firing_rate"] == self.brain.getAverageFiringRate()
        assert stats["spike_count"] == self.brain.getTotalSpikeCount()
        assert stats["excitatory_inhibition_ratio"] == self.brain.getExcitationInhibitionRatio()
        assert stats["developmental_stage"] == self.brain.getDevelopmentalStage()
    
    def test_agent_brain_status(self):
        """Test agent brain status functionality"""
        
        agent = pynlm.createAgentBrain(self.brain)
        
        status = agent.getAgentBrainStatus()
        
        expected_keys = {
            "brain_reference", "sensory_input_size", "motor_output_size",
            "reward_modulation_enabled", "structural_plasticity_enabled",
            "development_enabled", "curiosity_enabled", "sensory_neuron_groups",
            "current_curiosity_level", "current_novelty_level",
            "current_prediction_error", "developmental_stage"
        }
        
        assert set(status.keys()) == expected_keys
        assert status["brain_reference"] is self.brain
        assert status["sensory_input_size"] == self.brain.getTotalNeuronCount()
        assert status["motor_output_size"] == 8  # Default motor commands
    
    def test_config_stats(self):
        """Test configuration statistics"""
        
        # Add various config values
        self.config.set("brain.neuron_count", 500)
        self.config.set("brain.synapse_density", 0.15)
        self.config.set("learning.rate", 0.001)
        self.config.set("test.bool_value", True)
        self.config.set("test.float_value", 3.14)
        self.config.set("test.string_value", "test")
        
        stats = self.config.getConfigStats()
        
        assert "total_keys" in stats
        assert "sources" in stats
        assert "default_values" in stats
        assert "runtime_values" in stats
        
        assert stats["total_keys"] == 6
        assert "brain.neuron_count" in stats["runtime_values"]
        assert "brain.synapse_density" in stats["runtime_values"]
    
    def test_config_validation(self):
        """Test configuration validation"""
        
        # Test valid configuration
        errors = self.config.validateConfig()
        assert isinstance(errors, list)
        
        # Test config with invalid values
        invalid_config = pynlm.createDefaultConfig()
        
        # This should be handled gracefully
        invalid_config.set("test.invalid_int", -1)
        
        errors = invalid_config.validateConfig()
        assert isinstance(errors, list)
    
    def test_config_summary(self):
        """Test configuration summary"""
        
        self.config.set("brain.neuron_count", 1000)
        self.config.set("brain.synapse_density", 0.1)
        
        summary = self.config.getConfigSummary()
        
        assert isinstance(summary, str)
        assert "brain.neuron_count" in summary
        assert "brain.synapse_density" in summary
        assert "1000" in summary
        assert "0.1" in summary
    
    def test_world_status(self):
        """Test world status functionality"""
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20.0, height=20.0, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        status = world.getWorldStatus()
        
        expected_keys = {
            "width", "height", "vision_width", "vision_height",
            "agent_position", "agent_orientation", "agent_energy",
            "agent_health", "simulation_time", "objects_count",
            "resources_count", "hazards_count", "walls_count",
            "vision_data", "collision_detected"
        }
        
        assert set(status.keys()) == expected_keys
        assert status["width"] == 20.0
        assert status["height"] == 20.0
        assert status["agent_position"]["x"] == 10.0
        assert status["agent_position"]["y"] == 10.0
    
    def test_performance_stats(self):
        """Test performance statistics"""
        
        for i in range(50):
            self.brain.step(i)
        
        perf_stats = self.brain.getPerformanceStats()
        
        expected_keys = {
            "last_step_time_ms", "average_step_time_ms", "total_steps",
            "neuron_operations_per_step", "memory_usage_mb"
        }
        
        assert set(perf_stats.keys()) == expected_keys
        assert perf_stats["total_steps"] == 50
        assert perf_stats["last_step_time_ms"] >= 0
        assert perf_stats["average_step_time_ms"] >= 0
    
    def test_agent_performance_stats(self):
        """Test agent performance statistics"""
        
        agent = pynlm.createAgentBrain(self.brain)
        
        for i in range(20):
            self.brain.step(i)
        
        perf_stats = agent.getAgentPerformanceStats()
        
        expected_keys = {
            "steps_processed", "sensory_inputs_processed",
            "motor_commands_generated", "average_decision_time_ms",
            "exploration_rate"
        }
        
        assert set(perf_stats.keys()) == expected_keys
        assert perf_stats["steps_processed"] == 20
    
    def test_serialization(self):
        """Test serialization functionality"""
        
        # Test brain serialization
        with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as f:
            brain_file = f.name
        
        try:
            # Save brain
            save_result = self.brain.save(brain_file)
            assert save_result is True
            
            # Create new brain and load
            new_brain = pynlm.createBrain(self.config)
            new_brain.initialize()
            
            load_result = new_brain.load(brain_file)
            assert load_result is True
            
        finally:
            if os.path.exists(brain_file):
                os.unlink(brain_file)
        
        # Test config serialization
        with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.json') as f:
            config_file = f.name
        
        try:
            # Save config
            save_result = self.config.saveToFile(config_file)
            assert save_result is True
            
            # Load config
            new_config = pynlm.createDefaultConfig()
            load_result = new_config.loadFromFile(config_file)
            assert load_result is True
            
            # Verify data
            assert new_config.has("brain.neuron_count")
            
        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)
class TestNMErrorHandling:
    """Test NLM error handling improvements"""
    
    def test_brain_error_handling(self):
        """Test brain error handling"""
        
        config = pynlm.createDefaultConfig()
        
        # Test brain creation and initialization
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test operations after reset
        brain.reset()
        brain.step(0)  # Should work after reset
        
        # Test injection operations
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
        brain.step(1)
    
    def test_agent_error_handling(self):
        """Test agent error handling"""
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Test with None world (should handle gracefully)
        try:
            agent.initialize(None)
        except Exception:
            pass  # Exception is acceptable
        
        # Test sensory processing without proper initialization
        percept = pynlm.SensoryPercept()
        agent.processSensoryInput(percept)
    
    def test_config_error_handling(self):
        """Test configuration error handling"""
        
        config = pynlm.createDefaultConfig()
        
        # Test setting various values
        config.set("test.int_value", 42)
        config.set("test.float_value", 3.14)
        config.set("test.bool_value", True)
        config.set("test.string_value", "test")
        
        # Test getting non-existent keys
        value = config.get("non.existent.key")
        assert value is None
        
        # Test validation
        errors = config.validateConfig()
        assert isinstance(errors, list)
    
    def test_world_error_handling(self):
        """Test world error handling"""
        
        world = pynlm.createSimpleWorld()
        
        # Test with invalid parameters (should handle gracefully)
        try:
            world.configure(0.0, 0.0, 0, 0)  # Invalid dimensions
        except Exception:
            pass  # Exception is acceptable
        
        # Test operations with default world
        world.reset()
        world.update(0.1)
class TestNMIntegration:
    """Test NLM integration scenarios"""
    
    def test_complete_agent_workflow(self):
        """Test complete agent workflow"""
        
        # Create full system
        config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10.0, height=10.0, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run complete workflow
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        # Verify results
        assert brain.getTotalSpikeCount() > 0
        assert agent.getCuriosityLevel() >= 0
        assert agent.getNoveltyLevel() >= 0
    
    def test_performance_monitoring(self):
        """Test performance monitoring integration"""
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create monitor
        monitor = PerformanceMonitor(brain)
        
        # Run simulation
        for i in range(20):
            monitor.record_step(i)
        
        # Get performance summary
        summary = monitor.get_performance_summary()
        
        assert summary is not None
        assert summary["total_steps"] == 20
        assert summary["avg_step_time_ms"] >= 0
        assert summary["throughput_steps_per_second"] >= 0
    
    def test_serialization_workflow(self):
        """Test serialization workflow"""
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some simulation
        for i in range(10):
            brain.step(i)
        
        # Test brain backup
        backup_file = "test_backup.bin"
        assert brain.save(backup_file)
        
        # Simulate more learning
        for i in range(10, 20):
            brain.step(i)
        
        # Restore from backup
        assert brain.load(backup_file)
        
        # Cleanup
        if os.path.exists(backup_file):
            os.unlink(backup_file)
class PerformanceMonitor:
    """Monitor NLM performance over time"""
    
    def __init__(self, brain, agent=None):
        self.brain = brain
        self.agent = agent
        self.step_times = []
        self.performance_stats = []
    
    def record_step(self, step_number):
        """Record performance for a single step"""
        start_time = time.perf_counter()
        
        # Perform step
        self.brain.step(step_number)
        
        elapsed = time.perf_counter() - start_time
        self.step_times.append(elapsed)
        
        # Record performance stats
        if self.brain:
            perf_stats = self.brain.getPerformanceStats()
            self.performance_stats.append(perf_stats)
        
        return elapsed
    
    def get_performance_summary(self):
        """Get performance summary"""
        if not self.step_times:
            return None
        
        return {
            "total_steps": len(self.step_times),
            "avg_step_time_ms": statistics.mean(self.step_times) * 1000,
            "min_step_time_ms": min(self.step_times) * 1000,
            "max_step_time_ms": max(self.step_times) * 1000,
            "stddev_step_time_ms": statistics.stdev(self.step_times) * 1000 if len(self.step_times) > 1 else 0,
            "throughput_steps_per_second": len(self.step_times) / statistics.mean(self.step_times) if self.step_times else 0
        }

# Test performance monitoring standalone
class TestPerformanceMonitor:
    """Test PerformanceMonitor class"""
    
    def test_performance_monitor_creation(self):
        """Test PerformanceMonitor creation"""
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        monitor = PerformanceMonitor(brain)
        
        assert monitor.brain is brain
        assert len(monitor.step_times) == 0
        assert len(monitor.performance_stats) == 0
    
    def test_performance_monitor_record_step(self):
        """Test PerformanceMonitor record_step"""
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        monitor = PerformanceMonitor(brain)
        
        # Record a step
        elapsed = monitor.record_step(0)
        
        assert elapsed >= 0
        assert len(monitor.step_times) == 1
        assert len(monitor.performance_stats) == 1
        assert monitor.step_times[0] == elapsed
    
    def test_performance_monitor_summary(self):
        """Test PerformanceMonitor get_performance_summary"""
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        monitor = PerformanceMonitor(brain)
        
        # No steps recorded yet
        summary = monitor.get_performance_summary()
        assert summary is None
        
        # Record some steps
        for i in range(5):
            monitor.record_step(i)
        
        summary = monitor.get_performance_summary()
        
        assert summary is not None
        assert summary["total_steps"] == 5
        assert summary["avg_step_time_ms"] >= 0
        assert summary["min_step_time_ms"] >= 0
        assert summary["max_step_time_ms"] >= 0
        assert summary["throughput_steps_per_second"] >= 0

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
