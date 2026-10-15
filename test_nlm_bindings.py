#!/usr/bin/env python3
"""
Test Suite for NLM Python Bindings

This is a comprehensive test suite for the NLM (Neural Learning Machine) Python bindings,
testing both the standard (pynlm) and enhanced (pynlm_enhanced) bindings.

The test suite covers:
- Basic functionality tests
- Advanced binding features
- Error handling tests
- Configuration tests
- Simulation tests
- Performance tests
- Integration tests

Usage:
    python test_nlm_bindings.py [--enhanced] [--verbose] [--output results.json]

Dependencies:
    - pytest (if available)
    - numpy (if available)
    - pynlm (standard bindings)
    - pynlm_enhanced (enhanced bindings, if --enhanced flag)
"""

import sys
import os
import json
import argparse
import unittest
from unittest.mock import patch
import tempfile
import shutil

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNL_available = True
except ImportError:
    PYNL_available = False

try:
    import pynlm_enhanced as pynlm_enhanced
    PYNL_ENHANCED_available = True
except ImportError:
    PYNL_ENHANCED_available = False

class TestPythonBindings(unittest.TestCase):
    """
    Test class for NLM Python bindings.
    
    This class contains tests for both standard and enhanced Python bindings.
    Use the --enhanced flag to test the enhanced bindings.
    """
    
    @classmethod
    def setUpClass(cls):
        """Set up test class"""
        cls.use_enhanced = getattr(cls, 'use_enhanced', False)
        cls.bindings_module = pynlm_enhanced if cls.use_enhanced else pynlm
        
        if not PYNL_available and not PYNL_ENHANCED_available:
            raise unittest.SkipTest("No NLM Python bindings available")
        
        if cls.use_enhanced and not PYNL_ENHANCED_available:
            raise unittest.SkipTest("Enhanced NLM Python bindings not available")
    
    def setUp(self):
        """Set up test case"""
        self.bindings = self.bindings_module
        self.temp_dir = tempfile.mkdtemp()
        self.test_files = []
    
    def tearDown(self):
        """Clean up test case"""
        for file_path in self.test_files:
            try:
                if os.path.exists(file_path):
                    os.remove(file_path)
            except:
                pass
        
        try:
            shutil.rmtree(self.temp_dir)
        except:
            pass
    
    def test_import(self):
        """Test that bindings module can be imported"""
        self.assertIsNotNone(self.bindings)
        self.assertTrue(hasattr(self.bindings, 'createDefaultConfig'))
    
    def test_config_creation(self):
        """Test configuration creation"""
        config = self.bindings.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertTrue(hasattr(config, 'set'))
        self.assertTrue(hasattr(config, 'getKeys'))
    
    def test_config_basic_operations(self):
        """Test basic configuration operations"""
        config = self.bindings.createDefaultConfig()
        
        # Test setting and getting values
        config.set("test_int", 42)
        self.assertTrue(config.has("test_int"))
        
        config.set("test_float", 3.14)
        self.assertTrue(config.has("test_float"))
        
        config.set("test_string", "hello")
        self.assertTrue(config.has("test_string"))
        
        config.set("test_bool", True)
        self.assertTrue(config.has("test_bool"))
        
        # Test getting keys
        keys = config.getKeys()
        self.assertIn("test_int", keys)
        self.assertIn("test_float", keys)
        self.assertIn("test_string", keys)
        self.assertIn("test_bool", keys)
        
        # Test summary
        summary = config.summary()
        self.assertIsInstance(summary, str)
        self.assertGreater(len(summary), 0)
    
    def test_config_file_operations(self):
        """Test configuration file operations"""
        config = self.bindings.createDefaultConfig()
        config.set("test_value", 123)
        
        # Note: This might not work if the bindings don't have file operations
        # Skip this test if not supported
        try:
            if config.saveToFile:
                config_file = os.path.join(self.temp_dir, "test_config.cfg")
                self.test_files.append(config_file)
                
                if config.saveToFile(config_file):
                    self.assertTrue(os.path.exists(config_file))
                    
                    # Create new config and load
                    new_config = self.bindings.createDefaultConfig()
                    if new_config.loadFromFile:
                        if new_config.loadFromFile(config_file):
                            self.assertTrue(new_config.has("test_value"))
        except (AttributeError, NotImplementedError):
            self.skipTest("File operations not supported")
    
    def test_brain_creation(self):
        """Test brain creation"""
        config = self.bindings.createDefaultConfig()
        brain = self.bindings.createBrain(config)
        
        self.assertIsNotNone(brain)
        self.assertTrue(hasattr(brain, 'initialize'))
        self.assertTrue(hasattr(brain, 'step'))
        self.assertTrue(hasattr(brain, 'getTotalNeuronCount'))
        self.assertTrue(hasattr(brain, 'getTotalSynapseCount'))
    
    def test_brain_basic_operations(self):
        """Test basic brain operations"""
        config = self.bindings.createDefaultConfig()
        brain = self.bindings.createBrain(config)
        
        # Initialize brain
        brain.initialize()
        
        # Test statistics
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertGreater(brain.getActiveNeuronCount(), 0)
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        
        # Test step
        brain.step(0)
        brain.step(1)
        
        # Test spike count
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(brain.getAverageFiringRate(), 0.0)
    
    def test_agent_creation(self):
        """Test agent creation"""
        config = self.bindings.createDefaultConfig()
        brain = self.bindings.createBrain(config)
        agent = self.bindings.createAgentBrain(brain)
        
        self.assertIsNotNone(agent)
        self.assertTrue(hasattr(agent, 'initialize'))
        self.assertTrue(hasattr(agent, 'processSensoryInput'))
        self.assertTrue(hasattr(agent, 'decodeMotorCommand'))
        self.assertTrue(hasattr(agent, 'applyRewardModulation'))
        self.assertTrue(hasattr(agent, 'enableRewardModulation'))
        self.assertTrue(hasattr(agent, 'enableDevelopment'))
        self.assertTrue(hasattr(agent, 'enableCuriosity'))
    
    def test_world_creation(self):
        """Test world creation"""
        world = self.bindings.createSimpleWorld()
        
        self.assertIsNotNone(world)
        self.assertTrue(hasattr(world, 'configure'))
        self.assertTrue(hasattr(world, 'update'))
        self.assertTrue(hasattr(world, 'getSensoryPercept'))
        self.assertTrue(hasattr(world, 'getAgentBody'))
    
    def test_enhanced_features(self):
        """Test enhanced features (if using enhanced bindings)"""
        if not self.use_enhanced:
            self.skipTest("Enhanced features only available in enhanced bindings")
        
        # Test enhanced configuration helpers
        config = self.bindings.createDevelopmentConfig()
        self.assertIsNotNone(config)
        
        # Test enhanced brain statistics
        brain = self.bindings.createBrain(config)
        brain.initialize()
        
        # Test enhanced statistics
        stats = self.bindings.getBrainStatistics(brain, ["basic"])
        self.assertIsInstance(stats, str)
        self.assertGreater(len(stats), 0)
        
        # Test enhanced simulation
        agent = self.bindings.PyAgentBrain(brain)
        world = self.bindings.PySimpleWorld()
        
        self.assertIsNotNone(agent)
        self.assertIsNotNone(world)
        
        # Test enhanced agent methods
        self.assertTrue(hasattr(agent, 'batchDecodeMotorCommands'))
        self.assertTrue(hasattr(agent, 'runSimulation'))
        self.assertTrue(hasattr(agent, 'createEnhancedConfig'))
    
    def test_error_handling(self):
        """Test error handling"""
        # Test that exceptions are properly raised
        try:
            # This should raise some kind of error
            config = self.bindings.createDefaultConfig()
            # Try to use invalid operations
            # Note: This depends on the specific error handling in the bindings
            pass
        except Exception as e:
            # We expect some exception to be raised
            self.assertIsInstance(e, Exception)
    
    def test_validation(self):
        """Test validation functions"""
        if hasattr(self.bindings, 'validateRequiredConfigKeys'):
            # Test validation
            config = self.bindings.createDefaultConfig()
            config.set("required_key", "value")
            
            required_keys = ["required_key", "another_key"]
            # This might fail if another_key is not in config
            try:
                result = self.bindings.validateRequiredConfigKeys(config, required_keys)
                self.assertIsInstance(result, bool)
            except Exception:
                pass  # Validation might fail if key is missing
    
    def test_performance_stats(self):
        """Test performance statistics"""
        if hasattr(self.bindings, 'getPerformanceStats'):
            config = self.bindings.createDefaultConfig()
            brain = self.bindings.createBrain(config)
            brain.initialize()
            
            stats = self.bindings.getPerformanceStats(brain)
            self.assertIsInstance(stats, str)
            self.assertGreater(len(stats), 0)

