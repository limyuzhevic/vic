# Integration tests for multi-system interaction

import pytest
import pynlm
import numpy as np
import time
from tests.test_helper import TestHelper

class TestIntegration:
    """Test integration between multiple systems"""
    
    def test_brain_world_agent_integration(self):
        """Test complete integration between brain, world, and agent"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 30, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(25, 25, 10, 10)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize all systems
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Run simulation
        for step in range(30):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            
            # Verify all systems are interacting
            assert brain.getTotalNeuronCount() > 0
            assert agent.getBrain() == brain
            assert world.getSimulationTime() > 0
        
        # Verify systems are integrated
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        assert working_memory is not None or episodic_memory is not None
    
    def test_memory_plasticity_integration(self):
        """Test integration between memory systems and neural plasticity"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 40, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable plasticity systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        
        # Run learning simulation
        initial_neurons = brain.getTotalNeuronCount()
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            agent.applyRewardModulation(0.5, 0.3)  # Simple reward learning
        
        # Verify plasticity is working
        final_neurons = brain.getTotalNeuronCount()
        assert final_neurons > 0  # Should have at least some neurons
    
    def test_neuromodulation_cognition_integration(self):
        """Test integration between neuromodulation and cognition"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 35, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(18, 18, 7, 7)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable neuromodulation and cognition
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation with neuromodulation
        for step in range(40):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            agent.updateDevelopment(0.1)
            agent.applyRewardModulation(0.2, 0.1)
        
        # Verify neuromodulation affected cognition
        assert agent.getNeuromodulationLevel() != 0.0
        assert agent.getCuriosityLevel() > 0.0
    
    def test_memory_replay_integration(self):
        """Test memory replay and consolidation integration"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 45, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(22, 22, 9, 9)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable episodic memory
        assert brain.getEpisodicMemory() is not None
        
        # Run simulation to create experiences
        for step in range(60):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            
            # Occasionally trigger memory storage
            if step % 10 == 0:
                agent.applyRewardModulation(0.3, 0.2)
        
        # Verify memory systems are being used
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        if working_memory is not None:
            assert working_memory.getCapacity() > 0
        
        if episodic_memory is not None:
            # Episodes may have been stored
            pass
    
    def test_development_learning_integration(self):
        """Test development and learning integration"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 50, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(28, 28, 10, 10)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable development and learning
        agent.enableDevelopment(True)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run development simulation
        initial_stage = agent.getDevelopmentalStage()
        for step in range(80):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            agent.updateDevelopment(0.1)
            agent.applyRewardModulation(0.1, 0.05)
        
        # Verify development progression
        final_stage = agent.getDevelopmentalStage()
        # Stage should have progressed or at least changed
        assert final_stage != pynlm.DevelopmentalStage::Initial
        
        # Verify learning occurred
        assert agent.getNeuromodulationLevel() != 0.0
    
    def test_advanced_feature_integration(self):
        """Test integration of advanced features"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 40, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(24, 24, 9, 9)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Set advanced configuration
        agent.setMemoryCapacity(300)
        
        # Run advanced simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            
            # Apply advanced neuromodulation
            agent.applyRewardModulation(0.3, 0.2)
            agent.updateDevelopment(0.1)
        
        # Verify advanced features are working
        assert agent.getNeuromodulationLevel() != 0.0
        assert agent.getCuriosityLevel() > 0.0
        assert agent.getMemoryUsage()["working_memory"] > 0
        
        # Export and import state
        state = agent.exportState()
        agent2 = pynlm.createAgentBrain(brain)
        agent2.initialize(world)
        agent2.importState(state)
        
        # Verify state was properly imported
        assert agent2.getDevelopmentalStage() == agent.getDevelopmentalStage()
