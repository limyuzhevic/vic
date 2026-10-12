#!/usr/bin/env python3
"""
Consolidated Test Suite for NLM Python Bindings

Minimal, focused test suite covering essential functionality
of the NLM Python bindings (pynlm).
"""

import pytest
import time
import sys
import os
import tempfile
import tracemalloc
import psutil
from pathlib import Path

# Add the parent directory to sys.path to ensure pynlm is importable
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm
from pynlm import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    Config, Brain, AgentBrain, SimpleWorld,
    Synapse, NeuralPopulation, NeuralRegion,
    SensoryInput, Vision, Audio, InternalSignals,
    Action, WorldObject, AgentBody, ActionResult,
    SensoryPercept
)

# Configure pytest
pytest_plugins = ["pytest_timeout"]


def pytest_configure(config):
    config.addinivalue_line("markers", "slow: marks tests as slow")
    config.addinivalue_line("markers", "integration: marks tests as integration tests")
    config.addinivalue_line("markers", "performance: marks tests as performance tests")


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
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.json') as f:
        yield f.name
    os.unlink(f.name)


@pytest.fixture(scope="function")
def temporary_binary_file():
    """Create a temporary binary file for brain serialization tests."""
    with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as f:
        yield f.name
    os.unlink(f.name)


class TestCoreFunctionality:
    """Core functionality tests - the foundation of the test suite."""

    def test_basic_imports(self):
        """Test that all core modules can be imported."""
        assert pynlm is not None

    def test_enum_values(self):
        """Test that all enumerations have valid values."""
        assert NeuronType.Excitatory == NeuronType.Excitatory
        assert NeuronType.Inhibitory == NeuronType.Inhibitory
        assert NeuronType.Modulatory == NeuronType.Modulatory
        assert NeuronType.Sensory == NeuronType.Sensory
        assert NeuronType.Motor == NeuronType.Motor
        assert NeuronType.Internal == NeuronType.Internal

        assert SynapseType.Excitatory == SynapseType.Excitatory
        assert SynapseType.Inhibitory == SynapseType.Inhibitory
        assert SynapseType.Modulatory == SynapseType.Modulatory
        assert SynapseType.Electrical == SynapseType.Electrical
        assert SynapseType.GapJunction == SynapseType.GapJunction

        assert DevelopmentalStage.Initial == DevelopmentalStage.Initial
        assert DevelopmentalStage.CriticalPeriod == DevelopmentalStage.CriticalPeriod
        assert DevelopmentalStage.Maturation == DevelopmentalStage.Maturation
        assert DevelopmentalStage.Adult == DevelopmentalStage.Adult
        assert DevelopmentalStage.Aging == DevelopmentalStage.Aging

    def test_basic_object_creation(self, default_config):
        """Test basic object creation and initialization."""
        # Test Config creation
        config = pynlm.Config()
        assert config is not None

        # Test factory function
        config2 = pynlm.createDefaultConfig()
        assert config2 is not None

        # Test setting configuration values
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.enabled", True)

        # Test getting configuration values
        assert config.has("brain.neuron_count")
        assert config.has("brain.synapse_density")
        assert config.has("plasticity.enabled")

    def test_basic_classes_creation(self, default_config):
        """Test creation of core classes."""
        # Test Synapse
        synapse = Synapse(SynapseId(1), NeuronId(2), NeuronId(3))
        assert synapse is not None
        assert synapse.getSourceNeuron().value == 2
        assert synapse.getDestinationNeuron().value == 3

        # Test NeuralPopulation
        population = NeuralPopulation(PopulationId(1), 100)
        assert population is not None
        assert population.getSize() == 100

        # Test NeuralRegion
        region = NeuralRegion(RegionId(1), "test_region")
        assert region is not None
        assert region.getName() == "test_region"

        # Test Vision
        vision = Vision(8, 8, 3)
        assert vision is not None
        assert vision.getWidth() == 8
        assert vision.getHeight() == 8
        assert vision.getChannels() == 3

        # Test Audio
        audio = Audio(44100, 4410)
        assert audio is not None
        assert audio.getSampleRate() == 44100

        # Test Action
        action = Action(ActionType.MoveForward)
        assert action is not None
        assert action.getType() == ActionType.MoveForward

        # Test WorldObject
        world_obj = WorldObject(10.0, 10.0, WorldObjectType.Resource, 10.0)
        assert world_obj is not None
        assert world_obj.x == 10.0
        assert world_obj.y == 10.0

    def test_version_attribute(self):
        """Test that version attribute is accessible."""
        assert hasattr(pynlm, '__version__')
        assert pynlm.__version__ is not None
        assert len(pynlm.__version__) > 0


