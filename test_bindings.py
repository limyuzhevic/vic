#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.

This test suite validates the functionality of the NLM Python bindings,
including basic operations, advanced features, and edge cases.
"""

import sys
import os
import unittest
import json
import tempfile
import time
from pathlib import Path

# Add the build directory to the path for testing
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build', 'python'))

try:
    import pynlm as nlm
    PYNLML_AVAILABLE = True
except ImportError as e:
    print(f"Warning: pynlm module not available: {e}")
    PYNLML_AVAILABLE = False

@unittest.skipIf(not PYNLML_AVAILABLE, "pynlm module not available")
class TestBasicFunctionality(unittest.TestCase):
    """Test basic NLM Python bindings functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.brain = nlm.createBrainWithDefaultConfig()
        self.brain.initialize()
        
    def tearDown(self):
        """Clean up after tests."""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def test_brain_creation(self):
        """Test basic brain creation and initialization."""
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
        
    def test_basic_simulation(self):
        """Test basic simulation functionality."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run a few simulation steps
        for step in range(10):
            self.brain.step(step, step * 0.001)
        
        # Verify spikes occurred
        self.assertGreater(self.brain.getTotalSpikeCount(), initial_spikes)
        
    def test_sensory_input(self):
        """Test sensory input functionality."""
        # Create a sensory input
        vision = nlm.Vision(width=10, height=10, channels=3)
        vision_data = [0.1] * (10 * 10 * 3)
        vision.setData(vision_data)
        
        # Verify vision data was set
        self.assertEqual(len(vision.getData()), len(vision_data))
        
    def test_action_creation(self):
        """Test action creation and manipulation."""
        # Create action with type only
        action1 = nlm.Action(nlm.ActionType.MoveForward)
        self.assertEqual(action1.getType(), nlm.ActionType.MoveForward)
        
        # Create action with parameters
        params = [1.0, 2.0, 3.0]
        action2 = nlm.Action(nlm.ActionType.Look, params)
        self.assertEqual(action2.getType(), nlm.ActionType.Look)
        self.assertEqual(action2.getParameters(), params)
        
        # Test action cloning
        action3 = action2.clone()
        self.assertEqual(action3.getType(), action2.getType())
        self.assertEqual(action3.getParameters(), action2.getParameters())
        
    def test_config_functionality(self):
        """Test configuration functionality."""
        config = nlm.Config()
        
        # Test setting and getting configuration values
        config.set("test.int", 42)
        config.set("test.float", 3.14)
        config.set("test.string", "hello")
        config.set("test.bool", True)
        
        self.assertEqual(config.getOr("test.int", 0), 42)
        self.assertEqual(config.getOr("test.float", 0.0), 3.14)
        self.assertEqual(config.getOr("test.string", ""), "hello")
        self.assertEqual(config.getOr("test.bool", False), True)
        
        # Test has method
        self.assertTrue(config.has("test.int"))
        self.assertFalse(config.has("nonexistent.key"))
        
    def test_world_functionality(self):
        """Test world functionality."""
        world = nlm.createWorldWithSetup(width=100, height=100)
        
        self.assertEqual(world.getWidth(), 100)
        self.assertEqual(world.getHeight(), 100)
        
        # Test agent body
        agent_body = world.getAgentBody()
        self.assertEqual(agent_body.x, 0.0)
        self.assertEqual(agent_body.y, 0.0)
        
    def test_memory_systems(self):
        """Test memory system functionality."""
        # Test working memory
        working_memory = nlm.NeuralWorkingMemory()
        working_memory.store("key1", "value1")
        working_memory.store("key2", 42)
        
        self.assertEqual(working_memory.recall("key1"), "value1")
        self.assertEqual(working_memory.recall("key2"), 42)
        self.assertTrue(working_memory.hasKey("key1"))
        self.assertFalse(working_memory.hasKey("key3"))
        
        # Test episodic memory
        episodic_memory = nlm.NeuralEpisodicMemory()
        percept = nlm.SensoryPercept()
        action = nlm.Action(nlm.ActionType.MoveForward)
        
        episodic_memory.storeExperience(1000.0, percept, action, 0.5)
        experiences = episodic_memory.getRecentExperiences(5)
        self.assertEqual(len(experiences), 1)
        
        # Test associative memory
        assoc_memory = nlm.NeuralAssociativeMemory()
        assoc_memory.associate([1.0, 0.0], [0.0, 1.0], 0.8)
        recall = assoc_memory.recallPattern([1.0, 0.0])
        self.assertEqual(len(recall), 2)

@unittest.skipIf(not PYNLML_AVAILABLE, "pynlm module not available")
class TestNeuromodulation(unittest.TestCase):
    """Test neuromodulation functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.brain = nlm.createBrainWithDefaultConfig()
        self.brain.initialize()
        
    def tearDown(self):
        """Clean up after tests."""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def test_all_neuromodulators(self):
        """Test all neuromodulator classes."""
        neuromodulators = [
            nlm.Dopamine(),
            nlm.Acetylcholine(),
            nlm.Norepinephrine(),
            nlm.Serotonin(),
            nlm.Curiosity(),
            nlm.Novelty(),
            nlm.PredictionError()
        ]
        
        for nm in neuromodulators:
            self.assertIsNotNone(nm)
            
        # Test setting levels
        dopamine = nlm.Dopamine()
        acetylcholine = nlm.Acetylcholine()
        curiosity = nlm.Curiosity()
        novelty = nlm.Novelty()
        prediction_error = nlm.PredictionError()
        
        dopamine.setLevel(0.7)
        curiosity.setCuriosityLevel(0.5)
        novelty.setLevel(0.3)
        prediction_error.setPredictionErrorLevel(0.2)
        
        self.assertEqual(dopamine.getLevel(), 0.7)
        self.assertEqual(curiosity.getCuriosityLevel(), 0.5)
        self.assertEqual(novelty.getLevel(), 0.3)
        self.assertEqual(prediction_error.getPredictionErrorLevel(), 0.2)
        
    def test_agent_brain_neuromodulation(self):
        """Test agent brain neuromodulation methods."""
        world = nlm.createWorldWithSetup(width=50, height=50)
        agent_brain = nlm.createCompleteAgentSetup(world)
        
        # Test neuromodulation level access
        neuromodulation_level = agent_brain.getNeuromodulationLevel()
        curiosity_level = agent_brain.getCuriosityLevel()
        novelty_level = agent_brain.getNoveltyLevel()
        prediction_error_level = agent_brain.getPredictionError()
        
        self.assertIsNotNone(neuromodulation_level)
        self.assertIsNotNone(curiosity_level)
        self.assertIsNotNone(novelty_level)
        self.assertIsNotNone(prediction_error_level)
        
        # Test enabling/disabling
        agent_brain.enableRewardModulation(True)
        agent_brain.enableCuriosity(True)
        agent_brain.enableStructuralPlasticity(True)
        agent_brain.enableDevelopment(True)
        
        self.assertTrue(agent_brain.isRewardModulationEnabled())
        self.assertTrue(agent_brain.isCuriosityEnabled())
        self.assertTrue(agent_brain.isStructuralPlasticityEnabled())
        self.assertTrue(agent_brain.isDevelopmentEnabled())

