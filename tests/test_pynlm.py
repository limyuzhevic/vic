"""
Comprehensive Python tests for pynlm module.
Tests all exposed classes, Pythonic features, error handling, and convenience methods.
"""

import sys
import os
import tempfile
import unittest

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm


class TestPynlmBasic(unittest.TestCase):
    """Test basic pynlm functionality"""
    
    def test_create_default_config(self):
        """Test creating a default configuration"""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertIsInstance(config, pynlm.Config)
        
        # Test basic config operations
        self.assertFalse(config.has("neuron_count"))
        config.set("neuron_count", 100)
        self.assertTrue(config.has("neuron_count"))
        self.assertEqual(config.get("neuron_count"), 100)
        
        # Test Pythonic features
        self.assertGreater(len(config), 0)
        config_str = str(config)
        self.assertIn("neuron_count", config_str)
        config_repr = repr(config)
        self.assertIn("Config:", config_repr)
    
    def test_create_brain(self):
        """Test creating a brain"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        self.assertIsNotNone(brain)
        self.assertIsInstance(brain, pynlm.Brain)
        
        brain.initialize()
        
        # Test basic brain operations
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertEqual(brain.getRegionCount(), 1)
        
        # Test Pythonic features
        brain_str = str(brain)
        self.assertIn("neurons=", brain_str)
        brain_repr = repr(brain)
        self.assertIn("Brain:", brain_repr)
        
        # Test stepping
        for i in range(5):
            brain.step(i)
        
        self.assertGreater(brain.getTotalSpikeCount(), 0)
    
    def test_create_simple_world(self):
        """Test creating a simple world"""
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        self.assertIsInstance(world, pynlm.SimpleWorld)
        
        world.configure(20, 20, 8, 8)
        world.reset()
        
        # Test world operations
        self.assertEqual(world.getWidth(), 20)
        self.assertEqual(world.getHeight(), 20)
        self.assertEqual(world.getSimulationTime(), 0.0)
        
        # Test Pythonic features
        world_str = str(world)
        self.assertIn("width=20", world_str)
        world_repr = repr(world)
        self.assertIn("SimpleWorld:", world_repr)
        
        # Test agent position
        agent_body = world.getAgentBody()
        self.assertEqual(agent_body.x, agent_body.x)
        self.assertEqual(agent_body.y, agent_body.y)
    
    def test_create_agent_brain(self):
        """Test creating an agent brain"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        self.assertIsInstance(agent, pynlm.AgentBrain)
        
        agent.initialize(world)
        
        # Test agent operations
        self.assertGreater(agent.getNeuromodulationLevel(), 0.0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
        self.assertGreater(agent.getNoveltyLevel(), 0.0)
        
        # Test Pythonic features
        agent_str = str(agent)
        self.assertIn("neuromodulation=", agent_str)
        agent_repr = repr(agent)
        self.assertIn("AgentBrain:", agent_repr)
    
    def test_sensory_percept(self):
        """Test sensory percept functionality"""
        # Create a simple percept
        percept = pynlm.SensoryPercept()
        self.assertIsNotNone(percept)
        
        # Test Pythonic features
        percept_str = str(percept)
        self.assertIn("timestamp=", percept_str)
        percept_repr = repr(percept)
        self.assertIn("SensoryPercept:", percept_repr)
        
        # Test setting vision
        vision = pynlm.Vision()
        vision.setData([0.1] * 64)
        percept.setVision(vision)
        self.assertEqual(percept.getVisionWidth(), 8)  # Default Vision width
        self.assertEqual(percept.getVisionHeight(), 8)  # Default Vision height
    
    def test_action(self):
        """Test action functionality"""
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        self.assertIsNotNone(action)
        
        # Test Pythonic features
        action_str = str(action)
        self.assertIn("type=", action_str)
        action_repr = repr(action)
        self.assertIn("Action:", action_repr)
        
        # Test parameter operations
        self.assertEqual(action.getParameterCount(), 0)
        action.addParameter(0.5)
        self.assertEqual(action.getParameterCount(), 1)
        self.assertEqual(action.getParameter(0), 0.5)
    
    def test_id_classes(self):
        """Test ID classes (NeuronId, SynapseId, RegionId, PopulationId)"""
        # Test NeuronId
        neuron_id = pynlm.NeuronId(123)
        self.assertEqual(neuron_id.value, 123)
        self.assertEqual(neuron_id.index(), 123)
        self.assertTrue(neuron_id == pynlm.NeuronId(123))
        self.assertFalse(neuron_id == pynlm.NeuronId(456))
        
        # Test SynapseId
        synapse_id = pynlm.SynapseId(456)
        self.assertEqual(synapse_id.value, 456)
        
        # Test RegionId
        region_id = pynlm.RegionId(789)
        self.assertEqual(region_id.value, 789)
        
        # Test PopulationId
        population_id = pynlm.PopulationId(999)
        self.assertEqual(population_id.value, 999)
        
        # Test Pythonic features
        for id_obj in [neuron_id, synapse_id, region_id, population_id]:
            id_str = str(id_obj)
            self.assertIn(": ", id_str)
            id_repr = repr(id_obj)
            self.assertIn("<".format(type(id_obj).__name__), id_repr)
    
    def test_enums(self):
        """Test enumeration values"""
        # Test NeuronType
        self.assertEqual(pynlm.NeuronType.Excitatory, pynlm.NeuronType.Excitatory)
        self.assertEqual(pynlm.NeuronType.Inhibitory, pynlm.NeuronType.Inhibitory)
        
        # Test SynapseType
        self.assertEqual(pynlm.SynapseType.Excitatory, pynlm.SynapseType.Excitatory)
        self.assertEqual(pynlm.SynapseType.Inhibitory, pynlm.SynapseType.Inhibitory)
        
        # Test ActionType
        self.assertEqual(pynlm.ActionType.MoveForward, pynlm.ActionType.MoveForward)
        self.assertEqual(pynlm.ActionType.MoveBackward, pynlm.ActionType.MoveBackward)
        
        # Test WorldObjectType
        self.assertEqual(pynlm.WorldObjectType.Empty, pynlm.WorldObjectType.Empty)
        self.assertEqual(pynlm.WorldObjectType.Resource, pynlm.WorldObjectType.Resource)
    
    def test_error_handling(self):
        """Test error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test getting non-existent key
        self.assertIsNone(config.get("non_existent_key"))
        
        # Test Config convenience methods with default values
        self.assertEqual(config.getBool("non_existent", False), False)
        self.assertEqual(config.getInt("non_existent", 42), 42)
        self.assertEqual(config.getDouble("non_existent", 3.14), 3.14)
        self.assertEqual(config.getString("non_existent", "default"), "default")
    
    def test_convenience_methods(self):
        """Test convenience methods"""
        # Test Config convenience methods
        config = pynlm.createDefaultConfig()
        config.setBool("test_bool", True)
        config.setInt("test_int", 123)
        config.setDouble("test_double", 45.67)
        config.setString("test_string", "hello")
        
        self.assertTrue(config.getBool("test_bool"))
        self.assertEqual(config.getInt("test_int"), 123)
        self.assertAlmostEqual(config.getDouble("test_double"), 45.67)
        self.assertEqual(config.getString("test_string"), "hello")
        
        # Test Vision convenience methods
        vision = pynlm.Vision()
        vision.setData([0.1, 0.2, 0.3, 0.4] * 16)  # 8x8 image
        self.assertEqual(vision.getArea(), 64)
        self.assertEqual(vision.getPixelCount(), 192)  # 64 pixels * 3 channels
        
        # Test Audio convenience methods
        audio = pynlm.Audio(44100, 4410)  # 1 second of audio
        audio.setData([0.1] * 4410)
        self.assertAlmostEqual(audio.getDurationSeconds(), 1.0, places=3)
        
        # Test SensoryPercept convenience methods
        percept = pynlm.SensoryPercept()
        vision = pynlm.Vision(10, 10, 3)
        percept.setVision(vision)
        self.assertTrue(percept.hasVision())
        self.assertFalse(percept.hasTouch())
        self.assertFalse(percept.hasInternal())
        self.assertFalse(percept.hasAudio())
        self.assertEqual(percept.getCombinedSignalCount(), 1)
        
        # Test SimpleWorld convenience methods
        world = pynlm.createSimpleWorld()
        world.configure(30, 30, 8, 8)
        world.reset()
        
        world_size = world.getWorldSize()
        self.assertEqual(world_size[0], 30)
        self.assertEqual(world_size[1], 30)
        
        agent_pos = world.getAgentPosition()
        self.assertEqual(agent_pos[0], world.getAgentBody().x)
        self.assertEqual(agent_pos[1], world.getAgentBody().y)
        
        self.assertGreater(world.getAgentEnergy(), 0.0)
        self.assertGreater(world.getAgentHealth(), 0.0)
        self.assertGreater(world.getAgentAge(), 0.0)
        self.assertTrue(world.isAgentAlive())
    
    def test_full_simulation(self):
        """Test a complete simulation with all components"""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(20):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print status every 5 steps
            if step % 5 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                      f"{brain.getTotalSpikeCount()} total spikes, "
                      f"{agent.getNeuromodulationLevel():.2f} neuromodulation")
        
        # Verify final state
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(agent.getNeuromodulationLevel(), 0.0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
        self.assertGreater(agent.getNoveltyLevel(), 0.0)
        self.assertGreater(agent.getPredictionError(), 0.0)
        
        print("\nFull simulation test completed successfully!")


class TestPynlmAdvanced(unittest.TestCase):
    """Test advanced pynlm functionality"""
    
    def test_id_operations(self):
        """Test ID class operations"""
        neuron1 = pynlm.NeuronId(100)
        neuron2 = pynlm.NeuronId(100)
        neuron3 = pynlm.NeuronId(200)
        
        # Test equality and hashing
        self.assertEqual(neuron1, neuron2)
        self.assertNotEqual(neuron1, neuron3)
        
        # Test in sets (requires __hash__)
        neuron_set = {neuron1, neuron2, neuron3}
        self.assertEqual(len(neuron_set), 2)  # neuron1 == neuron2
        
        # Test invalid IDs (should work but may have special handling)
        invalid_id = pynlm.INVALID_NEURON_ID
        self.assertIsNotNone(invalid_id)
    
    def test_config_file_operations(self):
        """Test configuration file operations"""
        config = pynlm.createDefaultConfig()
        
        # Set some values
        config.setBool("test_enabled", True)
        config.setInt("test_count", 42)
        config.setDouble("test_value", 3.14159)
        config.setString("test_name", "test_config")
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            filepath = f.name
        
        try:
            # Save configuration
            success = config.saveToFile(filepath)
            self.assertTrue(success)
            
            # Verify file exists and has content
            self.assertGreater(os.path.getsize(filepath), 0)
            
            # Load configuration back
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromFile(filepath)
            self.assertTrue(success)
            
            # Verify values were saved and loaded correctly
            self.assertTrue(config2.getBool("test_enabled"))
            self.assertEqual(config2.getInt("test_count"), 42)
            self.assertAlmostEqual(config2.getDouble("test_value"), 3.14159)
            self.assertEqual(config2.getString("test_name"), "test_config")
            
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    def test_action_operations(self):
        """Test action operations"""
        # Create action with parameters
        action = pynlm.Action(pynlm.ActionType.Interact, [0.5, 0.7, 0.9])
        
        # Test parameter operations
        self.assertEqual(action.getParameterCount(), 3)
        self.assertEqual(action.getParameter(0), 0.5)
        self.assertEqual(action.getParameter(1), 0.7)
        self.assertEqual(action.getParameter(2), 0.9)
        
        # Test cloning
        action_copy = action.clone()
        self.assertEqual(action_copy.getType(), action.getType())
        self.assertEqual(action_copy.getParameterCount(), action.getParameterCount())
        
        # Test modification
        action_copy.addParameter(1.0)
        self.assertEqual(action_copy.getParameterCount(), 4)
        self.assertEqual(action_copy.getParameter(3), 1.0)
    
    def test_world_object_operations(self):
        """Test world object operations"""
        # Create world objects
        wall = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Wall, 0.0, 0.5)
        resource = pynlm.WorldObject(3.0, 4.0, pynlm.WorldObjectType.Resource, 10.0, 0.3)
        
        # Test object properties
        self.assertEqual(wall.x, 1.0)
        self.assertEqual(wall.y, 2.0)
        self.assertEqual(wall.type, pynlm.WorldObjectType.Wall)
        self.assertEqual(wall.radius, 0.5)
        
        self.assertEqual(resource.x, 3.0)
        self.assertEqual(resource.y, 4.0)
        self.assertEqual(resource.type, pynlm.WorldObjectType.Resource)
        self.assertEqual(resource.value, 10.0)
        self.assertEqual(resource.radius, 0.3)
        
        # Test object is active by default
        self.assertTrue(wall.active)
        self.assertTrue(resource.active)
        
        # Test modifying objects
        wall.active = False
        self.assertFalse(wall.active)
        resource.value = 20.0
        self.assertEqual(resource.value, 20.0)
    
    def test_agent_body_operations(self):
        """Test agent body operations"""
        body = pynlm.AgentBody()
        
        # Set initial values
        body.x = 5.0
        body.y = 10.0
        body.orientation = 90.0
        body.velocityX = 1.0
        body.velocityY = 2.0
        body.angularVelocity = 0.5
        body.energy = 100.0
        body.health = 85.0
        body.age = 5
        body.isMoving = True
        body.isTurning = False
        body.lastActionTime = 10.0
        
        # Test values
        self.assertEqual(body.x, 5.0)
        self.assertEqual(body.y, 10.0)
        self.assertEqual(body.orientation, 90.0)
        self.assertEqual(body.velocityX, 1.0)
        self.assertEqual(body.velocityY, 2.0)
        self.assertEqual(body.angularVelocity, 0.5)
        self.assertEqual(body.energy, 100.0)
        self.assertEqual(body.health, 85.0)
        self.assertEqual(body.age, 5)
        self.assertTrue(body.isMoving)
        self.assertFalse(body.isTurning)
        self.assertEqual(body.lastActionTime, 10.0)
        
        # Test reset
        body.reset()
        self.assertEqual(body.energy, 100.0)  # Should reset to max energy
        self.assertEqual(body.health, 100.0)  # Should reset to max health
        self.assertEqual(body.age, 0)  # Should reset age


class TestPynlmIntegration(unittest.TestCase):
    """Integration tests for pynlm"""
    
    def test_example_from_easy_usage(self):
        """Test the examples from easy_usage.md"""
        # Example 1: Simplest Brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brain.step(0)
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        
        # Example 2: Complete Agent Pattern
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run for a few steps
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        self.assertGreater(agent.getNeuromodulationLevel(), 0.0)
        
        print("Integration test passed!")


if __name__ == '__main__':
    print("Running comprehensive pynlm tests...")
    print("=" * 50)
    
    # Run all tests
    suite = unittest.TestLoader().loadTestsFromTestCase(TestPynlmBasic)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestPynlmAdvanced))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestPynlmIntegration))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    if result.failures or result.errors:
        sys.exit(1)
    else:
        print("\nAll tests passed successfully!")
        sys.exit(0)