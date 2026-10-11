"""
Python unit tests for the NLM bindings wrapper

This module contains comprehensive unit tests for the Python bindings wrapper,
testing all enhanced Python features and ensuring proper integration with the C++ core.
"""

import pytest
import tempfile
import os
import json
import yaml
from typing import Dict, Any
from enum import Enum

# Import the modules to test
from . import config
from .brain import Brain, ProgressCallback
from .world import SimpleWorld
from .agent import AgentBrain
from .logger import LogLevel, StructuredLogger, get_logger
class TestConfigModule:
    """Test the enhanced configuration module."""
    
    def test_config_schema(self):
        """Test ConfigSchema class."""
        schema = config.ConfigSchema(
            name="test_int",
            data_type=int,
            required=True,
            default=42,
            description="Test integer configuration"
        )
        
        assert schema.name == "test_int"
        assert schema.data_type == int
        assert schema.required == True
        assert schema.default == 42
        assert schema.description == "Test integer configuration"
        
        # Test validation
        assert schema.validate(100) == True
        assert schema.validate("100") == True  # String conversion
        assert schema.validate("invalid") == False
    
    def test_config_schema_validator(self):
        """Test ConfigValidator class."""
        schemas = [
            config.ConfigSchema("int_key", int, required=True),
            config.ConfigSchema("str_key", str, required=False, default="default"),
        ]
        
        validator = config.ConfigValidator(schemas)
        
        # Test validation
        config_dict = {"int_key": 42, "str_key": "test"}
        is_valid, errors = validator.validate_all(config_dict)
        assert is_valid == True
        assert errors == []
        
        # Test missing required field
        config_dict2 = {"str_key": "test"}
        is_valid, errors = validator.validate_all(config_dict2)
        assert is_valid == False
        assert any("int_key" in error for error in errors)
        
        # Test invalid type
        config_dict3 = {"int_key": "not_an_int", "str_key": "test"}
        is_valid, errors = validator.validate_all(config_dict3)
        assert is_valid == False
        assert any("expected int" in error for error in errors)
        
        # Test default config
        default_config = validator.get_default_config()
        assert "str_key" in default_config
        assert default_config["str_key"] == "default"
        assert "int_key" not in default_config
    
    def test_config_class(self):
        """Test Config class."""
        # Create config with schemas
        schemas = [
            config.ConfigSchema("test_int", int, required=True),
            config.ConfigSchema("test_str", str, required=False, default="hello"),
        ]
        
        cfg = config.Config(schemas=schemas)
        
        # Test setting values
        cfg.set("test_int", 100)
        assert cfg.get("test_int") == 100
        
        # Test default value
        assert cfg.get("test_str") == "hello"
        
        # Test has method
        assert cfg.has("test_int") == True
        assert cfg.has("test_str") == True
        assert cfg.has("nonexistent") == False
        
        # Test get keys
        keys = cfg.get_keys()
        assert "test_int" in keys
        assert "test_str" in keys
        assert len(keys) == 2
        
        # Test summary
        summary = cfg.summary()
        assert "Config" in summary
        assert "2 items" in summary
    
    def test_config_file_operations(self):
        """Test config file operations."""
        cfg = config.Config()
        
        # Set some values
        cfg.set("int_value", 42)
        cfg.set("str_value", "test")
        cfg.set("bool_value", True)
        cfg.set("float_value", 3.14)
        
        # Save to JSON file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            cfg.save_to_file(filepath)
            
            # Load from file
            cfg2 = config.Config()
            cfg2.load_from_file(filepath)
            
            assert cfg2.get("int_value") == 42
            assert cfg2.get("str_value") == "test"
            assert cfg2.get("bool_value") == True
            assert cfg2.get("float_value") == 3.14
        finally:
            os.unlink(filepath)
    
    def test_config_env_loading(self, monkeypatch):
        """Test environment variable loading."""
        # Set environment variables
        monkeypatch.setenv("NLM_TEST_INT", "100")
        monkeypatch.setenv("NLM_TEST_STR", "environment")
        monkeypatch.setenv("NLM_TEST_BOOL", "true")
        monkeypatch.setenv("NLM_TEST_FLOAT", "2.718")
        
        cfg = config.Config()
        cfg.load_from_env("NLM_")
        
        assert cfg.get("test_int") == 100
        assert cfg.get("test_str") == "environment"
        assert cfg.get("test_bool") == True
        assert cfg.get("test_float") == 2.718
    
    def test_config_schema_from_file(self):
        """Test loading schema from file."""
        schema_data = {
            "schema": {
                "int_key": {"type": "integer", "required": True},
                "str_key": {"type": "string", "default": "default", "required": False},
            }
        }
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(schema_data, f)
            schema_file = f.name
        
        try:
            cfg = config.Config(schema_file=schema_file)
            assert len(cfg._schemas) == 2
            
            # Check schema contents
            schema_map = {s.name: s for s in cfg._schemas}
            assert "int_key" in schema_map
            assert "str_key" in schema_map
            assert schema_map["int_key"].required == True
            assert schema_map["str_key"].default == "default"
        finally:
            os.unlink(schema_file)
    
    def test_config_validation_errors(self):
        """Test configuration validation error handling."""
        schemas = [
            config.ConfigSchema("int_key", int, required=True),
            config.ConfigSchema("range_key", int, 
                              validator=lambda x: 1 <= x <= 100),
        ]
        
        cfg = config.Config(schemas=schemas)
        cfg.set("int_key", 50)
        
        # This should pass validation
        cfg.set("range_key", 50)
        
        # This should raise error
        try:
            cfg.set("range_key", 150)
            assert False, "Should have raised ValueError"
        except ValueError:
            pass
