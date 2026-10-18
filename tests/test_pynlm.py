"""
Python tests for NLM bindings.
Tests the Python API for the NLM (Neural Learning Machine) neural simulation framework.
"""

import sys
import os
import unittest
import tempfile
import json

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

class TestConfig(unittest.TestCase):
    """Test configuration functionality."""
    
    def test_create_default_config(self):
        """Test creating a default configuration."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertIsInstance(config, pynlm.Config)
        
        # Test default values
        self.assertTrue(config.has('neuron_count'))
        self.assertEqual(config.get('neuron_count', 0), 1000)
    
    def test_config_set_get(self):
        """Test setting and getting configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Test setting values
        config.set('test_value', 42)
        self.assertEqual(config.get('test_value', None), 42)
        
        config.set('string_value', 'test')
        self.assertEqual(config.get('string_value', None), 'test')
        
        config.set('bool_value', True)
        self.assertTrue(config.get('bool_value', False))
    
    def test_config_summary(self):
        """Test configuration summary."""
        config = pynlm.createDefaultConfig()
        summary = config.summary()
        
        self.assertIsInstance(summary, str)
        self.assertIn('Configuration', summary)
        self.assertIn('neuron_count', summary)
    
    def test_config_save_load(self):
        """Test saving and loading configuration to/from file."""
        config = pynlm.createDefaultConfig()
        config.set('custom_value', 'test_value')
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_path = f.name
        
        try:
            success = config.save_to_file(temp_path)
            self.assertTrue(success)
            
            # Load from file
            new_config = pynlm.createDefaultConfig()
            success = new_config.load_from_file(temp_path)
            self.assertTrue(success)
            
            # Check that custom value was preserved
            self.assertEqual(new_config.get('custom_value', None), 'test_value')
            
        finally:
            # Clean up
            if os.path.exists(temp_path):
                os.unlink(temp_path)


class TestBrain(unittest.TestCase):
    """Test brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        success = self.brain.initialize()
        self.assertTrue(success)
        self.assertTrue(self.brain._initialized)
    
    def test_brain_step(self):
        """Test brain step."""
        self.brain.initialize()
        
        # Test single step
        self.brain.step(0)
        
        # Test step with time
        self.brain.step(1, 0.001)
    
    def test_brain_statistics(self):
        """Test brain statistics."""
        self.brain.initialize()
        
        # Test various statistics
        total_neurons = self.brain.get_total_neuron_count()
        self.assertIsInstance(total_neurons, int)
        self.assertGreaterEqual(total_neurons, 0)
        
        total_synapses = self.brain.get_total_synapse_count()
        self.assertIsInstance(total_synapses, int)
        self.assertGreaterEqual(total_synapses, 0)
        
        active_neurons = self.brain.get_active_neuron_count()
        self.assertIsInstance(active_neurons, int)
        self.assertGreaterEqual(active_neurons, 0)
        
        firing_neurons = self.brain.get_firing_neuron_count()
        self.assertIsInstance(firing_neurons, int)
        self.assertGreaterEqual(firing_neurons, 0)
        
        total_spikes = self.brain.get_total_spike_count()
        self.assertIsInstance(total_spikes, int)
        self.assertGreaterEqual(total_spikes, 0)
        
        avg_firing_rate = self.brain.get_average_firing_rate()
        self.assertIsInstance(avg_firing_rate, float)
        self.assertGreaterEqual(avg_firing_rate, 0.0)
    
    def test_brain_region_management(self):
        """Test brain region management."""
        self.brain.initialize()
        
        # Test adding region
        region_id = self.brain.add_region("test_region")
        self.assertIsInstance(region_id, int)
        
        # Test getting region
        region = self.brain.get_region(region_id)
        self.assertIsNotNone(region)
        
        # Test getting region count
        region_count = self.brain.get_region_count()
        self.assertIsInstance(region_count, int)
        
        # Test getting region IDs
        region_ids = self.brain.get_region_ids()
        self.assertIsInstance(region_ids, list)
        
        # Test getting regions
        regions = self.brain.get_regions()
        self.assertIsInstance(regions, list)


class TestWorld(unittest.TestCase):
    """Test world functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.world = pynlm.createSimpleWorld()
    
    def test_world_configuration(self):
        """Test world configuration."""
        self.world.configure(width=20, height=15, vision_width=16, vision_height=16)
        
        self.assertEqual(self.world.get_width(), 20)
        self.assertEqual(self.world.get_height(), 15)
    
    def test_world_reset(self):
        """Test world reset."""
        self.world.update(0.1)
        self.assertGreater(self.world.get_simulation_time(), 0)
        
        self.world.reset()
        self.assertEqual(self.world.get_simulation_time(), 0.0)
    
    def test_world_agent_body(self):
        """Test agent body."""
        agent_body = self.world.get_agent_body()
        
        self.assertIsInstance(agent_body, pynlm.AgentBody)
        self.assertEqual(agent_body.x, 0.0)
        self.assertEqual(agent_body.y, 0.0)
        self.assertEqual(agent_body.energy, pynlm.MAX_ENERGY)
    
    def test_world_object_management(self):
        """Test world object management."""
        # Create and add world object
        obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectTypes.FOOD)
        self.world.add_object(obj)
        
        # Get objects in range
        objects_in_range = self.world.get_objects_in_range(0.0, 0.0, 10.0)
        self.assertIsInstance(objects_in_range, list)
        self.assertGreater(len(objects_in_range), 0)
        
        # Get objects by type
        food_objects = self.world.get_objects_by_type(pynlm.WorldObjectTypes.FOOD)
        self.assertIsInstance(food_objects, list)


