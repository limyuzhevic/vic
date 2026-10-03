#!/usr/bin/env python3
"""
NLM Python Test Suite

This is a comprehensive test suite for the enhanced NLM Python bindings.
It covers unit tests, integration tests, and performance tests.

Usage:
    python nlm_test_suite.py [options]

Options:
    --quick     Run quick tests only (no performance tests)
    --integration Run integration tests only
    --performance Run performance tests only
    --all       Run all tests (default)
    --help       Show this help message
"""

import unittest
import sys
import time
import numpy as np
from unittest.mock import patch, MagicMock
import pynlm
class TestConfig(unittest.TestCase):
    """Test configuration management."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
    
    def test_config_creation(self):
        """Test default config creation."""
        self.assertIsNotNone(self.config)
        self.assertTrue(hasattr(self.config, 'has'))
        self.assertTrue(hasattr(self.config, 'set'))
        self.assertTrue(hasattr(self.config, 'get'))
    
    def test_config_set_and_get(self):
        """Test setting and getting configuration values."""
        # Test int
        self.config.set("test_int", 42)
        self.assertTrue(self.config.has("test_int"))
        self.assertEqual(self.config.getInt("test_int"), 42)
        
        # Test float
        self.config.set("test_float", 3.14)
        self.assertEqual(self.config.getFloat("test_float"), 3.14)
        
        # Test string
        self.config.set("test_string", "hello")
        self.assertEqual(self.config.getString("test_string"), "hello")
        
        # Test bool
        self.config.set("test_bool", True)
        self.assertTrue(self.config.getBool("test_bool"))
    
    def test_config_get_or(self):
        """Test getOr method."""
        # Get non-existent key with default
        value = self.config.getOr("missing", "default")
        self.assertEqual(value, "default")
        
        # Get existing key
        self.config.set("existing", 100)
        value = self.config.getOr("existing", 200)
        self.assertEqual(value, 100)
    
    def test_config_clear(self):
        """Test clearing configuration."""
        self.config.set("key1", 1)
        self.config.set("key2", 2)
        self.config.clear()
        self.assertFalse(self.config.has("key1"))
        self.assertFalse(self.config.has("key2"))
    
    def test_config_keys(self):
        """Test getting configuration keys."""
        self.assertEqual(len(self.config.getKeys()), 0)  # Empty config
        
        self.config.set("key1", 1)
        self.config.set("key2", 2)
        keys = self.config.getKeys()
        self.assertIn("key1", keys)
        self.assertIn("key2", keys)
        self.assertEqual(len(keys), 2)
    
    def test_config_file_operations(self):
        """Test file operations."""
        # Test save/load
        self.config.set("test_value", 123)
        self.config.saveToFile("test_config.json")
        
        # Create new config and load
        new_config = pynlm.createDefaultConfig()
        self.assertTrue(new_config.loadFromFile("test_config.json"))
        self.assertEqual(new_config.getInt("test_value"), 123)
        
        # Clean up
        import os
        os.remove("test_config.json")
    
    def test_config_summary(self):
        """Test configuration summary."""
        self.config.set("key1", 1)
        self.config.set("key2", "value")
        
        summary = self.config.summary()
        self.assertIn("key1", summary)
        self.assertIn("key2", summary)
class TestBrain(unittest.TestCase):
    """Test brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        """Test brain creation."""
        self.assertIsNotNone(self.brain)
        self.assertTrue(hasattr(self.brain, 'initialize'))
        self.assertTrue(hasattr(self.brain, 'step'))
        self.assertTrue(hasattr(self.brain, 'getTotalNeuronCount'))
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
        self.assertTrue(self.brain.getTotalSynapseCount() > 0)
        self.assertEqual(self.brain.getDevelopmentalStage(), 
                        pynlm.DevelopmentalStage.Initial)
    
    def test_brain_step(self):
        """Test brain simulation step."""
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_firing = self.brain.getFiringNeuronCount()
        
        self.brain.step(0)
        
        self.assertEqual(self.brain.getTotalSpikeCount(), initial_spikes)
        self.assertEqual(self.brain.getFiringNeuronCount(), initial_firing)
    
    def test_brain_stats(self):
        """Test brain statistics."""
        stats = self.brain.toDict()
        
        self.assertIn("neuronCount", stats)
        self.assertIn("synapseCount", stats)
        self.assertIn("firingNeurons", stats)
        self.assertIn("activeNeurons", stats)
        self.assertIn("totalSpikes", stats)
        self.assertIn("pendingSpikes", stats)
        self.assertIn("avgFiringRate", stats)
        self.assertIn("eiRatio", stats)
    
    def test_brain_reset(self):
        """Test brain reset."""
        self.brain.step(0)
        self.brain.reset()
        
        # After reset, should be back to initial state
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
        self.assertTrue(self.brain.getTotalSynapseCount() > 0)
    
    def test_brain_region_management(self):
        """Test brain region management."""
        region_id = self.brain.addRegion("test_region")
        self.assertIsNotNone(region_id)
        
        region = self.brain.getRegion(region_id)
        self.assertIsNotNone(region)
        
        self.assertEqual(self.brain.getRegionCount(), 1)
        region_ids = self.brain.getRegionIds()
        self.assertIn(region_id, region_ids)
    
    def test_brain_save_load(self):
        """Test brain save/load functionality."""
        # Modify brain state
        for i in range(5):
            self.brain.step(i)
        
        # Save brain state
        self.brain.save("test_brain.bin")
        
        # Create new brain and load
        new_brain = pynlm.createBrain(self.config)
        new_brain.load("test_brain.bin")
        
        # Verify state
        self.assertEqual(new_brain.getTotalSpikeCount(), 
                        self.brain.getTotalSpikeCount())
        
        # Clean up
        import os
        os.remove("test_brain.bin")
    
    def test_brain_getters(self):
        """Test brain getter methods."""
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
        self.assertGreaterEqual(self.brain.getActiveNeuronCount(), 0)
        self.assertGreaterEqual(self.brain.getFiringNeuronCount(), 0)
        self.assertGreaterEqual(self.brain.getAverageFiringRate(), 0.0)
        self.assertGreaterEqual(self.brain.getExcitationInhibitionRatio(), 0.0)
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(self.brain.getPendingSpikeEventCount(), 0)
class TestAgentBrain(unittest.TestCase):
    """Test agent brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
    
    def test_agent_creation(self):
        """Test agent brain creation."""
        self.assertIsNotNone(self.agent)
        self.assertEqual(self.agent.getBrain(), self.brain)
        self.assertEqual(self.agent.getSensoryInputSize(), 8 * 8 * 3)
        self.assertEqual(self.agent.getMotorOutputSize(), 8)
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        # Create a simple percept
        from pynlm import Vision
        vision = Vision(8, 8, 3)
        data = [i * 0.1 for i in range(8 * 8 * 3)]
        vision.setData(data)
        
        self.agent.processSensoryInput(vision)
        # If no exception, processing succeeded
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Create a percept
        from pynlm import Vision
        vision = Vision(8, 8, 3)
        vision.setData([0.1] * (8 * 8 * 3))
        
        self.agent.processSensoryInput(vision)
        self.brain.step(0)
        
        action = self.agent.decodeMotorCommand()
        self.assertIsNotNone(action)
        self.assertEqual(action.getType(), pynlm.MotorCommand.Wait)  # Default action
    
    def test_agent_neuromodulation(self):
        """Test neuromodulation."""
        # Enable neuromodulation
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Check values
        self.assertGreater(self.agent.getCuriosityLevel(), 0.0)
        self.assertGreater(self.agent.getNoveltyLevel(), 0.0)
        self.assertGreater(self.agent.getPredictionError(), 0.0)
        self.assertGreater(self.agent.getNeuromodulationLevel(), 0.0)
    
    def test_agent_subsystem_controls(self):
        """Test subsystem enable/disable controls."""
        # Test enabling
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())
        
        # Test disabling
        self.agent.enableRewardModulation(False)
        self.agent.enableStructuralPlasticity(False)
        self.agent.enableDevelopment(False)
        self.agent.enableCuriosity(False)
        
        self.assertFalse(self.agent.isRewardModulationEnabled())
        self.assertFalse(self.agent.isStructuralPlasticityEnabled())
        self.assertFalse(self.agent.isDevelopmentEnabled())
        self.assertFalse(self.agent.isCuriosityEnabled())
    
    def test_agent_state_conversion(self):
        """Test agent state conversion."""
        state = self.agent.toDict()
        
        self.assertIn("curiosityLevel", state)
        self.assertIn("noveltyLevel", state)
        self.assertIn("predictionError", state)
        self.assertIn("neuromodulationLevel", state)
        self.assertIn("developmentalStage", state)
        self.assertIn("rewardModEnabled", state)
        self.assertIn("plasticityEnabled", state)
        self.assertIn("developmentEnabled", state)
        self.assertIn("curiosityEnabled", state)
class TestWorld(unittest.TestCase):
    """Test world functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
    
    def test_world_creation(self):
        """Test world creation."""
        self.assertIsNotNone(self.world)
        self.assertEqual(self.world.getWidth(), 20)
        self.assertEqual(self.world.getHeight(), 20)
    
    def test_world_configuration(self):
        """Test world configuration."""
        self.assertEqual(self.world.getVisionDimensions(), (8, 8))
        self.assertEqual(self.world.getAgentPosition(), (10.0, 10.0))
        self.assertEqual(self.world.getAgentOrientation(), 0.0)
    
    def test_world_update(self):
        """Test world update."""
        initial_time = self.world.getSimulationTime()
        self.world.update(0.1)
        
        self.assertGreater(self.world.getSimulationTime(), initial_time)
    
    def test_world_agent_movement(self):
        """Test agent movement in world."""
        from pynlm import Action, ActionType
        action = Action(ActionType.MoveForward, [1.0])
        
        initial_position = self.world.getAgentPosition()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        new_position = self.world.getAgentPosition()
        self.assertNotEqual(initial_position, new_position)
    
    def test_world_sensory_percept(self):
        """Test world sensory percept."""
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
        self.assertTrue(percept.hasVision())
        self.assertTrue(percept.getVision() is not None)
    
    def test_world_object_management(self):
        """Test world object management."""
        from pynlm import WorldObject, WorldObjectType
        
        # Add object
        obj = WorldObject(5.0, 5.0, WorldObjectType.Resource, 10.0)
        self.world.addObject(obj)
        
        # Remove object
        self.world.removeObject(5.0, 5.0)
        
        # Check validity
        self.assertTrue(self.world.isValidPosition(15.0, 15.0))
        self.assertTrue(self.world.isValidPosition(0.0, 0.0))
    
    def test_world_state_access(self):
        """Test world state access."""
        body = self.world.getAgentBody()
        self.assertIsNotNone(body)
        self.assertEqual(body.x, 10.0)
        self.assertEqual(body.y, 10.0)
        self.assertEqual(body.orientation, 0.0)
