#!/usr/bin/env python3
"""
Python Binding Integration Tests for NLM.

This test suite validates the Python bindings and the enhanced Python interface
for the Neural Learning Machine framework.
"""

import sys
import os
import tempfile
import json
import time
import unittest
from unittest.mock import Mock, patch
import numpy as np

# Add pynlm to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'pynlm'))

try:
    import pynlm
    from pynlm import Brain, AgentBrain, SimpleWorld, Config, Vision, Audio, InternalSignals, Action
    from pynlm.types import NeuronId, SynapseId, RegionId, PopulationId, NeuronType, ActionType
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: pynlm not available: {e}")
    PYNLM_AVAILABLE = False

# Mock for testing when actual bindings aren't available
class MockBrain:
    def __init__(self, config=None):
        self.config = config or {}
        self.total_neurons = 1000
        self.total_synapses = 5000
        self.total_spikes = 0
        self.active_neurons = 100
        self.firing_neurons = 50
        
    def initialize(self):
        return True
        
    def step(self, step):
        self.total_spikes += 10
        self.active_neurons = 50 + int(np.random.random() * 50)
        self.firing_neurons = 20 + int(np.random.random() * 30)
        
    def getTotalNeuronCount(self):
        return self.total_neurons
        
    def getTotalSynapseCount(self):
        return self.total_synapses
        
    def getTotalSpikeCount(self):
        return self.total_spikes
        
    def getActiveNeuronCount(self):
        return self.active_neurons
        
    def getFiringNeuronCount(self):
        return self.firing_neurons
        
    def getAverageFiringRate(self):
        return self.total_spikes / max(self.total_neurons, 1) * 1000

class MockAgentBrain:
    def __init__(self, brain):
        self.brain = brain
        self.development_stage = "Adult"
        self.neuromodulation_level = 0.5
        self.curiosity_level = 0.3
        self.novelty_level = 0.2
        self.prediction_error = 0.1
        
    def initialize(self, world):
        return True
        
    def getSensoryInputSize(self):
        return 64 * 64 * 3
        
    def getMotorOutputSize(self):
        return 20
        
    def processSensoryInput(self, percept):
        pass
        
    def decodeMotorCommand(self):
        return "MoveForward"
        
    def applyRewardModulation(self, reward, predicted_reward=None):
        self.neuromodulation_level = min(1.0, self.neuromodulation_level + reward * 0.1)
        
    def updateDevelopment(self, timestep):
        pass
        
    def getDevelopmentalStage(self):
        return self.development_stage
        
    def getNeuromodulationLevel(self):
        return self.neuromodulation_level
        
    def getCuriosityLevel(self):
        return self.curiosity_level
        
    def getNoveltyLevel(self):
        return self.novelty_level
        
    def getPredictionError(self):
        return self.prediction_error
        
    def reset(self):
        self.neuromodulation_level = 0.5
        self.prediction_error = 0.0

class MockWorld:
    def __init__(self):
        self.width = 100
        self.height = 100
        self.agent_x = 50
        self.agent_y = 50
        self.energy = 100
        
    def configure(self, width, height, vision_width, vision_height):
        self.width = width
        self.height = height
        
    def update(self, timestep):
        self.energy -= 1
        
    def getSensoryPercept(self):
        return MockVision()
        
    def getAgentBody(self):
        return MockAgentBody()
        
    def applyMotorCommand(self, cmd, current_time=None):
        if cmd == "MoveForward":
            self.agent_y = min(self.height - 1, self.agent_y + 1)
            
class MockVision:
    def __init__(self):
        self.width = 64
        self.height = 64
        self.channels = 3
        
    def getWidth(self):
        return self.width
        
    def getHeight(self):
        return self.height
        
    def getChannels(self):
        return self.channels
        
class MockAgentBody:
    def __init__(self):
        self.x = 50
        self.y = 50
        self.orientation = 0
        self.velocityX = 0
        self.velocityY = 0
        self.angularVelocity = 0
        self.energy = 100
        self.health = 100
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = time.time()
        
    def reset(self):
        self.energy = 100
        self.health = 100

class MockConfig:
    def __init__(self):
        self.values = {
            'random_seed': 42,
            'simulation_timestep': 0.001,
            'neuron_count': 1000,
            'region_count': 2,
        }
        
    def getOr(self, key, default):
        return self.values.get(key, default)
        
    def has(self, key):
        return key in self.values


