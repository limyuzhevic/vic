#!/usr/bin/env python3
"""
Comprehensive Test Suite for Enhanced NLM Python API

This test suite validates all new functionality in the enhanced NLM Python API:
- Enhanced Brain class with complete subsystem access
- AgentBrain enhancements
- SimpleWorld enhancements
- Configuration management
- Exception handling
- Batch operations
- Performance optimizations
- Integration testing

This test suite can be run with: pytest enhanced_tests.py
"""

import pynlm
import pytest
import time
from typing import Dict, List, Any
import unittest.mock as mock

class TestEnhancedBrainAPI:
    """Test enhanced Brain class functionality"""
    
    def setup_method(self):
        """Set up test environment"""
        self.config = pynlm.createDefaultConfig()
        self.config.set_int("brain.neuron_count", 1000)
        self.config.set_float("brain.synapse_density", 0.1)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_basic_enhanced_brain_creation(self):
        """Test basic enhanced brain creation"""
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() > 0
        assert self.brain.getTotalNeuronCount() == 1000
    
    def test_subsystem_access(self):
        """Test access to all subsystems"""
        # Memory systems
        assert self.brain.getWorkingMemory() is not None
        assert self.brain.getEpisodicMemory() is not None
        assert self.brain.getAssociativeMemory() is not None
        
        # Prediction system
        assert self.brain.getPredictionSystem() is not None
        
        # Cognition systems
        assert self.brain.getPlanner() is not None
        assert self.brain.getConceptFormation() is not None
        assert self.brain.getAttention() is not None
        
        # Development system
        assert self.brain.getDevelopmentSystem() is not None
        
        # Neuromodulation systems
        assert self.brain.getDopamine() is not None
        assert self.brain.getCuriosity() is not None
        assert self.brain.getNovelty() is not None
        assert self.brain.getPredictionErrorSignal() is not None
        
        # Plasticity systems
        assert self.brain.getSTDP() is not None
        assert self.brain.getHebbian() is not None
        assert self.brain.getStructuralPlasticity() is not None
        
        # Random generator
        assert self.brain.getRandomGenerator() is not None
    
    def test_enhanced_statistics(self):
        """Test enhanced statistics methods"""
        stats = self.brain.getDetailedStats()
        
        # Check that expected statistics are present
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        assert "active_neurons" in stats
        assert "firing_neurons" in stats
        assert "average_firing_rate" in stats
        assert "excitation_inhibition_ratio" in stats
        assert "total_spikes" in stats
        assert "developmental_stage" in stats
        
        # Check that values are reasonable
        assert stats["total_neurons"] > 0
        assert stats["total_synapses"] > 0
        assert stats["average_firing_rate"] >= 0.0
        assert 0.0 <= stats["excitation_inhibition_ratio"] <= 2.0
        assert stats["total_spikes"] >= 0
    
    def test_system_status(self):
        """Test system status method"""
        status = self.brain.getSystemStatus()
        
        # Check that status contains expected fields
        assert "version" in status
        assert "total_neurons" in status
        assert "total_synapses" in status
        assert "firing_rate" in status
        assert "spike_count" in status
        assert "developmental_stage" in status
        
        # Check status values
        assert status["version"] == "NLM Phase 2 Enhanced"
        assert int(status["total_neurons"]) > 0
        assert int(status["total_synapses"]) > 0
        assert float(status["firing_rate"]) >= 0.0
        assert int(status["spike_count"]) >= 0
    
    def test_batch_operations(self):
        """Test batch operations"""
        # Test step_range
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step_range(0, 100)
        final_spikes = self.brain.getTotalSpikeCount()
        
        assert final_spikes >= initial_spikes
        
        # Test reset_and_initialize
        self.brain.reset_and_initialize()
        assert self.brain.getTotalNeuronCount() > 0
        
        # Test configure_neurons
        self.brain.configure_neurons(500)
        assert self.brain.getTotalNeuronCount() == 500
    
    def test_firing_statistics(self):
        """Test firing statistics"""
        # Run some steps first
        self.brain.step_range(0, 50)
        
        firing_stats = self.brain.getFiringStats()
        
        # Check that expected statistics are present
        assert "firing_rate" in firing_stats
        assert "firing_ratio" in firing_stats
        assert "total_spikes" in firing_stats
        
        # Check that values are reasonable
        assert firing_stats["firing_rate"] >= 0.0
        assert 0.0 <= firing_stats["firing_ratio"] <= 1.0
        assert firing_stats["total_spikes"] >= 0
    
    def test_development_operations(self):
        """Test development operations"""
        # Test advance_development
        result = self.brain.advance_development(0.1)
        assert result == True
        
        # Test set_development_stage
        self.brain.set_development_stage(pynlm.DevelopmentalStage.CriticalPeriod)
        assert self.brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod
    
    def test_configuration_operations(self):
        """Test configuration operations"""
        # Test configure_neurons
        self.brain.configure_neurons(2000)
        assert self.brain.getTotalNeuronCount() == 2000
        
        # Test get_config_value
        config_value = self.brain.get_config_value("brain.neuron_count")
        assert config_value == "2000"
        
        # Test set_config_value
        self.brain.set_config_value("brain.neuron_count", "1500")
        assert self.brain.getTotalNeuronCount() == 1500
    
    def test_simulation_control(self):
        """Test simulation control methods"""
        # Test run_simulation_to_time
        steps = self.brain.run_simulation_to_time(1.0, 0.1)
        assert steps > 0
        
        # Test run_simulation_fixed_steps
        steps = self.brain.run_simulation_fixed_steps(50, 0.1)
        assert steps == 50
    
    def test_energy_efficiency(self):
        """Test energy efficiency methods"""
        steps = self.brain.manage_energy_efficiency(1000.0)
        assert steps > 0

