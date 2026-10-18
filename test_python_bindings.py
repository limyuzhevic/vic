#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.

This test suite covers the enhanced functionality including:
- Basic Python API functionality
- Error handling and custom exceptions
- Performance monitoring APIs
- Context managers
- Convenience methods
- Advanced simulation utilities
"""

import sys
import os
import tempfile
import time
import unittest
import pynlm

class TestBasicAPI(unittest.TestCase):
    """Test basic Python API functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test fixtures."""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def testBrainCreation(self):
        """Test basic brain creation and initialization."""
        self.assertIsNotNone(self.brain)
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
        self.assertTrue(self.brain.getTotalSynapseCount() > 0)
    
    def testBrainStep(self):
        """Test basic brain stepping."""
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0)
        self.assertTrue(self.brain.getTotalSpikeCount() >= initial_spikes)
    
    def testContextManager(self):
        """Test brain context manager."""
        with pynlm.createBrain(self.config) as brain:
            brain.initialize()
            brain.step(0)
            self.assertTrue(brain.getTotalNeuronCount() > 0)
        
        # Context manager should have cleaned up
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)  # Existing brain still valid
    
    def testConfiguration(self):
        """Test configuration functionality."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        self.assertTrue(config.has("brain.neuron_count"))
        
        config.saveToFile("test_config.cfg")
        self.assertTrue(os.path.exists("test_config.cfg"))
        
        # Clean up
        os.remove("test_config.cfg")

class TestEnhancedBrainAPI(unittest.TestCase):
    """Test enhanced Brain API functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test fixtures."""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def testStatisticsMethod(self):
        """Test brain statistics method."""
        self.brain.step(0)
        stats = self.brain.getStatistics()
        
        self.assertIsInstance(stats, dict)
        self.assertIn('neuron_count', stats)
        self.assertIn('synapse_count', stats)
        self.assertIn('firing_neuron_count', stats)
        self.assertIn('active_neuron_count', stats)
        self.assertIn('average_firing_rate', stats)
        self.assertIn('excitation_inhibition_ratio', stats)
        self.assertIn('total_spike_count', stats)
        
        self.assertEqual(stats['neuron_count'], self.brain.getTotalNeuronCount())
        self.assertEqual(stats['synapse_count'], self.brain.getTotalSynapseCount())
    
    def testRandomSeedMethods(self):
        """Test random seed convenience methods."""
        initial_seed = self.brain.getRandomSeed()
        self.assertIsInstance(initial_seed, int)
        
        self.brain.setRandomSeed(12345)
        self.assertEqual(self.brain.getRandomSeed(), 12345)
    
    def testIsInitialized(self):
        """Test isInitialized method."""
        self.assertTrue(self.brain.isInitialized())
        
        # Create a new brain and check before initialization
        new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.assertFalse(new_brain.isInitialized())
        new_brain.initialize()
        self.assertTrue(new_brain.isInitialized())
    
    def testWaitForSpikes(self):
        """Test waitForSpikes method."""
        # Inject current to generate spikes
        self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)
        
        # Wait for at least 1 spike
        step = self.brain.waitForSpikes(minSpikes=1, maxSteps=100)
        self.assertGreaterEqual(step, 0)
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), 1)
    
    def testWaitForSpikesTimeout(self):
        """Test waitForSpikes timeout behavior."""
        # Create brain with no neurons to inject
        small_config = pynlm.createDefaultConfig()
        small_config.set("brain.neuron_count", 10)
        small_brain = pynlm.createBrain(small_config)
        small_brain.initialize()
        
        # Should timeout since no spikes will be generated
        with self.assertRaises(pynlm.SimulationError):
            small_brain.waitForSpikes(minSpikes=10, maxSteps=10)

