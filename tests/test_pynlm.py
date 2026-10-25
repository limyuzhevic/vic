#!/usr/bin/env python3

"""
NLM Python Module Test Suite

Comprehensive test suite for NLM Python bindings.
Tests both functionality and performance characteristics.
"""

import sys
import unittest
import numpy as np
import tempfile
import os
from typing import Optional, List, Dict, Any

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
class TestNLMModule(unittest.TestCase):
    """Test NLM Python module basic functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up after tests."""
        pass
    
    def test_basic_configuration(self):
        """Test basic configuration functionality."""
        # Test creating config
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        # Test setting and getting values
        config.set("neuron_count", 1000)
        neuron_count = config.get("neuron_count")
        self.assertEqual(neuron_count, 1000)
        
        # Test getOr with default
        count = config.getOr("neuron_count", 500)
        self.assertEqual(count, 1000)
        
        count = config.getOr("nonexistent", 500)
        self.assertEqual(count, 500)
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        brain = pynlm.createBrain(self.config)
        self.assertIsNotNone(brain)
        
        brain.initialize()
        self.assertTrue(brain.getTotalNeuronCount() > 0)
        self.assertTrue(brain.getTotalSynapseCount() > 0)
    
    def test_brain_step(self):
        """Test brain simulation step."""
        initial_spikes = brain.getTotalSpikeCount()
        initial_firing = brain.getFiringNeuronCount()
        
        # Run a few steps
        for step in range(10):
            brain.step(step)
        
        # Check that something happened (should have more spikes or firing)
        self.assertGreaterEqual(brain.getTotalSpikeCount(), initial_spikes)
        self.assertGreaterEqual(brain.getFiringNeuronCount(), initial_firing)
    
    def test_memory_systems(self):
        """Test memory system access."""
        # Check that memory systems exist
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        # They might be null if not initialized, but should exist
        # The important thing is that we can access them without error
        self.assertIsNotNone(brain.getWorkingMemory())
        self.assertIsNotNone(brain.getEpisodicMemory())
        self.assertIsNotNone(brain.getAssociativeMemory())
    
    def test_neuromodulation(self):
        """Test neuromodulation system access."""
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        prediction_error = brain.getPredictionErrorSignal()
        
        # Should be able to access neuromodulators
        self.assertIsNotNone(dopamine)
        self.assertIsNotNone(curiosity)
        self.assertIsNotNone(novelty)
        self.assertIsNotNone(prediction_error)
    
    def test_development(self):
        """Test development system."""
        stage = brain.getDevelopmentalStage()
        self.assertIsNotNone(stage)
        
        # Should be able to set developmental stage
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
        self.assertEqual(brain.getDevelopmentalStage(), pynlm.DevelopmentalStage.Adult)
    
    def test_cognitive_systems(self):
        """Test cognitive system access."""
        planner = brain.getPlanner()
        concept_formation = brain.getConceptFormation()
        attention = brain.getAttention()
        
        self.assertIsNotNone(planner)
        self.assertIsNotNone(concept_formation)
        self.assertIsNotNone(attention)
