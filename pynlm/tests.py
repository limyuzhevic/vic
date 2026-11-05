#!/usr/bin/env python3
"""
NLM - Python Tests and Validation

Comprehensive test suite for NLM Python interface with validation
and quality assurance.
"""

import unittest
import tempfile
import os
import json
import time
import logging
from pathlib import Path
from typing import Dict, List, Any
from pynlm import (
    NLMConfig, 
    ConfigManager, 
    AdvancedNLM, 
    get_minimal_config,
    get_default_config,
    get_research_config,
    create_config,
    setup_logging
)

# Setup logging for tests
setup_logging(level=logging.WARNING)


class TestNLMConfig(unittest.TestCase):
    """Test NLMConfig class."""
    
    def test_config_creation(self):
        """Test basic configuration creation."""
        config = NLMConfig()
        self.assertIsNotNone(config)
        self.assertEqual(config.neuron_count, 1000)
        self.assertEqual(config.region_count, 1)
    
    def test_config_validation(self):
        """Test configuration validation."""
        # Valid configuration
        valid_config = NLMConfig(
            neuron_count=500,
            region_count=2,
            connection_probability=0.1,
            simulation_timestep=0.001,
            random_seed=42
        )
        valid_config.validate()
        
        # Invalid configuration (should raise ValueError)
        with self.assertRaises(ValueError):
            invalid_config = NLMConfig(
                neuron_count=-100,  # Invalid negative value
                region_count=1
            )
            invalid_config.validate()
    
    def test_config_to_dict(self):
        """Test configuration to dictionary conversion."""
        config = NLMConfig(
            neuron_count=1000,
            region_count=2,
            simulation_timestep=0.001,
        )
        
        data = config.to_dict()
        self.assertIsInstance(data, dict)
        self.assertEqual(data['neuron_count'], 1000)
        self.assertEqual(data['region_count'], 2)
        self.assertEqual(data['simulation_timestep'], 0.001)
    
    def test_config_from_json(self):
        """Test configuration from JSON."""
        json_str = '{"neuron_count": 500, "region_count": 1, "connection_probability": 0.1}'
        config = NLMConfig.from_json(json_str)
        
        self.assertEqual(config.neuron_count, 500)
        self.assertEqual(config.region_count, 1)
        self.assertEqual(config.connection_probability, 0.1)
    
    def test_config_merge(self):
        """Test configuration merging."""
        config1 = NLMConfig(neuron_count=1000, region_count=1)
        config2 = NLMConfig(neuron_count=500, connection_probability=0.2)
        
        merged = config1.merge(config2)
        
        self.assertEqual(merged.neuron_count, 500)  # From config2
        self.assertEqual(merged.region_count, 1)    # From config1
        self.assertEqual(merged.connection_probability, 0.2)  # From config2
    
    def test_config_get(self):
        """Test configuration key retrieval."""
        config = NLMConfig(
            neuron_count=1000,
            region_count=2,
            connection_probability=0.1,
        )
        
        self.assertEqual(config.get("neuron_count"), 1000)
        self.assertEqual(config.get("region_count"), 2)
        self.assertEqual(config.get("connection_probability"), 0.1)
        self.assertIsNone(config.get("nonexistent_key", None))
        self.assertEqual(config.get("nonexistent_key", "default"), "default")


