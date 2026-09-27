#!/usr/bin/env python3
"""
NLM Python Bindings Test Suite

Tests for the Neural Learning Machine (NLM) Python bindings.
This test suite validates the functionality exposed through pybind11.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm
    HAS_PYBINDINGS = True
except ImportError:
    HAS_PYBINDINGS = False
    print("Warning: pynlm module not found. Python binding tests skipped.")

from typing import Optional, List, Dict, Any
import unittest
import tempfile
import shutil
class TestPythonBindings:
    """Base class for Python binding tests."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
    
    def tearDown(self):
        """Clean up after tests."""
        pass
class TestConfigBindings(TestPythonBindings, unittest.TestCase):
    """Test Config class bindings."""
    
    def testConfigCreation(self):
        """Test basic config creation."""
        config = pynlm.Config()
        self.assertIsNotNone(config)
        
        # Test that empty config has no keys
        keys = config.getKeys()
        self.assertEqual(len(keys), 0)
    
    def testConfigSetGet(self):
        """Test setting and getting configuration values."""
        config = pynlm.Config()
        
        # Test string value
        config.set("string_key", "test_value")
        val = config.get("string_key")
        self.assertIsNotNone(val)
        self.assertEqual(val, "test_value")
        
        # Test integer value
        config.set("int_key", 42)
        val = config.get("int_key")
        self.assertIsNotNone(val)
        self.assertEqual(val, 42)
        
        # Test boolean value
        config.set("bool_key", True)
        val = config.get("bool_key")
        self.assertIsNotNone(val)
        self.assertTrue(val)
        
        # Test double value
        config.set("double_key", 3.14)
        val = config.get("double_key")
        self.assertIsNotNone(val)
        self.assertAlmostEqual(val, 3.14)
    
    def testConfigGetOr(self):
        """Test getOr method with default values."""
        config = pynlm.Config()
        
        # Test with non-existent key
        val = config.getOr("non_existent", "default_value")
        self.assertEqual(val, "default_value")
        
        # Test with existing key
        config.set("existing_key", "actual_value")
        val = config.getOr("existing_key", "default_value")
        self.assertEqual(val, "actual_value")
    
    def testConfigHas(self):
        """Test has method."""
        config = pynlm.Config()
        
        self.assertFalse(config.has("non_existent_key"))
        
        config.set("existing_key", 123)
        self.assertTrue(config.has("existing_key"))
    
    def testConfigRemove(self):
        """Test remove method."""
        config = pynlm.Config()
        
        config.set("key_to_remove", "value")
        self.assertTrue(config.has("key_to_remove"))
        
        config.remove("key_to_remove")
        self.assertFalse(config.has("key_to_remove"))
    
    def testConfigKeys(self):
        """Test getKeys method."""
        config = pynlm.Config()
        
        keys = config.getKeys()
        self.assertEqual(len(keys), 0)
        
        config.set("key1", "value1")
        config.set("key2", "value2")
        config.set("key3", "value3")
        
        keys = config.getKeys()
        self.assertEqual(len(keys), 3)
        self.assertIn("key1", keys)
        self.assertIn("key2", keys)
        self.assertIn("key3", keys)
    
    def testConfigClear(self):
        """Test clear method."""
        config = pynlm.Config()
        
        config.set("key1", "value1")
        config.set("key2", "value2")
        
        self.assertEqual(len(config.getKeys()), 2)
        
        config.clear()
        self.assertEqual(len(config.getKeys()), 0)
    
    def testConfigSummary(self):
        """Test summary method."""
        config = pynlm.Config()
        
        summary = config.summary()
        self.assertIsInstance(summary, str)
        self.assertIn("Configuration", summary)
    
    def testConfigBatchSet(self):
        """Test batch configuration loading."""
        config = pynlm.Config()
        
        # Test batch set (if available)
        if hasattr(config, 'batchSet'):
            settings = [
                ("batch_key1", "batch_value1"),
                ("batch_key2", "batch_value2"),
                ("batch_key3", 123)
            ]
            config.batchSet(settings)
            
            self.assertTrue(config.has("batch_key1"))
            self.assertTrue(config.has("batch_key2"))
            self.assertTrue(config.has("batch_key3"))
            self.assertEqual(config.get("batch_key3"), 123)
    
    def testConfigBatchRemove(self):
        """Test batch remove."""
        config = pynlm.Config()
        
        # Add some keys
        for i in range(5):
            config.set(f"key_{i}", f"value_{i}")
        
        # Batch remove
        if hasattr(config, 'batchRemove'):
            keys_to_remove = ["key_0", "key_2", "key_4"]
            config.batchRemove(keys_to_remove)
            
            self.assertFalse(config.has("key_0"))
            self.assertTrue(config.has("key_1"))
            self.assertFalse(config.has("key_2"))
            self.assertTrue(config.has("key_3"))
            self.assertFalse(config.has("key_4"))
