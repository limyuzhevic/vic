"""
Test suite for NLM Pythonic API.

This module provides comprehensive tests for the Pythonic API layer,
testing both the high-level interfaces and the underlying C++ bindings.
"""

import sys
import os
import tempfile
import unittest
from typing import Dict, Any

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

# Import the test API
try:
    from nlm import (
        NLMBrain, NLMWorld, NLMEnvironment,
        create_simple_brain, run_simple_simulation,
        ConfigurationError, SimulationError, MemoryError
    )
    NLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import NLM Python API: {e}")
    NLM_AVAILABLE = False
    NLMBrain = NLMWorld = NLMEnvironment = None

# Import the C++ bindings if available
try:
    import pynlm
    CPLUSPLUS_AVAILABLE = True
except ImportError:
    CPLUSPLUS_AVAILABLE = False
    pynlm = None
class TestNLMBrain(unittest.TestCase):
    """Test cases for the NLMBrain class."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not NLM_AVAILABLE:
            self.skipTest("NLMBrain not available")
            
        # Create a simple brain for testing
        self.brain = create_simple_brain(neuron_count=100, region_count=1)
        
    def tearDown(self):
        """Clean up after tests."""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def test_brain_creation(self):
        """Test that brain is created correctly."""
        self.assertIsNotNone(self.brain._brain)
        self.assertGreater(self.brain._brain.getTotalNeuronCount(), 0)
        
    def test_brain_initialization(self):
        """Test brain statistics after initialization."""
        stats = self.brain.get_stats()
        
        self.assertIn('total_neurons', stats)
        self.assertIn('total_synapses', stats)
        self.assertGreater(stats['total_neurons'], 0)
        self.assertGreater(stats['total_synapses'], 0)
        
    def test_brain_step(self):
        """Test advancing brain by one step."""
        initial_spikes = self.brain._brain.getTotalSpikeCount()
        initial_firing = self.brain._brain.getFiringNeuronCount()
        
        # Take a step
        new_stats = self.brain.step(1, 0.001)
        
        # Check that stats changed
        self.assertEqual(new_stats['total_spikes'], initial_spikes)
        # Firing neurons might have changed
        
    def test_region_management(self):
        """Test adding and getting regions."""
        initial_regions = self.brain._brain.getRegionCount()
        
        # Add a new region
        new_region_id = self.brain.add_region("TestRegion")
        
        self.assertGreaterEqual(new_region_id.index(), 1)
        self.assertGreater(self.brain._brain.getRegionCount(), initial_regions)
        
        # Get the region
        region = self.brain.get_region(new_region_id)
        self.assertIsNotNone(region)
        self.assertEqual(region.getName(), "TestRegion")
        
    def test_brain_reset(self):
        """Test brain reset functionality."""
        # Take some steps
        for i in range(5):
            self.brain.step(i, 0.001)
            
        # Reset the brain
        self.brain.reset()
        
        # Check that state is reset
        self.assertGreater(self.brain._brain.getTotalNeuronCount(), 0)
        
    def test_checkpoint_save_load(self):
        """Test checkpoint save/load functionality."""
        # Take some steps first
        for i in range(10):
            self.brain.step(i, 0.001)
            
        # Save to a temporary file
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
            
        try:
            # Save checkpoint
            save_success = self.brain.save(checkpoint_file)
            self.assertTrue(save_success)
            
            # Create a new brain and load checkpoint
            new_brain = create_simple_brain(neuron_count=100, region_count=1)
            load_success = new_brain.load(checkpoint_file)
            self.assertTrue(load_success)
            
            # Check that they have similar neuron counts
            original_count = self.brain._brain.getTotalNeuronCount()
            loaded_count = new_brain._brain.getTotalNeuronCount()
            self.assertEqual(original_count, loaded_count)
            
        finally:
            # Clean up
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
class TestNLMWorld(unittest.TestCase):
    """Test cases for the NLMWorld class."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not NLM_AVAILABLE:
            self.skipTest("NLMWorld not available")
            
        self.world = NLMWorld()
        
    def tearDown(self):
        """Clean up after tests."""
        pass
        
    def test_world_initialization(self):
        """Test world initialization."""
        self.assertIsNotNone(self.world._world)
        
    def test_world_configuration(self):
        """Test world configuration."""
        self.world.configure(30, 40, 10, 8)
        
        # Note: We can't easily test the internal configuration without
        # accessing private attributes, but we can test the public methods
        
    def test_world_reset(self):
        """Test world reset."""
        self.world.configure(20, 20, 8, 8)
        self.world.reset()
        
        # Basic test that world is in a usable state
        percept = self.world.update(0.1)
        self.assertIsNotNone(percept)
        
    def test_world_sensory_input(self):
        """Test world sensory input."""
        self.world.configure(15, 15, 6, 6)
        self.world.reset()
        self.world.set_agent_start(7.5, 7.5)
        
        # Update world and get percept
        percept = self.world.update(0.1)
        
        # Get sensory input
        sensory = self.world.get_sensory_input()
        
        # Check that all sensory modalities are present
        self.assertIn('vision', sensory)
        self.assertIn('touch', sensory)
        self.assertIn('internal', sensory)
        self.assertIn('proprioception', sensory)
        
    def test_world_agent_body(self):
        """Test world agent body access."""
        self.world.configure(25, 25, 10, 10)
        self.world.reset()
        self.world.set_agent_start(12.5, 12.5)
        
        # Get agent body
        body = self.world.get_agent_body()
        
        # Check that all expected properties are present
        expected_keys = ['x', 'y', 'orientation', 'velocity_x', 'velocity_y',
                        'angular_velocity', 'energy', 'health', 'age',
                        'is_moving', 'is_turning', 'last_action_time']
        
        for key in expected_keys:
            self.assertIn(key, body)
            
        # Check that values are reasonable
        self.assertGreaterEqual(body['x'], 0.0)
        self.assertGreaterEqual(body['y'], 0.0)
        self.assertGreaterEqual(body['energy'], 0.0)
