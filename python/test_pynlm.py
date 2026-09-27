#!/usr/bin/env python3
"""
Comprehensive tests for NLM Python bindings.

This module provides comprehensive tests for the NLM Python bindings,
including unit tests, integration tests, and performance tests.
"""

import sys
import os
import tempfile
import json
import time
import numpy as np
import unittest
from typing import Any, Dict, List, Tuple

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from pynlm import (
    Config,
    EnhancedConfig,
    Brain,
    EnhancedBrain,
    SimpleWorld,
    EnhancedSimpleWorld,
    AgentBrain,
    EnhancedAgentBrain,
    Vision,
    Audio,
    SensoryPercept,
    Action,
    WorldObject,
    ActionResult,
    AgentBody,
    NLMError,
    ConfigurationError,
    SimulationError,
    SerializationError,
    ActionError,
    WorldError,
    AgentError,
    SensoryError,
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    createAction,
    createWorldObject,
    createAgentBody,
    createActionResult,
)
class TestConfig(unittest.TestCase):
    """Test configuration functionality."""
    
    def test_default_config_creation(self):
        """Test creation of default configuration."""
        config = Config()
        self.assertIsNotNone(config)
        self.assertTrue(config.has("brain.neuron_count"))
    
    def test_enhanced_config_creation(self):
        """Test creation of enhanced configuration."""
        config = EnhancedConfig()
        self.assertIsNotNone(config)
        self.assertTrue(config.has("brain.neuron_count"))
    
    def test_config_set_get(self):
        """Test setting and getting configuration values."""
        config = EnhancedConfig()
        config.set("brain.neuron_count", 1000)
        self.assertEqual(config.get("brain.neuron_count"), 1000)
    
    def test_config_get_or(self):
        """Test getting configuration value with default."""
        config = EnhancedConfig()
        value = config.get_or("nonexistent.key", "default")
        self.assertEqual(value, "default")
    
    def test_config_clear(self):
        """Test clearing configuration."""
        config = EnhancedConfig()
        config.set("brain.neuron_count", 1000)
        config.clear()
        self.assertFalse(config.has("brain.neuron_count"))
    
    def test_config_save_load(self):
        """Test saving and loading configuration from file."""
        config = EnhancedConfig()
        config.set("brain.neuron_count", 1000)
        config.set("world.width", 20)
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
            config.save_to_file(config_file)
        
        loaded_config = EnhancedConfig()
        loaded_config.load_from_file(config_file)
        
        self.assertEqual(loaded_config.get("brain.neuron_count"), 1000)
        self.assertEqual(loaded_config.get("world.width"), 20)
        
        os.unlink(config_file)
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = EnhancedConfig()
        config.set("brain.neuron_count", 1000)
        
        # Valid configuration
        errors = config.validate()
        self.assertEqual(len(errors), 0)
        
        # Invalid configuration
        config.set("brain.neuron_count", -1)
        errors = config.validate()
        self.assertGreater(len(errors), 0)
    
    def test_config_summary(self):
        """Test configuration summary."""
        config = EnhancedConfig()
        config.set("brain.neuron_count", 1000)
        
        summary = config.summary()
        self.assertIsInstance(summary, str)
        self.assertGreater(len(summary), 0)


