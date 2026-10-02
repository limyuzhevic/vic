"""
Python Test Suite for NLM Bindings

This module provides comprehensive tests for the NLM Python bindings,
enabling automated testing of the Python API functionality.
"""

import unittest
import tempfile
import os
import sys
import pynlm

class TestNLMConfig(unittest.TestCase):
    """Test configuration management functionality."""
    
    def test_default_config_creation(self):
        """Test creating default configuration."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        # Test some default values
        self.assertTrue(config.has("random_seed"))
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("connection_probability"))
    
    def test_config_get_set(self):
        """Test getting and setting configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Set some values
        config.set("neuron_count", 2000)
        config.set("learning_rate", 0.01)
        config.set("enable_curiosity", True)
        config.set("enable_development", True)
        
        # Verify they were set
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("learning_rate"))
        
        # Test getting values (note: these might not have exact get methods,
        # but we test the general functionality)
        keys = config.getKeys()
        self.assertIn("neuron_count", keys)
        self.assertIn("learning_rate", keys)
    
    def test_config_save_load(self):
        """Test saving and loading configuration files."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1500)
        config.set("plasticity.enable", True)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            temp_file = f.name
        
        try:
            # Save configuration
            success = config.saveToFile(temp_file)
            self.assertTrue(success)
            
            # Load configuration
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromFile(temp_file)
            self.assertTrue(success)
            
            # Verify basic properties are preserved
            self.assertTrue(config2.has("neuron_count"))
            
        finally:
            # Clean up
            if os.path.exists(temp_file):
                os.unlink(temp_file)

class TestNLMBrain(unittest.TestCase):
    """Test brain creation and basic functionality."""
    
    def test_brain_creation(self):
        """Test basic brain creation and initialization."""
        config = pynlm.createDefaultConfig()
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify brain properties
        self.assertEqual(brain.getTotalNeuronCount(), 1000)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertEqual(brain.getRegionCount(), 2)
        
        # Test basic simulation
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        self.assertGreaterEqual(brain.getTotalSpikeCount(), initial_spikes)
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        # Reset brain
        brain.reset()
        
        # Verify reset
        self.assertEqual(brain.getTotalNeuronCount(), 1000)
        # Other properties should be reset

class TestNLMWorld(unittest.TestCase):
    """Test world creation and interaction."""
    
    def test_simple_world_creation(self):
        """Test simple world creation and configuration."""
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(
            width=20,
            height=20,
            visionWidth=8,
            visionHeight=8
        )
        
        # Reset world
        world.reset()
        
        # Verify configuration
        self.assertEqual(world.getWidth(), 20)
        self.assertEqual(world.getHeight(), 20)
        self.assertEqual(world.getVisionWidth(), 8)
        self.assertEqual(world.getVisionHeight(), 8)
        
        # Set agent position
        world.setAgentStart(10.0, 10.0)
        
        # Update world
        world.update(0.1)
        
        # Get agent position
        body = world.getAgentBody()
        self.assertIsNotNone(body)
    
    def test_world_objects(self):
        """Test adding and removing world objects."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        # Add an object
        object_id = world.addObject(pynlm.WorldObjectType.Resource, value=1.0)
        self.assertIsNotNone(object_id)
        
        # Check if position is valid
        self.assertTrue(world.isValidPosition(5.0, 5.0))
        
        # Remove object
        world.removeObject(5.0, 5.0)
        
        # Verify removal (object should no longer be at that position)
        self.assertFalse(world.isValidPosition(5.0, 5.0))

class TestNLMAgent(unittest.TestCase):
    """Test agent creation and coordination."""
    
    def test_agent_brain_creation(self):
        """Test creating agent-brain interface."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        
        self.assertIsNotNone(agent)
        self.assertEqual(agent.getBrain(), brain)
    
    def test_agent_initialization(self):
        """Test agent initialization with world."""
        # Create brain and agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Initialize agent
        agent.initialize(world)
        
        # Verify agent properties
        self.assertGreater(agent.getSensoryInputSize(), 0)
        self.assertGreater(agent.getMotorOutputSize(), 0)
        
        # Test enable/disable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        self.assertTrue(agent.isRewardModulationEnabled())
        self.assertTrue(agent.isCuriosityEnabled())
        self.assertTrue(agent.isDevelopmentEnabled())
    
    def test_agent_sensory_processing(self):
        """Test sensory processing."""
        # Setup agent system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        brain.initialize()
        
        # Create world and initialize agent
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        agent.initialize(world)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Verify processing occurred (no errors is good)
        self.assertIsNotNone(percept)
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Setup agent system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        brain.initialize()
        
        # Create world and initialize agent
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        agent.initialize(world)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Verify action is valid (could be any MotorCommand enum value)
        self.assertIsNotNone(action)
    
    def test_agent_reward_modulation(self):
        """Test reward modulation and learning."""
        # Setup agent system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        brain.initialize()
        
        # Create world and initialize agent
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        agent.initialize(world)
        
        # Apply reward modulation
        agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)
        
        # Verify neuromodulation effects
        self.assertGreater(agent.getNeuromodulationLevel(), -1.0)
        self.assertLessEqual(agent.getNeuromodulationLevel(), 1.0)
        
        # Test prediction error
        self.assertLessEqual(agent.getPredictionError(), 1.0)

class TestNLMIntegration(unittest.TestCase):
    """Test integration of complete agent system."""
    
    def test_complete_agent_simulation(self):
        """Test running a complete agent simulation."""
        def run_agent_simulation():
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
            for step in range(100):
                # Update world
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Decode motor command
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            return brain, agent, world
        
        # Run the simulation
        brain, agent, world = run_agent_simulation()
        
        # Verify results
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0)
        self.assertGreater(agent.getNoveltyLevel(), 0)
        
        # Verify world state
        body = world.getAgentBody()
        self.assertIsNotNone(body)
        self.assertGreater(body.x(), 0)
        self.assertGreater(body.y(), 0)

class TestNLMPerformance(unittest.TestCase):
    """Test performance characteristics."""
    
    def test_brain_performance(self):
        """Test basic brain performance metrics."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure time to initialize
        import time
        start_time = time.time()
        brain.step(0)
        step_time = time.time() - start_time
        
        # Brain should process steps quickly
        self.assertLess(step_time, 0.1, "Brain step too slow")
        
        # Measure basic metrics
        self.assertGreaterEqual(brain.getAverageFiringRate(), 0)
        self.assertGreaterEqual(brain.getExcitationInhibitionRatio(), 0)

if __name__ == '__main__':
    # Run tests with verbose output
    unittest.main(verbosity=2)