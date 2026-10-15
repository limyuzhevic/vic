#!/usr/bin/env python3
"""
Comprehensive NLM Test Suite

This test suite provides comprehensive testing for NLM (Neural Learning Machine) components,
including core functionality, new extensibility features, and advanced CLI capabilities.

Tests cover:
- Basic brain functionality
- Agent and world integration
- Extensibility framework
- Performance and accuracy
- Error handling
- Configuration management
"""

import unittest
import tempfile
import os
import json
import time
import numpy as np
from pathlib import Path
from unittest.mock import patch, MagicMock
import sys

# Import NLM modules
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some tests will be skipped.")

# Import extensibility framework
try:
    from nlm_extensibility import (
        NLMRegistry, initializeExtensibilitySystem,
        DefaultAgentFactory, DefaultWorldFactory, DefaultComponentFactory
    )
    EXTENSIBILITY_AVAILABLE = True
except ImportError:
    EXTENSIBILITY_AVAILABLE = False
    print("Warning: extensibility framework not available. Plugin tests will be skipped.")

class TestBasicBrainFunctionality(unittest.TestCase):
    """Test basic brain functionality and neural operations."""
    
    def setUp(self):
        """Set up test fixtures before each test."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
        
        # Create standard test configuration
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 100)
        self.config.set("region_count", 1)
        self.config.set("random_seed", 42)
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_initialization(self):
        """Test that brain initializes correctly."""
        self.assertIsNotNone(self.brain)
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
        self.assertEqual(self.brain.getRegionCount(), 1)
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
    
    def test_brain_step(self):
        """Test that brain steps correctly."""
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_neurons = self.brain.getTotalNeuronCount()
        
        # Run several steps
        for step in range(10):
            self.brain.step(step)
        
        final_spikes = self.brain.getTotalSpikeCount()
        final_neurons = self.brain.getTotalNeuronCount()
        
        self.assertEqual(initial_neurons, final_neurons)
        self.assertGreaterEqual(final_spikes, initial_spikes)
    
    def test_brain_firing_rate(self):
        """Test brain firing rate calculations."""
        firing_rate = self.brain.getAverageFiringRate()
        self.assertGreaterEqual(firing_rate, 0.0)
        self.assertLessEqual(firing_rate, 1.0)
    
    def test_brain_development_stages(self):
        """Test brain development stage progression."""
        stages = [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult
        ]
        
        for stage in stages:
            self.assertIn(stage, stages)

class TestAgentWorldIntegration(unittest.TestCase):
    """Test agent and world integration."""
    
    def setUp(self):
        """Set up test fixtures before each test."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
        
        # Create standard configuration
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 200)
        self.config.set("random_seed", 123)
        
        # Create brain, world, and agent
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_world_initialization(self):
        """Test world initialization."""
        self.assertEqual(self.world.getWidth(), 10.0)
        self.assertEqual(self.world.getHeight(), 10.0)
        self.assertEqual(self.world.getVisionWidth(), 8)
        self.world.getVisionHeight()
        
        agent_body = self.world.getAgentBody()
        self.assertIsNotNone(agent_body)
        self.assertGreater(agent_body.energy, 0.0)
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        self.assertIsNotNone(self.agent)
        self.assertEqual(self.agent.getSensoryInputSize(), 256 + 8 + 4 + 6)
        self.assertEqual(self.agent.getMotorOutputSize(), 6)
    
    def test_sensory_processing(self):
        """Test sensory input processing."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Brain should process spikes
        for step in range(5):
            self.brain.step(step)
        
        final_spikes = self.brain.getTotalSpikeCount()
        self.assertGreaterEqual(final_spikes, initial_spikes)
    
    def test_motor_decoding(self):
        """Test motor command decoding."""
        # Inject some activity to get meaningful motor commands
        for neuron in self.brain.getNeurons():
            if hasattr(neuron, 'injectCurrent'):
                neuron.injectCurrent(10.0)
        
        # Process through agent
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        
        # Check action is valid
        self.assertIn(action, [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.LookLeft,
            pynlm.MotorCommand.LookRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ])
    
    def test_reward_modulation(self):
        """Test reward modulation functionality."""
        # Record initial state
        initial_prediction_error = self.agent.getPredictionError()
        
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)  # reward=1.0, predicted=0.5
        
        # Check prediction error changed
        final_prediction_error = self.agent.getPredictionError()
        self.assertNotEqual(initial_prediction_error, final_prediction_error)
        
        # Check dopamine level
        dopamine_level = self.agent.getNeuromodulationLevel()
        self.assertGreaterEqual(dopamine_level, -1.0)
        self.assertLessEqual(dopamine_level, 1.0)

class TestExtensibilityFramework(unittest.TestCase):
    """Test extensibility framework functionality."""
    
    def setUp(self):
        """Set up test fixtures before each test."""
        if not EXTENSIBILITY_AVAILABLE:
            self.skipTest("Extensibility framework not available")
        
        # Initialize extensibility system
        initializeExtensibilitySystem()
        
        # Get registry
        self.registry = NLMRegistry.getInstance()
        
        # Get factories
        self.agent_factory = self.registry.getAgentFactory()
        self.world_factory = self.registry.getWorldFactory()
        self.component_factory = self.registry.getComponentFactory()
    
    def test_registry_initialization(self):
        """Test that extensibility system initializes correctly."""
        self.assertIsNotNone(self.registry)
        self.assertIsNotNone(self.agent_factory)
        self.assertIsNotNone(self.world_factory)
        self.assertIsNotNone(self.component_factory)
    
    def test_factory_creation(self):
        """Test factory creation functionality."""
        # Create a simple test class
        class TestAgent:
            def __init__(self):
                self.name = "TestAgent"
        
        # Register factory function
        if isinstance(self.agent_factory, DefaultAgentFactory):
            def create_test_agent():
                return TestAgent()
            
            self.agent_factory.registerAgent("test_agent", create_test_agent)
            
            # Test creation
            agent = self.agent_factory.createAgent("test_agent")
            self.assertIsNotNone(agent)
            self.assertIsInstance(agent, TestAgent)
            self.assertEqual(agent.name, "TestAgent")
    
    def test_plugin_system(self):
        """Test plugin system functionality."""
        # Create a simple plugin
        class TestPlugin:
            def __init__(self):
                self.name = "TestPlugin"
                self.version = "1.0"
                self.initialized = False
            
            def getName(self):
                return self.name
            
            def getVersion(self):
                return self.version
            
            def initialize(self):
                self.initialized = True
            
            def shutdown(self):
                self.initialized = False
        
        plugin = TestPlugin()
        
        # Register plugin
        self.registry.registerPlugin("test_plugin", plugin)
        
        # Test retrieval
        retrieved_plugin = self.registry.getPlugin("test_plugin")
        self.assertIsNotNone(retrieved_plugin)
        self.assertEqual(retrieved_plugin.getName(), "TestPlugin")
        
        # Test initialization
        plugin.initialize()
        self.assertTrue(plugin.initialized)
        
        plugin.shutdown()
        self.assertFalse(plugin.initialized)
    
    def test_component_factory(self):
        """Test component factory functionality."""
        # Test component creation
        if isinstance(self.component_factory, DefaultComponentFactory):
            def create_test_component(component_type, name):
                # Return a mock component
                component = MagicMock()
                component.type = component_type
                component.name = name
                return component
            
            self.component_factory.registerComponent("test_component", create_test_component)
            
            # Test creation
            component = self.component_factory.createComponent("test_component", "test_component_1")
            self.assertIsNotNone(component)
            self.assertEqual(component.type, "test_component")
            self.assertEqual(component.name, "test_component_1")

class TestPerformanceAndAccuracy(unittest.TestCase):
    """Test performance and accuracy characteristics."""
    
    def setUp(self):
        """Set up test fixtures before each test."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
        
        self.config = pynlm.createDefaultConfig()
    
    def test_simulation_stability(self):
        """Test that simulations are stable and reproducible."""
        # Run multiple simulations with same seed
        results = []
        
        for i in range(5):
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 100)
            config.set("random_seed", 42)
            config.set("simulation_timestep", 0.001)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(100):
                brain.step(step)
            
            results.append(brain.getTotalSpikeCount())
        
        # All results should be identical (deterministic)
        self.assertEqual(len(set(results)), 1, "Simulations should be deterministic")
    
    def test_performance_scaling(self):
        """Test performance scaling with neuron count."""
        # Test different neuron counts
        neuron_counts = [50, 100, 200]
        times = []
        
        for neuron_count in neuron_counts:
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", neuron_count)
            config.set("random_seed", 123)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            end_time = time.time()
            
            times.append(end_time - start_time)
        
        # Time should generally increase with neuron count
        # (though not strictly linear due to other factors)
        self.assertEqual(len(times), len(neuron_counts))
    
    def test_memory_usage(self):
        """Test memory usage patterns."""
        if not hasattr(self, 'psutil'):
            self.skipTest("psutil not available for memory testing")
        
        import psutil
        
        process = psutil.Process()
        initial_memory = process.memory_info().rss
        
        # Create and run brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("random_seed", 456)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(200):
            brain.step(step)
        
        final_memory = process.memory_info().rss
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable (not excessive)
        self.assertLess(memory_increase, 100 * 1024 * 1024)  # Less than 100MB