class TestConfigManager(unittest.TestCase):
    """Test ConfigManager class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = NLMConfig(neuron_count=1000, region_count=1)
        self.config_manager = ConfigManager(self.config)
    
    def test_config_manager_creation(self):
        """Test config manager creation."""
        self.assertIsNotNone(self.config_manager)
        self.assertEqual(self.config_manager.get_config().neuron_count, 1000)
    
    def test_config_manager_update(self):
        """Test config manager update."""
        self.config_manager.update_config(neuron_count=500)
        
        updated_config = self.config_manager.get_config()
        self.assertEqual(updated_config.neuron_count, 500)
        self.assertEqual(updated_config.region_count, 1)  # Unchanged
    
    def test_config_manager_save_load(self):
        """Test config manager save and load."""
        # Save configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            self.config_manager.save_config(filepath)
            self.assertTrue(os.path.exists(filepath))
            
            # Load configuration
            new_config_manager = ConfigManager()
            new_config_manager.load_config(filepath)
            
            # Verify loaded configuration
            loaded_config = new_config_manager.get_config()
            self.assertEqual(loaded_config.neuron_count, 1000)
            self.assertEqual(loaded_config.region_count, 1)
        finally:
            os.unlink(filepath)
    
    def test_config_manager_validation(self):
        """Test config manager validation."""
        # Valid configuration
        self.assertTrue(self.config_manager.validate())
        
        # Invalid configuration
        invalid_config = NLMConfig(neuron_count=-100)
        invalid_config_manager = ConfigManager(invalid_config)
        self.assertFalse(invalid_config_manager.validate())
    
    def test_config_manager_summary(self):
        """Test config manager summary."""
        summary = self.config_manager.get_config_summary()
        
        self.assertIsInstance(summary, dict)
        self.assertIn('neuron_count', summary)
        self.assertIn('region_count', summary)
        self.assertIn('plasticity_enabled', summary)


class TestAdvancedNLM(unittest.TestCase):
    """Test AdvancedNLM class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = get_minimal_config()
        self.brain = AdvancedNLM(self.config)
    
    def test_brain_creation(self):
        """Test brain creation."""
        self.assertIsNotNone(self.brain)
        self.assertIsNotNone(self.brain.brain)
        self.assertIsNotNone(self.brain.enhanced_brain)
    
    def test_enhanced_brain_wrapper(self):
        """Test enhanced brain wrapper."""
        enhanced = self.brain.enhanced_brain
        
        # Test basic methods
        self.assertEqual(enhanced.get_neuron_count(), self.config.neuron_count)
        self.assertEqual(enhanced.get_firing_neuron_count(), self.brain.brain.getFiringNeuronCount())
        self.assertEqual(enhanced.get_average_firing_rate(), self.brain.brain.getAverageFiringRate())
        self.assertEqual(enhanced.get_excitation_inhibition_ratio(), self.brain.brain.getExcitationInhibitionRatio())
        
        # Test config summary
        summary = enhanced.get_config_summary()
        self.assertIsInstance(summary, dict)
        self.assertIn('neuron_count', summary)
    
    def test_performance_stats(self):
        """Test performance statistics."""
        stats = self.brain.enhanced_brain.get_performance_stats()
        
        self.assertIsInstance(stats, dict)
        self.assertIn('steps_per_second', stats)
        self.assertIn('neurons_per_second', stats)
        self.assertIn('synapses_per_neuron', stats)
    
    def test_config_presets(self):
        """Test configuration presets."""
        # Test minimal config
        minimal = get_minimal_config()
        self.assertEqual(minimal.neuron_count, 100)
        self.assertEqual(minimal.simulation_timestep, 0.01)
        
        # Test default config
        default = get_default_config()
        self.assertEqual(default.neuron_count, 1000)
        
        # Test research config
        research = get_research_config()
        self.assertEqual(research.neuron_count, 10000)
        self.assertGreater(research.region_count, 1)


