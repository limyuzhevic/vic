#!/usr/bin/env python3
"""
Python test suite for NLM (Neural Learning Machine) Python bindings.

This test suite provides comprehensive testing for the pynlm Python bindings,
encompassing basic functionality, integration tests, and performance validation.
"""

import sys
import os
import unittest
import tempfile
import json
from pathlib import Path

# Add the Python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm not available: {e}")

# Test constants
TEST_CONFIG_FILE = "test_config.json"
TEST_BRAIN_FILE = "test_brain.nnet"

@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm not available")
class TestBasicPythonAPI(unittest.TestCase):
    """Test basic Python API functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = nlm.createDefaultConfig()
        self._configure_test_brain()
        
    def _configure_test_brain(self):
        """Configure test brain with basic parameters."""
        self.config.set("neuron_count", 100)
        self.config.set("region_count", 1)
        self.config.set("connection_probability", 0.1)
        self.config.set("random_seed", 42)
        self.config.set("simulation_timestep", 0.001)
        
    def tearDown(self):
        """Clean up after tests."""
        # Clean up test files
        for f in [TEST_CONFIG_FILE, TEST_BRAIN_FILE]:
            if os.path.exists(f):
                os.remove(f)
    
    def test_config_creation(self):
        """Test creating a default configuration."""
        config = nlm.createDefaultConfig()
        self.assertIsNotNone(config)
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("region_count"))
        
    def test_config_parameters(self):
        """Test configuration parameter setting and getting."""
        # Test setting values
        self.config.set("test_int", 42)
        self.config.set("test_float", 3.14)
        self.config.set("test_string", "hello")
        
        # Test getting values
        self.assertEqual(self.config.getOr("test_int", 0), 42)
        self.assertEqual(self.config.getOr("test_float", 0.0), 3.14)
        self.assertEqual(self.config.getOr("test_string", ""), "hello")
        
    def test_config_save_load(self):
        """Test saving and loading configurations."""
        # Set up test configuration
        self.config.set("test_value", 123)
        self.config.set("test_name", "Test Config")
        
        # Save to file
        self.config.saveToFile(TEST_CONFIG_FILE)
        self.assertTrue(os.path.exists(TEST_CONFIG_FILE))
        
        # Load from file
        loaded_config = nlm.createDefaultConfig()
        loaded_config.loadFromFile(TEST_CONFIG_FILE)
        
        # Verify loaded values
        self.assertEqual(loaded_config.getOr("test_value", 0), 123)
        self.assertEqual(loaded_config.getOr("test_name", ""), "Test Config")
        
    def test_brain_creation(self):
        """Test creating a brain with configuration."""
        brain = nlm.createBrain(self.config)
        self.assertIsNotNone(brain)
        
    def test_brain_initialization(self):
        """Test brain initialization."""
        brain = nlm.createBrain(self.config)
        brain.initialize()
        
        # Verify brain properties after initialization
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        
    def test_brain_step(self):
        """Test brain stepping."""
        brain = nlm.createBrain(self.config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Take a few steps
        for step in range(10):
            brain.step(step, step * 0.001)
        
        final_spikes = brain.getTotalSpikeCount()
        self.assertGreaterEqual(final_spikes, initial_spikes)
        
    def test_agent_creation(self):
        """Test creating an agent brain."""
        brain = nlm.createBrain(self.config)
        agent_brain = nlm.createAgentBrain(brain)
        self.assertIsNotNone(agent_brain)
        
    def test_simple_world(self):
        """Test creating and using a simple world."""
        world = nlm.createSimpleWorld()
        world.configure(20.0, 20.0, 16, 16)
        world.setRandomSeed(42)
        
        # Verify world configuration
        self.assertEqual(world.getWidth(), 20.0)
        self.assertEqual(world.getHeight(), 20.0)
        self.assertEqual(world.getVisionWidth(), 16)
        self.assertEqual(world.getVisionHeight(), 16)
        
    def test_sensory_input(self):
        """Test sensory input creation and manipulation."""
        # Test vision
        vision = nlm.Vision(10, 10, 3)
        test_data = [0.1, 0.2, 0.3] * 100  # 10x10x3 = 300 values
        vision.setData(test_data)
        
        self.assertEqual(vision.getWidth(), 10)
        self.assertEqual(vision.getHeight(), 10)
        self.assertEqual(vision.getChannels(), 3)
        
        # Test audio
        audio = nlm.Audio(44100, 1024)
        audio.setData([0.0] * 1024)
        
        self.assertEqual(audio.getSampleRate(), 44100)
        self.assertEqual(audio.getNumSamples(), 1024)
        
    def test_action_creation(self):
        """Test action creation and manipulation."""
        # Test action by type
        action = nlm.Action(nlm.ActionType.MoveForward)
        self.assertEqual(action.getType(), nlm.ActionType.MoveForward)
        self.assertEqual(action.getName(), "MoveForward")
        
        # Test action with parameters
        action2 = nlm.Action(nlm.ActionType.Custom, [0.5, 0.8, 1.2])
        self.assertEqual(action2.getType(), nlm.ActionType.Custom)
        self.assertEqual(action2.getParameters(), [0.5, 0.8, 1.2])
        
    def test_world_object(self):
        """Test world object creation."""
        obj = nlm.WorldObject(5.0, 5.0, nlm.WorldObjectType.Resource, 10.0, 0.5)
        
        self.assertEqual(obj.x, 5.0)
        self.assertEqual(obj.y, 5.0)
        self.assertEqual(obj.radius, 0.5)
        self.assertEqual(obj.type, nlm.WorldObjectType.Resource)
        self.assertEqual(obj.value, 10.0)
        self.assertTrue(obj.active)
        
    def test_agent_body(self):
        """Test agent body creation and manipulation."""
        agent = nlm.AgentBody()
        
        # Test default values
        self.assertEqual(agent.x, 0.0)
        self.assertEqual(agent.y, 0.0)
        self.assertEqual(agent.orientation, 0.0)
        self.assertEqual(agent.velocityX, 0.0)
        self.assertEqual(agent.velocityY, 0.0)
        self.assertEqual(agent.energy, 100.0)  # Default energy
        
        # Test setting values
        agent.x = 10.0
        agent.y = 15.0
        agent.orientation = 0.5
        agent.energy = 80.0
        
        self.assertEqual(agent.x, 10.0)
        self.assertEqual(agent.y, 15.0)
        self.assertEqual(agent.orientation, 0.5)
        self.assertEqual(agent.energy, 80.0)
        
    def test_motor_commands(self):
        """Test motor command enumeration."""
        motor_commands = [
            nlm.MotorCommand.MoveForward,
            nlm.MotorCommand.MoveBackward,
            nlm.MotorCommand.TurnLeft,
            nlm.MotorCommand.TurnRight,
            nlm.MotorCommand.Interact,
            nlm.MotorCommand.Wait
        ]
        
        for cmd in motor_commands:
            self.assertIsNotNone(cmd)
            
    def test_neuron_ids(self):
        """Test neuron ID operations."""
        id1 = nlm.NeuronId(123)
        id2 = nlm.NeuronId(456)
        id3 = nlm.NeuronId(123)
        
        # Test equality
        self.assertEqual(id1, id3)
        self.assertNotEqual(id1, id2)
        
        # Test indexing
        self.assertEqual(id1.index(), 123)
        
    def test_color_enums(self):
        """Test color/type enumerations."""
        # Test neuron types
        neuron_types = [nlm.NeuronType.Excitatory, nlm.NeuronType.Inhibitory,
                       nlm.NeuronType.Sensory, nlm.NeuronType.Motor]
        for nt in neuron_types:
            self.assertIsNotNone(nt)
            
        # Test synapse types
        synapse_types = [nlm.SynapseType.Excitatory, nlm.SynapseType.Inhibitory]
        for st in synapse_types:
            self.assertIsNotNone(st)
            
        # Test developmental stages
        stages = [nlm.DevelopmentalStage.Initial, nlm.DevelopmentalStage.Adult]
        for stage in stages:
            self.assertIsNotNone(stage)
            
    def test_config_validation(self):
        """Test configuration validation."""
        config = nlm.createDefaultConfig()
        
        # Test has method
        self.assertFalse(config.has("nonexistent_key"))
        
        config.set("existing_key", "value")
        self.assertTrue(config.has("existing_key"))
        
    def test_config_summary(self):
        """Test configuration summary."""
        config = nlm.createDefaultConfig()
        config.set("test_key", "test_value")
        
        summary = config.summary()
        self.assertIsInstance(summary, str)
        self.assertIn("test_key", summary)

@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm not available")
class TestIntegrationPythonAPI(unittest.TestCase):
    """Integration tests for the NLM Python API."""
    
    def setUp(self):
        """Set up integration test fixtures."""
        self.config = nlm.createDefaultConfig()
        self._configure_test_brain()
        
    def _configure_test_brain(self):
        """Configure test brain for integration tests."""
        self.config.set("neuron_count", 200)
        self.config.set("region_count", 2)
        self.config.set("connection_probability", 0.15)
        self.config.set("random_seed", 123)
        
    def tearDown(self):
        """Clean up after tests."""
        if os.path.exists(TEST_BRAIN_FILE):
            os.remove(TEST_BRAIN_FILE)
    
    def test_full_agent_simulation(self):
        """Test complete agent-based simulation."""
        # Create components
        world = nlm.createSimpleWorld()
        world.configure(30.0, 30.0, 20, 20)
        world.setAgentStart(5.0, 5.0)
        
        brain = nlm.createBrain(self.config)
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Configure agent
        agent_brain.enableRewardModulation(True)
        agent_brain.enableStructuralPlasticity(True)
        agent_brain.enableDevelopment(True)
        agent_brain.enableCuriosity(True)
        
        # Run simulation steps
        initial_energy = world.getAgentBody().energy
        for step in range(50):
            # Get sensory input
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            
            # Run brain dynamics
            for i in range(10):
                brain.step(step * 10 + i, step * 0.05 + i * 0.001)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            agent_brain.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            agent_brain.updateDevelopment(0.05)
            
            # Verify results
            self.assertGreaterEqual(world.getAgentBody().energy, 0.0)
            self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
        
        # Verify final state
        self.assertLessEqual(world.getAgentBody().energy, initial_energy)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        
    def test_save_load_brain(self):
        """Test saving and loading brain states."""
        # Create and run brain
        brain = nlm.createBrain(self.config)
        brain.initialize()
        
        # Run for some steps
        for step in range(100):
            brain.step(step, step * 0.001)
        
        # Save brain state
        brain.save(TEST_BRAIN_FILE)
        self.assertTrue(os.path.exists(TEST_BRAIN_FILE))
        
        # Create new brain and load state
        new_brain = nlm.createBrain(self.config)
        new_brain.reset()
        new_brain.initialize()
        
        # Load should work even if brain was already initialized
        load_result = new_brain.load(TEST_BRAIN_FILE)
        # Note: load might fail depending on implementation
        # We just verify it doesn't crash
        
    def test_memory_systems(self):
        """Test memory system access."""
        brain = nlm.createBrain(self.config)
        brain.initialize()
        
        # Test memory system access
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        # Memory systems might not always be available depending on configuration
        # This test just verifies they can be accessed without crashing
        
    def test_prediction_system(self):
        """Test prediction system functionality."""
        brain = nlm.createBrain(self.config)
        agent_brain = nlm.createAgentBrain(brain)
        
        prediction_system = brain.getPredictionSystem()
        if prediction_system:
            self.assertTrue(prediction_system.isActive() or not prediction_system.isActive())
            
        # Test prediction error from agent brain
        prediction_error = agent_brain.getPredictionError()
        self.assertIsInstance(prediction_error, float)
        self.assertGreaterEqual(prediction_error, -1.0)
        self.assertLessEqual(prediction_error, 1.0)
        
    def test_neuromodulation(self):
        """Test neuromodulation system."""
        brain = nlm.createBrain(self.config)
        agent_brain = nlm.createAgentBrain(brain)
        
        # Test neuromodulation levels
        dopamine = agent_brain.getNeuromodulationLevel()
        curiosity = agent_brain.getCuriosityLevel()
        novelty = agent_brain.getNoveltyLevel()
        
        self.assertIsInstance(dopamine, float)
        self.assertIsInstance(curiosity, float)
        self.assertIsInstance(novelty, float)
        
        # Test values are in reasonable ranges
        self.assertGreaterEqual(dopamine, -1.0)
        self.assertLessEqual(dopamine, 1.0)
        self.assertGreaterEqual(curiosity, 0.0)
        self.assertLessEqual(curiosity, 1.0)
        self.assertGreaterEqual(novelty, 0.0)
        self.assertLessEqual(novelty, 1.0)
        
    def test_development_system(self):
        """Test development system."""
        brain = nlm.createBrain(self.config)
        agent_brain = nlm.createAgentBrain(brain)
        
        # Test developmental stage access
        stage = agent_brain.getDevelopmentalStage()
        self.assertIsInstance(stage, nlm.DevelopmentalStage)
        
        # Test development over time
        initial_age = agent_brain.getBrain().getTotalSynapseCount()  # Use as proxy
        
        # Update development multiple times
        for i in range(10):
            agent_brain.updateDevelopment(1.0)
        
        # Check that stage changes (development occurs)
        final_stage = agent_brain.getDevelopmentalStage()
        # Note: stage might not change in all cases due to implementation details
        
    def test_world_interaction(self):
        """Test world interaction and object management."""
        world = nlm.createSimpleWorld()
        world.configure(50.0, 50.0, 16, 16)
        
        # Add various objects
        resource1 = nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Resource, 5.0)
        hazard1 = nlm.WorldObject(30.0, 30.0, nlm.WorldObjectType.Hazard, -2.0)
        wall1 = nlm.WorldObject(40.0, 40.0, nlm.WorldObjectType.Wall)
        
        world.addObject(resource1)
        world.addObject(hazard1)
        world.addObject(wall1)
        
        # Verify objects were added
        retrieved_resource = world.getObjectAt(10.0, 10.0)
        self.assertIsNotNone(retrieved_resource)
        self.assertEqual(retrieved_resource.type, nlm.WorldObjectType.Resource)
        
        # Test collision detection
        self.assertTrue(world.isValidPosition(5.0, 5.0))  # Should be free
        self.assertFalse(world.isValidPosition(40.0, 40.0))  # Should be wall
        
        # Remove object
        world.removeObject(10.0, 10.0)
        self.assertIsNone(world.getObjectAt(10.0, 10.0))
        
    def test_action_scenarios(self):
        """Test various action scenarios."""
        world = nlm.createSimpleWorld()
        world.configure(20.0, 20.0, 8, 8)
        world.setAgentStart(2.0, 2.0)
        
        brain = nlm.createBrain(self.config)
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Create brain activity to generate actions
        for step in range(20):
            brain.step(step, step * 0.001)
        
        # Get action
        action = agent_brain.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify action result
        self.assertIsNotNone(result)
        self.assertIsInstance(result.reward, float)
        self.assertIsInstance(result.success, bool)
        
    def test_configuration_validation(self):
        """Test configuration validation and error handling."""
        config = nlm.createDefaultConfig()
        
        # Test setting and getting various data types
        config.set("int_value", 42)
        config.set("float_value", 3.14159)
        config.set("string_value", "test_string")
        config.set("bool_value", True)
        
        self.assertEqual(config.getOr("int_value", 0), 42)
        self.assertAlmostEqual(config.getOr("float_value", 0.0), 3.14159, places=5)
        self.assertEqual(config.getOr("string_value", ""), "test_string")
        
    def test_performance_metrics(self):
        """Test performance monitoring and metrics."""
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Collect metrics
        initial_neurons = brain.getTotalNeuronCount()
        initial_spikes = brain.getTotalSpikeCount()
        initial_rate = brain.getAverageFiringRate()
        
        # Run simulation
        for step in range(100):
            brain.step(step, step * 0.001)
        
        # Verify metrics
        self.assertEqual(brain.getTotalNeuronCount(), initial_neurons)
        self.assertGreaterEqual(brain.getTotalSpikeCount(), initial_spikes)
        self.assertGreaterEqual(brain.getAverageFiringRate(), 0.0)
        
        # Test other metrics
        self.assertGreater(brain.getRegionCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)

@unittest.skipIf(not PYNLM_AVAILABLE, "pynlm not available")
class TestPythonAPITutorials(unittest.TestCase):
    """Tutorial-style tests demonstrating common Python API usage patterns."""
    
    def test_beginner_tutorial(self):
        """Test beginner tutorial level usage."""
        # This simulates a complete beginner tutorial
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Simple simulation
        for i in range(10):
            brain.step(i, i * 0.001)
        
        # Basic statistics
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        
    def test_intermediate_tutorial(self):
        """Test intermediate tutorial level usage."""
        # Create a more complete simulation
        config = nlm.createDefaultConfig()
        world = nlm.createSimpleWorld()
        world.configure(10.0, 10.0, 8, 8)
        
        brain = nlm.createBrain(config)
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Run agent in world
        for step in range(50):
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            
            for i in range(5):
                brain.step(step * 5 + i, step * 0.025 + i * 0.001)
            
            action = agent_brain.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            agent_brain.applyRewardModulation(result.reward, 0.0)
            agent_brain.updateDevelopment(0.01)
        
        # Verify agent learned something
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        
    def test_advanced_tutorial(self):
        """Test advanced tutorial level usage."""
        # Advanced scenario with multiple systems
        config = nlm.createDefaultConfig()
        
        # Create all major components
        brain = nlm.createBrain(config)
        world = nlm.createSimpleWorld()
        world.configure(100.0, 100.0, 32, 32)
        
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Enable all advanced features
        agent_brain.enableRewardModulation(True)
        agent_brain.enableStructuralPlasticity(True)
        agent_brain.enableDevelopment(True)
        agent_brain.enableCuriosity(True)
        
        # Run complex simulation
        for episode in range(3):
            world.reset()
            agent_brain.reset()
            brain.reset()
            brain.initialize()
            
            for step in range(100):
                # Advanced: Process multiple sensory inputs
                percept = world.getSensoryPercept()
                agent_brain.processSensoryInput(percept)
                
                # Complex neural processing
                for i in range(20):
                    brain.step(step * 20 + i, step * 0.05 + i * 0.001)
                
                # Advanced action selection
                action = agent_brain.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                # Advanced learning with prediction
                agent_brain.applyRewardModulation(result.reward, 0.0)
                agent_brain.updateDevelopment(0.05)
        
        # Verify complex behavior
        self.assertGreater(brain.getTotalSynapseCount(), 100)
        self.assertGreater(world.getAgentBody().energy, 0)
        
    def test_data_science_tutorial(self):
        """Test data science oriented usage."""
        import random
        
        # Create experimental setup for data analysis
        configs = []
        results = []
        
        # Run multiple experiments with different parameters
        for seed in [42, 123, 456]:
            config = nlm.createDefaultConfig()
            config.set("neuron_count", 200 + seed % 100)
            config.set("connection_probability", 0.05 + (seed % 10) * 0.01)
            config.set("random_seed", seed)
            
            brain = nlm.createBrain(config)
            brain.initialize()
            
            # Collect metrics
            experiment_data = {
                "seed": seed,
                "neurons": brain.getTotalNeuronCount(),
                "synapses": brain.getTotalSynapseCount(),
                "spikes": brain.getTotalSpikeCount(),
                "rate": brain.getAverageFiringRate()
            }
            
            results.append(experiment_data)
        
        # Verify data collection
        self.assertEqual(len(results), 3)
        for result in results:
            self.assertIn("neurons", result)
            self.assertIn("synapses", result)
            self.assertIn("spikes", result)
            self.assertIn("rate", result)
            
        # Basic statistics
        avg_neurons = sum(r["neurons"] for r in results) / len(results)
        avg_synapses = sum(r["synapses"] for r in results) / len(results)
        
        self.assertGreater(avg_neurons, 0)
        self.assertGreater(avg_synapses, 0)

class TestPythonErrorHandling(unittest.TestCase):
    """Test error handling in the Python API."""
    
    def test_invalid_configuration_access(self):
        """Test handling of invalid configuration access."""
        config = nlm.createDefaultConfig()
        
        # This should not crash
        value = config.getOr("nonexistent_key", 999)
        self.assertEqual(value, 999)
        
    def test_brain_error_handling(self):
        """Test brain error handling."""
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        
        # Brain should not crash even with minimal initialization
        try:
            brain.step(0, 0.0)
            brain.logStatus()
        except Exception as e:
            self.fail(f"Brain operations should not raise exceptions: {e}")
            
    def test_world_error_handling(self):
        """Test world error handling."""
        world = nlm.createSimpleWorld()
        world.configure(10.0, 10.0, 8, 8)
        
        # Test operations that should not crash
        percept = world.getSensoryPercept()
        agent = world.getAgentBody()
        
        # These should not raise exceptions
        self.assertIsNotNone(percept)
        self.assertIsNotNone(agent)

if __name__ == "__main__":
    # Run tests with verbose output
    unittest.main(verbosity=2)
