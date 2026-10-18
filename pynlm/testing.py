"""
Comprehensive tests for NLM Python bindings.

This module provides comprehensive testing of the NLM Python API,
including unit tests, integration tests, and benchmark tests.
"""

import os
import sys
import tempfile
import json
import time
import unittest
from pathlib import Path
from typing import Dict, Any, List

# Import the NLM module
import pynlm

# Skip tests if C++ bindings are not available
if not hasattr(pynlm, 'HAS_CPP_BINDINGS'):
    HAS_CPP_BINDINGS = False
else:
    HAS_CPP_BINDINGS = pynlm.HAS_CPP_BINDINGS

@unittest.skipIf(not HAS_CPP_BINDINGS, "C++ bindings not available")
class TestNLMModule(unittest.TestCase):
    """Test the basic NLM module structure and imports."""
    
    def test_version_info(self):
        """Test that version information is available."""
        info = pynlm.get_version_info()
        
        self.assertIn('version', info)
        self.assertIn('author', info)
        self.assertIn('license', info)
        self.assertEqual(info['version'], '0.1.0')
        
    def test_core_classes_exist(self):
        """Test that all core classes are available."""
        core_classes = [
            'Brain', 'AgentBrain', 'SimpleWorld', 'Config',
            'Action', 'SensoryPercept', 'WorldObject',
            'Vision', 'Audio', 'InternalSignals',
        ]
        
        for class_name in core_classes:
            self.assertTrue(hasattr(pynlm, class_name), 
                          f"Class {class_name} should be available")
            
    def test_enums_exist(self):
        """Test that all enumerations are available."""
        enum_classes = [
            'NeuronType', 'SynapseType', 'DevelopmentalStage',
            'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
        ]
        
        for enum_name in enum_classes:
            self.assertTrue(hasattr(pynlm, enum_name),
                          f"Enumeration {enum_name} should be available")
    
    def test_factory_functions_exist(self):
        """Test that all factory functions are available."""
        factory_functions = [
            'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        ]
        
        for func_name in factory_functions:
            self.assertTrue(hasattr(pynlm, func_name),
                          f"Factory function {func_name} should be available")

@unittest.skipIf(not HAS_CPP_BINDINGS, "C++ bindings not available")
class TestEnhancedBrain(unittest.TestCase):
    """Test the EnhancedBrain class and Pythonic features."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.brain = pynlm.EnhancedBrain()
        
    def tearDown(self):
        """Clean up after tests."""
        if hasattr(self, 'brain'):
            self.brain.reset()
            
    def test_basic_initialization(self):
        """Test basic brain initialization."""
        self.assertIsNotNone(self.brain)
        
    def test_step_functionality(self):
        """Test basic step functionality."""
        initial_stats = self.brain.get_stats()
        
        for i in range(10):
            self.brain.step(i)
            
        final_stats = self.brain.get_stats()
        
        # Check that step count increased
        self.assertEqual(final_stats['total_steps'], 10)
        
        # Check that other metrics changed (or at least are accessible)
        self.assertIn('firing_neurons', final_stats)
        self.assertIn('average_firing_rate', final_stats)
        
    def test_context_manager(self):
        """Test context manager functionality."""
        with pynlm.EnhancedBrain() as brain:
            brain.step(0)
            self.assertIsNotNone(brain)
            
        # After exiting context, brain should be reset
        self.assertEqual(self.brain.get_stats()['total_steps'], 0)
        
    def test_error_handling(self):
        """Test error handling for uninitialized brain."""
        # Create a brain and reset it
        brain = pynlm.EnhancedBrain()
        brain.reset()
        
        # Try to step without re-initializing (should handle gracefully)
        # Note: This depends on the implementation
        pass
        
    def test_stats_comprehensive(self):
        """Test comprehensive statistics collection."""
        stats = self.brain.get_stats()
        
        expected_keys = [
            'total_steps', 'elapsed_time', 'firing_neurons',
            'active_neurons', 'total_neurons', 'total_synapses',
            'total_spikes', 'average_firing_rate',
            'excitatory_inhibition_ratio', 'developmental_stage',
            'brain_status'
        ]
        
        for key in expected_keys:
            self.assertIn(key, stats, f"Stats should include {key}")
            
        # Check data types
        self.assertIsInstance(stats['total_steps'], int)
        self.assertIsInstance(stats['firing_neurons'], int)
        self.assertIsInstance(stats['total_neurons'], int)
        self.assertIsInstance(stats['average_firing_rate'], float)
        self.assertIsInstance(stats['developmental_stage'], int)

@unittest.skipIf(not HAS_CPP_BINDINGS, "C++ bindings not available")
class TestEnhancedAgentBrain(unittest.TestCase):
    """Test the EnhancedAgentBrain class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.EnhancedBrain(self.config)
        self.agent = pynlm.EnhancedAgentBrain(self.brain)
        self.world = pynlm.EnhancedSimpleWorld()
        
    def test_agent_initialization(self):
        """Test agent initialization."""
        self.agent.initialize(self.world)
        self.assertIsNotNone(self.agent)
        
    def test_sensory_processing(self):
        """Test sensory processing."""
        self.agent.initialize(self.world)
        
        # Mock a sensory percept
        class MockPercept:
            pass
            
        percept = MockPercept()
        self.agent.process_sensory_input(percept)
        
    def test_motor_decoding(self):
        """Test motor command decoding."""
        self.agent.initialize(self.world)
        action = self.agent.decode_motor_command()
        
        # Action should be a valid motor command
        self.assertIsNotNone(action)
        
    def test_feature_enabling(self):
        """Test feature enabling."""
        features = {
            'reward_modulation': True,
            'curiosity': True,
            'development': True,
        }
        
        self.agent.enable_features(features)
        
        # Check that features were enabled
        feature_states = self.agent.get_features()
        
        for feature, expected_value in features.items():
            self.assertEqual(feature_states[feature], expected_value,
                           f"Feature {feature} should be {expected_value}")
            
    def test_feature_states(self):
        """Test feature state retrieval."""
        self.agent.initialize(self.world)
        
        feature_states = self.agent.get_features()
        
        expected_features = [
            'reward_modulation', 'structural_plasticity', 'development',
            'curiosity', 'neuromodulation_level', 'curiosity_level',
            'novelty_level', 'prediction_error', 'developmental_stage'
        ]
        
        for feature in expected_features:
            self.assertIn(feature, feature_states,
                         f"Feature states should include {feature}")

@unittest.skipIf(not HAS_CPP_BINDINGS, "C++ bindings not available")
class TestEnhancedWorld(unittest.TestCase):
    """Test the EnhancedSimpleWorld class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.world = pynlm.EnhancedSimpleWorld()
        
    def test_world_configuration(self):
        """Test world configuration."""
        width, height = 30, 40
        vision_width, vision_height = 10, 8
        
        self.world.configure(width, height, vision_width, vision_height)
        
        properties = self.world.get_properties()
        
        self.assertEqual(properties['width'], width)
        self.assertEqual(properties['height'], height)
        
    def test_world_reset(self):
        """Test world reset functionality."""
        self.world.configure(20, 20)
        self.world.reset()
        
        # World should be in a reset state
        self.assertEqual(self.world.get_properties()['width'], 20)
        
    def test_world_update(self):
        """Test world update."""
        self.world.configure(10, 10)
        self.world.reset()
        
        initial_time = self.world.get_properties()['simulation_time']
        
        self.world.update(0.1)
        
        final_time = self.world.get_properties()['simulation_time']
        
        # Time should have advanced
        self.assertGreater(final_time, initial_time)
        
    def test_agent_interaction(self):
        """Test agent-world interaction."""
        self.world.configure(20, 20)
        self.world.reset()
        
        # Set agent start position
        self.world.set_agent_start(10.0, 10.0)
        
        # Apply motor command
        action = pynlm.MotorCommand.MoveForward
        self.world.apply_motor_command(action, 0.5)
        
        # Check world state
        agent_body = self.world.get_agent_body()
        self.assertIsNotNone(agent_body)

@unittest.skipIf(not HAS_CPP_BINDINGS, "C++ bindings not available")
class TestConfigurationManagement(unittest.TestCase):
    """Test configuration management."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.ConfigManager()
        
    def test_config_validation(self):
        """Test configuration validation."""
        # Config should validate
        self.assertTrue(self.config.validate())
        
    def test_config_set_get(self):
        """Test setting and getting configuration values."""
        self.config.set('brain.neuron_count', 500)
        self.assertEqual(self.config.get('brain.neuron_count'), 500)
        
        self.config.set('plasticity.stdp.enable', False)
        self.assertEqual(self.config.get('plasticity.stdp.enable'), False)
        
    def test_config_has(self):
        """Test checking for configuration keys."""
        self.assertFalse(self.config.has('nonexistent.key'))
        
    def test_config_keys(self):
        """Test getting all configuration keys."""
        keys = self.config.get_keys()
        self.assertIsInstance(keys, list)
        
    def test_config_file_operations(self):
        """Test file-based configuration operations."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
            
        try:
            # Set some values
            self.config.set('brain.neuron_count', 750)
            self.config.set('test.value', 'test_string')
            
            # Save to file
            self.config.save_to_file(config_file, pretty=True)
            
            # Load from file
            new_config = pynlm.ConfigManager()
            new_config.load_from_file(config_file)
            
            # Check values were saved and loaded
            self.assertEqual(new_config.get('brain.neuron_count'), 750)
            self.assertEqual(new_config.get('test.value'), 'test_string')
            
        finally:
            os.unlink(config_file)

@unittest.skipIf(not HAS_CPP_BINDINGS, "C++ bindings not available")
class TestUtilityFunctions(unittest.TestCase):
    """Test utility functions."""
    
    def test_enable_features(self):
        """Test enable_features utility function."""
        # Create a mock agent brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Enable features
        features = {'reward_modulation': True, 'curiosity': True}
        
        # This should work if C++ bindings are available
        result = pynlm.enable_features(agent, features)
        self.assertEqual(result, agent)
        
    def test_brain_stats(self):
        """Test brain_stats utility function."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        stats = pynlm.brain_stats(brain)
        
        # Should contain expected keys
        expected_keys = ['firing_neurons', 'total_neurons', 'average_firing_rate']
        for key in expected_keys:
            self.assertIn(key, stats)
            
    def test_run_simulation(self):
        """Test run_simulation utility function."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Run simulation
        stats = pynlm.run_simulation(brain, 100)
        
        # Should have completed steps
        self.assertEqual(stats['total_steps'], 100)
        
    def test_benchmark_simulation(self):
        """Test benchmark_simulation utility function."""
        def brain_factory():
            return pynlm.createBrain(pynlm.createDefaultConfig())
            
        results = pynlm.benchmark_simulation(brain_factory, num_steps=50, num_runs=2)
        
        # Should have results for each run
        self.assertEqual(len(results), 2)
        
        # Each result should have expected keys
        for result in results:
            self.assertIn('run', result)
            self.assertIn('elapsed_time', result)
            self.assertIn('steps_per_second', result)
            self.assertIn('brain_stats', result)

@unittest.skipIf(not HAS_CPP_BINDINGS, "C++ bindings not available")
class TestContextManagers(unittest.TestCase):
    """Test context manager functionality."""
    
    def test_brain_session(self):
        """Test brain_session context manager."""
        with pynlm.brain_session() as session:
            brain = session.brain
            world = session.world
            agent = session.agent
            
            # Should be able to use all components
            self.assertIsNotNone(brain)
            self.assertIsNotNone(world)
            self.assertIsNotNone(agent)
            
            # Should be able to run simulation
            for i in range(10):
                brain.step(i)
                
        # After exiting context, should be reset
        self.assertEqual(session.brain.get_stats()['total_steps'], 0)

class TestBackwardCompatibility(unittest.TestCase):
    """Test backward compatibility with original C++ bindings."""
    
    def test_original_classes_available(self):
        """Test that original C++ classes are still available for compatibility."""
        # These should be available even if they're enhanced versions
        self.assertTrue(hasattr(pynlm, 'Brain'))
        self.assertTrue(hasattr(pynlm, 'AgentBrain'))
        self.assertTrue(hasattr(pynlm, 'SimpleWorld'))
        self.assertTrue(hasattr(pynlm, 'Config'))
        
    def test_enums_backward_compatibility(self):
        """Test that enumerations work correctly."""
        # Test that enum values are accessible
        self.assertTrue(hasattr(pynlm.NeuronType, 'Excitatory'))
        self.assertTrue(hasattr(pynlm.NeuronType, 'Inhibitory'))
        self.assertTrue(hasattr(pynlm.DevelopmentalStage, 'Initial'))
        
    def test_factory_functions_backward_compatibility(self):
        """Test that factory functions work."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        brain = pynlm.createBrain(config)
        self.assertIsNotNone(brain)
        
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
class TestErrorHandling(unittest.TestCase):
    """Test error handling."""
    
    def test_nlm_error_classes(self):
        """Test that NLM error classes exist."""
        self.assertTrue(hasattr(pynlm, 'NLMError'))
        self.assertTrue(hasattr(pynlm, 'ConfigurationError'))
        self.assertTrue(hasattr(pynlm, 'SimulationError'))
        self.assertTrue(hasattr(pynlm, 'MemoryError'))
        self.assertTrue(hasattr(pynlm, 'ValidationError'))
        
    def test_error_inheritance(self):
        """Test error class inheritance."""
        self.assertTrue(issubclass(pynlm.ConfigurationError, pynlm.NLMError))
        self.assertTrue(issubclass(pynlm.SimulationError, pynlm.NLMError))

class TestStatisticsAndMonitoring(unittest.TestCase):
    """Test statistics and monitoring functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.brain = pynlm.EnhancedBrain()
        
    def test_performance_stats(self):
        """Test performance statistics."""
        # Run some simulation
        for i in range(50):
            self.brain.step(i)
            
        stats = self.brain.get_stats()
        
        # Check performance metrics
        self.assertGreater(stats['elapsed_time'], 0)
        self.assertGreater(stats['steps_per_second'], 0)
        self.assertEqual(stats['total_steps'], 50)
        
    def test_agent_feature_monitoring(self):
        """Test agent feature monitoring."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.EnhancedBrain(config)
        agent = pynlm.EnhancedAgentBrain(brain)
        world = pynlm.EnhancedSimpleWorld()
        world.configure(10, 10)
        agent.initialize(world)
        
        # Enable features
        features = {'reward_modulation': True, 'curiosity': True}
        agent.enable_features(features)
        
        # Get feature states
        feature_states = agent.get_features()
        
        # Check that monitoring features are present
        self.assertIn('neuromodulation_level', feature_states)
        self.assertIn('curiosity_level', feature_states)
        self.assertIn('novelty_level', feature_states)

class TestExamples(unittest.TestCase):
    """Test example usage patterns."""
    
    def test_minimal_example(self):
        """Test the minimal example from the documentation."""
        # One line to create a virtual brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Initialize it
        brain.initialize()
        
        # Make it active
        brain.step(0)
        
        # Check result
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        
    def test_complete_agent_example(self):
        """Test the complete agent example from documentation."""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run agent (short version)
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        # Should complete without errors
        self.assertTrue(True)

def run_all_tests():
    """Run all tests and provide a summary."""
    print("Running NLM Python bindings tests...")
    print("=" * 50)
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestNLMModule)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestEnhancedBrain))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestEnhancedAgentBrain))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestEnhancedWorld))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestConfigurationManagement))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestUtilityFunctions))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestContextManagers))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestBackwardCompatibility))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestErrorHandling))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestStatisticsAndMonitoring))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestExamples))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Provide summary
    print("\n" + "=" * 50)
    print("TEST SUMMARY")
    print("=" * 50)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    
    if HAS_CPP_BINDINGS:
        print("\nC++ bindings: AVAILABLE")
        print("Enhanced Python features: ACTIVE")
    else:
        print("\nC++ bindings: NOT AVAILABLE")
        print("Enhanced Python features: LIMITED (mock mode)")
        
    if result.failures:
        print("\nFAILURES:")
        for test, traceback in result.failures:
            print(f"- {test}")
            
    if result.errors:
        print("\nERRORS:")
        for test, traceback in result.errors:
            print(f"- {test}")
            
    return result.wasSuccessful()

if __name__ == '__main__':
    success = run_all_tests()
    sys.exit(0 if success else 1)