"""
Comprehensive test suite for NLM Python bindings.
Tests all the improvements made to the Python package.
"""

import sys
import os
import tempfile
import json
from typing import Dict, Any, List
import pytest

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(__file__))

from nlm import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, ActionType, MotorCommand,
    Config, Brain, SimpleWorld, AgentBrain,
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    NLMError, NLMConfigurationError, NLMValidationError,
    NLMMemoryError, NLMIndexError, NLMTypeError,
    NLMRuntimeError, NLMNotImplementedError,
    NLMValueError, NLMKeyError, NLMFileError,
    NLMResourceError, NLMPerformanceError,
    InputValidator, ErrorRecovery,
    validate_brain_config, validate_world_parameters,
    validate_sensory_data, validate_motor_action,
)

class TestCoreTypes:
    """Test core type classes and enumerations."""
    
    def test_neuron_id(self):
        """Test NeuronId functionality."""
        # Test default constructor
        neuron_id = NeuronId()
        assert neuron_id.value == 0
        assert neuron_id.index() == 0
        
        # Test parameterized constructor
        neuron_id = NeuronId(42)
        assert neuron_id.value == 42
        assert neuron_id.index() == 42
        
        # Test equality
        neuron_id1 = NeuronId(1)
        neuron_id2 = NeuronId(1)
        neuron_id3 = NeuronId(2)
        assert neuron_id1 == neuron_id2
        assert neuron_id1 != neuron_id3
        
        # Test hashing
        ids = {neuron_id1, neuron_id2, neuron_id3}
        assert len(ids) == 2
        
        # Test string representation
        assert repr(neuron_id1) == "<NeuronId: 1>"
    
    def test_synapse_id(self):
        """Test SynapseId functionality."""
        synapse_id = SynapseId(99)
        assert synapse_id.value == 99
        assert synapse_id.index() == 99
        
        # Test equality and hashing
        synapse_id1 = SynapseId(5)
        synapse_id2 = SynapseId(5)
        assert synapse_id1 == synapse_id2
        assert hash(synapse_id1) == hash(synapse_id2)
    
    def test_region_id(self):
        """Test RegionId functionality."""
        region_id = RegionId(100)
        assert region_id.value == 100
        assert region_id.index() == 100
    
    def test_population_id(self):
        """Test PopulationId functionality."""
        population_id = PopulationId(50)
        assert population_id.value == 50
        assert population_id.index() == 50
    
    def test_neuron_type_enum(self):
        """Test NeuronType enumeration."""
        assert NeuronType.Excitatory.value == 0
        assert NeuronType.Inhibitory.value == 1
        assert NeuronType.Modulatory.value == 2
        assert NeuronType.Sensory.value == 3
        assert NeuronType.Motor.value == 4
        assert NeuronType.Internal.value == 5
        
        # Test enum export
        from nlm.bindings import NeuronType as CNeuronType
        assert NeuronType.Excitatory == CNeuronType.Excitatory
    
    def test_synapse_type_enum(self):
        """Test SynapseType enumeration."""
        assert SynapseType.Excitatory.value == 0
        assert SynapseType.Inhibitory.value == 1
        assert SynapseType.Modulatory.value == 2
        assert SynapseType.Electrical.value == 3
        assert SynapseType.GapJunction.value == 4
    
    def test_action_type_enum(self):
        """Test ActionType enumeration."""
        assert ActionType.MoveForward.value == 0
        assert ActionType.MoveBackward.value == 1
        assert ActionType.TurnLeft.value == 2
        assert ActionType.TurnRight.value == 3
    
    def test_motor_command_enum(self):
        """Test MotorCommand enumeration."""
        assert MotorCommand.MoveForward.value == 0
        assert MotorCommand.TurnLeft.value == 1
        assert MotorCommand.TurnRight.value == 2

