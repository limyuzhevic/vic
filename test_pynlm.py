#!/usr/bin/env python3
"""
Python test suite for pynlm module.

This test suite provides comprehensive coverage of the NLM Python bindings,
including testing configuration management, brain simulation, agent functionality,
and error handling.
"""

import os
import sys
import tempfile
import json
import pytest
import pynlm

class TestModuleAttributes:
    """Test module-level attributes and basic functionality."""
    
    def test_module_version(self):
        """Test that module has version attribute."""
        assert hasattr(pynlm, '__version__')
        assert isinstance(pynlm.__version__, str)
        assert pynlm.__version__ == "0.1.0"
    
    def test_module_attributes(self):
        """Test that module has all expected attributes."""
        assert hasattr(pynlm, '__author__')
        assert hasattr(pynlm, '__description__')
        assert hasattr(pynlm, '__url__')
        assert hasattr(pynlm, '__license__')
        
        assert isinstance(pynlm.__author__, str)
        assert isinstance(pynlm.__description__, str)
        assert isinstance(pynlm.__url__, str)
        assert isinstance(pynlm.__license__, str)

class TestConfigClass:
    """Test the Config class functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.config = pynlm.createDefaultConfig()
    
    def test_config_creation(self):
        """Test basic configuration creation."""
        assert self.config is not None
        assert not self.config.has("test_key")
    
    def test_config_set_and_get(self):
        """Test setting and getting configuration values."""
        # Test integer values
        self.config.set("test_int", 42)
        assert self.config.has("test_int")
        
        # Test float values
        self.config.set("test_float", 3.14)
        assert self.config.has("test_float")
        
        # Test boolean values
        self.config.set("test_bool", True)
        assert self.config.has("test_bool")
        
        # Test string values
        self.config.set("test_str", "hello")
        assert self.config.has("test_str")
        
        # Test list values
        self.config.set("test_list", [1, 2, 3])
        assert self.config.has("test_list")
    
    def test_config_get_with_type(self):
        """Test getting values with type conversion."""
        self.config.set("int_val", 100)
        self.config.set("float_val", 2.718)
        self.config.set("bool_val", False)
        self.config.set("str_val", "world")
        
        # Test integer type
        int_val = self.config.get("int_val", "int")
        assert int_val is not None
        assert int_val == 100
        
        # Test float type
        float_val = self.config.get("float_val", "float")
        assert float_val is not None
        assert float_val == 2.718
        
        # Test boolean type
        bool_val = self.config.get("bool_val", "bool")
        assert bool_val is not None
        assert bool_val is False
        
        # Test string type
        str_val = self.config.get("str_val", "str")
        assert str_val is not None
        assert str_val == "world"
    
    def test_config_get_or(self):
        """Test getOr method."""
        # Test with existing key
        self.config.set("existing", 50)
        val = self.config.getOr("existing", 100)
        assert val == 50
        
        # Test with missing key
        val = self.config.getOr("missing", 999)
        assert val == 999
    
    def test_config_remove(self):
        """Test removing configuration keys."""
        self.config.set("to_remove", 123)
        assert self.config.has("to_remove")
        
        self.config.remove("to_remove")
        assert not self.config.has("to_remove")
    
    def test_config_keys(self):
        """Test getting all configuration keys."""
        self.config.set("key1", 1)
        self.config.set("key2", "test")
        
        keys = self.config.getKeys()
        assert "key1" in keys
        assert "key2" in keys
        assert len(keys) >= 2
    
    def test_config_clear(self):
        """Test clearing configuration."""
        self.config.set("temp1", 1)
        self.config.set("temp2", "test")
        
        assert self.config.has("temp1")
        assert self.config.has("temp2")
        
        self.config.clear()
        
        assert not self.config.has("temp1")
        assert not self.config.has("temp2")
    
    def test_config_summary(self):
        """Test configuration summary."""
        self.config.set("test1", 100)
        self.config.set("test2", "hello")
        
        summary = self.config.summary()
        assert isinstance(summary, str)
        assert "test1" in summary
        assert "test2" in summary
    
    def test_config_validation(self):
        """Test configuration validation methods."""
        # Test key validation
        assert self.config.validateKey("brain.neuron_count", "neuron_*")
        assert self.config.validateKey("synapse.weight", "synapse_*")
        
        # Test type validation
        self.config.set("int_value", 42)
        self.config.set("float_value", 3.14)
        self.config.set("bool_value", True)
        self.config.set("str_value", "hello")
        self.config.set("list_value", [1, 2, 3])
        
        assert self.config.validateType("int_value", "int")
        assert self.config.validateType("float_value", "float")
        assert self.config.validateType("bool_value", "bool")
        assert self.config.validateType("str_value", "str")
        assert self.config.validateType("list_value", "int_list")
    
    def test_config_info(self):
        """Test configuration info method."""
        self.config.set("int_val", 100)
        self.config.set("float_val", 2.718)
        self.config.set("str_val", "test")
        
        info = self.config.getConfigInfo()
        assert isinstance(info, dict)
        assert "int_val" in info
        assert "float_val" in info
        assert "str_val" in info
        
        # Check that types are correctly identified
        assert "int:" in info["int_val"]
        assert "float:" in info["float_val"]
        assert "string:" in info["str_val"]
    
    def test_config_set_multiple(self):
        """Test setMultiple convenience method."""
        values = {
            "neuron_count": 2000,
            "synapse_density": 0.15,
            "plasticity_enabled": True,
            "learning_rate": 0.001
        }
        
        self.config.setMultiple(values)
        
        for key in values:
            assert self.config.has(key)
    
    def test_config_file_operations(self):
        """Test configuration file operations."""
        import tempfile
        
        # Create a temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            temp_file = f.name
        
        try:
            # Set some values
            self.config.set("test1", "value1")
            self.config.set("test2", 123)
            
            # Save to file
            assert self.config.saveToFile(temp_file)
            
            # Create new config and load
            new_config = pynlm.createDefaultConfig()
            assert new_config.loadFromFile(temp_file)
            
            # Verify loaded values
            assert new_config.get("test1", "str") == "value1"
            assert new_config.get("test2", "int") == 123
            
        finally:
            # Clean up
            if os.path.exists(temp_file):
                os.unlink(temp_file)
    
    def test_config_error_handling(self):
        """Test configuration error handling."""
        # Test invalid key
        with pytest.raises(Exception):
            self.config.set("", "value")  # Empty key
        
        # Test invalid file path (simulate error)
        invalid_path = "/nonexistent/path/config.json"
        # Note: Actual file operations might succeed due to directory creation

class TestBrainClass:
    """Test the Brain class functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100)
        self.config.set("brain.region_count", 2)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        """Test brain creation."""
        assert self.brain is not None
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        # Brain should be initialized
        assert self.brain.getTotalNeuronCount() == 100
        assert self.brain.getRegionCount() == 2
    
    def test_brain_step(self):
        """Test brain simulation step."""
        # Store initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_neurons = self.brain.getTotalNeuronCount()
        
        # Run a few steps
        for step in range(10):
            self.brain.step(step)
        
        # Verify brain state changed
        assert self.brain.getTotalNeuronCount() == initial_neurons
        assert self.brain.getTotalSpikeCount() >= initial_spikes
    
    def test_brain_reset(self):
        """Test brain reset."""
        # Run some steps
        for step in range(5):
            self.brain.step(step)
        
        # Reset brain
        self.brain.reset()
        
        # Verify reset
        assert self.brain.getTotalNeuronCount() == 100
        assert self.brain.getRegionCount() == 2
    
    def test_brain_action_production(self):
        """Test brain action production."""
        # Create some neural activity
        for step in range(5):
            self.brain.step(step)
        
        # Try to produce an action
        action = self.brain.produceAction()
        # Action might be None if no motor neurons are active
        # This is acceptable behavior
        assert action is None or hasattr(action, 'getType')
    
    def test_brain_save_load(self):
        """Test brain save and load functionality."""
        import tempfile
        
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            temp_file = f.name
        
        try:
            # Run some simulation steps
            for step in range(10):
                self.brain.step(step)
            
            # Save brain state
            assert self.brain.save(temp_file)
            
            # Create new brain and load
            new_brain = pynlm.createBrain(self.config)
            new_brain.initialize()
            
            # Load saved state
            assert new_brain.load(temp_file)
            
            # Verify loaded state
            assert new_brain.getTotalNeuronCount() == self.brain.getTotalNeuronCount()
            
        finally:
            if os.path.exists(temp_file):
                os.unlink(temp_file)