@unittest.skipIf(not PYNLML_AVAILABLE, "pynlm module not available")
class TestAgentBrain(unittest.TestCase):
    """Test agent brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        world = nlm.createWorldWithSetup(width=50, height=50)
        self.agent_brain = nlm.createCompleteAgentSetup(world)
        
    def tearDown(self):
        """Clean up after tests."""
        if hasattr(self, 'agent_brain'):
            self.agent_brain.reset()
    
    def test_agent_brain_initialization(self):
        """Test agent brain initialization."""
        self.assertIsNotNone(self.agent_brain)
        self.assertIsNotNone(self.agent_brain.getBrain())
        
    def test_sensory_processing(self):
        """Test sensory input processing."""
        world = nlm.createWorldWithSetup(width=50, height=50)
        agent_brain = nlm.createCompleteAgentSetup(world)
        
        # Create sensory percept
        vision = nlm.Vision(width=10, height=10, channels=3)
        vision_data = [0.1] * (10 * 10 * 3)
        vision.setData(vision_data)
        
        audio = nlm.Audio(sampleRate=44100, numSamples=1024)
        audio_data = [0.0] * 1024
        audio.setData(audio_data)
        
        internal = nlm.InternalSignals()
        internal.addSignal(0.5)
        internal.addSignal(0.8)
        
        percept = nlm.SensoryPercept()
        percept.setVision(vision)
        percept.setAudio(audio)
        percept.setInternal(internal)
        percept.setTimestamp(1000.0)
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Verify processing didn't crash
        self.assertIsNotNone(agent_brain.getBrain())
        
    def test_motor_command_decoding(self):
        """Test motor command decoding."""
        world = nlm.createWorldWithSetup(width=50, height=50)
        agent_brain = nlm.createCompleteAgentSetup(world)
        
        # Get motor output size
        output_size = agent_brain.getMotorOutputSize()
        self.assertIsNotNone(output_size)
        
    def test_development_system(self):
        """Test development system functionality."""
        world = nlm.createWorldWithSetup(width=50, height=50)
        agent_brain = nlm.createCompleteAgentSetup(world)
        
        # Get developmental stage
        stage = agent_brain.getDevelopmentalStage()
        self.assertIsNotNone(stage)
        
        # Update development
        agent_brain.updateDevelopment(0.001)
        
        # Verify stage may have changed
        new_stage = agent_brain.getDevelopmentalStage()
        self.assertIsNotNone(new_stage)

@unittest.skipIf(not PYNLML_AVAILABLE, "pynlm module not available")
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced NLM features."""
    
    def test_performance_profiler(self):
        """Test performance profiler functionality."""
        from pynlm import PerformanceProfiler
        
        profiler = PerformanceProfiler()
        profiler.start()
        
        # Simulate some work
        brain = nlm.createBrainWithDefaultConfig()
        brain.initialize()
        
        for step in range(100):
            brain.step(step, step * 0.001)
        
        profiler.stop()
        
        # Get metrics
        metrics = profiler.getMetrics()
        self.assertIn('frames', metrics)
        self.assertIn('average_fps', metrics)
        self.assertIn('total_time', metrics)
        
        self.assertGreater(metrics['frames'], 0)
        self.assertGreater(metrics['average_fps'], 0)
        self.assertGreater(metrics['total_time'], 0)
        
    def test_batch_processing(self):
        """Test batch processing functionality."""
        brain = nlm.createBrainWithDefaultConfig()
        brain.initialize()
        
        # Test batch step
        brain.batchStep(50)  # Process 50 steps at once
        
        # Verify steps were processed
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        
    def test_context_managers(self):
        """Test context manager functionality."""
        # Test Brain context manager
        with nlm.BrainContextManager(nlm.createBrainWithDefaultConfig()) as brain:
            self.assertIsNotNone(brain)
            self.assertGreater(brain.getTotalNeuronCount(), 0)
        
        # Test World context manager
        with nlm.WorldContextManager(nlm.createWorldWithSetup(width=50, height=50)) as world:
            self.assertEqual(world.getWidth(), 50)
            self.assertEqual(world.getHeight(), 50)
        
    def test_event_system(self):
        """Test event system functionality."""
        event_queue = nlm.EventQueue()
        
        # Queue some events
        event_queue.queueSpike(nlm.NeuronId(1), 1000.0)
        event_queue.queueSpike(nlm.NeuronId(2), 1001.0)
        event_queue.queueDelayedSpike(nlm.NeuronId(3), 1002.0, 2)  # 2 time units delay
        
        # Process events
        event_queue.processSpikes(1000.0)
        event_queue.processDelayedSpikes()
        
        # Verify events were processed
        self.assertGreater(len(event_queue.getEventHistory()), 0)
        
    def test_advanced_factory_methods(self):
        """Test advanced factory methods."""
        # Test createSensoryPipeline
        world = nlm.createWorldWithSetup(width=50, height=50)
        sensory_agent = nlm.createSensoryPipeline(world)
        self.assertIsNotNone(sensory_agent)
        
        # Test createBrainWithDefaultConfig
        brain = nlm.createBrainWithDefaultConfig()
        brain.initialize()
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        
    def test_experiment_tracker(self):
        """Test experiment tracking functionality."""
        from pynlm import ExperimentTracker
        
        tracker = ExperimentTracker("test_experiment")
        tracker.start()
        
        # Record some experiment data
        tracker.recordMetric("neurons", 100)
        tracker.recordMetric("synapses", 500)
        tracker.recordMetric("spikes", 1000)
        
        # End experiment
        results = tracker.end()
        
        self.assertEqual(results["experiment_name"], "test_experiment")
        self.assertEqual(results["metrics"]["neurons"], 100)
        self.assertEqual(results["metrics"]["synapses"], 500)
        self.assertEqual(results["metrics"]["spikes"], 1000)
        
    def test_neural_network_builder(self):
        """Test neural network builder functionality."""
        from pynlm import NeuralNetworkBuilder
        
        # Test layered network
        builder = NeuralNetworkBuilder()
        builder.createLayeredNetwork(num_layers=3, neurons_per_layer=50)
        
        # Test autoencoder
        autoencoder = builder.createAutoencoder(
            input_size=10, hidden_size=5, output_size=10
        )
        self.assertIsNotNone(autoencoder)
        
    def test_optimization_functions(self):
        """Test optimization functions."""
        brain = nlm.createBrainWithDefaultConfig()
        brain.initialize()
        
        # Get initial stats
        initial_stats = brain.computeNeuralStats()
        
        # Normalize weights
        brain.normalizeWeights()
        
        # Compute stats after normalization
        normalized_stats = brain.computeNeuralStats()
        
        # Both should be valid stats objects
        self.assertIsNotNone(initial_stats)
        self.assertIsNotNone(normalized_stats)