class TestConfiguration:
    """Test configuration functionality."""
    
    def test_config_creation(self):
        """Test Config creation."""
        config = Config()
        assert config is not None
        
        # Test default config has no keys
        assert len(config.getKeys()) == 0
        
        # Test repr
        assert "<Config:" in repr(config)
    
    def test_config_factory_functions(self):
        """Test factory functions."""
        config = createDefaultConfig()
        assert config is not None
        assert isinstance(config, Config)
        
        # Test that config has expected keys
        keys = config.getKeys()
        assert isinstance(keys, list)
    
    def test_config_save_load(self):
        """Test saving and loading configuration."""
        config = createDefaultConfig()
        
        # Add some test data
        config.set("test_key", "test_value")
        config.set("number", 42)
        config.set("list", [1, 2, 3])
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            config.saveToFile(filepath)
            
            # Load into new config
            new_config = createDefaultConfig()
            new_config.loadFromFile(filepath)
            
            # Check that data was preserved
            assert new_config.get("test_key") == "test_value"
            assert new_config.get("number") == 42
            assert new_config.get("list") == [1, 2, 3]
            
        finally:
            os.unlink(filepath)
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = createDefaultConfig()
        
        # Add invalid values
        config.set("learning_rate", 1.5)  # Too high
        config.set("neuron_count", -100)  # Negative
        
        # Test validation using nlm validation
        config_dict = {
            "learning_rate": 1.5,
            "neuron_count": -100,
            "world_width": 100.0,
            "world_height": 100.0
        }
        
        errors = validate_brain_config(config_dict)
        assert len(errors) > 0
        assert any("learning_rate" in error for error in errors)
        assert any("neuron_count" in error for error in errors)

class TestExceptionHierarchy:
    """Test exception hierarchy and handling."""
    
    def test_exception_inheritance(self):
        """Test exception inheritance."""
        assert issubclass(NLMConfigurationError, NLMError)
        assert issubclass(NLMValidationError, NLMError)
        assert issubclass(NLMMemoryError, NLMError)
        assert issubclass(NLMTypeError, NLMError)
        assert issubclass(NLMRuntimeError, NLMError)
        assert issubclass(NLMValueError, NLMError)
        assert issubclass(NLMKeyError, NLMError)
        assert issubclass(NLMFileError, NLMError)
        assert issubclass(NLMResourceError, NLMError)
        assert issubclass(NLMPerformanceError, NLMError)
    
    def test_exception_creation(self):
        """Test exception creation."""
        # Test basic exception
        error = NLMError("Test error")
        assert str(error) == "Test error"
        assert error.error_code is None
        
        # Test exception with error code
        error = NLMError("Test error", error_code="TEST_ERROR")
        assert str(error) == "[TEST_ERROR] Test error"
        
        # Test configuration error
        config_error = NLMConfigurationError("Config error", filepath="/test/path")
        assert config_error.filepath == "/test/path"
        assert config_error.error_code == "CONFIG_ERROR"
        
        # Test validation error
        val_error = NLMValidationError("Validation error", field="learning_rate", value=2.0)
        assert val_error.field == "learning_rate"
        assert val_error.value == 2.0
        assert val_error.error_code == "VALIDATION_ERROR"
        
        # Test context management
        error = NLMError("Test")
        error.add_context(key1="value1", key2="value2")
        assert error.context["key1"] == "value1"
        assert error.context["key2"] == "value2"
    
    def test_exception_helper_functions(self):
        """Test exception helper functions."""
        # Test raise_config_error
        try:
            raise_config_error("Config error", filepath="/test")
            assert False, "Should have raised NLMConfigurationError"
        except NLMConfigurationError as e:
            assert e.filepath == "/test"
        
        # Test raise_validation_error
        try:
            raise_validation_error("Validation error", field="test", value="bad")
            assert False, "Should have raised NLMValidationError"
        except NLMValidationError as e:
            assert e.field == "test"
            assert e.value == "bad"
        
        # Test raise_type_error
        try:
            raise_type_error("Type error")
            assert False, "Should have raised NLMTypeError"
        except NLMTypeError:
            pass
        
        # Test raise_runtime_error
        try:
            raise_runtime_error("Runtime error")
            assert False, "Should have raised NLMRuntimeError"
        except NLMRuntimeError:
            pass
    
    def test_exception_from_cpp(self):
        """Test conversion from C++ exceptions."""
        from nlm import create_exception_from_cpp
        
        # Test runtime error conversion
        cpp_error = ValueError("Runtime error")
        py_error = create_exception_from_cpp(cpp_error, {"context": "test"})
        assert isinstance(py_error, NLMRuntimeError)
        assert str(py_error) == "Runtime error"
        
        # Test invalid argument conversion
        cpp_error = TypeError("Invalid argument")
        py_error = create_exception_from_cpp(cpp_error)
        assert isinstance(py_error, NLMError)
        
        # Test unknown exception
        cpp_error = RuntimeError("Unknown error")
        py_error = create_exception_from_cpp(cpp_error)
        assert isinstance(py_error, NLMError)

