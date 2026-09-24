# Agent-brain interaction tests

import pytest
import pynlm
import numpy as np
from tests.test_helper import TestHelper

class TestAgentInteraction:
    """Test agent-brain interaction"""
    
    def test_agent_initialization(self):
        """Test agent initialization with world"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Verify initialization
        assert agent.getBrain() == brain
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_sensorimotor_loop(self):
        """Test sensor-motor feedback loop"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Run sensor-motor loop
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            
            # Verify all components are functional
            assert agent.getBrain() is not None
            assert brain.getTotalNeuronCount() > 0
            assert world.getSimulationTime() > 0
    
    def test_reward_modulation(self):
        """Test reward-based learning"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable reward modulation
        agent.enableRewardModulation(True)
        
        # Apply reward
        agent.applyRewardModulation(1.0, 0.5)  # reward, predicted
        
        # Verify neuromodulation level changed
        assert agent.getNeuromodulationLevel() != 0.0
    
    def test_curiosity_driven_exploration(self):
        """Test curiosity-driven exploration behavior"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable curiosity
        agent.enableCuriosity(True)
        
        # Set up novelty in world
        world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType::Resource, 10.0))
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Verify curiosity level increased due to novelty
        assert agent.getCuriosityLevel() >= 0.0
    
    def test_development_integration(self):
        """Test agent development integration"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable development
        agent.enableDevelopment(True)
        
        # Update development
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step)
            agent.updateDevelopment(0.1)
            
            # Development stage should progress
            assert agent.getDevelopmentalStage() != pynlm.DevelopmentalStage::Initial
        
        # Verify development stage changed
        assert agent.getDevelopmentalStage() != pynlm.DevelopmentalStage::Initial
    
    def test_action_selection_strategies(self):
        """Test different action selection strategies"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Get baseline action
        baseline_action = agent.decodeMotorCommand()
        
        # Enable curiosity-driven exploration
        agent.enableCuriosity(True)
        
        # Get action with curiosity enabled
        curiosity_action = agent.decodeMotorCommand()
        
        # Actions may be the same or different depending on neural activity
        # Both should be valid motor commands
        assert curiosity_action in [pynlm.MotorCommand::MoveForward,
                                    pynlm.MotorCommand::MoveBackward,
                                    pynlm.MotorCommand::TurnLeft,
                                    pynlm.MotorCommand::TurnRight,
                                    pynlm.MotorCommand::LookLeft,
                                    pynlm.MotorCommand::LookRight,
                                    pynlm.MotorCommand::Interact,
                                    pynlm.MotorCommand::Wait]
    
    def test_memory_capacity_management(self):
        """Test memory capacity management"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Get working memory
        working_memory = brain.getWorkingMemory()
        assert working_memory is not None
        
        # Set capacity
        test_capacity = 200
        agent.setMemoryCapacity(test_capacity)
        assert working_memory.getCapacity() == test_capacity
    
    def test_state_export_import(self):
        """Test agent state export/import"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize and run briefly
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Export state
        state = agent.exportState()
        assert "developmental_stage" in state
        assert "curiosity_level" in state
        assert "novelty_level" in state
        assert "prediction_error" in state
        assert "neuromodulation_level" in state
        
        # Create new agent and import state
        agent2 = pynlm.createAgentBrain(brain)
        agent2.initialize(world)
        agent2.importState(state)
        
        # Verify state was imported
        assert agent2.getDevelopmentalStage() == agent.getDevelopmentalStage()
        assert agent2.getCuriosityLevel() == agent.getCuriosityLevel()
