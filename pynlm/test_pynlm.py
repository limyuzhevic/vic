#!/usr/bin/env python3
"""
Python tests for NLM (Neural Learning Machine) Python bindings.

Tests the core functionality exposed through the Python bindings.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_import():
    """Test that the module can be imported successfully."""
    print("Testing basic import...")
    assert hasattr(pynlm, 'createDefaultConfig')
    assert hasattr(pynlm, 'createBrain')
    assert hasattr(pynlm, 'createSimpleWorld')
    assert hasattr(pynlm, 'createAgentBrain')
    print("✓ Basic import test passed")

def test_config():
    """Test configuration functionality."""
    print("Testing config functionality...")
    config = pynlm.createDefaultConfig()
    
    # Test basic operations
    assert config.has("test_key") == False
    
    config.set("test_int", 42)
    assert config.has("test_int") == True
    
    # Test get method
    # Note: The Python bindings might not expose the get method directly
    # We'll test what's available
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert len(summary) > 0
    
    print("✓ Config test passed")

def test_brain_creation():
    """Test brain creation and initialization."""
    print("Testing brain creation...")
    config = pynlm.createDefaultConfig()
    
    # Set some basic parameters
    config.set("brain.neuron_count", 100)
    config.set("brain.region_count", 1)
    
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Note: The Python bindings might not expose initialize method directly
    # We'll test what's available
    
    print("✓ Brain creation test passed")

def test_world_creation():
    """Test world creation."""
    print("Testing world creation...")
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    # Test configuration
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Test reset
    world.reset()
    
    print("✓ World creation test passed")

def test_agent_creation():
    """Test agent creation."""
    print("Testing agent creation...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    print("✓ Agent creation test passed")

def test_constants():
    """Test that constants are exposed."""
    print("Testing constants...")
    
    # Check that constants are exposed
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    print("✓ Constants test passed")

def test_enumerations():
    """Test that enumerations are exposed."""
    print("Testing enumerations...")
    
    # Check enumerations
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'SynapseType')
    assert hasattr(pynlm, 'DevelopmentalStage')
    assert hasattr(pynlm, 'FiringState')
    assert hasattr(pynlm, 'ActionType')
    assert hasattr(pynlm, 'MotorCommand')
    assert hasattr(pynlm, 'WorldObjectType')
    
    # Check some values
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.SynapseType.Excitatory is not None
    assert pynlm.DevelopmentalStage.Initial is not None
    assert pynlm.FiringState.Resting is not None
    
    print("✓ Enumerations test passed")

def test_wrapper_classes():
    """Test the Python wrapper classes."""
    print("Testing wrapper classes...")
    
    # Test DefaultConfig wrapper
    config = pynlm.DefaultConfig()
    assert config is not None
    
    # Test Simulation wrapper
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    simulation = pynlm.Simulation(brain, world, agent)
    assert simulation is not None
    
    print("✓ Wrapper classes test passed")

def test_version():
    """Test version information."""
    print("Testing version...")
    
    assert hasattr(pynlm, '__version__')
    assert isinstance(pynlm.__version__, str)
    
    print(f"✓ Version test passed (version {pynlm.__version__})")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_import,
        test_constants,
        test_enumerations,
        test_config,
        test_brain_creation,
        test_world_creation,
        test_agent_creation,
        test_wrapper_classes,
        test_version,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)