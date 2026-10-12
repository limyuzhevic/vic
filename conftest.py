#!/usr/bin/env python3
"""
Test Fixtures for NLM Python Bindings

Common test fixtures and utilities used across all test files.
"""

import pytest
import pynlm
import pybind11.exceptions


@pytest.fixture(scope="class")
def default_config():
    """Create a default configuration for testing."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("brain.connection_probability", 0.1)
    return config


@pytest.fixture(scope="function")
def fresh_brain(default_config):
    """Create a fresh brain instance for each test."""
    brain = pynlm.createBrain(default_config)
    brain.initialize()
    return brain


@pytest.fixture(scope="function")
def fresh_world():
    """Create a fresh world instance for each test."""
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    return world


@pytest.fixture(scope="function")
def fresh_agent(fresh_brain, fresh_world):
    """Create a fresh agent instance for each test."""
    agent = pynlm.createAgentBrain(fresh_brain)
    agent.initialize(fresh_world)
    return agent


@pytest.fixture(scope="function")
def temporary_file():
    """Create a temporary file for serialization tests."""
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.json') as f:
        yield f.name
    os.unlink(f.name)


@pytest.fixture(scope="function")
def temporary_binary_file():
    """Create a temporary binary file for brain serialization tests."""
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as f:
        yield f.name
    os.unlink(f.name)


def assert_value_error(func, *args, **kwargs):
    """Assert that a function raises ValueError."""
    with pytest.raises(pybind11.exceptions.ValueError):
        func(*args, **kwargs)


def assert_config_validation(config):
    """Validate configuration with common test cases."""
    # Test valid value types
    config.set("brain.neuron_count", 100)  # int
    config.set("brain.synapse_density", 0.1)  # float
    config.set("plasticity.enabled", True)  # bool
    config.set("brain.name", "test_brain")  # string
    config.set("brain.weights", [0.1, 0.5, 0.9])  # list
    config.set("brain.mapping", {"key1": 0.5, "key2": 0.3})  # dict

    # Test error handling
    with pytest.raises(pybind11.exceptions.ValueError):
        config.set("brain.invalid_key", object())  # Unsupported type
    with pytest.raises(pybind11.exceptions.ValueError):
        config.set("", "value")  # Empty key
    with pytest.raises(pybind11.exceptions.ValueError):
        config.set("brain.key", None)  # None value


def assert_key_management(config):
    """Assert key management operations work correctly."""
    # Test has() method
    assert config.has("brain.neuron_count")
    assert not config.has("nonexistent.key")

    # Test getKeys() method
    keys = config.getKeys()
    assert isinstance(keys, list)
    assert len(keys) > 0

    # Test clear() method
    config.clear()
    assert len(config.getKeys()) == 0