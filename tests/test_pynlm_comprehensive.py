#!/usr/bin/env python3
"""
Comprehensive test suite for pynlm Python bindings.

This module tests all major functionality of the NLM Python API including:
- Brain creation and initialization
- World integration and agent interaction
- Memory systems and neuromodulation
- Advanced features and utilities
- Error handling and edge cases
"""

import unittest
import tempfile
import os
import sys
import time
import math
from typing import List, Dict, Any

# Import pynlm
import pynlm
class TestBasicBrainFunctionality(unittest.TestCase):
    """Test basic brain creation and initialization."""
    
    def test_brain_creation(self):
        """Test basic brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        self.assertIsNotNone(brain)
        self.assertEqual(brain.getTotalNeuronCount(), 1000)  # Default config
        
    def test_brain_initialization(self):
        """Test brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Brain should not be initialized yet
        # (implementation-dependent)
        
        # Initialize brain
        brain.initialize()
        
        # After initialization, brain should be ready
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        
    def test_brain_step(self):
        """Test brain step execution."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run several steps
        initial_spikes = brain.getTotalSpikeCount()
        
        for i in range(10):
            brain.step(i)
        
        self.assertGreater(brain.getTotalSpikeCount(), initial_spikes)
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        
    def test_brain_reset(self):
        """Test brain reset functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for i in range(5):
            brain.step(i)
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Reset brain
        brain.reset()
        brain.initialize()
        
        # After reset, spikes should be different
        self.assertNotEqual(brain.getTotalSpikeCount(), initial_spikes)
class TestConfigFunctionality(unittest.TestCase):
    """Test configuration management."""
    
    def test_config_creation(self):
        """Test default config creation."""
        config = pynlm.createDefaultConfig()
        
        self.assertIsNotNone(config)
        self.assertTrue(config.has("brain.neuron_count"))
        self.assertEqual(config.getOr("brain.neuron_count", 1000), 1000)
        
    def test_config_modification(self):
        """Test config modification."""
        config = pynlm.createDefaultConfig()
        
        # Modify config
        config.set("brain.neuron_count", 2000)
        self.assertEqual(config.getOr("brain.neuron_count", 0), 2000)
        
        # Test other types
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("learning.rate", 0.001)
        
    def test_config_get_keys(self):
        """Test getting all config keys."""
        config = pynlm.createDefaultConfig()
        keys = config.getKeys()
        
        self.assertIsInstance(keys, list)
        self.assertGreater(len(keys), 0)
        self.assertIn("brain.neuron_count", keys)
        
    def test_config_summary(self):
        """Test config summary generation."""
        config = pynlm.createDefaultConfig()
        summary = config.summary()
        
        self.assertIsInstance(summary, str)
        self.assertGreater(len(summary), 0)
        self.assertIn("Configuration", summary)
        
    def test_config_save_load(self):
        """Test config save and load functionality."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("learning.rate", 0.01)
        
        # Save to temp file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            # Save config
            self.assertTrue(config.saveToFile(filepath))
            
            # Verify file was created and has content
            self.assertGreater(os.path.getsize(filepath), 0)
            
            # Create new config and load
            config2 = pynlm.createDefaultConfig()
            self.assertTrue(config2.loadFromFile(filepath))
            
            # Check that key values were preserved
            self.assertEqual(config2.getOr("brain.neuron_count", 0), 3000)
            self.assertEqual(config2.getOr("learning.rate", 0.0), 0.01)
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
class TestWorldFunctionality(unittest.TestCase):
    """Test world simulation and interaction."""
    
    def test_world_creation(self):
        """Test world creation."""
        world = pynlm.createSimpleWorld()
        
        self.assertIsNotNone(world)
        self.assertEqual(world.getWidth(), 0)  # Default
        self.assertEqual(world.getHeight(), 0)  # Default
        
    def test_world_configuration(self):
        """Test world configuration."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        self.assertEqual(world.getWidth(), 20)
        self.assertEqual(world.getHeight(), 20)
        self.assertEqual(world.getWidth(), world.getVisionWidth() * 2)  # 8 * 2.5
        
    def test_world_reset(self):
        """Test world reset functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # Set agent position
        world.setAgentStart(5.0, 5.0)
        
        # Reset world
        world.reset()
        
        # Agent body should be reset
        body = world.getAgentBody()
        self.assertGreaterEqual(body.x, 0.0)
        self.assertGreaterEqual(body.y, 0.0)
        
    def test_world_update(self):
        """Test world update."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Update world
        initial_time = world.getSimulationTime()
        world.update(0.1)
        
        self.assertGreater(world.getSimulationTime(), initial_time)
        
    def test_sensory_percept(self):
        """Test sensory percept functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        self.assertIsNotNone(percept)
        
        # Check vision data
        vision = percept.getVision()
        self.assertIsNotNone(vision)
        self.assertEqual(vision.getWidth(), 8)
        self.assertEqual(vision.getHeight(), 8)
        
    def test_agent_body(self):
        """Test agent body state."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        body = world.getAgentBody()
        
        self.assertIsNotNone(body)
        self.assertGreaterEqual(body.x, 0.0)
        self.assertGreaterEqual(body.y, 0.0)
        self.assertGreaterEqual(body.orientation, 0.0)
        self.assertLessEqual(body.orientation, 360.0)
