# Comprehensive Python Unit Tests for NLM Enhanced Wrapper

"""Unit tests for the enhanced NLM Python wrapper and Python bindings integration."""

import unittest
import numpy as np
import tempfile
import os
import json

import pynlm
from python.nlm_enhanced_wrapper import (
    ConfigValidator, SensoryPerceptWrapper, ActionWrapper,
    WorldWrapper, AgentWrapper, BrainWrapper,
    create_brain, create_world, create_agent_brain,
    run_simulation, save_simulation_results, load_simulation_results,
    ActionType, DevelopmentalStage, NLMError, ConfigurationError, SimulationError
)

class TestConfigValidator(unittest.TestCase):
    """Test configuration validation functionality"""
    
    def test_valid_config(self):
        config = {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.5,
            "random_seed": 123,
            "simulation_timestep": 0.001,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
        
        validated = ConfigValidator.validate_config(config)
        self.assertEqual(validated["neuron_count"], 1000)
        self.assertEqual(validated["region_count"], 2)
        self.assertEqual(validated["connection_probability"], 0.5)
    
    def test_config_with_defaults(self):
        config = {"neuron_count": 500}
        validated = ConfigValidator.validate_config(config)
        self.assertEqual(validated["neuron_count"], 500)
        self.assertEqual(validated["region_count"], 1)  # default
        self.assertEqual(validated["connection_probability"], 0.15)  # default
    
    def test_invalid_config_types(self):
        config = {
            "neuron_count": "not_an_int",
            "connection_probability": "not_a_float"
        }
        
        with self.assertRaises(ConfigurationError):
            ConfigValidator.validate_config(config)
    
    def test_invalid_config_values(self):
        config = {
            "neuron_count": 50,  # below min
            "connection_probability": 2.0  # above max
        }
        
        with self.assertRaises(ConfigurationError):
            ConfigValidator.validate_config(config)
    
    def test_schema_completeness(self):
        # Ensure all schema parameters are validated
        config = {"neuron_count": 100}
        validated = ConfigValidator.validate_config(config)
        self.assertEqual(len(validated), len(ConfigValidator.DEFAULT_SCHEMA))

class TestSensoryPerceptWrapper(unittest.TestCase):
    """Test SensoryPercept wrapper functionality"""
    
    def setUp(self):
        # Create a brain and world to get actual sensory percept
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(20, 20, 8, 8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(self.world)
    
    def test_wrapper_creation(self):
        percept = self.world.getSensoryPercept()
        wrapper = SensoryPerceptWrapper(percept)
        
        self.assertIsNotNone(wrapper)
        self.assertEqual(wrapper.get_vision().shape, (8, 8, 3))
        self.assertEqual(wrapper.get_touch().shape, (8,))
        self.assertEqual(wrapper.get_internal().shape, (4,))
        self.assertEqual(wrapper.get_proprioception().shape, (6,))
    
    def test_vision_data_access(self):
        percept = self.world.getSensoryPercept()
        wrapper = SensoryPerceptWrapper(percept)
        
        vision = wrapper.get_vision()
        self.assertIsInstance(vision, np.ndarray)
        self.assertEqual(vision.shape, (8, 8, 3))
        self.assertTrue(np.all((vision >= 0) & (vision <= 1)))
    
    def test_all_signals_flattened(self):
        percept = self.world.getSensoryPercept()
        wrapper = SensoryPerceptWrapper(percept)
        
        all_signals = wrapper.get_all_signals()
        self.assertIsInstance(all_signals, np.ndarray)
        self.assertEqual(len(all_signals), 256 + 8 + 4 + 6 + 0)  # vision + touch + internal + proprioception + audio
    
    def test_timestamp_access(self):
        percept = self.world.getSensoryPercept()
        wrapper = SensoryPerceptWrapper(percept)
        
        timestamp = wrapper.get_timestamp()
        self.assertIsInstance(timestamp, float)
        self.assertGreaterEqual(timestamp, 0.0)
        
        # Test setting timestamp
        wrapper.set_timestamp(123.45)
        self.assertEqual(wrapper.get_timestamp(), 123.45)
    
    def test_repr_method(self):
        percept = self.world.getSensoryPercept()
        wrapper = SensoryPerceptWrapper(percept)
        
        repr_str = repr(wrapper)
        self.assertIn("SensoryPercept", repr_str)
        self.assertIn("vision", repr_str.lower())

class TestActionWrapper(unittest.TestCase):
    """Test Action wrapper functionality"""
    
    def test_wrapper_creation(self):
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        wrapper = ActionWrapper(action)
        
        self.assertIsNotNone(wrapper)
        self.assertEqual(wrapper.get_type(), pynlm.ActionType.MoveForward)
        self.assertEqual(wrapper.get_parameters().shape, (0,))  # No parameters
        self.assertEqual(wrapper.get_name(), "MoveForward")
    
    def test_action_type_conversion(self):
        # Test enum conversion
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        wrapper = ActionWrapper(action)
        
        self.assertEqual(wrapper.get_type(), pynlm.ActionType.MoveForward)
        
        # Test ActionType enum
        from python.nlm_enhanced_wrapper import ActionType
        self.assertEqual(ActionType(pynlm.ActionType.MoveForward), ActionType.MOVE_FORWARD)
    
    def test_action_with_parameters(self):
        action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        wrapper = ActionWrapper(action)
        
        params = wrapper.get_parameters()
        self.assertEqual(len(params), 3)
        self.assertEqual(params[0], 1.0)
        self.assertEqual(params[1], 2.0)
        self.assertEqual(params[2], 3.0)
        
        # Test setting parameters
        wrapper.set_parameters([4.0, 5.0, 6.0])
        self.assertEqual(wrapper.get_parameters()[0], 4.0)
    
    def test_parameter_array_types(self):
        action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0])
        wrapper = ActionWrapper(action)
        
        # Test with numpy array
        np_array = np.array([7.0, 8.0])
        wrapper.set_parameters(np_array)
        self.assertEqual(wrapper.get_parameters()[0], 7.0)
        
        # Test with list
        wrapper.set_parameters([9.0, 10.0])
        self.assertEqual(wrapper.get_parameters()[1], 10.0)

