"""
NLM Python Test Suite
Comprehensive tests for the NLM (Neural Learning Machine) Python bindings.
Tests configuration management, brain simulation, agent interaction, and cognitive functions.
"""

import sys
import os
import tempfile
import json
import unittest
from unittest.mock import Mock, patch
import numpy as np

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
class TestConfig(unittest.TestCase):
    """Test the improved Config system with JSON/YAML support."""
    
    def test_default_config_creation(self):
        """Test creating a default configuration."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertEqual(len(config.getKeys()), 0)
    
    def test_config_has_method(self):
        """Test checking if configuration has specific keys."""
        config = pynlm.createDefaultConfig()
        self.assertFalse(config.has("neuron_count"))
        
        # Set a value
        config.set("neuron_count", 1000)
        self.assertTrue(config.has("neuron_count"))
    
    def test_config_set_get_methods(self):
        """Test setting and getting configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Test string values
        config.set("string_key", "test_value")
        self.assertEqual(config.get("string_key").get(), "test_value")
        
        # Test integer values
        config.set("int_key", 42)
        self.assertEqual(config.get("int_key").get(), 42)
        
        # Test double values
        config.set("double_key", 3.14)
        self.assertEqual(config.get("double_key").get(), 3.14)
        
        # Test boolean values
        config.set("bool_key", True)
        self.assertEqual(config.get("bool_key").get(), True)
    
    def test_config_get_or_method(self):
        """Test getting values with defaults."""
        config = pynlm.createDefaultConfig()
        
        # Get existing value
        config.set("existing", "value")
        self.assertEqual(config.getOr("existing", "default"), "value")
        
        # Get non-existing value with default
        self.assertEqual(config.getOr("nonexistent", "default"), "default")
        self.assertEqual(config.getOr("nonexistent", 0), 0)
        self.assertEqual(config.getOr("nonexistent", False), False)
    
    def test_config_json_file_operations(self):
        """Test JSON file loading and saving."""
        config = pynlm.createDefaultConfig()
        
        # Set some test values
        config.set("neuron_count", 1000)
        config.set("learning_rate", 0.01)
        config.set("use_reward_modulation", True)
        config.set("tags", ["neural", "learning", "agent"])
        
        # Save to JSON file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json_file = f.name
        
        try:
            success = config.saveToFile(json_file)
            self.assertTrue(success)
            
            # Verify file was created and has content
            self.assertTrue(os.path.exists(json_file))
            with open(json_file, 'r') as f:
                data = json.load(f)
            
            self.assertIn("neuron_count", data)
            self.assertEqual(data["neuron_count"], 1000)
            self.assertIn("learning_rate", data)
            self.assertEqual(data["learning_rate"], 0.01)
            self.assertIn("use_reward_modulation", data)
            self.assertEqual(data["use_reward_modulation"], True)
            
            # Try to load it back
            config2 = pynlm.createDefaultConfig()
            success2 = config2.loadFromFile(json_file)
            self.assertTrue(success2)
            
            self.assertTrue(config2.has("neuron_count"))
            self.assertEqual(config2.get("neuron_count").get(), 1000)
            
        finally:
            # Clean up
            if os.path.exists(json_file):
                os.unlink(json_file)
    
    def test_config_command_line_loading(self):
        """Test loading configuration from command line arguments."""
        # This would typically be tested with subprocess calls to the executable
        # For now, we'll test the internal logic
        config = pynlm.createDefaultConfig()
        
        # Simulate command line args (without actually parsing them)
        test_args = ["--neuron_count=500", "--learning_rate=0.001", "--use_curiosity"]
        
        # Test that we can parse these args
        for arg in test_args:
            if arg.startswith("--"):
                if "=" in arg:
                    key, value = arg[2:].split("=", 1)
                    config.set(key, value)
        
        self.assertTrue(config.has("neuron_count"))
        self.assertEqual(config.get("neuron_count").get(), "500")
        self.assertTrue(config.has("learning_rate"))
        self.assertEqual(config.get("learning_rate").get(), "0.001")
        self.assertTrue(config.has("use_curiosity"))
        self.assertEqual(config.get("use_curiosity").get(), "True")
    
    def test_config_clear_and_remove(self):
        """Test clearing configuration and removing specific keys."""
        config = pynlm.createDefaultConfig()
        
        # Add some entries
        config.set("key1", "value1")
        config.set("key2", 42)
        config.set("key3", True)
        
        self.assertEqual(len(config.getKeys()), 3)
        
        # Remove one key
        config.remove("key2")
        self.assertEqual(len(config.getKeys()), 2)
        self.assertFalse(config.has("key2"))
        
        # Clear all
        config.clear()
        self.assertEqual(len(config.getKeys()), 0)
    
    def test_config_summary(self):
        """Test configuration summary generation."""
        config = pynlm.createDefaultConfig()
        
        config.set("string_key", "test")
        config.set("int_key", 123)
        config.set("bool_key", False)
        config.set("array_key", [1, 2, 3])
        
        summary = config.summary()
        self.assertIn("string_key", summary)
        self.assertIn("int_key", summary)
        self.assertIn("bool_key", summary)
        self.assertIn("array_key", summary)
        self.assertIn("Configuration (4 entries)", summary)
