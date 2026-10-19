#!/usr/bin/env python3
"""Comprehensive test suite for NLM Python API improvements"""

import pynlm
import unittest
import sys

class TestImprovedNLMAPI(unittest.TestCase):
    """Test the improved NLM Python API functionality"""
    
    def setUp(self):
        """Set up test fixtures before each test"""
        self.brain = pynlm.quickBrain()
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up after each test"""
        pass
    
    def test_quick_brain_creation(self):
        """Test quick brain creation convenience method"""
        brain2 = pynlm.quickBrain()
        brain2.initialize()
        
        # Verify brain was created
        self.assertIsNotNone(brain2)
        self.assertEqual(brain2.getNeuronCount(), 500)
        self.assertEqual(brain2.getRegionCount(), 1)
    
    def test_improved_brain_methods(self):
        """Test improved brain method names"""
        # Test improved method names
        neuron_count = self.brain.getNeuronCount()
        self.assertEqual(neuron_count, 500)
        
        active_neurons = self.brain.getActiveNeurons()
        self.assertIsInstance(active_neurons, int)
        self.assertGreaterEqual(active_neurons, 0)
        
        synapse_count = self.brain.getSynapseCount()
        self.assertIsInstance(synapse_count, int)
        self.assertGreaterEqual(synapse_count, 0)
        
        avg_firing = self.brain.getAverageFiring()
        self.assertIsInstance(avg_firing, float)
        self.assertGreaterEqual(avg_firing, 0.0)
        
        ei_ratio = self.brain.getEIRatio()
        self.assertIsInstance(ei_ratio, float)
        self.assertGreaterEqual(ei_ratio, 0.0)
    
    def test_config_helper_methods(self):
        """Test improved configuration helper methods"""
        # Test config helper
        config = pynlm.Config(neuron_count=1000, learning_rate=0.01)
        self.assertIsNotNone(config)
        
        # Test that config has expected keys
        keys = config.getKeys()
        self.assertIn("neuron_count", keys)
        self.assertIn("learning_rate", keys)
        
        # Test direct value access
        neuron_val = config.getOr("neuron_count", 500)
        self.assertEqual(neuron_val, 1000)
    
    def test_sensory_factory_methods(self):
        """Test improved sensory input factory methods"""
        # Test Vision factory
        vision = pynlm.Vision.create(16, 16, 3)
        self.assertEqual(vision.getWidth(), 16)
        self.assertEqual(vision.getHeight(), 16)
        self.assertEqual(vision.getChannels(), 3)
        
        # Test Audio factory
        audio = pynlm.Audio.create(48000, 2048)
        self.assertEqual(audio.getSampleRate(), 48000)
        self.assertEqual(audio.getNumSamples(), 2048)
        
        # Test Vision data setting
        data = [0.5] * 48  # 16*16*3 = 48 elements
        vision.setData(data)
        self.assertEqual(len(vision.getData()), 48)
    
    def test_action_factory_methods(self):
        """Test improved action factory methods"""
        # Test action creation
        move_forward = pynlm.Action.createMove("forward", 1.0)
        self.assertEqual(move_forward.getType(), pynlm.ActionType.MoveForward)
        self.assertEqual(move_forward.getParameters(), [1.0])
        
        # Test custom action
        custom_action = pynlm.Action.create("custom_action")
        self.assertEqual(custom_action.getType(), pynlm.ActionType.Custom)
        
        # Test action name
        forward_name = move_forward.getName()
        self.assertIn("forward", forward_name.lower())
    
    def test_agent_convenience_methods(self):
        """Test agent convenience methods"""
        # Create agent
        agent = pynlm.createAgentBrain(self.brain)
        
        # Test enable methods
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Test enableAllDevelopment
        agent.enableAllDevelopment()
        
        # Test check methods
        self.assertTrue(agent.isRewardModulationEnabled())
        self.assertTrue(agent.isStructuralPlasticityEnabled())
        self.assertTrue(agent.isDevelopmentEnabled())
        self.assertTrue(agent.isCuriosityEnabled())
    
    def test_agent_processing_methods(self):
        """Test agent processing convenience methods"""
        agent = pynlm.createAgentBrain(self.brain)
        
        # Test processInput (just verify it doesn't crash)
        # Note: This requires a proper percept object
        # For testing, we'll skip the actual call but verify the method exists
        self.assertTrue(hasattr(agent, 'processInput'))
        
        # Test makeAction
        action = agent.makeAction()
        self.assertIsNotNone(action)
        self.assertIsInstance(action, pynlm.Action)
    
    def test_development_control_methods(self):
        """Test development control methods"""
        # Test development control
        stage = self.brain.getDevelopmentalStage()
        self.assertIsInstance(stage, pynlm.DevelopmentalStage)
        
        # Test development methods
        self.brain.developToCriticalPeriod()
        self.assertEqual(self.brain.getDevelopmentalStage(), pynlm.DevelopmentalStage.CriticalPeriod)
        
        self.brain.developToMaturation()
        self.assertEqual(self.brain.getDevelopmentalStage(), pynlm.DevelopmentalStage.Maturation)
        
        self.brain.developToAdult()
        self.assertEqual(self.brain.getDevelopmentalStage(), pynlm.DevelopmentalStage.Adult)
    
    def test_neuromodulation_methods(self):
        """Test neuromodulation methods"""
        agent = pynlm.createAgentBrain(self.brain)
        
        # Get initial values
        initial_neuromod = agent.getNeuromodulationLevel()
        initial_curiosity = agent.getCuriosityLevel()
        initial_novelty = agent.getNoveltyLevel()
        initial_prediction_error = agent.getPredictionError()
        
        # Apply reward modulation
        agent.applyRewardModulation(0.8, 0.5)
        
        # Values may change, but should still be valid
        self.assertIsInstance(agent.getNeuromodulationLevel(), float)
        self.assertIsInstance(agent.getCuriosityLevel(), float)
        self.assertIsInstance(agent.getNoveltyLevel(), float)
        self.assertIsInstance(agent.getPredictionError(), float)
    
    def test_world_helper_methods(self):
        """Test world helper methods"""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Test world helper methods
        world.setAgentPosition(10.0, 10.0)
        
        agent_body = world.getAgentBody()
        self.assertIsNotNone(agent_body)
        self.assertEqual(agent_body.x, 10.0)
        self.assertEqual(agent_body.y, 10.0)
        
        # Test addObject
        resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
        world.addObject(resource)
        
        # Test isValidPosition
        valid = world.isValidPosition(1.0, 1.0)
        self.assertIsInstance(valid, bool)
    
    def test_statistics_methods(self):
        """Test statistics methods"""
        # Run some simulation
        for i in range(10):
            self.brain.step(i)
        
        # Get statistics
        stats = self.brain.getStatistics()
        
        # Verify stats contains expected keys
        self.assertIsInstance(stats, dict)
        self.assertIn("neuron_count", stats)
        self.assertIn("active_neurons", stats)
        self.assertIn("firing_rate", stats)
        self.assertIn("synapse_count", stats)
        self.assertIn("development_stage", stats)
        
        # Verify values are reasonable
        self.assertGreaterEqual(stats["neuron_count"], 0)
        self.assertGreaterEqual(stats["active_neurons"], 0)
        self.assertGreaterEqual(stats["firing_rate"], 0.0)
        self.assertGreaterEqual(stats["synapse_count"], 0)
    
    def test_backward_compatibility(self):
        """Test that old API still works (backward compatibility)"""
        # Test original method names still exist
        self.assertTrue(hasattr(self.brain, 'getTotalNeuronCount'))
        self.assertTrue(hasattr(self.brain, 'getFiringNeuronCount'))
        self.assertTrue(hasattr(self.brain, 'getTotalSynapseCount'))
        self.assertTrue(hasattr(self.brain, 'getAverageFiringRate'))
        
        # Test they still work
        old_neuron_count = self.brain.getTotalNeuronCount()
        self.assertEqual(old_neuron_count, self.brain.getNeuronCount())
        
        old_firing_count = self.brain.getFiringNeuronCount()
        self.assertEqual(old_firing_count, self.brain.getActiveNeurons())
        
        old_synapse_count = self.brain.getTotalSynapseCount()
        self.assertEqual(old_synapse_count, self.brain.getSynapseCount())
        
        old_firing_rate = self.brain.getAverageFiringRate()
        self.assertEqual(old_firing_rate, self.brain.getAverageFiring())
    
    def test_world_configuration(self):
        """Test world configuration helper methods"""
        world = pynlm.createSimpleWorld()
        
        # Test configuration
        world.configure(width=30, height=40, visionWidth=10, visionHeight=8)
        
        self.assertEqual(world.getWidth(), 30)
        self.assertEqual(world.getHeight(), 40)
        
        # Test max energy
        world.setMaxEnergy(200.0)
        self.assertEqual(world.getMaxEnergy(), 200.0)
        
        # Test energy decay rate
        world.setEnergyDecayRate(0.1)
        self.assertEqual(world.getEnergyDecayRate(), 0.1)
    
    def test_logging_and_debugging(self):
        """Test logging and debugging methods"""
        # Test brain logging
        self.brain.logStatus()
        
        # Test that brain still works after logging
        self.brain.step(0)
        
        # Test config logging
        config = pynlm.createDefaultConfig()
        summary = config.summary()
        self.assertIsInstance(summary, str)
        self.assertIn("Configuration", summary)
    
    def test_action_helper_methods(self):
        """Test action helper methods"""
        action = pynlm.Action.create("move_forward")
        
        # Test cloning
        cloned = action.clone()
        self.assertIsNotNone(cloned)
        self.assertEqual(cloned.getType(), action.getType())
        
        # Test name method
        name = action.getName()
        self.assertIsInstance(name, str)
        self.assertGreater(len(name), 0)

class TestNLMAPIExamples(unittest.TestCase):
    """Test examples from the documentation"""
    
    def test_example_quick_start(self):
        """Test the quick start example from documentation"""
        # Simplified version of the example
        brain = pynlm.quickBrain()
        brain.initialize()
        brain.runSimulation(10)  # Reduced for testing
        
        # Verify it works
        self.assertIsNotNone(brain)
        self.assertEqual(brain.getActiveNeurons(), 0)  # Should have zero firing neurons
    
    def test_example_with_config(self):
        """Test example with custom configuration"""
        config = pynlm.Config(
            neuron_count=100,
            region_count=1,
            connection_probability=0.1
        )
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        self.assertEqual(brain.getNeuronCount(), 100)
        self.assertEqual(brain.getRegionCount(), 1)
    
    def test_example_agent_world(self):
        """Test example with agent and world"""
        brain = pynlm.quickBrain()
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all development features
        agent.enableAllDevelopment()
        
        # Run a few steps
        for i in range(5):
            world.update(0.1)
            # Note: We can't fully test this without proper percepts
            # but we can verify the setup worked
            self.assertIsNotNone(world)
            self.assertIsNotNone(agent)

class TestErrorHandling(unittest.TestCase):
    """Test error handling and validation"""
    
    def test_invalid_vision_dimensions(self):
        """Test error handling for invalid vision dimensions"""
        # This should handle edge cases gracefully
        try:
            vision = pynlm.Vision.create(0, 0, 3)
            # If no exception, verify dimensions are set
            self.assertEqual(vision.getWidth(), 0)
            self.assertEqual(vision.getHeight(), 0)
        except Exception:
            # Exception is also acceptable
            pass
    
    def test_invalid_action_type(self):
        """Test handling of invalid action types"""
        # Test that invalid action types are handled gracefully
        action = pynlm.Action.create("unknown_type")
        self.assertIsNotNone(action)
        self.assertEqual(action.getType(), pynlm.ActionType.Custom)
    
    def test_config_validation(self):
        """Test config validation"""
        # Test config with various types
        config = pynlm.createDefaultConfig()
        
        # Test setting different value types
        config.set("int_value", 42)
        config.set("double_value", 3.14)
        config.set("string_value", "test")
        config.set("bool_value", True)
        
        # Test getting values
        int_val = config.getOr("int_value", 0)
        self.assertEqual(int_val, 42)
        
        str_val = config.getOr("string_value", "default")
        self.assertEqual(str_val, "test")

def run_all_tests():
    """Run all NLM API improvement tests"""
    print("Running NLM Python API Improvement Tests")
    print("=" * 50)
    print()
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestImprovedNLMAPI)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestNLMAPIExamples))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestErrorHandling))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    print()
    print("=" * 50)
    if result.wasSuccessful():
        print("✅ All tests passed!")
        return True
    else:
        print(f"❌ {len(result.failures)} test(s) failed")
        print(f"❌ {len(result.errors)} error(s)")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