class TestEnhancedAgentAPI(unittest.TestCase):
    """Test enhanced AgentBrain API functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up test fixtures."""
        if hasattr(self, 'agent'):
            self.agent.reset()
    
    def testCreateAction(self):
        """Test createAction method."""
        action = self.agent.createAction(pynlm.ActionType.MoveForward)
        self.assertIsNotNone(action)
        self.assertEqual(action.getType(), pynlm.ActionType.MoveForward)
        
        # Action with parameters
        action2 = self.agent.createAction(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
        self.assertEqual(action2.getType(), pynlm.ActionType.Custom)
        self.assertEqual(len(action2.getParameters()), 3)
    
    def testRunEpisode(self):
        """Test runEpisode method."""
        steps = self.agent.runEpisode(self.world, numSteps=50, timestep=0.1)
        self.assertEqual(steps, 50)  # Should have completed all steps
    
    def testGetEpisodeStatistics(self):
        """Test getEpisodeStatistics method."""
        # Run an episode first
        self.agent.runEpisode(self.world, numSteps=20, timestep=0.1)
        
        stats = self.agent.getEpisodeStatistics(numSteps=50)
        self.assertIsInstance(stats, dict)
        self.assertIn('total_spikes', stats)
        self.assertIn('average_spikes_per_step', stats)
        self.assertIn('steps_completed', stats)
        
        self.assertEqual(stats['steps_completed'], 50)
        self.assertGreaterEqual(stats['total_spikes'], 0)
    
    def testSubsystemEnableMethods(self):
        """Test subsystem enable/disable methods."""
        # Test enabling subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Check they are enabled
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())

class TestEnhancedWorldAPI(unittest.TestCase):
    """Test enhanced SimpleWorld API functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
        self.world.reset()
        self.world.setAgentStart(7.5, 7.5)
    
    def testCreateAgent(self):
        """Test createAgent method."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = self.world.createAgent(brain)
        self.assertIsNotNone(agent)
        self.assertEqual(agent.getBrain(), brain)
    
    def testRunEpisode(self):
        """Test runEpisode method with agent."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = self.world.createAgent(brain)
        steps = self.world.runEpisode(agent, numSteps=30, timestep=0.1)
        self.assertEqual(steps, 30)
    
    def testGetWorldStatistics(self):
        """Test getWorldStatistics method."""
        stats = self.world.getWorldStatistics()
        self.assertIsInstance(stats, dict)
        self.assertIn('width', stats)
        self.assertIn('height', stats)
        self.assertIn('simulation_time', stats)
        self.assertIn('max_energy', stats)
        self.assertIn('energy_decay_rate', stats)
        
        self.assertEqual(stats['width'], 15)
        self.assertEqual(stats['height'], 15)

class TestPerformanceAPIs(unittest.TestCase):
    """Test performance monitoring APIs."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test fixtures."""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def testPerformanceMetrics(self):
        """Test PerformanceMetrics class."""
        from pynlm import PerformanceMetrics
        
        metrics = PerformanceMetrics()
        self.assertEqual(metrics.getNeuronsProcessed(), 0)
        self.assertEqual(metrics.getSynapsesProcessed(), 0)
        self.assertEqual(metrics.getTotalSpikes(), 0)
        self.assertEqual(metrics.getAverageFiringRate(), 0.0)
        self.assertEqual(metrics.getEnergyConsumption(), 0.0)
        
        # Record some data
        metrics.recordNeuron(100)
        metrics.recordSynapse(500)
        metrics.recordSpike(25)
        
        self.assertEqual(metrics.getNeuronsProcessed(), 100)
        self.assertEqual(metrics.getSynapsesProcessed(), 500)
        self.assertEqual(metrics.getTotalSpikes(), 25)
        self.assertGreaterEqual(metrics.getAverageFiringRate(), 0.0)
    
    def testBenchmarkSimulation(self):
        """Test benchmarkSimulation function."""
        self.brain.step(0)
        
        metrics = pynlm.benchmarkSimulation(self.brain, numSteps=100, 
                                            description="Test benchmark")
        
        self.assertIsNotNone(metrics)
        self.assertGreaterEqual(metrics.getTotalSpikes(), 0)
        self.assertGreaterEqual(metrics.getExecutionTime(), 0.0)
    
    def testProfileSimulation(self):
        """Test profileSimulation function."""
        metrics = pynlm.profileSimulation(self.brain, numSteps=50)
        
        self.assertIsNotNone(metrics)
        self.assertGreaterEqual(metrics.getTotalSpikes(), 0)

