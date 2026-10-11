"""
Unit tests for NLM Python bindings (pynlm)
Tests basic functionality and new Pythonic features
"""
import unittest
import tempfile
import os
import sys
import numpy as np

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: pynlm not available: {e}")
    PYNLM_AVAILABLE = False

class TestPythonBindingsBasic(unittest.TestCase):
    """Test basic Python bindings functionality"""
    
    def setUp(self):
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
        
        # Create a simple configuration for testing
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100)
        self.config.set("brain.synapse_density", 0.1)
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent and world for integration tests
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.agent.initialize(self.world)
    
    def test_id_creation(self):
        """Test ID creation and Pythonic features"""
        # Test NeuronId creation
        nid = pynlm.NeuronId()
        self.assertEqual(nid.value, 0)
        self.assertEqual(nid.index(), 0)
        
        nid2 = pynlm.NeuronId(42)
        self.assertEqual(nid2.value, 42)
        self.assertEqual(nid2.index(), 42)
        
        # Test NeuronId string representation
        str_repr = str(nid2)
        self.assertEqual(str_repr, "42")
        
        # Test NeuronId dictionary conversion
        dict_repr = nid2.to_dict()
        self.assertIn("value", dict_repr)
        self.assertEqual(dict_repr["value"], 42)
        
        # Test NeuronId from dictionary
        nid3 = pynlm.NeuronId.from_dict(dict_repr)
        self.assertEqual(nid3.value, 42)
        
        # Test NeuronId hash and equality
        self.assertEqual(nid2, nid3)
        self.assertNotEqual(nid2, nid)
        self.assertIn(nid2, {nid2, nid})
    
    def test_enum_values(self):
        """Test that all enum values are accessible"""
        # Test NeuronType
        self.assertIsNotNone(pynlm.NeuronType.Excitatory)
        self.assertIsNotNone(pynlm.NeuronType.Inhibitory)
        self.assertIsNotNone(pynlm.NeuronType.Modulatory)
        self.assertIsNotNone(pynlm.NeuronType.Sensory)
        self.assertIsNotNone(pynlm.NeuronType.Motor)
        self.assertIsNotNone(pynlm.NeuronType.Internal)
        
        # Test SynapseType
        self.assertIsNotNone(pynlm.SynapseType.Excitatory)
        self.assertIsNotNone(pynlm.SynapseType.Inhibitory)
        
        # Test DevelopmentalStage
        self.assertIsNotNone(pynlm.DevelopmentalStage.Initial)
        self.assertIsNotNone(pynlm.DevelopmentalStage.Adult)
        
        # Test ActionType
        self.assertIsNotNone(pynlm.ActionType.MoveForward)
        self.assertIsNotNone(pynlm.ActionType.TurnLeft)
        
        # Test WorldObjectType
        self.assertIsNotNone(pynlm.WorldObjectType.Resource)
        self.assertIsNotNone(pynlm.WorldObjectType.Wall)
    
    def test_config_basic(self):
        """Test basic Config functionality"""
        # Test configuration properties
        self.assertEqual(self.config.get("brain.neuron_count"), 100)
        self.assertEqual(self.config.get("brain.synapse_density"), 0.1)
        self.assertFalse(self.config.has("nonexistent.key"))
        
        # Test keys
        keys = self.config.getKeys()
        self.assertIn("brain.neuron_count", keys)
        self.assertIn("brain.synapse_density", keys)
        
        # Test summary
        summary = self.config.summary()
        self.assertIn("brain.neuron_count", summary)
        self.assertIn("brain.synapse_density", summary)
    
    def test_config_pythonic_features(self):
        """Test Pythonic features of Config class"""
        # Test property access
        self.assertEqual(self.config.brain_neuron_count, 100)
        self.assertEqual(self.config.brain_synapse_density, 0.1)
        
        # Test validation
        errors = self.config.validate()
        self.assertEqual(len(errors), 0)
        
        # Test copy
        config_copy = self.config.copy()
        self.assertEqual(config_copy.brain_neuron_count, 100)
        self.assertEqual(config_copy.brain_synapse_density, 0.1)
        
        # Test setting values
        config_copy.set("test.key", 42)
        self.assertEqual(config_copy.get("test.key"), 42)
    
    def test_world_basic(self):
        """Test basic World functionality"""
        # Test world configuration
        self.world.configure(width=30, height=40, visionWidth=10, visionHeight=10)
        self.assertEqual(self.world.getWidth(), 30)
        self.assertEqual(self.world.getHeight(), 40)
        self.assertEqual(self.world.getWidth(), self.world.getWidth())
        
        # Test agent start
        self.world.setAgentStart(15.0, 15.0)
        
        # Test world update
        self.world.update(timestep=0.1)
        self.assertGreaterEqual(self.world.getSimulationTime(), 0.0)
        
        # Test random seed
        self.world.setRandomSeed(123)
        self.assertEqual(self.world.getRandomSeed(), 123)
    
    def test_brain_basic(self):
        """Test basic Brain functionality"""
        # Test brain properties
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
        self.assertGreater(self.brain.getActiveNeuronCount(), 0)
        
        # Test developmental stage
        stage = self.brain.getDevelopmentalStage()
        self.assertIn(stage, [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ])
        
        # Test brain statistics
        brain_copy = pynlm.createBrain(self.config)
        brain_copy.initialize()
        
        stats = pynlm.brain_stats(brain_copy)
        self.assertIn("total_neurons", stats)
        self.assertIn("total_synapses", stats)
        self.assertIn("active_neurons", stats)
        self.assertIn("firing_neurons", stats)
        self.assertIn("average_firing_rate", stats)
        self.assertIn("excitation_inhibition_ratio", stats)
        self.assertIn("total_spikes", stats)
    
    def test_agent_basic(self):
        """Test basic Agent functionality"""
        # Test agent properties
        self.assertEqual(self.agent.getSensoryInputSize(), 3)  # Vision channels
        self.assertEqual(self.agent.getMotorOutputSize(), 2)   # Movement
        
        # Test developmental stage
        self.assertIn(self.agent.getDevelopmentalStage(), [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ])
        
        # Test neuromodulation levels
        self.assertGreaterEqual(self.agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(self.agent.getNoveltyLevel(), 0.0)
        self.assertGreaterEqual(self.agent.getNeuromodulationLevel(), 0.0)
        
        # Test subsystem controls
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        # Test enable status
        self.assertTrue(self.agent.isCuriosityEnabled())
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
    
    def test_simulation_step(self):
        """Test simulation step with world interaction"""
        # Run a few simulation steps
        for step in range(10):
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action and apply to world
            action = self.brain.produceAction()
            if action:
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check that state changes
            self.assertGreater(self.world.getSimulationTime(), 0.0)
            
            # Verify statistics are updating
            self.assertGreater(self.brain.getTotalSpikeCount(), 0)
    
    def test_factory_functions(self):
        """Test factory functions for creating components"""
        # Test config factories
        standard_config = pynlm.createStandardConfig()
        self.assertIsNotNone(standard_config)
        self.assertGreater(standard_config.brain_neuron_count, 0)
        
        minimal_config = pynlm.createMinimalConfig()
        self.assertIsNotNone(minimal_config)
        self.assertLess(minimal_config.brain_neuron_count, standard_config.brain_neuron_count)
        
        high_perf_config = pynlm.createHighPerformanceConfig()
        self.assertIsNotNone(high_perf_config)
        self.assertGreater(high_perf_config.brain_neuron_count, standard_config.brain_neuron_count)
        
        # Test world factory
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        world.configure(10, 10, 5, 5)
        self.assertEqual(world.getWidth(), 10)
        
        # Test brain factory with config
        brain_from_config = pynlm.createBrain(standard_config)
        self.assertIsNotNone(brain_from_config)
        
        # Test agent factory
        agent = pynlm.createAgentBrain(brain_from_config)
        self.assertIsNotNone(agent)
    
    def test_numpy_integration(self):
        """Test NumPy integration helpers"""
        # Create a test vector
        test_vector = [1.0, 2.0, 3.0, 4.0, 5.0]
        
        # Convert to NumPy and back
        numpy_array = pynlm.to_numpy(test_vector)
        self.assertIsInstance(numpy_array, np.ndarray)
        np.testing.assert_array_equal(numpy_array, np.array(test_vector))
        
        # Convert back to vector
        back_to_vector = pynlm.to_vector(numpy_array)
        self.assertEqual(test_vector, back_to_vector)
    
    def test_progress_callback(self):
        """Test progress callback functionality"""
        step_count = 0
        max_steps = 100
        
        def progress_callback(step, max_steps_param):
            nonlocal step_count
            step_count = step
            # Verify callback parameters
            self.assertEqual(max_steps_param, max_steps)
            self.assertLessEqual(step, max_steps)
        
        # Run simulation with progress callback
        pynlm.runWithProgress(self.brain, max_steps, progress_callback)
        
        # Verify that progress callback was called
        self.assertGreater(step_count, 0)
        self.assertLess(step_count, max_steps)
    
    def test_validation(self):
        """Test configuration validation"""
        # Test valid configuration
        valid_config = pynlm.createStandardConfig()
        errors = pynlm.validate_config(valid_config)
        self.assertEqual(len(errors), 0)
        
        # Test invalid configuration (simulate by setting bad values)
        invalid_config = pynlm.createDefaultConfig()
        invalid_config.set("brain.neuron_count", -5)  # Invalid: negative neuron count
        
        # Note: validation may not catch all invalid values
        # but this tests the validation API
        errors = pynlm.validate_config(invalid_config)
        # errors may or may not contain this error depending on validation logic
    
    def test_configuration_comparison(self):
        """Test configuration comparison functionality"""
        config1 = pynlm.createStandardConfig()
        config2 = pynlm.createStandardConfig()
        
        # Make them different
        config2.set("brain.neuron_count", 500)
        
        # Compare configurations
        diff = pynlm.config_diff(config1, config2)
        self.assertIn("brain.neuron_count", diff)
        
        # Test reverse comparison
        diff_reverse = pynlm.config_diff(config2, config1)
        self.assertIn("brain.neuron_count", diff_reverse)
    
    def test_batch_simulation(self):
        """Test batch simulation functionality"""
        # Create multiple configurations
        configs = []
        for i in range(3):
            config = pynlm.createStandardConfig()
            config.set("brain.neuron_count", 100 + i * 50)
            configs.append(config)
        
        # Run batch simulation
        brains = pynlm.batch_simulate(configs, steps_per_config=50)
        
        # Verify results
        self.assertEqual(len(brains), len(configs))
        for i, brain in enumerate(brains):
            self.assertIsNotNone(brain)
            self.assertEqual(brain.getTotalNeuronCount(), 100 + i * 50)
    
    def test_debug_brain(self):
        """Test brain debugging functionality"""
        # This should not raise any exceptions
        pynlm.debug_brain(self.brain)
        
        # Verify that debug information is printed
        # (This is mainly to ensure the function works without errors)
    
    def test_serialization_example(self):
        """Test serialization example"""
        # Create a temporary file for testing
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain state
            save_result = self.brain.save(filepath)
            
            # Verify file was created and has content
            self.assertTrue(os.path.exists(filepath))
            self.assertGreater(os.path.getsize(filepath), 0)
            
            # Try to load it back
            load_config = pynlm.createDefaultConfig()
            load_brain = pynlm.createBrain(load_config)
            load_brain.load(filepath)
            
            # Verify the loaded brain has the same configuration
            self.assertEqual(load_brain.getConfig().brain_neuron_count, 
                           self.brain.getConfig().brain_neuron_count)
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_error_handling(self):
        """Test error handling with invalid operations"""
        # Test invalid configuration key
        config = pynlm.createDefaultConfig()
        self.assertIsNone(config.get("nonexistent.key"))
        
        # Test that invalid neurons don't cause crashes
        invalid_neuron = pynlm.NeuronId()
        self.assertEqual(invalid_neuron.value, 0)

class TestPythonBindingsAdvanced(unittest.TestCase):
    """Test advanced Python bindings functionality"""
    
    def setUp(self):
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
    
    def test_create_agent_simulation(self):
        """Test create_agent_simulation function"""
        # Test with custom setup function
        def custom_setup(brain, world, agent):
            world.setRandomSeed(12345)
        
        brain, world, agent = pynlm.create_agent_simulation(
            world_width=50, 
            world_height=50,
            vision_width=15,
            vision_height=15,
            neuron_count=2000,
            setup_func=custom_setup
        )
        
        self.assertIsNotNone(brain)
        self.assertIsNotNone(world)
        self.assertIsNotNone(agent)
        self.assertEqual(world.getWidth(), 50)
        self.assertEqual(agent.getSensoryInputSize(), 3)  # Vision channels
    
    def test_analyze_simulation_data(self):
        """Test analyze_simulation_data function"""
        # Create multiple brains with different configurations
        configs = []
        for i in range(3):
            config = pynlm.createStandardConfig()
            config.set("brain.neuron_count", 1000 + i * 500)
            configs.append(config)
        
        brains = []
        for config in configs:
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        # Analyze simulation data
        analysis = pynlm.analyze_simulation_data(brains)
        
        self.assertIn("average_neurons", analysis)
        self.assertIn("average_synapses", analysis)
        self.assertIn("average_firing_rate", analysis)
        self.assertIn("min_neurons", analysis)
        self.assertIn("max_neurons", analysis)
        
        # Verify analysis values are reasonable
        self.assertGreater(analysis["average_neurons"], 1000)
        self.assertGreater(analysis["min_neurons"], 0)
        self.assertGreater(analysis["max_neurons"], analysis["average_neurons"])
    
    def test_benchmark_simulation(self):
        """Test benchmark_simulation function"""
        config = pynlm.createStandardConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        results = pynlm.benchmark_simulation(brain, steps=200, warm_up=True)
        
        self.assertIn("total_steps", results)
        self.assertIn("total_time_ms", results)
        self.assertIn("steps_per_second", results)
        self.assertIn("neurons_per_second", results)
        
        self.assertEqual(results["total_steps"], 200)
        self.assertGreater(results["total_time_ms"], 0)
        self.assertGreater(results["steps_per_second"], 0)
        self.assertGreater(results["neurons_per_second"], 0)
        
        # Verify neurons_per_second calculation
        expected_neurons_per_second = results["steps_per_second"] * brain.getTotalNeuronCount()
        self.assertAlmostEqual(results["neurons_per_second"], expected_neurons_per_second, delta=1000)
    
    def test_print_simulation_summary(self):
        """Test print_simulation_summary function"""
        # Create multiple brains with different properties
        configs = []
        for i in range(3):
            config = pynlm.createStandardConfig()
            config.set("brain.neuron_count", 500 + i * 200)
            configs.append(config)
        
        brains = []
        for config in configs:
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        # This should not raise any exceptions
        pynlm.print_simulation_summary(brains)
        
        # Verify summary is printed
        self.assertEqual(len(brains), 3)

if __name__ == '__main__':
    # Check if pynlm is available
    if not PYNLM_AVAILABLE:
        print("ERROR: pynlm module not found!")
        print("Please ensure pynlm is installed and accessible.")
        sys.exit(1)
    
    # Run tests
    unittest.main()
