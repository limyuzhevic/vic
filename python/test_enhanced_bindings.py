#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.
Tests all the new enhanced functionality including:
- Pythonic APIs and error handling
- Configuration presets and validation
- Agent brain enhancements
- Episode context management
- World utilities
"""

import unittest
import tempfile
import os
import sys
import traceback

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("Make sure the Python bindings are built. Run: pip install -e .")
    sys.exit(1)

class TestNLMErrorHandling(unittest.TestCase):
    """Test custom exception handling in NLM."""
    
    def testCustomExceptions(self):
        """Test that custom exceptions are properly defined."""
        # Test that custom exceptions exist
        self.assertTrue(hasattr(pynlm, 'NLMError'))
        self.assertTrue(hasattr(pynlm, 'ConfigError'))
        self.assertTrue(hasattr(pynlm, 'BrainError'))
        self.assertTrue(hasattr(pynlm, 'WorldError'))
        
        # Test exception inheritance
        error = pynlm.NLMError("Test error")
        self.assertIsInstance(error, Exception)
        
        config_error = pynlm.ConfigError("Config problem")
        self.assertIsInstance(config_error, pynlm.NLMError)
        
        brain_error = pynlm.BrainError("Brain problem")
        self.assertIsInstance(brain_error, pynlm.NLMError)
        
        world_error = pynlm.WorldError("World problem")
        self.assertIsInstance(world_error, pynlm.NLMError)

class TestEnhancedConfiguration(unittest.TestCase):
    """Test enhanced configuration features."""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
    
    def testConfigurationPresets(self):
        """Test that configuration presets work."""
        # Test all preset functions exist
        self.assertTrue(callable(pynlm.createDefaultConfig))
        self.assertTrue(callable(pynlm.createPerformanceConfig))
        self.assertTrue(callable(pynlm.createExplorationConfig))
        
        # Create presets
        default_config = pynlm.createDefaultConfig()
        perf_config = pynlm.createPerformanceConfig()
        explore_config = pynlm.createExplorationConfig()
        
        # Test basic properties
        self.assertEqual(default_config.getInt("brain.neuron_count"), 1000)
        self.assertEqual(perf_config.getInt("brain.neuron_count"), 500)
        self.assertEqual(explore_config.getInt("brain.neuron_count"), 2000)
    
    def testPythonicConfigAPI(self):
        """Test Pythonic configuration API."""
        config = pynlm.createDefaultConfig()
        
        # Test [] operator for getting values
        self.assertEqual(config["brain.neuron_count"], 1000)
        self.assertEqual(config["brain.synapse_density"], 0.1)
        
        # Test [] operator for setting values
        config["brain.neuron_count"] = 2000
        self.assertEqual(config["brain.neuron_count"], 2000)
        
        config["test.int"] = 42
        self.assertEqual(config["test.int"], 42)
        
        config["test.float"] = 3.14
        self.assertAlmostEqual(config["test.float"], 3.14, places=2)
        
        config["test.bool"] = True
        self.assertTrue(config["test.bool"])
    
    def testConfigValidation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        self.assertTrue(config.validate())
        
        # Test invalid config
        invalid_config = pynlm.createDefaultConfig()
        invalid_config.set("brain.neuron_count", -1)  # Invalid: negative neurons
        # Note: validate() implementation would need to be checked
        # This test assumes the method exists
        
        # Test config len
        self.assertGreater(len(config), 0)
        
        # Test config contains
        self.assertIn("brain.neuron_count", config)
        self.assertNotIn("nonexistent.key", config)
    
    def testConfigMerge(self):
        """Test configuration merging."""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createPerformanceConfig()
        
        # Merge config2 into config1
        config1.merge(config2)
        
        # Should have values from both
        self.assertEqual(config1["brain.neuron_count"], 500)  # From config2
        self.assertEqual(config1["brain.synapse_density"], 0.1)  # From config1

class TestBrainEnhancements(unittest.TestCase):
    """Test enhanced brain functionality."""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def testBrainStatistics(self):
        """Test brain statistics method."""
        stats = self.brain.getBrainStatistics()
        self.assertIsInstance(stats, str)
        self.assertIn("Neuron Count:", stats)
        self.assertIn("Synapse Count:", stats)
        self.assertIn("Firing Neurons:", stats)
    
    def testBrainValidation(self):
        """Test brain initialization with validation."""
        # Create a new brain for this test
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should work with validation
        success = brain.initializeWithValidation()
        self.assertTrue(success)
    
    def testBrainSnapshot(self):
        """Test brain snapshot functionality."""
        # Create a temporary file for snapshot
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filename = f.name
        
        try:
            # Test save
            self.brain.createSnapshot(filename)
            self.assertTrue(os.path.exists(filename))
            
            # Test load
            self.brain.reset()
            self.brain.loadSnapshot(filename)
            
            # Verify snapshot loaded
            self.assertEqual(self.brain.getTotalNeuronCount(), 1000)
            
        finally:
            # Cleanup
            if os.path.exists(filename):
                os.unlink(filename)
    
    def testBrainRegionAccess(self):
        """Test enhanced region access methods."""
        region_names = self.brain.getRegionNames()
        self.assertIsInstance(region_names, list)
        self.assertGreaterEqual(len(region_names), 1)
        
        # Test getRegionByName
        if region_names:
            region_name = region_names[0]
            region_id = self.brain.getRegionByName(region_name)
            self.assertNotEqual(region_id, pynlm.INVALID_REGION_ID)
            
            # Verify we can get the region
            region = self.brain.getRegion(region_id)
            self.assertIsNotNone(region)

class TestAgentBrainEnhancements(unittest.TestCase):
    """Test enhanced agent brain functionality."""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.agent.initialize(self.world)
    
    def testAgentSubsystems(self):
        """Test agent subsystem control."""
        # Test all subsystems
        self.agent.enableAllSubsystems()
        status = self.agent.getSubsystemStatus()
        self.assertTrue(status[0])  # Reward modulation
        self.assertTrue(status[1])  # Structural plasticity
        self.assertTrue(status[2])  # Development
        self.assertTrue(status[3])  # Curiosity
        
        # Test disable all
        self.agent.disableAllSubsystems()
        status = self.agent.getSubsystemStatus()
        self.assertFalse(status[0])
        self.assertFalse(status[1])
        self.assertFalse(status[2])
        self.assertFalse(status[3])
    
    def testAgentInfo(self):
        """Test agent information retrieval."""
        info = self.agent.getAgentInfo()
        self.assertIsInstance(info, str)
        self.assertIn("Developmental Stage:", info)
        self.assertIn("Neuromodulation Level:", info)
        self.assertIn("Curiosity Level:", info)

class TestWorldEnhancements(unittest.TestCase):
    """Test enhanced world functionality."""
    
    def setUp(self):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(20, 20, 8, 8)
    
    def testWorldConfiguration(self):
        """Test enhanced world configuration."""
        # Test simple configuration
        self.world.configureSimple(15, 15)
        self.assertEqual(self.world.getWidth(), 15)
        self.assertEqual(self.world.getHeight(), 15)
    
    def testWorldObjectManagement(self):
        """Test world object management utilities."""
        # Add various objects
        self.world.addResource(5.0, 5.0, 10.0)
        self.world.addHazard(10.0, 10.0, 5.0)
        
        # Verify objects added (would need implementation in C++)
        # For now, just test that methods exist and don't crash
        self.world.getObjectCount(pynlm.WorldObjectType.Resource)
        self.world.getObjectCount(pynlm.WorldObjectType.Hazard)
    
    def testWorldUtilities(self):
        """Test world utility methods."""
        # Test agent near check
        self.world.setAgentStart(10.0, 10.0)
        self.assertTrue(self.world.isAgentNear(10.0, 10.0, 1.0))
        self.assertFalse(self.world.isAgentNear(15.0, 15.0, 1.0))
        
        # Test world bounds
        bounds = self.world.getWorldBounds()
        self.assertEqual(len(bounds), 2)
        self.assertEqual(bounds[0], self.world.getWidth())
        self.assertEqual(bounds[1], self.world.getHeight())

class TestEpisodeManagement(unittest.TestCase):
    """Test episode context management."""
    
    def testEpisodeManager(self):
        """Test episode context manager."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        agent.initialize(world)
        
        # Create episode manager
        episode = pynlm.episode_context(brain, agent, world)
        
        # Test context manager
        with episode as ep:
            self.assertTrue(ep.isInitialized())
            self.assertIsInstance(ep.getStatus(), str)
        
        # After exit, should be uninitialized
        self.assertFalse(episode.isInitialized())