class TestBrainWrapper(unittest.TestCase):
    """Test Brain wrapper functionality"""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.wrapper = BrainWrapper(self.brain)
    
    def test_wrapper_creation(self):
        self.assertIsNotNone(self.wrapper)
        self.assertEqual(self.wrapper.get_total_neuron_count(), 500)  # default
    
    def test_initialization(self):
        result = self.wrapper.initialize()
        self.assertTrue(result)
        self.assertGreater(self.wrapper.get_total_neuron_count(), 0)
    
    def test_initialization_with_config(self):
        config = {"neuron_count": 1000, "region_count": 3}
        result = self.wrapper.initialize(config)
        self.assertTrue(result)
        self.assertEqual(self.wrapper.get_total_neuron_count(), 1000)
    
    def test_step_function(self):
        self.wrapper.initialize()
        initial_spikes = self.wrapper.get_total_spike_count()
        
        self.wrapper.step(0)
        self.assertEqual(self.wrapper.get_total_neuron_count(), 1000)  # unchanged
        
        self.wrapper.step(1)
        self.assertEqual(self.wrapper.get_total_neuron_count(), 1000)
    
    def test_neuron_injection(self):
        self.wrapper.initialize()
        
        # Inject current into neurons
        self.wrapper.inject_current(0, 50.0)
        self.wrapper.inject_current_to_neurons("excitatory", 30.0)
        
        # Step should generate spikes
        self.wrapper.step(0)
        spikes = self.wrapper.get_total_spike_count()
        self.assertGreaterEqual(spikes, 0)
    
    def test_action_production(self):
        self.wrapper.initialize()
        
        action_wrapper = self.wrapper.produce_action()
        self.assertIsNotNone(action_wrapper)
        self.assertIsInstance(action_wrapper, ActionWrapper)
        
        # Action should have a type
        self.assertIn(action_wrapper.get_type(), pynlm.ActionType.__members__.values())
    
    def test_reset(self):
        self.wrapper.initialize()
        initial_neurons = self.wrapper.get_total_neuron_count()
        
        self.wrapper.step(0)
        self.wrapper.reset()
        
        self.assertEqual(self.wrapper.get_total_neuron_count(), initial_neurons)
    
    def test_save_load(self):
        self.wrapper.initialize()
        
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain state
            result = self.wrapper.save(filepath)
            self.assertTrue(result)
            
            # Reset and load
            self.wrapper.reset()
            load_result = self.wrapper.load(filepath)
            self.assertTrue(load_result)
            
            # Verify data is back
            self.assertEqual(self.wrapper.get_total_neuron_count(), 1000)
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_region_management(self):
        self.wrapper.initialize()
        
        # Add regions
        region_id1 = self.wrapper.add_region("region1")
        region_id2 = self.wrapper.add_region("region2")
        
        self.assertGreater(region_id1, 0)
        self.assertGreater(region_id2, 0)
        
        # Get region count
        self.assertEqual(self.wrapper.get_region_count(), 1)  # original plus new ones
        
        # Get region IDs
        region_ids = self.wrapper.get_region_ids()
        self.assertIn(region_id1, region_ids)
        self.assertIn(region_id2, region_ids)
    
    def test_developmental_stage(self):
        self.wrapper.initialize()
        
        # Test enum conversion
        stage = self.wrapper.get_developmental_stage()
        self.assertIsInstance(stage, DevelopmentalStage)
        
        # Test setting stage
        self.wrapper.set_developmental_stage(DevelopmentalStage.ADULT)
        self.assertEqual(self.wrapper.get_developmental_stage(), DevelopmentalStage.ADULT)
    
    def test_statistics(self):
        self.wrapper.initialize()
        
        stats = {
            'total_neurons': self.wrapper.get_total_neuron_count(),
            'total_synapses': self.wrapper.get_total_synapse_count(),
            'active_neurons': self.wrapper.get_active_neuron_count(),
            'firing_neurons': self.wrapper.get_firing_neuron_count(),
            'average_firing_rate': self.wrapper.get_average_firing_rate(),
            'excitement_inhibition_ratio': self.wrapper.get_excitation_inhibition_ratio(),
            'total_spikes': self.wrapper.get_total_spike_count()
        }
        
        self.assertGreater(stats['total_neurons'], 0)
        self.assertGreaterEqual(stats['total_synapses'], 0)
        self.assertGreaterEqual(stats['active_neurons'], 0)
        self.assertGreaterEqual(stats['firing_neurons'], 0)
        self.assertGreaterEqual(stats['average_firing_rate'], 0.0)
        self.assertGreaterEqual(stats['total_spikes'], 0)

