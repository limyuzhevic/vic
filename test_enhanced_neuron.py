#!/usr/bin/env python3
"""
Tests for Enhanced Neuron Models

This module contains tests for the enhanced neuron models,
including LIF, Hodgkin-Huxley, and Izhikevich models.
"""

import sys
import os
import unittest
import numpy as np

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

from brain.EnhancedNeuron import (
    EnhancedNeuron, 
    EnhancedNeuronConfig, 
    NeuronModelType, 
    NeuronType,
    get_enhanced_neuron_examples
)

class TestEnhancedNeuronConfig(unittest.TestCase):
    """Test cases for EnhancedNeuronConfig"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = EnhancedNeuronConfig()
    
    def testConfigCreation(self):
        """Test basic config creation"""
        self.assertIsNotNone(self.config)
        self.assertEqual(self.config.model_type, NeuronModelType.LIF)
        self.assertEqual(self.config.neuron_type, NeuronType.INTERNAL)
    
    def testConfigToDict(self):
        """Test config to dictionary conversion"""
        config_dict = self.config.to_dict()
        self.assertIsInstance(config_dict, dict)
        self.assertIn('resting_potential', config_dict)
        self.assertIn('threshold', config_dict)
        self.assertIn('model_type', config_dict)
    
    def testConfigFromDict(self):
        """Test config from dictionary creation"""
        config_dict = {
            'resting_potential': -75.0,
            'threshold': -50.0,
            'model_type': 'Izhikevich',
            'neuron_type': 'Excitatory',
        }
        config = EnhancedNeuronConfig.from_dict(config_dict)
        self.assertEqual(config.resting_potential, -75.0)
        self.assertEqual(config.threshold, -50.0)
        self.assertEqual(config.model_type, NeuronModelType.IZHIKEVICH)
        self.assertEqual(config.neuron_type, NeuronType.EXCITATORY)
    
    def testModelTypeSetting(self):
        """Test setting different model types"""
        config = EnhancedNeuronConfig()
        
        # Test LIF
        config.set_model_type(NeuronModelType.LIF)
        self.assertEqual(config.model_type, NeuronModelType.LIF)
        self.assertEqual(config.model_name, 'LIF')
        
        # Test Hodgkin-Huxley
        config.set_model_type(NeuronModelType.HODGKIN_HUXLEY)
        self.assertEqual(config.model_type, NeuronModelType.HODGKIN_HUXLEY)
        self.assertEqual(config.model_name, 'Hodgkin-Huxley')
        
        # Test Izhikevich
        config.set_model_type(NeuronModelType.IZHIKEVICH)
        self.assertEqual(config.model_type, NeuronModelType.IZHIKEVICH)
        self.assertEqual(config.model_name, 'Izhikevich')
    
    def testModelSpecificParams(self):
        """Test model-specific parameters"""
        config = EnhancedNeuronConfig()
        
        # Test LIF parameters
        config.set_model_type(NeuronModelType.LIF)
        params = config.get_model_specific_params()
        self.assertEqual(params['type'], 'LIF')
        self.assertIn('tau', params)
        
        # Test HH parameters
        config.set_model_type(NeuronModelType.HODGKIN_HUXLEY)
        params = config.get_model_specific_params()
        self.assertEqual(params['type'], 'Hodgkin-Huxley')
        self.assertIn('max_sodium_conductance', params)
        
        # Test Izhikevich parameters
        config.set_model_type(NeuronModelType.IZHIKEVICH)
        params = config.get_model_specific_params()
        self.assertEqual(params['type'], 'Izhikevich')
        self.assertIn('a', params)
        self.assertIn('b', params)

class TestEnhancedNeuron(unittest.TestCase):
    """Test cases for EnhancedNeuron"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.neuron_lif = EnhancedNeuron(1, NeuronType.EXCITATORY)
        self.neuron_hh = EnhancedNeuron(2, NeuronType.INHIBITORY)
        self.neuron_izh = EnhancedNeuron(3, NeuronType.MODULATORY)
    
    def testNeuronCreation(self):
        """Test basic neuron creation"""
        self.assertIsNotNone(self.neuron_lif)
        self.assertIsNotNone(self.neuron_hh)
        self.assertIsNotNone(self.neuron_izh)
        
        self.assertEqual(self.neuron_lif.neuron_id, 1)
        self.assertEqual(self.neuron_lif.neuron_type, NeuronType.EXCITATORY)
        self.assertEqual(self.neuron_hh.neuron_id, 2)
        self.assertEqual(self.neuron_izh.neuron_id, 3)
    
    def testModelInitialization(self):
        """Test model initialization"""
        # Check default model (LIF)
        self.assertEqual(self.neuron_lif.get_model_name(), 'LIF')
        self.assertEqual(self.neuron_hh.get_model_name(), 'LIF')  # Default for inhibitory
        self.assertEqual(self.neuron_izh.get_model_name(), 'LIF')  # Default for modulatory
        
        # Change model types
        self.neuron_hh.set_model_type(NeuronModelType.HODGKIN_HUXLEY)
        self.assertEqual(self.neuron_hh.get_model_name(), 'Hodgkin-Huxley')
        
        self.neuron_izh.set_model_type(NeuronModelType.IZHIKEVICH)
        self.assertEqual(self.neuron_izh.get_model_name(), 'Izhikevich')
    
    def testExternalInput(self):
        """Test external input setting"""
        # Test LIF model
        self.neuron_lif.set_external_input(0.5)
        
        # Test HH model
        self.neuron_hh.set_model_type(NeuronModelType.HODGKIN_HUXLEY)
        self.neuron_hh.set_external_input(0.3)
        
        # Test Izhikevich model
        self.neuron_izh.set_model_type(NeuronModelType.IZHIKEVICH)
        self.neuron_izh.set_external_input(0.7)
    
    def testUpdateMethod(self):
        """Test update method with different models"""
        # Test LIF update
        spiked = self.neuron_lif.update(0.001, [0.1, 0.2])
        self.assertFalse(spiked)  # Should not spike with small inputs
        
        # Test HH update
        self.neuron_hh.set_model_type(NeuronModelType.HODGKIN_HUXLEY)
        spiked = self.neuron_hh.update(0.001, [0.1, 0.2])
        self.assertFalse(spiked)
        
        # Test Izhikevich update
        self.neuron_izh.set_model_type(NeuronModelType.IZHIKEVICH)
        spiked = self.neuron_izh.update(0.001, [0.5, 0.5])  # Larger input
        # May or may not spike depending on model parameters
        # Just ensure method doesn't crash
    
    def testReset(self):
        """Test neuron reset"""
        # Set some state
        self.neuron_lif.set_external_input(0.5)
        self.neuron_lif.update(0.001, [0.1, 0.2])
        
        # Reset neuron
        self.neuron_lif.reset()
        
        # Check that membrane potential is reset
        self.assertAlmostEqual(self.neuron_lif.membrane_potential, 
                              self.neuron_lif.config.resting_potential, places=5)
    
    def testSpikeTracking(self):
        """Test spike tracking functionality"""
        # Add some spikes manually
        self.neuron_lif.add_spike(0.1)
        self.neuron_lif.add_spike(0.2)
        self.neuron_lif.add_spike(0.3)
        
        # Check spike count
        self.assertEqual(self.neuron_lif.get_spike_count(), 3)
        
        # Check last spike time
        last_spike = self.neuron_lif.get_last_spike_time()
        self.assertAlmostEqual(last_spike, 0.3, places=5)
    
    def testToFromDict(self):
        """Test serialization to and from dictionary"""
        # Convert neuron to dictionary
        neuron_dict = self.neuron_lif.to_dict()
        
        # Create new neuron from dictionary
        new_neuron = EnhancedNeuron.from_dict(neuron_dict)
        
        # Check that properties are preserved
        self.assertEqual(new_neuron.neuron_id, self.neuron_lif.neuron_id)
        self.assertEqual(new_neuron.neuron_type, self.neuron_lif.neuron_type)
        self.assertEqual(new_neuron.config.model_type, self.neuron_lif.config.model_type)
    
    def testModelSpecificParameters(self):
        """Test model-specific parameter handling"""
        config = EnhancedNeuronConfig()
        
        # Test LIF parameters
        config.set_model_type(NeuronModelType.LIF)
        self.assertIn('tau', config.get_model_specific_params())
        
        # Test HH parameters
        config.set_model_type(NeuronModelType.HODGKIN_HUXLEY)
        self.assertIn('max_sodium_conductance', config.get_model_specific_params())
        
        # Test Izhikevich parameters
        config.set_model_type(NeuronModelType.IZHIKEVICH)
        self.assertIn('a', config.get_model_specific_params())
        self.assertIn('b', config.get_model_specific_params())

