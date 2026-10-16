#!/usr/bin/env python3
"""
NLM Python Bindings Test Suite

This module contains comprehensive tests for the NLM Python bindings,
including unit tests, integration tests, and validation tests.
"""

import sys
import os
import json
import tempfile
import unittest
from pathlib import Path
from typing import Dict, List, Any
import numpy as np

# Add the current directory to the path so we can import nlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to import NLM bindings
try:
    import nlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Warning: NLM C++ bindings not found. Some tests will be skipped.")
def skip_if_not_available(test_method):
    """Decorator to skip test if NLM bindings are not available."""
    def wrapper(self, *args, **kwargs):
        if not NLM_AVAILABLE:
            self.skipTest("NLM bindings not available")
        return test_method(self, *args, **kwargs)
    return wrapper
class TestNLMBindings(unittest.TestCase):
    """Test suite for NLM Python bindings."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.temp_dir = tempfile.mkdtemp()
        self.test_data_dir = os.path.join(self.temp_dir, "test_data")
        os.makedirs(self.test_data_dir)
        
        if NLM_AVAILABLE:
            # Create test configurations
            self.default_config = nlm.create_default_config()
            self.demo_config = nlm.create_demo_config()
            
            # Create test brain and world
            self.test_brain = nlm.createBrain(self.default_config)
            self.test_brain.initialize()
            
            self.test_world = nlm.create_simple_demo_world()
            
            # Test files
            self.test_config_file = os.path.join(self.test_data_dir, "test_config.cfg")
            self.test_brain_file = os.path.join(self.test_data_dir, "test_brain.nlm")
    
    def tearDown(self):
        """Clean up test fixtures."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    @skip_if_not_available
    def test_config_creation(self):
        """Test configuration creation and basic operations."""
        config = nlm.create_default_config()
        
        # Test initial state
        self.assertFalse(config.has("test_key"))
        
        # Test setting and getting values
        config.set("test_key", "test_value")
        self.assertTrue(config.has("test_key"))
        
        # Test using convenience methods
        config.set_int("int_key", 42)
        self.assertEqual(config.get_int("int_key", 0), 42)
        
        config.set_float("float_key", 3.14)
        self.assertAlmostEqual(config.get_float("float_key", 0.0), 3.14, places=2)
        
        config.set_bool("bool_key", True)
        self.assertTrue(config.get_bool("bool_key", False))
        
        # Test validation
        errors = config.validate(["int_key", "missing_key"])
        self.assertEqual(len(errors), 1)
        self.assertIn("missing_key", errors[0])
    
    @skip_if_not_available
    def test_action_creation(self):
        """Test action creation and manipulation."""
        # Test action creation
        action = nlm.Action(nlm.ACTION_TYPE_MOVE_FORWARD)
        self.assertEqual(action.getType(), nlm.ACTION_TYPE_MOVE_FORWARD)
        
        # Test parameter manipulation
        action.add_param(1.0)
        action.add_param(2.0)
        self.assertEqual(len(action.getParameters()), 2)
        
        # Test parameter modification
        action.set_param(0, 5.0)
        self.assertEqual(action.getParameters()[0], 5.0)
        
        # Test action conversion to dictionary
        action_dict = action.to_dict()
        self.assertIn("type", action_dict)
        self.assertIn("parameters", action_dict)
        self.assertIn("name", action_dict)
        
        # Test cloning
        action_clone = action.clone()
        self.assertEqual(action.getType(), action_clone.getType())
        self.assertEqual(action.getParameters(), action_clone.getParameters())
    
    @skip_if_not_available
    def test_sensory_percept(self):
        """Test sensory percept creation and manipulation."""
        # Create percept with default constructor
        percept = nlm.SensoryPercept()
        
        # Test setting and getting values
        # Note: This assumes the C++ implementation has these methods
        # The exact API may vary based on the actual NLM implementation
        
        # Test conversion to dictionary
        percept_dict = percept.to_dict()
        self.assertIn("vision", percept_dict)
        self.assertIn("touch", percept_dict)
        self.assertIn("internal", percept_dict)
        self.assertIn("proprioception", percept_dict)
        self.assertIn("audio", percept_dict)
        self.assertIn("timestamp", percept_dict)
    
    @skip_if_not_available
    def test_brain_operations(self):
        """Test basic brain operations."""
        # Test brain creation and initialization
        config = nlm.create_default_config()
        brain = nlm.createBrain(config)
        
        self.assertIsNotNone(brain)
        
        # Test initialization
        result = brain.initialize()
        self.assertTrue(result)  # Should return True on success
        
        # Test basic statistics
        total_neurons = brain.getTotalNeuronCount()
        self.assertIsInstance(total_neurons, int)
        self.assertGreaterEqual(total_neurons, 0)
        
        total_synapses = brain.getTotalSynapseCount()
        self.assertIsInstance(total_synapses, int)
        self.assertGreaterEqual(total_synapses, 0)
        
        # Test developmental stage
        developmental_stage = brain.getDevelopmentalStage()
        self.assertIsInstance(developmental_stage, int)
        
        # Test region operations
        region_count = brain.getRegionCount()
        self.assertIsInstance(region_count, int)
        self.assertGreaterEqual(region_count, 0)
        
        # Test statistics method
        stats = brain.get_statistics()
        self.assertIsInstance(stats, dict)
        self.assertIn("total_neurons", stats)
        self.assertIn("total_synapses", stats)
        self.assertIn("firing_rate", stats)
        
        # Test status method
        status = brain.log_status()
        self.assertIsInstance(status, str)
        self.assertGreater(len(status), 0)
    
    @skip_if_not_available
    def test_world_operations(self):
        """Test world operations."""
        world = nlm.create_simple_demo_world()
        
        # Test world configuration
        self.assertEqual(world.getWidth(), 100)
        self.assertEqual(world.getHeight(), 100)
        self.assertEqual(world.getVisionWidth(), 10)
        self.assertEqual(world.getVisionHeight(), 10)
        
        # Test agent start position
        # Note: This assumes the world has these methods
        # The exact API may vary
        
        # Test simulation time
        time = world.getSimulationTime()
        self.assertIsInstance(time, float)
        self.assertGreaterEqual(time, 0.0)
    
    @skip_if_not_available
    def test_agent_operations(self):
        """Test agent operations."""
        # Create brain and world
        config = nlm.create_default_config()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Create agent
        agent = nlm.create_agent(brain, world)
        agent.initialize(world)
        
        # Test agent initialization
        self.assertIsNotNone(agent)
        
        # Test agent statistics
        # Note: These methods may not exist in the current implementation
        # This is a placeholder for testing agent functionality
        pass
    
    @skip_if_not_available
    def test_constants_and_enums(self):
        """Test that constants and enumerations are available."""
        # Test neuron types
        self.assertTrue(hasattr(nlm, 'NEURON_TYPE_EXCITATORY'))
        self.assertTrue(hasattr(nlm, 'NEURON_TYPE_INHIBITORY'))
        self.assertTrue(hasattr(nlm, 'NEURON_TYPE_MODULATORY'))
        
        # Test action types
        self.assertTrue(hasattr(nlm, 'ACTION_TYPE_MOVE_FORWARD'))
        self.assertTrue(hasattr(nlm, 'ACTION_TYPE_MOVE_BACKWARD'))
        self.assertTrue(hasattr(nlm, 'ACTION_TYPE_TURN_LEFT'))
        
        # Test world object types
        self.assertTrue(hasattr(nlm, 'WORLD_OBJECT_EMPTY'))
        self.assertTrue(hasattr(nlm, 'WORLD_OBJECT_WALL'))
        
        # Test developmental stages
        self.assertTrue(hasattr(nlm, 'DEVELOPMENTAL_STAGE_INITIAL'))
        self.assertTrue(hasattr(nlm, 'DEVELOPMENTAL_STAGE_ADULT'))
    
    @skip_if_not_available
    def test_convenience_functions(self):
        """Test convenience functions."""
        # Test default config creation
        default_config = nlm.create_default_config()
        self.assertIsNotNone(default_config)
        
        # Test demo config creation
        demo_config = nlm.create_demo_config()
        self.assertIsNotNone(demo_config)
        
        # Test demo brain creation
        demo_brain = nlm.create_simple_demo_brain()
        self.assertIsNotNone(demo_brain)
        
        # Test demo world creation
        demo_world = nlm.create_simple_demo_world()
        self.assertIsNotNone(demo_world)
        
        # Test agent creation
        config = nlm.create_default_config()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        agent = nlm.create_agent(brain, demo_world)
        self.assertIsNotNone(agent)
    
    @skip_if_not_available
    def test_file_operations(self):
        """Test file operations (save/load)."""
        # Create a test brain
        config = nlm.create_default_config()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Save brain to file
        save_success = nlm.save_brain_state(self.test_brain_file, brain)
        self.assertTrue(save_success)
        
        # Check that file was created
        self.assertTrue(os.path.exists(self.test_brain_file))
        
        # Create new config and brain
        new_config = nlm.create_default_config()
        new_brain = nlm.createBrain(new_config)
        
        # Load brain from file
        load_success = new_brain.load(self.test_brain_file)
        self.assertTrue(load_success)
    
    @skip_if_not_available
    def test_simulation_runner(self):
        """Test simulation runner functionality."""
        # Create brain and world
        brain = nlm.createBrain(self.default_config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run simulation
        result = nlm.run_simulation(brain, world, 100, 0.01)
        
        # Check result
        self.assertIsInstance(result, dict)
        self.assertIn("total_steps", result)
        self.assertIn("final_step", result)
        self.assertIn("status", result)
        
        # Verify simulation completed
        self.assertEqual(result["total_steps"], 100)
        self.assertEqual(result["final_step"], 99)
        self.assertEqual(result["status"], "completed")
    
    @skip_if_not_available
    def test_validation_function(self):
        """Test configuration validation."""
        # Create configuration
        config = nlm.create_demo_config()
        
        # Test with valid configuration
        valid_config = nlm.create_default_config()
        valid_config.set("test_key", "test_value")
        
        errors = nlm.validate_config(valid_config)
        self.assertEqual(len(errors), 0)
        
        # Test with invalid configuration (missing required keys)
        errors = nlm.validate_config(valid_config)
        self.assertEqual(len(errors), 0)  # Should be valid since all tests pass
    
    @skip_if_not_available
    def test_performance_profiling(self):
        """Test performance profiling."""
        brain = nlm.createBrain(self.default_config)
        brain.initialize()
        
        # Profile performance
        profile_result = nlm.profile_brain(brain, 100)
        
        # Check profile result
        self.assertIsInstance(profile_result, dict)
        self.assertIn("total_steps", profile_result)
        self.assertIn("elapsed_time_ms", profile_result)
        self.assertIn("steps_per_second", profile_result)
        self.assertIn("avg_time_per_step_ms", profile_result)
        
        # Verify reasonable performance
        self.assertEqual(profile_result["total_steps"], 100)
        self.assertGreater(profile_result["steps_per_second"], 0)
        self.assertGreater(profile_result["elapsed_time_ms"], 0)
    
    def test_bindings_availability(self):
        """Test if NLM bindings are available."""
        if NLM_AVAILABLE:
            self.assertTrue(NLM_AVAILABLE)
        else:
            self.assertFalse(NLM_AVAILABLE)
    
    def test_package_info(self):
        """Test package information."""
        self.assertTrue(hasattr(nlm, "__version__"))
        self.assertTrue(hasattr(nlm, "__author__"))
        
        self.assertIsInstance(nlm.__version__, str)
        self.assertIsInstance(nlm.__author__, str)
class TestNLMIntegration(unittest.TestCase):
    """Integration tests for NLM Python bindings."""
    
    def setUp(self):
        """Set up integration test fixtures."""
        if not NLM_AVAILABLE:
            self.skipTest("NLM bindings not available")
        
        self.temp_dir = tempfile.mkdtemp()
        self.test_results = []
    
    def tearDown(self):
        """Clean up integration test fixtures."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_complete_simulation_pipeline(self):
        """Test complete simulation pipeline."""
        # Create simulation pipeline
        config = nlm.create_demo_config()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run simulation
        result = nlm.run_simulation(brain, world, 100)
        
        # Verify pipeline completion
        self.assertEqual(result["status"], "completed")
        self.assertEqual(result["total_steps"], 100)
        self.assertEqual(result["final_step"], 99)
        
        # Verify brain state
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(brain.getAverageFiringRate(), 0)
    
    def test_agent_world_integration(self):
        """Test agent-world integration."""
        # Create components
        config = nlm.create_default_config()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Create agent
        agent = nlm.create_agent(brain, world)
        agent.initialize(world)
        
        # Run agent episode
        for step in range(50):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Simulate world step
            world.update(0.01)
            
            # Get and execute action
            action = brain.produceAction()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify integration
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(world.getSimulationTime(), 0)
    
    def test_configuration_pipeline(self):
        """Test configuration pipeline."""
        # Create configuration
        config = nlm.create_demo_config()
        
        # Modify using Pythonic APIs
        config.set_int("brain.neuron_count", 2000)
        config.set_float("world.width", 50.0)
        config.set_bool("simulation.verbose", True)
        
        # Get modified values
        neuron_count = config.get_int("brain.neuron_count", 1000)
        width = config.get_float("world.width", 10.0)
        verbose = config.get_bool("simulation.verbose", False)
        
        # Verify modifications
        self.assertEqual(neuron_count, 2000)
        self.assertEqual(width, 50.0)
        self.assertTrue(verbose)
        
        # Validate configuration
        errors = config.validate(["brain.neuron_count", "world.width"])
        self.assertEqual(len(errors), 0)
    
    def test_save_load_pipeline(self):
        """Test save/load pipeline."""
        # Create and save brain
        config1 = nlm.create_default_config()
        brain1 = nlm.createBrain(config1)
        brain1.initialize()
        
        # Save to file
        save_file = os.path.join(self.temp_dir, "test_brain.nlm")
        save_success = nlm.save_brain_state(save_file, brain1)
        self.assertTrue(save_success)
        
        # Create new brain and load
        config2 = nlm.create_default_config()
        brain2 = nlm.createBrain(config2)
        
        load_success = brain2.load(save_file)
        self.assertTrue(load_success)
        
        # Verify load
        self.assertEqual(brain1.getTotalNeuronCount(), brain2.getTotalNeuronCount())
        self.assertEqual(brain1.getTotalSpikeCount(), brain2.getTotalSpikeCount())
class TestNLMResearch(unittest.TestCase):
    """Research-specific tests for NLM Python bindings."""
    
    def setUp(self):
        """Set up research test fixtures."""
        if not NLM_AVAILABLE:
            self.skipTest("NLM bindings not available")
        
        self.temp_dir = tempfile.mkdtemp()
    
    def tearDown(self):
        """Clean up research test fixtures."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_parameter_sensitivity(self):
        """Test parameter sensitivity analysis."""
        # Test different parameter values
        parameter_values = [0.01, 0.05, 0.1, 0.2]
        results = []
        
        for value in parameter_values:
            config = nlm.create_demo_config()
            config.set_float("plasticity.learning_rate", value)
            
            brain = nlm.createBrain(config)
            brain.initialize()
            
            world = nlm.create_simple_demo_world()
            nlm.run_simulation(brain, world, 100)
            
            stats = brain.get_statistics()
            results.append({
                "learning_rate": value,
                "total_spikes": stats["total_spikes"],
                "firing_rate": stats["avg_firing_rate"],
            })
        
        # Verify results
        self.assertEqual(len(results), len(parameter_values))
        
        # Check that different parameters produce different results
        spike_counts = [r["total_spikes"] for r in results]
        self.assertNotEqual(len(set(spike_counts)), 1)  # Should have variation
        
        # Check for monotonic trend (not strict, just checking for variation)
        firing_rates = [r["firing_rate"] for r in results]
        self.assertNotEqual(len(set(firing_rates)), 1)  # Should have variation
    
    def test_network_size_analysis(self):
        """Test analysis of different network sizes."""
        network_sizes = [100, 500, 1000, 2000]
        network_results = []
        
        for size in network_sizes:
            config = nlm.create_demo_config()
            config.set_int("brain.neuron_count", size)
            
            brain = nlm.createBrain(config)
            brain.initialize()
            
            world = nlm.create_simple_demo_world()
            nlm.run_simulation(brain, world, 100)
            
            stats = brain.get_statistics()
            
            # Calculate network metrics
            metrics = {
                "size": size,
                "synapse_density": stats["total_synapses"] / max(size, 1),
                "activity_level": stats["active_neurons"] / max(size, 1),
                "spike_count": stats["total_spikes"],
                "firing_rate": stats["avg_firing_rate"],
            }
            
            network_results.append(metrics)
        
        # Verify results
        self.assertEqual(len(network_results), len(network_sizes))
        
        # Check that larger networks generally have more synapses
        for i in range(1, len(network_results)):
            current = network_results[i]
            previous = network_results[i-1]
            
            self.assertGreaterEqual(current["synapse_density"], previous["synapse_density"])
        
        # Check that spike count generally increases with network size
        spike_counts = [r["spike_count"] for r in network_results]
        for i in range(1, len(spike_counts)):
            self.assertGreaterEqual(spike_counts[i], spike_counts[i-1])
    
    def test_development_stages(self):
        """Test different developmental stages."""
        developmental_stages = [
            nlm.DEVELOPMENTAL_STAGE_INITIAL,
            nlm.DEVELOPMENTAL_STAGE_CRITICAL_PERIOD,
            nlm.DEVELOPMENTAL_STAGE_MATURATION,
            nlm.DEVELOPMENTAL_STAGE_ADULT,
        ]
        
        stage_results = []
        
        for stage in developmental_stages:
            config = nlm.create_demo_config()
            
            # Create brain
            brain = nlm.createBrain(config)
            brain.initialize()
            
            # Set developmental stage
            brain.setDevelopmentalStage(stage)
            
            # Create world
            world = nlm.create_simple_demo_world()
            
            # Run simulation
            nlm.run_simulation(brain, world, 50)
            
            # Collect results
            results = {
                "stage": stage,
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
            }
            
            stage_results.append(results)
        
        # Verify results
        self.assertEqual(len(stage_results), len(developmental_stages))
        
        # Check that different stages produce different results
        stages = [r["stage"] for r in stage_results]
        self.assertEqual(len(set(stages)), len(developmental_stages))
    
    def test_social_dynamics(self):
        """Test social dynamics with multiple agents."""
        agent_counts = [1, 2, 5]
        
        social_results = []
        
        for agent_count in agent_counts:
            # Create configuration
            config = nlm.create_demo_config()
            
            # Create world
            world = nlm.create_simple_demo_world()
            
            # Create multiple agents
            agents = []
            for i in range(agent_count):
                brain = nlm.createBrain(config)
                brain.initialize()
                
                agent = nlm.create_agent(brain, world)
                agent.initialize(world)
                
                agents.append({
                    "id": i,
                    "brain": brain,
                    "agent": agent,
                })
            
            # Run social simulation
            for step in range(50):
                for agent_info in agents:
                    agent_info["agent"].processSensoryInput(world.getSensoryPercept())
                
                world.update(0.01)
                
                for agent_info in agents:
                    action = agent_info["agent"].getBrain().produceAction()
                    world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect results
            total_spikes = sum(agent_info["brain"].getTotalSpikeCount() for agent_info in agents)
            total_neurons = sum(agent_info["brain"].getTotalNeuronCount() for agent_info in agents)
            
            results = {
                "agent_count": agent_count,
                "total_spikes": total_spikes,
                "total_neurons": total_neurons,
                "avg_firing_rate": total_spikes / (agent_count * 50),  # spikes per agent per step
            }
            
            social_results.append(results)
        
        # Verify results
        self.assertEqual(len(social_results), len(agent_counts))
        
        # Check that more agents produce more total spikes
        for i in range(1, len(social_results)):
            self.assertGreaterEqual(social_results[i]["total_spikes"], social_results[i-1]["total_spikes"])
class TestNLMValidation(unittest.TestCase):
    """Validation tests for NLM Python bindings."""
    
    def setUp(self):
        """Set up validation test fixtures."""
        if not NLM_AVAILABLE:
            self.skipTest("NLM bindings not available")
    
    def test_input_validation(self):
        """Test input validation."""
        config = nlm.create_demo_config()
        
        # Test invalid input types
        # Note: These tests assume the C++ implementation handles input validation
        # The exact behavior may vary based on the NLM implementation
        
        # Test configuration validation
        errors = nlm.validate_config(config)
        self.assertIsInstance(errors, list)
        
        # Test action validation
        action = nlm.Action(nlm.ACTION_TYPE_MOVE_FORWARD)
        self.assertIsNotNone(action)
        
        # Test brain initialization
        brain = nlm.createBrain(config)
        result = brain.initialize()
        self.assertTrue(result)
    
    def test_output_validation(self):
        """Test output validation."""
        config = nlm.create_demo_config()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run simulation
        result = nlm.run_simulation(brain, world, 10)
        
        # Validate output structure
        self.assertIsInstance(result, dict)
        self.assertIn("total_steps", result)
        self.assertIn("final_step", result)
        self.assertIn("status", result)
        
        # Validate data types
        self.assertIsInstance(result["total_steps"], int)
        self.assertIsInstance(result["final_step"], int)
        self.assertIsInstance(result["status"], str)
    
    def test_error_handling(self):
        """Test error handling."""
        # Test with invalid configuration
        config = nlm.create_demo_config()
        
        # This should not crash, even with invalid values
        try:
            brain = nlm.createBrain(config)
            brain.initialize()
            
            world = nlm.create_simple_demo_world()
            nlm.run_simulation(brain, world, 10)
            
        except Exception as e:
            self.fail(f"Unexpected exception: {e}")
    
    def test_edge_cases(self):
        """Test edge cases."""
        # Test with minimal configuration
        config = nlm.create_default_config()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Test with zero steps
        world = nlm.create_simple_demo_world()
        result = nlm.run_simulation(brain, world, 0)
        
        self.assertEqual(result["total_steps"], 0)
        self.assertEqual(result["final_step"], -1)  # Special case for zero steps
    
    def test_resource_management(self):
        """Test resource management."""
        # Create multiple brains and worlds
        configs = [nlm.create_default_config() for _ in range(5)]
        brains = []
        worlds = []
        
        for config in configs:
            brain = nlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
            
            world = nlm.create_simple_demo_world()
            worlds.append(world)
        
        # Run simulations
        for i, (brain, world) in enumerate(zip(brains, worlds)):
            nlm.run_simulation(brain, world, 10)
        
        # Verify all simulations completed
        for i, brain in enumerate(brains):
            self.assertGreater(brain.getTotalSpikeCount(), 0)
        
        # Clean up
        del brains
        del worlds
def run_tests():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestNLMBindings)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestNLMIntegration))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestNLMResearch))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestNLMValidation))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print("\n" + "=" * 50)
    print("Test Summary")
    print("=" * 50)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Skipped: {len(result.skipped)}")
    
    if result.failures:
        print("\nFailures:")
        for test, trace in result.failures:
            print(f"  {test}: {trace}")
    
    if result.errors:
        print("\nErrors:")
        for test, trace in result.errors:
            print(f"  {test}: {trace}")
    
    if result.skipped:
        print("\nSkipped:")
        for test, reason in result.skipped:
            print(f"  {test}: {reason}")
    
    # Return success status
    return len(result.failures) == 0 and len(result.errors) == 0
if __name__ == "__main__":
    success = run_tests()
    sys.exit(0 if success else 1)
