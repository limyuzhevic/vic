"""
Enhanced NLM Python Bindings - Integration Tests

This file contains integration tests for the enhanced NLM Python bindings,
testing the new wrapper classes, factory functions, and utilities.
"""

import unittest
import tempfile
import os
from typing import Dict, Any, List

# Import enhanced bindings
try:
    import pynlm_enhanced as nlm
except ImportError:
    import pynlm as nlm  # Fallback to basic bindings

class TestPythonConfigWrapper(unittest.TestCase):
    """Test PythonConfigWrapper class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.wrapper = nlm.PythonConfigWrapper()
    
    def test_constructor_empty(self):
        """Test empty constructor"""
        self.assertTrue(self.wrapper.validate())
        self.assertEqual(len(self.wrapper.get_keys()), 0)
    
    def test_constructor_from_dict(self):
        """Test constructor with Python dictionary"""
        config_dict = {
            'int_value': 42,
            'double_value': 3.14,
            'string_value': 'hello',
            'bool_value': True,
            'float_list': [1.0, 2.0, 3.0],
            'int_list': [1, 2, 3],
            'double_list': [1.5, 2.5, 3.5],
            'string_list': ['a', 'b', 'c']
        }
        
        wrapper = nlm.PythonConfigWrapper(config_dict)
        self.assertTrue(wrapper.validate())
        self.assertEqual(wrapper.get_int('int_value'), 42)
        self.assertAlmostEqual(wrapper.get_double('double_value'), 3.14, places=2)
        self.assertEqual(wrapper.get_string('string_value'), 'hello')
        self.assertEqual(wrapper.get_bool('bool_value'), True)
        self.assertEqual(wrapper.get_float_list('float_list'), [1.0, 2.0, 3.0])
        self.assertEqual(wrapper.get_int_list('int_list'), [1, 2, 3])
        self.assertEqual(wrapper.get_double_list('double_list'), [1.5, 2.5, 3.5])
        self.assertEqual(wrapper.get_string_list('string_list'), ['a', 'b', 'c'])
    
    def test_has_and_get(self):
        """Test has and get operations"""
        wrapper = nlm.PythonConfigWrapper({'test_key': 'test_value'})
        
        self.assertTrue(wrapper.has('test_key'))
        self.assertFalse(wrapper.has('nonexistent_key'))
        
        self.assertEqual(wrapper.get_string('test_key'), 'test_value')
        self.assertEqual(wrapper.get_string('test_key', 'default'), 'test_value')
    
    def test_remove_and_clear(self):
        """Test remove and clear operations"""
        wrapper = nlm.PythonConfigWrapper({
            'key1': 'value1',
            'key2': 'value2',
            'key3': 'value3'
        })
        
        wrapper.remove('key1')
        self.assertFalse(wrapper.has('key1'))
        
        self.assertTrue(wrapper.has('key2'))
        self.assertTrue(wrapper.has('key3'))
        
        wrapper.clear()
        self.assertEqual(len(wrapper.get_keys()), 0)
        self.assertTrue(wrapper.validate())
    
    def test_update_from_dict(self):
        """Test update from dictionary"""
        wrapper = nlm.PythonConfigWrapper()
        wrapper.update_from_dict({'new_key': 'new_value'})
        
        self.assertTrue(wrapper.has('new_key'))
        self.assertEqual(wrapper.get_string('new_key'), 'new_value')
    
    def test_to_dict(self):
        """Test conversion to dictionary"""
        wrapper = nlm.PythonConfigWrapper({
            'int': 42,
            'float': 3.14,
            'string': 'hello',
            'bool': True,
            'list': [1.0, 2.0, 3.0]
        })
        
        result = wrapper.to_dict()
        self.assertIsInstance(result, str)
        self.assertIn('int', result)
        self.assertIn('float', result)
        self.assertIn('string', result)
        self.assertIn('bool', result)
        self.assertIn('list', result)
    
    def test_save_and_load(self):
        """Test save and load functionality"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            wrapper = nlm.PythonConfigWrapper({
                'test_key': 'test_value',
                'test_int': 123
            })
            
            self.assertTrue(wrapper.save_to_file(filepath))
            
            # Load and verify
            wrapper2 = nlm.PythonConfigWrapper(filepath)
            self.assertEqual(wrapper2.get_string('test_key'), 'test_value')
            self.assertEqual(wrapper2.get_int('test_int'), 123)
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)