class TestInputValidator:
    """Test input validation functionality."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.validator = InputValidator()
    
    def test_basic_validation(self):
        """Test basic validation functionality."""
        # Test neuron ID validation
        errors = self.validator.validate('neuron_id', 1)
        assert errors == []  # Valid
        
        errors = self.validator.validate('neuron_id', -1)
        assert len(errors) > 0
        
        # Test synapse ID validation
        errors = self.validator.validate('synapse_id', 0)
        assert errors == []
        
        errors = self.validator.validate('synapse_id', -1)
        assert len(errors) > 0
    
    def test_coordinate_validation(self):
        """Test coordinate validation."""
        errors = self.validator.validate('coordinates', {'x': 10, 'y': 20})
        assert errors == []  # Valid
        
        errors = self.validator.validate('coordinates', -1)
        assert len(errors) > 0
    
    def test_timestep_validation(self):
        """Test timestep validation."""
        errors = self.validator.validate('timestep', 0.01)
        assert errors == []  # Valid
        
        errors = self.validator.validate('timestep', 0)
        assert len(errors) > 0
        
        errors = self.validator.validate('timestep', 2.0)
        assert len(errors) > 0
    
    def test_probability_validation(self):
        """Test probability validation."""
        errors = self.validator.validate('probability', 0.5)
        assert errors == []  # Valid
        
        errors = self.validator.validate('probability', -0.1)
        assert len(errors) > 0
        
        errors = self.validator.validate('probability', 1.5)
        assert len(errors) > 0
    
    def test_array_size_validation(self):
        """Test array size validation."""
        errors = self.validator.validate('array_size', [1, 2, 3, 4, 5])
        assert errors == []  # Valid
        
        errors = self.validator.validate('array_size', 1000001)  # Too large
        assert len(errors) > 0
    
    def test_learning_rate_validation(self):
        """Test learning rate validation."""
        errors = self.validator.validate('learning_rate', 0.01)
        assert errors == []  # Valid
        
        errors = self.validator.validate('learning_rate', -0.1)
        assert len(errors) > 0
        
        errors = self.validator.validate('learning_rate', 2.0)
        assert len(errors) > 0
    
    def test_validation_all(self):
        """Test validate_all method."""
        values = {
            'neuron_count': 100,
            'learning_rate': 0.01,
            'timestep': 0.01,
            'temperature': 1.0
        }
        
        results = self.validator.validate_all(values)
        
        # All should be valid
        assert all(len(errors) == 0 for errors in results.values())
        
        # Test with invalid values
        values['learning_rate'] = 1.5
        results = self.validator.validate_all(values)
        assert len(results['learning_rate']) > 0

class TestErrorRecovery:
    """Test error recovery functionality."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.recovery = ErrorRecovery()
    
    def test_recovery_strategies(self):
        """Test recovery strategies."""
        # Test memory error recovery
        memory_error = NLMMemoryError("Out of memory", memory_type="neurons")
        recovery_result = self.recovery.recover(memory_error, {"neuron_count": 100})
        # Should return reduced neuron count
        assert recovery_result is not None
        
        # Test validation error recovery
        val_error = NLMValidationError("Invalid value", field="learning_rate", value=2.0)
        recovery_result = self.recovery.recover(val_error)
        # Should return default value for learning_rate
        assert recovery_result == 0.01
        
        # Test index error recovery
        index_error = NLMIndexError("Out of bounds", index=10, max_index=5)
        recovery_result = self.recovery.recover(index_error)
        # Should return closest valid index
        assert recovery_result == 4
        
        # Test file error recovery
        file_error = NLMFileError("File not found", filepath="/test/file.json")
        recovery_result = self.recovery.recover(file_error)
        # Should return None for file error (no backup)
        assert recovery_result is None
    
    def test_unknown_error(self):
        """Test handling of unknown error types."""
        # Create a custom error that's not in the registry
        class CustomError(NLMError):
            pass
        
        custom_error = CustomError("Custom error")
        recovery_result = self.recovery.recover(custom_error)
        # Should return None for unknown error types
        assert recovery_result is None
    
    def test_try_with_recovery(self):
        """Test try_with_recovery method."""
        # Create a function that fails once then succeeds
        call_count = 0
        
        def failing_function():
            nonlocal call_count
            call_count += 1
            if call_count == 1:
                raise NLMMemoryError("Out of memory")
            return "success"
        
        # Test recovery
        result = self.recovery.try_with_recovery(failing_function, max_attempts=3)
        
        # Should eventually succeed (recovery might reduce memory usage)
        assert result == "success"
        assert call_count == 2  # First attempt fails, second succeeds

