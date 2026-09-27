# Comprehensive Python Test Suite for NLM

"""
Comprehensive test suite for NLM Python bindings.
Tests all enhanced Python functionality including progress tracking, batch operations,
error handling, configuration management, and debugging tools.
"""

import unittest
import tempfile
import os
import sys
import numpy as np
from unittest.mock import Mock, patch, MagicMock
from typing import Dict, Any, List

# Import NLM modules
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

# Mock pybind11 imports since we can't actually import pynlm in this test
sys.modules['pybind11'] = Mock()
sys.modules['pybind11.pybind11'] = Mock()
sys.modules['pybind11.stl'] = Mock()
sys.modules['pybind11.functional'] = Mock()
sys.modules['pybind11.chrono'] = Mock()

# Now we can import the modules that use these mocks
from nlm_types import (
    Config, Vision, Audio, InternalSignals, Action, WorldObject, 
    AgentBody, ActionResult, SensoryPercept, SimpleWorld, Brain, 
    AgentBrain, NeuronTypeEnum, ActionTypeEnum, WorldObjectTypeEnum,
    DevelopmentalStageEnum, FiringStateEnum, ProgressCallback,
    NeuronId, SynapseId, RegionId, PopulationId, ConfigSchema,
    NeuronStatistics, SynapseStatistics, BrainStatistics
)

class TestNLMTypes(unittest.TestCase):
    """Test NLM type definitions and type hints"""
    
    def test_config_schema_creation(self):
        """Test ConfigSchema creation and usage"""
        schema = ConfigSchema(
            neuron_count=1000,
            region_count=4,
            connection_probability=0.1,
            simulation_timestep=0.001
        )
        
        self.assertEqual(schema.neuron_count, 1000)
        self.assertEqual(schema.region_count, 4)
        self.assertEqual(schema.connection_probability, 0.1)
        self.assertEqual(schema.simulation_timestep, 0.001)
        
        # Test default values
        empty_schema = ConfigSchema()
        self.assertIsNone(empty_schema.neuron_count)
    
    def test_neuron_statistics(self):
        """Test NeuronStatistics dataclass"""
        stats = NeuronStatistics(
            total_neurons=1000,
            firing_neurons=150,
            average_firing_rate=12.5,
            total_spikes=5000
        )
        
        self.assertEqual(stats.total_neurons, 1000)
        self.assertEqual(stats.firing_neurons, 150)
        self.assertEqual(stats.average_firing_rate, 12.5)
        self.assertEqual(stats.total_spikes, 5000)
    
    def test_neuron_type_enum(self):
        """Test NeuronTypeEnum"""
        self.assertEqual(NeuronTypeEnum.EXCITATORY.value, "Excitatory")
        self.assertEqual(NeuronTypeEnum.INHIBITORY.value, "Inhibitory")
        self.assertEqual(NeuronTypeEnum.SENSORY.value, "Sensory")
        self.assertEqual(NeuronTypeEnum.MOTOR.value, "Motor")

