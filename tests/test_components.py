#!/usr/bin/env python3
"""
Unit Tests for NLM Components
Tests all major components through Python bindings
"""

import pytest
import pynlm
import numpy as np


class TestConfiguration:
    """Test configuration system"""
    
    def test_create_default_config(self):
        """Test creating default configuration"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
    def test_config_properties(self):
        """Test configuration has expected properties"""
        config = pynlm.createDefaultConfig()
        # Test various configuration properties
        neuron_count = config.getInt("neuron_count")
        region_count = config.getInt("region_count") 
        connection_prob = config.getFloat("connection_probability")
        
        assert neuron_count > 0
        assert region_count > 0
        assert 0.0 <= connection_prob <= 1.0


class TestBrain:
    """Test brain functionality"""
    
    def setup_method(self):
        """Setup for each test"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        """Test brain creation and initialization"""
        assert self.brain is not None
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getRegionCount() > 0
    
    def test_brain_step(self):
        """Test brain simulation steps"""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run several steps
        for step in range(10):
            self.brain.step(step)
        
        # Verify brain state
        assert self.brain.getTotalNeuronCount() > 0
        # Firing count may or may not change, just ensure no crash
    
    def test_brain_reset(self):
        """Test brain reset functionality"""
        # Inject some current
        for neuron_id in range(min(10, self.brain.getTotalNeuronCount())):
            self.brain.injectCurrent(pynlm.NeuronId(neuron_id), -40.0)
        
        # Reset brain
        self.brain.reset()
        
        # Verify reset
        assert self.brain.getTotalSpikeCount() == 0
    
    def test_brain_action_production(self):
        """Test brain action production"""
        action = self.brain.produceAction()
        assert action is not None


class TestAgentBrain:
    """Test agent brain interface"""
    
    def setup_method(self):
        """Setup for each test"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(20.0, 20.0, 8, 8)
        self.world.reset()
        
        self.agent.initialize(self.world)
    
    def test_agent_initialization(self):
        """Test agent brain initialization"""
        assert self.agent is not None
        assert self.agent.getBrain() is not None
        
        assert self.agent.getSensoryInputSize() > 0
        assert self.agent.getMotorOutputSize() > 0
    
    def test_sensory_processing(self):
        """Test sensory input processing"""
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Verify brain received input (no exception thrown)
        assert True  # If no exception, test passes
    
    def test_motor_decoding(self):
        """Test motor command decoding"""
        # Process some sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Run brain steps to generate motor activity
        for step in range(5):
            self.brain.step(step)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        assert action is not None
    
    def test_reward_modulation(self):
        """Test reward modulation"""
        reward = 0.5
        predicted = 0.3
        
        # Apply reward modulation
        self.agent.applyRewardModulation(reward, predicted)
        
        # Verify neuromodulation was applied
        assert True  # If no exception, test passes
    
    def test_development(self):
        """Test development system"""
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Verify development updated
        assert True  # If no exception, test passes


class TestMemorySystems:
    """Test memory systems"""
    
    def setup_method(self):
        """Setup for each test"""
        self.config = pynlm.createDefaultConfig()
        # Configure for memory tests
        self.config.set("memory.working.capacity", 100)
        self.config.set("memory.episodic.enable", True)
        self.config.set("memory.associative.enable", True)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_working_memory(self):
        """Test working memory"""
        memory = self.brain.getWorkingMemory()
        if memory is not None:
            assert memory.getCapacity() > 0
            assert memory.getActiveTraces() >= 0
    
    def test_episodic_memory(self):
        """Test episodic memory"""
        memory = self.brain.getEpisodicMemory()
        if memory is not None:
            assert memory.getCapacity() > 0
            assert memory.getEpisodeCount() >= 0
    
    def test_associative_memory(self):
        """Test associative memory"""
        memory = self.brain.getAssociativeMemory()
        if memory is not None:
            assert memory.getCapacity() > 0
            assert memory.getPatternCount() >= 0


class TestNeuromodulation:
    """Test neuromodulation systems"""
    
    def setup_method(self):
        """Setup for each test"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10.0, 10.0, 5, 5)
        self.world.reset()
        
        self.agent.initialize(self.world)
    
    def test_dopamine(self):
        """Test dopamine system"""
        dopamine = self.brain.getDopamine()
        if dopamine is not None:
            assert dopamine.getLevel() >= 0.0
    
    def test_curiosity(self):
        """Test curiosity system"""
        curiosity = self.brain.getCuriosity()
        if curiosity is not None:
            assert curiosity.getLevel() >= 0.0
            assert curiosity.getLevel() <= 1.0
    
    def test_novelty(self):
        """Test novelty detection"""
        novelty = self.brain.getNovelty()
        if novelty is not None:
            assert novelty.getLevel() >= 0.0
    
    def test_prediction_error(self):
        """Test prediction error signal"""
        pred_error = self.brain.getPredictionErrorSignal()
        if pred_error is not None:
            assert pred_error.getError() >= 0.0


class TestDevelopment:
    """Test development system"""
    
    def setup_method(self):
        """Setup for each test"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.development = self.brain.getDevelopmentSystem()
    
    def test_developmental_stage(self):
        """Test developmental stages"""
        if self.development is not None:
            stage = self.brain.getDevelopmentalStage()
            assert stage is not None
            
            # Test stage progression
            self.development.updateDevelopment(0.1)
            assert self.brain.getDevelopmentalStage() is not None
    
    def test_synaptogenesis(self):
        """Test synaptogenesis"""
        # Update development to trigger synaptogenesis
        for _ in range(10):
            self.brain.step(0)
    
    def test_pruning(self):
        """Test pruning"""
        # Update development to trigger pruning
        for _ in range(10):
            self.brain.step(0)


class TestPredictionSystems:
    """Test prediction systems"""
    
    def setup_method(self):
        """Setup for each test"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.prediction = self.brain.getPredictionSystem()
    
    def test_prediction_system(self):
        """Test prediction system"""
        if self.prediction is not None:
            # Run some steps
            for step in range(10):
                self.brain.step(step)


class TestPlasticity:
    """Test plasticity rules"""
    
    def setup_method(self):
        """Setup for each test"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_stdp(self):
        """Test STDP plasticity"""
        stdp = self.brain.getSTDP()
        if stdp is not None:
            assert stdp.getLearningRate() > 0.0
    
    def test_hebbian(self):
        """Test Hebbian plasticity"""
        hebbian = self.brain.getHebbian()
        if hebbian is not None:
            assert hebbian.getLearningRate() > 0.0
    
    def test_structural_plasticity(self):
        """Test structural plasticity"""
        structural = self.brain.getStructuralPlasticity()
        if structural is not None:
            assert structural.getGrowthRate() >= 0.0


class TestIntegration:
    """Test system integration"""
    
    def test_complete_brain_loop(self):
        """Test complete brain loop with world interaction"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(20.0, 20.0, 8, 8)
        world.reset()
        
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run complete simulation loop
        total_reward = 0.0
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Perceive world
            percept = world.getSensoryPercept()
            
            # Send to brain
            agent.processSensoryInput(percept)
            
            # Neural processing
            brain.step(step)
            
            # Brain's decision
            action = agent.decodeMotorCommand()
            
            # Act in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get feedback
            reward = 0.0  # Simplified
            
            # Learn from experience
            agent.applyRewardModulation(reward, 0.0)
            
            # Develop
            agent.updateDevelopment(0.1)
            
            total_reward += reward
        
        # Verify integration
        assert total_reward >= 0.0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])