class TestSensoryInput(unittest.TestCase):
    """Test sensory input classes."""
    
    def test_vision_creation(self):
        """Test vision creation."""
        vision = pynlm.Vision(10, 10, 3)
        self.assertEqual(vision.getWidth(), 10)
        self.assertEqual(vision.getHeight(), 10)
        self.assertEqual(vision.getChannels(), 3)
    
    def test_vision_data(self):
        """Test vision data operations."""
        vision = pynlm.Vision(2, 2, 1)
        data = [0.1, 0.2, 0.3, 0.4]
        vision.setData(data)
        
        retrieved_data = vision.getData()
        self.assertEqual(retrieved_data, data)
    
    def test_audio_creation(self):
        """Test audio creation."""
        audio = pynlm.Audio(44100, 1000)
        self.assertEqual(audio.getSampleRate(), 44100)
        self.assertEqual(audio.getNumSamples(), 1000)
    
    def test_internal_signals(self):
        """Test internal signals."""
        internal = pynlm.InternalSignals()
        internal.addSignal(0.5)
        internal.addSignal(0.8)
        
        signals = internal.getSignals()
        self.assertEqual(len(signals), 2)
        self.assertEqual(signals[0], 0.5)
        self.assertEqual(signals[1], 0.8)
class TestAction(unittest.TestCase):
    """Test action classes."""
    
    def test_action_creation(self):
        """Test action creation."""
        action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 0.5])
        self.assertEqual(action.getType(), pynlm.ActionType.MoveForward)
        self.assertEqual(action.getParameters(), [1.0, 0.5])
        self.assertEqual(action.getName(), "MoveForward")
    
    def test_action_modification(self):
        """Test action modification."""
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        action.setType(pynlm.ActionType.TurnLeft)
        action.setParameters([0.5])
        
        self.assertEqual(action.getType(), pynlm.ActionType.TurnLeft)
        self.assertEqual(action.getParameters(), [0.5])
    
    def test_action_conversion(self):
        """Test action conversion."""
        action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0, 3.0])
        
        # Convert to dictionary
        action_dict = action.toDict()
        self.assertEqual(action_dict["type"], pynlm.ActionType.MoveForward)
        self.assertEqual(action_dict["parameters"], [1.0, 2.0, 3.0])
        
        # Restore from dictionary
        restored_action = pynlm.Action.fromDict(action_dict)
        self.assertEqual(restored_action.getType(), pynlm.ActionType.MoveForward)
        self.assertEqual(restored_action.getParameters(), [1.0, 2.0, 3.0])
