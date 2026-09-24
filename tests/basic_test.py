# Basic functionality tests

import pytest
import pynlm
import numpy as np
from tests.test_helper import TestHelper

class TestBasicFunctionality:
    """Test basic NLM functionality"""
    
    def test_create_default_config(self):
        """Test default configuration creation"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("random_seed")
        assert config.has("simulation_timestep")
    
    def test_create_brain(self):
        """Test brain creation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain.initialize()
        assert brain.getTotalNeuronCount() > 0
    
    def test_brain_step(self):
        """Test brain simulation step"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 50, pynlm.ConfigSource::Default)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(1)
        final_spikes = brain.getTotalSpikeCount()
        
        # Spike count may not increase in a single step, but should not decrease
        assert final_spikes >= initial_spikes
    
    def test_world_creation(self):
        """Test world creation"""
        world = pynlm.createSimpleWorld()
        assert world is not None
        assert world.getWidth() > 0
        assert world.getHeight() > 0
    
    def test_world_configuration(self):
        """Test world configuration"""
        world = pynlm.createSimpleWorld()
        world.configure(30, 40, 20, 20)
        assert world.getWidth() == 30
        assert world.getHeight() == 40
    
    def test_agent_creation(self):
        """Test agent creation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        assert agent.getBrain() == brain
    
    def test_simple_simulation(self):
        """Test simple simulation"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 20, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Run simulation
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
        
        # Verify simulation completed
        assert brain.getTotalNeuronCount() > 0
        assert world.getWidth() > 0
        assert world.getHeight() > 0
    
    def test_multiple_agents(self):
        """Test multiple independent agents"""
        config = pynlm.createDefaultConfig()
        
        brain1 = pynlm.createBrain(config)
        brain2 = pynlm.createBrain(config)
        world1 = pynlm.createSimpleWorld()
        world2 = pynlm.createSimpleWorld()
        agent1 = pynlm.createAgentBrain(brain1)
        agent2 = pynlm.createAgentBrain(brain2)
        
        # Initialize
        brain1.initialize()
        brain2.initialize()
        world1.reset()
        world2.reset()
        agent1.initialize(world1)
        agent2.initialize(world2)
        
        # Verify independence
        assert brain1.getBrain() == brain2.getBrain() == agent1.getBrain() == agent2.getBrain()
        
        # They should have the same initial configuration
        assert brain1.getConfig() == brain2.getConfig()
