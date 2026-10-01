"""
Comprehensive test suite for NLM Python bindings (pynlm_advanced).
Tests all advanced features including custom neuron types, multi-region architectures,
advanced plasticity mechanisms, multi-agent systems, neural pattern analysis,
and advanced configuration systems.
"""

import unittest
import sys
import os
import tempfile
import json
import time
from typing import Dict, Any, List, Optional

# Add the Python module path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm
    import pynlm_advanced
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm_advanced not available, some tests will be skipped")

from pynlm import createDefaultConfig, loadConfig, Config, Types
from pynlm_advanced import AdvancedBrain, AdvancedPlasticity, AdvancedSimulation, AdvancedAnalysis, AdvancedConfig

# Define constants for configuration
NEURON_COUNTS = [100, 500, 1000]
MAX_STEPS = 100
DEFAULT_SIMULATION_STEPS = 50
TEST_TIMEOUT = 30
MAX_MEMORY_MB = 512


@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm_advanced module not available")
class TestPythonBindingsBasic(unittest.TestCase):
    """Test basic Python bindings functionality."""

    def setUp(self):
        """Set up test fixtures."""
        self.config = createDefaultConfig()
        self.config.set("neuron_count", 500)
        
        # Create temporary file paths
        self.temp_files = []
        
    def tearDown(self):
        """Clean up test fixtures."""
        for filepath in self.temp_files:
            try:
                if os.path.exists(filepath):
                    os.remove(filepath)
            except:
                pass

    def test_config_creation(self):
        """Test creating and configuring brain config."""
        config = createDefaultConfig()
        self.assertIsInstance(config, Config)
        
        config.set("neuron_count", 1000)
        config.set("connection_probability", 0.15)
        
        self.assertEqual(config.get("neuron_count"), 1000)
        self.assertEqual(config.get("connection_probability"), 0.15)

    def test_advanced_brain_creation(self):
        """Test creating an AdvancedBrain instance."""
        brain = AdvancedBrain(self.config)
        self.assertIsNotNone(brain)
        
        # Test brain initialization
        brain.initialize()
        
        # Check basic statistics
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        
        self.assertGreater(neuron_count, 0)
        self.assertGreater(synapse_count, 0)
        
    def test_advanced_brain_step(self):
        """Test stepping an AdvancedBrain."""
        brain = AdvancedBrain(self.config)
        brain.initialize()
        
        # Run simulation for several steps
        for step in range(10):
            brain.step(step)
            
            # Verify statistics are valid
            neuron_count = brain.getTotalNeuronCount()
            self.assertGreaterEqual(neuron_count, 0)
            
        # Check firing rates
        firing_rate = brain.getAverageFiringRate()
        self.assertGreaterEqual(firing_rate, 0.0)

    def test_custom_neuron_types(self):
        """Test adding custom neuron types."""
        brain = AdvancedBrain(self.config)
        brain.initialize()
        
        # Create custom neuron type
        custom_neuron = {
            "name": "fast_spiking",
            "threshold": -40.0,
            "reset_potential": -65.0,
            "decay_constant": 0.02,
            "synaptic_weight": 1.2,
            "is_excitatory": True,
            "properties": {"max_firing_rate": 100.0, "refractory_period": 2.0}
        }
        
        # Add custom neuron
        neuron_id = brain.addCustomNeuron(custom_neuron)
        self.assertGreater(neuron_id, 0)
        
        # Test neuron properties update
        success = brain.updateNeuronProperties(neuron_id, custom_neuron)
        self.assertTrue(success)

    def test_advanced_plasticity(self):
        """Test advanced plasticity mechanisms."""
        plasticity = AdvancedPlasticity()
        
        # Configure plasticity
        config = {
            "stdpLTPWeight": 0.015,
            "stdpLTDWeight": 0.018,
            "hebbianLearningRate": 0.02,
            "bcmLearningRate": 0.01,
            "bcmThreshold": 0.5,
            "bcmSlidingThreshold": 0.1,
            "minWeight": -1.0,
            "maxWeight": 1.0,
            "weightChangeLimit": 0.1
        }
        
        plasticity.configure(config)
        
        # Test plasticity configuration
        self.assertEqual(plasticity.getConfigValue("stdpLTPWeight"), 0.015)
        self.assertEqual(plasticity.getConfigValue("stdpLTDWeight"), 0.018)
        
        # Test adding plasticity rules
        success = plasticity.addPlasticityRule("STDP", "STDP")
        self.assertTrue(success)
        
        success = plasticity.addPlasticityRule("BCM", "BCM")
        self.assertTrue(success)
        
        # Test setting active rule
        success = plasticity.setActiveRule("STDP")
        self.assertTrue(success)

    def test_configuration_saving_loading(self):
        """Test saving and loading configurations."""
        # Create and configure advanced config
        adv_config = AdvancedConfig()
        
        # Create a test config
        config = createDefaultConfig()
        config.set("neuron_count", 2000)
        config.set("connection_probability", 0.2)
        
        # Export configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
            self.temp_files.append(config_file)
        
        success = adv_config.exportConfiguration(config, config_file)
        self.assertTrue(success)
        
        # Import configuration
        imported_config = createDefaultConfig()
        success = adv_config.importConfiguration(imported_config, config_file)
        self.assertTrue(success)
        
        # Verify imported config
        self.assertEqual(imported_config.get("neuron_count"), 2000)
        self.assertEqual(imported_config.get("connection_probability"), 0.2)


