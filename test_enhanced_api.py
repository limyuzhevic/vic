#!/usr/bin/env python3
"""
Test suite for enhanced NLM Python bindings
Tests the improved error handling, type hints, and enhanced API functionality
"""

import sys
import unittest
import json
import tempfile
import os

# Import both basic and enhanced NLM bindings
try:
    import pynlm
    from pynlm import (
        # Basic API
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        # Enhanced API
        EnhancedConfig, EnhancedBrain, EnhancedAgentBrain, EnhancedSimpleWorld
    )
    ENHANCED_API_AVAILABLE = True
except ImportError as e:
    ENHANCED_API_AVAILABLE = False
    print(f"Enhanced API not available: {e}")

class TestEnhancedPythonAPI(unittest.TestCase):
    """Test suite for enhanced Python API functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not ENHANCED_API_AVAILABLE:
            self.skipTest("Enhanced API not available")
        
        # Create basic objects
        self.config = createDefaultConfig()
        self.config.set("brain.neuron_count", 1000)
        self.config.set("brain.synapse_density", 0.1f)
        
        self.brain = createBrain(self.config)
        self.brain.initialize()
        
        self.world = createSimpleWorld()
        self.world.configure(20, 20, 8, 8)
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        self.agent = createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up after tests"""
        pass
    
    def test_enhanced_config_basic_operations(self):
        """Test enhanced config basic operations"""
        enhanced_config = EnhancedConfig()
        
        # Test setting values
        enhanced_config.set("test.int", 42)
        enhanced_config.set("test.float", 3.14)
        enhanced_config.set("test.string", "hello")
        enhanced_config.set("test.bool", True)
        
        # Test getting values
        self.assertEqual(enhanced_config.getInt("test.int"), 42)
        self.assertAlmostEqual(enhanced_config.getFloat("test.float"), 3.14)
        self.assertEqual(enhanced_config.getString("test.string"), "hello")
        self.assertTrue(enhanced_config.getBool("test.bool"))
        
        # Test has method
        self.assertTrue(enhanced_config.has("test.int"))
        self.assertFalse(enhanced_config.has("nonexistent.key"))
        
        # Test get keys
        keys = enhanced_config.getKeys()
        self.assertIn("test.int", keys)
        self.assertIn("test.float", keys)
    
    def test_enhanced_config_configure_methods(self):
        """Test enhanced config configuration helper methods"""
        enhanced_config = EnhancedConfig()
        
        # Test brain configuration
        enhanced_config.configureBrain(2000, 0.2f)
        self.assertEqual(enhanced_config.getInt("brain.neuron_count"), 2000)
        self.assertAlmostEqual(enhanced_config.getFloat("brain.synapse_density"), 0.2f)
        
        # Test memory configuration
        enhanced_config.configureMemory(50000)
        self.assertEqual(enhanced_config.getInt("memory.working_memory.capacity"), 50000)
        
        # Test neuromodulation configuration
        enhanced_config.configureNeuromodulation(2.0f, False)
        self.assertAlmostEqual(enhanced_config.getFloat("neuromod.dopamine.scale"), 2.0f)
        self.assertFalse(enhanced_config.getBool("neuromod.curiosity.enable"))
    
    def test_enhanced_config_file_operations(self):
        """Test enhanced config file I/O operations"""
        enhanced_config = EnhancedConfig()
        enhanced_config.set("test.value", 123)
        
        # Save to file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            # Enhanced config save (uses underlying pynlm config)
            success = self.config.saveToFile(config_file)
            self.assertTrue(success)
            
            # Verify file was created and contains data
            self.assertTrue(os.path.exists(config_file))
            
            # Test load
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                f.write(json.dumps({"test": "loaded value"}))
                load_file = f.name
            
            # Basic config load
            success = self.config.loadFromFile(load_file)
            self.assertTrue(success)
            
            os.unlink(load_file)
        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def test_enhanced_brain_basic_operations(self):
        """Test enhanced brain basic operations"""
        enhanced_brain = EnhancedBrain(self.config)
        
        # Test initialization
        self.assertTrue(enhanced_brain.initialize())
        
        # Test step
        enhanced_brain.step(10)
        enhanced_brain.step(20, 2.0)
        
        # Test statistics
        stats = enhanced_brain.getStatistics()
        self.assertIn("total_neurons", stats)
        self.assertIn("total_synapses", stats)
        self.assertIn("firing_neurons", stats)
        self.assertIn("average_firing_rate", stats)
        self.assertEqual(stats["total_neurons"], 1000)
        
        # Test region info
        region_info = enhanced_brain.getRegionInfo(0)
        self.assertIn("id", region_info)
        self.assertIn("name", region_info)
        self.assertIn("neuron_count", region_info)
        self.assertIn("synapse_count", region_info)
        
        # Test all regions
        regions = enhanced_brain.getAllRegions()
        self.assertIsInstance(regions, list)
        self.assertGreater(len(regions), 0)
    
    def test_enhanced_brain_simulation_with_callback(self):
        """Test enhanced brain simulation with progress callback"""
        enhanced_brain = EnhancedBrain(self.config)
        enhanced_brain.initialize()
        
        # Track steps with callback
        step_count = 0
        callback_called = []
        
        def progress_callback(step, stats):
            nonlocal step_count, callback_called
            step_count = step
            callback_called.append(stats)
        
        # Run simulation with callback
        enhanced_brain.runSimulation(100, progress_callback)
        
        # Verify callback was called
        self.assertEqual(step_count, 99)  # 0-indexed
        self.assertEqual(len(callback_called), 100)
        
        # Verify callback received statistics
        for stats in callback_called:
            self.assertIn("total_neurons", stats)
            self.assertIn("firing_neurons", stats)
    
    def test_enhanced_brain_state_management(self):
        """Test enhanced brain state management"""
        enhanced_brain = EnhancedBrain(self.config)
        enhanced_brain.initialize()
        
        # Collect initial state
        initial_stats = enhanced_brain.getStatistics()
        initial_spikes = initial_stats["total_spikes"]
        
        # Run some steps
        for i in range(50):
            enhanced_brain.step(i)
        
        # Verify state changed
        final_stats = enhanced_brain.getStatistics()
        self.assertGreaterEqual(final_stats["total_spikes"], initial_spikes)
        
        # Test reset
        enhanced_brain.reset()
        reset_stats = enhanced_brain.getStatistics()
        # Reset should clear accumulated spikes
        self.assertLess(reset_stats["total_spikes"], final_stats["total_spikes"])
    
    def test_enhanced_agent_basic_operations(self):
        """Test enhanced agent basic operations"""
        enhanced_agent = EnhancedAgentBrain(self.brain)
        
        # Test initialization
        enhanced_agent.initialize(self.world)
        
        # Test agent state
        state = enhanced_agent.getAgentState()
        self.assertIn("curiosity_level", state)
        self.assertIn("novelty_level", state)
        self.assertIn("neuromodulation_level", state)
        self.assertIn("prediction_error", state)
        self.assertIn("developmental_stage", state)
        
        # Test statistics
        stats = enhanced_agent.getStatistics()
        self.assertIn("curiosity_level", stats)
        self.assertIn("novelty_level", stats)
        
        # Test subsystem controls
        enhanced_agent.enableRewardModulation(True)
        enhanced_agent.enableCuriosity(True)
        enhanced_agent.enableDevelopment(True)
        enhanced_agent.enableStructuralPlasticity(True)
        
        # Verify status
        self.assertTrue(enhanced_agent.isRewardModulationEnabled())
        self.assertTrue(enhanced_agent.isCuriosityEnabled())
        self.assertTrue(enhanced_agent.isDevelopmentEnabled())
        self.assertTrue(enhanced_agent.isStructuralPlasticityEnabled())
    
    def test_enhanced_agent_neuromodulation(self):
        """Test enhanced agent neuromodulation"""
        enhanced_agent = EnhancedAgentBrain(self.brain)
        enhanced_agent.initialize(self.world)
        
        # Test reward modulation
        enhanced_agent.applyRewardModulation(1.0, 0.5)
        
        # Test development update
        enhanced_agent.updateDevelopment(0.1)
        
        # Test state changes
        state = enhanced_agent.getAgentState()
        self.assertGreaterEqual(state["curiosity_level"], 0.0)
        self.assertGreaterEqual(state["novelty_level"], 0.0)
    
    def test_enhanced_world_basic_operations(self):
        """Test enhanced world basic operations"""
        enhanced_world = EnhancedSimpleWorld()
        
        # Test configuration
        enhanced_world.configure(30, 30, 16, 16)
        
        # Test agent start
        enhanced_world.setAgentStart(15.0, 15.0)
        
        # Test update
        enhanced_world.update(0.1)
        
        # Test agent body
        body = enhanced_world.getAgentBody()
        self.assertIn("x", body)
        self.assertIn("y", body)
        self.assertIn("orientation", body)
        self.assertIn("velocity_x", body)
        self.assertIn("velocity_y", body)
        
        # Test dimensions
        dimensions = enhanced_world.getDimensions()
        self.assertEqual(dimensions["width"], 30)
        self.assertEqual(dimensions["height"], 30)
        self.assertEqual(dimensions["vision_width"], 16)
        self.assertEqual(dimensions["vision_height"], 16)
        
        # Test simulation time
        self.assertGreaterEqual(enhanced_world.getSimulationTime(), 0.0)
        
        # Test random seed
        enhanced_world.setRandomSeed(12345)
        seed = enhanced_world.getRandomSeed()
        self.assertEqual(seed, 12345)
    
    def test_enhanced_world_object_management(self):
        """Test enhanced world object management"""
        enhanced_world = EnhancedSimpleWorld()
        enhanced_world.configure(20, 20, 8, 8)
        
        # Add world object
        enhanced_world.addObject(5.0, 5.0, 1, 10.0, 1.0)  # Type: Resource
        
        # Verify object added (test position validity)
        self.assertTrue(enhanced_world.isValidPosition(5.0, 5.0))
        
        # Remove object
        enhanced_world.removeObject(5.0, 5.0)
        
        # Position might not be valid after removal (depends on implementation)
        # We just test that the method doesn't crash
    
    def test_factory_functions(self):
        """Test factory functions for enhanced objects"""
        # Test enhanced config factory
        enhanced_config = EnhancedConfig.createEnhancedConfig()
        self.assertIsNotNone(enhanced_config)
        
        # Test enhanced brain factory
        enhanced_brain = EnhancedBrain.createEnhancedBrain(self.config)
        self.assertIsNotNone(enhanced_brain)
        
        # Test enhanced agent factory
        enhanced_agent = EnhancedAgentBrain.createEnhancedAgentBrain(self.brain)
        self.assertIsNotNone(enhanced_agent)
        
        # Test enhanced world factory
        enhanced_world = EnhancedSimpleWorld.createEnhancedSimpleWorld()
        self.assertIsNotNone(enhanced_world)
    
    def test_complete_agent_simulation(self):
        """Test complete agent simulation with enhanced API"""
        # This test uses the enhanced convenience function
        from pynlm import runCompleteAgentSimulation
        
        results = runCompleteAgentSimulation(100)
        
        # Verify results structure
        self.assertIn("final_stats", results)
        self.assertIn("success", results)
        self.assertTrue(results["success"])
        
        final_stats = results["final_stats"]
        self.assertIn("total_neurons", final_stats)
        self.assertIn("total_synapses", final_stats)
        self.assertIn("firing_neurons", final_stats)
        self.assertIn("curiosity_level", final_stats)
        
        # Verify some expected values
        self.assertEqual(final_stats["total_neurons"], 1000)
    
    def test_configuration_helpers(self):
        """Test configuration helper functions"""
        from pynlm import getDefaultConfiguration, generateConfigurationCode
        
        # Test default configuration
        config = getDefaultConfiguration()
        self.assertIn("brain.neuron_count", config)
        self.assertIn("brain.synapse_density", config)
        self.assertIn("brain.v_thresh", config)
        
        # Test configuration code generation
        code = generateConfigurationCode(config)
        self.assertIn("import pynlm", code)
        self.assertIn("config = pynlm.createDefaultConfig()", code)
        self.assertIn("config.set", code)
        self.assertIn("brain.neuron_count", code)