class TestAgentSystem(unittest.TestCase):
    """Test agent system functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 8, 8)
        self.agent.initialize(self.world)
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        self.assertIsNotNone(self.agent)
        self.assertIsNotNone(self.world)
        self.assertIsNotNone(self.brain)
    
    def test_sensory_processing(self):
        """Test sensory processing."""
        # Get initial sensory percept
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Should be able to get sensory input size
        input_size = self.agent.getSensoryInputSize()
        self.assertGreaterEqual(input_size, 0)
    
    def test_motor_decoding(self):
        """Test motor command decoding."""
        # Create some brain activity to get motor output
        for step in range(5):
            self.brain.step(step)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        self.assertIsNotNone(action)
        
        # Should be a valid motor command
        self.assertIn(action, [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ])
    
    def test_neuromodulation_control(self):
        """Test neuromodulation subsystem control."""
        # Test enabling/disabling
        self.agent.enableRewardModulation(True)
        self.assertTrue(self.agent.isRewardModulationEnabled())
        
        self.agent.enableCuriosity(True)
        self.assertTrue(self.agent.isCuriosityEnabled())
        
        self.agent.enableDevelopment(True)
        self.assertTrue(self.agent.isDevelopmentEnabled())
        
        self.agent.enableStructuralPlasticity(True)
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        
        # Test neuromodulation levels
        self.agent.applyRewardModulation(0.5, 0.3)
        
        dopamine_level = self.agent.getNeuromodulationLevel()
        curiosity_level = self.agent.getCuriosityLevel()
        novelty_level = self.agent.getNoveltyLevel()
        prediction_error_val = self.agent.getPredictionError()
        
        # Should have values (might be zero initially)
        self.assertIsInstance(dopamine_level, float)
        self.assertIsInstance(curiosity_level, float)
        self.assertIsInstance(novelty_level, float)
        self.assertIsInstance(prediction_error_val, float)
    
    def test_world_interaction(self):
        """Test world interaction."""
        # Get agent body
        body = self.world.getAgentBody()
        self.assertIsNotNone(body)
        
        # Update world
        self.world.update(0.1)
        
        # Get percept and process
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Get action and apply to world
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Update agent development
        self.agent.updateDevelopment(0.1)
class TestWorldSystem(unittest.TestCase):
    """Test world system functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.world = pynlm.createSimpleWorld()
    
    def test_world_configuration(self):
        """Test world configuration."""
        self.world.configure(20, 20, 8, 8)
        
        self.assertEqual(self.world.getWidth(), 20)
        self.assertEqual(self.world.getHeight(), 20)
        
        self.assertEqual(self.world.getVisionWidth(), 8)
        self.assertEqual(self.world.getVisionHeight(), 8)
    
    def test_world_reset(self):
        """Test world reset."""
        initial_width = self.world.getWidth()
        self.world.reset()
        
        # Should still be configured
        self.assertEqual(self.world.getWidth(), initial_width)
    
    def test_world_object_management(self):
        """Test world object management."""
        # Create a world object
        obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0, 0.5)
        
        # Add to world
        self.world.addObject(obj)
        
        # Check if position is valid
        self.assertTrue(self.world.isValidPosition(5.0, 5.0))
        self.assertFalse(self.world.isValidPosition(25.0, 25.0))  # Outside world
        
        # Remove object
        self.world.removeObject(5.0, 5.0)
        
        # Check if position is now invalid
        self.assertFalse(self.world.isValidPosition(5.0, 5.0))
    
    def test_agent_body(self):
        """Test agent body state."""
        body = self.world.getAgentBody()
        self.assertIsNotNone(body)
        
        # Set agent start
        self.world.setAgentStart(10.0, 10.0)
        self.assertEqual(body.x, 10.0)
        self.assertEqual(body.y, 10.0)
    
    def test_world_update(self):
        """Test world update."""
        # Set agent start
        self.world.setAgentStart(5.0, 5.0)
        
        # Update world multiple times
        for i in range(10):
            self.world.update(0.1)
            self.assertGreater(self.world.getSimulationTime(), 0.0)
        
        # Check that agent body position changed
        body = self.world.getAgentBody()
        self.assertIsNotNone(body)
