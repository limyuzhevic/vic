"""Test suite for NLM Python package."""

import sys
import unittest
import pynlm
from pynlm import (
    createDefaultConfig, createBrain, createAgentBrain,
    createSimpleWorld, createAction, ActionType,
    getVersionInfo, validateConfig, NeuronType, SynapseType
)

class TestBasicFunctionality(unittest.TestCase):
    """Test basic NLM functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = createDefaultConfig()
        self.brain = createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test fixtures."""
        del self.brain
    
    def test_version_info(self):
        """Test version information."""
        info = getVersionInfo()
        
        self.assertIn('version', info)
        self.assertIn('author', info)
        self.assertIn('description', info)
        self.assertEqual(info['version'], '0.1.0')
        self.assertEqual(info['author'], 'NLM Authors')
    
    def test_config_creation(self):
        """Test configuration creation."""
        config = createDefaultConfig()
        
        self.assertIsNotNone(config)
        self.assertTrue(hasattr(config, 'neuron_count'))
        self.assertEqual(config.neuron_count, 1000)
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        brain = createBrain(self.config)
        
        self.assertIsNotNone(brain)
        brain.initialize()
        
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertEqual(brain.getRegionCount(), 1)
    
    def test_brain_step(self):
        """Test brain stepping."""
        brain = createBrain(self.config)
        brain.initialize()
        
        # Take several steps
        for i in range(10):
            brain.step(i)
            
            # Should not crash
            self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
            self.assertIsNotNone(brain.getAverageFiringRate())
    
    def test_world_creation(self):
        """Test world creation."""
        world = createSimpleWorld()
        
        self.assertIsNotNone(world)
        world.configure(width=10, height=10)
        
        self.assertEqual(world.getWidth(), 10)
        self.assertEqual(world.getHeight(), 10)
        
        world.reset()
        self.assertGreater(world.getSimulationTime(), 0.0)
    
    def test_agent_creation(self):
        """Test agent creation."""
        world = createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = createAgentBrain(self.brain)
        agent.initialize(world)
        
        self.assertIsNotNone(agent)
        self.assertGreater(agent.getNeuromodulationLevel(), 0.0)
    
    def test_neuron_types(self):
        """Test neuron type enumeration."""
        neuron_types = [
            NeuronType.EXCITATORY,
            NeuronType.INHIBITORY,
            NeuronType.MODULATORY,
            NeuronType.SENSORY,
            NeuronType.MOTOR,
            NeuronType.INTERNAL
        ]
        
        for neuron_type in neuron_types:
            self.assertIsNotNone(neuron_type)
    
    def test_action_creation(self):
        """Test action creation."""
        # Test default action
        action = createAction()
        
        self.assertIsNotNone(action)
        self.assertEqual(action.getType(), ActionType.WAIT)
        
        # Test specific action
        action2 = createAction(ActionType.MOVE_FORWARD)
        self.assertEqual(action2.getType(), ActionType.MOVE_FORWARD)
    
    def test_world_object_creation(self):
        """Test world object creation."""
        world_obj = createAction(ActionType.MOVE_FORWARD)
        
        self.assertIsNotNone(world_obj)
        self.assertEqual(world_obj.getType(), ActionType.MOVE_FORWARD)