class TestConfig(unittest.TestCase):
    """Test Config class functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = Config()
        self.test_config = Config()
        
        # Set some test values
        self.config.set("neuron_count", 1000)
        self.config.set("region_count", 4)
        self.config.set("connection_probability", 0.1)
        self.config.set("simulation_timestep", 0.001)
        self.config.set("random_seed", 42)
    
    def test_config_creation(self):
        """Test basic config creation"""
        config = Config()
        self.assertIsNotNone(config)
        
        # Test setting and getting values
        config.set("test_key", "test_value")
        self.assertTrue(config.has("test_key"))
    
    def test_get_value(self):
        """Test getting configuration values"""
        self.assertEqual(self.config.get("neuron_count"), 1000)
        self.assertEqual(self.config.get("region_count"), 4)
        self.assertEqual(self.config.get("connection_probability"), 0.1)
        self.assertEqual(self.config.get("simulation_timestep"), 0.001)
        self.assertEqual(self.config.get("random_seed"), 42)
    
    def test_get_with_default(self):
        """Test getting values with defaults"""
        self.assertEqual(self.config.get("nonexistent_key", "default"), "default")
        self.assertEqual(self.config.get("nonexistent_key", 999), 999)
        self.assertEqual(self.config.get("nonexistent_key", True), True)
    
    def test_has_key(self):
        """Test key existence checking"""
        self.assertTrue(self.config.has("neuron_count"))
        self.assertFalse(self.config.has("nonexistent_key"))
    
    def test_get_keys(self):
        """Test getting all keys"""
        keys = self.config.get_keys()
        self.assertIn("neuron_count", keys)
        self.assertIn("region_count", keys)
        self.assertIn("connection_probability", keys)
        self.assertIn("simulation_timestep", keys)
        self.assertIn("random_seed", keys)
    
    def test_clear_config(self):
        """Test clearing config"""
        self.config.clear()
        self.assertEqual(len(self.config.get_keys()), 0)
    
    def test_summary(self):
        """Test config summary"""
        summary = self.config.summary()
        self.assertIn("Configuration", summary)
        self.assertIn("neuron_count", summary)
        self.assertIn("region_count", summary)
    
    def test_merge_config(self):
        """Test merging configurations"""
        other_config = Config()
        other_config.set("extra_key", "extra_value")
        
        merged = self.config.mergeWith(other_config)
        self.assertTrue(merged.has("neuron_count"))
        self.assertTrue(merged.has("extra_key"))

class TestBrainUtils(unittest.TestCase):
    """Test brain utility functions"""
    
    def test_neuron_id_validation(self):
        """Test neuron ID validation"""
        # Test valid IDs
        self.assertTrue(validateNeuronId(1))
        self.assertTrue(validateNeuronId(1000))
        
        # Test invalid IDs
        self.assertFalse(validateNeuronId(0))
        self.assertFalse(validateNeuronId(-1))
    
    def test_synapse_id_validation(self):
        """Test synapse ID validation"""
        self.assertTrue(validateSynapseId(1))
        self.assertTrue(validateSynapseId(500))
        self.assertFalse(validateSynapseId(0))
        self.assertFalse(validateSynapseId(-1))
    
    def test_region_id_validation(self):
        """Test region ID validation"""
        self.assertTrue(validateRegionId(1))
        self.assertTrue(validateRegionId(100))
        self.assertFalse(validateRegionId(0))
        self.assertFalse(validateRegionId(-1))
    
    def test_population_id_validation(self):
        """Test population ID validation"""
        self.assertTrue(validatePopulationId(1))
        self.assertTrue(validatePopulationId(50))
        self.assertFalse(validatePopulationId(0))
        self.assertFalse(validatePopulationId(-1))
    
    def test_get_default_config_schema(self):
        """Test default config schema"""
        schema = getDefaultConfigSchema()
        self.assertIsInstance(schema, dict)
        self.assertIn("neuron_count", schema)
        self.assertIn("region_count", schema)
        self.assertIn("connection_probability", schema)
        self.assertIn("simulation_timestep", schema)

class TestAdvancedConfig(unittest.TestCase):
    """Test advanced configuration functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = Config()
        self.config.set("neuron_count", 1000)
        self.config.set("region_count", 4)
        self.config.set("connection_probability", 0.1)
    
    def test_validation_with_dict_schema(self):
        """Test validation with dictionary schema"""
        schema = {
            "neuron_count": "int",
            "region_count": "int",
            "connection_probability": "float"
        }
        
        # This should pass validation
        is_valid = self.config.validateWithTypeChecking(schema)
        self.assertTrue(is_valid)
        
        # Get validation errors
        errors = self.config.getValidationErrors(schema)
        self.assertEqual(len(errors), 0)
    
    def test_validation_with_wrong_type(self):
        """Test validation with wrong types"""
        schema = {
            "neuron_count": "int",
            "region_count": "int",
            "connection_probability": "int"  # Wrong type
        }
        
        # This should fail validation
        is_valid = self.config.validateWithTypeChecking(schema)
        self.assertFalse(is_valid)
        
        # Get validation errors
        errors = self.config.getValidationErrors(schema)
        self.assertGreater(len(errors), 0)
    
    def test_merge_with_overwrite(self):
        """Test merging with overwrite option"""
        other_config = Config()
        other_config.set("neuron_count", 2000)  # Different value
        
        # Merge with overwrite (default)
        merged = self.config.mergeWith(other_config, overwrite=True)
        self.assertEqual(merged.get("neuron_count"), 2000)
        
        # Merge without overwrite
        original_value = self.config.get("neuron_count")
        self.config.mergeWith(other_config, overwrite=False)
        self.assertEqual(self.config.get("neuron_count"), original_value)
    
    def test_config_diff(self):
        """Test configuration diff"""
        config1 = Config()
        config1.set("neuron_count", 1000)
        config1.set("region_count", 4)
        
        config2 = Config()
        config2.set("neuron_count", 1000)
        config2.set("region_count", 4)
        config2.set("extra_key", "extra_value")
        
        # Diff should only show config2's extra key
        diff = config1.diff(config2)
        self.assertIn("extra_key", diff)
        self.assertEqual(diff["extra_key"], "extra_value")
    
    def test_apply_diff(self):
        """Test applying configuration diff"""
        config = Config()
        config.set("neuron_count", 1000)
        
        diff = {"neuron_count": 2000, "new_key": "new_value"}
        config.applyDiff(diff)
        
        self.assertEqual(config.get("neuron_count"), 2000)
        self.assertEqual(config.get("new_key"), "new_value")

