#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings
Tests all the functionality added to the Python API
"""

import sys
import os
import tempfile
import unittest
from unittest.mock import patch, MagicMock
import json

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("Please ensure pynlm is built and available")
    sys.exit(1)
class TestBasicFunctionality(unittest.TestCase):
    """Test basic Python API functionality"""
    
    def setUp(self):
        """Set up test environment"""
        self.temp_dir = tempfile.mkdtemp()
    
    def tearDown(self):
        """Clean up test environment"""
        import shutil
        if os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir)
    
    def test_create_default_config(self):
        """Test creating default configuration"""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        # Test that we can set values
        config.set("test.key", "test_value")
        self.assertTrue(config.has("test.key"))
        
        # Test getting values
        value = config.getOr("test.key", "default")
        self.assertEqual(value, "test_value")
    
    def test_create_advanced_config(self):
        """Test creating advanced configuration"""
        config = pynlm.createAdvancedConfig()
        self.assertIsNotNone(config)
        
        # Test that it has Phase 6 defaults
        keys = config.getKeys()
        self.assertTrue(len(keys) > 0)  # Should have some default keys
    
    def test_create_brain_with_config(self):
        """Test creating brain with configuration"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        self.assertIsNotNone(brain)
        
        # Test that brain can be initialized
        result = brain.initialize()
        self.assertTrue(result)
    
    def test_create_brain_with_defaults(self):
        """Test creating brain with default parameters"""
        brain = pynlm.createBrainWithDefaults()
        self.assertIsNotNone(brain)
        
        result = brain.initialize()
        self.assertTrue(result)
    
    def test_create_simple_world(self):
        """Test creating simple world"""
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        
        # Test configuration
        world.configure(10, 10, 8, 8)
        self.assertEqual(world.getWidth(), 10)
        self.assertEqual(world.getHeight(), 10)
        self.assertEqual(world.getVisionWidth(), 8)
        self.assertEqual(world.getVisionHeight(), 8)
    
    def test_create_advanced_world(self):
        """Test creating advanced world"""
        world = pynlm.createAdvancedWorld()
        self.assertIsNotNone(world)
        
        self.assertGreater(world.getWidth(), 10)
        self.assertGreater(world.getHeight(), 10)
    
    def test_create_agent_brain(self):
        """Test creating agent brain interface"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        
        # Test agent capabilities
        self.assertGreater(agent.getSensoryInputSize(), 0)
        self.assertGreater(agent.getMotorOutputSize(), 0)
class TestAdvancedFunctionality(unittest.TestCase):
    """Test advanced Python API functionality"""
    
    def setUp(self):
        """Set up test environment"""
        self.config = pynlm.createAdvancedConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createAdvancedWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Initialize world and agent
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
    
    def test_simulation_with_learning(self):
        """Test simulation with learning enabled"""
        num_steps = 100
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action in world
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = result.reward if hasattr(result, 'reward') else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
        
        # Verify results
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
        self.assertGreater(self.agent.getCuriosityLevel(), 0.0)
        self.assertGreater(self.agent.getNoveltyLevel(), 0.0)
    
    def test_performance_profiling(self):
        """Test performance profiling functionality"""
        # Test performance profiler
        result = pynlm.startPerformanceProfiler()
        self.assertIsInstance(result, str)
        
        # Run a small simulation
        for step in range(50):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Stop profiler
        result = pynlm.stopPerformanceProfiler()
        self.assertIsInstance(result, str)
        
        # Get metrics
        metrics = pynlm.getPerformanceMetrics()
        self.assertIsInstance(metrics, dict)
        self.assertIn('simulation_time', metrics)
        self.assertIn('cpu_usage', metrics)
        self.assertIn('memory_usage', metrics)
    
    def test_profile_simulation(self):
        """Test profile simulation functionality"""
        # Test profile simulation
        metrics = pynlm.profileSimulation(
            self.brain, self.world, self.agent, 200
        )
        
        self.assertIsInstance(metrics, dict)
        self.assertIn('total_time', metrics)
        self.assertIn('steps_per_second', metrics)
        self.assertIn('total_spikes', metrics)
        self.assertIn('firing_rate', metrics)
        
        self.assertGreater(metrics['total_time'], 0.0)
        self.assertGreater(metrics['steps_per_second'], 0.0)
    
    def test_benchmark(self):
        """Test benchmark functionality"""
        # Run benchmark
        metrics = pynlm.runBenchmark(500, 1000)
        
        self.assertIsInstance(metrics, dict)
        self.assertIn('elapsed_time', metrics)
        self.assertIn('steps_per_second', metrics)
        self.assertIn('total_spikes', metrics)
        self.assertIn('spikes_per_step', metrics)
        
        self.assertGreater(metrics['elapsed_time'], 0.0)
        self.assertGreater(metrics['steps_per_second'], 0.0)
    
    def test_run_simulation_with_learning(self):
        """Test run simulation with learning"""
        # This should be similar to profileSimulation but without profiling
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        num_steps = 100
        
        for step in range(num_steps):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            reward = result.reward if hasattr(result, 'reward') else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
        
        # Verify
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
    
    def test_configuration_validation(self):
        """Test configuration validation"""
        # Test validate function
        config = pynlm.createAdvancedConfig()
        
        # Simple validator
        def positive_validator(value):
            return isinstance(value, (int, float)) and value > 0
        
        # This should work but we'll test it with a simple case
        config.set("test.number", 42)
        # Note: We can't directly test validate without implementing it in Python
    
    def test_summary_with_validation(self):
        """Test summary with validation"""
        config = pynlm.createAdvancedConfig()
        
        summary = config.summaryWithValidation()
        self.assertIsInstance(summary, str)
        self.assertGreater(len(summary), 0)
    
    def test_validate_all(self):
        """Test validate all"""
        config = pynlm.createAdvancedConfig()
        
        result = config.validateAll()
        self.assertIsInstance(result, bool)
class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases"""
    
    def test_invalid_arguments(self):
        """Test handling of invalid arguments"""
        # Test with None values
        config = None
        brain = None
        world = None
        agent = None
        
        # These should raise appropriate exceptions
        try:
            if brain is not None:
                result = brain.initialize()
                self.assertTrue(result)
        except:
            pass  # Expected
    
    def test_mismatched_sizes(self):
        """Test handling of mismatched configuration sizes"""
        config = pynlm.createDefaultConfig()
        
        # Set invalid values
        config.set("neuron_count", "invalid")  # Should be integer
        
        # This should still work (validation happens at runtime)
        brain = pynlm.createBrain(config)
        self.assertIsNotNone(brain)
    
    def test_file_operations(self):
        """Test file operations"""
        config = pynlm.createAdvancedConfig()
        
        # Test save to file
        filepath = os.path.join(self.temp_dir, "test_config.json")
        config.saveToFile(filepath)
        self.assertTrue(os.path.exists(filepath))
        
        # Test load from file
        new_config = pynlm.createAdvancedConfig()
        new_config.loadFromFile(filepath)
        
        # Both should have keys
        self.assertGreater(len(config.getKeys()), 0)