class TestIntegration(unittest.TestCase):
    """Integration tests."""
    
    def test_complete_agent_workflow(self):
        """Test complete agent workflow."""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(20):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check that everything is working
            self.assertIsNotNone(percept)
            self.assertIsNotNone(action)
        
        # Verify state
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
    
    def test_configuration_workflow(self):
        """Test configuration workflow."""
        # Create custom config
        config = pynlm.createDefaultConfig(
            neuron_count=500,
            connection_probability=0.02
        )
        
        # Modify config
        config.set("brain.v_thresh", -45.0)
        config.set("plasticity.stdp.enable", True)
        
        # Create brain with config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify config
        self.assertEqual(brain.getTotalNeuronCount(), 500)
        self.assertEqual(config.getFloat("brain.v_thresh"), -45.0)
    
    def test_error_handling_workflow(self):
        """Test error handling workflow."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test valid error handling
        from pynlm import Vision
        
        vision = Vision(4, 4, 3)
        
        # This should work
        valid_data = [0.1] * (4 * 4 * 3)
        vision.setData(valid_data)
        
        # This should throw an error
        invalid_data = [0.1, 0.2]  # Wrong size
        
        try:
            vision.setData(invalid_data)
            self.fail("Should have thrown an error for invalid data size")
        except Exception:
            pass  # Expected
class TestPerformance(unittest.TestCase):
    """Performance tests (run with caution)."""
    
    def test_simulation_speed(self):
        """Test simulation speed."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        
        for i in range(100):
            brain.step(i)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Should be reasonably fast
        self.assertLess(duration, 10.0)  # Should complete in under 10 seconds
    
    def test_memory_usage(self):
        """Test memory usage."""
        import tracemalloc
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        tracemalloc.start()
        
        for i in range(50):
            brain.step(i)
        
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Memory usage should be reasonable
        self.assertLess(peak, 100 * 1024 * 1024)  # Less than 100MB
    
    def test_scaling(self):
        """Test scaling with different brain sizes."""
        sizes = [100, 500, 1000]
        times = []
        
        for size in sizes:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for i in range(50):
                brain.step(i)
            end_time = time.time()
            
            times.append(end_time - start_time)
        
        # Check that scaling is reasonable (larger brains take longer)
        self.assertLess(times[0], times[1])  # 100 neurons faster than 500
        self.assertLess(times[1], times[2])  # 500 neurons faster than 1000