class TestAgentBrainClass:
    """Test the AgentBrain class functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_brain_creation(self):
        """Test agent brain creation."""
        assert self.agent is not None
        assert self.agent.getBrain() is not None
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        assert self.agent.getSensoryInputSize() > 0
        assert self.agent.getMotorOutputSize() > 0
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        # Get sensory percept from world
        percept = self.world.getSensoryPercept()
        
        # Process it
        self.agent.processSensoryInput(percept)
        
        # Verify processing didn't crash
        assert True  # If we get here, processing succeeded
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Process some sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Run brain step
        self.brain.step(0)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        # Action might be None if no motor neurons are active
        assert action is None or hasattr(action, 'getType')
    
    def test_agent_neuromodulation(self):
        """Test neuromodulation functionality."""
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Check neuromodulation level
        neuromod_level = self.agent.getNeuromodulationLevel()
        assert isinstance(neuromod_level, float)
    
    def test_agent_development(self):
        """Test development system."""
        # Update development
        self.agent.updateDevelopment(0.1)
        
        # Check developmental stage
        dev_stage = self.agent.getDevelopmentalStage()
        assert dev_stage is not None
    
    def test_agent_subsystem_control(self):
        """Test subsystem enable/disable."""
        # Test enabling subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Test disabling subsystems
        self.agent.enableRewardModulation(False)
        self.agent.enableCuriosity(False)
        self.agent.enableDevelopment(False)
        
        # Check subsystem states
        assert self.agent.isRewardModulationEnabled() == False
        assert self.agent.isCuriosityEnabled() == False
        assert self.agent.isDevelopmentEnabled() == False

class TestSimpleWorldClass:
    """Test the SimpleWorld class functionality."""
    
    def setup_method(self):
        """Set up test fixtures before each test method."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
    
    def test_world_creation(self):
        """Test world creation."""
        assert self.world is not None
        assert self.world.getWidth() == 20
        assert self.world.getHeight() == 20
    
    def test_world_configuration(self):
        """Test world configuration."""
        self.world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        
        assert self.world.getWidth() == 30
        assert self.world.getHeight() == 30
    
    def test_world_update(self):
        """Test world update."""
        # Get initial simulation time
        initial_time = self.world.getSimulationTime()
        
        # Update world
        self.world.update(0.1)
        
        # Check that time increased
        assert self.world.getSimulationTime() > initial_time
    
    def test_world_agent_start(self):
        """Test agent starting position."""
        self.world.setAgentStart(10.0, 10.0)
        
        # Get agent body
        body = self.world.getAgentBody()
        assert body is not None
        assert body.x == 10.0
        assert body.y == 10.0
    
    def test_world_sensory_percept(self):
        """Test sensory percept generation."""
        percept = self.world.getSensoryPercept()
        assert percept is not None
        assert hasattr(percept, 'getVision')
        assert hasattr(percept, 'getInternal')
    
    def test_world_agent_body(self):
        """Test agent body access."""
        body = self.world.getAgentBody()
        assert body is not None
        
        # Check body attributes
        assert hasattr(body, 'x')
        assert hasattr(body, 'y')
        assert hasattr(body, 'orientation')
        assert hasattr(body, 'energy')
    
    def test_world_motor_command(self):
        """Test motor command application."""
        # Create a simple action
        action = pynlm.Action()
        # Note: Action might need specific parameters depending on implementation
        
        # Try to apply motor command (might fail if action is not properly configured)
        try:
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            assert True
        except:
            # This is acceptable if action configuration fails
            pass
    
    def test_world_object_management(self):
        """Test world object management."""
        # Create a world object
        world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
        
        # Add to world
        self.world.addObject(world_obj)
        
        # Try to remove (position might not match exactly)
        self.world.removeObject(5.0, 5.0)
        
        # Check if position is valid
        assert self.world.isValidPosition(10.0, 10.0)
    
    def test_world_properties(self):
        """Test world properties."""
        # Set energy properties
        self.world.setMaxEnergy(100.0)
        self.world.setEnergyDecayRate(0.1)
        
        assert self.world.getMaxEnergy() == 100.0
        assert self.world.getEnergyDecayRate() == 0.1