class TestValidationFunctions:
    """Test validation functions."""
    
    def test_validate_brain_config(self):
        """Test brain configuration validation."""
        # Valid configuration
        valid_config = {
            "learning_rate": 0.01,
            "neuron_count": 1000,
            "world_width": 100.0,
            "world_height": 100.0,
            "region_count": 5
        }
        
        errors = validate_brain_config(valid_config)
        assert len(errors) == 0
        
        # Invalid configuration
        invalid_config = {
            "learning_rate": 1.5,  # Too high
            "neuron_count": -100,  # Negative
            "world_width": 100.0,
            "world_height": 100.0,
            "region_count": 5
        }
        
        errors = validate_brain_config(invalid_config)
        assert len(errors) > 0
        assert any("learning_rate" in error for error in errors)
        assert any("neuron_count" in error for error in errors)
    
    def test_validate_world_parameters(self):
        """Test world parameters validation."""
        # Valid parameters
        errors = validate_world_parameters(100.0, 100.0, 20.0, 20.0)
        assert len(errors) == 0
        
        # Invalid parameters
        errors = validate_world_parameters(-1.0, 100.0, 20.0, 20.0)
        assert len(errors) > 0
        assert any("width" in error for error in errors)
        
        errors = validate_world_parameters(100.0, 100.0, 150.0, 20.0)
        assert len(errors) > 0
        assert any("vision_width" in error for error in errors)
    
    def test_validate_sensory_data(self):
        """Test sensory data validation."""
        # Valid data
        errors = validate_sensory_data([0.1, 0.2, 0.3], 3)
        assert len(errors) == 0
        
        # Invalid data - wrong dimensions
        errors = validate_sensory_data([0.1, 0.2], 3)
        assert len(errors) > 0
        
        # Invalid data - wrong type
        errors = validate_sensory_data([0.1, "invalid", 0.3], 3)
        assert len(errors) > 0
        
        # Invalid data - out of range
        errors = validate_sensory_data([0.1, 1.5, 0.3], 3)
        assert len(errors) > 0
    
    def test_validate_motor_action(self):
        """Test motor action validation."""
        # Valid action
        errors = validate_motor_action("move", {"dx": 1.0, "dy": 2.0})
        assert len(errors) == 0
        
        # Invalid action type
        errors = validate_motor_action("invalid_action", {"dx": 1.0, "dy": 2.0})
        assert len(errors) > 0
        
        # Invalid parameters for move action
        errors = validate_motor_action("move", {"dx": "invalid", "dy": 2.0})
        assert len(errors) > 0

def run_tests():
    """Run all tests."""
    print("Running NLM Python Bindings Tests")
    print("=" * 50)
    
    test_classes = [
        TestCoreTypes,
        TestConfiguration,
        TestExceptionHierarchy,
        TestInputValidator,
        TestErrorRecovery,
        TestValidationFunctions,
    ]
    
    passed = 0
    failed = 0
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        print("-" * 30)
        
        test_instance = test_class()
        
        for method_name in dir(test_instance):
            if method_name.startswith('test_'):
                method = getattr(test_instance, method_name)
                
                # Setup if available
                if hasattr(test_instance, 'setup_method'):
                    test_instance.setup_method()
                
                try:
                    method()
                    print(f"✓ {method_name}")
                    passed += 1
                except Exception as e:
                    print(f"✗ {method_name}: {e}")
                    failed += 1
    
    print("\n" + "=" * 50)
    print(f"Summary: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("✓ All tests passed!")
        return 0
    else:
        print("✗ Some tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(run_tests())