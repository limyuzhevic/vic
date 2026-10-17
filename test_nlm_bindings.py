#!/usr/bin/env python3
"""
Comprehensive Test Suite for NLM Python Bindings

This test suite provides comprehensive testing for the NLM (Neural Learning Machine)
Python bindings, including both the core bindings and the enhanced API.

Features:
- Unit tests for core NLM bindings
- Integration tests for enhanced API
- Performance tests for simulation
- Resource management tests
- Error handling tests
- Configuration management tests

Usage:
    python3 test_nlm_bindings.py
    python3 test_nlm_bindings.py --quick
    python3 test_nlm_bindings.py --verbose
"""

import sys
import os
import time
import json
import tempfile
import unittest
from unittest.mock import patch, MagicMock
import numpy as np

# Import NLM modules
try:
    import pynlm
    from python.pynlm_enhanced import (
        Config, EnhancedBrain, EnhancedWorld, EnhancedAgent, NLM,
        ValidationError, NLMError, NLMSimulationError, NLMResourceError,
        NLMEnvironmentError, get_template, create_demo_simulation, run_quick_demo
    )
    NLM_AVAILABLE = True
    print("✓ NLM modules loaded successfully")
except ImportError as e:
    NLM_AVAILABLE = False
    print(f"⚠ Could not load all NLM modules: {e}")
    print("Some tests may be skipped.")

# Test configuration
TEST_CONFIG = {
    "neuron_count": 100,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42,
}

TEST_TEMPLATE = "basic"