class TestPythonicAPI(unittest.TestCase):
    """Test Pythonic API features."""
    
    def test_convenience_functions(self):
        """Test convenience functions."""
        # Test createDefaultConfig
        config1 = pynlm.createDefaultConfig()
        self.assertIsNotNone(config1)
        
        config2 = pynlm.createDefaultConfig(neuron_count=1000)
        self.assertEqual(config2.getInt("brain.neuron_count"), 1000)
        
        # Test createBrain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.assertIsNotNone(brain)
        
        # Test createSimpleWorld
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        
        # Test createAgentBrain
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
    
    def test_enhanced_documentation(self):
        """Test that enhanced documentation is available."""
        # Check that the module has enhanced attributes
        self.assertTrue(hasattr(pynlm, '__version__'))
        self.assertTrue(hasattr(pynlm, '__author__'))
        self.assertTrue(hasattr(pynlm, '__description__'))
        
        # Check that helper functions exist
        self.assertTrue(hasattr(pynlm, 'getDocumentation'))
        self.assertTrue(hasattr(pynlm, 'getExamples'))
        self.assertTrue(hasattr(pynlm, 'getQuickStart'))
    
    def test_type_safety(self):
        """Test type safety features."""
        config = pynlm.createDefaultConfig()
        
        # Test type-specific getters
        config.set("test_int", 42)
        self.assertEqual(config.getInt("test_int"), 42)
        
        config.set("test_float", 3.14)
        self.assertEqual(config.getFloat("test_float"), 3.14)
        
        config.set("test_string", "hello")
        self.assertEqual(config.getString("test_string"), "hello")
        
        config.set("test_bool", True)
        self.assertTrue(config.getBool("test_bool"))
    
    def test_string_representations(self):
        """Test improved string representations."""
        config = pynlm.createDefaultConfig()
        config.set("test_value", 123)
        
        # Config should have nice string representation
        config_str = str(config)
        self.assertIn("Config", config_str)
        self.assertIn("test_value", config_str)
        
        # Test with enhanced ID classes
        from pynlm import NeuronId, SynapseId, RegionId
        
        neuron_id = NeuronId(42)
        self.assertIn("NeuronId", str(neuron_id))
        
        synapse_id = SynapseId(99)
        self.assertIn("SynapseId", str(synapse_id))
        
        region_id = RegionId(1)
        self.assertIn("RegionId", str(region_id))