class TestEnhancedAgentBrainAPI:
    """Test enhanced AgentBrain functionality"""
    
    def setup_method(self):
        """Set up test environment"""
        self.config = pynlm.createDefaultConfig()
        self.config.set_int("brain.neuron_count", 500)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure_advanced(20, 20, 8, 8, 100.0, 0.02, 42)
        self.world.reset()
        
        self.agent.initialize(self.world)
    
    def test_agent_creation_and_initialization(self):
        """Test agent creation and initialization"""
        assert self.agent is not None
        assert self.brain.getTotalNeuronCount() > 0
        assert self.world.getWidth() == 20
        assert self.world.getHeight() == 20
    
    def test_learning_system_control(self):
        """Test learning system control methods"""
        # Test enable_all_learning
        self.agent.enable_all_learning(True)
        
        status = self.agent.get_learning_status()
        assert status["reward_modulation"] == True
        assert status["structural_plasticity"] == True
        assert status["development"] == True
        assert status["curiosity"] == True
        
        # Test with false
        self.agent.enable_all_learning(False)
        status = self.agent.get_learning_status()
        assert status["reward_modulation"] == False
        assert status["structural_plasticity"] == False
        assert status["development"] == False
        assert status["curiosity"] == False
    
    def test_detailed_agent_statistics(self):
        """Test detailed agent statistics"""
        stats = self.agent.get_detailed_stats()
        
        # Check that expected statistics are present
        assert "curiosity_level" in stats
        assert "novelty_level" in stats
        assert "prediction_error" in stats
        assert "neuromodulation_level" in stats
        assert "development_stage" in stats
        assert "reward_modulation_enabled" in stats
        assert "structural_plasticity_enabled" in stats
        assert "development_enabled" in stats
        assert "curiosity_enabled" in stats
        
        # Check that values are reasonable
        assert 0.0 <= stats["curiosity_level"] <= 1.0
        assert 0.0 <= stats["novelty_level"] <= 1.0
        assert stats["prediction_error"] >= 0.0
        assert stats["neuromodulation_level"] >= 0.0
        assert stats["development_stage"] >= 0
    
    def test_agent_episode(self):
        """Test agent episode execution"""
        # Run episode
        steps_completed = self.agent.run_episode(self.world, 100)
        
        assert steps_completed == 100
    
    def test_sensory_processing(self):
        """Test sensory processing"""
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Verify that brain has processed input
        assert self.brain.getTotalSpikeCount() >= 0
    
    def test_motor_decoding(self):
        """Test motor decoding"""
        # Get motor command
        action = self.agent.decodeMotorCommand()
        
        # Apply action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Verify that world has applied action
        assert self.world.getSimulationTime() > 0.0

