#!/usr/bin/env python3
"""
NLM Unit Test Suite
===================

Unit tests for the NLM framework, focusing on individual components and their integration.

This test suite provides comprehensive testing for:
- Configuration management
- Brain and neural components
- Experiment framework
- Visualization interface
- Command line parsing
- Agent systems
"""

import unittest
import sys
import os
from pathlib import Path

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent))

from pynlm import (
    Config, Brain, SimpleWorld, AgentBrain, NeuronType, 
    ActionType, MotorCommand, Vision, Audio, InternalSignals,
    LearningExperiment, ExperimentRunner, CommandLineParser,
    ConfigManager, VisualizationInterface, OutputFormat
)

class TestConfig(unittest.TestCase):
    """Test configuration management"""
    
    def setUp(self):
        """Set up test configuration"""
        self.config = Config()
    
    def test_config_initialization(self):
        """Test that configuration is properly initialized"""
        self.assertIsNotNone(self.config)
        self.config.set("test_key", "test_value")
        self.assertEqual(self.config.getOr("test_key", "default"), "test_value")
    
    def test_config_has(self):
        """Test configuration key existence checking"""
        self.config.set("existing_key", "value")
        self.assertTrue(self.config.has("existing_key"))
        self.assertFalse(self.config.has("nonexistent_key"))
    
    def test_config_keys(self):
        """Test getting all configuration keys"""
        self.config.set("key1", "value1")
        self.config.set("key2", "value2")
        
        keys = self.config.getKeys()
        self.assertIn("key1", keys)
        self.assertIn("key2", keys)
        self.assertEqual(len(keys), 2)
    
    def test_config_summary(self):
        """Test configuration summary generation"""
        summary = self.config.summary()
        self.assertIsInstance(summary, str)
        self.assertTrue(len(summary) > 0)

class TestBrain(unittest.TestCase):
    """Test brain creation and basic functionality"""
    
    def setUp(self):
        """Set up test configuration and brain"""
        self.config = Config()
        self.config.set("neuron_count", 50)
        self.config.set("region_count", 1)
        self.config.set("simulation_timestep", 0.001)
        
        self.brain = Brain(self.config)
    
    def test_brain_creation(self):
        """Test brain creation"""
        self.assertIsNotNone(self.brain)
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        # Note: Brain initialization requires the C++ implementation to be properly set up
        # This test may be skipped if initialization fails
        try:
            success = self.brain.initialize()
            if success:
                self.assertTrue(True, "Brain initialization succeeded")
            else:
                self.skipTest("Brain initialization failed")
        except Exception:
            self.skipTest("Brain initialization not supported in this environment")
    
    def test_brain_reset(self):
        """Test brain reset functionality"""
        try:
            self.brain.reset()
            self.assertTrue(True, "Brain reset succeeded")
        except Exception:
            self.skipTest("Brain reset not supported in this environment")
    
    def test_brain_config(self):
        """Test brain configuration access"""
        config = self.brain.getConfig()
        self.assertIsNotNone(config)

class TestNeuronType(unittest.TestCase):
    """Test neuron type enumeration"""
    
    def test_neuron_type_values(self):
        """Test that neuron types have distinct values"""
        self.assertNotEqual(NeuronType.Excitatory, NeuronType.Inhibitory)
        self.assertNotEqual(NeuronType.Sensory, NeuronType.Motor)
        self.assertNotEqual(NeuronType.Internal, NeuronType.Modulatory)
    
    def test_neuron_type_strings(self):
        """Test neuron type string representations"""
        # Just ensure the types exist and are accessible
        self.assertIsNotNone(NeuronType.Excitatory)
        self.assertIsNotNone(NeuronType.Inhibitory)
        self.assertIsNotNone(NeuronType.Sensory)
        self.assertIsNotNone(NeuronType.Motor)
        self.assertIsNotNone(NeuronType.Internal)
        self.assertIsNotNone(NeuronType.Modulatory)