class TestQuickStart(unittest.TestCase):
    """Quick start tests."""
    
    def test_minimal_example(self):
        """Test the minimal working example from the quick start guide."""
        import pynlm
        
        # Simplest possible brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brain.step(0)
        
        self.assertGreater(brain.getTotalNeuronCount(), 0)
    
    def test_complete_example(self):
        """Test the complete example from the quick start guide."""
        import pynlm
        
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for i in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0)
class NLMTestSuite(unittest.TestCase):
    """Main test suite that runs all tests."""
    
    def run_all_tests(self, options=None):
        """Run all tests based on options."""
        options = options or {}
        
        # Create test suite
        loader = unittest.TestLoader()
        suite = unittest.TestSuite()
        
        # Add test classes based on options
        if options.get('quick', True):
            # Quick tests (unit tests only)
            test_classes = [
                TestConfig,
                TestBrain,
                TestAgentBrain,
                TestWorld,
                TestSensoryInput,
                TestAction,
                TestIntegration,
                TestPythonicAPI,
                TestQuickStart,
            ]
        else:
            # All tests
            test_classes = [
                TestConfig,
                TestBrain,
                TestAgentBrain,
                TestWorld,
                TestSensoryInput,
                TestAction,
                TestIntegration,
                TestPythonicAPI,
                TestQuickStart,
                TestPerformance,
            ]
        
        if options.get('integration', True):
            test_classes.extend([
                TestIntegration,
            ])
        
        if options.get('performance', False):
            test_classes.append(TestPerformance)
        
        # Add all test cases
        for test_class in test_classes:
            tests = loader.loadTestsFromTestCase(test_class)
            suite.addTests(tests)
        
        # Run the tests
        runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
        result = runner.run(suite)
        
        return result.wasSuccessful()
def parse_arguments():
    """Parse command line arguments."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Python Test Suite",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        '--quick', action='store_true',
        help='Run quick tests only (no performance tests)'
    )
    
    parser.add_argument(
        '--integration', action='store_true',
        help='Run integration tests only'
    )
    
    parser.add_argument(
        '--performance', action='store_true',
        help='Run performance tests only'
    )
    
    parser.add_argument(
        '--all', action='store_true',
        help='Run all tests'
    )
    
    parser.add_argument(
        '--verbose', '-v', action='store_true',
        help='Verbose output'
    )
    
    return parser.parse_args()
def main():
    """Main function."""
    args = parse_arguments()
    
    # Set options
    options = {
        'quick': not args.performance and not args.integration,
        'integration': args.integration,
        'performance': args.performance,
    }
    
    if not options['quick'] and not options['integration'] and not options['performance']:
        # Default: run quick tests
        options['quick'] = True
    
    print("🚀 NLM Python Test Suite")
    print("=" * 50)
    print(f"Running tests: {options}")
    print("=" * 50)
    
    # Run tests
    test_suite = NLMTestSuite()
    success = test_suite.run_all_tests(options)
    
    if success:
        print("\n✅ All tests passed!")
        return 0
    else:
        print("\n❌ Some tests failed!")
        return 1
if __name__ == '__main__':
    sys.exit(main())