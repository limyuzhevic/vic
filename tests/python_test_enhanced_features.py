"""
Comprehensive Python tests for NLM enhanced functionality
Tests for error handling, plugin system, and new features
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pybind11
from pybind11 import std::exception
import pytest

# Import the NLM module
from pynlm import (
    NLMException, ConfigException, ConfigLoadException, ConfigValidationException,
    BrainException, BrainInitializationException, SimulationStepException,
    ErrorCode, Result, Config, Brain, createDefaultConfig, createBrain, createSimpleWorld
)
def test_exception_hierarchy():
    """Test that the exception hierarchy is correct"""
    # Test that exceptions can be caught
    try:
        raise NLMException("Test base exception")
    except NLMException as e:
        assert "Test base exception" in str(e)
    
    try:
        raise ConfigException("Test config exception")
    except NLMException as e:
        assert "Config Error: Test config exception" in str(e)
    
    try:
        raise ConfigLoadException("test.json", "File not found")
    except ConfigException as e:
        assert "Failed to load config from test.json: File not found" in str(e)
    
    try:
        raise ConfigValidationException("param", "int", "string")
    except ConfigException as e:
        assert "Invalid value for key 'param': expected int, got string" in str(e)
    
    try:
        raise BrainException("Test brain exception")
    except NLMException as e:
        assert "Brain Error: Test brain exception" in str(e)


def test_error_code_enums():
    """Test error code enumeration"""
    assert ErrorCode.Success == 0
    assert ErrorCode.FileNotFound == 1
    assert ErrorCode.InvalidFormat == 2
    assert ErrorCode.OutOfMemory == 3
    assert ErrorCode.InvalidParameter == 4
    assert ErrorCode.InitializationFailed == 5
    assert ErrorCode.SimulationError == 6
    assert ErrorCode.ConfigError == 7
    assert ErrorCode.PluginError == 8
    assert ErrorCode.UnknownError == 9


def test_result_class():
    """Test the Result class functionality"""
    # Test successful result
    result = Result()
    assert result.success() == True
    assert result.errorCode() == ErrorCode.Success
    assert result.message() == "Success"
    
    # Test error result
    error_result = Result(ErrorCode.FileNotFound, "File not found")
    assert error_result.success() == False
    assert error_result.errorCode() == ErrorCode.FileNotFound
    assert error_result.message() == "File not found"
    
    # Test throwIfError method
    error_result.throwIfError()  # Should not raise
    
    error_result2 = Result(ErrorCode.InvalidParameter, "Invalid parameter")
    try:
        error_result2.throwIfError()
        assert False, "Should have raised exception"
    except NLMException as e:
        assert "Invalid parameter" in str(e)
    
    # Test valueOr method
    result.valueOr(42)
    error_result.valueOr(100)  # Should return default value
    
    # Test setValue method
    result2 = Result()
    result2.setValue("test_value")


def test_config_enhanced():
    """Test enhanced configuration functionality"""
    config = createDefaultConfig()
    
    # Test setting values
    config.set("test_int", 42)
    config.set("test_double", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Test getting values
    val_int = config.get("test_int")
    assert val_int.has_value() == True
    assert val_int.value() == 42
    
    val_double = config.get("test_double")
    assert val_double.has_value() == True
    assert val_double.value() > 3.13 and val_double.value() < 3.15
    
    val_string = config.get("test_string")
    assert val_string.has_value() == True
    assert val_string.value() == "hello"
    
    val_bool = config.get("test_bool")
    assert val_bool.has_value() == True
    assert val_bool.value() == True
    
    # Test getOr method
    val_missing = config.getOr("missing_key", 999)
    assert val_missing == 999
    
    # Test validation error handling
    try:
        config.set("invalid_key", "This should fail validation")
        assert False, "Should have raised ConfigValidationException"
    except ConfigValidationException:
        pass  # Expected
    
    # Test safe methods
    safe_result = config.setSafe("safe_int", 123)
    assert safe_result.success() == True
    
    error_result = config.setSafe("invalid_validation", "bad_value")
    assert error_result.success() == False
    assert error_result.errorCode() == ErrorCode.InvalidParameter


def test_config_serialization():
    """Test configuration serialization and deserialization"""
    config = createDefaultConfig()
    
    # Set various values
    config.set("int_val", 100)
    config.set("float_val", 3.14159)
    config.set("string_val", "test")
    config.set("bool_val", False)
    config.set("vector_int", {1, 2, 3, 4, 5})
    config.set("vector_float", {1.5, 2.5, 3.5})
    
    # Test summary
    summary = config.summary()
    assert "Configuration Summary:" in summary
    assert "int_val" in summary
    assert "100" in summary
    
    # Test saving (basic test - would require actual file)
    # This would test file I/O in a real environment


def test_error_handling_integration():
    """Test integration of error handling in configuration"""
    config = createDefaultConfig()
    
    # Test that loadFromFile throws appropriate exceptions
    try:
        config.loadFromFile("non_existent_file.json")
        assert False, "Should have raised ConfigLoadException"
    except ConfigLoadException as e:
        assert "Failed to load config from non_existent_file.json" in str(e)
    
    # Test safe load
    result = config.loadFromFileSafe("non_existent_file.json")
    assert result.success() == False
    assert result.errorCode() == ErrorCode.ConfigError


def test_plugin_system():
    """Test plugin system functionality"""
    from pynlm import PluginManager, PluginType, IPlugin, registerPlugin
    
    # Test plugin manager creation
    plugin_manager = PluginManager()
    assert plugin_manager is not None
    
    # Test plugin type enumeration
    assert PluginType.Plasticity == 0
    assert PluginType.Neuromodulator == 1
    assert PluginType.Development == 2
    assert PluginType.Memory == 3
    assert PluginType.Cognition == 4
    assert PluginType.Sensor == 5
    assert PluginType.Motor == 6
    assert PluginType.Environment == 7
    
    # Test getting plugin manager instance
    from pynlm import getPluginManager
    manager = getPluginManager()
    assert manager is not None


def test_comprehensive_workflow():
    """Test a comprehensive workflow using the enhanced features"""
    # Create components
    config = createDefaultConfig()
    world = createSimpleWorld()
    brain = createBrain(config)
    
    # Configure brain
    config.set("simulation_step", 1)
    config.set("timestep_duration", 0.001)
    config.set("max_neurons", 1000)
    config.set("enable_stdp", True)
    config.set("enable_hebbian", True)
    
    # Initialize brain
    brain.initialize()
    
    # Test brain operations
    regions = brain.getRegionIds()
    assert len(regions) == 0  # No regions yet
    
    total_neurons = brain.getTotalNeuronCount()
    assert total_neurons == 0  # No neurons yet
    
    # Log status
    brain.logStatus()
    
    # Test reset
    brain.reset()
    
    # Verify reset
    total_neurons_after_reset = brain.getTotalNeuronCount()
    assert total_neurons_after_reset == 0


def test_memory_usage():
    """Test memory usage tracking (placeholder for profiling tools)"""
    # This would test the profiling and debugging tools
    # For now, just a placeholder
    config = createDefaultConfig()
    assert config is not None


def run_all_tests():
    """Run all tests and return results"""
    test_results = []
    
    tests = [
        ("Exception Hierarchy", test_exception_hierarchy),
        ("Error Code Enums", test_error_code_enums),
        ("Result Class", test_result_class),
        ("Config Enhanced", test_config_enhanced),
        ("Config Serialization", test_config_serialization),
        ("Error Handling Integration", test_error_handling_integration),
        ("Plugin System", test_plugin_system),
        ("Comprehensive Workflow", test_comprehensive_workflow),
        ("Memory Usage", test_memory_usage),
    ]
    
    for test_name, test_func in tests:
        try:
            test_func()
            test_results.append((test_name, "PASSED"))
            print(f"✓ {test_name}: PASSED")
        except Exception as e:
            test_results.append((test_name, f"FAILED: {str(e)}"))
            print(f"✗ {test_name}: FAILED - {str(e)}")
    
    return test_results

if __name__ == "__main__":
    print("Running comprehensive NLM enhanced functionality tests...")
    print("=" * 60)
    
    results = run_all_tests()
    
    print("=" * 60)
    print("Test Summary:")
    
    passed = sum(1 for _, status in results if status == "PASSED")
    failed = len(results) - passed
    
    print(f"Total Tests: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed > 0:
        print("\nFailed Tests:")
        for test_name, status in results:
            if status != "PASSED":
                print(f"  - {test_name}: {status}")
        
        sys.exit(1)
    else:
        print("\nAll tests passed! ✓")
        sys.exit(0)