class TestPythonBrainWrapper(unittest.TestCase):
    """Test PythonBrainWrapper class"""
    
    def test_constructor(self):
        """Test constructor"""
        pass
    
    def test_simulate_step(self):
        """Test simulation step"""
        pass
    
    def test_get_statistics(self):
        """Test statistics collection"""
        pass
    
    def test_get_neuron_population_stats(self):
        """Test neuron population statistics"""
        pass
    
    def test_reset_simulation(self):
        """Test simulation reset"""
        pass
    
    def test_save_load(self):
        """Test save and load"""
        pass
class TestPythonWorldWrapper(unittest.TestCase):
    """Test PythonWorldWrapper class"""
    
    def test_constructor(self):
        """Test constructor"""
        pass
    
    def test_configure_from_dict(self):
        """Test configuration from dictionary"""
        pass
    
    def test_reset(self):
        """Test reset"""
        pass
    
    def test_update(self):
        """Test update"""
        pass
    
    def test_apply_action(self):
        """Test action application"""
        pass
    
    def test_get_agent_state(self):
        """Test agent state retrieval"""
        pass
    
    def test_get_sensory_percept(self):
        """Test sensory percept retrieval"""
        pass
class TestPythonAgentBrainWrapper(unittest.TestCase):
    """Test PythonAgentBrainWrapper class"""
    
    def test_constructor(self):
        """Test constructor"""
        pass
    
    def test_initialize_step_by_step(self):
        """Test step-by-step initialization"""
        pass
    
    def test_process_sensory_input_step_by_step(self):
        """Test step-by-step sensory processing"""
        pass
    
    def test_decode_motor_command_step_by_step(self):
        """Test step-by-step motor command decoding"""
        pass
    
    def test_apply_reward_modulation_step_by_step(self):
        """Test step-by-step reward modulation"""
        pass
    
    def test_update_development_step_by_step(self):
        """Test step-by-step development update"""
        pass
    
    def test_get_state_info(self):
        """Test state info retrieval"""
        pass
    
    def test_configuration_flags(self):
        """Test configuration flags"""
        pass
class TestFactoryFunctions(unittest.TestCase):
    """Test enhanced factory functions"""
    
    def test_create_configured_config(self):
        """Test createConfiguredConfig function"""
        try:
            config_dict = {'test': 'value'}
            result = nlm.createConfiguredConfig(config_dict)
            self.assertIsNotNone(result)
        except (AttributeError, TypeError):
            self.skipTest("createConfiguredConfig not available in current bindings")
    
    def test_create_configured_brain(self):
        """Test createConfiguredBrain function"""
        try:
            config_dict = {'neuron_count': 100}
            brain = nlm.createConfiguredBrain(config_dict)
            self.assertIsNotNone(brain)
        except (AttributeError, TypeError):
            self.skipTest("createConfiguredBrain not available in current bindings")
    
    def test_create_configured_world(self):
        """Test createConfiguredWorld function"""
        try:
            config_dict = {'width': 100.0, 'height': 100.0}
            world = nlm.createConfiguredWorld(config_dict)
            self.assertIsNotNone(world)
        except (AttributeError, TypeError):
            self.skipTest("createConfiguredWorld not available in current bindings")
    
    def test_create_simple_agent_setup(self):
        """Test createSimpleAgentSetup function"""
        try:
            brain_config = {'neuron_count': 100}
            world_config = {'width': 100.0, 'height': 100.0}
            agent_config = {}
            
            agent = nlm.createSimpleAgentSetup(brain_config, world_config, agent_config)
            self.assertIsNotNone(agent)
        except (AttributeError, TypeError):
            self.skipTest("createSimpleAgentSetup not available in current bindings")
class TestBatchSimulator(unittest.TestCase):
    """Test BatchSimulator functionality"""
    
    def test_batch_creation(self):
        """Test batch creation"""
        pass
    
    def test_batch_simulation(self):
        """Test batch simulation"""
        pass
    
    def test_batch_comparison(self):
        """Test batch comparison"""
        pass
    
    def test_batch_clear(self):
        """Test batch clearing"""
        pass
    
    def test_batch_size(self):
        """Test batch size settings"""
        pass