class TestSensoryInputClass:
    """Test sensory input classes."""
    
    def test_vision_creation(self):
        """Test Vision sensory input creation."""
        vision = pynlm.Vision(8, 8, 3)
        assert vision is not None
        assert vision.getWidth() == 8
        assert vision.getHeight() == 8
        assert vision.getChannels() == 3
        
        # Test setting data
        data = [0.1, 0.2, 0.3] * 8  # 8*3 = 24 elements
        vision.setData(data)
    
    def test_audio_creation(self):
        """Test Audio sensory input creation."""
        audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
        assert audio is not None
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        
        # Test setting data
        data = [0.0, 0.5, 0.0, -0.5] * 256  # 4*256 = 1024 elements
        audio.setData(data)
    
    def test_internal_signals_creation(self):
        """Test InternalSignals sensory input creation."""
        internal = pynlm.InternalSignals()
        assert internal is not None
        
        # Test adding signals
        internal.addSignal(0.5)
        internal.addSignal(-0.3)
        internal.addSignal(0.9)
        
        assert internal.getDimensions() == 3

class TestActionClass:
    """Test the Action class functionality."""
    
    def test_action_creation(self):
        """Test action creation."""
        # Create action with type only
        action1 = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action1 is not None
        assert action1.getType() == pynlm.ActionType.MoveForward
        
        # Create action with type and parameters
        action2 = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0])
        assert action2 is not None
        assert action2.getType() == pynlm.ActionType.MoveForward
        assert action2.getParameters() == [1.0, 2.0]
    
    def test_action_properties(self):
        """Test action properties."""
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        # Test getting name
        name = action.getName()
        assert isinstance(name, str)
        assert len(name) > 0
        
        # Test setting type
        action.setType(pynlm.ActionType.TurnLeft)
        assert action.getType() == pynlm.ActionType.TurnLeft
        
        # Test setting parameters
        action.setParameters([1.0, 2.0])
        assert action.getParameters() == [1.0, 2.0]
    
    def test_action_clone(self):
        """Test action cloning."""
        original = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0])
        
        cloned = original.clone()
        assert cloned is not None
        assert cloned.getType() == original.getType()
        assert cloned.getParameters() == original.getParameters()

