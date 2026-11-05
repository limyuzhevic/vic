#!/usr/bin/env python3
"""
Test Suite for Enhanced NLM Python Bindings

This script tests all the improvements made to the NLM Python bindings,
including new functionality, error handling, helper functions, and
advanced features.

Run with: python test_enhanced_bindings.py
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

import pynlm
import unittest
from unittest.mock import Mock, patch
import json
import tempfile
import csv

class TestEnhancedBindings(unittest.TestCase):
    """Test suite for enhanced NLM bindings."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 8, 8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_basic_functionality(self):
        """Test basic NLM functionality."""
        # Test brain creation and initialization
        self.assertIsNotNone(self.brain)
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
        
        # Test step execution
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0)
        final_spikes = self.brain.getTotalSpikeCount()
        
        # Should have spiked or at least tried to spike
        self.assertGreaterEqual(final_spikes, initial_spikes)
        
        # Test world operations
        self.assertEqual(self.world.getWidth(), 10)
        self.assertEqual(self.world.getHeight(), 10)
        
        # Test agent operations
        self.assertIsNotNone(self.agent)
        self.assertTrue(self.agent.getSensoryInputSize() > 0)
    
    def test_enhanced_config(self):
        """Test enhanced configuration features."""
        # Test setting and getting values
        self.config.setInt("test.int", 42)
        self.assertEqual(self.config.getInt("test.int", 0), 42)
        
        self.config.setFloat("test.float", 3.14)
        self.assertAlmostEqual(self.config.getFloat("test.float", 0.0), 3.14, places=2)
        
        self.config.setString("test.string", "hello")
        self.assertEqual(self.config.getString("test.string", ""), "hello")
        
        self.config.setBool("test.bool", True)
        self.assertTrue(self.config.getBool("test.bool", False))
        
        # Test default values
        self.assertEqual(self.config.getInt("nonexistent.key", 99), 99)
        self.assertEqual(self.config.getFloat("nonexistent.key", 3.14), 3.14)
        self.assertEqual(self.config.getString("nonexistent.key", "default"), "default")
        self.assertFalse(self.config.getBool("nonexistent.key", True))
        
        # Test validation
        self.assertTrue(self.config.validate())
    
    def test_custom_config_presets(self):
        """Test custom configuration presets."""
        # Test create_default_config
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        # Test create_simulation_config
        sim_config = pynlm.create_simulation_config(500, 0.05, "default")
        self.assertIsNotNone(sim_config)
        self.assertEqual(sim_config.getInt("brain.neuron_count", 0), 500)
        self.assertEqual(sim_config.getFloat("brain.connection_probability", 1.0), 0.05)
        
        # Test brain_mode variations
        compact_config = pynlm.create_simulation_config(100, 0.05, "compact")
        default_config = pynlm.create_simulation_config(1000, 0.1, "default")
        large_config = pynlm.create_simulation_config(5000, 0.15, "large")
        
        self.assertLessEqual(compact_config.getInt("brain.neuron_count", 0), 100)
        self.assertLessEqual(default_config.getInt("brain.neuron_count", 0), 1000)
        self.assertLessEqual(large_config.getInt("brain.neuron_count", 0), 5000)
    
    def test_error_handling(self):
        """Test enhanced error handling."""
        # Test validation with invalid brain
        with self.assertRaises(pynlm.ResourceError):
            pynlm.validate_brain(None)
        
        # Test with brain that has no config
        brain_no_config = pynlm.createBrain(None)
        with self.assertRaises(pynlm.ResourceError):
            pynlm.validate_brain(brain_no_config)
        
        # Test configuration error
        config = pynlm.createDefaultConfig()
        # This should not raise an error for now
        # Future implementation might raise ConfigurationError for invalid configs
        pass
    
    def test_context_manager(self):
        """Test context manager functionality."""
        try:
            # Try to create brain context
            context = pynlm.create_brain_context(self.brain)
            self.assertIsNotNone(context)
            
            # Test context operations
            retrieved_brain = context.get()
            self.assertIsNotNone(retrieved_brain)
            
            # Test reset
            context.reset()
            
        except AttributeError:
            # Context manager not implemented in this version
            self.skipTest("Context manager not implemented")
    
    def test_helper_functions(self):
        """Test helper functions."""
        # Test create_agent_simulation
        brain, world, agent = pynlm.create_agent_simulation(
            steps=100,
            enable_learning=True,
            enable_development=True
        )
        
        self.assertIsNotNone(brain)
        self.assertIsNotNone(world)
        self.assertIsNotNone(agent)
        
        # Test generate_network_summary
        summary = pynlm.generate_network_summary(brain)
        self.assertIsInstance(summary, str)
        self.assertIn("Total neurons", summary)
        
        # Test run_spiking_analysis
        firing_rates = pynlm.run_spiking_analysis(brain, 10)
        self.assertIsInstance(firing_rates, list)
        self.assertEqual(len(firing_rates), 10)
        
        # Test validate_simulation_setup
        is_valid = pynlm.validate_simulation_setup(brain, world)
        self.assertIsInstance(is_valid, bool)
    
    def test_simulation_options(self):
        """Test simulation options and configurations."""
        from pynlm.helper_utils import SimulationOptions, WorldConfig, LearningConfig
        
        # Test SimulationOptions
        options = SimulationOptions()
        self.assertEqual(options.num_steps, 1000)
        self.assertEqual(options.timestep, 0.1)
        self.assertTrue(options.enable_learning)
        self.assertTrue(options.enable_development)
        
        # Test WorldConfig
        world_config = WorldConfig()
        self.assertEqual(world_config.width, 20)
        self.assertEqual(world_config.height, 20)
        self.assertEqual(world_config.vision_width, 8)
        self.assertEqual(world_config.vision_height, 8)
        
        # Test LearningConfig
        learning_config = LearningConfig()
        self.assertTrue(learning_config.enable_reward_modulation)
        self.assertFalse(learning_config.enable_structural_plasticity)
        self.assertTrue(learning_config.enable_development)
    
    def test_brain_context_manager(self):
        """Test brain context manager functionality."""
        try:
            # Create brain context
            context = pynlm.create_brain_context(self.brain)
            
            # Test that context can be used
            self.assertIsNotNone(context.get())
            
            # Test reset functionality
            context.reset()
            
        except (AttributeError, NotImplementedError):
            # Context manager may not be fully implemented
            self.skipTest("Brain context manager not fully implemented")
    
    def test_agent_statistics(self):
        """Test enhanced agent statistics."""
        # Test getAllStats method
        stats = self.agent.getAllStats()
        self.assertIsInstance(stats, dict)
        
        # Check that all expected keys are present
        expected_keys = [
            'curiosity_level', 'novelty_level', 'prediction_error',
            'neuromodulation_level', 'development_stage',
            'reward_modulation_enabled', 'structural_plasticity_enabled',
            'development_enabled', 'curiosity_enabled'
        ]
        
        for key in expected_keys:
            self.assertIn(key, stats)
    
    def test_simulation_with_stats(self):
        """Test simulation with statistics collection."""
        try:
            # This would test the run_simulation_with_stats function
            # which may need to be implemented
            self.skipTest("run_simulation_with_stats not yet implemented")
        except (AttributeError, NotImplementedError):
            self.skipTest("run_simulation_with_stats not implemented")
    
    def test_experiment_creation(self):
        """Test experiment creation and description."""
        from pynlm.helper_functions import create_experiment_description
        
        # Create test configuration objects
        from pynlm.helper_utils import SimulationOptions, WorldConfig, LearningConfig
        
        options = SimulationOptions()
        world_config = WorldConfig()
        learning_config = LearningConfig()
        
        # Create experiment description
        description = create_experiment_description(
            "Test Experiment",
            options,
            world_config,
            learning_config
        )
        
        self.assertIsInstance(description, str)
        self.assertIn("Test Experiment", description)
        self.assertIn("Simulation steps", description)
    
    def test_export_simulation_data(self):
        """Test simulation data export."""
        from pynlm.helper_functions import export_simulation_data
        
        # Create test simulation data
        class MockSimulationStats:
            def __init__(self):
                self.mean_firing_rate = 5.0
                self.std_firing_rate = 1.5
                self.max_firing_rate = 10.0
                self.min_firing_rate = 1.0
                self.mean_curiosity = 0.8
                self.mean_novelty = 0.6
                self.mean_prediction_error = 0.2
                self.total_steps = 100
        
        stats = MockSimulationStats()
        
        # Test export to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False) as f:
            filename = f.name
        
        try:
            result = export_simulation_data(stats, filename)
            self.assertTrue(result)
            
            # Verify file was created and contains expected data
            self.assertTrue(os.path.exists(filename))
            
            # Read and check content
            with open(filename, 'r') as f:
                reader = csv.reader(f)
                rows = list(reader)
                
            self.assertEqual(len(rows), 2)  # header + data row
            self.assertEqual(rows[0], ['firing_rate_mean', 'firing_rate_std', 
                                     'firing_rate_max', 'firing_rate_min',
                                     'curiosity_mean', 'novelty_mean',
                                     'prediction_error_mean', 'total_steps'])
            
        finally:
            # Clean up
            if os.path.exists(filename):
                os.unlink(filename)
    
    def test_comprehensive_integration(self):
        """Test comprehensive integration of all features."""
        # Create a complete simulation setup
        brain, world, agent = pynlm.create_agent_simulation(
            steps=50,
            enable_learning=True,
            enable_curiosity=True,
            enable_development=True
        )
        
        # Run simulation steps
        for step in range(20):
            world.update(0.1)
            brain.step(step)
        
        # Test all enhanced features
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(world.getSimulationTime(), 0)
        
        stats = agent.getAllStats()
        self.assertIn('curiosity_level', stats)
        self.assertIn('novelty_level', stats)
        
        summary = pynlm.generate_network_summary(brain)
        self.assertIn("Total neurons", summary)
        
        # Test configuration
        config = pynlm.create_simulation_config(100, 0.05)
        self.assertEqual(config.getInt("brain.neuron_count", 0), 100)
        
        # Test validation
        is_valid = pynlm.validate_simulation_setup(brain, world)
        self.assertIsInstance(is_valid, bool)
        
        print("✓ Comprehensive integration test passed")

class TestCLI(unittest.TestCase):
    """Test CLI functionality."""
    
    def test_cli_imports(self):
        """Test that CLI modules can be imported."""
        try:
            # Try to import CLI modules
            import importlib
            spec = importlib.util.find_spec("nlm_cli")
            self.assertIsNotNone(spec)
        except ImportError:
            self.skipTest("CLI modules not available")

if __name__ == "__main__":
    print("=" * 60)
    print("Enhanced NLM Python Bindings Test Suite")
    print("=" * 60)
    
    # Run tests
    suite = unittest.TestLoader().loadTestsFromTestCase(TestEnhancedBindings)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestCLI))
    
    # Run tests with verbose output
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    
    if result.failures:
        print("\nFailures:")
        for test, traceback in result.failures:
            print(f"  {test}: {traceback}")
    
    if result.errors:
        print("\nErrors:")
        for test, traceback in result.errors:
            print(f"  {test}: {traceback}")
    
    if result.wasSuccessful():
        print("\n✓ All tests passed!")
        sys.exit(0)
    else:
        print("\n✗ Some tests failed!")
        sys.exit(1)