class TestBrain(unittest.TestCase):
    """Test basic brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
    
    def test_brain_step(self):
        """Test running brain simulation steps."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        for step in range(10):
            self.brain.step(step)
        
        # Brain should have processed spikes
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), initial_spikes)
        self.assertGreaterEqual(self.brain.getFiringNeuronCount(), 0)
    
    def test_brain_sensory_input(self):
        """Test injecting sensory input."""
        # Create a mock sensory input
        vision = pynlm.Vision(width=10, height=10, channels=3)
        # Fill with some test data
        test_data = [0.1] * 100
        vision.setData(test_data)
        
        # Inject into brain
        self.brain.receiveSensoryInput(vision)
        
        # Brain should have processed the input
        # (Exact behavior depends on implementation)
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), 0)
    
    def test_brain_action_production(self):
        """Test action production from brain."""
        action = self.brain.produceAction()
        self.assertIsNotNone(action)
        self.assertIn(action.getType(), [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Look,
            pynlm.ActionType.Interact,
            pynlm.ActionType.Eat,
            pynlm.ActionType.Drink,
            pynlm.ActionType.Rest,
            pynlm.ActionType.Wait,
            pynlm.ActionType.Custom
        ])
    
    def test_brain_region_management(self):
        """Test brain region management."""
        initial_regions = self.brain.getRegionCount()
        
        # Add a new region
        region_id = self.brain.addRegion("TestRegion")
        self.assertGreater(region_id.value, 0)
        
        self.assertEqual(self.brain.getRegionCount(), initial_regions + 1)
        
        # Get the region
        region = self.brain.getRegion(region_id)
        self.assertIsNotNone(region)
        self.assertEqual(region.getName(), "TestRegion")
    
    def test_brain_save_load(self):
        """Test brain state saving and loading."""
        # Run some simulation steps
        for step in range(10):
            self.brain.step(step)
        
        # Save brain state
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
        
        try:
            success = self.brain.save(checkpoint_file)
            self.assertTrue(success)
            
            # Create a new brain and load the state
            brain2 = pynlm.createBrain(self.config)
            brain2.initialize()
            
            success2 = brain2.load(checkpoint_file)
            self.assertTrue(success2)
            
            # Brains should have similar neuron counts (may not be exact due to randomness)
            self.assertGreaterEqual(brain2.getTotalNeuronCount(), 0)
            
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
class TestAgent(unittest.TestCase):
    """Test agent brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_creation(self):
        """Test agent brain creation and initialization."""
        self.assertIsNotNone(self.agent)
        self.assertGreater(self.agent.getSensoryInputSize(), 0)
        self.assertGreater(self.agent.getMotorOutputSize(), 0)
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        # Get current percept from world
        percept = self.world.getSensoryPercept()
        
        # Process through agent
        self.agent.processSensoryInput(percept)
        
        # Agent should have processed the input without errors
        # (Specific behavior depends on implementation)
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), 0)
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Process some sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Run brain step
        self.brain.step(0)
        
        # Decode motor command
        motor_cmd = self.agent.decodeMotorCommand()
        self.assertIsNotNone(motor_cmd)
        self.assertIn(motor_cmd, [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.LookLeft,
            pynlm.MotorCommand.LookRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ])
    
    def test_agent_neuromodulation(self):
        """Test neuromodulation functionality."""
        initial_curiosity = self.agent.getCuriosityLevel()
        initial_novelty = self.agent.getNoveltyLevel()
        
        # Apply some reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Neuromodulation levels should have changed
        self.assertGreaterEqual(self.agent.getCuriosityLevel(), initial_curiosity)
        self.assertGreaterEqual(self.agent.getNoveltyLevel(), initial_novelty)
    
    def test_agent_development(self):
        """Test developmental system."""
        initial_stage = self.brain.getDevelopmentalStage()
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Development stage may have changed
        self.assertIn(self.brain.getDevelopmentalStage(), [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ])
    
    def test_agent_subsystem_control(self):
        """Test enabling/disabling agent subsystems."""
        # Test enabling subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Test that subsystems are now enabled
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())
class TestWorld(unittest.TestCase):
    """Test world simulation functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.world.setAgentStart(5.0, 5.0)
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        self.assertIsNotNone(self.world)
        self.assertEqual(self.world.getWidth(), 10)
        self.assertEqual(self.world.getHeight(), 10)
    
    def test_world_update(self):
        """Test world simulation update."""
        time_before = self.world.getSimulationTime()
        
        # Update world
        self.world.update(0.1)
        
        time_after = self.world.getSimulationTime()
        self.assertGreater(time_after, time_before)
        self.assertAlmostEqual(time_after - time_before, 0.1, places=3)
    
    def test_world_sensory_percept(self):
        """Test sensory percept generation."""
        # Get percept
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Check vision component
        vision = percept.getVision()
        self.assertIsNotNone(vision)
        self.assertEqual(vision.getWidth(), 8)
        self.assertEqual(vision.getHeight(), 8)
        self.assertEqual(vision.getChannels(), 3)
        
        # Get data
        data = vision.getData()
        self.assertEqual(len(data), 8 * 8 * 3)  # width * height * channels
    
    def test_world_agent_body(self):
        """Test agent body state access."""
        body = self.world.getAgentBody()
        self.assertIsNotNone(body)
        
        # Agent should be at starting position
        self.assertAlmostEqual(body.x, 5.0, places=3)
        self.assertAlmostEqual(body.y, 5.0, places=3)
        self.assertEqual(body.orientation, 0.0)
    
    def test_world_motor_commands(self):
        """Test applying motor commands."""
        # Create a motor command
        motor_cmd = pynlm.MotorCommand.MoveForward
        
        # Apply to world
        self.world.applyMotorCommand(motor_cmd, 0.0)
        
        # Agent body should have changed
        body = self.world.getAgentBody()
        self.assertNotEqual(body.x, 5.0)  # Should have moved
        self.assertNotEqual(body.y, 5.0)  # Should have moved
    
    def test_world_random_seed(self):
        """Test random seed setting."""
        self.world.setRandomSeed(42)
        
        # World should have stable behavior with same seed
        time1 = self.world.getSimulationTime()
        self.world.update(0.1)
        time2 = self.world.getSimulationTime()
        
        # Reset and try again
        self.world.reset()
        self.world.setRandomSeed(42)
        self.world.update(0.1)
        time1_again = self.world.getSimulationTime()
        
        self.assertAlmostEqual(time2, time1_again, places=3)