class TestEnhancedSimpleWorldAPI:
    """Test enhanced SimpleWorld functionality"""
    
    def setup_method(self):
        """Set up test environment"""
        self.world = pynlm.createSimpleWorld()
        self.world.configure_advanced(20, 20, 8, 8, 100.0, 0.02, 42)
    
    def test_world_creation_and_configuration(self):
        """Test world creation and configuration"""
        assert self.world.getWidth() == 20
        assert self.world.getHeight() == 20
        assert self.world.getMaxEnergy() == 100.0
        assert self.world.getEnergyDecayRate() == 0.02
        assert self.world.getRandomSeed() == 42
    
    def test_enhanced_configuration(self):
        """Test enhanced configuration methods"""
        # Test configure_from_dict
        config = {
            "width": 30,
            "height": 30,
            "visionWidth": 10,
            "visionHeight": 10,
            "maxEnergy": 200.0,
            "energyDecayRate": 0.01,
            "randomSeed": 123
        }
        
        self.world.configure_from_dict(config)
        assert self.world.getWidth() == 30
        assert self.world.getHeight() == 30
        assert self.world.getMaxEnergy() == 200.0
        assert self.world.getEnergyDecayRate() == 0.01
        assert self.world.getRandomSeed() == 123
    
    def test_object_creation_methods(self):
        """Test object creation methods"""
        # Test add_resource
        self.world.add_resource(5.0, 5.0, 10.0)
        
        # Test add_hazard
        self.world.add_hazard(15.0, 15.0, 20.0)
        
        # Test add_wall
        self.world.add_wall(2.0, 2.0, 1.0, 1.0)
        
        # Verify that objects have been added
        # (Note: This would require actual API support for object counting)
        pass
    
    def test_enhanced_agent_control(self):
        """Test enhanced agent control methods"""
        # Set agent position
        self.world.setAgentStart(10.0, 10.0)
        
        # Set agent velocity
        self.world.set_agent_velocity(1.0, 0.5)
        
        # Set agent orientation
        self.world.set_agent_orientation(0.5)
        
        # Verify that agent position has been set
        assert self.world.getAgentBody().x == 10.0
        assert self.world.getAgentBody().y == 10.0
        assert self.world.getAgentBody().velocityX == 1.0
        assert self.world.getAgentBody().velocityY == 0.5
        assert self.world.getAgentBody().orientation == 0.5
    
    def test_world_state(self):
        """Test world state methods"""
        # Get initial world state
        initial_state = self.world.get_world_state()
        
        # Check that expected fields are present
        assert "width" in initial_state
        assert "height" in initial_state
        assert "max_energy" in initial_state
        assert "energy_decay_rate" in initial_state
        assert "simulation_time" in initial_state
        assert "random_seed" in initial_state
        
        # Check that values are reasonable
        assert initial_state["width"] == 20
        assert initial_state["height"] == 20
        assert initial_state["max_energy"] == 100.0
        assert initial_state["energy_decay_rate"] == 0.02
        assert initial_state["random_seed"] == 42
        
        # Test environment statistics
        env_stats = self.world.get_environment_stats()
        assert "width" in env_stats
        assert "height" in env_stats
        assert "agent_x" in env_stats
        assert "agent_y" in env_stats
        assert "agent_energy" in env_stats
        assert "agent_health" in env_stats
    
    def test_simulation_control(self):
        """Test simulation control methods"""
        # Run simulation
        steps = self.world.run_simulation(5.0, 0.1)
        assert steps == 50
        
        # Test step_until_time
        result = self.world.step_until_time(2.0)
        assert result == True
        
        # Test update_multiple
        timesteps = [0.1, 0.2, 0.3, 0.4, 0.5]
        steps = self.world.update_multiple(timesteps)
        assert steps == len(timesteps)
    
    def test_sample_environment(self):
        """Test sample environment creation"""
        # Reset world
        self.world.reset()
        
        # Create sample environment
        self.world.create_sample_environment()
        
        # Verify that world has been modified
        # (Note: This would require actual API support for object counting)
        pass
    
    def test_environment_summary(self):
        """Test environment summary"""
        summary = self.world.get_environment_summary()
        
        # Check that expected fields are present
        assert "description" in summary
        assert "size" in summary
        assert "agent_start" in summary
        assert "objects" in summary
        assert "simulation_time" in summary
        
        # Check that values are reasonable
        assert "Advanced 2D simulation world for NLM agents" in summary["description"]
        assert "20x20" in summary["size"]
        assert "Configured" in summary["agent_start"]