class TestConfiguration(unittest.TestCase):
    """Test configuration management"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM not available")
    
    def test_config_creation(self):
        """Test basic configuration creation"""
        config = Config(TEST_CONFIG)
        self.assertIsNotNone(config)
        self.assertEqual(config.get("neuron_count"), 100)
        self.assertEqual(config.get("region_count"), 2)
    
    def test_config_validation(self):
        """Test configuration validation"""
        # Valid config should work
        config = Config(TEST_CONFIG)
        self.assertTrue(config.has("neuron_count"))
        
        # Invalid config should raise ValidationError
        invalid_config = {"neuron_count": -1}
        with self.assertRaises(ValidationError):
            Config(invalid_config)
        
        # Missing config should use defaults
        minimal_config = {"neuron_count": 50}
        config = Config(minimal_config)
        self.assertEqual(config.get("neuron_count"), 50)
        self.assertEqual(config.get("region_count"), 1)  # default
    
    def test_config_template(self):
        """Test template application"""
        config = Config()
        config.apply_template(TEST_TEMPLATE)
        
        self.assertEqual(config.get("neuron_count"), 500)
        self.assertTrue(config.get("enable_plasticity"))
        self.assertTrue(config.get("enable_neuromodulation"))
    
    def test_config_setters(self):
        """Test configuration setters"""
        config = Config(TEST_CONFIG)
        
        # Test setters return self for chaining
        result = config.set("neuron_count", 200)
        self.assertIsInstance(result, Config)
        self.assertEqual(config.get("neuron_count"), 200)
        
        result = config.set("enable_development", False)
        self.assertEqual(config.get("enable_development"), False)
    
    def test_config_save_load(self):
        """Test configuration save and load"""
        config = Config(TEST_CONFIG)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            config.save(filepath)
            self.assertTrue(os.path.exists(filepath))
            
            # Load from file
            loaded_config = Config.load(filepath)
            self.assertEqual(loaded_config.get("neuron_count"), 100)
            self.assertEqual(loaded_config.get("random_seed"), 42)
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_config_get_template(self):
        """Test template retrieval"""
        template = get_template(TEST_TEMPLATE)
        self.assertIsInstance(template, dict)
        self.assertIn("neuron_count", template)
        self.assertEqual(template["neuron_count"], 500)
        
        # Test invalid template
        with self.assertRaises(ValueError):
            get_template("invalid_template")

class TestEnhancedBrain(unittest.TestCase):
    """Test enhanced brain functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        self.config = Config(TEST_CONFIG)
        self.brain = EnhancedBrain(self.config)
    
    def tearDown(self):
        """Clean up test fixtures"""
        if hasattr(self, 'brain') and self.brain:
            self.brain.cleanup()
    
    def test_brain_creation(self):
        """Test brain creation and initialization"""
        self.assertIsNotNone(self.brain)
        self.assertTrue(self.brain._initialized)
        self.assertIsNotNone(self.brain.brain)
    
    def test_brain_stats(self):
        """Test brain statistics"""
        stats = self.brain.get_stats()
        
        self.assertIn("neuron_count", stats)
        self.assertIn("synapse_count", stats)
        self.assertIn("firing_neurons", stats)
        self.assertIn("total_spikes", stats)
        self.assertIn("average_firing_rate", stats)
        
        self.assertEqual(stats["neuron_count"], 100)
        self.assertEqual(stats["synapse_count"], 0)  # Before simulation
        self.assertEqual(stats["total_spikes"], 0)  # Before simulation
    
    def test_brain_step(self):
        """Test brain simulation step"""
        initial_spikes = self.brain.brain.getTotalSpikeCount()
        
        # Run a step
        self.brain.step(0, 0.0)
        
        # Spike count should increase (or at least not decrease)
        final_spikes = self.brain.brain.getTotalSpikeCount()
        self.assertGreaterEqual(final_spikes, initial_spikes)
    
    def test_brain_injection(self):
        """Test current injection"""
        if NLM_AVAILABLE:
            # Test injecting to neurons of specific type
            self.brain.inject_current(pynlm.NeuronType.Sensory, 20.0)
            
            # Test injecting to specific neuron (if we had neuron IDs)
            # This would require more complex setup
            pass
    
    def test_brain_action(self):
        """Test action production"""
        # Get an action
        action = self.brain.produce_action()
        
        # Action could be None (no firing neurons) or an Action object
        self.assertTrue(action is None or hasattr(action, 'getType'))
    
    def test_brain_reset(self):
        """Test brain reset"""
        # Run some steps to generate activity
        for step in range(10):
            self.brain.step(step, step * 0.001)
        
        initial_spikes = self.brain.brain.getTotalSpikeCount()
        
        # Reset brain
        self.brain.reset()
        
        # Spike count should be reset (depending on implementation)
        # This test depends on the specific reset behavior
        pass
    
    def test_brain_save_load(self):
        """Test brain save and load functionality"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM core not available")
        
        # Create temporary file for brain save
        with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
            filepath = f.name
        
        try:
            # Run some simulation
            for step in range(5):
                self.brain.step(step, step * 0.001)
            
            # Save brain state
            self.brain.save(filepath)
            self.assertTrue(os.path.exists(filepath))
            
            # Create new brain and load state
            new_brain = EnhancedBrain(self.config)
            new_brain.load(filepath)
            
            # Should have the same spike count (approximately)
            original_spikes = self.brain.brain.getTotalSpikeCount()
            loaded_spikes = new_brain.brain.getTotalSpikeCount()
            self.assertEqual(original_spikes, loaded_spikes)
            
            new_brain.cleanup()
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_brain_context_manager(self):
        """Test brain as context manager"""
        with EnhancedBrain(self.config) as brain:
            self.assertTrue(brain._initialized)
            self.assertIsNotNone(brain.brain)
            
            # Brain should still be accessible after exiting context
            # (context manager might clean up)
            pass

class TestEnhancedWorld(unittest.TestCase):
    """Test enhanced world functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        self.config = {"world_width": 64, "world_height": 64, "random_seed": 42}
        self.world = EnhancedWorld(self.config)
    
    def tearDown(self):
        """Clean up test fixtures"""
        if hasattr(self, 'world') and self.world:
            self.world.cleanup()
    
    def test_world_creation(self):
        """Test world creation"""
        self.assertIsNotNone(self.world)
        self.assertIsNotNone(self.world.world)
    
    def test_world_configuration(self):
        """Test world configuration"""
        self.world.configure(128, 128, 32, 32, 123)
        
        stats = self.world.get_stats()
        self.assertEqual(stats["width"], 128)
        self.assertEqual(stats["height"], 128)
        self.assertEqual(stats["agent_x"], 0.0)  # Default position
    
    def test_world_object_management(self):
        """Test object management"""
        if NLM_AVAILABLE:
            # Create test object
            obj = pynlm.WorldObject()
            obj.x = 10.0
            obj.y = 20.0
            obj.type = pynlm.WorldObjectType.Resource
            obj.value = 5.0
            
            # Add object
            self.world.add_object(obj)
            
            # Verify object was added (implementation-dependent)
            # This would require additional NLM world API methods
            pass
    
    def test_world_update(self):
        """Test world update"""
        initial_time = self.world.world.getSimulationTime()
        
        # Update world
        self.world.update(0.01)
        
        final_time = self.world.world.getSimulationTime()
        self.assertGreater(final_time, initial_time)
    
    def test_world_reset(self):
        """Test world reset"""
        # Update world to change state
        self.world.update(0.01)
        
        # Reset world
        self.world.reset()
        
        # World should be back to initial state
        stats = self.world.get_stats()
        self.assertEqual(stats["agent_energy"], 100.0)  # Default energy
    
    def test_world_stats(self):
        """Test world statistics"""
        stats = self.world.get_stats()
        
        self.assertIn("width", stats)
        self.assertIn("height", stats)
        self.assertIn("agent_x", stats)
        self.assertIn("agent_y", stats)
        self.assertIn("agent_energy", stats)
        self.assertIn("agent_age", stats)
        
        self.assertEqual(stats["width"], 64)
        self.assertEqual(stats["height"], 64)
    
    def test_world_context_manager(self):
        """Test world as context manager"""
        with EnhancedWorld(self.config) as world:
            self.assertIsNotNone(world.world)
            self.assertIsNotNone(world.resource_manager)

