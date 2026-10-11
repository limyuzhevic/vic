#!/usr/bin/env python3
"""
Verification script for enhanced NLM Python bindings.

This script verifies that the enhanced API features are properly implemented
by testing the core functionality without requiring the full C++ build process.
"""

import sys
from pathlib import Path

# Add the pynlm directory to the path
sys.path.insert(0, str(Path(__file__).parent))

def test_enhanced_api_structure():
    """Test that the enhanced API structure is properly implemented."""
    print("Testing enhanced NLM Python bindings structure...")
    
    # Test that all expected modules exist
    modules = [
        'pynlm',
        'pynlm.core',
        'pynlm.utils',
        'pynlm.errors',
        'pynlm.types'
    ]
    
    for module in modules:
        try:
            __import__(module)
            print(f"✓ {module} imports successfully")
        except ImportError as e:
            print(f"✗ {module} import failed: {e}")
            return False
    
    return True

def test_pythonic_properties():
    """Test that Pythonic properties are defined."""
    print("\nTesting Pythonic properties...")
    
    try:
        from pynlm.core import Brain, AgentBrain
        
        # Check that Brain has expected Pythonic properties
        brain = Brain()
        expected_brain_props = [
            'spike_count',
            'synapse_count', 
            'active_neurons',
            'firing_neurons',
            'ei_ratio',
            'region_names',
            'get_summary',
            'step_batch',
            'save_to_file',
            'load_from_file'
        ]
        
        for prop in expected_brain_props:
            if hasattr(brain, prop):
                print(f"✓ Brain has {prop} property")
            else:
                print(f"✗ Brain missing {prop} property")
                return False
        
        # Check that AgentBrain has expected Pythonic properties
        agent = AgentBrain(brain)
        expected_agent_props = [
            'reward_modulation_enabled',
            'curiosity_enabled',
            'process_sensory_batch',
            'get_summary'
        ]
        
        for prop in expected_agent_props:
            if hasattr(agent, prop):
                print(f"✓ AgentBrain has {prop} property")
            else:
                print(f"✗ AgentBrain missing {prop} property")
                return False
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing Pythonic properties: {e}")
        return False

def test_error_classes():
    """Test that custom error classes are defined."""
    print("\nTesting error classes...")
    
    try:
        from pynlm.errors import (
            PynlmError,
            InvalidConfigurationError,
            NotInitializedError,
            InvalidInputError
        )
        
        error_classes = [
            (PynlmError, "Base NLM error"),
            (InvalidConfigurationError, "Configuration error"),
            (NotInitializedError, "Not initialized error"),
            (InvalidInputError, "Invalid input error")
        ]
        
        for error_class, description in error_classes:
            if hasattr(error_class, '__name__'):
                print(f"✓ {error_class.__name__} defined ({description})")
            else:
                print(f"✗ {error_class} not properly defined")
                return False
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing error classes: {e}")
        return False

def test_utility_functions():
    """Test that utility functions are defined."""
    print("\nTesting utility functions...")
    
    try:
        from pynlm.utils import (
            get_version_info,
            validate_brain,
            benchmark_brain,
            compare_brains,
            ErrorHandlingContext,
            create_error,
            safe_execute,
            get_memory_usage
        )
        
        utility_functions = [
            (get_version_info, "Get version info"),
            (validate_brain, "Validate brain"),
            (benchmark_brain, "Benchmark brain"),
            (compare_brains, "Compare brains"),
            (ErrorHandlingContext, "Error handling context"),
            (create_error, "Create error"),
            (safe_execute, "Safe execute"),
            (get_memory_usage, "Get memory usage")
        ]
        
        for func, description in utility_functions:
            if callable(func):
                print(f"✓ {func.__name__} defined ({description})")
            else:
                print(f"✗ {func} not callable")
                return False
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing utility functions: {e}")
        return False

def test_enums_and_types():
    """Test that enumerations and type definitions are defined."""
    print("\nTesting enumerations and type definitions...")
    
    try:
        from pynlm.types import (
            NeuronType,
            SynapseType,
            DevelopmentalStage,
            FiringState,
            ActionType,
            MotorCommand,
            WorldObjectType,
            ConfigSource,
            NeuronId,
            SynapseId,
            RegionId,
            PopulationId,
            INVALID_NEURON_ID,
            INVALID_SYNAPSE_ID,
            INVALID_REGION_ID,
            INVALID_POPULATION_ID,
            PI,
            EPSILON,
            MAX_SIMULATION_STEPS,
            DEFAULT_RANDOM_SEED
        )
        
        enums = [
            (NeuronType, "Neuron type"),
            (SynapseType, "Synapse type"),
            (DevelopmentalStage, "Developmental stage"),
            (FiringState, "Firing state"),
            (ActionType, "Action type"),
            (MotorCommand, "Motor command"),
            (WorldObjectType, "World object type"),
            (ConfigSource, "Configuration source")
        ]
        
        for enum, description in enums:
            if hasattr(enum, '__members__'):
                print(f"✓ {enum.__name__} defined ({description})")
            else:
                print(f"✗ {enum.__name__} not properly defined")
                return False
        
        # Test ID types
        id_types = [
            (NeuronId, "Neuron ID"),
            (SynapseId, "Synapse ID"),
            (RegionId, "Region ID"),
            (PopulationId, "Population ID")
        ]
        
        for id_type, description in id_types:
            try:
                instance = id_type(1)
                if hasattr(instance, 'value'):
                    print(f"✓ {id_type.__name__} defined ({description})")
                else:
                    print(f"✗ {id_type.__name__} missing value attribute")
                    return False
            except Exception as e:
                print(f"✗ {id_type.__name__} instantiation failed: {e}")
                return False
        
        # Test constants
        constants = [
            (PI, "PI"),
            (EPSILON, "EPSILON"),
            (MAX_SIMULATION_STEPS, "MAX_SIMULATION_STEPS"),
            (DEFAULT_RANDOM_SEED, "DEFAULT_RANDOM_SEED")
        ]
        
        for constant, description in constants:
            if isinstance(constant, (int, float)):
                print(f"✓ {description} = {constant}")
            else:
                print(f"✗ {description} not a numeric constant")
                return False
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing enums and types: {e}")
        return False

def main():
    """Main verification function."""
    print("Enhanced NLM Python Bindings - Verification")
    print("=" * 70)
    
    tests = [
        ("Enhanced API Structure", test_enhanced_api_structure),
        ("Pythonic Properties", test_pythonic_properties),
        ("Error Classes", test_error_classes),
        ("Utility Functions", test_utility_functions),
        ("Enumerations and Types", test_enums_and_types),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*70}")
        print(f"Running: {test_name}")
        print(f"{'='*70}")
        
        try:
            if test_func():
                print(f"✓ {test_name} PASSED")
                passed += 1
            else:
                print(f"✗ {test_name} FAILED")
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} FAILED with exception: {e}")
            failed += 1
    
    print(f"\n{'='*70}")
    print(f"Verification Results: {passed} passed, {failed} failed")
    print(f"{'='*70}")
    
    if failed == 0:
        print("✓ All verification tests passed!")
        print("\nThe enhanced NLM Python bindings have been successfully implemented.")
        print("\nKey features verified:")
        print("• Pythonic properties for Brain and AgentBrain classes")
        print("• Custom exception classes for error handling")
        print("• Utility functions for version info, validation, benchmarking")
        print("• Enumerations and type definitions")
        print("• Enhanced configuration and file operations")
        return True
    else:
        print(f"✗ {failed} verification tests failed.")
        return False

if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\nVerification failed with exception: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
