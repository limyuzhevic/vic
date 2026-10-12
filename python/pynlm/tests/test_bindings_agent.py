#!/usr/bin/env python3
"""
Agent functionality test suite for NLM Python bindings.
Tests agent brain interface, sensory processing, and motor output.
"""

import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

class TestAgentBrainInitialization:
    """Test AgentBrain initialization and setup."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        assert brain.initialize()
        
        self.brain = brain
        self.world = nlm.create_test_environment()
        self.agent_brain = nlm.AgentBrain(brain)
        assert self.agent_brain.initialize(self.world)
    
    def test_agent_brain_creation(self):
        """Test creating AgentBrain with brain."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        assert brain.initialize()
        
        agent_brain = nlm.AgentBrain(brain)
        assert agent_brain is not None
        assert isinstance(agent_brain, nlm.AgentBrain)
    
    def test_agent_brain_initialization(self):
        """Test AgentBrain initialization with world."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        assert brain.initialize()
        
        agent_brain = nlm.AgentBrain(brain)
        assert agent_brain.initialize(self.world)
        
        # Verify initialization worked
        assert agent_brain.getSensoryInputSize() > 0
        assert agent_brain.getMotorOutputSize() > 0

class TestSensoryProcessing:
    """Test sensory processing and perception."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        assert brain.initialize()
        
        self.brain = brain
        self.world = nlm.create_test_environment()
        self.agent_brain = nlm.AgentBrain(brain)
        assert self.agent_brain.initialize(self.world)
    
    def test_vision_perception(self):
        """Test vision sensory processing."""
        # Create vision data (simulated 2D image)
        vision_data = []
        width = 32
        height = 32
        channels = 3
        
        # Create test image with some patterns
        for y in range(height):
            for x in range(width):
                for c in range(channels):
                    # Create simple gradient pattern
                    value = ((x + y) % 10) / 10.0
                    vision_data.append(value)
        
        # Set vision in world
        self.world.configure_simulation(100.0, 100.0, width, height)
        
        # The world would normally generate vision, but we'll test the interface
        percept = self.world.getSensoryPercept()
        assert percept is not None
        assert percept.getVisionWidth() == width
        assert percept.getVisionHeight() == height
    
    def test_audio_perception(self):
        """Test audio sensory processing."""
        # Create audio data (simulated sound)
        audio_data = []
        sample_rate = 44100
        num_samples = 1024
        
        for i in range(num_samples):
            # Create simple sine wave pattern
            frequency = 440.0  # A4 note
            amplitude = 0.5
            time = i / float(sample_rate)
            value = amplitude * (1.0 if i % 100 == 0 else 0.0)  # Simple pulses
            audio_data.append(value)
        
        # Test audio data structure
        audio = nlm.Audio(sample_rate, num_samples)
        assert audio.getSampleRate() == sample_rate
        assert audio.getNumSamples() == num_samples
        assert audio.getType() == nlm.SensoryInput
    
    def test_internal_signals(self):
        """Test internal signals processing."""
        internal = nlm.InternalSignals()
        
        # Add various signals
        internal.addSignal(0.1)   # Low activity
        internal.addSignal(0.5)   # Medium activity
        internal.addSignal(0.9)   # High activity
        internal.addSignal(0.0)   # No activity
        
        signals = internal.getData()
        assert len(signals) == 4
        assert signals[0] == 0.1
        assert signals[1] == 0.5
        assert signals[2] == 0.9
        assert signals[3] == 0.0
        
        # Clear and verify
        internal.clearSignals()
        assert len(internal.getData()) == 0
    
    def test_sensory_percept(self):
        """Test sensory percept data structure."""
        percept = nlm.SensoryPercept()
        
        # Set different sensory modalities
        vision_data = [0.1] * 100
        touch_data = [0.5] * 50
        internal_data = [0.8] * 20
        proprioception_data = [0.3] * 10
        audio_data = [0.7] * 30
        
        percept.setVision(vision_data)
        percept.setTouch(touch_data)
        percept.setInternal(internal_data)
        percept.setProprioception(proprioception_data)
        percept.setAudio(audio_data)
        
        # Verify data retrieval
        assert percept.getVision() == vision_data
        assert percept.getTouch() == touch_data
        assert percept.getInternal() == internal_data
        assert percept.getProprioception() == proprioception_data
        assert percept.getAudio() == audio_data
        
        # Verify dimensions
        assert percept.getVisionWidth() == int(len(vision_data) ** 0.5)
        assert percept.getVisionHeight() == int(len(vision_data) ** 0.5)
        
        # Test timestamp
        percept.setTimestamp(1234567890)
        assert percept.getTimestamp() == 1234567890