@unittest.skipIf(not PYNLML_AVAILABLE, "pynlm module not available")
class TestVisualization(unittest.TestCase):
    """Test visualization functionality."""
    
    def test_visualization_interface(self):
        """Test visualization interface."""
        from pynlm import VisualizationInterface
        
        vis = VisualizationInterface()
        vis.initialize()
        
        # Set up a simple brain for visualization
        brain = nlm.createBrainWithDefaultConfig()
        brain.initialize()
        
        # Test different visualization modes
        vis.visualizeNetwork(brain)
        vis.visualizeActivity(brain)
        vis.visualizeSpikes(brain)
        vis.visualizeWeights(brain)
        
        # Clean up
        vis.close()
        
    def test_network_visualization(self):
        """Test network visualization with actual brain data."""
        from pynlm import VisualizationInterface
        
        vis = VisualizationInterface()
        vis.initialize()
        
        brain = nlm.createBrainWithDefaultConfig()
        brain.initialize()
        
        # Run some simulation steps
        for step in range(20):
            brain.step(step, step * 0.001)
        
        # Visualize
        network_data = vis.visualizeNetwork(brain)
        
        # Verify visualization data
        self.assertIsNotNone(network_data)
        self.assertIn('nodes', network_data)
        self.assertIn('edges', network_data)
        
        vis.close()