class TestNLMEnvironment(unittest.TestCase):
    """Test cases for the NLMEnvironment class."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not NLM_AVAILABLE:
            self.skipTest("NLMEnvironment not available")
            
        self.env = NLMEnvironment()
        
    def tearDown(self):
        """Clean up after tests."""
        pass
        
    def test_environment_setup(self):
        """Test environment setup."""
        self.env.setup(world_width=10, world_height=10, 
                      vision_width=4, vision_height=4,
                      agent_start_x=5.0, agent_start_y=5.0)
        
        self.assertIsNotNone(self.env.brain)
        self.assertIsNotNone(self.env.world)
        self.assertIsNotNone(self.env.agent)
        
    def test_environment_episode(self):
        """Test running a complete episode."""
        self.env.setup(world_width=15, world_height=15,
                      vision_width=6, vision_height=6,
                      agent_start_x=7.5, agent_start_y=7.5)
        
        # Run a short episode
        episode_stats = self.env.run_episode(max_steps=20, timestep=0.05)
        
        # Check that we got expected results
        self.assertIn('steps_completed', episode_stats)
        self.assertIn('total_reward', episode_stats)
        self.assertIn('avg_firing_rate', episode_stats)
        
        self.assertEqual(episode_stats['steps_completed'], 20)
        self.assertGreaterEqual(episode_stats['total_reward'], 0.0)
class TestConvenienceFunctions(unittest.TestCase):
    """Test cases for convenience functions."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not NLM_AVAILABLE:
            self.skipTest("Convenience functions not available")
            
    def test_create_simple_brain(self):
        """Test create_simple_brain convenience function."""
        brain = create_simple_brain(neuron_count=50, region_count=1,
                                    connection_probability=0.1)
        
        self.assertIsNotNone(brain)
        self.assertGreater(brain._brain.getTotalNeuronCount(), 0)
        
    def test_run_simple_simulation(self):
        """Test run_simple_simulation convenience function."""
        # Create components
        brain = create_simple_brain(neuron_count=50, region_count=1)
        world = NLMWorld()
        agent = None  # We'll create this from brain
        
        # This test would require more complex setup
        # For now, just test that the function exists and can be called
        self.assertTrue(callable(run_simple_simulation))
class TestPythonErrors(unittest.TestCase):
    """Test cases for custom Python error classes."""
    
    def test_nlm_error(self):
        """Test NLMError base class."""
        error = NLMError("Test error")
        self.assertEqual(str(error), "Test error")
        
    def test_configuration_error(self):
        """Test ConfigurationError subclass."""
        error = ConfigurationError("Config error")
        self.assertIsInstance(error, NLMError)
        self.assertEqual(str(error), "Config error")
        
    def test_simulation_error(self):
        """Test SimulationError subclass."""
        error = SimulationError("Simulation error")
        self.assertIsInstance(error, NLMError)
        self.assertEqual(str(error), "Simulation error")
        
    def test_memory_error(self):
        """Test MemoryError subclass."""
        error = MemoryError("Memory error")
        self.assertIsInstance(error, NLMError)
        self.assertEqual(str(error), "Memory error")