class TestAgentFunctionality(unittest.TestCase):
    """Test agent-brain integration."""
    
    def test_agent_creation(self):
        """Test agent creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        self.assertIsNotNone(agent)
        self.assertEqual(agent.getBrain(), brain)
        
    def test_agent_initialization(self):
        """Test agent initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Agent should be initialized
        self.assertIsNotNone(agent.getBrain())
        self.assertEqual(agent.getBrain(), brain)
        
    def test_sensory_processing(self):
        """Test sensory input processing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain should have received the input
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        
    def test_motor_decoding(self):
        """Test motor command decoding."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run brain step to generate activity
        brain.step(0)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Should get a valid motor command
        self.assertIsNotNone(motor_cmd)
        
    def test_reward_modulation(self):
        """Test reward modulation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Enable reward modulation
        agent.enableRewardModulation(True)
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Check neuromodulation level
        neuromod_level = agent.getNeuromodulationLevel()
        self.assertGreaterEqual(neuromod_level, 0.0)
        
    def test_development(self):
        """Test development system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableDevelopment(True)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Check developmental stage
        dev_stage = agent.getDevelopmentalStage()
        self.assertGreaterEqual(dev_stage, 0)
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced NLM features."""
    
    def test_plasticity_features(self):
        """Test plasticity system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable plasticity subsystems
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation with plasticity
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        # Should have some learning metrics
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0)
        
    def test_memory_systems(self):
        """Test integrated memory systems."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Run simulation to generate memories
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
        # Memory should have been updated
        # (specific metrics depend on implementation)
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        
    def test_prediction_system(self):
        """Test prediction system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Get prediction error
        prediction_error = agent.getPredictionError()
        self.assertGreaterEqual(prediction_error, 0.0)
        
    def test_serialization(self):
        """Test brain state serialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        # Save to temp file
        with tempfile.NamedTemporaryFile(mode='wb', suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain
            brain.save(filepath)
            
            # Verify file was created
            self.assertGreater(os.path.getsize(filepath), 0)
            
            # Create new brain and load
            brain2 = pynlm.createBrain(config)
            brain2.load(filepath)
            
            # Both brains should have similar state
            self.assertEqual(brain2.getTotalNeuronCount(), brain.getTotalNeuronCount())
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases."""
    
    def test_invalid_file_paths(self):
        """Test handling of invalid file paths."""
        config = pynlm.createDefaultConfig()
        
        # Try to save to invalid path
        self.assertFalse(config.saveToFile("/invalid/path/that/does/not/exist/config.json"))
        
        # Try to load non-existent file
        self.assertFalse(config.loadFromFile("/nonexistent/file.json"))
        
    def test_brain_step_before_initialize(self):
        """Test brain step before initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should handle step before initialize gracefully
        try:
            brain.step(0)
            # If it doesn't crash, that's fine
        except Exception:
            # Exception might be expected
            pass
            
    def test_agent_without_world(self):
        """Test agent without world initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize without world should be handled
        try:
            agent.initialize(None)
        except Exception:
            # Expected behavior
            pass
            
    def test_config_edge_cases(self):
        """Test config edge cases."""
        config = pynlm.createDefaultConfig()
        
        # Set and get various types
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        config.set("bool_value", True)
        config.set("string_value", "test")
        
        self.assertEqual(config.getOr("int_value", 0), 42)
        self.assertAlmostEqual(config.getOr("float_value", 0.0), 3.14)
        self.assertTrue(config.getOr("bool_value", False))
        self.assertEqual(config.getOr("string_value", ""), "test")
        
    def test_large_simulation(self):
        """Test large simulation without crashing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run larger simulation
        start_time = time.time()
        for step in range(200):
            world.update(0.05)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        end_time = time.time()
        
        # Should complete without crashing
        self.assertLess(end_time - start_time, 60.0)  # Should be much faster
class TestPerformance(unittest.TestCase):
    """Performance tests for NLM."""
    
    def test_brain_creation_performance(self):
        """Test brain creation performance."""
        import time
        
        start_time = time.time()
        
        for i in range(5):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
        end_time = time.time()
        
        # Should be reasonably fast
        avg_time = (end_time - start_time) / 5
        self.assertLess(avg_time, 1.0)  # Less than 1 second per brain
        
    def test_step_performance(self):
        """Test simulation step performance."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Measure step performance
        start_time = time.time()
        
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        end_time = time.time()
        
        # Should be reasonably fast
        avg_step_time = (end_time - start_time) / 100
        self.assertLess(avg_step_time, 0.1)  # Less than 100ms per step
if __name__ == '__main__':
    # Run tests with verbose output
    unittest.main(verbosity=2)