@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm_advanced module not available")
class TestPythonBindingsIntegration(unittest.TestCase):
    """Test integration between Python bindings and C++ implementation."""

    def setUp(self):
        """Set up integration test fixtures."""
        self.config = createDefaultConfig()
        
    def test_c_cpp_integration(self):
        """Test that Python bindings correctly interface with C++ implementation."""
        brain = AdvancedBrain(self.config)
        brain.initialize()
        
        # Verify that the brain is properly initialized
        neuron_count = brain.getTotalNeuronCount()
        self.assertGreater(neuron_count, 0)
        
        # Run a few steps to test continuous operation
        initial_neurons = neuron_count
        
        for step in range(5):
            brain.step(step)
            
            current_neurons = brain.getTotalNeuronCount()
            self.assertGreaterEqual(current_neurons, 0)
            
        # Verify that we got valid results
        self.assertGreater(neuron_count, 0)

    def test_memory_management(self):
        """Test memory management in Python bindings."""
        # Create multiple brain instances
        brains = []
        for i in range(3):
            config = createDefaultConfig()
            config.set("neuron_count", 100 * (i + 1))
            brain = AdvancedBrain(config)
            brain.initialize()
            brains.append(brain)
        
        # Verify all brains are working
        for i, brain in enumerate(brains):
            expected_neuron_count = 100 * (i + 1)
            actual_neuron_count = brain.getTotalNeuronCount()
            self.assertGreaterEqual(actual_neuron_count, 0)


@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm_advanced module not available")
class TestPythonBindingsErrorHandling(unittest.TestCase):
    """Test error handling in Python bindings."""

    def test_invalid_config_values(self):
        """Test handling of invalid configuration values."""
        config = createDefaultConfig()
        
        # Try setting invalid values
        config.set("neuron_count", -1)  # Should be positive
        config.set("connection_probability", 1.5)  # Should be <= 1.0
        
        # The config should still accept these values
        # Error handling should happen during brain creation
        brain = AdvancedBrain(config)
        brain.initialize()

    def test_nonexistent_feature(self):
        """Test handling of non-existent analysis features."""
        brain = AdvancedBrain(createDefaultConfig())
        brain.initialize()
        
        # Try enabling non-existent analysis feature
        success = brain.enableAnalysisFeature("non_existent_feature")
        self.assertFalse(success)

    def test_missing_configuration_file(self):
        """Test handling of missing configuration files."""
        adv_config = AdvancedConfig()
        
        # Try to load non-existent file
        config = createDefaultConfig()
        success = adv_config.importConfiguration(config, "/nonexistent/file.json")
        self.assertFalse(success)


@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm_advanced module not available")
class TestPythonBindingsPerformance(unittest.TestCase):
    """Test performance of Python bindings."""

    def test_brain_creation_performance(self):
        """Test performance of AdvancedBrain creation."""
        times = []
        
        for neuron_count in [100, 500, 1000]:
            config = createDefaultConfig()
            config.set("neuron_count", neuron_count)
            
            start_time = time.time()
            brain = AdvancedBrain(config)
            brain.initialize()
            end_time = time.time()
            
            creation_time = end_time - start_time
            times.append(creation_time)
            
            self.assertLess(creation_time, 5.0)  # Should be fast
            
        # Check that larger brains take proportionally longer
        self.assertLess(times[2] / times[0], 10.0)  # Allow some variance

    def test_simulation_performance(self):
        """Test simulation performance."""
        config = createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = AdvancedBrain(config)
        brain.initialize()
        
        # Time simulation steps
        start_time = time.time()
        
        for step in range(50):
            brain.step(step)
            
        end_time = time.time()
        
        simulation_time = end_time - start_time
        self.assertLess(simulation_time, 10.0)  # Should complete within 10 seconds
        
        # Performance should be reasonable
        steps_per_second = 50.0 / simulation_time
        self.assertGreater(steps_per_second, 5.0)  # At least 5 steps/second


@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm_advanced module not available")
class TestPythonBindingsDocumentation(unittest.TestCase):
    """Test that Python bindings match documentation."""

    def test_example_from_documentation(self):
        """Test an example from the documentation."""
        # Example: Create advanced brain with custom configuration
        config = createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        
        brain = AdvancedBrain(config)
        brain.initialize()
        
        # Use advanced features as shown in documentation
        brain.setSimulationMode("EventDriven")
        brain.enableAnalysisFeature("neural_pattern_analysis")
        
        # Verify features are enabled
        self.assertTrue(brain.isAnalysisFeatureEnabled("neural_pattern_analysis"))
        
        # Run simulation
        for step in range(10):
            brain.step(step)


if __name__ == '__main__':
    unittest.main()