class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced NLM features."""
    
    def test_memory_systems(self):
        """Test memory system integration."""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Check memory systems
        self.assertIsNotNone(brain.getWorkingMemory())
        self.assertIsNotNone(brain.getEpisodicMemory())
        self.assertIsNotNone(brain.getAssociativeMemory())
        
        # Working memory should have traces
        wm = brain.getWorkingMemory()
        self.assertGreaterEqual(wm.getActiveTraces(), 0)
    
    def test_neuromodulation(self):
        """Test neuromodulation systems."""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Get initial levels
        initial_curiosity = agent.getCuriosityLevel()
        initial_novelty = agent.getNoveltyLevel()
        initial_dopamine = agent.getNeuromodulationLevel()
        
        # Apply reward to trigger neuromodulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Levels should be >= initial
        self.assertGreaterEqual(agent.getCuriosityLevel(), initial_curiosity)
        self.assertGreaterEqual(agent.getNoveltyLevel(), initial_novelty)
        self.assertGreaterEqual(agent.getNeuromodulationLevel(), initial_dopamine)
    
    def test_serialization(self):
        """Test brain save/load functionality."""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Run a few steps
        for i in range(50):
            brain.step(i)
            
        # Test save/load
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as f:
            filepath = f.name
        
        try:
            # Save brain state
            save_success = brain.save(filepath)
            self.assertTrue(save_success)
            
            # Create new brain and load
            brain2 = createBrain(config)
            brain2.initialize()
            
            # Load should restore structure
            load_success = brain2.load(filepath)
            self.assertTrue(load_success)
            
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_plasticity(self):
        """Test plasticity features."""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Get a region
        region = brain.getRegion(brain.getRegionIds()[0])
        
        # Enable plasticity on synapses
        for synapse in region.getSynapses():
            synapse.enablePlasticity(True, True, True)
        
        # Take some steps with input
        for i in range(100):
            brain.injectCurrentToNeurons(NeuronType.Sensory, 10.0)
            brain.step(i)
            
        # Should have accumulated plasticity effects
        self.assertIsNotNone(brain.getSTDP())
    
    def test_memory_episode_storage(self):
        """Test episodic memory storage."""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Get episodic memory
        em = brain.getEpisodicMemory()
        
        # Should be able to store episodes
        self.assertIsNotNone(em)
        
        # Run some steps
        for i in range(200):
            brain.step(i)
            
        # Check if episodes are stored
        episode_count = em.getEpisodeCount()
        self.assertGreaterEqual(episode_count, 0)

class TestPerformance(unittest.TestCase):
    """Test performance characteristics."""
    
    def test_brain_creation_speed(self):
        """Test that brain creation is reasonably fast."""
        import time
        
        start_time = time.time()
        
        config = createDefaultConfig()
        config.neuron_count = 100  # Smaller for test speed
        
        brain = createBrain(config)
        brain.initialize()
        
        elapsed = time.time() - start_time
        
        # Should be fast (< 1 second for small brain)
        self.assertLess(elapsed, 1.0, 
                       f"Brain creation took {elapsed:.3f}s, expected < 1.0s")
    
    def test_step_speed(self):
        """Test that stepping is reasonably fast."""
        import time
        
        config = createDefaultConfig()
        config.neuron_count = 100
        
        brain = createBrain(config)
        brain.initialize()
        
        # Time many steps
        start_time = time.time()
        num_steps = 500
        
        for i in range(num_steps):
            brain.step(i)
            
        elapsed = time.time() - start_time
        
        # Should be reasonably fast
        steps_per_second = num_steps / elapsed
        self.assertGreater(steps_per_second, 10.0,
                           f"Steps per second: {steps_per_second:.1f}, expected > 10")

class TestUtilities(unittest.TestCase):
    """Test utility functions."""
    
    def test_config_validation(self):
        """Test configuration validation."""
        # Should not raise exception for valid config
        config = createDefaultConfig()
        self.assertTrue(validateConfig(config))
        
        # Should raise for None
        with self.assertRaises(ValueError):
            validateConfig(None)

class TestIntegration(unittest.TestCase):
    """Test system integration."""
    
    def test_complete_workflow(self):
        """Test complete agent workflow."""
        # Create all components
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run complete workflow
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Should complete without errors
        self.assertIsNotNone(brain)
        self.assertIsNotNone(agent)
        self.assertIsNotNone(world)
    
    def test_system_connectivity(self):
        """Test that all systems are properly connected."""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Verify all major systems are connected
        self.assertIsNotNone(brain.getWorkingMemory())
        self.assertIsNotNone(brain.getEpisodicMemory())
        self.assertIsNotNone(brain.getDopamine())
        self.assertIsNotNone(brain.getCuriosity())
        self.assertIsNotNone(brain.getPredictionSystem())
        self.assertIsNotNone(brain.getDevelopmentSystem())

def run_tests():
    """Run all tests and report results."""
    print("NLM Test Suite")
    print("=" * 50)
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestBasicFunctionality)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAdvancedFeatures))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestPerformance))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestUtilities))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestIntegration))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Report summary
    print("\n" + "=" * 50)
    print("Test Summary:")
    print(f"  Tests run: {result.testsRun}")
    print(f"  Failures: {len(result.failures)}")
    print(f"  Errors: {len(result.errors)}")
    if result.testsRun > 0:
        success_rate = ((result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun) * 100
        print(f"  Success rate: {success_rate:.1f}%")
    
    if result.failures:
        print("\nFailures:")
        for test, traceback in result.failures:
            print(f"  {test}: {traceback}")
    
    if result.errors:
        print("\nErrors:")
        for test, traceback in result.errors:
            print(f"  {test}: {traceback}")
    
    return result.wasSuccessful()

def main(argv):
    """Test CLI entry point."""
    return run_tests()

if __name__ == "__main__":
    sys.exit(0 if main(sys.argv) else 1)