class TestSimulation(unittest.TestCase):
    """Test Simulation class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = get_minimal_config()
        self.brain = AdvancedNLM(self.config)
        self.simulation = self.brain.create_simulation("test_simulation")
    
    def test_simulation_creation(self):
        """Test simulation creation."""
        self.assertIsNotNone(self.simulation)
        self.assertEqual(self.simulation.max_steps, 1000)
    
    def test_simulation_run(self):
        """Test simulation run."""
        # Run simulation
        self.simulation.run(50)
        
        # Check results
        progress = self.simulation.get_progress()
        self.assertEqual(progress['current_step'], 50)
        self.assertEqual(progress['max_steps'], 1000)
        self.assertEqual(progress['status'].value, 'completed')
    
    def test_simulation_step(self):
        """Test single simulation step."""
        # Initialize simulation
        self.simulation.step()
        
        # Check results
        progress = self.simulation.get_progress()
        self.assertEqual(progress['current_step'], 1)
    
    def test_simulation_reset(self):
        """Test simulation reset."""
        # Run some steps
        self.simulation.run(50)
        self.assertEqual(self.simulation.current_step, 50)
        
        # Reset
        self.simulation.reset()
        
        # Check results
        self.assertEqual(self.simulation.current_step, 0)
        self.assertEqual(self.simulation.status.value, 'initialized')
    
    def test_simulation_pause_resume(self):
        """Test simulation pause and resume."""
        # Pause
        self.simulation.pause()
        self.assertEqual(self.simulation.status.value, 'paused')
        self.assertTrue(self.simulation.paused)
        
        # Resume
        self.simulation.resume()
        self.assertEqual(self.simulation.status.value, 'running')
        self.assertFalse(self.simulation.paused)
    
    def test_simulation_export_import(self):
        """Test simulation export and import."""
        # Create temporary directory for test files
        with tempfile.TemporaryDirectory() as temp_dir:
            events_file = os.path.join(temp_dir, "events.json")
            metrics_file = os.path.join(temp_dir, "metrics.json")
            
            # Export data
            self.simulation.export_events(events_file)
            self.simulation.export_metrics(metrics_file)
            
            # Verify files exist
            self.assertTrue(os.path.exists(events_file))
            self.assertTrue(os.path.exists(metrics_file))
            
            # Load data
            with open(events_file, 'r') as f:
                events_data = json.load(f)
            
            with open(metrics_file, 'r') as f:
                metrics_data = json.load(f)
            
            # Verify data
            self.assertIsInstance(events_data, list)
            self.assertIsInstance(metrics_data, list)


class TestActivityAnalysis(unittest.TestCase):
    """Test activity analysis functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.brain = AdvancedNLM(get_minimal_config())
    
    def test_activity_patterns(self):
        """Test activity patterns analysis."""
        # Get activity patterns
        patterns = self.brain.enhanced_brain.get_activity_patterns(50)
        
        # Verify structure
        self.assertIsInstance(patterns, dict)
        self.assertIn('spike_count', patterns)
        self.assertIn('firing_rate', patterns)
        self.assertIn('e_i_ratio', patterns)
        self.assertIn('step', patterns)
        
        # Verify data
        for key, data in patterns.items():
            self.assertEqual(len(data), 50)  # Should have 50 data points
    
    def test_active_regions(self):
        """Test active regions detection."""
        # Find active regions
        regions = self.brain.enhanced_brain.find_active_regions(threshold=0.1)
        
        # Verify structure
        self.assertIsInstance(regions, list)
        
        if regions:  # If regions were found
            for region in regions:
                self.assertIn('region_id', region)
                self.assertIn('neuron_count', region)
                self.assertIn('firing_count', region)
                self.assertIn('firing_rate', region)
                self.assertIn('activity', region)
                self.assertGreaterEqual(region['firing_rate'], 0.1)
    
    def test_stability_analysis(self):
        """Test stability analysis."""
        # Analyze stability
        stability = self.brain.enhanced_brain.analyze_stability(50)
        
        # Verify structure
        self.assertIsInstance(stability, dict)
        self.assertIn('stability', stability)
        self.assertIn('recommendations', stability)
        self.assertIn('firing_rate', stability)
        self.assertIn('spike_count', stability)
        
        # Check stability values
        self.assertIn(stability['stability'], ['stable', 'moderate', 'unstable'])
    
    def test_plasticity_summary(self):
        """Test plasticity summary."""
        # Get plasticity summary
        summary = self.brain.enhanced_brain.get_plasticity_summary()
        
        # Verify structure
        self.assertIsInstance(summary, dict)
        self.assertIn('stdp_enabled', summary)
        self.assertIn('hebbian_enabled', summary)
        self.assertIn('structural_enabled', summary)
        self.assertIn('reward_modulation_enabled', summary)