class TestBrain(unittest.TestCase):
    """Test brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = EnhancedConfig()
        self.config.set("brain.neuron_count", 100)
        self.brain = EnhancedBrain(self.config)
        self.brain.initialize()
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        self.assertTrue(self.brain.initialized)
        self.assertEqual(self.brain.total_neuron_count, 100)
    
    def test_brain_step(self):
        """Test brain simulation step."""
        initial_spikes = self.brain.total_spike_count
        self.brain.step(0)
        self.assertGreaterEqual(self.brain.total_spike_count, initial_spikes)
    
    def test_brain_step_with_time(self):
        """Test brain simulation step with timestamp."""
        initial_spikes = self.brain.total_spike_count
        self.brain.step_with_time(1, 0.1)
        self.assertGreaterEqual(self.brain.total_spike_count, initial_spikes)
    
    def test_brain_statistics(self):
        """Test brain statistics properties."""
        self.assertIsInstance(self.brain.total_neuron_count, int)
        self.assertIsInstance(self.brain.firing_neuron_count, int)
        self.assertIsInstance(self.brain.total_spike_count, int)
        self.assertIsInstance(self.brain.average_firing_rate, float)
        self.assertIsInstance(self.brain.excitation_inhibition_ratio, float)
        self.assertIsInstance(self.brain.developmental_stage, Any)
    
    def test_brain_regions(self):
        """Test brain region functionality."""
        region_count = self.brain.region_count
        self.assertIsInstance(region_count, int)
        
        if region_count > 0:
            region_ids = self.brain.get_region_ids()
            self.assertEqual(len(region_ids), region_count)
    
    def test_brain_save_load(self):
        """Test brain save and load functionality."""
        # Save brain state
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            brain_file = f.name
            self.brain.save(brain_file)
        
        # Load brain state
        loaded_brain = EnhancedBrain(self.config)
        loaded_brain.load(brain_file)
        
        self.assertEqual(loaded_brain.total_neuron_count, self.brain.total_neuron_count)
        
        os.unlink(brain_file)
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        self.brain.reset()
        self.assertFalse(self.brain.initialized)
    
    def test_brain_add_region(self):
        """Test brain region addition."""
        initial_count = self.brain.region_count
        region_id = self.brain.add_region("test_region")
        self.assertIsNotNone(region_id)
        self.assertEqual(self.brain.region_count, initial_count + 1)


class TestAgentBrain(unittest.TestCase):
    """Test agent brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = EnhancedConfig()
        self.config.set("brain.neuron_count", 100)
        
        self.brain = EnhancedBrain(self.config)
        self.brain.initialize()
        
        self.world = EnhancedSimpleWorld()
        self.world.configure(width=20, height=20, vision_width=8, vision_height=8)
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        self.agent = EnhancedAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_initialization(self):
        """Test agent brain initialization."""
        self.assertTrue(self.agent.initialized)
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        percept = self.world.getSensoryPercept()
        action = self.agent.processSensoryInput(percept)
        self.assertIsNotNone(action)
    
    def test_agent_mixed_command_decoding(self):
        """Test motor command decoding."""
        action = self.agent.decodeMotorCommand()
        self.assertIsNotNone(action)
    
    def test_agent_reward_modulation(self):
        """Test reward modulation."""
        self.agent.applyRewardModulation(10.0, 8.0)
        self.assertGreaterEqual(self.agent.reward_level, 0.0)
    
    def test_agent_curiosity(self):
        """Test curiosity system."""
        self.agent.applyRewardModulation(1.0, 0.5)  # Trigger curiosity
        self.assertGreaterEqual(self.agent.curiosity_level, 0.0)
    
    def test_agent_development(self):
        """Test development system."""
        initial_stage = self.agent.developmental_stage
        self.agent.updateDevelopment(0.1)
        # Development may not always change, but shouldn't crash
        self.assertIsNotNone(self.agent.developmental_stage)
    
    def test_agent_learning_systems(self):
        """Test learning system control."""
        # Test enabling learning systems
        self.agent.enable_reward_modulation(True)
        self.agent.enable_curiosity(True)
        self.agent.enable_development(True)
        self.agent.enable_structural_plasticity(True)
        
        # Verify they are enabled
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())