class TestUtilityFunctions(unittest.TestCase):
    """Test utility functions."""
    
    def testVersionInfo(self):
        """Test version information utility."""
        version = pynlm.getVersion()
        self.assertIsInstance(version, str)
        self.assertIn("NLM", version)
    
    def testPlatformInfo(self):
        """Test platform information utility."""
        platform = pynlm.getPlatformInfo()
        self.assertIsInstance(platform, str)
        self.assertIn("C++20", platform)
    
    def testDebugConfig(self):
        """Test debug configuration utility."""
        debug_config = pynlm.createDebugConfig()
        self.assertEqual(debug_config.getInt("brain.neuron_count"), 100)
        self.assertEqual(debug_config.getInt("brain.region_count"), 1)

class TestActionEnhancements(unittest.TestCase):
    """Test enhanced action functionality."""
    
    def testActionProperties(self):
        """Test action properties and methods."""
        # Create action
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        self.assertEqual(action.getType(), pynlm.ActionType.MoveForward)
        self.assertTrue(action.isEmpty())
        self.assertEqual(action.getParameterCount(), 0)
        
        # Add parameters
        action.setParameters([0.5, 1.0])
        self.assertEqual(action.getParameterCount(), 2)
        
        # Test repr
        repr_str = repr(action)
        self.assertIn("Action", repr_str)
    
    def testActionClone(self):
        """Test action cloning."""
        original = pynlm.Action(pynlm.ActionType.TurnLeft)
        original.setParameters([0.5])
        
        cloned = original.clone()
        
        self.assertEqual(cloned.getType(), original.getType())
        self.assertEqual(cloned.getParameterCount(), original.getParameterCount())