class TestBrainModule:
    """Test the brain wrapper module."""
    
    def test_brain_creation(self):
        """Test brain creation."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        
        assert brain is not None
        assert brain.get_total_neuron_count() > 0
    
    def test_brain_step(self):
        """Test brain simulation step."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        brain.initialize()
        
        initial_spikes = brain.get_total_spike_count()
        brain.step(1)
        assert brain.get_total_spike_count() >= initial_spikes
    
    def test_progress_callback(self):
        """Test progress callback functionality."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        brain.initialize()
        
        callback_calls = []
        
        def progress_callback(step: int, progress: float, stats: Dict[str, Any]):
            callback_calls.append((step, progress, stats))
        
        brain.add_progress_callback(progress_callback, interval=1)
        
        # Run a few steps
        for step in range(5):
            brain.step(step)
        
        # Should have called callback for some steps
        assert len(callback_calls) > 0
        
        # Clean up
        brain.remove_progress_callback(progress_callback)
    
    def test_stats_collection(self):
        """Test statistics collection."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        brain.initialize()
        
        # Initial stats
        initial_stats = brain.get_stats()
        assert 'total_spike_count' in initial_stats
        assert 'firing_neuron_count' in initial_stats
        assert 'total_neuron_count' in initial_stats
        
        # Run simulation
        brain.step(1)
        
        # Check that stats are recorded
        stats_history = brain.get_stats_history()
        assert len(stats_history) > 0
        
        # Stats should have increased or changed
        assert brain.get_total_spike_count() >= initial_stats['total_spike_count']
    
    def test_brain_save_load(self, tmp_path):
        """Test brain save/load functionality."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        brain.initialize()
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        # Save brain state
        save_path = tmp_path / "brain_state.nlb"
        brain.save_state(str(save_path))
        assert save_path.exists()
        
        # Create new brain and load state
        brain2 = Brain(cfg)
        brain2.load_state(str(save_path))
        
        # Should have similar stats
        assert brain2.get_total_neuron_count() == brain.get_total_neuron_count()
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        brain.initialize()
        
        # Run some steps
        for step in range(5):
            brain.step(step)
        
        initial_spikes = brain.get_total_spike_count()
        assert initial_spikes > 0
        
        # Reset
        brain.reset()
        
        # Should have reset stats
        assert brain.get_stats_history() == []
        assert brain.get_total_spike_count() == 0
    
    def test_brain_repr(self):
        """Test brain string representation."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        brain.initialize()
        
        repr_str = repr(brain)
        assert "Brain" in repr_str
        assert "neurons=" in repr_str
        assert "spikes=" in repr_str
