# Test Python bindings for NLM
import unittest
import sys
import os
import tempfile

# Add the Python module to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# We need to build the module first, but for testing we'll just test what we can
# Import the module after building
try:
    import pynlm as nlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False
    print("Warning: pynlm module not found. Skipping Python tests.")

class TestNLMConfiguration(unittest.TestCase):
    """Test NLM configuration functionality"""
    
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_config_creation(self):
        """Test creating default configuration"""
        config = nlm.create_config()
        self.assertIsNotNone(config)
        self.assertEqual(config.get_keys(), [])
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_config_summary(self):
        """Test getting configuration summary"""
        config = nlm.create_config()
        summary = config.summary()
        self.assertIsInstance(summary, str)
        self.assertIn("Config:", summary)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_config_clear(self):
        """Test clearing configuration"""
        config = nlm.create_config()
        config.clear()
        self.assertEqual(config.get_keys(), [])

class TestNLMBrain(unittest.TestCase):
    """Test NLM brain functionality"""
    
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def setUp(self):
        """Set up test fixtures"""
        self.config = nlm.create_config()
        self.brain = nlm.create_brain(self.config)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_brain_initialization(self):
        """Test brain initialization"""
        self.brain.initialize()
        # Should not raise exception
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_brain_reset(self):
        """Test brain reset"""
        self.brain.initialize()
        self.brain.reset()
        # Should not raise exception
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_brain_region_count(self):
        """Test getting region count"""
        region_count = self.brain.get_region_count()
        self.assertIsInstance(region_count, int)
        self.assertGreaterEqual(region_count, 0)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_brain_neuron_count(self):
        """Test getting neuron count"""
        neuron_count = self.brain.get_total_neuron_count()
        self.assertIsInstance(neuron_count, int)
        self.assertGreaterEqual(neuron_count, 0)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_brain_synapse_count(self):
        """Test getting synapse count"""
        synapse_count = self.brain.get_total_synapse_count()
        self.assertIsInstance(synapse_count, int)
        self.assertGreaterEqual(synapse_count, 0)

class TestNLMWorld(unittest.TestCase):
    """Test NLM world functionality"""
    
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def setUp(self):
        """Set up test fixtures"""
        self.world = nlm.create_world()
        self.world.configure(50, 50, 10, 10)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_world_creation(self):
        """Test world creation"""
        world = nlm.create_world()
        self.assertIsNotNone(world)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_world_configuration(self):
        """Test world configuration"""
        self.assertEqual(self.world.get_width(), 50)
        self.assertEqual(self.world.get_height(), 50)
        self.assertEqual(self.world.get_vision_width(), 10)
        self.assertEqual(self.world.get_vision_height(), 10)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_world_reset(self):
        """Test world reset"""
        self.world.reset()
        # Should not raise exception
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_world_position_validation(self):
        """Test position validation"""
        # Valid position
        self.assertTrue(self.world.is_valid_position(10, 10))
        # Invalid position (outside bounds)
        self.assertFalse(self.world.is_valid_position(60, 60))

class TestNLMAction(unittest.TestCase):
    """Test NLM action functionality"""
    
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_action_creation(self):
        """Test action creation"""
        action = nlm.Action()
        self.assertIsNotNone(action)
        
        # Test with type
        action_with_type = nlm.Action(nlm.ActionType.MoveForward)
        self.assertEqual(action_with_type.get_type(), nlm.ActionType.MoveForward)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_action_parameters(self):
        """Test action parameters"""
        action = nlm.Action(nlm.ActionType.Custom, [1.0, 2.0, 3.0])
        params = action.get_parameters()
        self.assertEqual(len(params), 3)
        self.assertEqual(params[0], 1.0)
        self.assertEqual(params[1], 2.0)
        self.assertEqual(params[2], 3.0)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_action_name(self):
        """Test action name"""
        action = nlm.Action(nlm.ActionType.MoveForward)
        name = action.get_name()
        self.assertIsInstance(name, str)
        self.assertIn("Move", name)

class TestNLMNeuronId(unittest.TestCase):
    """Test NLM NeuronId functionality"""
    
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_neuron_id_creation(self):
        """Test NeuronId creation"""
        neuron_id = nlm.NeuronId()
        self.assertIsNotNone(neuron_id)
        
        neuron_id_with_value = nlm.NeuronId(42)
        self.assertEqual(neuron_id_with_value.value, 42)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_neuron_id_operations(self):
        """Test NeuronId operations"""
        neuron_id1 = nlm.NeuronId(42)
        neuron_id2 = nlm.NeuronId(42)
        neuron_id3 = nlm.NeuronId(43)
        
        # Equality
        self.assertEqual(neuron_id1, neuron_id2)
        self.assertNotEqual(neuron_id1, neuron_id3)
        
        # Hash
        neuron_set = {neuron_id1, neuron_id2, neuron_id3}
        self.assertEqual(len(neuron_set), 2)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_neuron_id_index(self):
        """Test NeuronId index"""
        neuron_id = nlm.NeuronId(42)
        index = neuron_id.index()
        self.assertEqual(index, 42)

class TestNLMConvenienceFunctions(unittest.TestCase):
    """Test NLM convenience functions"""
    
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_create_default_config(self):
        """Test create_default_config"""
        config = nlm.create_config()
        self.assertIsNotNone(config)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_create_world(self):
        """Test create_world"""
        world = nlm.create_world()
        self.assertIsNotNone(world)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_create_brain_from_config(self):
        """Test create_brain_from_config"""
        config = nlm.create_config()
        brain = nlm.create_brain(config)
        self.assertIsNotNone(brain)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_create_advanced_agent(self):
        """Test create_advanced_agent"""
        config = nlm.create_config()
        world = nlm.create_world()
        world.configure(50, 50, 10, 10)
        
        agent = nlm.create_advanced_agent(config, world)
        self.assertIsNotNone(agent)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_create_training_environment(self):
        """Test create_training_environment"""
        world = nlm.create_training_environment(50, 50, 10, 10)
        self.assertIsNotNone(world)
        self.assertEqual(world.get_width(), 50)

class TestNLMDebugFunctions(unittest.TestCase):
    """Test NLM debug and utility functions"""
    
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_print_enum_values(self):
        """Test print_enum_values"""
        # This function prints to stdout, so we just verify it doesn't crash
        nlm.print_enum_values()
        # Should not raise exception
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_demonstrate_basic_usage(self):
        """Test demonstrate_basic_usage"""
        # This function prints to stdout, so we just verify it doesn't crash
        nlm.demonstrate_basic_usage()
        # Should not raise exception

class TestNLMExperimentalSetup(unittest.TestCase):
    """Test NLM experimental setup"""
    
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_create_experiment_setup(self):
        """Test create_experiment_setup"""
        config, world, agent = nlm.create_experiment_setup(50, 50, 10, 10)
        self.assertIsNotNone(config)
        self.assertIsNotNone(world)
        self.assertIsNotNone(agent)
        self.assertEqual(world.get_width(), 50)
        
    @unittest.skipIf(not HAS_PYNLM, "pynlm module not available")
    def test_quick_prototype(self):
        """Test quick_prototype"""
        agent, world = nlm.quick_prototype()
        self.assertIsNotNone(agent)
        self.assertIsNotNone(world)

if __name__ == '__main__':
    unittest.main()