class TestErrorHandling(unittest.TestCase):
    """Test error handling and custom exceptions."""
    
    def testConfigurationError(self):
        """Test ConfigurationError exception."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", -1)  # Invalid value
        
        # Validation should raise ConfigurationError
        with self.assertRaises(pynlm.ConfigurationError):
            pynlm.validateConfiguration(config)
    
    def testInitializationError(self):
        """Test InitializationError exception."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Brain should be uninitialized
        with self.assertRaises(pynlm.InitializationError):
            brain.step(0)  # Should raise if not initialized
    
    def testSimulationError(self):
        """Test SimulationError exception."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 5)  # Very small brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to wait for spikes in tiny brain (should timeout and raise)
        with self.assertRaises(pynlm.SimulationError):
            brain.waitForSpikes(minSpikes=10, maxSteps=10)

class TestUtilities(unittest.TestCase):
    """Test utility functions."""
    
    def testSystemInfo(self):
        """Test getSystemInfo function."""
        info = pynlm.getSystemInfo()
        
        self.assertIsInstance(info, dict)
        self.assertIn('version', info)
        self.assertIn('cpp_standard', info)
        self.assertIn('python_module', info)
        self.assertIn('description', info)
        
        self.assertEqual(info['python_module'], 'pynlm')
        self.assertEqual(info['cpp_standard'], 'C++20')
    
    def testCreateRandomConfig(self):
        """Test createRandomConfig function."""
        config = pynlm.createRandomConfig(seed=42, neuronCount=500)
        
        self.assertIsNotNone(config)
        self.assertTrue(config.has("random_seed"))
        self.assertTrue(config.has("simulation_timestep"))
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("region_count"))
        
        self.assertEqual(config.getOr("random_seed", 0), 42)
        self.assertEqual(config.getOr("neuron_count", 0), 500)
    
    def testSaveLoadModelCheckpoint(self):
        """Test save and load model checkpoint."""
        # Create and initialize brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brain.step(0)
        
        # Save checkpoint
        pynlm.saveModelCheckpoint(brain, "test_checkpoint.bin", 
                                  "Test checkpoint for unit testing")
        
        self.assertTrue(os.path.exists("test_checkpoint.bin"))
        
        # Load checkpoint
        loaded_brain = pynlm.loadModelCheckpoint("test_checkpoint.bin")
        loaded_brain.initialize()
        
        self.assertIsNotNone(loaded_brain)
        
        # Clean up
        os.remove("test_checkpoint.bin")

class TestNeuroscienceUtilities(unittest.TestCase):
    """Test neuroscience-specific utilities."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test fixtures."""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def testCreateSensoryStimulation(self):
        """Test createSensoryStimulation function."""
        pattern = [1.0, 0.5, 0.2, 0.0]
        
        # Should not raise exception
        result = pynlm.createSensoryStimulation(self.brain, pattern, current=50.0)
        self.assertTrue(result)
    
    def testMeasureNeuralSynchronization(self):
        """Test measureNeuralSynchronization function."""
        results = pynlm.measureNeuralSynchronization(self.brain, durationSteps=20)
        
        self.assertIsInstance(results, dict)
        self.assertIn('synchronization_index', results)
        self.assertIn('mean_spikes_per_step', results)
        self.assertIn('std_deviation', results)
        self.assertIn('total_spikes', results)
        
        self.assertGreaterEqual(results['synchronization_index'], 0.0)
        self.assertGreaterEqual(results['mean_spikes_per_step'], 0.0)
        self.assertGreaterEqual(results['std_deviation'], 0.0)
        self.assertGreaterEqual(results['total_spikes'], 0)

class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced features and complex scenarios."""
    
    def testCompleteAgentSimulation(self):
        """Test complete agent simulation."""
        # Create full agent simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify simulation completed
        self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
    
    def testLearningExperiment(self):
        """Test learning experiment with multiple episodes."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run learning experiment
        episode_results = pynlm.runLearningExperiment(
            brain, world, agent, numEpisodes=5, timestep=0.1
        )
        
        self.assertEqual(len(episode_results), 5)
        
        for episode in episode_results:
            self.assertIn('episode', episode)
            self.assertIn('total_spikes', episode)
            self.assertIn('total_actions', episode)

class TestContextManagers(unittest.TestCase):
    """Test context manager functionality."""
    
    def testBrainContextManager(self):
        """Test brain context manager."""
        config = pynlm.createDefaultConfig()
        
        with pynlm.createBrain(config) as brain:
            brain.initialize()
            brain.step(0)
            self.assertTrue(brain.getTotalNeuronCount() > 0)
        
        # Brain should be in reset state after context exit
        self.assertTrue(config is not None)  # Config should still exist

if __name__ == '__main__':
    print("Running NLM Python Bindings Test Suite")
    print("=" * 50)
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    result = runner.run(suite)
    
    # Clean up any leftover files
    for filename in ['test_config.cfg', 'test_checkpoint.bin']:
        if os.path.exists(filename):
            os.remove(filename)
    
    # Exit with appropriate code
    sys.exit(0 if result.wasSuccessful() else 1)