"""
Test suite for NLM Python bindings.

Run with: python -m pytest nlm_tests.py
"""

import pytest
import pynlm
import os
import tempfile
import json

class TestBasicFunctionality:
    """Test basic NLM functionality."""
    
    def test_create_brain(self):
        """Test brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should have neurons after initialization
        assert brain.getTotalNeuronCount() > 0
    
    def test_brain_step(self):
        """Test brain simulation steps."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run a few steps
        for step in range(10):
            brain.step(step)
        
        final_spikes = brain.getTotalSpikeCount()
        
        # Spike count should have increased or stayed the same
        assert final_spikes >= initial_spikes
    
    def test_world_creation(self):
        """Test world creation."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getAgentBody() is not None
    
    def test_agent_creation(self):
        """Test agent brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        assert agent.getBrain() == brain
    
    def test_enums(self):
        """Test enumerations."""
        # Test that enumerations exist
        assert hasattr(pynlm, 'NeuronType')
        assert hasattr(pynlm, 'SynapseType')
        assert hasattr(pynlm, 'DevelopmentalStage')
        
        # Test enum values
        assert pynlm.NeuronType.Excitatory is not None
        assert pynlm.NeuronType.Inhibitory is not None
    
    def test_constants(self):
        """Test constant values."""
        assert hasattr(pynlm, 'INVALID_NEURON_ID')
        assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
        assert hasattr(pynlm, 'INVALID_REGION_ID')
        assert hasattr(pynlm, 'INVALID_POPULATION_ID')
class TestConfiguration:
    """Test configuration functionality."""
    
    def test_config_creation(self):
        """Test default configuration creation."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Config should have some default keys
        keys = config.getKeys()
        assert len(keys) > 0
    
    def test_config_set_get(self):
        """Test configuration value setting and getting."""
        config = pynlm.createDefaultConfig()
        
        # Set a test value
        config.set("test.value", 42)
        
        # Config should have the key
        assert "test.value" in config.getKeys()
    
    def test_config_summary(self):
        """Test configuration summary."""
        config = pynlm.createDefaultConfig()
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
    
    def test_config_save_load(self):
        """Test configuration save/load functionality."""
        config = pynlm.createDefaultConfig()
        config.set("test.key", "test_value")
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            # Save configuration
            config.saveToFile(config_file)
            
            # Create new config and load
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(config_file)
            
            # Both should have the key
            assert "test.key" in config.getKeys()
            assert "test.key" in config2.getKeys()
            
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)
class TestAgentFunctionality:
    """Test agent-specific functionality."""
    
    def setup_method(self):
        """Set up test fixtures."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_sensory_processing(self):
        """Test agent sensory processing."""
        # Get sensory input from world
        percept = self.world.getSensoryPercept()
        assert percept is not None
        
        # Process it through agent
        self.agent.processSensoryInput(percept)
        
        # Should not crash
    
    def test_agent_motor_decoding(self):
        """Test agent motor command decoding."""
        action = self.agent.decodeMotorCommand()
        assert action is not None
        
        # Should be a valid action type
        assert hasattr(pynlm, 'ActionType')
    
    def test_agent_enable_features(self):
        """Test enabling agent features."""
        # Enable various features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Check they are enabled
        assert self.agent.isRewardModulationEnabled() == True
        assert self.agent.isCuriosityEnabled() == True
        assert self.agent.isDevelopmentEnabled() == True
        assert self.agent.isStructuralPlasticityEnabled() == True
    
    def test_agent_neuromodulation(self):
        """Test agent neuromodulation."""
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Check neuromodulation levels
        assert self.agent.getNeuromodulationLevel() is not None
        assert self.agent.getCuriosityLevel() is not None
        assert self.agent.getNoveltyLevel() is not None
        assert self.agent.getPredictionError() is not None
    
    def test_agent_development(self):
        """Test agent development system."""
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Check developmental stage
        stage = self.agent.getDevelopmentalStage()
        assert stage is not None
class TestWorldFunctionality:
    """Test world-specific functionality."""
    
    def test_world_configuration(self):
        """Test world configuration."""
        world = pynlm.createSimpleWorld()
        
        world.configure(width=30, height=20, visionWidth=10, visionHeight=8)
        
        assert world.getWidth() == 30
        assert world.getHeight() == 20
        assert world.getAgentBody() is not None
    
    def test_world_update(self):
        """Test world update."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        # Update world
        world.update(0.1)
        
        # Should not crash
        assert world.getSimulationTime() > 0
    
    def test_world_actions(self):
        """Test world actions."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        # Test setting agent start position
        world.setAgentStart(5.0, 5.0)
        
        # Test applying motor command
        action = pynlm.ActionType.MoveForward
        world.applyMotorCommand(action, 0.0)
        
        # Should not crash
    
    def test_world_objects(self):
        """Test world object management."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        # Create a world object
        obj = pynlm.WorldObject(3.0, 3.0, pynlm.WorldObjectType.Resource, value=5.0)
        
        # Add to world
        world.addObject(obj)
        
        # Check if position is valid
        assert world.isValidPosition(3.0, 3.0)
        
        # Remove object
        world.removeObject(3.0, 3.0)
        
        # Position should still be valid (empty)
        assert world.isValidPosition(3.0, 3.0)
class TestConvenienceFunctions:
    """Test convenience functions."""
    
    def test_examples_module(self):
        """Test that examples module can be imported."""
        import pynlm.examples
        assert pynlm.examples is not None
    
    def test_main_module(self):
        """Test that main module can be imported."""
        import pynlm.examples.main
        assert pynlm.examples.main is not None
    
    def test_advanced_examples(self):
        """Test that advanced examples module can be imported."""
        import pynlm.examples.advanced_examples
        assert pynlm.examples.advanced_examples is not None
if __name__ == "__main__":
    # Run tests if executed directly
    pytest.main([__file__, "-v"])