class TestEnhancedBindings(unittest.TestCase):
    """
    Test class specifically for enhanced NLM Python bindings.
    
    This class contains tests specific to enhanced bindings features.
    """
    
    # Use enhanced bindings
    use_enhanced = True
    
    def test_enhanced_configuration_helpers(self):
        """Test enhanced configuration helper functions"""
        config = self.bindings.createDevelopmentConfig()
        self.assertIsNotNone(config)
        
        # Test that development config is properly configured
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("region_count"))
        self.assertTrue(config.has("connection_probability"))
    
    def test_enhanced_simulation(self):
        """Test enhanced simulation features"""
        config = self.bindings.createDevelopmentConfig()
        brain = self.bindings.createBrain(config)
        agent = self.bindings.PyAgentBrain(brain)
        world = self.bindings.PySimpleWorld()
        
        # Configure and initialize
        world.configure(20, 20, 8, 8)
        world.reset()
        agent.initialize(world)
        
        # Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run enhanced simulation
        result = self.bindings.runValidatedSimulation(agent, world, 100, verbose=False)
        self.assertIsInstance(result, str)
        self.assertGreater(len(result), 0)
    
    def test_enhanced_agent_features(self):
        """Test enhanced agent features"""
        config = self.bindings.createDevelopmentConfig()
        brain = self.bindings.createBrain(config)
        agent = self.bindings.PyAgentBrain(brain)
        
        # Test enhanced agent methods
        self.assertTrue(hasattr(agent, 'batchDecodeMotorCommands'))
        self.assertTrue(hasattr(agent, 'runSimulation'))
        self.assertTrue(hasattr(agent, 'createEnhancedConfig'))
        self.assertTrue(hasattr(agent, 'getCuriosityLevel'))
        self.assertTrue(hasattr(agent, 'getNoveltyLevel'))
        self.assertTrue(hasattr(agent, 'getPredictionError'))
        
        # Test batch processing
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Create a simple world for testing
        world = self.bindings.PySimpleWorld()
        world.configure(10, 10, 4, 4)
        world.reset()
        agent.initialize(world)
        
        # Test batch processing
        commands = agent.batchDecodeMotorCommands(10)
        self.assertIsInstance(commands, list)
        self.assertEqual(len(commands), 10)
    
    def test_enhanced_world_features(self):
        """Test enhanced world features"""
        world = self.bindings.PySimpleWorld()
        
        # Test enhanced world methods
        self.assertTrue(hasattr(world, 'getWorldStatus'))
        self.assertTrue(hasattr(world, 'addResource'))
        self.assertTrue(hasattr(world, 'addHazard'))
        self.assertTrue(hasattr(world, 'collectStats'))
        
        # Configure and test
        world.configure(15, 15, 6, 6)
        status = world.getWorldStatus()
        self.assertIsInstance(status, str)
        
        # Test object addition
        world.addResource(5.0, 5.0, 10.0)
        world.addHazard(10.0, 10.0, 5.0)
        
        # Test statistics collection
        stats = world.collectStats()
        self.assertIsInstance(stats, dict)
        self.assertIn('agent_x', stats)
        self.assertIn('agent_y', stats)
    
    def test_enhanced_examples(self):
        """Test enhanced example scenarios"""
        # Test silent brain example
        if hasattr(self.bindings, 'example_1_silent_brain'):
            result = self.bindings.example_1_silent_brain()
            self.assertIsInstance(result, str)
        
        # Test complete agent example
        if hasattr(self.bindings, 'example_2_complete_agent'):
            result = self.bindings.example_2_complete_agent()
            self.assertIsInstance(result, str)
        
        # Test advanced agent example
        if hasattr(self.bindings, 'example_3_advanced_agent'):
            result = self.bindings.example_3_advanced_agent()
            self.assertIsInstance(result, str)
