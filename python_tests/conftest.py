import pytest
import pynlm
import numpy as np
from typing import Generator

@pytest.fixture
def default_config():
    """Create a default configuration for testing"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 2)
    return config

@pytest.fixture
def small_brain(default_config):
    """Create a small brain for fast unit tests"""
    brain = pynlm.createBrain(default_config)
    brain.initialize()
    return brain

@pytest.fixture
def medium_brain():
    """Create a medium-sized brain for integration tests"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 4)
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

@pytest.fixture
def test_world():
    """Create a test world for integration tests"""
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    return world

@pytest.fixture
def test_agent(brain, test_world):
    """Create a test agent for world integration tests"""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(test_world)
    return agent
@pytest.fixture
def cleanup():
    """Cleanup fixture to ensure test isolation"""
    yield
    # Force garbage collection
    import gc
    gc.collect()
