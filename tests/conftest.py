"""Test fixtures for enhanced NLM Python bindings."""
import pytest
import pynlm
import tempfile
import os
from pathlib import Path

@pytest.fixture
def config():
    """Create a default configuration for tests."""
    config = pynlm.Config()
    config.set("simulation_max_steps", 100)
    return config

@pytest.fixture
def brain(config):
    """Create and initialize a brain for tests."""
    brain = pynlm.Brain(config)
    brain.initialize()
    return brain

@pytest.fixture
def agent_brain(brain):
    """Create an agent brain for tests."""
    agent = pynlm.AgentBrain(brain)
    return agent

@pytest.fixture
def world():
    """Create a simple world for tests."""
    world = pynlm.SimpleWorld()
    return world

@pytest.fixture
def temp_file():
    """Create a temporary file for file operations."""
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.nlm') as f:
        temp_path = f.name
    yield temp_path
    os.unlink(temp_path)

@pytest.fixture
def cleanup_files():
    """Clean up created files after tests."""
    yield
    for file in Path(".").glob("*.nlm"):
        file.unlink(missing_ok=True)

# Helper functions for testing
def run_steps(brain, steps):
    """Helper to run multiple steps."""
    for _ in range(steps):
        brain.step()

def create_test_data(size=10):
    """Helper to create test data."""
    return [i for i in range(size)]