class TestWorld(unittest.TestCase):
    """Test world functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.world = EnhancedSimpleWorld()
        self.world.configure(width=20, height=20, vision_width=8, vision_height=8)
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
    
    def test_world_configuration(self):
        """Test world configuration."""
        self.assertEqual(self.world.width, 20)
        self.assertEqual(self.world.height, 20)
        self.assertEqual(self.world.simulation_time, 0.0)
    
    def test_world_update(self):
        """Test world simulation update."""
        initial_time = self.world.simulation_time
        self.world.update(0.1)
        self.assertGreater(self.world.simulation_time, initial_time)
    
    def test_world_agent_body(self):
        """Test agent body access."""
        body = self.world.getAgentBody()
        self.assertIsNotNone(body)
        self.assertEqual(body.x, 10.0)
        self.assertEqual(body.y, 10.0)
    
    def test_world_object_management(self):
        """Test world object management."""
        # Create world object
        obj = WorldObject(5.0, 5.0, 1, 0.5)
        self.world.addObject(obj)
        
        # Check if object is valid
        self.assertTrue(self.world.isValidPosition(5.0, 5.0))
        
        # Remove object
        self.world.removeObject(5.0, 5.0)
        self.assertFalse(self.world.isValidPosition(5.0, 5.0))
    
    def test_world_sensory_percept(self):
        """Test world sensory percept."""
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Check all signal types
        signals = percept.getAllSignals()
        # At least vision should be present
        self.assertTrue('vision' in signals or 'touch' in signals or 'internal' in signals)


class TestSensory(unittest.TestCase):
    """Test sensory functionality."""
    
    def test_vision_processing(self):
        """Test vision processing."""
        vision = Vision(32, 32, 3)
        self.assertEqual(vision.width, 32)
        self.assertEqual(vision.height, 32)
        self.assertEqual(vision.channels, 3)
        
        # Test setting data
        data = [0.1] * (32 * 32 * 3)
        vision.setData(data)
        
        # Test getting data
        retrieved = vision.to_numpy()
        self.assertEqual(retrieved.shape, (32, 32, 3))
    
    def test_audio_processing(self):
        """Test audio processing."""
        audio = Audio(44100, 1024)
        self.assertEqual(audio.sample_rate, 44100)
        self.assertEqual(audio.num_samples, 1024)
        
        # Test setting data
        data = np.random.randn(1024).tolist()
        audio.setData(data)
        
        # Test getting data
        retrieved = audio.to_numpy()
        self.assertEqual(len(retrieved), 1024)
    
    def test_sensory_percept(self):
        """Test sensory percept functionality."""
        percept = SensoryPercept()
        
        # Test setting different signals
        percept.setVision([[0.1, 0.2], [0.3, 0.4]])
        percept.setTouch([0.1, 0.2, 0.3])
        percept.setAudio([0.5, 0.6, 0.7])
        
        # Test getting signals
        self.assertIsInstance(percept.getVision(), list)
        self.assertIsInstance(percept.getTouch(), list)
        self.assertIsInstance(percept.getAudio(), list)
        
        # Test all signals
        signals = percept.getAllSignals()
        self.assertIsInstance(signals, dict)
        
        # Test timestamp
        self.assertIsInstance(percept.getTimestamp(), float)
        percept.setTimestamp(1.0)
        self.assertEqual(percept.getTimestamp(), 1.0)


class TestAction(unittest.TestCase):
    """Test action functionality."""
    
    def test_action_creation(self):
        """Test action creation."""
        action = Action()
        self.assertIsNotNone(action)
    
    def test_action_from_type(self):
        """Test action creation from type."""
        action = Action(1)  # ActionType.MoveForward
        self.assertEqual(action.getType(), 1)
    
    def test_action_with_parameters(self):
        """Test action creation with parameters."""
        action = Action(1, [1.0, 0.5, 0.2])
        self.assertEqual(action.getType(), 1)
        self.assertEqual(action.getParameters(), [1.0, 0.5, 0.2])
    
    def test_action_name(self):
        """Test action name."""
        action = Action(1)  # ActionType.MoveForward
        name = action.getName()
        self.assertIsInstance(name, str)
        self.assertGreater(len(name), 0)
    
    def test_action_clone(self):
        """Test action cloning."""
        action = Action(1, [1.0, 0.5])
        cloned = action.clone()
        self.assertEqual(cloned.getType(), action.getType())
        self.assertEqual(cloned.getParameters(), action.getParameters())
    
    def test_action_result(self):
        """Test action result."""
        result = ActionResult(10.0, True, "Success")
        self.assertEqual(result.reward, 10.0)
        self.assertTrue(result.success)
        self.assertEqual(result.message, "Success")


class TestFactoryFunctions(unittest.TestCase):
    """Test factory functions."""
    
    def test_factory_functions(self):
        """Test all factory functions."""
        # Test createDefaultConfig
        config = createDefaultConfig()
        self.assertIsInstance(config, Config)
        
        # Test createBrain
        brain = createBrain(config)
        self.assertIsInstance(brain, Brain)
        
        # Test createSimpleWorld
        world = createSimpleWorld()
        self.assertIsInstance(world, SimpleWorld)
        
        # Test createAgentBrain
        agent = createAgentBrain(brain)
        self.assertIsInstance(agent, AgentBrain)
        
        # Test createAction
        action = createAction(1, [1.0, 0.5])
        self.assertIsInstance(action, Action)
        
        # Test createWorldObject
        obj = createWorldObject(5.0, 5.0, 1, 0.5)
        self.assertIsInstance(obj, WorldObject)
        
        # Test createAgentBody
        body = createAgentBody()
        self.assertIsInstance(body, WorldObject)
        
        # Test createActionResult
        result = createActionResult(10.0, True, "Success")
        self.assertIsInstance(result, ActionResult)


class TestErrorHandling(unittest.TestCase):
    """Test error handling."""
    
    def test_configuration_error(self):
        """Test configuration error handling."""
        try:
            raise ConfigurationError("Invalid configuration")
        except ConfigurationError as e:
            self.assertEqual(str(e), "Invalid configuration")
    
    def test_simulation_error(self):
        """Test simulation error handling."""
        try:
            raise SimulationError("Simulation failed")
        except SimulationError as e:
            self.assertEqual(str(e), "Simulation failed")
    
    def test_serialization_error(self):
        """Test serialization error handling."""
        try:
            raise SerializationError("File I/O failed")
        except SerializationError as e:
            self.assertEqual(str(e), "File I/O failed")
    
    def test_action_error(self):
        """Test action error handling."""
        try:
            raise ActionError("Action failed")
        except ActionError as e:
            self.assertEqual(str(e), "Action failed")
    
    def test_world_error(self):
        """Test world error handling."""
        try:
            raise WorldError("World error")
        except WorldError as e:
            self.assertEqual(str(e), "World error")
    
    def test_agent_error(self):
        """Test agent error handling."""
        try:
            raise AgentError("Agent error")
        except AgentError as e:
            self.assertEqual(str(e), "Agent error")
    
    def test_sensory_error(self):
        """Test sensory error handling."""
        try:
            raise SensoryError("Sensory error")
        except SensoryError as e:
            self.assertEqual(str(e), "Sensory error")


class TestIntegration(unittest.TestCase):
    """Test integration between components."""
    
    def test_full_simulation(self):
        """Test full simulation integration."""
        # Create world
        world = EnhancedSimpleWorld()
        world.configure(width=20, height=20, vision_width=8, vision_height=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Create brain
        config = EnhancedConfig()
        config.set("brain.neuron_count", 500)
        
        brain = EnhancedBrain(config)
        brain.initialize()
        
        # Create agent
        agent = EnhancedAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(10):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            action = agent.processSensoryInput(percept)
            world.applyMotorCommand(action, world.simulation_time)
            
            brain.step(step)
        
        # Verify results
        self.assertGreater(brain.total_spike_count, 0)
        self.assertGreater(world.getAgentBody().energy, 0.0)
    
    def test_file_io_integration(self):
        """Test file I/O integration."""
        # Create and save config
        config = EnhancedConfig()
        config.set("brain.neuron_count", 1000)
        config.set("world.width", 30)
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
            config.save_to_file(config_file)
        
        # Load config
        loaded_config = EnhancedConfig()
        loaded_config.load_from_file(config_file)
        
        # Verify loaded config
        self.assertEqual(loaded_config.get("brain.neuron_count"), 1000)
        self.assertEqual(loaded_config.get("world.width"), 30)
        
        os.unlink(config_file)
    
    def test_learning_systems_integration(self):
        """Test learning systems integration."""
        # Create brain and agent
        config = EnhancedConfig()
        config.set("brain.neuron_count", 200)
        
        brain = EnhancedBrain(config)
        brain.initialize()
        
        world = EnhancedSimpleWorld()
        world.configure(width=20, height=20, vision_width=8, vision_height=8)
        world.reset()
        
        agent = EnhancedAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning systems
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        agent.enable_structural_plasticity(True)
        
        # Run simulation with learning
        for step in range(5):
            world.update(0.1)
            percept = world.getSensoryPercept()
            action = agent.processSensoryInput(percept)
            world.applyMotorCommand(action, world.simulation_time)
            brain.step(step)
            
            # Apply learning
            if step % 2 == 0:
                agent.applyRewardModulation(1.0, 0.5)
        
        # Verify learning systems affected agent state
        self.assertGreaterEqual(agent.curiosity_level, 0.0)
        self.assertGreaterEqual(agent.reward_level, 0.0)


class TestPerformance(unittest.TestCase):
    """Test performance benchmarks."""
    
    def test_brain_performance(self):
        """Test brain performance."""
        config = EnhancedConfig()
        config.set("brain.neuron_count", 1000)
        
        brain = EnhancedBrain(config)
        brain.initialize()
        
        # Time simulation steps
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        # Verify performance (should complete within reasonable time)
        elapsed_time = end_time - start_time
        steps_per_second = 100 / elapsed_time if elapsed_time > 0 else 0
        self.assertGreater(steps_per_second, 10.0)  # At least 10 steps per second
    
    def test_memory_usage(self):
        """Test memory usage."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create many objects
        configs = [EnhancedConfig() for _ in range(10)]
        brains = [EnhancedBrain(config) for config in configs]
        
        for brain in brains:
            brain.initialize()
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable (< 100 MB for 10 brains)
        self.assertLess(memory_increase, 100.0)


def run_all_tests():
    """Run all tests and generate report."""
    print("=" * 60)
    print("Running NLM Python Bindings Tests")
    print("=" * 60)
    print()
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Run tests with verbose output
    runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
    result = runner.run(suite)
    
    print()
    print("=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    
    if result.failures:
        print("\nFailures:")
        for test, traceback in result.failures:
            print(f"  {test}: {traceback}")
    
    if result.errors:
        print("\nErrors:")
        for test, traceback in result.errors:
            print(f"  {test}: {traceback}")
    
    # Return success status
    success = len(result.failures) == 0 and len(result.errors) == 0
    return success


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