class TestBasicImports(unittest.TestCase):
    """Test basic imports and availability."""
    
    def test_imports_available(self):
        """Test that all modules can be imported."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
            
        # Check main classes
        self.assertTrue(Brain is not None)
        self.assertTrue(AgentBrain is not None)
        self.assertTrue(SimpleWorld is not None)
        self.assertTrue(Config is not None)
        self.assertTrue(Vision is not None)
        self.assertTrue(Audio is not None)
        self.assertTrue(InternalSignals is not None)
        self.assertTrue(Action is not None)
        
        # Check type definitions
        self.assertTrue(NeuronId is not None)
        self.assertTrue(SynapseId is not None)
        self.assertTrue(RegionId is not None)
        self.assertTrue(PopulationId is not None)
        self.assertTrue(NeuronType is not None)
        self.assertTrue(ActionType is not None)
    
    def test_version(self):
        """Test version information."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
            
        self.assertTrue(hasattr(pynlm, '__version__'))
        self.assertTrue(isinstance(pynlm.__version__, str))


class TestBrainInterface(unittest.TestCase):
    """Test Brain interface functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
            
        self.config = Config()
        self.config.set('random_seed', 42)
        self.config.set('neuron_count', 100)
        
    @patch('pynlm.createBrain', return_value=MockBrain())
    def test_brain_initialization(self, mock_create):
        """Test brain initialization."""
        brain = Brain(self.config)
        self.assertTrue(brain.initialize())
        
    @patch('pynlm.createBrain', return_value=MockBrain())
    def test_brain_step(self, mock_create):
        """Test brain stepping."""
        brain = Brain(self.config)
        brain.initialize()
        brain.step(0)
        brain.step(1)
        
        self.assertEqual(brain.total_spike_count, 20)
        
    @patch('pynlm.createBrain', return_value=MockBrain())
    def test_brain_properties(self, mock_create):
        """Test brain property access."""
        brain = Brain(self.config)
        mock_brain = MockBrain()
        mock_create.return_value = mock_brain
        brain.initialize()
        
        self.assertEqual(brain.total_neuron_count, 1000)
        self.assertEqual(brain.total_synapse_count, 5000)
        self.assertEqual(brain.total_spike_count, 0)
        
    @patch('pynlm.createBrain', return_value=MockBrain())
    def test_brain_population_summary(self, mock_create):
        """Test brain population summary."""
        brain = Brain(self.config)
        mock_brain = MockBrain()
        mock_create.return_value = mock_brain
        brain.initialize()
        
        summary = brain.get_neuron_population_summary()
        self.assertIn('total_neurons', summary)
        self.assertIn('active_neurons', summary)
        self.assertIn('firing_neurons', summary)
        
    @patch('pynlm.createBrain', return_value=MockBrain())
    def test_brain_simulation_episode(self, mock_create):
        """Test brain simulation episode."""
        brain = Brain(self.config)
        mock_brain = MockBrain()
        mock_create.return_value = mock_brain
        brain.initialize()
        
        results = brain.simulate_episode(5)
        self.assertEqual(len(results), 5)
        
        for result in results:
            self.assertIn('step', result)
            self.assertIn('neuron_count', result)
            self.assertIn('action', result)


class TestAgentBrainInterface(unittest.TestCase):
    """Test AgentBrain interface functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
            
        self.config = Config()
        
    @patch('pynlm.createBrain')
    @patch('pynlm.createAgentBrain', return_value=MockAgentBrain(None))
    def test_agent_brain_initialization(self, mock_create_agent, mock_create_brain):
        """Test agent brain initialization."""
        mock_brain = MockBrain()
        mock_create_brain.return_value = mock_brain
        
        agent_brain = AgentBrain(mock_brain)
        world = MockWorld()
        
        self.assertTrue(agent_brain.initialize(world))
        
    @patch('pynlm.createBrain')
    @patch('pynlm.createAgentBrain', return_value=MockAgentBrain(None))
    def test_agent_brain_actions(self, mock_create_agent, mock_create_brain):
        """Test agent brain actions."""
        mock_brain = MockBrain()
        mock_create_brain.return_value = mock_brain
        
        agent_brain = AgentBrain(mock_brain)
        world = MockWorld()
        agent_brain.initialize(world)
        
        # Test status
        status = agent_brain.get_agent_status()
        self.assertIn('developmental_stage', status)
        self.assertIn('neuromodulation_level', status)
        self.assertIn('brain_total_neurons', status)
        
        # Test system enable/disable
        agent_brain.enable_systems(reward_modulation=False)
        systems = agent_brain.get_enabled_systems()
        self.assertFalse(systems['reward_modulation'])
        
    @patch('pynlm.createBrain')
    @patch('pynlm.createAgentBrain', return_value=MockAgentBrain(None))
    def test_agent_brain_act(self, mock_create_agent, mock_create_brain):
        """Test agent brain act method."""
        mock_brain = MockBrain()
        mock_create_brain.return_value = mock_brain
        
        agent_brain = AgentBrain(mock_brain)
        world = MockWorld()
        agent_brain.initialize(world)
        
        percept = MockVision()
        action = agent_brain.act(percept)
        
        self.assertIsInstance(action, str)


