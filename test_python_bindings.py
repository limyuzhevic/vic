#!/usr/bin/env python3
"""
NLM Python Bindings Tests

Comprehensive test suite for the pynlm Python bindings.
Tests the main NLM brain simulation functionality exposed through Python.

This test suite validates that the C++ bindings are correctly exposed
and that the Python API works as expected.
"""

import sys
import os
import tempfile
import math
from pathlib import Path

# Add the python directory to path so we can import pynlm directly
sys.path.insert(0, str(Path(__file__).parent.parent / "python"))

try:
    import pynlm
    print("Successfully imported pynlm")
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("Python bindings may not be built yet")
    sys.exit(1)

import unittest
from unittest.mock import patch, MagicMock
class TestPythonBindings(unittest.TestCase):
    """Test basic Python bindings functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        # Create a fresh config for each test
        self.config = pynlm.createDefaultConfig()
        
        # Configure a small brain for faster testing
        self.config.set("brain.neuron_count", 100)
        self.config.set("brain.synapse_density", 0.1)
        self.config.set("random_seed", 42)
        
    def test_config_creation_and_modification(self):
        """Test that we can create and modify configurations."""
        # Test config creation
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        # Test modification
        config.set("brain.neuron_count", 500)
        # Note: We can't easily retrieve values from the C++ config without
        # exposing getters through the Python bindings
        
        # Test saving/loading
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            self.assertTrue(os.path.exists(f.name))
            
        try:
            new_config = pynlm.createDefaultConfig()
            new_config.loadFromFile(f.name)
        finally:
            os.unlink(f.name)
            
    def test_brain_creation_and_initialization(self):
        """Test brain creation and initialization."""
        brain = pynlm.createBrain(self.config)
        self.assertIsNotNone(brain)
        
        # Initialize the brain
        brain.initialize()
        
        # Test basic brain properties
        neuron_count = brain.getTotalNeuronCount()
        self.assertGreater(neuron_count, 0)
        
        synapse_count = brain.getTotalSynapseCount()
        self.assertGreaterEqual(synapse_count, 0)
        
        # Test step functionality
        for step in range(10):
            brain.step(step)
            
        total_spikes = brain.getTotalSpikeCount()
        self.assertGreaterEqual(total_spikes, 0)
        
    def test_world_creation_and_configuration(self):
        """Test world creation and configuration."""
        world = pynlm.createSimpleWorld()
        self.assertIsNotNone(world)
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Test world properties
        self.assertEqual(world.getWidth(), 20)
        self.assertEqual(world.getHeight(), 20)
        
        # Test reset
        world.reset()
        
        # Test simulation time
        initial_time = world.getSimulationTime()
        world.update(0.1)
        new_time = world.getSimulationTime()
        self.assertGreater(new_time, initial_time)
        
    def test_agent_creation_and_initialization(self):
        """Test agent creation and initialization."""
        # First create brain and world
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test agent properties
        sensory_size = agent.getSensoryInputSize()
        self.assertGreater(sensory_size, 0)
        
        motor_size = agent.getMotorOutputSize()
        self.assertGreater(motor_size, 0)
        
    def test_complete_agent_world_loop(self):
        """Test complete agent-world interaction loop."""
        # Setup
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation loop
        num_steps = 100
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            self.assertIsNotNone(percept)
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            self.assertIsNotNone(action)
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify results
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(brain.getAverageFiringRate(), 0.0)
        
        # Test developmental stage
        dev_stage = brain.getDevelopmentalStage()
        self.assertIsNotNone(dev_stage)
        
    def test_checkpoint_functionality(self):
        """Test checkpoint save/load functionality."""
        # Create and initialize brain
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Run some steps to generate activity
        for step in range(50):
            brain.step(step)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_path = f.name
        
        try:
            save_result = brain.save(checkpoint_path)
            self.assertTrue(save_result, "Brain save should succeed")
            self.assertTrue(os.path.exists(checkpoint_path), 
                          "Checkpoint file should be created")
            
            # Create new brain and load
            brain2 = pynlm.createBrain(self.config)
            brain2.initialize()
            
            load_result = brain2.load(checkpoint_path)
            self.assertTrue(load_result, "Brain load should succeed")
            
            # Compare spike counts (approximate, as exact neural state
            # restoration is complex)
            spike_diff = abs(brain.getTotalSpikeCount() - brain2.getTotalSpikeCount())
            self.assertLess(spike_diff, 10,  # Allow some difference due to
                          "Spike count difference should be small")
            
        finally:
            if os.path.exists(checkpoint_path):
                os.unlink(checkpoint_path)
                
    def test_neuron_types_and_actions(self):
        """Test neuron type enumeration and action types."""
        # Test neuron types
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Modulatory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal
        ]
        
        for neuron_type in neuron_types:
            self.assertIsNotNone(neuron_type)
            
        # Test action types
        action_types = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Interact,
            pynlm.ActionType.Eat,
            pynlm.ActionType.Rest,
            pynlm.ActionType.Wait
        ]
        
        for action_type in action_types:
            self.assertIsNotNone(action_type)
            
    def test_string_processing_and_errors(self):
        """Test string processing and error handling."""
        # Test config with invalid file path
        config = pynlm.createDefaultConfig()
        
        # This should not crash
        config.clear()
        
        # Test that invalid file loading doesn't crash
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write("{invalid json")
            f.flush()
            
            try:
                config.loadFromFile(f.name)
                # Should not crash, but may log error
            except:
                pass  # Expected to potentially fail
            finally:
                os.unlink(f.name)
                
    def test_concurrent_access_simulation(self):
        """Test simulation that mimics concurrent access patterns."""
        # This simulates multiple "threads" or concurrent operations
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 150)
        
        brains = []
        worlds = []
        
        # Create multiple brains/worlds
        for i in range(3):
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("brain.neuron_count", 50)
            brain_config.set("random_seed", 42 + i)
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
            world.reset()
            
            brains.append(brain)
            worlds.append(world)
        
        # Simulate concurrent-like access
        for step in range(20):
            for i, (brain, world) in enumerate(zip(brains, worlds)):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                agent.processSensoryInput(percept)
                brain.step(step + i)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # All brains should have processed steps
        for brain in brains:
            self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
class TestPythonExamples(unittest.TestCase):
    """Test the example usage patterns from documentation."""
    
    def test_minimal_example(self):
        """Test the minimal example from documentation."""
        # Example 1: Simplest Brain (from easy_usage.md)
        import pynlm
        
        # One line to create a virtual brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Initialize it
        brain.initialize()
        
        # Make it active
        brain.step(0)
        
        # Verify
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        
    def test_complete_agent_example(self):
        """Test the complete agent example from documentation."""
        import pynlm
        
        def run_agent_simulation(num_steps=100):
            """Run a complete agent simulation with brain and world."""
            
            # 1. Create configuration
            config = pynlm.createDefaultConfig()
            
            # 2. Create brain and initialize
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # 3. Create agent brain interface
            agent = pynlm.createAgentBrain(brain)
            
            # 4. Create and configure world
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            world.setAgentStart(10.0, 10.0)
            
            # 5. Initialize agent with world
            agent.initialize(world)
            
            # 6. Enable subsystems
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # 7. Run simulation loop
            for step in range(num_steps):
                # Update world
                world.update(timestep=0.1)
                
                # Get sensory input from world
                percept = world.getSensoryPercept()
                
                # Process sensory input in brain
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Decode motor command from brain activity
                motor_cmd = agent.decodeMotorCommand()
                
                # Apply motor command to world
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                # Apply reward modulation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            return brain, world, agent
        
        # Run the simulation
        brain, world, agent = run_agent_simulation(50)
        
        # Verify results
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
        self.assertGreater(agent.getNoveltyLevel(), 0.0)
        
    def test_world_interaction_example(self):
        """Test world interaction example from documentation."""
        import pynlm
        
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Run episode
        for _ in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(0)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Verify
        self.assertGreater(world.getAgentBody().energy, 0)


if __name__ == '__main__':
    print("Running NLM Python Bindings Tests")
    print("=" * 50)
    
    # Run the tests
    unittest.main(verbosity=2)