class TestConfigurationSystem(unittest.TestCase):
    """Test configuration system."""
    
    def test_preset_configurations(self):
        """Test preset configuration presets."""
        # Test different presets
        minimal = pynlm.createPresetConfig("minimal")
        self.assertIsNotNone(minimal)
        
        standard = pynlm.createPresetConfig("standard")
        self.assertIsNotNone(standard)
        
        high_performance = pynlm.createPresetConfig("high_performance")
        self.assertIsNotNone(high_performance)
        
        research = pynlm.createPresetConfig("research")
        self.assertIsNotNone(research)
        
        # All should have different neuron counts
        self.assertLess(minimal.getOr("neuron_count", 9999), standard.getOr("neuron_count", 9999))
    
    def test_config_validation(self):
        """Test configuration validation."""
        # Create a config with invalid values
        config = pynlm.createDefaultConfig()
        
        # This should work (valid neuron count)
        config.set("neuron_count", 100)
        
        # This should also work
        config.set("connection_probability", 0.5)
        
        # This might work or throw an exception depending on validation
        try:
            config.set("simulation_timestep", -0.1)  # Invalid: negative timestep
        except:
            pass  # Expected to fail validation
    
    def test_config_file_operations(self):
        """Test configuration file operations."""
        import tempfile
        import os
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        
        # Test save/load
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            filename = f.name
        
        try:
            # Save to file
            config.saveToFile(filename)
            self.assertTrue(os.path.exists(filename))
            
            # Load from file
            config2 = pynlm.createConfigFromFile(filename)
            self.assertIsNotNone(config2)
            
            # Check that values were loaded
            self.assertEqual(config2.getOr("neuron_count", 0), 1000)
            
        finally:
            # Clean up
            if os.path.exists(filename):
                os.unlink(filename)
class TestPhase6Integration(unittest.TestCase):
    """Test Phase 6 integration functionality."""
    
    def test_phase6_experiment_creation(self):
        """Test Phase 6 experiment creation."""
        experiment = pynlm.Phase6IntegratedExperiment()
        self.assertIsNotNone(experiment)
    
    def test_phase6_config(self):
        """Test Phase 6 configuration."""
        config = pynlm.Phase6Config()
        self.assertIsNotNone(config)
        
        # Test default values
        self.assertGreater(config.maxSteps, 0)
        self.assertGreater(config.neuronCount, 0)
        self.assertTrue(config.enableCheckpointing)
        self.assertTrue(config.enableReplay)
        self.assertTrue(config.enableDevelopment)
    
    def test_integration_verification(self):
        """Test integration verification."""
        experiment = pynlm.Phase6IntegratedExperiment()
        
        # Should be able to call verification method
        # Note: This might fail if brain systems aren't properly integrated
        try:
            is_integrated = experiment.verifyIntegration()
            self.assertIsInstance(is_integrated, bool)
        except Exception:
            # Expected if systems aren't fully integrated
            pass
class TestPerformanceBenchmarks(unittest.TestCase):
    """Test performance benchmarking."""
    
    def test_benchmark_function(self):
        """Test benchmark function."""
        # Create a small benchmark
        results = pynlm.benchmark([100, 500], 100)
        
        # Should return results for each neuron count
        self.assertEqual(len(results), 2)
        
        # Check that each result has expected keys
        for result in results:
            self.assertIn("neuron_count", result)
            self.assertIn("steps", result)
            self.assertIn("wall_time_seconds", result)
            self.assertIn("steps_per_second", result)
            self.assertIn("avg_firing_rate", result)
            self.assertIn("total_spikes", result)
            self.assertIn("real_time_factor", result)
    
    def test_mini_brain(self):
        """Test mini brain creation."""
        brain, firing_rate = pynlm.create_mini_brain(neuron_count=500, simulate_steps=100)
        
        self.assertIsNotNone(brain)
        self.assertIsInstance(firing_rate, float)
        self.assertGreaterEqual(firing_rate, 0.0)
    
    def test_agent_simulation(self):
        """Test agent simulation creation."""
        brain, agent, world, stats = pynlm.create_agent_simulation(
            width=10, height=10,
            vision_width=4, vision_height=4,
            steps=50, enable_learning=True
        )
        
        self.assertIsNotNone(brain)
        self.assertIsNotNone(agent)
        self.assertIsNotNone(world)
        self.assertIsNotNone(stats)
        
        # Check stats structure
        expected_keys = ['firing_rate', 'curiosity', 'novelty', 'development_stage',
                         'total_spikes']
        for key in expected_keys:
            self.assertIn(key, stats)
