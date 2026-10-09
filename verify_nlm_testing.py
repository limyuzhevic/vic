#!/usr/bin/env python3

"""
Simple Test to Verify NLM Enhanced Components Test Suite Structure

This script provides a basic test to verify that the NLM enhanced components
test suite is properly structured and can be executed.
"""

import sys
import os

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import test framework
from tests.comprehensive.core.test_framework import NLMTestRunner, TestType

# Simple test function
def test_basic_functionality():
    """Test basic functionality of NLM enhanced components."""
    try:
        # Test configuration
        from pynlm import Config, create_default_config
        
        config = create_default_config()
        assert isinstance(config, Config)
        assert config.has("neuron_count")
        assert config.get("neuron_count") == 1000
        
        # Test type validation
        from pynlm.types import NeuronId, RegionId, validate_neuron_id, validate_region_id
        
        neuron_id = NeuronId(123)
        validated_neuron = validate_neuron_id(neuron_id)
        assert validated_neuron == neuron_id
        
        region_id = RegionId(456)
        validated_region = validate_region_id(region_id)
        assert validated_region == region_id
        
        return True
        
    except Exception as e:
        print(f"Test failed with error: {e}")
        return False

def test_config_functionality():
    """Test configuration functionality."""
    try:
        from pynlm import Config, create_default_config
        
        config = create_default_config()
        
        # Test setting values
        config.set("test_value", 42)
        assert config.get("test_value") == 42
        
        # Test fluent interface
        config.set("fluent_test1", "value1").set("fluent_test2", "value2")
        assert config.get("fluent_test1") == "value1"
        assert config.get("fluent_test2") == "value2"
        
        # Test configuration summary
        summary = config.summary()
        assert "neuron_count" in summary
        assert "region_count" in summary
        
        return True
        
    except Exception as e:
        print(f"Config test failed with error: {e}")
        return False

def test_type_validation():
    """Test type validation."""
    try:
        from pynlm.types import NeuronId, RegionId, SynapseId, PopulationId
        from pynlm.types import validate_neuron_id, validate_region_id, validate_synapse_id, validate_population_id
        
        # Test ID creation
        neuron_id = NeuronId(123)
        region_id = RegionId(456)
        synapse_id = SynapseId(789)
        population_id = PopulationId(999)
        
        # Test validation
        validated_neuron = validate_neuron_id(neuron_id)
        validated_region = validate_region_id(region_id)
        validated_synapse = validate_synapse_id(synapse_id)
        validated_population = validate_population_id(population_id)
        
        assert validated_neuron == neuron_id
        assert validated_region == region_id
        assert validated_synapse == synapse_id
        assert validated_population == population_id
        
        return True
        
    except Exception as e:
        print(f"Type validation test failed with error: {e}")
        return False

def test_memory_system_interface():
    """Test memory system interface."""
    try:
        from pynlm.memory import WorkingMemory, EpisodicMemory, AssociativeMemory
        
        # Test that memory classes can be imported
        assert WorkingMemory is not None
        assert EpisodicMemory is not None
        assert AssociativeMemory is not None
        
        return True
        
    except Exception as e:
        print(f"Memory system interface test failed with error: {e}")
        return False

def test_neuromodulation_interface():
    """Test neuromodulation interface."""
    try:
        from pynlm.neuromodulation import NeuromodulationSystem, Dopamine, Curiosity, Novelty, PredictionError
        
        # Test that neuromodulation classes can be imported
        assert NeuromodulationSystem is not None
        assert Dopamine is not None
        assert Curiosity is not None
        assert Novelty is not None
        assert PredictionError is not None
        
        return True
        
    except Exception as e:
        print(f"Neuromodulation interface test failed with error: {e}")
        return False

def main():
    """Run basic verification tests."""
    print("="*80)
    print("NLM Enhanced Components Test Suite - Basic Verification")
    print("="*80)
    
    # Create test runner
    test_runner = NLMTestRunner()
    
    # Register basic test functions
    test_functions = [
        (test_basic_functionality, TestType.CONFIGURATION),
        (test_config_functionality, TestType.CONFIGURATION),
        (test_type_validation, TestType.CONFIGURATION),
        (test_memory_system_interface, TestType.MEMORY),
        (test_neuromodulation_interface, TestType.NEUROSIMULATION),
    ]
    
    for test_func, test_type in test_functions:
        test_runner.register_test(
            test_func,
            test_type,
            test_name=test_func.__name__,
            tags=["verification", "basic"]
        )
    
    # Run tests
    print("\nRunning basic verification tests...")
    test_filter = [TestType.CONFIGURATION, TestType.MEMORY, TestType.NEUROSIMULATION]
    metrics = test_runner.run_all_tests(test_filter=test_filter, parallel=False)
    
    # Print results
    print("\n" + "="*80)
    print("VERIFICATION RESULTS")
    print("="*80)
    print(f"Total Tests: {metrics.total_tests}")
    print(f"Passed: {metrics.passed_tests}")
    print(f"Failed: {metrics.failed_tests}")
    print(f"Skipped: {metrics.skipped_tests}")
    print(f"Pass Rate: {metrics.pass_rate:.1f}%")
    print(f"Total Duration: {metrics.total_duration:.2f}s")
    print("="*80)
    
    if metrics.failed_tests == 0:
        print("SUCCESS: All basic verification tests passed!")
        return 0
    else:
        print("FAILURE: Some basic verification tests failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