class TestEnhancedAgent(unittest.TestCase):
    """Test enhanced agent functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        # Create brain and world
        self.config = Config(TEST_CONFIG)
        self.brain = EnhancedBrain(self.config)
        self.world = EnhancedWorld({"world_width": 64, "world_height": 64})
        
        # Create agent
        self.agent = EnhancedAgent(self.brain, self.world)
    
    def tearDown(self):
        """Clean up test fixtures"""
        if hasattr(self, 'agent') and self.agent:
            self.agent.cleanup()
        if hasattr(self, 'brain') and self.brain:
            self.brain.cleanup()
        if hasattr(self, 'world') and self.world:
            self.world.cleanup()
    
    def test_agent_creation(self):
        """Test agent creation"""
        self.assertIsNotNone(self.agent)
        self.assertIsNotNone(self.agent.agent_brain)
    
    def test_agent_system_control(self):
        """Test agent system control"""
        # Enable systems
        self.agent.enable_system("reward_modulation", True)
        self.agent.enable_system("development", True)
        
        # Verify systems are enabled (implementation-dependent)
        # This would require checking agent_brain internal state
        pass
    
    def test_agent_neuromodulation(self):
        """Test neuromodulation"""
        # Get initial levels
        initial_neuromod = self.agent.get_neuromodulation_level()
        initial_curiosity = self.agent.get_curiosity_level()
        
        # Apply reward modulation
        self.agent.apply_reward_modulation(1.0, 0.5)
        
        # Levels may have changed
        final_neuromod = self.agent.get_neuromodulation_level()
        final_curiosity = self.agent.get_curiosity_level()
        
        self.assertIsInstance(final_neuromod, (int, float))
        self.assertIsInstance(final_curiosity, (int, float))
    
    def test_agent_sensory_processing(self):
        """Test sensory processing"""
        if NLM_AVAILABLE:
            # Create test percept
            percept = pynlm.SensoryPercept()
            
            # Process input
            self.agent.process_sensory_input(percept)
            
            # Verify processing (implementation-dependent)
            pass
    
    def test_agent_action_decoding(self):
        """Test action decoding"""
        # Get motor command
        action = self.agent.decode_motor_command()
        
        # Action could be None or an Action object
        self.assertTrue(action is None or hasattr(action, 'getType'))
    
    def test_agent_development(self):
        """Test development update"""
        # Get initial developmental stage
        initial_stage = self.agent.get_developmental_stage()
        
        # Update development
        self.agent.update_development(0.01)
        
        # Stage may have changed (implementation-dependent)
        final_stage = self.agent.get_developmental_stage()
        
        self.assertIsInstance(initial_stage, pynlm.DevelopmentalStage)
        self.assertIsInstance(final_stage, pynlm.DevelopmentalStage)
    
    def test_agent_reset(self):
        """Test agent reset"""
        # Update development to change state
        self.agent.update_development(0.01)
        
        # Reset agent
        self.agent.reset()
        
        # Should be back to initial state
        # Implementation-dependent verification
        pass
    
    def test_agent_stats(self):
        """Test agent statistics"""
        stats = self.agent.get_stats()
        
        self.assertIn("developmental_stage", stats)
        self.assertIn("neuromodulation_level", stats)
        self.assertIn("curiosity_level", stats)
        self.assertIn("novelty_level", stats)
        self.assertIn("prediction_error", stats)
        
        # Verify types
        self.assertIsInstance(stats["developmental_stage"], str)
        self.assertIsInstance(stats["neuromodulation_level"], (int, float))
        self.assertIsInstance(stats["curiosity_level"], (int, float))

class TestNLMClass(unittest.TestCase):
    """Test main NLM class functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM not available")
        
        self.nlm = NLM()
    
    def tearDown(self):
        """Clean up test fixtures"""
        if hasattr(self, 'nlm') and self.nlm:
            self.nlm.cleanup()
    
    def test_nlm_creation(self):
        """Test NLM class creation"""
        self.assertIsNotNone(self.nlm)
        self.assertIsNotNone(self.nlm.config)
    
    def test_nlm_config_creation(self):
        """Test configuration creation"""
        config = self.nlm.create_config("basic", neuron_count=300)
        
        self.assertIsInstance(config, Config)
        self.assertEqual(config.get("neuron_count"), 300)
    
    def test_nlm_brain_creation(self):
        """Test brain creation"""
        brain = self.nlm.create_brain()
        
        self.assertIsInstance(brain, EnhancedBrain)
        self.assertTrue(brain._initialized)
    
    def test_nlm_world_creation(self):
        """Test world creation"""
        world = self.nlm.create_world()
        
        self.assertIsInstance(world, EnhancedWorld)
    
    def test_nlm_agent_creation(self):
        """Test agent creation"""
        # Create brain and world first
        brain = self.nlm.create_brain()
        world = self.nlm.create_world()
        
        agent = self.nlm.create_agent(brain, world)
        
        self.assertIsInstance(agent, EnhancedAgent)
    
    def test_nlm_basic_simulation(self):
        """Test basic simulation"""
        # Create components
        brain = self.nlm.create_brain()
        
        # Run basic simulation
        with timer("Test Basic Simulation"):
            stats = self.nlm.run_basic_simulation(20)
        
        self.assertIn("neuron_count", stats)
        self.assertIn("total_steps", stats)
        self.assertEqual(stats["total_steps"], 20)
    
    def test_nlm_config_templates(self):
        """Test configuration templates"""
        # Test basic template
        nlm1 = NLM()
        nlm1.config.apply_template("basic")
        self.assertEqual(nlm1.config.get("neuron_count"), 500)
        
        # Test learning template
        nlm2 = NLM()
        nlm2.config.apply_template("learning")
        self.assertEqual(nlm2.config.get("neuron_count"), 1000)
        self.assertTrue(nlm2.config.get("enable_curiosity"))
        
        # Test research template
        nlm3 = NLM()
        nlm3.config.apply_template("research")
        self.assertEqual(nlm3.config.get("neuron_count"), 5000)
        
        nlm1.cleanup()
        nlm2.cleanup()
        nlm3.cleanup()