class TestErrorHandling(unittest.TestCase):
    """Test error handling in enhanced API"""
    
    def test_enhanced_config_error_handling(self):
        """Test error handling in enhanced config"""
        enhanced_config = EnhancedConfig()
        
        # Test accessing non-existent key
        with self.assertRaises(Exception) as cm:
            enhanced_config.getInt("nonexistent.key")
        
        # Should be a ConfigurationError or similar
        self.assertIn("error", str(cm.exception).lower())
    
    def test_enhanced_brain_error_handling(self):
        """Test error handling in enhanced brain"""
        enhanced_brain = EnhancedBrain(self.config)
        
        # Brain should be uninitialized initially
        # Attempting to step should raise an error
        with self.assertRaises(Exception) as cm:
            enhanced_brain.step(1)
        
        # Initialize first
        enhanced_brain.initialize()
        
        # Then step should work
        enhanced_brain.step(1)
        
        # Test invalid region access
        with self.assertRaises(Exception) as cm:
            enhanced_brain.getRegionInfo(999)  # Invalid region index
        
        self.assertIn("error", str(cm.exception).lower())

class TestBackwardCompatibility(unittest.TestCase):
    """Test backward compatibility with basic API"""
    
    def test_basic_api_still_works(self):
        """Test that basic API still works as expected"""
        config = createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Test basic operations
        brain.step(10)
        
        stats = {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
        }
        
        self.assertEqual(stats["total_neurons"], 500)
        self.assertGreater(stats["total_synapses"], 0)
        self.assertIsInstance(stats["average_firing_rate"], (int, float))
    
    def test_enhanced_api_extends_basic(self):
        """Test that enhanced API extends basic API"""
        if not ENHANCED_API_AVAILABLE:
            self.skipTest("Enhanced API not available")
        
        # Basic objects
        config = createDefaultConfig()
        config.set("brain.neuron_count", 750)
        
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Verify we can access enhanced features
        enhanced_agent = EnhancedAgentBrain(brain)
        self.assertIsNotNone(enhanced_agent)
        
        # Enhanced brain should have additional functionality
        enhanced_brain = EnhancedBrain(config)
        enhanced_brain.initialize()
        stats = enhanced_brain.getStatistics()
        self.assertIn("pending_spike_events", stats)

if __name__ == "__main__":
    print("Running enhanced NLM Python bindings test suite...")
    print(f"Enhanced API available: {ENHANCED_API_AVAILABLE}")
    
    # Run tests
    unittest.main(verbosity=2)