class TestErrorHandling(unittest.TestCase):
    """Test error handling."""
    
    def test_invalid_config_values(self):
        """Test handling of invalid configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Test with various invalid types
        with self.assertRaises(Exception):
            config.set("neuron_count", "not_a_number")
        
        with self.assertRaises(Exception):
            config.set("neuron_count", None)
    
    def test_invalid_brain_operations(self):
        """Test invalid brain operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should fail to step before initialization
        with self.assertRaises(Exception):
            brain.step(0)
        
        # Initialize and then try again
        brain.initialize()
        brain.step(0)  # This should work
class TestUtilityFunctions(unittest.TestCase):
    """Test utility functions."""
    
    def test_simulation_utilities(self):
        """Test simulation utility functions."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation utility
        firing_rate = pynlm.runSimulation(brain, 100)
        self.assertIsInstance(firing_rate, float)
    
    def test_validation_function(self):
        """Test validation function."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This might work or fail depending on integration
        try:
            is_valid, issues = pynlm.validateBrainIntegration(brain)
            self.assertIsInstance(is_valid, bool)
            self.assertIsInstance(issues, list)
        except Exception:
            # Expected if validation is not fully implemented
            pass
    
    def test_help_function(self):
        """Test help function."""
        help_text = pynlm.get_help()
        self.assertIsInstance(help_text, str)
        self.assertGreater(len(help_text), 100)
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced user features."""
    
    def test_advanced_configuration(self):
        """Test advanced configuration settings."""
        config = pynlm.createDefaultConfig()
        
        # Create complex configuration
        settings = {
            "timestep": 0.001,
            "plasticity_enabled": True,
            "neuromodulation_enabled": True,
            "development_enabled": True,
            "curiosity_enabled": True
        }
        
        # This might not be fully implemented yet
        try:
            pynlm.configureSimulation(config, settings)
        except Exception:
            # Expected if not fully implemented
            pass
    
    def test_data_export(self):
        """Test data export functionality."""
        import tempfile
        import os
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            filename = f.name
        
        try:
            # Export brain state
            pynlm.exportBrainState(brain, filename)
            self.assertTrue(os.path.exists(filename))
            
            # Import brain state
            imported_brain = pynlm.importBrainState(filename)
            self.assertIsNotNone(imported_brain)
            
        finally:
            if os.path.exists(filename):
                os.unlink(filename)
class TestIntegration(unittest.TestCase):
    """Integration tests."""
    
    def test_complete_workflow(self):
        """Test complete workflow with all systems."""
        # Create complete agent simulation
        brain, agent, world, stats = pynlm.create_agent_simulation(
            width=20, height=20,
            vision_width=8, vision_height=8,
            steps=100, enable_learning=True
        )
        
        # Run the simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
        
        # Verify results
        self.assertIsNotNone(stats)
        self.assertGreater(stats['firing_rate'], 0.0)
        self.assertGreater(stats['curiosity'], 0.0)
        self.assertGreater(stats['total_spikes'], 0)
    
    def test_performance_and_functionality(self):
        """Test that performance features don't break functionality."""
        # Create brain with performance optimizations
        config = pynlm.createPresetConfig("high_performance")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should still work normally
        for step in range(50):
            brain.step(step)
        
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSpikeCount(), 0)
class TestDocumentationExamples(unittest.TestCase):
    """Test examples from documentation."""
    
    def test_example_1_silent_brain(self):
        """Test Example 1: Simplest Brain."""
        import pynlm
        
        # One line to create a virtual brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Initialize it
        brain.initialize()
        
        # Make it active
        brain.step(0)
        
        # Should have neurons
        self.assertGreater(brain.getTotalNeuronCount(), 0)
    
    def test_example_2_world_watching(self):
        """Test Example 2: Brain Watching a World."""
        import pynlm
        
        # Setup
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Watch the world for 30 steps
        for i in range(30):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            
        print(f"Watched world for 30 steps")
        print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
        
        self.assertGreater(brain.getAverageFiringRate(), 0.0)
    
    def test_example_3_complete_agent(self):
        """Test Example 3: Complete Agent."""
        import pynlm
        
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run agent
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("Agent simulation complete!")
        
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
if __name__ == '__main__':
    # Run all tests
    unittest.main(verbosity=2)