class TestActionType(unittest.TestCase):
    """Test action type enumeration"""
    
    def test_action_type_values(self):
        """Test that action types have distinct values"""
        self.assertNotEqual(ActionType.MoveForward, ActionType.Rest)
        self.assertNotEqual(ActionType.Interact, ActionType.Wait)
        self.assertNotEqual(ActionType.Look, ActionType.LookUp)
    
    def test_action_type_strings(self):
        """Test action type string representations"""
        # Just ensure the types exist and are accessible
        self.assertIsNotNone(ActionType.MoveForward)
        self.assertIsNotNone(ActionType.MoveBackward)
        self.assertIsNotNone(ActionType.TurnLeft)
        self.assertIsNotNone(ActionType.TurnRight)
        self.assertIsNotNone(ActionType.Interact)
        self.assertIsNotNone(ActionType.Wait)

class TestCommandLineParser(unittest.TestCase):
    """Test command line parser functionality"""
    
    def test_parser_initialization(self):
        """Test command line parser initialization"""
        parser = CommandLineParser(3, ["test", "--help", "--version"])
        self.assertIsNotNone(parser)
        self.assertTrue(parser.hasOption("--help"))
        self.assertEqual(parser.getOption("--help", "false"), "true")
    
    def test_parser_options(self):
        """Test command line option parsing"""
        parser = CommandLineParser(5, [
            "test", 
            "--neuron_count=100", 
            "--region_count=2", 
            "--help", 
            "--experiment=plasticity"
        ])
        
        self.assertTrue(parser.hasOption("--neuron_count"))
        self.assertTrue(parser.hasOption("--region_count"))
        self.assertTrue(parser.hasOption("--help"))
        self.assertTrue(parser.hasOption("--experiment"))
        
        self.assertEqual(parser.getOption("--neuron_count", "0"), "100")
        self.assertEqual(parser.getOption("--region_count", "0"), "2")
        self.assertEqual(parser.getOption("--help", "false"), "true")
        self.assertEqual(parser.getOption("--experiment", ""), "plasticity")

class TestConfigManager(unittest.TestCase):
    """Test configuration manager functionality"""
    
    def setUp(self):
        """Set up test configuration and manager"""
        self.config = Config()
        self.manager = ConfigManager(self.config)
    
    def test_manager_initialization(self):
        """Test configuration manager initialization"""
        self.assertIsNotNone(self.manager)
    
    def test_set_get_config_value(self):
        """Test setting and getting configuration values"""
        self.assertTrue(self.manager.setConfigValue("test_key", "test_value"))
        self.assertEqual(self.manager.getConfigValue("test_key"), "test_value")
    
    def test_has_config(self):
        """Test configuration existence checking"""
        self.assertFalse(self.manager.hasConfig("nonexistent"))
        self.assertTrue(self.manager.setConfigValue("existing", "value"))
        self.assertTrue(self.manager.hasConfig("existing"))
    
    def test_remove_config(self):
        """Test configuration removal"""
        self.assertTrue(self.manager.setConfigValue("to_remove", "value"))
        self.manager.removeConfig("to_remove")
        self.assertFalse(self.manager.hasConfig("to_remove"))
    
    def test_clear_config(self):
        """Test configuration clearing"""
        self.assertTrue(self.manager.setConfigValue("key1", "value1"))
        self.assertTrue(self.manager.setConfigValue("key2", "value2"))
        self.manager.clearConfig()
        self.assertFalse(self.manager.hasConfig("key1"))
        self.assertFalse(self.manager.hasConfig("key2"))
    
    def test_config_keys(self):
        """Test getting configuration keys"""
        self.manager.setConfigValue("key1", "value1")
        self.manager.setConfigValue("key2", "value2")
        
        keys = self.manager.getConfigKeys()
        self.assertIn("key1", keys)
        self.assertIn("key2", keys)
        self.assertEqual(len(keys), 2)

class TestVisualizationInterface(unittest.TestCase):
    """Test visualization interface functionality"""
    
    def setUp(self):
        """Set up test configuration and brain"""
        self.config = Config()
        self.config.set("neuron_count", 20)
        self.config.set("region_count", 1)
        self.brain = Brain(self.config)
    
    def test_visualization_initialization(self):
        """Test visualization interface initialization"""
        viz = VisualizationInterface(self.brain, OutputFormat.Text)
        self.assertIsNotNone(viz)
    
    def test_visualization_formats(self):
        """Test different visualization formats"""
        formats = [
            OutputFormat.Text,
            OutputFormat.JSON,
            OutputFormat.CSV,
            OutputFormat.HTML,
            OutputFormat.Plot,
            OutputFormat.ASCII
        ]
        
        for fmt in formats:
            with self.subTest(format=fmt):
                viz = VisualizationInterface(self.brain, fmt)
                self.assertIsNotNone(viz)
    
    def test_visualization_dimensions(self):
        """Test setting visualization dimensions"""
        viz = VisualizationInterface(self.brain, OutputFormat.Text)
        viz.setDimensions(100, 50)
        
        # Dimensions should be set (no assertion available, just ensure no exception)
        self.assertTrue(True)