class TestWorldInterface(unittest.TestCase):
    """Test World interface functionality."""
    
    @patch('pynlm.createSimpleWorld', return_value=MockWorld())
    def test_world_initialization(self, mock_create):
        """Test world initialization."""
        world = SimpleWorld()
        
        self.assertIsNotNone(world)
        self.assertEqual(world.width, 100)
        self.assertEqual(world.height, 100)
        
    @patch('pynlm.createSimpleWorld', return_value=MockWorld())
    def test_world_configuration(self, mock_create):
        """Test world configuration."""
        world = SimpleWorld()
        world.configure(200, 150, 32, 32)
        
        self.assertEqual(world.width, 200)
        self.assertEqual(world.height, 150)
        
    @patch('pynlm.createSimpleWorld', return_value=MockWorld())
    def test_world_properties(self, mock_create):
        """Test world properties."""
        world = SimpleWorld()
        
        self.assertTrue(world.width > 0)
        self.assertTrue(world.height > 0)
        self.assertTrue(world.max_energy > 0)
        self.assertTrue(world.energy_decay_rate > 0)
        
    @patch('pynlm.createSimpleWorld', return_value=MockWorld())
    def test_world_step(self, mock_create):
        """Test world stepping."""
        world = SimpleWorld()
        world.update(0.001)
        
    @patch('pynlm.createSimpleWorld', return_value=MockWorld())
    def test_world_simulation_step(self, mock_create):
        """Test world simulation step."""
        world = SimpleWorld()
        action = "MoveForward"
        
        result = world.simulate_step(action, 0.001)
        
        self.assertIn('timestep', result)
        self.assertIn('action', result)
        self.assertIn('percept_before', result)
        self.assertIn('percept_after', result)
        self.assertIn('agent', result)


class TestVisionInterface(unittest.TestCase):
    """Test Vision interface functionality."""
    
    def test_vision_creation(self):
        """Test vision creation."""
        vision = Vision(64, 64, 3)
        
        self.assertEqual(vision.width, 64)
        self.assertEqual(vision.height, 64)
        self.assertEqual(vision.channels, 3)
        
    def test_vision_set_data(self):
        """Test vision data setting."""
        vision = Vision(32, 32, 1)
        data = np.random.rand(32, 32)
        
        vision.set_data(data)
        
        retrieved_data = vision.get_data()
        self.assertEqual(len(retrieved_data), 32 * 32)
        
    def test_vision_features(self):
        """Test vision feature extraction."""
        vision = Vision(64, 64, 3)
        data = np.random.rand(64, 64, 3)
        vision.set_data(data)
        
        features = vision.get_features()
        
        self.assertIn('mean', features)
        self.assertIn('std', features)
        self.assertIn('min', features)
        self.assertIn('max', features)
        self.assertIn('edges_count', features)
        
    def test_vision_edge_detection(self):
        """Test vision edge detection."""
        vision = Vision(64, 64, 1)
        
        # Create simple test image with edges
        data = np.zeros((64, 64, 1))
        data[20:40, 20:40] = 1.0
        vision.set_data(data)
        
        edges = vision.get_edges(threshold=0.5)
        
        self.assertIsInstance(edges, np.ndarray)
        self.assertEqual(edges.shape, (64, 64))


class TestAudioInterface(unittest.TestCase):
    """Test Audio interface functionality."""
    
    def test_audio_creation(self):
        """Test audio creation."""
        audio = Audio(44100, 8192)
        
        self.assertEqual(audio.sample_rate, 44100)
        self.assertEqual(audio.num_samples, 8192)
        
    def test_audio_features(self):
        """Test audio feature extraction."""
        audio = Audio(16000, 4096)
        
        # Generate some test audio
        t = np.linspace(0, 1, 4096)
        data = 0.5 * np.sin(2 * np.pi * 440 * t)  # A4 note
        audio.set_data(data)
        
        features = audio.compute_features()
        
        self.assertIn('mean', features)
        self.assertIn('std', features)
        self.assertIn('duration', features)
        self.assertIn('pitch', features)
        
        # Should detect pitch around 440 Hz
        self.assertGreater(features['pitch'], 400)
        self.assertLess(features['pitch'], 500)