class TestEnhancedNeuronExamples(unittest.TestCase):
    """Test examples functionality"""
    
    def testExampleUsage(self):
        """Test example usage function"""
        examples = get_enhanced_neuron_examples()
        self.assertIsInstance(examples, dict)
        self.assertIn('lif_example', examples)
        self.assertIn('hh_example', examples)
        self.assertIn('izhikevich_example', examples)
        
        # Check example structure
        for example_name, example_data in examples.items():
            self.assertIn('description', example_data)
            self.assertIn('parameters', example_data)
            self.assertIn('usage', example_data)

class TestMathematicalHelpers(unittest.TestCase):
    """Test mathematical helper functions"""
    
    def testExpFunction(self):
        """Test exponential function"""
        # Import the exp function from the module
        try:
            from brain.EnhancedNeuron import exp
            
            # Test normal cases
            self.assertAlmostEqual(exp(0), 1.0)
            self.assertAlmostEqual(exp(1), np.e, places=5)
            self.assertAlmostEqual(exp(-1), 1.0/np.e, places=5)
            
            # Test edge cases
            self.assertAlmostEqual(exp(0), 1.0, places=5)
            
        except ImportError:
            self.skipTest("Mathematical helper functions not available")

if __name__ == '__main__':
    print("=== NLM Enhanced Neuron Models Tests ===")
    print()
    
    # Run all tests
    suite = unittest.TestLoader().loadTestsFromTestCase(TestEnhancedNeuronConfig)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestEnhancedNeuron))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestEnhancedNeuronExamples))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestMathematicalHelpers))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    if not result.wasSuccessful():
        sys.exit(1)
    else:
        print("\n=== All Enhanced Neuron Models Tests PASSED ===")
        print("\nEnhanced neuron models are working correctly!")
        print("\nAvailable models:")
        print("1. LIF (Leaky Integrate-and-Fire)")
        print("2. Hodgkin-Huxley (HH)")
        print("3. Izhikevich")
        print("\nFeatures:")
        print("- Multiple neuron model types")
        print("- Model-specific parameters")
        print("- Python and C++ implementation support")
        print("- Enhanced visualization support")
        print("- Comprehensive configuration system")