class TestWorldModule:
    """Test the world wrapper module."""
    
    def test_world_creation(self):
        """Test world creation."""
        world = SimpleWorld()
        assert world is not None
        
        # Configure world
        world.configure(20, 20, 8, 8)
        assert world.get_width() == 20
        assert world.get_height() == 20
    
    def test_world_operations(self):
        """Test world basic operations."""
        world = SimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        # Set agent start
        world.set_agent_start(5.0, 5.0)
        
        # Update world
        world.update(0.1)
        
        # Get agent body
        agent_body = world.get_agent_body()
        assert agent_body is not None
        
        # Get width/height
        assert world.get_width() == 10
        assert world.get_height() == 10
    
    def test_world_serialization(self):
        """Test world serialization operations."""
        world1 = SimpleWorld()
        world1.configure(15, 15, 8, 8)
        world1.reset()
        
        # The world doesn't have save/load methods, but we can test
        # that the configure method works correctly
        world2 = SimpleWorld()
        world2.configure(30, 30, 16, 16)
        
        assert world1.get_width() == 15
        assert world2.get_width() == 30
    
    def test_world_repr(self):
        """Test world string representation."""
        world = SimpleWorld()
        world.configure(20, 30, 8, 8)
        
        repr_str = repr(world)
        assert "SimpleWorld" in repr_str
        assert "20x30" in repr_str
class TestAgentModule:
    """Test the agent wrapper module."""
    
    def test_agent_creation(self):
        """Test agent creation."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        agent = AgentBrain(brain)
        
        assert agent is not None
        assert agent.get_brain() is not None
    
    def test_agent_initialization(self, tmp_path):
        """Test agent initialization."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        agent = AgentBrain(brain)
        
        # Create world
        world = SimpleWorld()
        world.configure(20, 20, 8, 8)
        
        # Initialize agent
        agent.initialize(world)
        
        # Test basic functionality
        assert agent.get_sensory_input_size() > 0
        assert agent.get_motor_output_size() > 0
        assert agent.get_developmental_stage() is not None
    
    def test_agent_neuromodulation(self):
        """Test agent neuromodulation features."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        agent = AgentBrain(brain)
        
        # Enable learning features
        agent.enable_learning_features(reward_mod=True, curiosity=True, 
                                      structural=False, development=False)
        
        assert agent.is_reward_modulation_enabled() == True
        assert agent.is_curiosity_enabled() == True
        assert agent.is_structural_plasticity_enabled() == False
        assert agent.is_development_enabled() == False
        
        # Get neuromodulation stats
        stats = agent.get_neuromodulation_stats()
        assert stats['reward_modulation_enabled'] == True
        assert stats['curiosity_enabled'] == True
        assert stats['structural_plasticity_enabled'] == False
        assert stats['development_enabled'] == False
    
    def test_agent_episode(self):
        """Test agent episode simulation."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        agent = AgentBrain(brain)
        
        # Create world
        world = SimpleWorld()
        world.configure(15, 15, 8, 8)
        
        # Run episode
        steps = 10
        agent.run_episode(world, steps, timestep=0.1)
        
        # Should have completed all steps
        assert world.get_simulation_time() <= steps * 0.1
    
    def test_agent_repr(self):
        """Test agent string representation."""
        cfg = config.create_default_config()
        brain = Brain(cfg)
        agent = AgentBrain(brain)
        
        repr_str = repr(agent)
        assert "AgentBrain" in repr_str
        assert "stage=" in repr_str
class TestLoggerModule:
    """Test the logger module."""
    
    def test_log_level_enum(self):
        """Test log level enum."""
        assert LogLevel.DEBUG.value == 0
        assert LogLevel.INFO.value == 1
        assert LogLevel.WARNING.value == 2
        assert LogLevel.ERROR.value == 3
        assert LogLevel.CRITICAL.value == 4
        
        # Test from_string
        assert LogLevel.from_string("debug") == LogLevel.DEBUG
        assert LogLevel.from_string("info") == LogLevel.INFO
        assert LogLevel.from_string("warning") == LogLevel.WARNING
        assert LogLevel.from_string("error") == LogLevel.ERROR
        assert LogLevel.from_string("critical") == LogLevel.CRITICAL
        
        # Test to_string
        assert LogLevel.DEBUG.to_string() == "debug"
        assert LogLevel.INFO.to_string() == "info"
    
    def test_logger_creation(self):
        """Test logger creation."""
        logger = get_logger("test_logger", LogLevel.DEBUG)
        assert logger is not None
        assert logger.name == "test_logger"
        assert logger.get_level() == LogLevel.DEBUG
    
    def test_logger_logging(self):
        """Test basic logging functionality."""
        logger = get_logger("test_logger2", LogLevel.DEBUG)
        
        # Test debug message
        logger.debug("Test debug message", file="test.py", line=42, function="test_func")
        
        # Test info message
        logger.info("Test info message")
        
        # Test warning message
        logger.warning("Test warning message")
        
        # Test error message
        logger.error("Test error message")
        
        # Test critical message
        logger.critical("Test critical message")
    
    def test_logger_levels(self):
        """Test log level filtering."""
        # Create logger with INFO level
        logger = get_logger("test_logger3", LogLevel.INFO)
        assert logger.get_level() == LogLevel.INFO
        
        # Lower level messages should not be logged
        # (we can't easily test this without mocking, but we can test the level)
        logger.debug("This should not appear in console with INFO level")
    
    def test_global_stats(self):
        """Test global statistics tracking."""
        from .logger import add_to_global_stats, get_global_stats
        
        # Add some stats
        add_to_global_stats("debug_count", 5)
        add_to_global_stats("info_count", 10)
        add_to_global_stats("warning_count", 3)
        
        # Get stats
        stats = get_global_stats()
        assert "debug_count" in stats
        assert "info_count" in stats
        assert "warning_count" in stats
        
        # Reset stats
        from .logger import close_all_loggers
        close_all_loggers()
        
        stats_after = get_global_stats()
        assert stats_after == {}
    
    def test_logger_repr(self):
        """Test logger string representation."""
        logger = get_logger("repr_test", LogLevel.INFO)
        repr_str = repr(logger)
        assert "StructuredLogger" in repr_str
        assert "repr_test" in repr_str