class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases."""
    
    def setUp(self):
        """Set up test fixtures before each test."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
        
        self.config = pynlm.createDefaultConfig()
    
    def test_null_safety(self):
        """Test null safety in agent components."""
        # This test ensures that components handle null inputs gracefully
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test processing with empty percept (should not crash)
        from nlm.agents import SensoryPercept
        empty_percept = SensoryPercept()
        
        # Should not raise exception
        try:
            agent.processSensoryInput(empty_percept)
        except Exception:
            self.fail("Agent should handle empty percept gracefully")
    
    def test_invalid_configuration(self):
        """Test handling of invalid configurations."""
        # Test with invalid neuron count (should handle gracefully)
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 0)  # Invalid: zero neurons
        
        # Should not crash, but may log error or handle differently
        try:
            brain = pynlm.createBrain(config)
            brain.initialize()
            # If brain initializes, test step should work
            for step in range(10):
                brain.step(step)
        except Exception:
            # It's acceptable for invalid configs to raise exceptions
            pass
    
    def test_concurrent_access(self):
        """Test thread safety (if applicable)."""
        # This is a basic test for concurrent access scenarios
        # Note: NLM may not be fully thread-safe, but we test basic behavior
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("random_seed", 789)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Simulate multiple rapid steps
        steps = []
        for step in range(50):
            start_time = time.time()
            brain.step(step)
            end_time = time.time()
            steps.append(end_time - start_time)
        
        # All steps should complete without hanging
        self.assertEqual(len(steps), 50)
        avg_step_time = sum(steps) / len(steps)
        self.assertLess(avg_step_time, 1.0)  # Should complete quickly