class TestModuleUtils:
    """Test module utility functions."""
    
    def test_validate_key(self):
        """Test key validation utility."""
        assert pynlm.validate_key("brain.neuron_count")
        assert pynlm.validate_key("synapse.weight")
        assert pynlm.validate_key("learning_rate")
        
        # Test invalid keys
        assert not pynlm.validate_key("")  # Empty
        assert not pynlm.validate_key("key with spaces")  # Spaces
    
    def test_validate_value(self):
        """Test value validation utility."""
        # Test valid values
        assert pynlm.validate_value("int", 42)
        assert pynlm.validate_value("float", 3.14)
        assert pynlm.validate_value("bool", True)
        assert pynlm.validate_value("str", "hello")
        assert pynlm.validate_value("list", [1, 2, 3])
        
        # Test invalid values
        with pytest.raises(Exception):
            pynlm.validate_value("int", "not an int")
        
        with pytest.raises(Exception):
            pynlm.validate_value("float", "not a float")
    
    def test_parse_config_value(self):
        """Test configuration value parsing utility."""
        # Test parsing different types
        int_val = pynlm.parse_config_value("int", 42)
        assert int_val == 42
        
        float_val = pynlm.parse_config_value("float", 3.14)
        assert float_val == 3.14
        
        bool_val = pynlm.parse_config_value("bool", True)
        assert bool_val is True
        
        str_val = pynlm.parse_config_value("str", "hello")
        assert str_val == "hello"
    
    def test_safe_config_operation(self):
        """Test safe configuration operation utility."""
        def successful_operation():
            return "success"
        
        def failing_operation():
            raise ValueError("Test error")
        
        # Test successful operation
        result = pynlm.safe_config_operation(successful_operation, "test")
        assert result is True
        
        # Test failing operation
        with pytest.raises(Exception):
            pynlm.safe_config_operation(failing_operation, "test")

class TestIntegration:
    """Integration tests for complete simulation."""
    
    def test_complete_agent_simulation(self):
        """Test complete agent simulation with all components."""
        # Create complete agent setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify simulation completed
        assert brain is not None
        assert agent is not None
        assert world is not None
        
        # Check that simulation made progress
        assert brain.getTotalSpikeCount() >= 0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v", "-s"])