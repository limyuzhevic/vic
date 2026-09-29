"""Python bindings tests for NLM.

Tests to verify that the Python bindings work correctly.
These tests require the pynlm module to be properly built and installed.
"""

import pynlm
import sys
import tempfile
import os


def test_version_attribute():
    """Test that version attributes are available."""
    assert hasattr(pynlm, "__version__")
    assert hasattr(pynlm, "__description__")
    assert hasattr(pynlm, "__author__")
    
    print(f"Version: {pynlm.__version__}")
    print(f"Description: {pynlm.__description__}")
    print(f"Author: {pynlm.__author__}")
    
    # Basic version format check (should be "0.1.0" or similar)
    assert isinstance(pynlm.__version__, str)
    assert len(pynlm.__version__) > 0


def test_constants():
    """Test that constants are available."""
    assert hasattr(pynlm, "INVALID_NEURON_ID")
    assert hasattr(pynlm, "INVALID_SYNAPSE_ID")
    assert hasattr(pynlm, "INVALID_REGION_ID")
    assert hasattr(pynlm, "INVALID_POPULATION_ID")
    
    print(f"INVALID_NEURON_ID: {pynlm.INVALID_NEURON_ID}")
    print(f"INVALID_SYNAPSE_ID: {pynlm.INVALID_SYNAPSE_ID}")
    print(f"INVALID_REGION_ID: {pynlm.INVALID_REGION_ID}")
    print(f"INVALID_POPULATION_ID: {pynlm.INVALID_POPULATION_ID}")


def test_basic_bindings():
    """Test basic Python bindings functionality."""
    # Create default config
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("Created default config")
    
    # Create a simple brain
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("Created brain")
    
    # Initialize the brain
    brain.initialize()
    print("Initialized brain")
    
    # Check brain properties
    neuron_count = brain.getTotalNeuronCount()
    assert neuron_count > 0
    print(f"Brain has {neuron_count} neurons")
    
    # Run a few simulation steps
    for i in range(5):
        brain.step(i)
    
    print("Completed simulation steps")
    
    # Check some statistics
    firing_count = brain.getFiringNeuronCount()
    total_spikes = brain.getTotalSpikeCount()
    print(f"Firing neurons: {firing_count}")
    print(f"Total spikes: {total_spikes}")


def test_world_bindings():
    """Test world-related Python bindings."""
    # Create a simple world
    world = pynlm.createSimpleWorld()
    assert world is not None
    print("Created simple world")
    
    # Configure the world
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    print("Configured world dimensions")
    
    # Reset world
    world.reset()
    print("Reset world")
    
    # Check world properties
    width = world.getWidth()
    height = world.getHeight()
    assert width == 10
    assert height == 10
    print(f"World size: {width}x{height}")


def test_agent_bindings():
    """Test agent-related Python bindings."""
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create an agent brain
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    print("Created agent brain")
    
    # Create a world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent with world
    agent.initialize(world)
    print("Initialized agent with world")
    
    # Test some agent methods
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    print(f"Curiosity level: {curiosity:.3f}")
    print(f"Novelty level: {novelty:.3f}")


def test_enum_types():
    """Test that enum types are accessible."""
    # Test neuron types
    assert hasattr(pynlm, "NeuronType")
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.NeuronType.Inhibitory is not None
    print("NeuronType enum accessible")
    
    # Test action types
    assert hasattr(pynlm, "ActionType")
    assert pynlm.ActionType.MoveForward is not None
    assert pynlm.ActionType.TurnLeft is not None
    print("ActionType enum accessible")
    
    # Test world object types
    assert hasattr(pynlm, "WorldObjectType")
    assert pynlm.WorldObjectType.Resource is not None
    assert pynlm.WorldObjectType.Wall is not None
    print("WorldObjectType enum accessible")


def test_config_operations():
    """Test configuration operations."""
    # Create config
    config = pynlm.createDefaultConfig()
    
    # Test config properties
    keys = config.getKeys()
    assert isinstance(keys, list)
    print(f"Config has {len(keys)} keys")
    
    # Test config summary
    summary = config.summary()
    assert isinstance(summary, str)
    print(f"Config summary: {summary}")


def run_all_tests():
    """Run all Python binding tests."""
    print("=== Running Python Binding Tests ===")
    print(f"Python bindings version: {pynlm.__version__}")
    print()
    
    test_version_attribute()
    print()
    
    test_constants()
    print()
    
    test_basic_bindings()
    print()
    
    test_world_bindings()
    print()
    
    test_agent_bindings()
    print()
    
    test_enum_types()
    print()
    
    test_config_operations()
    print()
    
    print("=== All tests passed! ===")


if __name__ == "__main__":
    try:
        run_all_tests()
    except Exception as e:
        print(f"Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)