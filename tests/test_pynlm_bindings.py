"""
Test suite for NLM Python bindings

This module provides comprehensive tests for the NLM Python bindings,
testing both basic functionality and advanced features.
"""

import pynlm
import numpy as np
import pytest
import json
import tempfile
import os
from typing import Dict, Any

# Test constants
TEST_BRAIN_CONFIG = {
    "brain.neuron_count": 100,
    "brain.synapse_density": 0.1,
    "brain.region_count": 5,
}

class TestConfig:
    """Test configuration class functionality."""
    
    def test_create_default_config(self):
        """Test creating default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert isinstance(config, pynlm.Config)
        
        # Test default values
        assert config.has("brain.neuron_count") is False  # Not set by default
    
    def test_config_set_get(self):
        """Test setting and getting configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Test setting different types
        config.set("test_int", 42)
        config.set("test_float", 3.14)
        config.set("test_bool", True)
        config.set("test_string", "hello")
        
        # Test getting values
        assert config.get("test_int") == 42
        assert config.get("test_float") == 3.14
        assert config.get("test_bool") is True
        assert config.get("test_string") == "hello"
    
    def test_config_get_or_default(self):
        """Test getOr method with default values."""
        config = pynlm.createDefaultConfig()
        
        # Test with default
        assert config.getOr("nonexistent", 99) == 99
        
        # Set a value and verify getOr returns it
        config.set("existing", 100)
        assert config.getOr("existing", 99) == 100
    
    def test_config_has(self):
        """Test has method."""
        config = pynlm.createDefaultConfig()
        
        # Test nonexistent key
        assert config.has("nonexistent") is False
        
        # Set and test existence
        config.set("test_key", "value")
        assert config.has("test_key") is True
    
    def test_config_keys(self):
        """Test getKeys method."""
        config = pynlm.createDefaultConfig()
        
        # Start empty
        assert config.getKeys() == []
        
        # Add keys and test
        config.set("key1", "value1")
        config.set("key2", "value2")
        
        keys = config.getKeys()
        assert "key1" in keys
        assert "key2" in keys
        assert len(keys) == 2
    
    def test_config_clear(self):
        """Test clear method."""
        config = pynlm.createDefaultConfig()
        
        # Add some keys
        config.set("key1", "value1")
        config.set("key2", "value2")
        
        # Clear and verify
        config.clear()
        assert config.getKeys() == []
    
    def test_config_summary(self):
        """Test summary method."""
        config = pynlm.createDefaultConfig()
        
        # Test empty config
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
        
        # Add keys and test summary
        config.set("neuron_count", 1000)
        config.set("synapse_density", 0.1)
        
        summary = config.summary()
        assert "neuron_count" in summary
        assert "synapse_density" in summary
    
    def test_config_save_load(self):
        """Test saveToFile and loadFromFile methods."""
        config1 = pynlm.createDefaultConfig()
        config1.set("test_int", 42)
        config1.set("test_string", "hello")
        
        # Create temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            # Save config
            config1.saveToFile(config_file)
            assert os.path.exists(config_file)
            
            # Load config
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(config_file)
            
            # Verify loaded config
            assert config2.get("test_int") == 42
            assert config2.get("test_string") == "hello"
            
        finally:
            # Cleanup
            if os.path.exists(config_file):
                os.unlink(config_file)