class TestActionInterface(unittest.TestCase):
    """Test Action interface functionality."""
    
    def test_action_creation(self):
        """Test action creation."""
        action = Action()
        self.assertIsNotNone(action)
        
    def test_action_properties(self):
        """Test action properties."""
        action = Action()
        
        # Test default type
        action_type = action.get_type()
        self.assertIsNotNone(action_type)
        
        # Test parameters
        params = action.get_parameters()
        self.assertIsInstance(params, dict)
        
    def test_action_modification(self):
        """Test action modification."""
        action = Action()
        
        # Test parameter setting
        action.set_parameters({'duration': 2.0, 'speed': 0.5})
        params = action.get_parameters()
        self.assertEqual(params.get('duration'), 2.0)
        self.assertEqual(params.get('speed'), 0.5)
        
    def test_action_cloning(self):
        """Test action cloning."""
        action = Action()
        action.set_parameters({'test': 'value'})
        
        cloned = action.clone()
        
        self.assertEqual(action.get_type(), cloned.get_type())
        self.assertEqual(action.get_parameters(), cloned.get_parameters())
        
    def test_action_factory_functions(self):
        """Test action factory functions."""
        from pynlm import create_movement_action, create_look_action, create_interaction_action
        
        # Test movement action
        move_action = create_movement_action('forward', 0.5, 1.0)
        self.assertIsNotNone(move_action)
        
        # Test look action
        look_action = create_look_action('up', 0.8)
        self.assertIsNotNone(look_action)
        
        # Test interaction action
        interact_action = create_interaction_action('interact', {'target': 'object'})
        self.assertIsNotNone(interact_action)


class TestTypeDefinitions(unittest.TestCase):
    """Test type definitions."""
    
    def test_id_types(self):
        """Test ID type definitions."""
        neuron_id = NeuronId(42)
        self.assertEqual(neuron_id.value, 42)
        self.assertEqual(neuron_id.index(), 42)
        
        synapse_id = SynapseId(100)
        self.assertEqual(synapse_id.value, 100)
        
        region_id = RegionId(5)
        self.assertEqual(region_id.value, 5)
        
        population_id = PopulationId(1)
        self.assertEqual(population_id.value, 1)
        
    def test_enum_types(self):
        """Test enumeration types."""
        # Test NeuronType
        self.assertEqual(NeuronType.Excitatory.value, 0)
        self.assertEqual(NeuronType.Sensory.value, 3)
        
        # Test ActionType
        self.assertEqual(ActionType.MoveForward.value, 0)
        self.assertEqual(ActionType.Look.value, 6)
        
    def test_type_operations(self):
        """Test type operations."""
        id1 = NeuronId(1)
        id2 = NeuronId(1)
        id3 = NeuronId(2)
        
        self.assertEqual(id1, id2)
        self.assertNotEqual(id1, id3)
        
        # Test hash
        ids = {id1, id2, id3}
        self.assertEqual(len(ids), 2)  # id1 and id2 are equal


class TestConfigInterface(unittest.TestCase):
    """Test configuration interface."""
    
    def setUp(self):
        """Set up test fixtures."""
        # Skip if pynlm not available
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm not available")
            
    def test_config_creation(self):
        """Test config creation."""
        config = Config()
        
        self.assertIsNotNone(config)
        self.assertTrue(config.has('random_seed'))
        self.assertTrue(config.has('simulation_timestep'))
        
    def test_config_getters(self):
        """Test config getters."""
        config = Config()
        
        random_seed = config.get_int('random_seed', 42)
        self.assertEqual(random_seed, 42)
        
        timestep = config.get_float('simulation_timestep', 0.001)
        self.assertEqual(timestep, 0.001)
        
    def test_config_setters(self):
        """Test config setters."""
        config = Config()
        
        config.set('test_key', 123)
        self.assertEqual(config.get('test_key'), 123)
        
        config.set('test_bool', True)
        self.assertTrue(config.get_bool('test_bool'))
        
    def test_config_profiles(self):
        """Test config profiles."""
        config = Config()
        
        # Test default profile
        self.assertTrue(config.apply_profile('default'))
        self.assertTrue(config.has('random_seed'))
        
        # Test fast profile
        config.apply_profile('fast')
        self.assertEqual(config.get_int('neuron_count', 0), 500)
        
        # Test detailed profile
        config.apply_profile('detailed')
        self.assertEqual(config.get_float('simulation_timestep', 0), 0.0001)
        
    def test_config_validation(self):
        """Test config validation."""
        config = Config()
        
        # Valid config should have no errors
        errors = config.validate()
        self.assertEqual(len(errors), 0)
        
        # Config with invalid timestep
        config.set('simulation_timestep', -1.0)
        errors = config.validate()
        self.assertGreater(len(errors), 0)
        
    def test_config_sections(self):
        """Test config sections."""
        config = Config()
        
        # Set some config values
        config.set('brain_neuron_count', 100)
        config.set('brain_region_count', 5)
        config.set('plasticity_learning_rate', 0.01)
        
        # Get brain section
        brain_section = config.get_section('brain')
        self.assertIn('neuron_count', brain_section)
        self.assertIn('region_count', brain_section)
        self.assertEqual(brain_section['neuron_count'], 100)
        
    def test_config_cloning(self):
        """Test config cloning."""
        config = Config()
        config.set('test_key', 'test_value')
        
        cloned = config.clone()
        
        self.assertEqual(config.get('test_key'), cloned.get('test_key'))
        
    def test_config_merging(self):
        """Test config merging."""
        config1 = Config()
        config1.set('key1', 'value1')
        config1.set('key2', 'value2')
        
        config2 = Config()
        config2.set('key2', 'new_value2')  # Should override
        config2.set('key3', 'value3')      # Should add
        
        config1.merge(config2, override=True)
        
        self.assertEqual(config1.get('key1'), 'value1')
        self.assertEqual(config1.get('key2'), 'new_value2')
        self.assertEqual(config1.get('key3'), 'value3')