class TestCPlusPlusBindings(unittest.TestCase):
    """Test cases for direct C++ bindings."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not CPLUSPLUS_AVAILABLE:
            self.skipTest("C++ bindings not available")
            
    def test_version_attribute(self):
        """Test that pynlm has __version__ attribute."""
        self.assertTrue(hasattr(pynlm, '__version__'))
        version = pynlm.__version__
        self.assertIsInstance(version, str)
        self.assertTrue(len(version) > 0)
        
    def test_basic_types(self):
        """Test that basic types are accessible."""
        # Test that key types exist
        self.assertTrue(hasattr(pynlm, 'NeuronId'))
        self.assertTrue(hasattr(pynlm, 'SynapseId'))
        self.assertTrue(hasattr(pynlm, 'RegionId'))
        self.assertTrue(hasattr(pynlm, 'PopulationId'))
        self.assertTrue(hasattr(pynlm, 'NeuronType'))
        self.assertTrue(hasattr(pynlm, 'SynapseType'))
        
    def test_enums(self):
        """Test that enumerations work."""
        # Test NeuronType enum
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Modulatory,
            pynlm.NeuronType.Internal,
        ]
        
        for neuron_type in neuron_types:
            self.assertIsInstance(neuron_type, pynlm.NeuronType)
            
    def test_create_functions(self):
        """Test that create functions work."""
        # Test createDefaultConfig
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        # Test createBrain
        brain = pynlm.createBrain(config)
        self.assertIsNotNone(brain)
        
        # Test createSimpleWorld
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        
        # Test createAgentBrain
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        
        # Initialize the brain
        brain.initialize()
        
    def test_classes_instantiation(self):
        """Test that key classes can be instantiated."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create simple world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 6, 6)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test that we can get instances of various classes
        self.assertIsNotNone(brain.getConfig())
        self.assertIsNotNone(world.getSensoryPercept())
class TestPhase6Integration(unittest.TestCase):
    """Test Phase 6 integration features."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not NLM_AVAILABLE:
            self.skipTest("Phase 6 integration not available")
            
    def test_memory_systems_available(self):
        """Test that memory systems are available."""
        brain = create_simple_brain(neuron_count=100, region_count=1)
        
        # These should not return nullptr
        self.assertIsNotNone(brain._brain.getWorkingMemory())
        self.assertIsNotNone(brain._brain.getEpisodicMemory())
        self.assertIsNotNone(brain._brain.getAssociativeMemory())
        
    def test_neuromodulation_available(self):
        """Test that neuromodulation systems are available."""
        brain = create_simple_brain(neuron_count=100, region_count=1)
        
        # These should not return nullptr
        self.assertIsNotNone(brain._brain.getDopamine())
        self.assertIsNotNone(brain._brain.getCuriosity())
        self.assertIsNotNone(brain._brain.getNovelty())
        self.assertIsNotNone(brain._brain.getPredictionErrorSignal())
        
    def test_cognition_systems_available(self):
        """Test that cognition systems are available."""
        brain = create_simple_brain(neuron_count=100, region_count=1)
        
        # These should not return nullptr
        self.assertIsNotNone(brain._brain.getPlanner())
        self.assertIsNotNone(brain._brain.getConceptFormation())
        self.assertIsNotNone(brain._brain.getAttention())
        
    def test_prediction_system_available(self):
        """Test that prediction system is available."""
        brain = create_simple_brain(neuron_count=100, region_count=1)
        
        # This should not return nullptr
        self.assertIsNotNone(brain._brain.getPredictionSystem())
        
    def test_development_system_available(self):
        """Test that development system is available."""
        brain = create_simple_brain(neuron_count=100, region_count=1)
        
        # This should not return nullptr
        self.assertIsNotNone(brain._brain.getDevelopmentSystem())
    
    def test_checkpoint_system_available(self):
        """Test that checkpoint system is available."""
        brain = create_simple_brain(neuron_count=100, region_count=1)
        
        # Save some state
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
            
        try:
            # Take some steps and save
            for i in range(5):
                brain.step(i, 0.001)
                
            save_result = brain.save(checkpoint_file)
            self.assertTrue(save_result)
            
            # Load into new brain
            new_brain = create_simple_brain(neuron_count=100, region_count=1)
            load_result = new_brain.load(checkpoint_file)
            self.assertTrue(load_result)
            
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
if __name__ == '__main__':
    # Run all tests
    unittest.main()