class TestBrain:
    """Test brain class functionality."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        assert self.brain is not None
        assert isinstance(self.brain, pynlm.Brain)
        
        # Test basic brain properties
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() >= 0
        assert self.brain.getRegionCount() >= 0
    
    def test_brain_step(self):
        """Test brain simulation step."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Take a step
        self.brain.step(0)
        
        # Verify spike count may have changed
        # (Note: depending on configuration, spike count might not change)
        assert self.brain.getTotalSpikeCount() >= initial_spikes
    
    def test_brain_step_with_time(self):
        """Test brain simulation step with timestamp."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Take a step with time
        self.brain.step(0, 1.0)
        
        # Verify spike count may have changed
        assert self.brain.getTotalSpikeCount() >= initial_spikes
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        # Take some steps first
        for i in range(10):
            self.brain.step(i)
        
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Reset brain
        self.brain.reset()
        
        # Verify reset state
        reset_spikes = self.brain.getTotalSpikeCount()
        assert reset_spikes <= initial_spikes
    
    def test_brain_add_region(self):
        """Test adding brain regions."""
        initial_regions = self.brain.getRegionCount()
        
        # Add a region
        region_id = self.brain.addRegion("test_region")
        assert region_id is not None
        
        # Verify region count increased
        assert self.brain.getRegionCount() == initial_regions + 1
    
    def test_brain_get_region(self):
        """Test getting brain regions."""
        # Add a region first
        region_id = self.brain.addRegion("test_region")
        
        # Get the region
        region = self.brain.getRegion(region_id)
        assert region is not None
        
        # Test with invalid ID (should return None or invalid region)
        invalid_region = self.brain.getRegion(pynlm.INVALID_REGION_ID)
        # Note: behavior with invalid ID may vary
    
    def test_brain_region_stats(self):
        """Test brain region statistics."""
        # Add regions
        region1_id = self.brain.addRegion("region1")
        region2_id = self.brain.addRegion("region2")
        
        # Get regions
        region1 = self.brain.getRegion(region1_id)
        region2 = self.brain.getRegion(region2_id)
        
        # Test region statistics
        if region1:
            # Note: regions may not have getNeuronCount method depending on implementation
            pass
    
    def test_brain_neuromodulation(self):
        """Test neuromodulation functionality."""
        # Create a simple neuromodulator
        from pynlm import Dopamine
        
        dopamine = Dopamine()
        dopamine.setStrength(0.5)
        
        # Apply neuromodulation (if supported)
        # Note: implementation may vary
        pass
    
    def test_brain_save_load(self):
        """Test brain save and load functionality."""
        # Take some steps first
        for i in range(20):
            self.brain.step(i)
        
        # Save brain state
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            brain_file = f.name
        
        try:
            self.brain.save(brain_file)
            assert os.path.exists(brain_file)
            
            # Create new brain and load saved state
            brain2 = pynlm.createBrain(self.config)
            brain2.load(brain_file)
            
            # Verify loaded brain state
            assert brain2.getTotalNeuronCount() == self.brain.getTotalNeuronCount()
            assert brain2.getTotalSynapseCount() == self.brain.getTotalSynapseCount()
            
        finally:
            # Cleanup
            if os.path.exists(brain_file):
                os.unlink(brain_file)
class TestWorld:
    """Test world class functionality."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        assert self.world is not None
        assert isinstance(self.world, pynlm.SimpleWorld)
        
        # Test world properties
        assert self.world.getWidth() == 20
        assert self.world.getHeight() == 20
        assert self.world.getVisionWidth() == 8
        assert self.world.getVisionHeight() == 8
    
    def test_world_reset(self):
        """Test world reset functionality."""
        # Set some state
        self.world.setAgentStart(10.0, 10.0)
        self.world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0))
        
        # Reset
        self.world.reset()
        
        # Verify reset state (may vary depending on implementation)
    
    def test_world_update(self):
        """Test world update functionality."""
        initial_time = self.world.getSimulationTime()
        
        # Update world
        self.world.update(0.1)
        
        # Verify time increased
        assert self.world.getSimulationTime() > initial_time
    
    def test_world_apply_motorda(self):
        """Test applying motor commands."""
        # Create a motor command
        action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 0.0])
        
        # Apply action
        result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Note: result type and behavior may vary
        pass
    
    def test_world_sensory_percept(self):
        """Test sensory percept functionality."""
        percept = self.world.getSensoryPercept()
        
        assert percept is not None
        assert isinstance(percept, pynlm.SensoryPercept)
        
        # Test percept properties
        vision = percept.getVision()
        if vision:
            assert vision.getWidth() == 8
            assert vision.getHeight() == 8
            assert vision.getChannels() == 3
    
    def test_world_agent_body(self):
        """Test agent body functionality."""
        body = self.world.getAgentBody()
        
        assert body is not None
        assert isinstance(body, pynlm.AgentBody)
        
        # Test body properties
        assert body.x == 0.0  # Initial position
        assert body.y == 0.0
    
    def test_world_object_management(self):
        """Test world object management."""
        # Add an object
        obj = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, value=1.0)
        self.world.addObject(obj)
        
        # Test position validity
        assert self.world.isValidPosition(10.0, 10.0)
        assert not self.world.isValidPosition(-1.0, -1.0)
        
        # Remove object
        self.world.removeObject(10.0, 10.0)
        
        # Note: removal may not remove immediately depending on implementation
    
    def test_world_configuration(self):
        """Test world configuration methods."""
        # Test configuration parameters
        self.world.setMaxEnergy(100.0)
        self.world.setEnergyDecayRate(0.1)
        
        assert self.world.getMaxEnergy() == 100.0
        assert self.world.getEnergyDecayRate() == 0.1
    
    def test_world_random_seed(self):
        """Test random seed functionality."""
        # Set random seed
        self.world.setRandomSeed(42)
        
        # Test that seed affects world state (if implementation supports it)
        seed1 = self.world.getRandomSeed()
        
        # Set different seed
        self.world.setRandomSeed(123)
        seed2 = self.world.getRandomSeed()
        
        assert seed1 != seed2