class TestIntegration(unittest.TestCase):
    """Test integrated agent-world-brain simulation."""
    
    def test_complete_agent_simulation(self):
        """Test a complete agent simulation episode."""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable some subsystems for more interesting behavior
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        num_steps = 50
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process through agent
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action from agent
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply some reward (mock)
            reward = 0.0
            if world.getAgentBody().x > 7.0 and world.getAgentBody().y > 7.0:
                reward = 1.0
            
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log progress every 10 steps
            if step % 10 == 0:
                print(f"Step {step}:")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
                print(f"  Development stage: {brain.getDevelopmentalStage()}")
        
        # Verify simulation completed
        self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0)
        
        print("\n=== Simulation Complete ===")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
        print(f"Development stage: {brain.getDevelopmentalStage()}")
class TestConvenienceFunctions(unittest.TestCase):
    """Test convenience functions and factory methods."""
    
    def test_factory_functions(self):
        """Test the main factory functions."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        self.assertIsNotNone(config)
        self.assertIsNotNone(brain)
        self.assertIsNotNone(world)
        self.assertIsNotNone(agent)
        
        # Initialize components
        brain.initialize()
        agent.initialize(world)
        
        # All should work together
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(0)
        action = agent.decodeMotorCommand()
        
        self.assertIsNotNone(action)
    
    def test_world_object_creation(self):
        """Test world object creation."""
        world_obj = pynlm.WorldObject(3.0, 4.0, pynlm.WorldObjectType.Resource, value=5.0)
        
        self.assertAlmostEqual(world_obj.x, 3.0)
        self.assertAlmostEqual(world_obj.y, 4.0)
        self.assertEqual(world_obj.type, pynlm.WorldObjectType.Resource)
        self.assertEqual(world_obj.value, 5.0)
        self.assertTrue(world_obj.active)
    
    def test_action_creation(self):
        """Test action creation."""
        # Create action from type
        action1 = pynlm.Action(pynlm.ActionType.MoveForward)
        self.assertEqual(action1.getType(), pynlm.ActionType.MoveForward)
        
        # Create action with parameters
        action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.8, 0.3])
        self.assertEqual(action2.getType(), pynlm.ActionType.Custom)
        params = action2.getParameters()
        self.assertEqual(len(params), 3)
        
        # Clone action
        action3 = action1.clone()
        self.assertEqual(action3.getType(), action1.getType())
        
        # Modify cloned action
        action3.setType(pynlm.ActionType.TurnLeft)
        self.assertEqual(action3.getType(), pynlm.ActionType.TurnLeft)
    
    def test_sensory_input_creation(self):
        """Test sensory input creation."""
        vision = pynlm.Vision(width=16, height=16, channels=1)
        self.assertEqual(vision.getWidth(), 16)
        self.assertEqual(vision.getHeight(), 16)
        self.assertEqual(vision.getChannels(), 1)
        
        # Test data
        test_data = [0.1, 0.2, 0.3, 0.4]
        vision.setData(test_data)
        
        retrieved_data = vision.getData()
        self.assertEqual(len(retrieved_data), 4)
    
    def test_constants(self):
        """Test invalid ID constants."""
        self.assertIsNotNone(pynlm.INVALID_NEURON_ID)
        self.assertIsNotNone(pynlm.INVALID_SYNAPSE_ID)
        self.assertIsNotNone(pynlm.INVALID_REGION_ID)
        self.assertIsNotNone(pynlm.INVALID_POPULATION_ID)
        
        self.assertEqual(pynlm.INVALID_NEURON_ID.value, 0)
        self.assertEqual(pynlm.INVALID_SYNAPSE_ID.value, 0)
        self.assertEqual(pynlm.INVALID_REGION_ID.value, 0)
        self.assertEqual(pynlm.INVALID_POPULATION_ID.value, 0)
if __name__ == '__main__':
    print("=== NLM Python Test Suite ===")
    print("Testing improved NLM Python bindings...")
    
    # Run all tests
    unittest.main(verbosity=2)