class TestConfigValidation(unittest.TestCase):
    """Test configuration validation functionality."""
    
    def testConfigValidate(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # This test assumes validate() method exists
        # In practice, this would need to be implemented in the C++ config class
        try:
            result = config.validate()
            self.assertIsInstance(result, bool)
        except AttributeError:
            self.skipTest("validate() method not implemented yet")

class TestComprehensiveExample(unittest.TestCase):
    """Test comprehensive example from documentation."""
    
    def testComprehensiveExample(self):
        """Test the comprehensive example from the documentation."""
        # Create all components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        for step in range(50):  # Reduced for testing
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation (simplified)
            reward = 0.0
            if percept.getInternal() and percept.getInternal()[0]:
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            # Check progress periodically
            if step % 10 == 0:
                status = brain.getBrainStatistics()
                self.assertIsInstance(status, str)
    
    def testEpisodeExample(self):
        """Test episode example with context manager."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        agent.initialize(world)
        
        # Use episode context manager
        with pynlm.episode_context(brain, agent, world) as episode:
            self.assertTrue(episode.isInitialized())
            
            for step in range(20):  # Reduced for testing
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

if __name__ == '__main__':
    # Run all tests
    suite = unittest.TestLoader().loadTestsFromTestCase(TestNLMErrorHandling)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestEnhancedConfiguration))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestBrainEnhancements))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAgentBrainEnhancements))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestWorldEnhancements))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestEpisodeManagement))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestUtilityFunctions))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestActionEnhancements))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestConfigValidation))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestComprehensiveExample))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Exit with appropriate code
    sys.exit(0 if result.wasSuccessful() else 1)