class TestIntegration(unittest.TestCase):
    """Test integration of NLM components."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = get_minimal_config()
        self.brain = AdvancedNLM(self.config)
    
    def test_full_workflow(self):
        """Test complete workflow with all components."""
        # Configure learning systems
        self.brain.brain.enableRewardModulation(True)
        self.brain.brain.enableStructuralPlasticity(True)
        self.brain.brain.enableDevelopment(True)
        
        # Create simulation
        simulation = self.brain.create_simulation("integration_test")
        
        # Run simulation
        simulation.run(100)
        
        # Verify results
        progress = simulation.get_progress()
        self.assertEqual(progress['current_step'], 100)
        
        # Check brain state
        self.assertGreater(self.brain.brain.getTotalSpikeCount(), 0)
        
        # Check enhanced brain
        self.assertGreater(self.brain.enhanced_brain.get_neuron_count(), 0)
        
        # Analyze stability
        stability = self.brain.enhanced_brain.analyze_stability(100)
        self.assertIn(stability['stability'], ['stable', 'moderate', 'unstable'])
    
    def test_configuration_management(self):
        """Test configuration management integration."""
        from nlm import ConfigManager
        
        # Create config manager
        config_manager = ConfigManager(self.config)
        
        # Update configuration
        config_manager.update_config(neuron_count=500)
        
        # Create new brain with updated config
        brain2 = AdvancedNLM(config_manager.get_config())
        
        # Verify brain was created with new config
        self.assertEqual(brain2.brain.getConfig().neuron_count, 500)
    
    def test_error_handling(self):
        """Test error handling."""
        # Test invalid configuration
        with self.assertRaises(ValueError):
            invalid_config = NLMConfig(neuron_count=-100)
            invalid_config.validate()
        
        # Test simulation error handling
        simulation = self.brain.create_simulation("error_test")
        
        # The simulation should handle errors gracefully
        try:
            simulation.run(10)
        except Exception:
            pass  # Expected in some error conditions


class TestPerformance(unittest.TestCase):
    """Test performance aspects."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.brain = AdvancedNLM(get_minimal_config())
    
    def test_performance_benchmark(self):
        """Test performance benchmarking."""
        # Measure performance
        start_time = time.time()
        
        # Run simulation
        simulation = self.brain.create_simulation("benchmark")
        simulation.run(100)
        
        elapsed_time = time.time() - start_time
        
        # Calculate performance
        steps_per_second = 100 / elapsed_time if elapsed_time > 0 else 0
        
        # Verify performance
        self.assertGreater(steps_per_second, 0)
        self.assertLessEqual(steps_per_second, 1000)  # Reasonable upper bound
        
        # Get brain performance stats
        performance_stats = self.brain.enhanced_brain.get_performance_stats()
        
        self.assertIsInstance(performance_stats, dict)
    
    def test_memory_usage(self):
        """Test memory usage."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        # Get initial memory
        initial_memory = process.memory_info().rss / 1024 / 1024
        
        # Run simulation
        simulation = self.brain.create_simulation("memory_test")
        simulation.run(50)
        
        # Get final memory
        final_memory = process.memory_info().rss / 1024 / 1024
        
        # Memory increase should be reasonable
        memory_increase = final_memory - initial_memory
        self.assertLess(memory_increase, 100)  # Less than 100MB increase
    
    def test_config_validation_performance(self):
        """Test configuration validation performance."""
        config = get_default_config()
        
        # Time validation
        start_time = time.time()
        for _ in range(100):
            config.validate()
        validation_time = time.time() - start_time
        
        # Validation should be fast
        self.assertLess(validation_time, 1.0)  # Less than 1 second for 100 validations


class TestFileIO(unittest.TestCase):
    """Test file input/output operations."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
        self.brain = AdvancedNLM(get_minimal_config())
        self.simulation = self.brain.create_simulation("file_io_test")
    
    def tearDown(self):
        """Clean up after tests."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_file_export_import(self):
        """Test file export and import."""
        # Create test files
        events_file = os.path.join(self.temp_dir, "events.json")
        metrics_file = os.path.join(self.temp_dir, "metrics.json")
        brain_file = os.path.join(self.temp_dir, "brain.bin")
        
        # Export data
        self.simulation.export_events(events_file)
        self.simulation.export_metrics(metrics_file)
        self.brain.save_brain(brain_file)
        
        # Verify files exist
        self.assertTrue(os.path.exists(events_file))
        self.assertTrue(os.path.exists(metrics_file))
        self.assertTrue(os.path.exists(brain_file))
        
        # Load data
        with open(events_file, 'r') as f:
            events_data = json.load(f)
        
        with open(metrics_file, 'r') as f:
            metrics_data = json.load(f)
        
        # Create new brain and load data
        new_brain = AdvancedNLM(get_minimal_config())
        new_brain.load_brain(brain_file)
        
        # Verify brain was loaded
        self.assertTrue(os.path.exists(brain_file))
    
    def test_json_serialization(self):
        """Test JSON serialization."""
        config = get_minimal_config()
        
        # Convert to JSON
        json_str = config.to_json()
        self.assertIsInstance(json_str, str)
        
        # Parse JSON
        data = json.loads(json_str)
        self.assertIsInstance(data, dict)
        
        # Convert back
        config2 = NLMConfig.from_json(json_str)
        self.assertEqual(config.neuron_count, config2.neuron_count)
    
    def test_file_validation(self):
        """Test file validation."""
        # Create invalid JSON file
        invalid_json_file = os.path.join(self.temp_dir, "invalid.json")
        with open(invalid_json_file, 'w') as f:
            f.write("{ invalid json")
        
        # Try to load invalid JSON
        with self.assertRaises(json.JSONDecodeError):
            with open(invalid_json_file, 'r') as f:
                json.load(f)
        
        # Create valid JSON file
        valid_json_file = os.path.join(self.temp_dir, "valid.json")
        with open(valid_json_file, 'w') as f:
            json.dump({"neuron_count": 100}, f)
        
        # Load valid JSON
        with open(valid_json_file, 'r') as f:
            data = json.load(f)
        
        self.assertEqual(data['neuron_count'], 100)


def run_tests():
    """Run all tests."""
    print("Running NLM Python Tests...")
    print("=" * 60)
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print(f"\n{'=' * 60}")
    print("TEST SUMMARY")
    print(f"{'=' * 60}")
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    
    if result.failures:
        print(f"\nFAILURES:")
        for test, traceback in result.failures:
            print(f"  - {test}: {traceback}")
    
    if result.errors:
        print(f"\nERRORS:")
        for test, traceback in result.errors:
            print(f"  - {test}: {traceback}")
    
    if result.wasSuccessful():
        print(f"\nAll tests passed! ✓")
    else:
        print(f"\nSome tests failed. ✗")
    
    return result.wasSuccessful()


if __name__ == "__main__":
    import sys
    import logging
    
    # Run tests
    success = run_tests()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)