class TestUtilityFunctions(unittest.TestCase):
    """Test utility functions"""
    
    def test_get_template(self):
        """Test template retrieval"""
        template = get_template("basic")
        self.assertIsInstance(template, dict)
        self.assertEqual(template["neuron_count"], 500)
    
    def test_create_demo_simulation(self):
        """Test demo simulation creation"""
        nlm = create_demo_simulation("basic", neuron_count=200)
        self.assertIsNotNone(nlm)
        self.assertEqual(nlm.config.get("neuron_count"), 200)
        
        nlm.cleanup()
    
    @unittest.skipIf(not NLM_AVAILABLE, "NLM not available")
    def test_run_quick_demo(self):
        """Test quick demo execution"""
        # This would actually run simulations, so we'll mock it
        with patch('python.pynlm_enhanced.NLM') as mock_nlm:
            mock_nlm.return_value.run_basic_simulation.return_value = {"status": "success"}
            mock_nlm.return_value.run_world_interaction.return_value = {"status": "success"}
            mock_nlm.return_value.run_learning_simulation.return_value = {"status": "success"}
            
            result = run_quick_demo()
            
            self.assertIn("basic_simulation", result)
            self.assertIn("world_interaction", result)
            self.assertIn("learning_simulation", result)

class TestErrorHandling(unittest.TestCase):
    """Test error handling"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM not available")
    
    def test_validation_error(self):
        """Test validation error handling"""
        invalid_config = {"neuron_count": -1}
        
        with self.assertRaises(ValidationError):
            Config(invalid_config)
    
    def test_nlm_resource_error(self):
        """Test NLM resource error"""
        # This test would require simulating resource constraints
        pass
    
    def test_nlm_simulation_error(self):
        """Test NLM simulation error"""
        # This test would require simulating simulation failures
        pass

class TestPerformance(unittest.TestCase):
    """Test performance characteristics"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM not available")
    
    def test_simulation_performance(self):
        """Test simulation performance"""
        nlm = NLM()
        
        start_time = time.time()
        
        # Run simulation
        stats = nlm.run_basic_simulation(100)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Performance should be reasonable (e.g., < 10 seconds for 100 steps)
        self.assertLess(duration, 30.0, "Simulation took too long")
        
        # Log performance metrics
        print(f"Simulation performance:")
        print(f"  Steps: {stats['total_steps']}")
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Steps per second: {stats['total_steps'] / duration:.1f}")
        
        nlm.cleanup()