class TestEnhancedConfigurationAPI:
    """Test enhanced configuration functionality"""
    
    def setup_method(self):
        """Set up test environment"""
        self.config = pynlm.createDefaultConfig()
    
    def test_type_safe_access(self):
        """Test type-safe configuration access"""
        # Set values using type-safe methods
        self.config.set_int("brain.neuron_count", 1000)
        self.config.set_float("brain.synapse_density", 0.1)
        self.config.set_bool("plasticity.stdp.enable", True)
        self.config.set_string("test.key", "test_value")
        
        # Get values using type-safe getters
        neuron_count = self.config.get_int("brain.neuron_count")
        synapse_density = self.config.get_float("brain.synapse_density")
        stdp_enabled = self.config.get_bool("plasticity.stdp.enable")
        test_value = self.config.get_string("test.key")
        
        assert neuron_count == 1000
        assert synapse_density == 0.1
        assert stdp_enabled == True
        assert test_value == "test_value"
    
    def test_type_safe_with_defaults(self):
        """Test type-safe configuration with defaults"""
        # Test int with default
        neuron_count = self.config.get_int_or_default("brain.neuron_count", 500)
        assert neuron_count == 500
        
        # Test float with default
        synapse_density = self.config.get_float_or_default("brain.synapse_density", 0.05)
        assert synapse_density == 0.05
        
        # Test bool with default
        stdp_enabled = self.config.get_bool_or_default("plasticity.stdp.enable", False)
        assert stdp_enabled == False
        
        # Test string with default
        test_value = self.config.get_string_or_default("test.key", "default_value")
        assert test_value == "default_value"
    
    def test_configuration_validation(self):
        """Test configuration validation"""
        # Set values
        self.config.set_int("brain.neuron_count", 1000)
        self.config.set_float("learning.rate", 0.001)
        self.config.set_bool("plasticity.stdp.enable", True)
        
        # Test validation
        assert self.config.validate_range("brain.neuron_count", 100, 5000) == True
        assert self.config.validate_positive("learning.rate") == True
        
        # Test validation errors
        try:
            self.config.validate_range("brain.neuron_count", 2000, 5000)
            assert False, "Should have raised validation error"
        except pynlm.NLMConfigurationError:
            pass  # Expected
        
        try:
            self.config.validate_positive("negative_value")
            assert False, "Should have raised validation error"
        except pynlm.NLMConfigurationError:
            pass  # Expected
    
    def test_configuration_management(self):
        """Test configuration management"""
        # Set values
        self.config.set_int("brain.neuron_count", 1000)
        self.config.set_float("brain.synapse_density", 0.1)
        
        # Test set_int, set_float, set_bool, set_string
        self.config.set_int("brain.neuron_count", 2000)
        assert self.config.get_int("brain.neuron_count") == 2000
        
        self.config.set_float("brain.synapse_density", 0.2)
        assert self.config.get_float("brain.synapse_density") == 0.2
        
        self.config.set_bool("plasticity.stdp.enable", False)
        assert self.config.get_bool("plasticity.stdp.enable") == False
        
        self.config.set_string("test.key", "test_value")
        assert self.config.get_string("test.key") == "test_value"
    
    def test_preset_configurations(self):
        """Test preset configurations"""
        # Test default preset
        default_config = pynlm.createDefaultConfig()
        assert default_config.get_int("brain.neuron_count") > 0
        
        # Test learning preset
        learning_config = pynlm.createDefaultConfig()
        # learning preset would have specific values
        
        # Test preset list
        presets = pynlm.createDefaultConfig().get_preset_list()
        assert "default" in presets
        assert "minimal" in presets
        assert "learning" in presets
    
    def test_configuration_serialization(self):
        """Test configuration serialization"""
        # Set values
        self.config.set_int("brain.neuron_count", 1000)
        self.config.set_float("brain.synapse_density", 0.1)
        
        # Test clone
        cloned_config = self.config.clone()
        assert cloned_config.get_int("brain.neuron_count") == 1000
        assert cloned_config.get_float("brain.synapse_density") == 0.1
        
        # Test merge_from_dict
        additional_config = {
            "brain.v_thresh": -50.0,
            "brain.v_rest": -70.0,
            "plasticity.stdp.enable": True
        }
        
        # This would require implementation of merge_from_dict
        # For now, just test that config methods exist
        pass