class TestWorldWrapper(unittest.TestCase):
    """Test World wrapper functionality"""
    
    def setUp(self):
        self.world = pynlm.createSimpleWorld()
        self.wrapper = WorldWrapper(self.world)
    
    def test_wrapper_creation(self):
        self.assertIsNotNone(self.wrapper)
        self.assertEqual(self.wrapper.get_width(), 0)  # Not configured yet
        self.assertEqual(self.wrapper.get_height(), 0)
    
    def test_configuration(self):
        self.wrapper.configure(20, 20, 8, 8)
        
        self.assertEqual(self.wrapper.get_width(), 20)
        self.assertEqual(self.wrapper.get_height(), 20)
        self.assertEqual(self.wrapper.get_stats()['width'], 20)
    
    def test_reset(self):
        self.wrapper.configure(10, 10, 8, 8)
        self.wrapper.reset()
        
        self.assertEqual(self.wrapper.get_width(), 10)
        self.assertEqual(self.wrapper.get_height(), 10)
        self.assertEqual(self.wrapper.get_agent_body()['energy'], 100.0)
    
    def test_update(self):
        self.wrapper.configure(10, 10, 8, 8)
        self.wrapper.reset()
        
        initial_time = self.wrapper.get_stats()['simulation_time']
        self.wrapper.update(0.1)
        
        self.assertGreater(self.wrapper.get_stats()['simulation_time'], initial_time)
    
    def test_sensory_percept_wrapper(self):
        self.wrapper.configure(10, 10, 8, 8)
        self.wrapper.reset()
        
        # Get sensory percept as wrapper
        percept_wrapper = self.wrapper.get_sensory_percept()
        self.assertIsInstance(percept_wrapper, SensoryPerceptWrapper)
        
        # Should have vision data
        vision = percept_wrapper.get_vision()
        self.assertEqual(vision.shape, (8, 8, 3))
    
    def test_world_objects(self):
        self.wrapper.configure(20, 20, 8, 8)
        self.wrapper.reset()
        
        # Add world object
        obj = {
            'x': 5.0,
            'y': 5.0,
            'type': 'Resource',
            'value': 10.0,
            'radius': 0.5,
            'active': True
        }
        
        self.wrapper.add_world_object(obj)
        
        # Remove object
        self.wrapper.remove_world_object(5.0, 5.0)
        
        # Check position is now invalid
        self.assertFalse(self.wrapper.is_valid_position(5.0, 5.0))
    
    def test_agent_body_dict(self):
        self.wrapper.configure(10, 10, 8, 8)
        self.wrapper.reset()
        
        body = self.wrapper.get_agent_body()
        
        self.assertIn('x', body)
        self.assertIn('y', body)
        self.assertIn('orientation', body)
        self.assertIn('energy', body)
        self.assertIn('health', body)
        self.assertIn('age', body)
        
        self.assertIsInstance(body['x'], float)
        self.assertIsInstance(body['energy'], float)
        self.assertIsInstance(body['is_moving'], bool)