class TestBrainBindings(TestPythonBindings, unittest.TestCase):
    """Test Brain class bindings."""
    
    def testBrainCreation(self):
        """Test basic brain creation."""
        config = pynlm.Config()
        brain = pynlm.Brain(config)
        self.assertIsNotNone(brain)
    
    def testBrainInitialize(self):
        """Test brain initialization."""
        config = pynlm.Config()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        
        brain = pynlm.Brain(config)
        
        # Test initialization (if available)
        if hasattr(brain, 'initialize'):
            result = brain.initialize()
            self.assertTrue(result)
    
    def testBrainGetConfig(self):
        """Test getConfig method."""
        config = pynlm.Config()
        config.set("test_param", "test_value")
        
        brain = pynlm.Brain(config)
        
        # Test getConfig method
        brain_config = brain.getConfig()
        self.assertIsNotNone(brain_config)
        self.assertEqual(brain_config.get("test_param"), "test_value")
    
    def testBrainGetRegionCount(self):
        """Test getRegionCount method."""
        config = pynlm.Config()
        config.set("region_count", 3)
        
        brain = pynlm.Brain(config)
        
        # Initialize if possible
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        region_count = brain.getRegionCount()
        self.assertEqual(region_count, 3)
    
    def testBrainGetNeuronCount(self):
        """Test getTotalNeuronCount method."""
        config = pynlm.Config()
        config.set("neuron_count", 500)
        
        brain = pynlm.Brain(config)
        
        # Initialize if possible
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        neuron_count = brain.getTotalNeuronCount()
        self.assertEqual(neuron_count, 500)
    
    def testBrainGetSynapseCount(self):
        """Test getTotalSynapseCount method."""
        config = pynlm.Config()
        config.set("neuron_count", 100)
        
        brain = pynlm.Brain(config)
        
        # Initialize if possible
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        synapse_count = brain.getTotalSynapseCount()
        self.assertIsInstance(synapse_count, int)
        self.assertGreaterThanEqual(synapse_count, 0)
class TestAgentBrainBindings(TestPythonBindings, unittest.TestCase):
    """Test AgentBrain class bindings."""
    
    def testAgentBrainCreation(self):
        """Test AgentBrain creation."""
        config = pynlm.Config()
        brain = pynlm.Brain(config)
        
        agent_brain = pynlm.AgentBrain(brain)
        self.assertIsNotNone(agent_brain)
    
    def testAgentBrainInitialize(self):
        """Test AgentBrain initialization."""
        config = pynlm.Config()
        brain = pynlm.Brain(config)
        
        agent_brain = pynlm.AgentBrain(brain)
        
        # Create a simple world for testing
        world = pynlm.SimpleWorld()
        world.configure(100, 100, 20, 20)
        
        # Test initialization
        if hasattr(agent_brain, 'initialize'):
            agent_brain.initialize(world)
    
    def testAgentBrainProcessSensoryInput(self):
        """Test processSensoryInput method."""
        config = pynlm.Config()
        brain = pynlm.Brain(config)
        
        agent_brain = pynlm.AgentBrain(brain)
        
        # Create sensory input
        vision = pynlm.Vision(20, 20, 3)
        vision.setData([0.1] * 400)  # Fill with dummy data
        
        # Create sensory percept
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        
        # Test processing
        if hasattr(agent_brain, 'processSensoryInput'):
            agent_brain.processSensoryInput(percept)