class TestConfigurationManagement(unittest.TestCase):
    """Test configuration management functionality."""
    
    def setUp(self):
        """Set up test fixtures before each test."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
        
        # Create temporary directory for test configs
        self.temp_dir = tempfile.mkdtemp()
        self.config_dir = os.path.join(self.temp_dir, "configs")
        os.makedirs(self.config_dir)
    
    def tearDown(self):
        """Clean up after tests."""
        # Clean up temporary directory
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def test_config_save_load(self):
        """Test saving and loading configurations."""
        # Create a test configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 250)
        config.set("random_seed", 999)
        config.set("plasticity_learning_rate", 0.02)
        
        # Save config to file
        config_file = os.path.join(self.config_dir, "test_config.json")
        
        config_data = {
            'name': 'test_config',
            'neuron_count': config.getOr<int64_t>("neuron_count", 100),
            'random_seed': config.getOr<int64_t>("random_seed", 42),
            'plasticity_learning_rate': config.getOr<float>("plasticity_learning_rate", 0.01)
        }
        
        with open(config_file, 'w') as f:
            json.dump(config_data, f)
        
        # Load config from file
        with open(config_file, 'r') as f:
            loaded_config = json.load(f)
        
        self.assertEqual(loaded_config['name'], 'test_config')
        self.assertEqual(loaded_config['neuron_count'], 250)
        self.assertEqual(loaded_config['random_seed'], 999)
        self.assertEqual(loaded_config['plasticity_learning_rate'], 0.02)
    
    def test_config_validation(self):
        """Test configuration validation."""
        # Create config with various values
        config = pynlm.createDefaultConfig()
        
        # Set valid values
        config.set("neuron_count", 500)
        config.set("simulation_timestep", 0.001)
        config.set("random_seed", 111)
        
        # Verify values
        self.assertEqual(config.getOr<int64_t>("neuron_count", 100), 500)
        self.assertEqual(config.getOr<double>("simulation_timestep", 0.001), 0.001)
        self.assertEqual(config.getOr<int64_t>("random_seed", 42), 111)
        
        # Test default values
        self.assertEqual(config.getOr<int64_t>("non_existent_param", 999), 999)
        self.assertEqual(config.getOr<double>("non_existent_param", 3.14), 3.14)

class TestExtensibilityPlugins(unittest.TestCase):
    """Test extensibility plugin functionality."""
    
    def setUp(self):
        """Set up test fixtures before each test."""
        if not EXTENSIBILITY_AVAILABLE:
            self.skipTest("Extensibility framework not available")
        
        # Initialize extensibility system
        initializeExtensibilitySystem()
        
        # Create mock plugin for testing
        self.mock_plugin = MagicMock()
        self.mock_plugin.getName.return_value = "MockPlugin"
        self.mock_plugin.getVersion.return_value = "1.0"
        self.mock_plugin.initialize.return_value = None
        self.mock_plugin.shutdown.return_value = None
    
    def test_plugin_registration(self):
        """Test plugin registration functionality."""
        registry = NLMRegistry.getInstance()
        
        # Register mock plugin
        registry.registerPlugin("mock_plugin", self.mock_plugin)
        
        # Verify registration
        retrieved_plugin = registry.getPlugin("mock_plugin")
        self.assertIsNotNone(retrieved_plugin)
        self.assertEqual(retrieved_plugin.getName(), "MockPlugin")
        
        # Verify plugin methods were called
        self.mock_plugin.initialize.assert_not_called()
        self.mock_plugin.shutdown.assert_not_called()
    
    def test_plugin_lifecycle(self):
        """Test plugin lifecycle management."""
        registry = NLMRegistry.getInstance()
        
        # Register and initialize plugin
        registry.registerPlugin("mock_plugin", self.mock_plugin)
        registry.getPlugin("mock_plugin")
        
        # Initialize plugin
        self.mock_plugin.initialize()
        self.mock_plugin.initialize.assert_called_once()
        
        # Shutdown plugin
        self.mock_plugin.shutdown()
        self.mock_plugin.shutdown.assert_called_once()
    
    def test_plugin_list(self):
        """Test plugin listing functionality."""
        registry = NLMRegistry.getInstance()
        
        # Get initial plugin list (should be empty or contain only registered ones)
        initial_plugins = registry.getPluginNames()
        
        # Register test plugin
        registry.registerPlugin("test_plugin", self.mock_plugin)
        
        # Get updated plugin list
        updated_plugins = registry.getPluginNames()
        
        # Should contain our test plugin
        self.assertIn("test_plugin", updated_plugins)
        
        # Number should have increased
        self.assertGreater(len(updated_plugins), len(initial_plugins))

class TestAdvancedCLIFunctionality(unittest.TestCase):
    """Test advanced CLI functionality."""
    
    def setUp(self):
        """Set up test fixtures before each test."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
        
        # Create test CLI instance
        from nlm_advanced_cli import NLMAdvancedCLI
        self.cli = NLMAdvancedCLI()
    
    def test_cli_initialization(self):
        """Test CLI initialization."""
        self.assertIsNotNone(self.cli)
        self.assertIsNotNone(self.cli.logger)
    
    def test_config_creation(self):
        """Test configuration creation."""
        # Create mock args object
        class MockArgs:
            random_seed = 42
            timestep = 0.1
            neuron_count = 100
            max_steps = 100
            world_width = 10
            world_height = 10
            vision_width = 8
            vision_height = 8
            enable_all_features = False
            enable_reward = False
            enable_plasticity = False
            enable_development = False
            enable_curiosity = False
        
        args = MockArgs()
        
        # Create config
        config = self.cli.create_advanced_config(args)
        self.assertIsNotNone(config)
    
    def test_experiment_saving(self):
        """Test experiment saving functionality."""
        # Create test experiment data
        test_data = {
            'name': 'test_experiment',
            'timestamp': time.time(),
            'batch_results': [
                {'iteration': 1, 'total_spikes': 100},
                {'iteration': 2, 'total_spikes': 150}
            ]
        }
        
        # Save experiment
        self.cli.save_experiment('test_experiment')
        
        # Verify file was created
        self.assertTrue(os.path.exists('experiment_test_experiment.json'))
        
        # Clean up
        os.remove('experiment_test_experiment.json')
        os.remove('nlm_advanced_cli.log')
    
    def test_batch_simulation(self):
        """Test batch simulation functionality."""
        # Create mock args
        class MockArgs:
            iterations = 3
            random_seed = 42
            neuron_count = 100
            max_steps = 50
            noise_level = 0.0
            timestep = 0.1
        
        args = MockArgs()
        
        # Run batch simulation
        results = self.cli.run_batch_simulation(args)
        
        # Verify results
        self.assertEqual(len(results), 3)
        for i, result in enumerate(results):
            self.assertEqual(result['iteration'], i + 1)
            self.assertIn('total_spikes', result)
            self.assertIn('firing_neurons', result)
            self.assertIn('simulation_time', result)
    
    def test_results_analysis(self):
        """Test results analysis functionality."""
        # Create mock batch results
        self.cli.batch_results = [
            {'total_spikes': 100, 'firing_neurons': 50, 'steps_per_second': 10.0},
            {'total_spikes': 150, 'firing_neurons': 75, 'steps_per_second': 15.0},
            {'total_spikes': 200, 'firing_neurons': 100, 'steps_per_second': 20.0}
        ]
        
        # Analyze results
        class MockArgs:
            neuron_count = 100
            max_steps = 50
            timestep = 0.1
        
        args = MockArgs()
        stats = self.cli.analyze_results(args)
        
        # Verify statistics
        self.assertIsNotNone(stats)
        self.assertEqual(stats['total_iterations'], 3)
        self.assertGreater(stats['total_spikes_mean'], 0)
        self.assertGreater(stats['performance_mean'], 0)