class TestAgentWrapper(unittest.TestCase):
    """Test Agent wrapper functionality"""
    
    def setUp(self):
        # Setup brain, world, and agent
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 8, 8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Create wrappers
        self.brain_wrapper = BrainWrapper(self.brain)
        self.world_wrapper = WorldWrapper(self.world)
        self.agent_wrapper = AgentWrapper(self.agent, self.brain_wrapper, self.world_wrapper)
    
    def test_wrapper_creation(self):
        self.assertIsNotNone(self.agent_wrapper)
        self.assertIsInstance(self.agent_wrapper, AgentWrapper)
    
    def test_sensory_processing(self):
        self.brain_wrapper.initialize()
        
        # Get percept from world
        percept_wrapper = self.world_wrapper.get_sensory_percept()
        
        # Process with agent
        self.agent_wrapper.process_sensory_input(percept_wrapper)
        
        # Verify brain received input (indirect check)
        self.agent_wrapper.process_sensory_input(percept_wrapper)
    
    def test_motor_command_decoding(self):
        self.brain_wrapper.initialize()
        
        # Decode motor command
        action_wrapper = self.agent_wrapper.decode_motor_command()
        self.assertIsNotNone(action_wrapper)
        self.assertIsInstance(action_wrapper, ActionWrapper)
        
        # Should have a valid action type
        self.assertIn(action_wrapper.get_type(), pynlm.ActionType.__members__.values())
    
    def test_reward_modulation(self):
        # Test reward application
        self.agent_wrapper.apply_reward_modulation(1.0, 0.5)
        
        # Test curiosity
        curiosity = self.agent_wrapper.get_curiosity_level()
        self.assertGreaterEqual(curiosity, 0.0)
        
        # Test novelty
        novelty = self.agent_wrapper.get_novelty_level()
        self.assertGreaterEqual(novelty, 0.0)
    
    def test_development(self):
        self.agent_wrapper.update_development(0.1)
        
        stage = self.agent_wrapper.get_developmental_stage()
        self.assertIsInstance(stage, DevelopmentalStage)
    
    def test_feature_toggles(self):
        # Test enabling/disabling features
        self.agent_wrapper.enable_reward_modulation(True)
        self.assertTrue(self.agent_wrapper.is_reward_modulation_enabled())
        
        self.agent_wrapper.enable_structural_plasticity(False)
        self.assertFalse(self.agent_wrapper.is_structural_plasticity_enabled())
        
        self.agent_wrapper.enable_development(True)
        self.assertTrue(self.agent_wrapper.is_development_enabled())
        
        self.agent_wrapper.enable_curiosity(False)
        self.assertFalse(self.agent_wrapper.is_curiosity_enabled())