class TestMotorOutput:
    """Test motor output and action production."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        assert brain.initialize()
        
        self.brain = brain
        self.world = nlm.create_test_environment()
        self.agent_brain = nlm.AgentBrain(brain)
        assert self.agent_brain.initialize(self.world)
    
    def test_action_creation(self):
        """Test action creation and manipulation."""
        # Create different types of actions
        move_forward = nlm.Action(nlm.ActionType.MoveForward)
        assert move_forward.getType() == nlm.ActionType.MoveForward
        
        turn_left = nlm.Action(nlm.ActionType.TurnLeft)
        assert turn_left.getType() == nlm.ActionType.TurnLeft
        
        look = nlm.Action(nlm.ActionType.Look)
        assert look.getType() == nlm.ActionType.Look
        
        # Set parameters
        action = nlm.Action(nlm.ActionType.Custom)
        action.setParameters([1.0, 2.0, 3.0])
        params = action.getParameters()
        assert len(params) == 3
        assert params[0] == 1.0
        
        # Test action name
        assert action.getName() != ""
    
    def test_motor_command_mapping(self):
        """Test motor command enumeration."""
        # Test all motor command types
        commands = [
            nlm.MotorCommand.MoveForward,
            nlm.MotorCommand.MoveBackward,
            nlm.MotorCommand.TurnLeft,
            nlm.MotorCommand.TurnRight,
            nlm.MotorCommand.LookLeft,
            nlm.MotorCommand.LookRight,
            nlm.MotorCommand.Interact,
            nlm.MotorCommand.Wait
        ]
        
        assert len(commands) == 8
        
        # Verify each command has unique value
        unique_values = set(cmd.value for cmd in commands if hasattr(cmd, 'value'))
        assert len(unique_values) == len(commands)

class TestNeuromodulation:
    """Test neuromodulation systems."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        assert brain.initialize()
        
        self.brain = brain
        self.world = nlm.create_test_environment()
        self.agent_brain = nlm.AgentBrain(brain)
        assert self.agent_brain.initialize(self.world)
    
    def test_neuromodulation_levels(self):
        """Test neuromodulation level getters."""
        # Verify methods exist
        assert hasattr(self.agent_brain, 'getNeuromodulationLevel')
        assert hasattr(self.agent_brain, 'getCuriosityLevel')
        assert hasattr(self.agent_brain, 'getNoveltyLevel')
        assert hasattr(self.agent_brain, 'getPredictionError')
        
        # Get levels (should be non-negative)
        neuromodulation_level = self.agent_brain.getNeuromodulationLevel()
        curiosity_level = self.agent_brain.getCuriosityLevel()
        novelty_level = self.agent_brain.getNoveltyLevel()
        prediction_error = self.agent_brain.getPredictionError()
        
        assert neuromodulation_level >= 0.0
        assert curiosity_level >= 0.0
        assert novelty_level >= 0.0
        assert prediction_error >= 0.0
    
    def test_neuromodulation_control(self):
        """Test neuromodulation control flags."""
        brain = self.agent_brain
        
        # Test initial state
        assert brain.isRewardModulationEnabled() == True  # Default enabled
        assert brain.isStructuralPlasticityEnabled() == True
        assert brain.isDevelopmentEnabled() == True
        assert brain.isCuriosityEnabled() == True
        
        # Test toggling
        brain.enableRewardModulation(False)
        assert brain.isRewardModulationEnabled() == False
        
        brain.enableRewardModulation(True)
        assert brain.isRewardModulationEnabled() == True
        
        # Test other flags
        brain.enableStructuralPlasticity(False)
        assert brain.isStructuralPlasticityEnabled() == False
        
        brain.enableDevelopment(False)
        assert brain.isDevelopmentEnabled() == False
        
        brain.enableCuriosity(False)
        assert brain.isCuriosityEnabled() == False

