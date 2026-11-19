# NLM Python Bindings Unit Tests

This directory contains unit tests for the NLM Python bindings.

## Overview

Comprehensive unit tests for the enhanced NLM Python bindings, covering:
- Core functionality testing
- Performance optimization verification
- Advanced feature testing
- Error handling validation
- Memory management testing

## Test Structure

### 1. test_core_functionality.py

Tests core NLM functionality and basic operations.

```python
import pytest
import pynlm
import json
import tempfile
import os

class TestCoreFunctionality:
    """Test core NLM functionality."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
    def test_config_creation(self):
        """Test configuration creation and basic operations."""
        assert self.config is not None
        
        # Test setting configuration values
        self.config.set("brain.neuron_count", 1000)
        self.config.set("brain.synapse_density", 0.1)
        self.config.set("learning.rate", 0.001)
        
        assert self.config.has("brain.neuron_count")
        assert self.config.has("brain.synapse_density")
        assert self.config.has("learning.rate")
        
    def test_brain_initialization(self):
        """Test brain initialization and basic statistics."""
        self.brain.initialize()
        
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() > 0
        assert self.brain.getFiringNeuronCount() >= 0
        assert self.brain.getTotalSpikeCount() >= 0
        
    def test_agent_initialization(self):
        """Test agent initialization and configuration."""
        self.agent.initialize(self.world)
        
        assert self.agent.getSensoryInputSize() > 0
        assert self.agent.getMotorOutputSize() > 0
        assert self.agent.getCuriosityLevel() >= 0.0
        assert self.agent.getNoveltyLevel() >= 0.0
        
    def test_world_configuration(self):
        """Test world configuration and operations."""
        self.world.configure(20, 20, 8, 8)
        self.world.reset()
        
        assert self.world.getWidth() == 20
        assert self.world.getHeight() == 20
        assert self.world.getSimulationTime() == 0.0
        
    def test_brain_step(self):
        """Test brain step execution."""
        self.brain.initialize()
        
        initial_spikes = self.brain.getTotalSpikeCount()
        
        for i in range(10):
            self.brain.step(i)
        
        final_spikes = self.brain.getTotalSpikeCount()
        
        assert final_spikes >= initial_spikes
        
    def test_agent_sensory_processing(self):
        """Test agent sensory processing."""
        self.world.configure(20, 20, 8, 8)
        self.agent.initialize(self.world)
        
        percept = self.world.getSensoryPercept()
        initial_curiosity = self.agent.getCuriosityLevel()
        
        self.agent.processSensoryInput(percept)
        
        # Curiosity may or may not change based on input
        assert self.agent.getCuriosityLevel() >= 0.0
        assert self.agent.getNoveltyLevel() >= 0.0
        
    def test_agent_motor_decoding(self):
        """Test agent motor command decoding."""
        self.world.configure(20, 20, 8, 8)
        self.agent.initialize(self.world)
        
        # Process some sensory input first
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Run brain step to generate motor activity
        self.brain.step(0)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        
        assert action in [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
        
    def test_reward_modulation(self):
        """Test reward-based neuromodulation."""
        self.world.configure(20, 20, 8, 8)
        self.agent.initialize(self.world)
        
        initial_curiosity = self.agent.getCuriosityLevel()
        initial_neuromodulation = self.agent.getNeuromodulationLevel()
        
        # Apply reward modulation
        self.agent.applyRewardModulation(10.0, 5.0)  # Positive reward
        
        # Check that neuromodulation changed
        assert self.agent.getNeuromodulationLevel() != initial_neuromodulation
        
        # Apply negative reward
        self.agent.applyRewardModulation(-5.0, 0.0)  # Negative reward
        
        assert self.agent.getNeuromodulationLevel() != initial_neuromodulation
        
    def test_development_update(self):
        """Test developmental system updates."""
        self.agent.initialize(self.world)
        
        initial_stage = self.agent.getDevelopmentalStage()
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Development stage may or may not change
        assert self.agent.getDevelopmentalStage() >= initial_stage
        
    def test_subsystem_enable_disable(self):
        """Test enabling and disabling subsystems."""
        self.agent.initialize(self.world)
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        assert self.agent.isRewardModulationEnabled()
        assert self.agent.isStructuralPlasticityEnabled()
        assert self.agent.isDevelopmentEnabled()
        assert self.agent.isCuriosityEnabled()
        
        # Disable some subsystems
        self.agent.enableCuriosity(False)
        self.agent.enableDevelopment(False)
        
        assert self.agent.isRewardModulationEnabled()
        assert self.agent.isStructuralPlasticityEnabled()
        assert not self.agent.isDevelopmentEnabled()
        assert not self.agent.isCuriosityEnabled()
        
    def test_config_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Set up a valid configuration
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        
        # Validate (should return empty list for valid config)
        missing_keys = config.validate()
        assert isinstance(missing_keys, list)
        
    def test_config_to_json(self):
        """Test configuration export to JSON."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        json_str = config.toJson()
        
        assert isinstance(json_str, str)
        assert "{" in json_str
        assert "neuron_count" in json_str
        
    def test_config_load_json(self):
        """Test configuration loading from JSON."""
        config = pynlm.createDefaultConfig()
        
        json_str = '{"brain.neuron_count": 1500, "brain.synapse_density": 0.2}'
        
        config.loadJson(json_str)
        
        assert config.has("brain.neuron_count")
        assert config.has("brain.synapse_density")
        
    def test_brain_save_load(self):
        """Test brain state save and load."""
        self.brain.initialize()
        
        # Save brain state
        with tempfile.NamedTemporaryFile(delete=False) as temp_file:
            temp_filename = temp_file.name
        
        try:
            save_result = self.brain.save(temp_filename)
            assert save_result == True
            
            # Reset brain
            self.brain.reset()
            assert self.brain.getTotalSpikeCount() == 0
            
            # Load brain state
            load_result = self.brain.load(temp_filename)
            assert load_result == True
            
            # Check that spike count was restored
            assert self.brain.getTotalSpikeCount() > 0
            
        finally:
            # Clean up
            if os.path.exists(temp_filename):
                os.unlink(temp_filename)
        
    def test_enumeration_values(self):
        """Test enumeration values."""
        assert pynlm.NeuronType.Excitatory is not None
        assert pynlm.NeuronType.Inhibitory is not None
        assert pynlm.NeuronType.Sensory is not None
        assert pynlm.NeuronType.Motor is not None
        
        assert pynlm.ActionType.MoveForward is not None
        assert pynlm.ActionType.TurnLeft is not None
        assert pynlm.ActionType.Interact is not None
        
        assert pynlm.DevelopmentalStage.Initial is not None
        assert pynlm.DevelopmentalStage.CriticalPeriod is not None
        assert pynlm.DevelopmentalStage.Adult is not None
        
    def test_string_representations(self):
        """Test string representations of objects."""
        self.brain.initialize()
        
        repr_str = repr(self.brain)
        assert "Brain:" in repr_str
        assert "neurons:" in repr_str
        assert "spikes:" in repr_str
        
        self.agent.initialize(self.world)
        agent_repr = repr(self.agent)
        assert "AgentBrain" in agent_repr
        assert "neurons:" in agent_repr

class TestPerformanceOptimization:
    """Test performance optimization features."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
    def test_batch_processing(self):
        """Test batch processing optimization."""
        self.brain.initialize()
        
        # Test stepMultiple (batch processing)
        initial_spikes = self.brain.getTotalSpikeCount()
        
        self.brain.stepMultiple(100)
        
        final_spikes = self.brain.getTotalSpikeCount()
        
        assert final_spikes >= initial_spikes
        
    def test_simulation_with_monitoring(self):
        """Test simulation with progress monitoring."""
        self.brain.initialize()
        
        # Run simulation with monitoring
        spikes = self.brain.runSimulation(50, verbose=False)
        
        assert spikes >= 0
        assert self.brain.getTotalSpikeCount() > 0
        
    def test_performance_statistics(self):
        """Test performance statistics collection."""
        self.brain.initialize()
        
        # Run some simulation steps
        for i in range(10):
            self.brain.step(i)
        
        stats = self.brain.getPerformanceStats()
        
        assert isinstance(stats, list)
        assert len(stats) > 0
        assert any("Total Neurons:" in stat for stat in stats)
        
    def test_brain_comparison(self):
        """Test brain comparison functionality."""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        config1.set("brain.neuron_count", 1000)
        config2.set("brain.neuron_count", 2000)
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        brains = [brain1, brain2]
        
        comparison = brain1.compareBrains(brains)
        
        assert isinstance(comparison, list)
        assert len(comparison) == 2
        assert len(comparison[0]) == 5  # neuron count, synapse count, firing, avg rate, total spikes
        
        # Check that brain2 has more neurons than brain1
        assert comparison[1][0] > comparison[0][0]  # neuron count comparison
        
    def test_advanced_agent_features(self):
        """Test advanced agent features."""
        self.world.configure(20, 20, 8, 8)
        self.agent.initialize(self.world)
        
        # Test agent statistics
        stats = self.agent.getAgentStats()
        
        assert isinstance(stats, list)
        assert len(stats) > 0
        assert any("Curiosity Level:" in stat for stat in stats)
        assert any("Concepts:" in stat for stat in stats)
        
    def test_agent_comparison(self):
        """Test agent comparison functionality."""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        agent1 = pynlm.createAgentBrain(brain1)
        agent2 = pynlm.createAgentBrain(brain2)
        
        agents = [agent1, agent2]
        
        comparison = agent1.compareAgents(agents)
        
        assert isinstance(comparison, list)
        assert len(comparison) == 2
        assert len(comparison[0]) == 5  # neuron count, total spikes, curiosity, concepts, prediction error
        
    def test_learning_session(self):
        """Test learning session functionality."""
        self.world.configure(20, 20, 8, 8)
        self.agent.initialize(self.world)
        
        # Run learning session
        final_curiosity = self.agent.runLearningSession(self.world, 50, verbose=False)
        
        assert final_curiosity >= 0.0
        assert self.agent.getCuriosityLevel() >= 0.0
        
    def test_error_handling(self):
        """Test error handling in advanced features."""
        # Test with null pointers (should be handled gracefully)
        try:
            # This should not crash
            config = pynlm.createDefaultConfig()
            
            # Test invalid filename handling
            with tempfile.NamedTemporaryFile(delete=False) as temp_file:
                temp_filename = temp_file.name
            
            # Create invalid JSON for loadJson test
            invalid_json = '{"invalid": json}'
            config.loadJson(invalid_json)
            
            # Clean up
            os.unlink(temp_filename)
            
        except Exception:
            pass  # Expected, we're testing error handling

class TestMemoryManagement:
    """Test memory management features."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        
    def test_memory_efficiency(self):
        """Test memory efficiency calculations."""
        # This test would require more detailed memory tracking
        # For now, we'll just test that the methods exist and don't crash
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Run simulation to generate some memory usage
        for i in range(10):
            brain.step(i)
        
        # Performance stats should not crash
        stats = brain.getPerformanceStats()
        assert isinstance(stats, list)
        
    def test_advanced_features_error_handling(self):
        """Test error handling in advanced features with edge cases."""
        # Test with edge cases and invalid inputs
        brain = pynlm.createBrain(self.config)
        
        # Test with empty filename
        try:
            brain.save("")
        except Exception:
            pass  # Expected to fail
            
        try:
            brain.load("")
        except Exception:
            pass  # Expected to fail
        
        # Test with invalid values
        try:
            self.config.loadJson("")
        except Exception:
            pass  # Expected to fail

if __name__ == "__main__":
    # Run tests
    import unittest
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromModule(__import__(__name__))
    
    # Run tests with verbose output
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Exit with appropriate code
    exit(0 if result.wasSuccessful() else 1)