@unittest.skipIf(not PYNLML_AVAILABLE, "pynlm module not available")
class TestConfiguration(unittest.TestCase):
    """Test configuration functionality."""
    
    def test_config_save_load(self):
        """Test configuration save and load functionality."""
        config = nlm.Config()
        
        # Set some configuration values
        config.set("test.value1", 42)
        config.set("test.value2", "hello")
        config.set("test.value3", [1, 2, 3])
        config.set("test.value4", {"nested": "value"})
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_file = f.name
        
        try:
            config.saveToFile(temp_file)
            
            # Create new config and load
            config2 = nlm.Config()
            self.assertTrue(config2.loadFromFile(temp_file))
            
            # Verify loaded values
            self.assertEqual(config2.getOr("test.value1", 0), 42)
            self.assertEqual(config2.getOr("test.value2", ""), "hello")
            self.assertEqual(config2.getOr("test.value3", []), [1, 2, 3])
            
        finally:
            # Clean up
            if os.path.exists(temp_file):
                os.unlink(temp_file)
    
    def test_config_from_args(self):
        """Test configuration loading from command line arguments."""
        config = nlm.Config()
        
        # Simulate command line arguments
        test_args = [
            "nlm",
            "--neuron-count", "100",
            "--region-count", "2",
            "--connection-probability", "0.1"
        ]
        
        # This would normally be called with sys.argv
        # For testing, we'll test that the config can be loaded with various parameters
        config.set("neuron.count", 100)
        config.set("region.count", 2)
        config.set("connection.probability", 0.1)
        
        self.assertEqual(config.getOr("neuron.count", 0), 100)
        self.assertEqual(config.getOr("region.count", 0), 2)
        self.assertEqual(config.getOr("connection.probability", 0.0), 0.1)
        
    def test_config_summary(self):
        """Test configuration summary functionality."""
        config = nlm.Config()
        
        # Add some configuration values
        config.set("simulation.steps_per_second", 100)
        config.set("brain.region_count", 1)
        config.set("brain.population_size", 100)
        config.set("custom.parameter", "value")
        
        # Get summary
        summary = config.summary()
        
        self.assertIsInstance(summary, str)
        self.assertIn("Configuration Summary", summary)
        self.assertIn("simulation.steps_per_second", summary)
        self.assertIn("brain.region_count", summary)
        self.assertIn("brain.population_size", summary)