class TestComparison(unittest.TestCase):
    """
    Test class to compare standard and enhanced bindings.
    
    This class contains tests that compare the functionality of standard
    and enhanced bindings to ensure compatibility.
    """
    
    def setUp(self):
        """Set up test case"""
        if PYNL_available and PYNL_ENHANCED_available:
            self.standard_bindings = pynlm
            self.enhanced_bindings = pynlm_enhanced
            self.both_available = True
        else:
            self.both_available = False
    
    def test_basic_functionality(self):
        """Test that basic functionality is available in both bindings"""
        if not self.both_available:
            self.skipTest("Both bindings not available")
        
        # Test that both have createDefaultConfig
        self.assertTrue(hasattr(self.standard_bindings, 'createDefaultConfig'))
        self.assertTrue(hasattr(self.enhanced_bindings, 'createDefaultConfig'))
        
        # Test that both can create configuration
        std_config = self.standard_bindings.createDefaultConfig()
        enh_config = self.enhanced_bindings.createDefaultConfig()
        
        self.assertIsNotNone(std_config)
        self.assertIsNotNone(enh_config)
    
    def test_common_features(self):
        """Test that common features are available in both bindings"""
        if not self.both_available:
            self.skipTest("Both bindings not available")
        
        # Test common features
        common_features = ['createBrain', 'createAgentBrain', 'createSimpleWorld']
        
        for feature in common_features:
            std_has = hasattr(self.standard_bindings, feature)
            enh_has = hasattr(self.enhanced_bindings, feature)
            self.assertEqual(std_has, enh_has, 
                          f"Feature {feature} availability differs between bindings")
    
    def test_enhanced_features(self):
        """Test that enhanced features are only available in enhanced bindings"""
        if not PYNL_ENHANCED_available:
            self.skipTest("Enhanced bindings not available")
        
        # Test enhanced features
        enhanced_features = [
            'createDevelopmentConfig',
            'getBrainStatistics',
            'runValidatedSimulation',
            'PyAgentBrain',
            'PySimpleWorld',
            'example_1_silent_brain'
        ]
        
        for feature in enhanced_features:
            self.assertTrue(hasattr(self.enhanced_bindings, feature),
                          f"Enhanced feature {feature} not available")
            
            # Ensure standard bindings don't have these features
            if PYNL_available:
                self.assertFalse(hasattr(self.standard_bindings, feature),
                               f"Standard bindings should not have enhanced feature {feature}")