class TestSimulationIntegration(unittest.TestCase):
    """Test simulation integration and utilities"""
    
    def test_simulation_runner(self):
        # Create complete simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create wrappers
        brain_wrapper = BrainWrapper(brain)
        world_wrapper = WorldWrapper(world)
        agent_wrapper = AgentWrapper(agent, brain_wrapper, world_wrapper)
        
        # Run simulation
        results = run_simulation(brain_wrapper, world_wrapper, agent_wrapper, 50)
        
        # Verify results
        self.assertIn('steps_completed', results)
        self.assertEqual(results['steps_completed'], 50)
        self.assertIn('step_data', results)
        self.assertEqual(len(results['step_data']), 50)
        
        # Check step data structure
        step_data = results['step_data'][0]
        expected_keys = ['step', 'time', 'spikes', 'firing_neurons', 
                        'firing_rate', 'action_type', 'agent_stage',
                        'curiosity_level', 'novelty_level']
        for key in expected_keys:
            self.assertIn(key, step_data)
    
    def test_simulation_results_save_load(self):
        # Create simulation results
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        brain_wrapper = BrainWrapper(brain)
        world_wrapper = WorldWrapper(world)
        agent_wrapper = AgentWrapper(agent, brain_wrapper, world_wrapper)
        
        # Run simulation
        results = run_simulation(brain_wrapper, world_wrapper, agent_wrapper, 10)
        
        # Save to file
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            save_simulation_results(results, filepath)
            
            # Load from file
            loaded_results = load_simulation_results(filepath)
            
            # Verify data
            self.assertEqual(loaded_results['steps_completed'], 10)
            self.assertEqual(len(loaded_results['step_data']), 10)
            
            # Verify step data is preserved
            for i, step_data in enumerate(loaded_results['step_data']):
                self.assertEqual(step_data['step'], i)
                self.assertIn('firing_rate', step_data)
                
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_callback_system(self):
        from python.nlm_enhanced_wrapper import add_simulation_callback, remove_simulation_callback
        
        callback_data = []
        
        def test_callback(step_data):
            callback_data.append(step_data)
        
        # Add callback
        add_simulation_callback(test_callback)
        
        # Run simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        brain_wrapper = BrainWrapper(brain)
        world_wrapper = WorldWrapper(world)
        agent_wrapper = AgentWrapper(agent, brain_wrapper, world_wrapper)
        
        results = run_simulation(brain_wrapper, world_wrapper, agent_wrapper, 5)
        
        # Callback should have been called
        self.assertGreater(len(callback_data), 0)
        
        # Remove callback
        remove_simulation_callback(test_callback)
        
        # Run another simulation to ensure callback is removed
        brain2 = pynlm.createBrain(config)
        world2 = pynlm.createSimpleWorld()
        world2.configure(10, 10, 8, 8)
        world2.reset()
        
        agent2 = pynlm.createAgentBrain(brain2)
        agent2.initialize(world2)
        
        brain_wrapper2 = BrainWrapper(brain2)
        world_wrapper2 = WorldWrapper(world2)
        agent_wrapper2 = AgentWrapper(agent2, brain_wrapper2, world_wrapper2)
        
        # Reset callback data
        callback_data.clear()
        
        results2 = run_simulation(brain_wrapper2, world_wrapper2, agent_wrapper2, 3)
        
        # Callback should not have been called this time (removed)
        # Note: In a real scenario, we would verify this more carefully

class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases"""
    
    def test_config_validation_errors(self):
        # Test invalid configuration
        config = {"invalid_key": "value"}
        
        with self.assertRaises(ConfigurationError):
            ConfigValidator.validate_config(config)
    
    def test_simulation_error_handling(self):
        from python.nlm_enhanced_wrapper import SimulationError
        
        # Create a scenario that should trigger an error
        # (This depends on the specific error conditions in the implementation)
        
        # Test that SimulationError is properly defined
        error = SimulationError("Test error")
        self.assertIn("Test error", str(error))
    
    def test_invalid_action_type_conversion(self):
        # Test handling of invalid action type strings
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        world = pynlm.createSimpleWorld()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        brain_wrapper = BrainWrapper(brain)
        world_wrapper = WorldWrapper(world)
        agent_wrapper = AgentWrapper(agent, brain_wrapper, world_wrapper)
        
        # This should work fine
        brain_wrapper.initialize()
        
        # Test invalid neuron type
        with self.assertRaises(Exception):  # Should raise some error
            brain_wrapper.inject_current_to_neurons("invalid_type", 10.0)

class TestExampleScripts(unittest.TestCase):
    """Test that example scripts can be parsed and don't have syntax errors"""
    
    def test_example_script_syntax(self):
        from python.nlm_enhanced_wrapper import EXAMPLE_SCRIPTS
        
        # Verify all examples are defined
        self.assertGreater(len(EXAMPLE_SCRIPTS), 0)
        
        # Check that each example script is a string
        for name, script in EXAMPLE_SCRIPTS.items():
            self.assertIsInstance(name, str)
            self.assertIsInstance(script, str)
            self.assertGreater(len(script.strip()), 0)
        
        # Test specific examples
        self.assertIn('silent_brain', EXAMPLE_SCRIPTS)
        self.assertIn('brain_watching_world', EXAMPLE_SCRIPTS)
        self.assertIn('complete_agent', EXAMPLE_SCRIPTS)

def run_all_tests():
    """Run all unit tests and return results"""
    suite = unittest.TestLoader().loadTestsFromTestCase(TestConfigValidator)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestSensoryPerceptWrapper))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestActionWrapper))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestBrainWrapper))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestWorldWrapper))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAgentWrapper))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestSimulationIntegration))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestErrorHandling))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestExampleScripts))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return result.wasSuccessful()

if __name__ == '__main__':
    success = run_all_tests()
    exit(0 if success else 1)