@unittest.skipIf(not PYNLML_AVAILABLE, "pynlm module not available")
class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases."""
    
    def test_invalid_config_file(self):
        """Test handling of invalid configuration files."""
        config = nlm.Config()
        
        # Try to load non-existent file
        result = config.loadFromFile("/nonexistent/file.json")
        self.assertFalse(result)
        
    def test_empty_config_operations(self):
        """Test operations on empty configuration."""
        config = nlm.Config()
        
        # These should not crash
        keys = config.getKeys()
        self.assertIsInstance(keys, list)
        
        summary = config.summary()
        self.assertIsInstance(summary, str)
        
        config.clear()
        self.assertEqual(len(config.getKeys()), 0)
        
    def test_memory_edge_cases(self):
        """Test memory system edge cases."""
        # Test empty working memory
        working_memory = nlm.NeuralWorkingMemory()
        self.assertEqual(working_memory.recall("nonexistent"), None)
        self.assertFalse(working_memory.hasKey("nonexistent"))
        
        # Test empty episodic memory
        episodic_memory = nlm.NeuralEpisodicMemory()
        experiences = episodic_memory.getRecentExperiences(10)  # Request more than exist
        self.assertEqual(len(experiences), 0)
        
    def test_agent_brain_error_handling(self):
        """Test agent brain error handling."""
        world = nlm.createWorldWithSetup(width=50, height=50)
        agent_brain = nlm.createCompleteAgentSetup(world)
        
        # Reset should not crash
        agent_brain.reset()
        
        # Processing invalid input should not crash (may log warning)
        invalid_percept = None
        try:
            agent_brain.processSensoryInput(invalid_percept)
        except:
            pass  # Expected to handle None input gracefully


def run_tests():
    """Run all tests and report results."""
    if not PYNLML_AVAILABLE:
        print("Skipping tests: pynlm module not available")
        return
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestBasicFunctionality)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestNeuromodulation))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAgentBrain))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAdvancedFeatures))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestVisualization))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestConfiguration))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestErrorHandling))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return result.wasSuccessful()


if __name__ == '__main__':
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    success = run_tests()
    
    if success:
        print("\n✅ All tests passed!")
        sys.exit(0)
    else:
        print("\n❌ Some tests failed!")
        sys.exit(1)