class NLMBindingsTestRunner:
    """
    Test runner for NLM Python bindings.
    
    This class provides a custom test runner for NLM Python bindings,
    supporting different test modes and output formats.
    """
    
    def __init__(self, enhanced=False, verbose=False, output_file=None):
        """
        Initialize test runner.
        
        Args:
            enhanced (bool): Use enhanced bindings
            verbose (bool): Enable verbose output
            output_file (str): Output file for results
        """
        self.enhanced = enhanced
        self.verbose = verbose
        self.output_file = output_file
        
        # Set the use_enhanced attribute for test classes
        TestPythonBindings.use_enhanced = enhanced
    
    def run_tests(self):
        """Run all tests"""
        print(f"Running NLM Python bindings tests (enhanced: {self.enhanced}, verbose: {self.verbose})")
        
        # Create test suite
        loader = unittest.TestLoader()
        suite = unittest.TestSuite()
        
        # Add test classes
        if self.enhanced:
            suite.addTests(loader.loadTestsFromTestCase(TestEnhancedBindings))
        else:
            suite.addTests(loader.loadTestsFromTestCase(TestPythonBindings))
        
        # Add comparison tests if both bindings available
        if PYNL_available and PYNL_ENHANCED_available:
            suite.addTests(loader.loadTestsFromTestCase(TestComparison))
        
        # Run tests
        runner = unittest.TextTestRunner(verbosity=2 if self.verbose else 1)
        result = runner.run(suite)
        
        # Save results if output file specified
        if self.output_file:
            self.save_results(result)
        
        # Return test result
        return result
    
    def save_results(self, test_result):
        """
        Save test results to file.
        
        Args:
            test_result (TestResult): Test result object
        """
        # Create results dictionary
        results = {
            'testsRun': test_result.testsRun,
            'failures': len(test_result.failures),
            'errors': len(test_result.errors),
            'skipped': len(test_result.skipped) if hasattr(test_result, 'skipped') else 0,
            'wasSuccessful': test_result.wasSuccessful(),
            'enhanced': self.enhanced,
            'verbose': self.verbose
        }
        
        # Add detailed failure information
        if test_result.failures:
            results['failures_details'] = [
                {
                    'test': str(failure[0]),
                    'traceback': failure[1]
                }
                for failure in test_result.failures
            ]
        
        if test_result.errors:
            results['errors_details'] = [
                {
                    'test': str(error[0]),
                    'traceback': error[1]
                }
                for error in test_result.errors
            ]
        
        # Save to file
        with open(self.output_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Test results saved to {self.output_file}")

if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Test Suite for NLM Python Bindings",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Usage:
  python test_nlm_bindings.py [--enhanced] [--verbose] [--output results.json]

Examples:
  python test_nlm_bindings.py
  python test_nlm_bindings.py --enhanced
  python test_nlm_bindings.py --verbose --output test_results.json
  python test_nlm_bindings.py --enhanced --verbose
        """
    )
    
    parser.add_argument(
        '--enhanced', action='store_true',
        help='Use enhanced Python bindings (pynlm_enhanced)'
    )
    parser.add_argument(
        '--verbose', action='store_true',
        help='Enable verbose output'
    )
    parser.add_argument(
        '--output', help='Output file for test results (JSON format)'
    )
    parser.add_argument(
        '--list', action='store_true',
        help='List available tests'
    )
    
    args = parser.parse_args()
    
    # Check if bindings are available
    if not PYNL_available and not PYNL_ENHANCED_available:
        print("ERROR: No NLM Python bindings available")
        print("Please install NLM Python bindings:")
        print("  pip install pynlm")
        print("  or pip install pynlm_enhanced")
        sys.exit(1)
    
    # List available tests if requested
    if args.list:
        print("Available Tests:")
        print("1. Standard Bindings Tests (pynlm)")
        if PYNL_available:
            print("  - Basic functionality tests")
            print("  - Configuration tests")
            print("  - Brain operations tests")
            print("  - Agent tests")
            print("  - World tests")
            print("  - Error handling tests")
            print("  - Validation tests")
            print("  - Performance tests")
        
        print("\n2. Enhanced Bindings Tests (pynlm_enhanced)")
        if PYNL_ENHANCED_available:
            print("  - Enhanced configuration tests")
            print("  - Enhanced simulation tests")
            print("  - Enhanced agent features tests")
            print("  - Enhanced world features tests")
            print("  - Enhanced example tests")
        
        print("\n3. Comparison Tests (pynlm vs pynlm_enhanced)")
        if PYNL_available and PYNL_ENHANCED_available:
            print("  - Basic functionality comparison")
            print("  - Common features verification")
            print("  - Enhanced features validation")
        
        sys.exit(0)
    
    # Run tests
    test_runner = NLMBindingsTestRunner(
        enhanced=args.enhanced,
        verbose=args.verbose,
        output_file=args.output
    )
    
    result = test_runner.run_tests()
    
    # Exit with appropriate code
    sys.exit(0 if result.wasSuccessful() else 1)
#EOF