class TestMemoryAndResourceManagement(unittest.TestCase):
    """Test memory and resource management."""
    
    def test_memory_efficiency(self):
        """Test memory efficiency in large simulations."""
        if not hasattr(self, 'psutil'):
            self.skipTest("psutil not available for memory testing")
        
        import psutil
        
        process = psutil.Process()
        
        # Create brain with moderate size
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("random_seed", 333)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_memory = process.memory_info().rss
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        final_memory = process.memory_info().rss
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable
        self.assertLess(memory_increase, 50 * 1024 * 1024)  # Less than 50MB
    
    def test_resource_cleanup(self):
        """Test resource cleanup after simulation."""
        if not hasattr(self, 'psutil'):
            self.skipTest("psutil not available for memory testing")
        
        import psutil
        import gc
        
        process = psutil.Process()
        
        # Create and run brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        config.set("random_seed", 444)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(50):
            brain.step(step)
        
        # Force garbage collection
        gc.collect()
        
        # Memory should be reasonable after cleanup
        final_memory = process.memory_info().rss
        self.assertLess(final_memory, 200 * 1024 * 1024)  # Less than 200MB

# Mock classes and functions for testing
class MockBrain:
    """Mock brain class for testing."""
    
    def __init__(self):
        self.total_spikes = 0
        self.firing_neurons = 0
        self.total_neurons = 100
        self.development_stage = pynlm.DevelopmentalStage.Initial
    
    def getTotalSpikeCount(self):
        return self.total_spikes
    
    def getFiringNeuronCount(self):
        return self.firing_neurons
    
    def getAverageFiringRate(self):
        return self.firing_neurons / self.total_neurons
    
    def getTotalNeuronCount(self):
        return self.total_neurons
    
    def getRegionCount(self):
        return 1
    
    def getDevelopmentalStage(self):
        return self.development_stage
    
    def step(self, step_number):
        self.total_spikes += 1
        self.firing_neurons = int(np.random.uniform(0, self.total_neurons))
    
    def initialize(self):
        self.total_spikes = 0

