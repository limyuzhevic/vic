"""
Agent NLM Python tests for agent brain interface.

Tests for agent brain creation, initialization, and behavior control.
"""

import pytest
import pynlm


class TestAgentBrainCreation:
    """Test agent brain creation and initialization."""
    
    def test_agent_brain_creation(self):
        """Test basic agent brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Check that agent has reference to brain
        assert agent.getBrain() is not None
        assert agent.getBrain() == brain
    
    def test_agent_brain_with_world(self):
        """Test agent brain initialization with world."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Verify agent is initialized with world
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_agent_brain_behavior_settings(self):
        """Test agent behavior settings."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable different behaviors
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Check behavior profile
        profile = agent.getBehaviorProfile()
        assert profile["rewardModulation"] == True
        assert profile["curiosity"] == True
        assert profile["development"] == True
        assert profile["structuralPlasticity"] == True


class TestAgentSensoryProcessing:
    """Test agent sensory processing functionality."""
    
    def test_process_sensory_input(self):
        """Test sensory input processing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Agent should still be functional
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_sensory_percept_properties(self):
        """Test sensory percept properties."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Check percept properties
        if percept.hasVision():
            assert percept.getVisionWidth() == 8
            assert percept.getVisionHeight() == 8
            assert percept.getVision().getChannels() == 3
        
        assert percept.getSignalCount() >= 0


class TestAgentMotorDecoding:
    """Test agent motor decoding functionality."""
    
    def test_decode_motor_command(self):
        """Test motor command decoding."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain simulation
        brain.step(0)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Check action properties
        assert action is not None
        assert action.getType() is not None
        assert action.getName() is not None
        assert action.isValid() == True
    
    def test_motor_command_types(self):
        """Test different motor command types."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Process sensory input and step brain
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(0)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Check that action is valid
        assert action.getType() is not None
        assert action.getName() != ""
        assert action.isValid() == True


class TestAgentNeuromodulation:
    """Test agent neuromodulation functionality."""
    
    def test_apply_reward_modulation(self):
        """Test reward modulation application."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)  # reward=1.0, predicted=0.5
        
        # Check neuromodulation level
        neuromodulation_level = agent.getNeuromodulationLevel()
        assert neuromodulation_level >= 0.0
    
    def test_apply_reward_modulation_with_different_values(self):
        """Test reward modulation with different values."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test different reward values
        test_rewards = [0.0, 0.5, 1.0, 2.0, -1.0]
        test_predicted = [0.0, 0.3, 0.8, 0.5, 0.2]
        
        for reward, predicted in zip(test_rewards, test_predicted):
            agent.applyRewardModulation(reward, predicted)
            neuromodulation_level = agent.getNeuromodulationLevel()
            assert neuromodulation_level >= 0.0


class TestAgentDevelopment:
    """Test agent development functionality."""
    
    def test_update_development(self):
        """Test development system update."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Check developmental stage
        stage = agent.getDevelopmentalStage()
        assert stage is not None
    
    def test_developmental_stage_progression(self):
        """Test developmental stage progression."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get initial developmental stage
        initial_stage = agent.getDevelopmentalStage()
        
        # Update development multiple times
        for i in range(10):
            agent.updateDevelopment(0.1)
            new_stage = agent.getDevelopmentalStage()
            # Stage should not regress
            assert new_stage >= initial_stage or initial_stage is None
    
    def test_developmental_stage_scenarios(self):
        """Test different developmental stage scenarios."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test different development enable/disable scenarios
        agent.enableDevelopment(True)
        agent.updateDevelopment(0.1)
        
        agent.enableDevelopment(False)
        agent.updateDevelopment(0.1)
        
        # Development should still work even when disabled
        stage = agent.getDevelopmentalStage()
        assert stage is not None


class TestAgentBehaviorManagement:
    """Test agent behavior management functionality."""
    
    def test_behavior_profile(self):
        """Test behavior profile management."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get initial behavior profile
        profile = agent.getBehaviorProfile()
        assert "rewardModulation" in profile
        assert "curiosity" in profile
        assert "development" in profile
        assert "structuralPlasticity" in profile
        
        # Modify behaviors
        agent.enableRewardModulation(False)
        agent.enableCuriosity(False)
        agent.enableDevelopment(False)
        agent.enableStructuralPlasticity(False)
        
        # Get updated behavior profile
        profile = agent.getBehaviorProfile()
        assert profile["rewardModulation"] == False
        assert profile["curiosity"] == False
        assert profile["development"] == False
        assert profile["structuralPlasticity"] == False
    
    def test_action_selection_mode(self):
        """Test action selection mode setting."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test different action selection modes
        agent.setActionSelectionMode("exploration")
        profile = agent.getBehaviorProfile()
        assert profile["curiosity"] == True
        
        agent.setActionSelectionMode("exploitation")
        profile = agent.getBehaviorProfile()
        assert profile["curiosity"] == False
        
        agent.setActionSelectionMode("balanced")
        profile = agent.getBehaviorProfile()
        assert profile["curiosity"] == True
        assert profile["rewardModulation"] == True


class TestAgentCompleteWorkflow:
    """Test complete agent workflow."""
    
    def test_complete_agent_workflow(self):
        """Test complete agent workflow with brain and world."""
        # Setup complete configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Setup world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Setup agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run complete workflow
        steps = 20
        successful_steps = 0
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinking
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            successful_steps += 1
        
        # Verify workflow completion
        assert successful_steps == steps
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert agent.getNeuromodulationLevel() >= 0.0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert agent.getPredictionError() >= 0.0
    
    def test_agent_world_interaction(self):
        """Test agent-world interaction."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get initial world state
        initial_percept = world.getSensoryPercept()
        initial_body = world.getAgentBody()
        
        # Process sensory input and get action
        agent.processSensoryInput(initial_percept)
        brain.step(0)
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get new world state
        new_percept = world.getSensoryPercept()
        new_body = world.getAgentBody()
        
        # Check that interaction occurred
        assert new_percept is not None
        assert new_body is not None
        
        # Update world and check changes
        world.update(0.1)
        assert world.getSimulationTime() > 0
        
        # Agent should still be functional
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