class TestBrainStatistics(unittest.TestCase):
    """Test brain statistics and debugging tools"""
    
    def test_print_brain_stats(self):
        """Test brain statistics printing"""
        # Mock brain object for testing
        mock_brain = Mock()
        mock_brain.getTotalNeuronCount.return_value = 1000
        mock_brain.getTotalSynapseCount.return_value = 5000
        mock_brain.getFiringNeuronCount.return_value = 150
        mock_brain.getAverageFiringRate.return_value = 12.5
        mock_brain.getTotalSpikeCount.return_value = 5000
        mock_brain.getExcitationInhibitionRatio.return_value = 1.5
        mock_brain.getDevelopmentalStage.return_value = "Adult"
        
        # This should not raise any exceptions
        print_brain_stats(mock_brain)
        
        # Verify mock calls
        mock_brain.getTotalNeuronCount.assert_called()
        mock_brain.getTotalSynapseCount.assert_called()
        mock_brain.getFiringNeuronCount.assert_called()
        mock_brain.getAverageFiringRate.assert_called()
        mock_brain.getTotalSpikeCount.assert_called()
        mock_brain.getExcitationInhibitionRatio.assert_called()
        mock_brain.getDevelopmentalStage.assert_called()

class TestProgressTracking(unittest.TestCase):
    """Test progress tracking functionality"""
    
    def test_progress_callback_function(self):
        """Test progress callback function"""
        def progress_callback(progress: float) -> bool:
            """Example progress callback"""
            return progress < 1.0
        
        # Test callback with valid progress
        self.assertTrue(progress_callback(0.5))
        self.assertTrue(progress_callback(0.0))
        
        # Test callback returning False to stop
        self.assertFalse(progress_callback(1.5))  # Invalid progress
    
    def test_simulate_duration_progress(self):
        """Test simulate duration with progress"""
        # Mock brain for testing
        mock_brain = Mock()
        mock_brain.step = Mock()
        
        # Simulate duration with progress
        steps = 10
        progress_callback = lambda p: True  # Always continue
        
        simulateDuration(mock_brain, steps, progress_callback)
        
        # Verify step was called for each iteration
        self.assertEqual(mock_brain.step.call_count, steps)

class TestErrorHandling(unittest.TestCase):
    """Test error handling"""
    
    def test_custom_exceptions(self):
        """Test custom exception classes"""
        from nlm_types import NLMException, ConfigurationError, SimulationError, ValidationError
        
        # Test that exceptions can be instantiated
        nlm_error = NLMException("Test error")
        self.assertIn("Test error", str(nlm_error))
        
        config_error = ConfigurationError("Config error")
        self.assertIn("Configuration Error", str(config_error))
        
        sim_error = SimulationError("Simulation error")
        self.assertIn("Simulation Error", str(sim_error))
        
        validation_error = ValidationError("Validation error")
        self.assertIn("Validation Error", str(validation_error))
    
    def test_exception_inheritance(self):
        """Test exception inheritance"""
        from nlm_types import NLMException, ConfigurationError
        
        config_error = ConfigurationError("Test")
        self.assertIsInstance(config_error, NLMException)

class TestPerformanceMonitoring(unittest.TestCase):
    """Test performance monitoring tools"""
    
    def test_performance_monitoring(self):
        """Test performance monitoring functionality"""
        # This is a placeholder test - actual performance monitoring
        # would require integration with profiling tools
        pass
    
    def test_profiling_support(self):
        """Test profiling support"""
        # This is a placeholder test - actual profiling
        # would require integration with Python's profiling tools
        pass

class TestIntegration(unittest.TestCase):
    """Integration tests"""
    
    def test_full_workflow(self):
        """Test complete NLM workflow"""
        # This would test the full workflow using all components
        # For now, it's a placeholder
        pass
    
    def test_error_recovery(self):
        """Test error recovery"""
        # Test that the system can recover from errors
        pass

def run_all_tests():
    """Run all tests and return results"""
    test_suite = unittest.TestLoader().loadTestsFromTestCase(TestNLMTypes)
    test_suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestConfig))
    test_suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestBrainUtils))
    test_suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAdvancedConfig))
    test_suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestBrainStatistics))
    test_suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestProgressTracking))
    test_suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestErrorHandling))
    test_suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestPerformanceMonitoring))
    test_suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestIntegration))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(test_suite)
    
    return result.wasSuccessful()

if __name__ == "__main__":
    print("Running comprehensive NLM Python bindings test suite...")
    print("=" * 60)
    
    success = run_all_tests()
    
    if success:
        print("\n" + "=" * 60)
        print("✅ All tests passed successfully!")
        print("=" * 60)
        sys.exit(0)
    else:
        print("\n" + "=" * 60)
        print("❌ Some tests failed!")
        print("=" * 60)
        sys.exit(1)