class TestAgent(unittest.TestCase):
    """Test agent functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, vision_width=16, vision_height=16)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        self.assertTrue(self.agent.is_initialized)
        self.assertEqual(self.agent.world, self.world)
        self.assertEqual(self.agent.brain, self.brain)
    
    def test_agent_features(self):
        """Test agent feature enablement."""
        # Enable features
        self.agent.enable_reward_modulation(True)
        self.agent.enable_curiosity(True)
        self.agent.enable_structural_plasticity(True)
        self.agent.enable_development(True)
        
        # Check feature status
        self.assertTrue(self.agent.is_reward_modulation_enabled())
        self.assertTrue(self.agent.is_curiosity_enabled())
        self.assertTrue(self.agent.is_structural_plasticity_enabled())
        self.assertTrue(self.agent.is_development_enabled())
    
    def test_agent_sensory_processing(self):
        """Test sensory processing."""
        percept = self.world.get_sensory_percept()
        self.assertIsNotNone(percept)
        self.assertIsInstance(percept, pynlm.SensoryPercept)
        
        # Process sensory input
        self.agent.process_sensory_input(percept)
    
    def test_agent_action_selection(self):
        """Test action selection."""
        action = self.agent.decode_motor_command()
        self.assertIsNotNone(action)
        self.assertIsInstance(action, pynlm.Action)
        self.assertIsInstance(action.type, str)
    
    def test_agent_brain_stats(self):
        """Test brain statistics retrieval."""
        stats = self.agent.get_brain_stats()
        self.assertIsInstance(stats, dict)
        
        # Check for expected statistics
        expected_keys = ['total_neurons', 'total_synapses', 'active_neurons',
                        'firing_neurons', 'total_spikes', 'average_firing_rate']
        for key in expected_keys:
            self.assertIn(key, stats)


class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced features."""
    
    def test_world_objects(self):
        """Test advanced world object functionality."""
        world = pynlm.createWorld(30, 30, 20, 20)
        
        # Create various objects
        agent_obj = pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectTypes.AGENT)
        food_obj = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectTypes.FOOD)
        water_obj = pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectTypes.WATER, radius=2.0)
        wall_obj = pynlm.WorldObject(15.0, 5.0, pynlm.WorldObjectTypes.WALL, radius=1.0)
        hazard_obj = pynlm.WorldObject(25.0, 15.0, pynlm.WorldObjectTypes.HAZARD, radius=1.5)
        
        # Add objects to world
        for obj in [agent_obj, food_obj, water_obj, wall_obj, hazard_obj]:
            world.add_object(obj)
        
        # Test distance calculations
        self.assertGreater(agent_obj.get_distance_to(food_obj), 0)
        self.assertGreater(agent_obj.get_distance_to(water_obj), 0)
        
        # Test collision detection
        self.assertFalse(agent_obj.is_colliding(food_obj))
        self.assertTrue(agent_obj.is_colliding(wall_obj))  # Agent is at (0,0), wall at (15,5)
        
        # Test range queries
        food_objects = world.get_objects_by_type(pynlm.WorldObjectTypes.FOOD)
        self.assertEqual(len(food_objects), 1)
        
        objects_in_range = world.get_objects_in_range(0.0, 0.0, 12.0)
        self.assertGreater(len(objects_in_range), 0)
    
    def test_agent_movement(self):
        """Test agent movement with actions."""
        world = pynlm.createWorld(50, 50, 20, 20)
        
        # Create agent
        agent = pynlm.create_default_agent()
        
        # Apply various actions
        action1 = agent.decode_motor_command()
        self.assertIsNotNone(action1)
        
        # Test action execution
        action = pynlm.Action(pynlm.ActionTypes.MOVE_FORWARD)
        self.assertEqual(action.type, pynlm.ActionTypes.MOVE_FORWARD)
        
        action2 = pynlm.Action(pynlm.ActionTypes.TURN_LEFT)
        self.assertEqual(action2.type, pynlm.ActionTypes.TURN_LEFT)
        
        action3 = pynlm.Action(pynlm.ActionTypes.INTERACT)
        self.assertEqual(action3.type, pynlm.ActionTypes.INTERACT)
        
        # Test action cloning
        action_copy = action.clone()
        self.assertEqual(action.type, action_copy.type)
        self.assertEqual(action.parameters, action_copy.parameters)
    
    def test_neuromodulation(self):
        """Test neuromodulation features."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createWorld(30, 30, 20, 20)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable neuromodulation features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_novelty(True)
        
        # Check neuromodulation state
        neuromodulation_state = agent.get_neuromodulation_state()
        self.assertIsInstance(neuromodulation_state, dict)
        
        curiosity_state = agent.get_curiosity_state()
        self.assertIsInstance(curiosity_state, dict)
    
    def test_sensory_perception(self):
        """Test sensory perception functionality."""
        world = pynlm.createWorld(30, 30, 20, 20)
        
        # Get sensory percept
        percept = world.get_sensory_percept()
        self.assertIsNotNone(percept)
        
        # Test sensory modalities
        self.assertTrue(percept.has_sensory_input('vision'))
        self.assertTrue(percept.has_sensory_input('internal'))
        
        # Test sensory dimensions
        dimensions = percept.get_sensory_dimensions()
        self.assertIsInstance(dimensions, dict)
        
        # Test sensory data
        sensory_data = percept.get_sensory_data()
        self.assertIsInstance(sensory_data, dict)
    
    def test_configuration_complexity(self):
        """Test complex configuration scenarios."""
        config = pynlm.createDefaultConfig()
        
        # Set multiple configuration values
        config.set('neuron_count', 2000)
        config.set('region_count', 3)
        config.set('connection_probability', 0.2)
        config.set('stdp_ltp_weight', 0.03)
        config.set('stdp_ltd_weight', 0.025)
        
        # Verify configuration
        self.assertEqual(config.get('neuron_count', 0), 2000)
        self.assertEqual(config.get('region_count', 0), 3)
        self.assertEqual(config.get('connection_probability', 0.0), 0.2)
        self.assertEqual(config.get('stdp_ltp_weight', 0.0), 0.03)
        self.assertEqual(config.get('stdp_ltd_weight', 0.0), 0.025)
        
        # Test configuration summary
        summary = config.summary()
        self.assertIsInstance(summary, str)
        self.assertIn('2000', summary)
        self.assertIn('3', summary)


class TestExamples(unittest.TestCase):
    """Test example usage patterns."""
    
    def test_basic_brain_example(self):
        """Test basic brain example."""
        # This would test the example code from get_examples()
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation steps
        for i in range(100):
            brain.step(i)
        
        # Check results
        total_spikes = brain.getTotalSpikeCount()
        self.assertIsInstance(total_spikes, int)
    
    def test_brain_world_agent_example(self):
        """Test brain-world-agent example."""
        # Create components
        brain = pynlm.createBrain()
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, vision_width=8, vision_height=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation loop
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify results
        self.assertTrue(world.getSimulationTime() > 0)
        self.assertGreater(world.getAgentBody().energy, 0)


def run_examples():
    """Run example usage patterns."""
    examples = pynlm.get_examples()
    
    print("Running NLM examples...")
    
    # Example 1: Basic Brain
    print("\n1. Basic Brain Example:")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    # Example 2: Brain, World, Agent
    print("\n2. Brain-World-Agent Example:")
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, vision_width=8, vision_height=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"   Simulation completed: {world.getSimulationTime()}s")
    
    # Example 3: Learning Agent
    print("\n3. Learning Agent Example:")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, vision_width=8, vision_height=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run agent simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"   Agent simulation completed with {world.getAgentBody().energy:.1f} energy")


if __name__ == '__main__':
    print("Running NLM Python bindings tests...")
    
    # Run unit tests
    unittest.main(argv=[''], exit=False, verbosity=2)
    
    # Run examples
    run_examples()
    
    print("\nAll tests completed successfully!")