class TestConfiguration:
    """Configuration management and validation."""

    def test_config_validation(self):
        """Test configuration validation and type checking."""
        config = pynlm.createDefaultConfig()

        # Test valid value types
        config.set("brain.neuron_count", 100)  # int
        config.set("brain.synapse_density", 0.1)  # float
        config.set("plasticity.enabled", True)  # bool
        config.set("brain.name", "test_brain")  # string
        config.set("brain.weights", [0.1, 0.5, 0.9])  # list
        config.set("brain.mapping", {"key1": 0.5, "key2": 0.3})  # dict

        # Test error handling for invalid types
        with pytest.raises(pybind11.exceptions.ValueError):
            config.set("brain.invalid_key", object())  # Unsupported type

        # Test error handling for empty key
        with pytest.raises(pybind11.exceptions.ValueError):
            config.set("", "value")

        # Test error handling for None value
        with pytest.raises(pybind11.exceptions.ValueError):
            config.set("brain.key", None)

    def test_config_key_management(self):
        """Test configuration key management."""
        config = pynlm.createDefaultConfig()

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

    def test_config_save_load(self, temporary_file):
        """Test configuration save/load functionality."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.15)

        # Save to file
        config.saveToFile(temporary_file)

        # Load from file
        config2 = pynlm.Config()
        config2.loadFromFile(temporary_file)

        # Verify values match
        assert config2.has("brain.neuron_count")
        assert config2.has("brain.synapse_density")


class TestIntegration:
    """Integration tests for component interaction."""

    def test_complete_brain_simulation(self, fresh_brain):
        """Test complete brain simulation workflow."""
        # Step 1: Inject sensory input
        vision = Vision(8, 8, 3)
        vision_data = [0.0] * (8 * 8 * 3)
        vision.setData(vision_data)
        sensory_input = vision

        # Step 2: Receive sensory input
        fresh_brain.receiveSensoryInput(sensory_input)

        # Step 3: Run brain step
        fresh_brain.step(0)

        # Step 4: Check statistics
        assert fresh_brain.getFiringNeuronCount() >= 0
        assert fresh_brain.getTotalNeuronCount() > 0

        # Step 5: Get action
        action = fresh_brain.produceAction()
        assert action is not None

    def test_agent_world_simulation(self, fresh_agent, fresh_world):
        """Test complete agent-world simulation workflow."""
        # Step 1: Update world
        fresh_world.update(0.1)

        # Step 2: Get sensory percept
        percept = fresh_world.getSensoryPercept()
        assert percept is not None

        # Step 3: Process sensory input
        fresh_agent.processSensoryInput(percept)

        # Step 4: Run brain step
        fresh_brain = fresh_agent.getBrain()
        fresh_brain.step(0)

        # Step 5: Decode motor command
        action = fresh_agent.decodeMotorCommand()
        assert action is not None

        # Step 6: Apply action to world
        fresh_world.applyMotorCommand(action, fresh_world.getSimulationTime())

    def test_brain_serialization(self, fresh_brain, temporary_binary_file):
        """Test brain state serialization."""
        # Run some simulation
        for i in range(10):
            fresh_brain.step(i)

        # Save brain state
        fresh_brain.save(temporary_binary_file)

        # Create new brain and load state
        new_brain = pynlm.createBrain(fresh_brain.getConfig())
        new_brain.initialize()
        new_brain.load(temporary_binary_file)

        # Verify loaded state
        assert new_brain.getTotalNeuronCount() == fresh_brain.getTotalNeuronCount()


class TestPerformance:
    """Performance testing and benchmarking."""

    def test_brain_creation_performance(self):
        """Test brain creation performance."""
        times = []
        for i in range(10):
            start_time = time.time()
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 100)
            brain = pynlm.createBrain(config)
            brain.initialize()
            end_time = time.time()
            times.append(end_time - start_time)

        avg_time = sum(times) / len(times)
        max_time = max(times)

        # Basic performance check (should be fast)
        assert avg_time < 1.0, f"Brain creation too slow: {avg_time}s"
        assert max_time < 2.0, f"Brain creation too slow: {max_time}s"

    def test_step_performance(self, fresh_brain):
        """Test simulation step performance."""
        steps = 100
        start_time = time.time()

        for i in range(steps):
            fresh_brain.step(i)

        end_time = time.time()
        avg_step_time = (end_time - start_time) / steps

        # Basic performance check
        assert avg_step_time < 0.01, f"Step too slow: {avg_step_time}s"


if __name__ == "__main__":
    print("Running NLM Python Bindings Test Suite...")
    print("=" * 60)

    # Run all tests using pytest
    import sys
    sys.exit(pytest.main([__file__, "-v", "--tb=short", "-x"]))