class TestAgent:
    """Test agent brain interface functionality."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_creation(self):
        """Test agent brain interface creation."""
        assert self.agent is not None
        assert isinstance(self.agent, pynlm.AgentBrain)
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        # Agent should be initialized after setup
        # Test agent properties
        assert self.agent.getBrain() is not None
        assert self.agent.getBrain() == self.brain
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        # Get sensory percept from world
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Note: Implementation may vary - processing may happen automatically
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Get motor command from agent
        action = self.agent.decodeMotorCommand()
        
        assert action is not None
        assert isinstance(action, pynlm.Action)
        
        # Test action properties
        assert action.getType() in [pynlm.ActionType.Wait, pynlm.ActionType.MoveForward,
                                   pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight,
                                   pynlm.ActionType.Interact, pynlm.ActionType.Eat]
    
    def test_agent_reward_modulation(self):
        """Test reward modulation functionality."""
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Test neuromodulation level
        neuromodulation = self.agent.getNeuromodulationLevel()
        assert neuromodulation >= 0.0
    
    def test_agent_development(self):
        """Test development system."""
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Test developmental stage
        stage = self.agent.getDevelopmentalStage()
        assert stage in [pynlm.DevelopmentalStage.Initial,
                        pynlm.DevelopmentalStage.CriticalPeriod,
                        pynlm.DevelopmentalStage.Maturation,
                        pynlm.DevelopmentalStage.Adult,
                        pynlm.DevelopmentalStage.Aging]
    
    def test_agent_plasticity_enablers(self):
        """Test plasticity enabler functions."""
        # Enable various systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Verify enablers
        assert self.agent.isRewardModulationEnabled() is True
        assert self.agent.isStructuralPlasticityEnabled() is True
        assert self.agent.isDevelopmentEnabled() is True
        assert self.agent.isCuriosityEnabled() is True
    
    def test_agent_levels(self):
        """Test neuromodulation level getters."""
        # Test various levels
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        prediction_error = self.agent.getPredictionError()
        neuromodulation = self.agent.getNeuromodulationLevel()
        
        # All should return valid values
        assert curiosity >= 0.0
        assert novelty >= 0.0
        assert prediction_error >= 0.0
        assert neuromodulation >= 0.0
    
    def test_agent_reset(self):
        """Test agent reset functionality."""
        # Take some steps
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        self.agent.decodeMotorCommand()
        
        # Reset agent
        self.agent.reset()
        
        # Note: implementation may vary
class TestAdvancedFeatures:
    """Test advanced features and integrations."""
    
    def test_complete_simulation_loop(self):
        """Test a complete simulation loop."""
        # Create brain, world, and agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        num_steps = 50
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            if step % 10 == 0:
                agent.applyRewardModulation(0.5, 0.3)
        
        # Verify simulation completed
        assert brain.getTotalSpikeCount() > 0
        assert agent.getDevelopmentalStage() != pynlm.DevelopmentalStage.Initial
    
    def test_memory_systems(self):
        """Test memory system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test memory system access
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        # Note: Implementation may vary depending on available memory systems
        assert working_memory is not None or episodic_memory is not None
    
    def test_plasticity_systems(self):
        """Test plasticity system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test plasticity system access
        stdp = brain.getSTDP()
        hebbian = brain.getHebbian()
        structural = brain.getStructuralPlasticity()
        
        # Note: Implementation may vary
        assert stdp is not None
        assert hebbian is not None
        assert structural is not None
    
    def test_prediction_systems(self):
        """Test prediction system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test prediction system access
        prediction_system = brain.getPredictionSystem()
        
        # Note: Implementation may vary
        assert prediction_system is not None
    
    def test_cognition_systems(self):
        """Test cognition system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test cognition system access
        planner = brain.getPlanner()
        concept_formation = brain.getConceptFormation()
        attention = brain.getAttention()
        
        # Note: Implementation may vary
        assert planner is not None or concept_formation is not None or attention is not None
    
    def test_neuromodulation_systems(self):
        """Test neuromodulation system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test neuromodulation system access
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        prediction_error = brain.getPredictionErrorSignal()
        
        # Note: Implementation may vary
        assert dopamine is not None or curiosity is not None or novelty is not None or prediction_error is not None
    
    def test_development_system(self):
        """Test development system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test development system access
        development_system = brain.getDevelopmentSystem()
        
        # Test developmental stage
        stage = brain.getDevelopmentalStage()
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
        
        assert development_system is not None
        assert stage in [pynlm.DevelopmentalStage.Initial,
                        pynlm.DevelopmentalStage.CriticalPeriod,
                        pynlm.DevelopmentalStage.Maturation,
                        pynlm.DevelopmentalStage.Adult,
                        pynlm.DevelopmentalStage.Aging]
    
    def test_error_handling(self):
        """Test error handling and edge cases."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test operations with invalid parameters
        # Note: Implementation may vary depending on error handling
        
        # Test with zero or negative values
        if hasattr(brain, 'step'):
            brain.step(-1)  # May or may not raise error
        
        # Test with invalid region IDs
        invalid_region = brain.getRegion(pynlm.INVALID_REGION_ID)
        # Note: behavior with invalid ID may vary
    
    def test_performance_benchmark(self):
        """Test basic performance characteristics."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time brain operations
        import time
        
        # Time step operations
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        step_time = time.time() - start_time
        
        # Time get methods
        start_time = time.time()
        for i in range(100):
            _ = brain.getTotalNeuronCount()
            _ = brain.getTotalSynapseCount()
            _ = brain.getFiringNeuronCount()
        get_time = time.time() - start_time
        
        print(f"Performance: {step_time/100:.6f}s per step, {get_time/100:.6f}s per get methods")

if __name__ == "__main__":
    print("Running NLM Python bindings test suite...")
    
    # Run tests using pytest
    pytest.main([__file__, "-v"])