class TestDevelopmentalSystems:
    """Test developmental system operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        assert brain.initialize()
        
        self.brain = brain
        self.world = nlm.create_test_environment()
        self.agent_brain = nlm.AgentBrain(brain)
        assert self.agent_brain.initialize(self.world)
    
    def test_developmental_stage(self):
        """Test developmental stage management."""
        brain = self.agent_brain
        
        # Get initial stage
        stage = brain.getDevelopmentalStage()
        assert stage is not None
        
        # Set different stages
        brain.setDevelopmentalStage(nlm.DevelopmentalStage.CriticalPeriod)
        assert brain.getDevelopmentalStage() == nlm.DevelopmentalStage.CriticalPeriod
        
        brain.setDevelopmentalStage(nlm.DevelopmentalStage.Maturation)
        assert brain.getDevelopmentalStage() == nlm.DevelopmentalStage.Maturation
    
    def test_development_update(self):
        """Test development system update."""
        timestep = 0.1
        
        # Update development
        self.agent_brain.updateDevelopment(timestep)
        
        # Should complete without error
        assert self.agent_brain.getDevelopmentalStage() is not None

class TestPythonicAgentFeatures:
    """Test Pythonic agent brain features."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        assert brain.initialize()
        
        self.brain = brain
        self.world = nlm.create_test_environment()
        self.agent_brain = nlm.AgentBrain(brain)
        assert self.agent_brain.initialize(self.world)
    
    def test_episode_context_manager(self):
        """Test episode context manager."""
        # Verify method exists
        assert hasattr(self.agent_brain, 'episode')
        
        # Create episode runner
        episode_runner = self.agent_brain.episode(max_duration=100.0, verbose=False)
        assert episode_runner is not None
        assert hasattr(episode_runner, 'getAgentBrain')
        
        # Get brain from runner
        runner_brain = episode_runner.getAgentBrain()
        assert runner_brain is not None
        assert runner_brain == self.agent_brain.getBrain()
    
    def test_act_method(self):
        """Test act method (action production)."""
        # Verify method exists
        assert hasattr(self.agent_brain, 'act')
        
        # Create a simple percept
        percept = nlm.SensoryPercept()
        
        # Act should produce a motor command
        motor_cmd = self.agent_brain.act(percept)
        assert motor_cmd is not None
        assert isinstance(motor_cmd, nlm.MotorCommand)
        
        # Should be a valid motor command
        valid_commands = [
            nlm.MotorCommand.MoveForward,
            nlm.MotorCommand.MoveBackward,
            nlm.MotorCommand.TurnLeft,
            nlm.MotorCommand.TurnRight,
            nlm.MotorCommand.LookLeft,
            nlm.MotorCommand.LookRight,
            nlm.MotorCommand.Interact,
            nlm.MotorCommand.Wait
        ]
        assert motor_cmd in valid_commands
    
    def test_observe_method(self):
        """Test observe method (sensory processing)."""
        # Verify method exists
        assert hasattr(self.agent_brain, 'observe')
        
        # Create a percept
        percept = nlm.SensoryPercept()
        
        # Observe should process input without producing action
        # This should complete without error
        self.agent_brain.observe(percept)
        
        # If we got here without error, it's working
    
    def test_brain_interface(self):
        """Test underlying brain interface."""
        brain = self.agent_brain.getBrain()
        assert brain is not None
        assert isinstance(brain, nlm.Brain)
        
        # Verify we can access brain statistics
        neuron_count = brain.getTotalNeuronCount()
        assert neuron_count > 0
        
        region_count = brain.getRegionCount()
        assert region_count > 0

if __name__ == "__main__":
    pytest.main([__file__, "-v"])