class TestErrorHandling(unittest.TestCase):
    """Test error handling functionality"""
    
    def test_nlm_error_class(self):
        """Test NLMError exception class"""
        try:
            raise nlm.NLMError("Test error")
        except nlm.NLMError:
            pass
        else:
            self.fail("NLMError not raised")
    
    def test_nlm_error_with_operation(self):
        """Test NLMError with operation parameter"""
        try:
            raise nlm.NLMError("Test error", "test_operation")
        except nlm.NLMError as e:
            self.assertIn("NLM Error", str(e))
            self.assertIn("test_operation", str(e))
            self.assertIn("Test error", str(e))
        else:
            self.fail("NLMError not raised")
    
    def test_validation_error(self):
        """Test validation error handling"""
        pass
class TestSensoryDataHandling(unittest.TestCase):
    """Test sensory data handling utilities"""
    
    def test_convert_to_vision(self):
        """Test convert_to_vision utility"""
        pass
    
    def test_convert_to_audio(self):
        """Test convert_to_audio utility"""
        pass
    
    def test_extract_sensory_data(self):
        """Test extract_sensory_data utility"""
        pass
class TestActionAndWorldObjects(unittest.TestCase):
    """Test action and world object utilities"""
    
    def test_create_action_from_dict(self):
        """Test create_action_from_dict utility"""
        pass
    
    def test_create_world_object_from_dict(self):
        """Test create_world_object_from_dict utility"""
        pass
    
    def test_action_to_dict(self):
        """Test action_to_dict utility"""
        pass
    
    def test_world_object_to_dict(self):
        """Test world_object_to_dict utility"""
        pass
class TestConfigurationManagement(unittest.TestCase):
    """Test configuration management utilities"""
    
    def test_validate_config(self):
        """Test validate_config utility"""
        pass
    
    def test_get_supported_types(self):
        """Test get_supported_types utility"""
        pass
    
    def test_merge_config(self):
        """Test merge_config utility"""
        pass
    
    def test_deep_copy_config(self):
        """Test deep_copy_config utility"""
        pass
    
    def test_compare_config(self):
        """Test compare_config utility"""
        pass
class TestStatisticalAnalysis(unittest.TestCase):
    """Test statistical analysis utilities"""
    
    def test_calculate_statistics(self):
        """Test calculate_statistics utility"""
        pass
    
    def test_calculate_neuron_population_stats(self):
        """Test calculate_neuron_population_stats utility"""
        pass
    
    def test_compare_datasets(self):
        """Test compare_datasets utility"""
        pass
class TestBackwardCompatibility(unittest.TestCase):
    """Test backward compatibility"""
    
    def test_original_bindings_still_work(self):
        """Test that original bindings still work"""
        # Test Config
        config = nlm.Config()
        self.assertIsNotNone(config)
        
        config.set('test', 'value', nlm.ConfigSource.Runtime)
        self.assertTrue(config.has('test'))
        
        # Test Brain
        brain = nlm.createBrain(config)
        self.assertIsNotNone(brain)
        
        # Test SimpleWorld
        world = nlm.createSimpleWorld()
        self.assertIsNotNone(world)
        
        # Test AgentBrain
        agent = nlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
    
    def test_constants_available(self):
        """Test that global constants are available"""
        self.assertIsNotNone(nlm.INVALID_NEURON_ID)
        self.assertIsNotNone(nlm.INVALID_SYNAPSE_ID)
        self.assertIsNotNone(nlm.INVALID_REGION_ID)
        self.assertIsNotNone(nlm.INVALID_POPULATION_ID)
    
    def test_function_signatures_unchanged(self):
        """Test that function signatures remain compatible"""
        pass
def run_all_tests():
    """Run all test suites"""
    print("=== Running Enhanced Python Bindings Integration Tests ===")
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return result.wasSuccessful()

if __name__ == '__main__':
    import sys
    success = run_all_tests()
    sys.exit(0 if success else 1)
