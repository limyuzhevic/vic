"""
Test suite for NLM Python bindings

Comprehensive tests for the improved NLM Python API, including unit tests
and integration tests for all core functionality.
"""

import unittest
import tempfile
import os
import json
from unittest.mock import patch, MagicMock
from nlm import (
    Config, Brain, AgentBrain, SimpleWorld, Vision, Audio, InternalSignals,
    SensoryPercept, Action, ActionResult, ActionType, Reward,
    NeuronId, SynapseId, RegionId, PopulationId,
    create_default_config, create_brain, create_simple_world,
    create_agent_brain, save_brain_checkpoint, load_brain_checkpoint,
    get_brain_statistics, SimulationLoop, ActionSequence,
    parse_action_string, format_action
)
class TestTypeDefinitions(unittest.TestCase):
    """Test type definitions and data structures"""
    
    def test_neuron_id(self):
        """Test NeuronId type"""
        id1 = NeuronId(1)
        id2 = NeuronId()
        
        self.assertEqual(id1.value, 1)
        self.assertEqual(id2.value, 0)
        self.assertEqual(id1.index(), 1)
        self.assertEqual(id2.index(), 0)
        self.assertNotEqual(id1, id2)
        self.assertEqual(id1, NeuronId(1))
        self.assertIn(id1, {id1, id2})
        self.assertTrue(repr(id1).startswith("<NeuronId:"))
    
    def test_synapse_id(self):
        """Test SynapseId type"""
        id1 = SynapseId(42)
        id2 = SynapseId()
        
        self.assertEqual(id1.value, 42)
        self.assertEqual(id1.index(), 42)
        self.assertEqual(id1, SynapseId(42))
        self.assertNotEqual(id1, SynapseId(43))
        self.assertTrue(repr(id1).startswith("<SynapseId:"))
    
    def test_region_id(self):
        """Test RegionId type"""
        id1 = RegionId(100)
        id2 = RegionId()
        
        self.assertEqual(id1.value, 100)
        self.assertEqual(id1.index(), 100)
        self.assertEqual(id1, RegionId(100))
    
    def test_population_id(self):
        """Test PopulationId type"""
        id1 = PopulationId(999)
        id2 = PopulationId()
        
        self.assertEqual(id1.value, 999)
        self.assertEqual(id1.index(), 999)
        self.assertEqual(id1, PopulationId(999))
        self.assertIn(id1, {id1, id2})


class TestEnumTypes(unittest.TestCase):
    """Test enumeration types"""
    
    def test_neuron_type(self):
        """Test NeuronType enum"""
        from nlm import NeuronType
        
        self.assertEqual(NeuronType.Excitatory.value, 0)
        self.assertEqual(NeuronType.Inhibitory.value, 1)
        self.assertEqual(NeuronType.Modulatory.value, 2)
        self.assertEqual(NeuronType.Sensory.value, 3)
        self.assertEqual(NeuronType.Motor.value, 4)
        self.assertEqual(NeuronType.Internal.value, 5)
        
        self.assertEqual(NeuronType.Excitatory.name, "Excitatory")
        self.assertEqual(NeuronType.Inhibitory.name, "Inhibitory")
    
    def test_synapse_type(self):
        """Test SynapseType enum"""
        from nlm import SynapseType
        
        self.assertEqual(SynapseType.Excitatory.value, 0)
        self.assertEqual(SynapseType.Inhibitory.value, 1)
        self.assertEqual(SynapseType.Electrical.value, 3)
        self.assertEqual(SynapseType.GapJunction.value, 4)
    
    def test_action_type(self):
        """Test ActionType enum"""
        from nlm import ActionType
        
        action_types = [
            ActionType.MoveForward, ActionType.MoveBackward,
            ActionType.MoveLeft, ActionType.MoveRight,
            ActionType.TurnLeft, ActionType.TurnRight,
            ActionType.Interact, ActionType.Eat, ActionType.Drink,
            ActionType.Rest, ActionType.Wait, ActionType.Custom
        ]
        
        for action_type in action_types:
            self.assertIsInstance(action_type, ActionType)
            self.assertTrue(action_type.name.startswith('Action'))