class TestPerformance(unittest.TestCase):
    """Test performance characteristics."""
    
    @unittest.skipIf(not PYNLM_AVAILABLE, "pynlm not available")
    def test_brain_performance(self):
        """Test brain performance."""
        config = Config()
        
        # Create brain
        start_time = time.time()
        brain = Brain(config)
        brain.initialize()
        init_time = time.time() - start_time
        
        # Test multiple steps
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        step_time = time.time() - start_time
        
        print(f"Brain initialization: {init_time:.3f}s")
        print(f"100 steps: {step_time:.3f}s (avg: {step_time/100:.3f}s per step)")
        
        # Should be able to handle reasonable performance
        self.assertLess(step_time, 10.0)  # 10 seconds for 100 steps
        
    @unittest.skipIf(not PYNLM_AVAILABLE, "pynlm not available")
    def test_config_performance(self):
        """Test configuration performance."""
        config = Config()
        
        # Test many get/set operations
        start_time = time.time()
        for i in range(1000):
            config.set(f'key_{i}', i)
            _ = config.get(f'key_{i}')
        
        access_time = time.time() - start_time
        print(f"1000 get/set operations: {access_time:.3f}s")
        
        # Should be reasonably fast
        self.assertLess(access_time, 5.0)  # 5 seconds for 1000 operations


class TestErrorHandling(unittest.TestCase):
    """Test error handling."""
    
    def test_vision_error_handling(self):
        """Test vision error handling."""
        vision = Vision(32, 32, 1)
        
        # Test invalid data type
        with self.assertRaises(TypeError):
            vision.set_data("invalid_data")
            
    def test_action_error_handling(self):
        """Test action error handling."""
        action = Action()
        
        # Test invalid parameter type (should handle gracefully)
        action.set_parameters({'valid_param': 1.0})
        
    def test_config_error_handling(self):
        """Test configuration error handling."""
        config = Config()
        
        # Test saving to invalid path
        result = config.save_to_file('/invalid/path/file.cfg')
        self.assertFalse(result)
        
    def test_import_error_handling(self):
        """Test import error handling."""
        # Test that missing pynlm doesn't crash
        if not PYNLM_AVAILABLE:
            # Should still be able to create mock objects for testing
            mock_config = MockConfig()
            self.assertIsNotNone(mock_config)


def run_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Binding Integration Tests")
    print("=" * 60)
    
    # Create test suite
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    # Add test classes
    test_classes = [
        TestBasicImports,
        TestBrainInterface,
        TestAgentBrainInterface,
        TestWorldInterface,
        TestVisionInterface,
        TestAudioInterface,
        TestActionInterface,
        TestTypeDefinitions,
        TestConfigInterface,
        TestPerformance,
        TestErrorHandling,
    ]
    
    for test_class in test_classes:
        tests = loader.loadTestsFromTestCase(test_class)
        suite.addTests(tests)
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Success rate: {(result.testsRun - len(result.failures) - len(result.errors)) / max(result.testsRun, 1) * 100:.1f}%")
    
    if result.failures:
        print("\nFailures:")
        for test, traceback in result.failures:
            print(f"  {test}: {traceback}")
    
    if result.errors:
        print("\nErrors:")
        for test, traceback in result.errors:
            print(f"  {test}: {traceback}")
    
    return result.wasSuccessful()


if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)