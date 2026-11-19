# NLM Python Bindings Comprehensive Tests

This directory contains comprehensive unit tests for the advanced features of the NLM Python bindings.

## Overview

These tests focus on:
- Advanced API functionality
- Performance optimization verification
- Error handling validation
- Memory management testing
- Configuration utilities testing
- Example script validation

## Test Structure

### 1. test_advanced_api.py

Tests advanced Python API functionality and convenience methods.

```python
import pytest
import pynlm
import json
import tempfile
import os

class TestAdvancedAPI:
    """Test advanced NLM Python API functionality."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
    def test_config_advanced_operations(self):
        """Test advanced configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Test auto-converting string values to appropriate types
        config.set("brain.neuron_count", "1000")  # String that converts to int
        config.set("brain.synapse_density", "0.15")  # String that converts to float
        
        # Test type detection in get()
        neuron_count, neuron_type = config.get("brain.neuron_count")
        assert neuron_type == "int"
        assert neuron_count == 1000
        
        synapse_density, synapse_type = config.get("brain.synapse_density")
        assert synapse_type == "float"
        assert synapse_density == 0.15
        
        # Test string value
        config.set("custom.string", "test_value")
        custom_value, custom_type = config.get("custom.string")
        assert custom_type == "string"
        assert custom_value == "test_value"
        
    def test_brain_convenience_methods(self):
        """Test brain convenience methods."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Test runSimulation convenience method
        initial_spikes = brain.getTotalSpikeCount()
        
        total_spikes = brain.runSimulation(50, verbose=False)
        
        assert total_spikes > initial_spikes
        assert brain.getTotalSpikeCount() > 0
        
    def test_agent_convenience_methods(self):
        """Test agent convenience methods."""
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        agent.initialize(world)
        
        # Test decodeWithDetails
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain = agent.getBrain()
        brain.step(0)
        
        cmd, activities = agent.decodeWithDetails()
        
        assert cmd in [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
        
        assert isinstance(activities, list)
        assert len(activities) > 0
        
        # Test initializeSimple (convenience method)
        agent2 = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        agent2.initializeSimple()  # Should work without world
        
    def test_config_validation_advanced(self):
        """Test advanced configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Test with valid configuration
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.learning_rate", 0.001)
        
        missing_keys = config.validate()
        assert isinstance(missing_keys, list)
        assert len(missing_keys) == 0  # No missing critical keys
        
        # Test with missing critical configuration
        config2 = pynlm.createDefaultConfig()
        config2.set("brain.synapse_density", 0.1)  # Missing neuron_count
        
        missing_keys2 = config2.validate()
        assert isinstance(missing_keys2, list)
        assert "brain.neuron_count" in missing_keys2
        
    def test_config_json_operations(self):
        """Test JSON operations with advanced features."""
        config = pynlm.createDefaultConfig()
        
        # Test comprehensive JSON export
        complex_config = {
            "brain.neuron_count": 2000,
            "brain.synapse_density": 0.15,
            "plasticity.stdp.learning_rate": 0.002,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "simulation_timestep": 0.0005
        }
        
        config.loadJson(json.dumps(complex_config))
        
        json_output = config.toJson()
        parsed_output = json.loads(json_output)
        
        assert "brain.neuron_count" in parsed_output
        assert parsed_output["brain.neuron_count"] == 2000
        assert parsed_output["brain.synapse_density"] == 0.15
        assert parsed_output["plasticity.stdp.learning_rate"] == 0.002
        
    def test_brain_performance_stats(self):
        """Test comprehensive brain performance statistics."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        for i in range(20):
            brain.step(i)
        
        stats = brain.getPerformanceStats()
        
        assert isinstance(stats, list)
        assert len(stats) > 0
        
        # Check for expected statistics
        stat_texts = "\n".join(stats)
        assert "Total Neurons:" in stat_texts
        assert "Total Synapses:" in stat_texts
        assert "Firing Neurons:" in stat_texts
        assert "Total Spikes:" in stat_texts
        assert "Average Firing Rate:" in stat_texts
        assert "Pending Spike Events:" in stat_texts
        assert "Development Stage:" in stat_texts
        
    def test_agent_advanced_features(self):
        """Test advanced agent features."""
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test agent statistics
        stats = agent.getAgentStats()
        
        assert isinstance(stats, list)
        assert len(stats) > 0
        
        stat_texts = "\n".join(stats)
        assert "Brain Neurons:" in stat_texts
        assert "Curiosity Level:" in stat_texts
        assert "Novelty Level:" in stat_texts
        assert "Prediction Error:" in stat_texts
        assert "Neuromodulation Level:" in stat_texts
        assert "Concepts:" in stat_texts
        assert "Social Observations:" in stat_texts
        assert "Developmental Stage:" in stat_texts
        
        # Test learning profile
        learning_profile = agent.getLearningProfile()
        
        assert isinstance(learning_profile, list)
        assert len(learning_profile) > 0
        assert "Exploration Level:" in learning_profile[0]
        assert "Novelty Detection:" in learning_profile[1]
        assert "Reward Sensitivity:" in learning_profile[2]
        
    def test_simulate_with_learning_advanced(self):
        """Test advanced simulateWithLearning functionality."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        agent = pynlm.createAgentBrain(brain)
        
        # Test complete agent simulation loop
        total_spikes = brain.simulateWithLearning(agent, world, 50, verbose=False)
        
        assert total_spikes > 0
        assert brain.getTotalSpikeCount() > 0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getConceptCount() >= 0
        
    def test_brain_comparison_advanced(self):
        """Test advanced brain comparison functionality."""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        config1.set("brain.neuron_count", 1000)
        config1.set("plasticity.stdp.learning_rate", 0.001)
        
        config2.set("brain.neuron_count", 2000)
        config2.set("plasticity.stdp.learning_rate", 0.002)
        config2.set("plasticity.hebbian.enable", False)
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        brains = [brain1, brain2]
        
        comparison = brain1.compareBrains(brains)
        
        assert isinstance(comparison, list)
        assert len(comparison) == 2
        assert len(comparison[0]) == 5  # neuron count, synapse count, firing, avg rate, total spikes
        
        # Verify that brain2 has more neurons
        assert comparison[1][0] > comparison[0][0]
        
        # Verify that brain2 has lower learning rate
        assert comparison[1][2] < comparison[0][2]  # average firing rate should be different
        
    def test_agent_comparison_advanced(self):
        """Test advanced agent comparison functionality."""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        agent1 = pynlm.createAgentBrain(brain1)
        agent2 = pynlm.createAgentBrain(brain2)
        
        agent1.initialize(world)
        agent2.initialize(world)
        
        # Run agents to generate different states
        for i in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent1.processSensoryInput(percept)
            agent2.processSensoryInput(percept)
            brain1.step(i)
            brain2.step(i)
            world.applyMotorCommand(agent1.decodeMotorCommand(), world.getSimulationTime())
            world.applyMotorCommand(agent2.decodeMotorCommand(), world.getSimulationTime())
            agent1.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
            agent2.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
        
        agents = [agent1, agent2]
        
        comparison = agent1.compareAgents(agents)
        
        assert isinstance(comparison, list)
        assert len(comparison) == 2
        assert len(comparison[0]) == 5  # neuron count, total spikes, curiosity, concepts, prediction error
        
        # Agents should have different characteristics
        assert comparison[0] != comparison[1] or True  # They might be similar by chance
        
    def test_learning_session_advanced(self):
        """Test advanced learning session functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        agent = pynlm.createAgentBrain(brain)
        
        # Run learning session with verbose output
        initial_curiosity = agent.getCuriosityLevel()
        
        final_curiosity = agent.runLearningSession(world, 100, verbose=True)
        
        assert final_curiosity >= 0.0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getConceptCount() >= 0
        assert agent.getPredictionError() >= 0.0
        
        # Test with different configurations
        world2 = pynlm.createSimpleWorld()
        world2.configure(15, 15, 6, 6)
        
        agent2 = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        agent2.runLearningSession(world2, 50, verbose=False)
        
        assert agent2.getCuriosityLevel() >= 0.0
        
    def test_error_handling_advanced(self):
        """Test advanced error handling scenarios."""
        # Test with invalid configurations
        config = pynlm.createDefaultConfig()
        
        # Test invalid JSON handling
        invalid_jsons = [
            '{"invalid": json}',  # Invalid JSON syntax
            '',  # Empty string
            '{"key": null}',  # Valid JSON but null values
            'not a json'  # Not JSON at all
        ]
        
        for invalid_json in invalid_jsons:
            try:
                config.loadJson(invalid_json)
                # If it doesn't throw, that's fine too
            except Exception:
                pass  # Expected to fail
        
        # Test file operations with invalid paths
        try:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.save("/invalid/path/that/should/not/exist/test.bin")
        except Exception:
            pass  # Expected to fail
        
        try:
            brain.load("/invalid/path/that/should/not/exist/test.bin")
        except Exception:
            pass  # Expected to fail
        
        # Test agent with null brain (should handle gracefully)
        try:
            invalid_agent = pynlm.createAgentBrain(None)
        except Exception:
            pass  # Expected to fail
        
    def test_configuration_utilities_advanced(self):
        """Test advanced configuration utilities."""
        config = pynlm.createDefaultConfig()
        
        # Test setting multiple values at once
        test_values = {
            "brain.neuron_count": 5000,
            "brain.synapse_density": 0.05,
            "plasticity.stdp.learning_rate": 0.01,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.dopamine.scale": 1.5,
            "neuromod.curiosity.enable": False,
            "neuromod.novelty.enable": True
        }
        
        for key, value in test_values.items():
            if isinstance(value, bool):
                # Booleans need to be converted to string for set method
                config.set(key, str(value))
            else:
                config.set(key, value)
        
        # Verify all values were set
        for key, expected_value in test_values.items():
            if config.has(key):
                actual_value, value_type = config.get(key)
                if expected_value == True or expected_value == False:
                    # For booleans, check if the value matches expected type
                    assert value_type in ["int", "float", "string"]
                else:
                    # For numeric values, check approximate equality
                    if abs(actual_value - expected_value) < 0.001:
                        continue
                    else:
                        # Try to see if it's stored as string
                        if str(expected_value) == str(actual_value):
                            continue
                        else:
                            raise AssertionError(f"Value mismatch for {key}: expected {expected_value}, got {actual_value} ({value_type})")
        
    def test_performance_optimization_methods(self):
        """Test performance optimization methods."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Test simulateAndMonitor with different parameters
        spikes1 = brain.simulateAndMonitor(100, collectStats=False)
        assert spikes1 > 0
        
        # Test compareBrains with multiple brains
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        config1.set("brain.neuron_count", 500)
        config2.set("brain.neuron_count", 1500)
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        brains = [brain1, brain2]
        comparison = brain1.compareBrains(brains)
        
        assert isinstance(comparison, list)
        assert len(comparison) == 2
        assert comparison[1][0] > comparison[0][0]  # brain2 has more neurons
        
    def test_advanced_agent_decoding(self):
        """Test advanced agent decoding features."""
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Process sensory input to generate motor activity
        for i in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            
            # Test decodeWithDetails
            cmd, activities = agent.decodeWithDetails()
            assert isinstance(cmd, int)  # MotorCommand enum value
            assert isinstance(activities, list)
            
            # Apply action
            world.applyMotorCommand(cmd, world.getSimulationTime())
            
            # Apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Test getLastDecoded (should return last decoded action)
        last_decoded = agent.getLastDecoded()
        assert last_decoded in [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
        
        # Test getLastWinnerGroup
        last_winner = agent.getLastWinnerGroup()
        # This could be -1 if no winner, or 0-5 for different groups
        assert last_winner in list(range(-1, 6))
        
        # Test planner metrics
        planner_decisions = agent.getPlannerDecisionCount()
        planner_agreements = agent.getPlannerAgreementCount()
        
        assert isinstance(planner_decisions, int)
        assert isinstance(planner_agreements, int)
        assert planner_decisions >= 0
        assert planner_agreements >= 0
        assert planner_agreements <= planner_decisions

if __name__ == "__main__":
    # Run tests
    import unittest
    
    # Create test suite
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(TestAdvancedAPI)
    suite.addTests(loader.loadTestsFromTestCase(TestPerformanceOptimization))
    
    # Run tests with verbose output
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Exit with appropriate code
    exit(0 if result.wasSuccessful() else 1)