class TestCoreClasses(unittest.TestCase):
    """Test core NLM classes"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = Config()
        self.brain = Brain(self.config)
        self.world = SimpleWorld()
        self.world.configure(640, 480, 320, 240)
    
    def test_config(self):
        """Test Config class"""
        # Test basic operations
        self.assertFalse(self.config.has("nonexistent_key"))
        
        # Test methods exist
        self.assertTrue(hasattr(self.config, "load_from_file"))
        self.assertTrue(hasattr(self.config, "save_to_file"))
        self.assertTrue(hasattr(self.config, "summary"))
        
        # Test string representation
        repr_str = repr(self.config)
        self.assertTrue(isinstance(repr_str, str))
        self.assertTrue(repr_str.startswith("<Config:"))
    
    def test_brain(self):
        """Test Brain class"""
        # Test initialization
        self.assertTrue(hasattr(self.brain, "initialize"))
        self.assertTrue(hasattr(self.brain, "step"))
        self.assertTrue(hasattr(self.brain, "produce_action"))
        
        # Test that methods exist
        self.assertTrue(hasattr(self.brain, "get_total_neuron_count"))
        self.assertTrue(hasattr(self.brain, "get_total_synapse_count"))
        self.assertTrue(hasattr(self.brain, "get_average_firing_rate"))
    
    def test_simple_world(self):
        """Test SimpleWorld class"""
        # Test configuration
        self.assertEqual(self.world.get_width(), 640)
        self.assertEqual(self.world.get_height(), 480)
        
        # Test methods exist
        self.assertTrue(hasattr(self.world, "set_agent_start"))
        self.assertTrue(hasattr(self.world, "update"))
        self.assertTrue(hasattr(self.world, "get_sensory_percept"))
        self.assertTrue(hasattr(self.world, "get_agent_body"))


class TestSensoryClasses(unittest.TestCase):
    """Test sensory input classes"""
    
    def test_vision(self):
        """Test Vision class"""
        vision = Vision(100, 200, 3)
        
        self.assertEqual(vision.getWidth(), 100)
        self.assertEqual(vision.getHeight(), 200)
        self.assertEqual(vision.getChannels(), 3)
        
        # Test data handling
        test_data = [0.5] * 100 * 200 * 3
        vision.setData(test_data)
        
        self.assertEqual(vision.getData(), test_data)
        self.assertEqual(vision.getDimensions(), [100, 200, 3])
        
        self.assertTrue(repr(vision).startswith("<Vision:"))
    
    def test_audio(self):
        """Test Audio class"""
        audio = Audio(44100, 1000)
        
        self.assertEqual(audio.getSampleRate(), 44100)
        self.assertEqual(audio.getNumSamples(), 1000)
        
        # Test data handling
        test_data = [i * 0.1 for i in range(1000)]
        audio.setData(test_data)
        
        self.assertEqual(audio.getData(), test_data)
        self.assertEqual(audio.getDimensions(), [1000])
        
        self.assertTrue(repr(audio).startswith("<Audio:"))
    
    def test_internal_signals(self):
        """Test InternalSignals class"""
        signals = InternalSignals()
        
        # Test signal operations
        signals.addSignal(1.5)
        signals.addSignal(-0.5)
        signals.addSignal(0.0)
        
        data = signals.getData()
        self.assertEqual(len(data), 3)
        self.assertEqual(data[0], 1.5)
        self.assertEqual(data[1], -0.5)
        self.assertEqual(data[2], 0.0)
        
        # Test clear
        signals.clearSignals()
        self.assertEqual(len(signals.getData()), 0)
    
    def test_sensory_percept(self):
        """Test SensoryPercept class"""
        percept = SensoryPercept()
        
        # Test all sensory inputs
        vision = Vision(10, 10, 3)
        audio = Audio(44100, 500)
        touch = InternalSignals()
        internal = InternalSignals()
        proprioception = InternalSignals()
        
        percept.setVision(vision)
        percept.setAudio(audio)
        percept.setTouch(touch)
        percept.setInternal(internal)
        percept.setProprioception(proprioception)
        
        # Test getters
        self.assertEqual(percept.getVisionWidth(), 10)
        self.assertEqual(percept.getVisionHeight(), 10)
        self.assertEqual(percept.getAudio(), audio)
        self.assertEqual(percept.getTouch(), touch)
        self.assertEqual(percept.getInternal(), internal)
        
        # Test all signals
        all_signals = percept.getAllSignals()
        self.assertIn('vision', all_signals)
        self.assertIn('audio', all_signals)
        self.assertIn('touch', all_signals)
        
        self.assertTrue(repr(percept).startswith("<SensoryPercept:"))


class TestMotorClasses(unittest.TestCase):
    """Test motor control classes"""
    
    def test_action(self):
        """Test Action class"""
        # Test creation with default
        action1 = Action()
        self.assertEqual(action1.getType(), ActionType.Wait)
        self.assertEqual(action1.getParameters(), [])
        self.assertEqual(action1.getName(), "Wait")
        
        # Test creation with type and parameters
        action2 = Action(ActionType.MoveForward, [1.0, 0.5])
        self.assertEqual(action2.getType(), ActionType.MoveForward)
        self.assertEqual(action2.getParameters(), [1.0, 0.5])
        self.assertEqual(action2.getName(), "Move Forward")
        
        # Test cloning
        action3 = action2.clone()
        self.assertEqual(action3.getType(), action2.getType())
        self.assertEqual(action3.getParameters(), action2.getParameters())
        self.assertIsNot(action3, action2)
        
        # Test string representation
        self.assertTrue(repr(action1).startswith("<Action: Wait"))
        self.assertTrue(repr(action2).startswith("<Action: Move Forward"))
    
    def test_action_result(self):
        """Test ActionResult class"""
        result1 = ActionResult()
        self.assertEqual(result1.getReward(), 0.0)
        self.assertFalse(result1.getSuccess())
        self.assertEqual(result1.getMessage(), "")
        
        result2 = ActionResult(5.0, True, "Success!")
        self.assertEqual(result2.getReward(), 5.0)
        self.assertTrue(result2.getSuccess())
        self.assertEqual(result2.getMessage(), "Success!")
        
        self.assertTrue(repr(result2).startswith("<ActionResult: SUCCESS"))


class TestAgentClasses(unittest.TestCase):
    """Test agent-related classes"""
    
    def test_agent_body(self):
        """Test AgentBody class"""
        body = AgentBody()
        
        # Test default values
        self.assertEqual(body.x, 0.0)
        self.assertEqual(body.y, 0.0)
        self.assertEqual(body.orientation, 0.0)
        self.assertEqual(body.energy, 100.0)
        self.assertEqual(body.health, 100.0)
        self.assertEqual(body.age, 0)
        
        # Test methods
        pos = body.getPosition()
        self.assertEqual(pos, (0.0, 0.0))
        
        body.setPosition(10.0, 20.0)
        self.assertEqual(body.x, 10.0)
        self.assertEqual(body.y, 20.0)
        
        # Test string representation
        self.assertTrue(repr(body).startswith("<AgentBody: pos="))
        
        # Test distance calculation
        body2 = AgentBody()
        body2.setPosition(3.0, 4.0)
        distance = body.getDistanceTo(body2)
        self.assertAlmostEqual(distance, 5.0, places=6)


class TestUtilityFunctions(unittest.TestCase):
    """Test utility functions and helper classes"""
    
    def test_create_default_config(self):
        """Test create_default_config function"""
        config = create_default_config()
        self.assertIsInstance(config, Config)
        self.assertTrue(hasattr(config, "get_keys"))
    
    def test_create_brain(self):
        """Test create_brain function"""
        config = create_default_config()
        brain = create_brain(config)
        self.assertIsInstance(brain, Brain)
        self.assertEqual(brain._config, config)
    
    def test_create_simple_world(self):
        """Test create_simple_world function"""
        world = create_simple_world()
        self.assertIsInstance(world, SimpleWorld)
    
    def test_create_agent_brain(self):
        """Test create_agent_brain function"""
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        agent_brain = create_agent_brain(brain, world)
        self.assertIsInstance(agent_brain, AgentBrain)
    
    def test_action_sequence(self):
        """Test ActionSequence class"""
        sequence = ActionSequence()
        
        # Test adding actions
        action1 = Action(ActionType.MoveForward)
        action2 = Action(ActionType.TurnLeft, [0.5])
        
        sequence.add_action(action1)
        sequence.add_action(action2)
        
        self.assertEqual(len(sequence.actions), 2)
        
        # Test action types extraction
        action_types = sequence.get_action_types()
        self.assertEqual(action_types[0], ActionType.MoveForward)
        self.assertEqual(action_types[1], ActionType.TurnLeft)
        
        # Test action names extraction
        action_names = sequence.get_action_names()
        self.assertEqual(action_names[0], "Move Forward")
        self.assertEqual(action_names[1], "Turn Left")
        
        # Test parameter summary
        param_summary = sequence.get_parameter_summary()
        self.assertEqual(param_summary[0], [])
        self.assertEqual(param_summary[1], [0.5])
        
        # Test pattern analysis
        analysis = sequence.analyze_patterns()
        self.assertEqual(analysis['total_actions'], 2)
        self.assertEqual(analysis['unique_actions'], 2)
        self.assertEqual(analysis['most_common_action'], 'Turn Left')
        self.assertEqual(analysis['most_common_count'], 1)
    
    def test_parse_action_string(self):
        """Test parse_action_string function"""
        # Test various action strings
        action1 = parse_action_string("move forward")
        self.assertEqual(action1.getType(), ActionType.MoveForward)
        
        action2 = parse_action_string("turn left")
        self.assertEqual(action2.getType(), ActionType.TurnLeft)
        
        action3 = parse_action_string("interact")
        self.assertEqual(action3.getType(), ActionType.Interact)
        
        # Test default action
        action4 = parse_action_string("unknown_command")
        self.assertEqual(action4.getType(), ActionType.Wait)
        
        # Test format_action
        action_str = format_action(action1)
        self.assertIn("Move Forward", action_str)
        
        action_str2 = format_action(action2)
        self.assertIn("Turn Left", action_str2)
    
    def test_format_duration(self):
        """Test format_duration function from utils"""
        # Import here to avoid import issues
        from nlm.utils import format_duration
        
        self.assertEqual(format_duration(0.0005), "0.000s")
        self.assertEqual(format_duration(0.5), "0.5s")
        self.assertEqual(format_duration(30.0), "30.0s")
        self.assertEqual(format_duration(90.0), "1.5m")
        self.assertEqual(format_duration(3600.0), "1.0h")
    
    def test_benchmark_function(self):
        """Test benchmark_function from utils"""
        # Import here to avoid import issues
        from nlm.utils import benchmark_function
        
        def dummy_function(x):
            return x * 2
        
        result = benchmark_function(dummy_function, 5)
        self.assertEqual(result['result'], 10)
        self.assertGreater(result['duration'], 0)
        self.assertIn('success', result)


class TestCheckpointing(unittest.TestCase):
    """Test checkpointing functionality"""
    
    def test_save_load_checkpoint(self):
        """Test save and load checkpoint functionality"""
        # Create brain and run some simulation
        config = create_default_config()
        brain = Brain(config)
        world = SimpleWorld()
        world.configure(640, 480, 320, 240)
        brain.initialize()
        
        # Run some steps
        loop = SimulationLoop(brain, world)
        loop.run_episode(5)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            checkpoint_file = f.name
        
        try:
            metadata = {'test': 'value', 'steps': 5}
            save_success = save_brain_checkpoint(brain, checkpoint_file, metadata)
            self.assertTrue(save_success)
            
            # Verify file was created
            self.assertTrue(os.path.exists(checkpoint_file))
            
            # Check file content
            with open(checkpoint_file, 'r') as f:
                content = f.read()
                self.assertIn('test', content)
                self.assertIn('value', content)
        
        finally:
            # Clean up
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
    
    def test_get_brain_statistics(self):
        """Test get_brain_statistics function"""
        # Create brain and run simulation
        config = create_default_config()
        brain = Brain(config)
        world = SimpleWorld()
        brain.initialize()
        
        # Run some steps
        loop = SimulationLoop(brain, world)
        loop.run_episode(3)
        
        # Get statistics
        stats = get_brain_statistics(brain)
        
        # Verify statistics structure
        self.assertIsInstance(stats, dict)
        
        required_keys = ['neuron_count', 'synapse_count', 'activity', 'region_info']
        for key in required_keys:
            self.assertIn(key, stats)
        
        # Check neuron count structure
        neuron_count = stats['neuron_count']
        self.assertIn('total', neuron_count)
        self.assertIn('active', neuron_count)
        self.assertIn('firing', neuron_count)
        
        # Check synapse count structure
        synapse_count = stats['synapse_count']
        self.assertIn('total', synapse_count)


class TestIntegration(unittest.TestCase):
    """Integration tests for complete workflows"""
    
    def test_complete_workflow(self):
        """Test a complete workflow with all components"""
        # Create all components
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        agent_brain = create_agent_brain(brain, world)
        
        # Configure world
        world.configure(640, 480, 320, 240)
        world.set_agent_start(100, 100)
        
        # Initialize all components
        brain.initialize()
        agent_brain.initialize(world)
        
        # Create simulation loop
        loop = SimulationLoop(brain, world, agent_brain)
        
        # Run simulation steps
        actions = []
        for step in range(5):
            action = loop.run_step()
            actions.append(action)
            self.assertIsInstance(action, Action)
        
        # Verify results
        self.assertEqual(len(actions), 5)
        self.assertGreater(loop.step_count, 0)
        
        # Check performance stats
        stats = loop.get_performance_stats()
        self.assertIn('steps_per_second', stats)
        self.assertIn('total_steps', stats)
        
        # Check brain stats
        brain_stats = loop.get_brain_stats()
        self.assertIn('neuron_count', brain_stats)
        self.assertIn('synapse_count', brain_stats)


class TestExceptions(unittest.TestCase):
    """Test exception handling"""
    
    def test_nlm_error(self):
        """Test NLMError"""
        error = NLMError("Test error")
        self.assertEqual(str(error), "Test error")
        self.assertIsInstance(error, Exception)
    
    def test_configuration_error(self):
        """Test ConfigurationError"""
        error = ConfigurationError("Config error")
        self.assertEqual(str(error), "Config error")
        self.assertIsInstance(error, NLMError)
        self.assertIsInstance(error, Exception)
    
    def test_brain_error(self):
        """Test BrainError"""
        error = BrainError("Brain error")
        self.assertEqual(str(error), "Brain error")
        self.assertIsInstance(error, NLMError)
        self.assertIsInstance(error, Exception)
    
    def test_world_error(self):
        """Test WorldError"""
        error = WorldError("World error")
        self.assertEqual(str(error), "World error")
        self.assertIsInstance(error, NLMError)
        self.assertIsInstance(error, Exception)


class TestPerformance(unittest.TestCase):
    """Performance tests"""
    
    def test_simulation_performance(self):
        """Test simulation performance"""
        import time
        
        # Create simulation
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        agent_brain = create_agent_brain(brain, world)
        
        world.configure(640, 480, 320, 240)
        brain.initialize()
        agent_brain.initialize(world)
        
        # Time simulation
        loop = SimulationLoop(brain, world, agent_brain)
        
        start_time = time.time()
        actions = loop.run_episode(10)
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_second = len(actions) / duration
        
        # Should be able to run reasonable number of steps
        self.assertGreater(steps_per_second, 1.0)
        self.assertEqual(len(actions), 10)
    
    def test_memory_efficiency(self):
        """Test memory efficiency"""
        import sys
        
        # Create objects
        config = Config()
        brain = Brain(config)
        world = SimpleWorld()
        
        # Get memory usage (approximate)
        initial_size = sys.getsizeof(config)
        brain_size = sys.getsizeof(brain)
        world_size = sys.getsizeof(world)
        
        # Objects should be reasonable size
        self.assertLess(initial_size, 1000)  # Should be small
        self.assertLess(brain_size, 10000)  # Should be reasonable
        self.assertLess(world_size, 5000)   # Should be reasonable


if __name__ == '__main__':
    # Run tests
    unittest.main(verbosity=2)