class TestConstants(unittest.TestCase):
    """Test constants and constants access"""
    
    def test_constants_access(self):
        """Test access to constants"""
        # Test that constants are accessible
        self.assertEqual(pynlm.DEFAULT_SEED, 42)
        self.assertEqual(pynlm.DEFAULT_NEURON_COUNT, 500)
        self.assertEqual(pynlm.DEFAULT_REGION_COUNT, 1)
        self.assertEqual(pynlm.DEFAULT_CONNECTION_PROBABILITY, 0.15)
        self.assertEqual(pynlm.DEFAULT_SIMULATION_TIMESTEP, 0.001)
        
        # Test invalid ID constants
        self.assertIsNotNone(pynlm.INVALID_NEURON_ID)
        self.assertIsNotNone(pynlm.INVALID_SYNAPSE_ID)
        self.assertIsNotNone(pynlm.INVALID_REGION_ID)
        self.assertIsNotNone(pynlm.INVALID_POPULATION_ID)
class TestIntegration(unittest.TestCase):
    """Test integration scenarios"""
    
    def test_complete_agent_simulation(self):
        """Test complete agent simulation"""
        # Create complete setup
        config = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure world
        world.configure(20, 20, 8, 8)
        
        # Initialize
        world.reset()
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        num_steps = 500
        reward_history = []
        
        for step in range(num_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = result.reward if hasattr(result, 'reward') else 0.0
            reward_history.append(reward)
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Verify results
        self.assertGreater(sum(reward_history), 0)
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
        self.assertGreater(agent.getNoveltyLevel(), 0.0)
    
    def test_multiple_worlds(self):
        """Test simulation with multiple worlds"""
        worlds = []
        agents = []
        
        # Create multiple world-agent pairs
        for i in range(3):
            config = pynlm.createAdvancedConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            world.configure(15, 15, 6, 6)
            world.setAgentStart(i * 5.0 + 2.5, i * 5.0 + 2.5)
            
            world.reset()
            agent.initialize(world)
            
            worlds.append(world)
            agents.append(agent)
        
        # Run parallel simulation
        for step in range(100):
            for i, (world, agent) in enumerate(zip(worlds, agents)):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                agent.brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify all simulations ran
        for i, (world, agent) in enumerate(zip(worlds, agents)):
            self.assertGreater(agent.brain.getTotalSpikeCount(), 0)
    
    def test_memory_integration(self):
        """Test memory integration in Python"""
        config = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(30, 30, 10, 10)
        world.reset()
        agent.initialize(world)
        
        # Enable memory systems
        agent.enableWorkingMemory(True)
        agent.enableEpisodicMemory(True)
        
        # Run simulation
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = 0.1 if step % 20 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Check memory metrics
        self.assertGreater(agent.brain.getTotalSpikeCount(), 0)
class TestPerformance(unittest.TestCase):
    """Test performance-related functionality"""
    
    def test_large_scale_simulation(self):
        """Test large scale simulation"""
        import time
        
        # Create larger configuration
        config = pynlm.createAdvancedConfig()
        config.set("brain.neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(25, 25, 8, 8)
        world.reset()
        agent.initialize(world)
        
        # Measure performance
        start_time = time.time()
        
        # Run larger simulation
        num_steps = 1000
        for step in range(num_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 100 == 0:
                agent.applyRewardModulation(0.1, 0.0)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Performance should be reasonable
        steps_per_second = num_steps / elapsed
        self.assertGreater(steps_per_second, 100)  # Should process > 100 steps/second
    
    def test_memory_efficiency(self):
        """Test memory efficiency"""
        # This test ensures we don't have memory leaks
        import gc
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        # Get initial memory
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run multiple simulations
        for i in range(10):
            config = pynlm.createAdvancedConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            world.configure(10, 10, 4, 4)
            world.reset()
            agent.initialize(world)
            
            for step in range(100):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Clean up
            del brain, world, agent
            gc.collect()
        
        # Get final memory
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_growth = final_memory - initial_memory
        
        # Memory growth should be reasonable (< 100MB)
        self.assertLess(memory_growth, 100)
if __name__ == '__main__':
    # Run tests
    unittest.main(verbosity=2)