class TestWorldBindings(TestPythonBindings, unittest.TestCase):
    """Test SimpleWorld class bindings."""
    
    def testWorldCreation(self):
        """Test world creation."""
        world = pynlm.SimpleWorld()
        self.assertIsNotNone(world)
    
    def testWorldConfigure(self):
        """Test world configuration."""
        world = pynlm.SimpleWorld()
        
        world.configure(100, 100, 20, 20)
        
        width = world.getWidth()
        height = world.getHeight()
        vision_width = world.getVisionWidth()
        vision_height = world.getVisionHeight()
        
        self.assertEqual(width, 100)
        self.assertEqual(height, 100)
        self.assertEqual(vision_width, 20)
        self.assertEqual(vision_height, 20)
    
    def testWorldReset(self):
        """Test world reset."""
        world = pynlm.SimpleWorld()
        world.configure(50, 50, 10, 10)
        
        # Set some state
        world.setAgentStart(25, 25)
        
        # Reset
        world.reset()
        
        # Check if state is reset
        self.assertEqual(world.getAgentBody().x, 0)
        self.assertEqual(world.getAgentBody().y, 0)
    
    def testWorldUpdate(self):
        """Test world update."""
        world = pynlm.SimpleWorld()
        world.configure(50, 50, 10, 10)
        
        # Update world
        world.update(0.001)
        
        # Check if time advanced
        time = world.getSimulationTime()
        self.assertGreaterEqual(time, 0.001)
class TestVisualizationBindings(TestPythonBindings, unittest.TestCase):
    """Test VisualizationInterface bindings."""
    
    def testVisualizationCreation(self):
        """Test visualization creation."""
        # Note: Visualization is implemented in C++, not exposed via Python bindings
        # This test would test the C++ visualization if exposed
        pass
class TestHelperFunctions(TestPythonBindings, unittest.TestCase):
    """Test helper functions."""
    
    def testCreateDefaultConfig(self):
        """Test createDefaultConfig helper."""
        if hasattr(pynlm, 'createDefaultConfig'):
            config = pynlm.createDefaultConfig()
            self.assertIsNotNone(config)
            self.assertIsInstance(config, pynlm.Config)
    
    def testCreateBrain(self):
        """Test createBrain helper."""
        config = pynlm.Config()
        if hasattr(pynlm, 'createBrain'):
            brain = pynlm.createBrain(config)
            self.assertIsNotNone(brain)
            self.assertIsInstance(brain, pynlm.Brain)
    
    def testCreateSimpleWorld(self):
        """Test createSimpleWorld helper."""
        if hasattr(pynlm, 'createSimpleWorld'):
            world = pynlm.createSimpleWorld()
            self.assertIsNotNone(world)
            self.assertIsInstance(world, pynlm.SimpleWorld)
    
    def testCreateAgentBrain(self):
        """Test createAgentBrain helper."""
        config = pynlm.Config()
        brain = pynlm.Brain(config)
        if hasattr(pynlm, 'createAgentBrain'):
            agent_brain = pynlm.createAgentBrain(brain)
            self.assertIsNotNone(agent_brain)
            self.assertIsInstance(agent_brain, pynlm.AgentBrain)
class TestErrorHandling(TestPythonBindings, unittest.TestCase):
    """Test error handling in Python bindings."""
    
    def testExceptionHandling(self):
        """Test that exceptions are properly raised."""
        # This test would check that C++ exceptions are properly converted
        # to Python exceptions in the bindings
        pass

if __name__ == '__main__':
    if HAS_PYBINDINGS:
        unittest.main()
    else:
        print("Skipping Python binding tests - pynlm module not available")
        sys.exit(0)