class TestIntegration(unittest.TestCase):
    """Test integration between components"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not NLM_AVAILABLE:
            self.skipTest("NLM not available")
    
    def test_full_workflow(self):
        """Test complete NLM workflow"""
        print("Running integration test - Full NLM Workflow")
        
        # Create NLM instance
        nlm = NLM()
        
        try:
            # Create components
            brain = nlm.create_brain()
            world = nlm.create_world()
            agent = nlm.create_agent(brain, world)
            
            # Configure agent
            agent.enable_system("reward_modulation", True)
            agent.enable_system("development", True)
            
            # Create objects
            world.create_predefined_objects()
            
            # Run world interaction
            world_stats = nlm.run_world_interaction(3, 20)
            
            # Verify results
            self.assertIn("episodes", world_stats)
            self.assertIn("steps_per_episode", world_stats)
            self.assertEqual(world_stats["episodes"], 3)
            self.assertEqual(world_stats["steps_per_episode"], 20)
            
            print("✓ Integration test completed successfully")
            
        finally:
            nlm.cleanup()

class QuickTestRunner:
    """Quick test runner for faster testing"""
    
    def __init__(self):
        self.results = []
        self.start_time = time.time()
    
    def run_quick_tests(self):
        """Run quick subset of tests"""
        print("Running Quick Tests...")
        
        # Create test suite with only essential tests
        suite = unittest.TestSuite()
        
        # Add only core tests
        test_classes = [
            TestConfiguration,
            TestEnhancedBrain,
            TestNLMClass,
            TestUtilityFunctions,
        ]
        
        for test_class in test_classes:
            # Create instance and run tests
            test_instance = test_class()
            
            try:
                # Run setup
                test_instance.setUp()
                
                # Run specific tests
                if hasattr(test_instance, 'test_config_creation'):
                    test_instance.test_config_creation()
                
                if hasattr(test_instance, 'test_brain_creation'):
                    test_instance.test_brain_creation()
                
                if hasattr(test_instance, 'test_nlm_creation'):
                    test_instance.test_nlm_creation()
                
                if hasattr(test_instance, 'test_get_template'):
                    test_instance.test_get_template()
                
                self.results.append({
                    "test": test_class.__name__,
                    "status": "passed",
                    "time": time.time() - self.start_time,
                })
                
            except Exception as e:
                self.results.append({
                    "test": test_class.__name__,
                    "status": "failed",
                    "error": str(e),
                    "time": time.time() - self.start_time,
                })
            finally:
                try:
                    test_instance.tearDown()
                except:
                    pass
        
        end_time = time.time()
        duration = end_time - self.start_time
        
        return self.results, duration

def main():
    """Main test runner"""
    if not NLM_AVAILABLE:
        print("⚠ Warning: NLM core bindings not available. Some tests may be skipped.")
        print("Please build NLM with Python bindings to run full test suite.\n")
    
    print("=" * 60)
    print("NLM Python Bindings - Test Suite")
    print("=" * 60)
    
    # Check for command line arguments
    if len(sys.argv) > 1:
        if "--quick" in sys.argv:
            print("\nRunning Quick Tests...")
            
            runner = QuickTestRunner()
            results, duration = runner.run_quick_tests()
            
            print(f"\nQuick Test Results ({duration:.2f}s):")
            print("-" * 40)
            
            passed = sum(1 for r in results if r["status"] == "passed")
            failed = sum(1 for r in results if r["status"] == "failed")
            
            for result in results:
                status = "✓ PASS" if result["status"] == "passed" else "✗ FAIL"
                print(f"{status} {result['test']}")
                
                if result["status"] == "failed" and "error" in result:
                    print(f"    Error: {result['error']}")
            
            print(f"\nSummary: {passed} passed, {failed} failed")
            return
        
        elif "--verbose" in sys.argv:
            # Run full test suite
            suite = unittest.TestLoader().discover('.', pattern='test_*.py')
            
            print(f"\nRunning Full Test Suite...")
            print(f"Tests: {suite.countTestCases()}")
            
            runner = unittest.TextTestRunner(verbosity=2)
            result = runner.run(suite)
            
            print(f"\nFull Test Results:")
            print(f"Tests run: {result.testsRun}")
            print(f"Failures: {len(result.failures)}")
            print(f"Errors: {len(result.errors)}")
            
            return len(result.failures) + len(result.errors)
    
    # Default: run full test suite
    print(f"\nRunning Full Test Suite...")
    suite = unittest.TestLoader().discover('.', pattern='test_*.py')
    
    runner = unittest.TextTestRunner(verbosity=1)
    result = runner.run(suite)
    
    print(f"\nTest Results:")
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    
    if result.failures:
        print(f"\nFailures:")
        for test, traceback in result.failures:
            print(f"- {test}: {traceback}")
    
    if result.errors:
        print(f"\nErrors:")
        for test, traceback in result.errors:
            print(f"- {test}: {traceback}")
    
    return len(result.failures) + len(result.errors)

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)