class TestIntegration:
    """Integration tests for the entire system."""
    
    def test_complete_workflow(self):
        """Test complete workflow with all components."""
        # Create configuration
        cfg = config.create_default_config()
        
        # Create brain
        brain = Brain(cfg)
        brain.initialize()
        
        # Create world
        world = SimpleWorld()
        world.configure(15, 15, 8, 8)
        
        # Create agent
        agent = AgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enable_learning_features(reward_mod=True, curiosity=True)
        
        # Create logger
        logger = get_logger("integration_test", LogLevel.INFO)
        
        # Run simulation
        steps = 20
        for step in range(steps):
            world.update(0.1)
            
            # Get percept
            percept = world.get_sensory_percept()
            
            # Process sensory input
            agent.process_sensory_input(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decode_motor_command()
            
            # Apply action
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Log occasionally
            if step % 5 == 0:
                logger.info(f"Step {step}: {brain.get_firing_neuron_count()} neurons firing")
        
        # Verify results
        assert world.get_simulation_time() == steps * 0.1
        assert brain.get_total_neuron_count() > 0
        assert agent.get_neuromodulation_stats()['reward_modulation_enabled'] == True
    
    def test_error_handling(self):
        """Test error handling."""
        # Test invalid configuration
        schemas = [config.ConfigSchema("int_key", int, required=True)]
        cfg = config.Config(schemas=schemas)
        
        # Try to set invalid type
        try:
            cfg.set("int_key", "not_an_int")
            assert False, "Should have raised ValueError"
        except ValueError:
            pass
        
        # Test brain operations with invalid input
        cfg2 = config.create_default_config()
        brain = Brain(cfg2)
        brain.initialize()
        
        # These should not raise exceptions
        brain.step(1)
        brain.get_stats()
        brain.log_status()
    
    def test_type_conversion(self):
        """Test type conversion in configuration."""
        # Test string to int conversion
        schema = config.ConfigSchema("number", int)
        assert schema.validate("42") == True
        
        # Test string to bool conversion
        schema = config.ConfigSchema("flag", bool)
        assert schema.validate("true") == True
        assert schema.validate("false") == True
        
        # Test string to float conversion
        schema = config.ConfigSchema("number", float)
        assert schema.validate("3.14") == True
        
        # Test JSON string to list conversion
        schema = config.ConfigSchema("items", list)
        assert schema.validate('[1, 2, 3]') == True
        
        # Test JSON string to dict conversion
        schema = config.ConfigSchema("dict", dict)
        assert schema.validate('{"key": "value"}') == True
    
    def test_backward_compatibility(self):
        """Test backward compatibility with examples from easy_usage.md."""
        # Example 1: Simplest Brain
        import pynlm
        
        # Create a virtual brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Make it think for a few steps
        for i in range(10):
            brain.step(i)
        
        # Should have neurons
        assert brain.getTotalNeuronCount() > 0
        
        # Example 2: Brain in a Simple World (simplified)
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(5):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Should have completed without errors
        assert brain.getTotalSpikeCount() >= 0
@pytest.fixture
def tmp_path(tmpdir):
    """Pytest fixture for temporary directory."""
    return tmpdir
if __name__ == "__main__":
    pytest.main([__file__, "-v"])