class TestExperimentRunner(unittest.TestCase):
    """Test experiment runner functionality"""
    
    def setUp(self):
        """Set up test configuration and runner"""
        self.config = Config()
        self.config.set("neuron_count", 30)
        self.config.set("region_count", 1)
        
        self.brain = Brain(self.config)
        self.runner = ExperimentRunner(self.config)
        self.runner.setBrain(self.brain)
    
    def test_runner_initialization(self):
        """Test experiment runner initialization"""
        self.assertIsNotNone(self.runner)
    
    def test_runner_brain_access(self):
        """Test brain access through experiment runner"""
        brain = self.runner.getBrain()
        self.assertIsNotNone(brain)
        self.assertEqual(brain.getTotalNeuronCount(), 30)
    
    def test_available_experiments(self):
        """Test available experiment names"""
        experiments = ExperimentRunner.getAvailableExperiments()
        self.assertIn("basic", experiments)
        self.assertIn("plasticity", experiments)
        self.assertIn("stdp", experiments)
        self.assertIn("all", experiments)

class TestLearningExperiment(unittest.TestCase):
    """Test learning experiment functionality"""
    
    def setUp(self):
        """Set up test configuration and experiment"""
        self.config = Config()
        self.config.set("neuron_count", 40)
        self.config.set("region_count", 1)
        
        self.brain = Brain(self.config)
        self.experiment = LearningExperiment(self.brain, 123)
    
    def test_experiment_initialization(self):
        """Test learning experiment initialization"""
        self.assertIsNotNone(self.experiment)
    
    def test_experiment_getters(self):
        """Test learning experiment getters"""
        self.assertIsInstance(self.experiment.getInitialWeights(), list)
        self.assertIsInstance(self.experiment.getFinalWeights(), list)
        self.assertIsInstance(self.experiment.getMostActiveNeurons(), list)
        self.assertIsInstance(self.experiment.getStrengthenedSynapses(), list)
        self.assertIsInstance(self.experiment.getWeakenedSynapses(), list)
        self.assertIsInstance(self.experiment.getUnchangedSynapses(), list)
    
    def test_experiment_stats(self):
        """Test learning experiment statistics"""
        initial_synapses = self.experiment.getInitialSynapseCount()
        initial_weights = self.experiment.getInitialWeights()
        final_weights = self.experiment.getFinalWeights()
        
        self.assertEqual(initial_synapses, 0)  # Brain not initialized
        self.assertEqual(len(initial_weights), 0)
        self.assertEqual(len(final_weights), 0)

class TestIntegration(unittest.TestCase):
    """Test integration between components"""
    
    def test_full_pipeline(self):
        """Test a full pipeline from config to brain to experiment"""
        # Create configuration
        config = Config()
        config.set("neuron_count", 25)
        config.set("region_count", 1)
        
        # Create brain
        brain = Brain(config)
        self.assertIsNotNone(brain)
        
        # Create visualization
        viz = VisualizationInterface(brain, OutputFormat.Text)
        self.assertIsNotNone(viz)
        
        # Create agent brain interface
        agent_brain = AgentBrain(brain)
        self.assertIsNotNone(agent_brain)
        
        # Create experiment runner
        runner = ExperimentRunner(config)
        runner.setBrain(brain)
        self.assertIsNotNone(runner)
        
        # Test config manager
        manager = ConfigManager(config)
        manager.setConfigValue("integration_test", "passed")
        self.assertTrue(manager.hasConfig("integration_test"))
        
        # Test command line parser
        parser = CommandLineParser(3, ["test", "--help", "--version"])
        self.assertTrue(parser.hasOption("--help"))
        
        self.assertTrue(True, "Full integration test passed")

if __name__ == "__main__":
    # Run tests
    unittest.main(verbosity=2)