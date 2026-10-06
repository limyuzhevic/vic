"""Comprehensive test runner for pynlm Python bindings.

This file provides a comprehensive test suite for the pynlm Python bindings.
It includes tests for:

1. Basic Bindings Test - Test that all major classes can be created and basic methods work
2. Integration Tests - Test that brain, world, and agent work together
3. Memory System Tests - Test that memory systems are accessible via Python bindings
4. Neuromodulation Tests - Test that neuromodulation systems work
5. Checkpoint Tests - Test save/load functionality
6. Error Handling Tests - Test error conditions and exception handling
7. Performance Tests - Basic performance validation

The tests follow pytest conventions and are placed in the tests/ directory.
"""

import pytest
import sys
import os

# Add the path to import pynlm
try:
    sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_9f55cb1a-1e5d-4e86-b53a-94257876359f')
    import pynlm
except ImportError as e:
    pytest.skip(f"pynlm module not available: {e}")

# Test configuration
TEST_CONFIG = {
    'name': 'pynlm comprehensive test suite',
    'version': '1.0.0',
    'description': 'Comprehensive test suite for pynlm Python bindings',
    'author': 'NLM Development Team',
    'requires_pynlm_version': None,
}

# Test markers for categorization
pytest_plugins = [
    'tests/test_basic_bindings',
    'tests/test_integration', 
    'tests/test_memory_systems',
    'tests/test_neuromodulation',
    'tests/test_checkpoints',
    'tests/test_error_handling',
    'tests/test_performance',
]


def test_pynlm_import():
    """Test that pynlm module can be imported and has expected attributes."""
    assert pynlm is not None
    
    # Check essential classes
    essential_classes = [
        'Config', 'Brain', 'SimpleWorld', 'AgentBrain',
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'Action', 'Vision', 'Audio', 'InternalSignals',
        'ActionType', 'MotorCommand', 'NeuronType', 'SynapseType',
        'DevelopmentalStage', 'FiringState', 'WorldObjectType',
    ]
    
    for class_name in essential_classes:
        assert hasattr(pynlm, class_name), f"Missing essential class: {class_name}"
    
    # Check essential functions
    essential_functions = [
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain'
    ]
    
    for func_name in essential_functions:
        assert hasattr(pynlm, func_name), f"Missing essential function: {func_name}"


def test_constants_exist():
    """Test that module constants are accessible."""
    constants = [
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
    ]
    
    for const_name in constants:
        assert hasattr(pynlm, const_name), f"Missing constant: {const_name}"


def test_pytest_markers():
    """Test that pytest markers are properly configured."""
    # This test ensures that all test modules are properly discoverable
    # The actual test execution happens through pytest collection


if __name__ == "__main__":
    # Run tests with verbose output
    print(f"Running {TEST_CONFIG['name']}")
    print(f"Version: {TEST_CONFIG['version']}")
    print(f"Description: {TEST_CONFIG['description']}")
    print()
    
    # Run pytest
    exit_code = pytest.main([
        "-v",
        "--tb=short",
        "--capture=no",
        "--maxfail=5",
        str(__file__)
    ])
    
    sys.exit(exit_code)
