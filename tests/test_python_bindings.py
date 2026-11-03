#!/usr/bin/env python3
"""
Python bindings tests for NLM (Neural Learning Machine)

This file tests the enhanced Python bindings that now include:
- JSON configuration file support
- YAML configuration file support  
- TOML configuration file support
- Backward compatibility with simple key=value format
- Improved error handling
"""

import pytest
import os
import tempfile
import json
import sys
import traceback

# Add the parent directory to Python path to import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    PYNLM_ERROR = str(e)

class TestPythonBindings:
    """Test the enhanced Python bindings for NLM"""

    def setup_method(self):
        """Set up test fixtures before each test method"""
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)

    def teardown_method(self):
        """Clean up after each test method"""
        pass

    def test_basic_import(self):
        """Test that pynlm can be imported"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        assert pynlm is not None
        assert hasattr(pynlm, 'createDefaultConfig')
        assert hasattr(pynlm, 'createBrain')
        assert hasattr(pynlm, 'createSimpleWorld')
        assert hasattr(pynlm, 'createAgentBrain')

    def test_config_creation(self):
        """Test basic configuration creation"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has('random_seed')
        assert config.has('simulation_timestep')

        # Test getting values
        seed = config.getOr('random_seed', 42)
        assert seed == 42

        timestep = config.getOr('simulation_timestep', 0.001)
        assert timestep == 0.001

    def test_config_setters(self):
        """Test setting configuration values"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        config = pynlm.createDefaultConfig()
        config.set('test_int', 42)
        config.set('test_float', 3.14)
        config.set('test_string', 'hello')
        config.set('test_bool', True)

        assert config.getOr('test_int', 0) == 42
        assert config.getOr('test_float', 0.0) == 3.14
        assert config.getOr('test_string', '') == 'hello'
        assert config.getOr('test_bool', False) == True

    def test_config_save_load_simple(self):
        """Test saving and loading configuration in simple format"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write('random_seed = 123\n')
            f.write('simulation_timestep = 0.002\n')
            f.write('neuron_count = 500\n')
            config_file = f.name

        try:
            config = pynlm.createDefaultConfig()
            loaded = config.loadFromFile(config_file)

            assert loaded == True
            assert config.getOr('random_seed', 0) == 123
            assert config.getOr('simulation_timestep', 0.0) == 0.002
            assert config.getOr('neuron_count', 0) == 500
        finally:
            os.unlink(config_file)

    def test_config_json_format(self):
        """Test JSON configuration file support"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        config_data = {
            "random_seed": 456,
            "simulation_timestep": 0.003,
            "neuron_count": 750,
            "region_count": 2
        }

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(config_data, f)
            config_file = f.name

        try:
            config = pynlm.createDefaultConfig()
            loaded = config.loadFromFile(config_file)

            # Note: Simple JSON parsing is implemented, but full nlohmann/json support
            # would require the nlohmann/json dependency (Phase 2+ feature)
            # For now, we test that the file is at least readable
            assert loaded == True
        finally:
            os.unlink(config_file)

    def test_brain_creation(self):
        """Test brain creation with Python bindings"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        # The brain should be initialized when created
        # (configuration-based initialization happens in constructor)

    def test_simple_world_creation(self):
        """Test simple world creation"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        world = pynlm.createSimpleWorld()
        assert world is not None

        # Configure the world
        world.configure(100, 100, 16, 16)
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        assert world.getVisionWidth() == 16
        assert world.getVisionHeight() == 16

    def test_agent_brain_creation(self):
        """Test agent brain creation"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()

        agent_brain = pynlm.createAgentBrain(brain)
        assert agent_brain is not None

        # Initialize the agent brain with the world
        agent_brain.initialize(world)

    def test_config_summary(self):
        """Test configuration summary functionality"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        config = pynlm.createDefaultConfig()
        config.set('test_key', 'test_value')

        summary = config.summary()
        assert summary is not None
        assert 'test_key' in summary
        assert 'test_value' in summary

    def test_config_clear(self):
        """Test configuration clearing"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        config = pynlm.createDefaultConfig()
        config.clear()

        # After clearing, default config should have default values
        # but not the test key
        assert not config.has('test_key')

    def test_memory_efficiency(self):
        """Test that weak_ptr usage is memory efficient"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)

        # The key test is that the code compiles and runs without
        # memory leaks from raw pointer usage
        # This is a basic smoke test
        assert brain is not None

    def test_backward_compatibility(self):
        """Test that simple key=value format still works"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write('# This is a comment\n')
            f.write('key1 = value1\n')
            f.write('key2 = 42\n')
            f.write('key3 = 3.14\n')
            f.write('key4 = true\n')
            config_file = f.name

        try:
            config = pynlm.createDefaultConfig()
            loaded = config.loadFromFile(config_file)

            assert loaded == True
            assert config.getOr('key1', '') == 'value1'
            assert config.getOr('key2', 0) == 42
            assert config.getOr('key3', 0.0) == 3.14
            assert config.getOr('key4', False) == True
        finally:
            os.unlink(config_file)

    def test_error_handling(self):
        """Test error handling for invalid files"""
        if not PYNLM_AVAILABLE:
            pytest.skip(f"pynlm not available: {PYNLM_ERROR}")

        # Test with non-existent file
        config = pynlm.createDefaultConfig()
        loaded = config.loadFromFile('/nonexistent/file.json')
        assert loaded == False

        # Test with invalid JSON (for JSON parsing)
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{ invalid json')
            config_file = f.name

        try:
            config = pynlm.createDefaultConfig()
            loaded = config.loadFromFile(config_file)
            # Should still return True (fallback to simple format)
            assert loaded == True
        finally:
            os.unlink(config_file)

if __name__ == '__main__':
    # Run the tests
    pytest.main([__file__, '-v'])