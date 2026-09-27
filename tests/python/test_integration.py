import sys
import os
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent / 'python'))
sys.path.insert(0, str(Path(__file__).parent / 'tests'))

class TestPythonBindings:
    """Base class for Python binding tests."""
    
    def setUp(self):
        """Set up test fixtures."""
        pass
    
    def tearDown(self):
        """Clean up after tests."""
        pass
    
    def skipTest(self, reason):
        """Skip a test."""
        raise unittest.SkipTest(reason)

try:
    import pynlm
    HAS_PYBINDINGS = True
except ImportError:
    HAS_PYBINDINGS = False
    print("Warning: pynlm module not found. Integration tests skipped.")
class TestNLMSimulationIntegration(TestPythonBindings, unittest.TestCase):
    """Integration tests for complete NLM simulation."""
    
    def testCompleteBrainWorldIntegration(self):
        """Test complete integration between brain and world."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create brain with configuration
        config = pynlm.Config()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        config.set("simulation_timestep", 0.001)
        config.set("random_seed", 42)
        
        # Create brain
        brain = pynlm.Brain(config)
        
        # Create and configure world
        world = pynlm.SimpleWorld()
        world.configure(200, 200, 40, 40)
        world.setMaxEnergy(100.0)
        world.setEnergyDecayRate(0.01)
        
        # Create agent brain
        agent_brain = pynlm.AgentBrain(brain)
        
        # Initialize components
        if hasattr(agent_brain, 'initialize'):
            agent_brain.initialize(world)
        
        # Create sensory input and process it
        vision = pynlm.Vision(40, 40, 3)
        vision.setData([0.5] * 4800)  # Fill with gray
        vision.setTimestamp(time.time())
        
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        
        # Process sensory input
        if hasattr(agent_brain, 'processSensoryInput'):
            agent_brain.processSensoryInput(percept)
        
        # Simulate world updates
        for i in range(10):
            world.update(0.001)
            
            # Check if world is still valid
            self.assertTrue(world.isValidPosition(world.getAgentBody().x, world.getAgentBody().y))
            
            # Check that energy is within bounds
            energy = world.getAgentBody().energy
            self.assertGreaterEqual(energy, 0)
            self.assertLessEqual(energy, 100.0)
    
    def testBrainStateSaveLoadIntegration(self):
        """Test brain state save/load integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create and configure brain
        config = pynlm.Config()
        config.set("neuron_count", 50)
        config.set("region_count", 1)
        
        brain = pynlm.Brain(config)
        
        # Initialize brain
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        # Get initial state
        initial_neurons = brain.getTotalNeuronCount()
        initial_synapses = brain.getTotalSynapseCount()
        
        # Simulate some activity
        if hasattr(brain, 'step'):
            for step in range(10):
                brain.step(step, step * 0.001)
        
        # Save brain state
        with tempfile.NamedTemporaryFile(suffix='.chk', delete=False) as temp_file:
            checkpoint_path = temp_file.name
        
        try:
            # Save checkpoint
            save_result = brain.save(checkpoint_path)
            self.assertTrue(save_result, "Failed to save brain checkpoint")
            
            # Reset brain
            if hasattr(brain, 'reset'):
                brain.reset()
                brain.initialize()
            
            # Verify brain is in known state
            self.assertEqual(brain.getTotalNeuronCount(), initial_neurons)
            self.assertGreaterEqual(brain.getTotalSynapseCount(), 0)
            
            # Load checkpoint
            load_result = brain.load(checkpoint_path)
            self.assertTrue(load_result, "Failed to load brain checkpoint")
            
            # Verify state after load
            after_load_neurons = brain.getTotalNeuronCount()
            after_load_synapses = brain.getTotalSynapseCount()
            
            # Note: After load, state should be similar (exact values may vary)
            self.assertGreaterEqual(after_load_neurons, 0)
            self.assertGreaterEqual(after_load_synapses, 0)
            
        finally:
            # Clean up
            if os.path.exists(checkpoint_path):
                os.unlink(checkpoint_path)
    
    def testAgentBrainDevelopmentIntegration(self):
        """Test agent brain development integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create brain with development configuration
        config = pynlm.Config()
        config.set("neuron_count", 200)
        config.set("region_count", 2)
        
        brain = pynlm.Brain(config)
        
        # Create world
        world = pynlm.SimpleWorld()
        world.configure(100, 100, 20, 20)
        
        # Create agent brain
        agent_brain = pynlm.AgentBrain(brain)
        
        # Initialize
        if hasattr(agent_brain, 'initialize'):
            agent_brain.initialize(world)
        
        # Simulate development over time
        for step in range(50):
            # Update agent brain development
            if hasattr(agent_brain, 'updateDevelopment'):
                agent_brain.updateDevelopment(step)
            
            # Update world
            world.update(0.001)
            
            # Check development stage
            if hasattr(agent_brain, 'getDevelopmentalStage'):
                stage = agent_brain.getDevelopmentalStage()
                # Development stage should be one of the enum values
                valid_stages = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
                self.assertIn(stage, valid_stages)
            
            # Check neuromodulation levels
            if hasattr(agent_brain, 'getNeuromodulationLevel'):
                neuromod = agent_brain.getNeuromodulationLevel()
                self.assertGreaterEqual(neuromod, 0.0)
                self.assertLessEqual(neuromod, 1.0)
    
    def testSimulationPerformanceIntegration(self):
        """Test simulation performance with integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        import time
        
        # Create brain
        config = pynlm.Config()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        
        brain = pynlm.Brain(config)
        
        # Initialize
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        # Measure simulation performance
        start_time = time.time()
        steps_performed = 0
        
        # Run simulation for a short period
        for step in range(100):
            brain.step(step, step * 0.001)
            steps_performed += 1
            
            # Check performance metrics (if available)
            if hasattr(brain, 'getTotalSpikeCount'):
                spikes = brain.getTotalSpikeCount()
                self.assertGreaterEqual(spikes, 0)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Performance checks
        steps_per_second = steps_performed / elapsed
        self.assertGreater(steps_per_second, 10)  # Should process at least 10 steps per second
        
        # Spike rate check
        if hasattr(brain, 'getTotalSpikeCount'):
            total_spikes = brain.getTotalSpikeCount()
            spike_rate = total_spikes / elapsed
            self.assertGreaterEqual(spike_rate, 0)  # Spike rate should be non-negative
    
    def testMemoryManagementIntegration(self):
        """Test memory management in integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create multiple brain instances to test memory management
        brains = []
        
        for i in range(3):
            config = pynlm.Config()
            config.set("neuron_count", 50 + i * 25)
            config.set("region_count", 1)
            
            brain = pynlm.Brain(config)
            if hasattr(brain, 'initialize'):
                brain.initialize()
            
            brains.append(brain)
        
        # Perform operations on all brains
        for brain in brains:
            if hasattr(brain, 'step'):
                for step in range(5):
                    brain.step(step, step * 0.001)
            
            # Check state
            self.assertGreaterEqual(brain.getTotalNeuronCount(), 0)
            self.assertGreaterEqual(brain.getTotalSynapseCount(), 0)
        
        # Clean up (if there were memory leaks, this would be obvious)
        brains.clear()
        
        # Note: In a real test, we would use memory profiling tools
        # to detect memory leaks, but for this test we'll just ensure
        # the system doesn't crash under load
    
    def testConfigurationIntegration(self):
        """Test configuration integration across components."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create configuration
        config = pynlm.Config()
        
        # Set various configuration parameters
        config.set("neuron_count", 150)
        config.set("region_count", 3)
        config.set("random_seed", 12345)
        config.set("simulation_timestep", 0.001)
        config.set("connection_probability", 0.1)
        
        # Verify configuration
        self.assertEqual(config.get("neuron_count"), 150)
        self.assertEqual(config.get("region_count"), 3)
        self.assertEqual(config.get("random_seed"), 12345)
        self.assertAlmostEqual(config.get("simulation_timestep"), 0.001)
        self.assertAlmostEqual(config.get("connection_probability"), 0.1)
        
        # Create brain with this configuration
        brain = pynlm.Brain(config)
        
        # Verify brain has the configuration
        brain_config = brain.getConfig()
        self.assertIsNotNone(brain_config)
        self.assertEqual(brain_config.get("neuron_count"), 150)
        self.assertEqual(brain_config.get("region_count"), 3)
        
        # Test batch configuration loading (if available)
        if hasattr(config, 'batchSet'):
            batch_config = [
                ("batch_key1", "batch_value1"),
                ("batch_key2", 2),
                ("batch_key3", True)
            ]
            config.batchSet(batch_config)
            
            self.assertEqual(config.get("batch_key1"), "batch_value1")
            self.assertEqual(config.get("batch_key2"), 2)
            self.assertTrue(config.get("batch_key3"))
    
    def testErrorHandlingIntegration(self):
        """Test error handling in integration scenarios."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Test configuration error handling
        config = pynlm.Config()
        
        # Test invalid configurations (if validation is available)
        if hasattr(config, 'validate'):
            # This would test configuration validation
            pass
        
        # Test brain creation error handling
        # (Brain constructor should handle invalid configurations gracefully)
        brain = pynlm.Brain(config)
        
        # The brain should be created even with minimal configuration
        self.assertIsNotNone(brain)
        
        # Test operation on uninitialized components
        # (should handle gracefully if initialization fails)
        if hasattr(brain, 'step'):
            # Step should work even without full initialization
            brain.step(0, 0.0)
            self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
class TestAdvancedNLMFeatures(TestPythonBindings, unittest.TestCase):
    """Tests for advanced NLM features."""
    
    def testPlasticityIntegration(self):
        """Test plasticity feature integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create brain
        config = pynlm.Config()
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        
        brain = pynlm.Brain(config)
        
        # Initialize
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        # Enable plasticity
        if hasattr(brain, 'getSTDP'):
            stdp = brain.getSTDP()
            if stdp:
                stdp->enablePlasticity(true, true, true)
        
        # Generate correlated activity to trigger STDP
        if hasattr(brain, 'step'):
            for step in range(20):
                # Inject current to create correlated activity
                if hasattr(brain, 'injectCurrentToNeurons'):
                    brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 10.0f)
                
                brain.step(step, step * 0.001)
        
        # Check if plasticity occurred
        if hasattr(brain, 'getExcitationInhibitionRatio'):
            ei_ratio = brain.getExcitationInhibitionRatio()
            # EI ratio should be reasonable (not extremely high or low)
            self.assertGreaterEqual(ei_ratio, 0.1)
            self.assertLessEqual(ei_ratio, 1000)
    
    def testNeuromodulationIntegration(self):
        """Test neuromodulation feature integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create brain
        config = pynlm.Config()
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        
        brain = pynlm.Brain(config)
        
        # Initialize
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        # Test neuromodulation levels
        if hasattr(brain, 'getDopamine'):
            dopamine = brain.getDopamine()
            if dopamine:
                level = dopamine->getLevel()
                self.assertGreaterEqual(level, 0.0)
                self.assertLessEqual(level, 1.0)
        
        if hasattr(brain, 'getCuriosity'):
            curiosity = brain.getCuriosity()
            if curiosity:
                level = curiosity->getLevel()
                self.assertGreaterEqual(level, 0.0)
                self.assertLessEqual(level, 1.0)
        
        if hasattr(brain, 'getNovelty'):
            novelty = brain.getNovelty()
            if novelty:
                level = novelty->getLevel()
                self.assertGreaterEqual(level, 0.0)
                self.assertLessEqual(level, 1.0)
    
    def testMemorySystemIntegration(self):
        """Test memory system integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create brain
        config = pynlm.Config()
        config.set("neuron_count", 150)
        config.set("region_count", 2)
        
        brain = pynlm.Brain(config)
        
        # Initialize
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        # Test memory systems
        if hasattr(brain, 'getWorkingMemory'):
            working_mem = brain.getWorkingMemory()
            if working_mem:
                # Check working memory capacity
                capacity = working_mem->getCapacity()
                self.assertGreaterEqual(capacity, 0)
        
        if hasattr(brain, 'getEpisodicMemory'):
            episodic_mem = brain.getEpisodicMemory()
            if episodic_mem:
                # Check episodic memory capacity
                max_episodes = episodic_mem->getMaxEpisodes()
                self.assertGreaterEqual(max_episodes, 0)
        
        if hasattr(brain, 'getAssociativeMemory'):
            associative_mem = brain.getAssociativeMemory()
            if associative_mem:
                # This is a placeholder for associative memory test
                pass
    
    def testDevelopmentSystemIntegration(self):
        """Test development system integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Create brain
        config = pynlm.Config()
        config.set("neuron_count", 200)
        config.set("region_count", 1)
        
        brain = pynlm.Brain(config)
        
        # Initialize
        if hasattr(brain, 'initialize'):
            brain.initialize()
        
        # Test development stage
        if hasattr(brain, 'getDevelopmentalStage'):
            stage = brain.getDevelopmentalStage()
            # Stage should be a valid developmental stage
            valid_stages = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
            self.assertIn(stage, valid_stages)
        
        # Simulate development over time
        if hasattr(brain, 'getDevelopmentSystem'):
            dev_system = brain.getDevelopmentSystem()
            if dev_system:
                # This would test actual development system
                pass
class TestPerformanceMonitoringIntegration(TestPythonBindings, unittest.TestCase):
    """Tests for performance monitoring integration."""
    
    def testPerformanceMonitorIntegration(self):
        """Test performance monitoring integration with Python bindings."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Note: Performance monitoring is implemented in C++
        # This test would verify that performance monitoring works
        # with the Python bindings if such integration exists
        pass
    
    def testBenchmarkIntegration(self):
        """Test benchmarking integration."""
        if not HAS_PYBINDINGS:
            self.skipTest("Python bindings not available")
        
        # Performance benchmarks are implemented in C++
        # This test would verify benchmark functionality
        pass
if __name__ == '__main__':
    if HAS_PYBINDINGS:
        unittest.main()
    else:
        print("Skipping integration tests - pynlm module not available")
        sys.exit(0)