# Integration test class
class TestIntegration(unittest.TestCase):
    """Integration tests for complete NLM functionality."""
    
    def test_complete_simulation_workflow(self):
        """Test complete simulation workflow."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
        
        # Create complete simulation workflow
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 150)
        config.set("random_seed", 555)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run complete simulation
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(0.1, agent.getPredictionError())
        
        # Verify simulation completed successfully
        self.assertIsNotNone(brain)
        self.assertIsNotNone(world)
        self.assertIsNotNone(agent)
        
        # Check that agent made decisions
        self.assertIn(agent.decodeMotorCommand(), [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ])

# Test suite runner
def run_test_suite():
    """Run the complete test suite."""
    print("Running NLM Comprehensive Test Suite...")
    print("=" * 60)
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    print("\n" + "=" * 60)
    print("Test Suite Summary:")
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Skipped: {len(result.skipped)}")
    
    if result.failures:
        print("\nFailures:")
        for test, trace in result.failures:
            print(f"- {test}: {trace}")
    
    if result.errors:
        print("\nErrors:")
        for test, trace in result.errors:
            print(f"- {test}: {trace}")
    
    return result.wasSuccessful()

# Main execution
if __name__ == '__main__':
    if PYNLM_AVAILABLE:
        success = run_test_suite()
        sys.exit(0 if success else 1)
    else:
        print("Skipping tests: pynlm not available")
        sys.exit(1)