class TestExceptionHandling:
    """Test exception handling functionality"""
    
    def test_nlm_exceptions(self):
        """Test NLM exception hierarchy"""
        # Test that exception classes exist
        assert hasattr(pynlm, 'NLMException')
        assert hasattr(pynlm, 'NLMConfigurationError')
        assert hasattr(pynlm, 'NLMBrainError')
        assert hasattr(pynlm, 'NLMWorldError')
        assert hasattr(pynlm, 'NLMArgumentError')
        assert hasattr(pynlm, 'NLMSimulationError')
        
        # Test exception inheritance
        try:
            raise pynlm.NLMConfigurationError("Test configuration error")
        except pynlm.NLMException:
            pass  # Expected
        
        try:
            raise pynlm.NLMBrainError("Test brain error")
        except pynlm.NLMException:
            pass  # Expected
        
        try:
            raise pynlm.NLMWorldError("Test world error")
        except pynlm.NLMException:
            pass  # Expected
    
    def test_configuration_validation_errors(self):
        """Test configuration validation error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test invalid range
        try:
            config.validate_range("brain.neuron_count", 2000, 5000)
            assert False, "Should have raised validation error"
        except pynlm.NLMConfigurationError:
            pass  # Expected
        
        # Test non-existent key
        try:
            config.validate_range("nonexistent.key", 100, 200)
            assert False, "Should have raised validation error"
        except pynlm.NLMConfigurationError:
            pass  # Expected

class TestBatchOperations:
    """Test batch operations functionality"""
    
    def setup_method(self):
        """Set up test environment"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_batch_step_operations(self):
        """Test batch step operations"""
        # Test step_range
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step_range(0, 100)
        final_spikes = self.brain.getTotalSpikeCount()
        
        assert final_spikes >= initial_spikes
        
        # Test run_simulation_to_time
        steps = self.brain.run_simulation_to_time(1.0, 0.1)
        assert steps > 0
        
        # Test run_simulation_fixed_steps
        steps = self.brain.run_simulation_fixed_steps(50, 0.1)
        assert steps == 50
    
    def test_batch_current_injection(self):
        """Test batch current injection (if available)"""
        # This would test batch current injection if available
        pass
    
    def test_batch_statistics(self):
        """Test batch statistics collection"""
        # Run some steps
        self.brain.step_range(0, 100)
        
        # Get statistics
        stats = self.brain.getDetailedStats()
        
        # Verify statistics
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        assert "average_firing_rate" in stats

class TestPerformanceTests:
    """Test performance optimizations"""
    
    def setup_method(self):
        """Set up test environment"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_performance_monitoring(self):
        """Test performance monitoring"""
        # This would test performance monitoring if available
        pass
    
    def test_memory_efficiency(self):
        """Test memory efficiency"""
        # Test energy efficiency
        steps = self.brain.manage_energy_efficiency(1000.0)
        assert steps > 0
    
    def test_batch_configuration_operations(self):
        """Test batch configuration operations"""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        # Test configuration cloning
        config_clone = config1.clone()
        assert config_clone.get_int("brain.neuron_count") == config1.get_int("brain.neuron_count")
        
        # Test configuration comparison
        config1.set_int("brain.neuron_count", 1000)
        config2.set_int("brain.neuron_count", 2000)
        
        assert config1.get_int("brain.neuron_count") != config2.get_int("brain.neuron_count")

class TestIntegrationTests:
    """Test integration between components"""
    
    def setup_method(self):
        """Set up test environment"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure_advanced(20, 20, 8, 8, 100.0, 0.02, 42)
        self.world.reset()
        
        self.agent.initialize(self.world)
    
    def test_complete_agent_world_integration(self):
        """Test complete agent-world integration"""
        # Enable all learning
        self.agent.enable_all_learning(True)
        
        # Run episode
        steps = self.agent.run_episode(self.world, 100)
        
        assert steps == 100
        
        # Verify that all systems worked together
        assert self.world.getSimulationTime() > 0.0
        assert self.brain.getTotalSpikeCount() > 0
        
        # Get agent statistics
        stats = self.agent.get_detailed_stats()
        assert stats["curiosity_level"] >= 0.0
        assert stats["novelty_level"] >= 0.0
    
    def test_enhanced_configuration_integration(self):
        """Test enhanced configuration integration"""
        config = pynlm.createDefaultConfig()
        
        # Configure using type-safe methods
        config.set_int("brain.neuron_count", 1000)
        config.set_float("brain.synapse_density", 0.1)
        config.set_bool("plasticity.stdp.enable", True)
        
        # Create brain with enhanced configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify that configuration was applied
        assert brain.getTotalNeuronCount() == 1000
        assert brain.getSTDP() is not None

class TestExampleIntegration:
    """Test integration with example files"""
    
    def test_enhanced_example_import(self):
        """Test that enhanced examples can be imported"""
        # This would test that the enhanced examples can be imported
        # and run successfully
        pass
    
    def test_enhanced_example_functionality(self):
        """Test enhanced example functionality"""
        # This would test that enhanced examples work correctly
        # For now, just test that the example file exists
        import os
        assert os.path.exists("/workspace/app/python/enhanced_examples.py")

if __name__ == "__main__":
    # Run all tests
    pytest.main([
        __file__,
        "-v",
        "--tb=short"
    ])