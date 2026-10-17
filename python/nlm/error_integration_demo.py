#!/usr/bin/env python3
"""
Comprehensive Error Handling and Type Safety Implementation

This script demonstrates the integration of comprehensive error handling and type safety
across all NLM Python bindings modules. It implements:

1. Custom Exception Hierarchy with 8 exception types
2. Type Safety Features with runtime checking
3. Error Handling Strategies with recovery mechanisms
4. Validation Framework for module-specific validation
5. Defensive Programming utilities
6. Python Integration (PEP 484, PEP 526 compliance)
7. Backward compatibility with existing code

The implementation follows the requirements from the integration request and
is designed to be used across all NLM modules:
- Core bindings (Brain, Config, AgentBrain, SimpleWorld, etc.)
- Statistical analysis module
- Machine learning utilities
- Visualization tools
- Development tools
- Profiling tools
"""

import sys
import os
from typing import Any, Dict, List, Optional, Type, Union
from pathlib import Path

# Add the error handler to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'nlm'))

# Import the error handling framework
from error_handler import (
    # Custom Exception Hierarchy
    NLMException, NLMConfigError, NLMBrainError, NLMWorldError,
    NLMMemoryError, NLMPrecisionError, NLMFileError, NLMValidationError,
    NLMResourceError, NLMSimulationError,
    
    # Validation Framework
    TypeValidator, ValueValidator, ParameterValidator,
    ErrorHandler, DefensiveProgramming, ValidationFramework,
    TypeSafetyChecker, NLMValidator,
    
    # Decorators
    validate_types, validate_range, safe_operation,
    
    # Global instances
    get_validator, set_validator, create_default_validator,
    validate_parameter, validate_function_call, error_context,
    
    # Utility functions
    get_type_info, get_validation_error_chain, format_validation_report
)

class ErrorHandlingExample:
    """Example class demonstrating comprehensive error handling and type safety."""
    
    def __init__(self):
        self.validator = NLMValidator(strict_mode=True, log_level="INFO")
        self.error_handler = ErrorHandler("INFO", enable_recovery=True)
        self.defensive = DefensiveProgramming()
        self.validation_framework = ValidationFramework(True)
        
        # Add recovery handlers
        self.error_handler.add_recovery_handler(
            NLMConfigError, self._handle_config_error
        )
        self.error_handler.add_recovery_handler(
            NLMValidationError, self._handle_validation_error
        )
    
    def _handle_config_error(self, error: NLMConfigError, context: Dict[str, Any]) -> Any:
        """Recovery handler for configuration errors."""
        print(f"Recovering from config error: {error}")
        return None
    
    def _handle_validation_error(self, error: NLMValidationError, context: Dict[str, Any]) -> Any:
        """Recovery handler for validation errors."""
        print(f"Recovering from validation error: {error}")
        return None
    
    def demonstrate_exception_hierarchy(self):
        """Demonstrate the custom exception hierarchy."""
        print("\n1. Exception Hierarchy Demonstration")
        print("-" * 40)
        
        exceptions = [
            ("NLMConfigError", NLMConfigError("Configuration error", error_code="CFG_001")),
            ("NLMBrainError", NLMBrainError("Brain initialization error", error_code="BRAIN_001")),
            ("NLMWorldError", NLMWorldError("World simulation error", error_code="WORLD_001")),
            ("NLMMemoryError", NLMMemoryError("Memory management error", error_code="MEM_001")),
            ("NLMPrecisionError", NLMPrecisionError("Numerical precision error", error_code="PREC_001")),
            ("NLMFileError", NLMFileError("File I/O error", error_code="FILE_001")),
            ("NLMValidationError", NLMValidationError("Input validation error", error_code="VAL_001")),
            ("NLMResourceError", NLMResourceError("Resource error", error_code="RES_001")),
            ("NLMSimulationError", NLMSimulationError("Simulation error", error_code="SIM_001")),
        ]
        
        for name, exc in exceptions:
            try:
                raise exc
            except NLMException as e:
                print(f"  ✓ {name}: {e}")
    
    def demonstrate_type_safety(self):
        """Demonstrate type safety features."""
        print("\n2. Type Safety Demonstration")
        print("-" * 40)
        
        validator = TypeValidator()
        
        # Type validation
        print("  Type validation:")
        try:
            result = validator.check_type(42, int, "test_param")
            print(f"    ✓ check_type: {result}")
        except NLMException as e:
            print(f"    ✗ check_type: {e}")
        
        # Optional type validation
        print("  Optional type validation:")
        try:
            result = validator.check_optional_type(None, int, "optional_param")
            print(f"    ✓ check_optional_type: {result}")
        except NLMException as e:
            print(f"    ✗ check_optional_type: {e}")
        
        try:
            result = validator.check_optional_type(42, int, "optional_param")
            print(f"    ✓ check_optional_type: {result}")
        except NLMException as e:
            print(f"    ✗ check_optional_type: {e}")
        
        # List type validation
        print("  List type validation:")
        try:
            result = validator.check_list_type([1, 2, 3], int, "list_param")
            print(f"    ✓ check_list_type: {result}")
        except NLMException as e:
            print(f"    ✗ check_list_type: {e}")
        
        # Dict type validation
        print("  Dict type validation:")
        try:
            result = validator.check_dict_type(
                {"key": "value"}, str, str, "dict_param"
            )
            print(f"    ✓ check_dict_type: {result}")
        except NLMException as e:
            print(f"    ✗ check_dict_type: {e}")
    
    def demonstrate_value_validation(self):
        """Demonstrate value validation."""
        print("\n3. Value Validation Demonstration")
        print("-" * 40)
        
        validator = ValueValidator()
        
        # Range validation
        print("  Range validation:")
        try:
            result = validator.check_range(50, 0, 100, "range_param")
            print(f"    ✓ check_range: {result}")
        except NLMException as e:
            print(f"    ✗ check_range: {e}")
        
        # Positive validation
        print("  Positive validation:")
        try:
            result = validator.check_positive(42, "positive_param")
            print(f"    ✓ check_positive: {result}")
        except NLMException as e:
            print(f"    ✗ check_positive: {e}")
        
        # Non-negative validation
        print("  Non-negative validation:")
        try:
            result = validator.check_non_negative(0, "non_negative_param")
            print(f"    ✓ check_non_negative: {result}")
        except NLMException as e:
            print(f"    ✗ check_non_negative: {e}")
        
        # String validation
        print("  String validation:")
        try:
            result = validator.check_string_not_empty("test", "string_param")
            print(f"    ✓ check_string_not_empty: {result}")
        except NLMException as e:
            print(f"    ✗ check_string_not_empty: {e}")
        
        # Array validation
        print("  Array validation:")
        try:
            result = validator.check_array_not_empty([1, 2, 3], "array_param")
            print(f"    ✓ check_array_not_empty: {result}")
        except NLMException as e:
            print(f"    ✗ check_array_not_empty: {e}")
    
    def demonstrate_parameter_validation(self):
        """Demonstrate parameter validation framework."""
        print("\n4. Parameter Validation Framework")
        print("-" * 40)
        
        validator = ParameterValidator()
        
        # Add validation rules
        validator.add_rule("param1", "type", {"type": int}, "Must be an integer")
        validator.add_rule("param2", "range", {"min": 0, "max": 100}, "Must be between 0 and 100")
        validator.add_rule("param3", "not_empty", {}, "Must not be empty")
        
        # Validate parameters
        print("  Parameter validation:")
        try:
            result = validator.validate_all(
                param1=42,
                param2=50,
                param3="test"
            )
            print(f"    ✓ validate_all: {result}")
        except NLMException as e:
            print(f"    ✗ validate_all: {e}")
        
        # Clear rules and add new ones
        validator.clear_rules()
        validator.add_rule("param1", "type", {"type": str}, "Must be a string")
        
        try:
            result = validator.validate_all(param1="test")
            print(f"    ✓ validate_all (cleared): {result}")
        except NLMException as e:
            print(f"    ✗ validate_all (cleared): {e}")
    
    def demonstrate_defensive_programming(self):
        """Demonstrate defensive programming utilities."""
        print("\n5. Defensive Programming")
        print("-" * 40)
        
        defensive = DefensiveProgramming()
        
        # Null check
        print("  Null check:")
        try:
            result = defensive.null_check(None, "param", allow_none=True)
            print(f"    ✓ null_check (allow_none): {result}")
        except NLMException as e:
            print(f"    ✗ null_check: {e}")
        
        # File existence check
        print("  File existence check:")
        try:
            result = defensive.file_existence_check(
                "/tmp/test_file.txt", 
                "read operation",
                create_if_missing=True
            )
            print(f"    ✓ file_existence_check: {result}")
        except NLMException as e:
            print(f"    ✗ file_existence_check: {e}")
        
        # Resource availability check
        print("  Resource availability check:")
        try:
            result = defensive.resource_availability_check(
                "gpu", {"cpu", "gpu"}, "compute operation"
            )
            print(f"    ✓ resource_availability_check: {result}")
        except NLMException as e:
            print(f"    ✗ resource_availability_check: {e}")
    
    def demonstrate_validation_framework(self):
        """Demonstrate validation framework."""
        print("\n6. Validation Framework")
        print("-" * 40)
        
        # Create test modules
        def brain_validator(data):
            return isinstance(data, dict) and "neurons" in data
        
        def world_validator(data):
            return isinstance(data, dict) and "world_size" in data
        
        def ml_validator(data):
            return isinstance(data, dict) and "model_type" in data
        
        validator = ValidationFramework(strict_mode=True)
        validator.add_module_validator("brain", brain_validator, NLMBrainError)
        validator.add_module_validator("world", world_validator, NLMWorldError)
        validator.add_module_validator("ml", ml_validator, NLMBrainError)
        
        # Validate modules
        print("  Module validation:")
        test_cases = [
            ("brain", {"neurons": [1, 2, 3], "config": "test"}),
            ("world", {"world_size": (100, 100), "elevation": "flat"}),
            ("ml", {"model_type": "random_forest", "features": 10}),
            ("unknown", {"data": "test"}),  # Should fail in strict mode
        ]
        
        for module_name, data in test_cases:
            try:
                result = validator.validate_module(module_name, data)
                print(f"    ✓ validate_module ({module_name}): {result}")
            except NLMException as e:
                print(f"    ✗ validate_module ({module_name}): {e}")
        
        # Get validation report
        print("  Validation report:")
        report = validator.get_validation_report()
        for key, value in report.items():
            print(f"    {key}: {value}")
    
    def demonstrate_type_safety_checker(self):
        """Demonstrate type safety checker."""
        print("\n7. Type Safety Checker")
        print("-" * 40)
        
        checker = TypeSafetyChecker()
        
        # Function type hints check
        print("  Function type hints:")
        def sample_function(x: int, y: str) -> Dict[str, Any]:
            return {"x": x, "y": y}
        
        result = checker.check_type_hints(sample_function)
        for key, value in result.items():
            print(f"    {key}: {value}")
        
        # Instance type check
        print("  Instance type check:")
        try:
            result = checker.check_instance_type(42, int, "test_context")
            print(f"    ✓ check_instance_type: {result}")
        except NLMException as e:
            print(f"    ✗ check_instance_type: {e}")
        
        # Generic type check
        print("  Generic type check:")
        result = checker.check_generic_type([1, 2, 3], list[int])
        print(f"    ✓ check_generic_type: {result}")
    
    def demonstrate_error_handling_strategies(self):
        """Demonstrate error handling strategies."""
        print("\n8. Error Handling Strategies")
        print("-" * 40)
        
        error_handler = ErrorHandler("INFO", enable_recovery=True)
        
        # Add recovery handler
        def handle_nlm_exception(error: NLMException, context: Dict[str, Any]) -> Any:
            return f"Recovered from {type(error).__name__}: {error}"
        
        error_handler.add_recovery_handler(NLMException, handle_nlm_exception)
        
        # Handle errors
        print("  Error handling:")
        test_errors = [
            NLMConfigError("Config error", error_code="TEST_001"),
            NLMValidationError("Validation error", error_code="TEST_002"),
            NLMFileError("File error", error_code="TEST_003"),
        ]
        
        for error in test_errors:
            try:
                result = error_handler.handle_error(error, {"test": True})
                print(f"    ✓ handle_error: {result}")
            except Exception as e:
                print(f"    ✗ handle_error: {e}")
        
        # Get error summary
        print("  Error summary:")
        summary = error_handler.get_error_summary()
        for key, value in summary.items():
            print(f"    {key}: {value}")
    
    def demonstrate_decorators(self):
        """Demonstrate validation decorators."""
        print("\n9. Validation Decorators")
        print("-" * 40)
        
        # Type validation decorator
        @validate_types({"x": int, "y": str})
        def typed_function(x, y):
            return {"x": x, "y": y}
        
        print("  Type validation decorator:")
        try:
            result = typed_function(42, "test")
            print(f"    ✓ typed_function: {result}")
        except NLMException as e:
            print(f"    ✗ typed_function: {e}")
        
        try:
            result = typed_function("not_int", "test")
            print(f"    ✗ typed_function (should have failed): {result}")
        except NLMException as e:
            print(f"    ✓ typed_function (caught error): {e}")
        
        # Range validation decorator
        @validate_range(min_val=0, max_val=100, param_name="value")
        def range_function(value):
            return value * 2
        
        print("  Range validation decorator:")
        try:
            result = range_function(50)
            print(f"    ✓ range_function: {result}")
        except NLMException as e:
            print(f"    ✗ range_function: {e}")
        
        try:
            result = range_function(150)
            print(f"    ✗ range_function (should have failed): {result}")
        except NLMException as e:
            print(f"    ✓ range_function (caught error): {e}")
    
    def demonstrate_backward_compatibility(self):
        """Demonstrate backward compatibility functions."""
        print("\n10. Backward Compatibility")
        print("-" * 40)
        
        # Legacy functions for backward compatibility
        print("  Legacy functions:")
        
        try:
            result = validate_parameter(42, int, "legacy_param")
            print(f"    ✓ validate_parameter: {result}")
        except Exception as e:
            print(f"    ✗ validate_parameter: {e}")
        
        try:
            result = validate_function_call(len, "test_string")
            print(f"    ✓ validate_function_call: {result}")
        except Exception as e:
            print(f"    ✗ validate_function_call: {e}")
    
    def run_all_demonstrations(self):
        """Run all demonstrations."""
        print("Comprehensive Error Handling and Type Safety Integration")
        print("=" * 60)
        
        self.demonstrate_exception_hierarchy()
        self.demonstrate_type_safety()
        self.demonstrate_value_validation()
        self.demonstrate_parameter_validation()
        self.demonstrate_defensive_programming()
        self.demonstrate_validation_framework()
        self.demonstrate_type_safety_checker()
        self.demonstrate_error_handling_strategies()
        self.demonstrate_decorators()
        self.demonstrate_backward_compatibility()
        
        print("\n" + "=" * 60)
        print("All demonstrations completed successfully!")

# Standalone functions for backward compatibility
def validate_parameter(value: Any, expected_type: Type[Any],
                      param_name: str) -> Any:
    """Legacy function for backward compatibility."""
    validator = get_validator()
    return validator.validate_parameter(value, expected_type, param_name)

def validate_function_call(func: Callable, *args, **kwargs) -> Any:
    """Legacy function for backward compatibility."""
    validator = get_validator()
    return validator.validate_function_call(func, *args, **kwargs)

def get_validator() -> NLMValidator:
    """Get global validator instance."""
    global _validator_instance
    if _validator_instance is None:
        _validator_instance = NLMValidator()
    return _validator_instance

def set_validator(validator: NLMValidator) -> None:
    """Set global validator instance."""
    global _validator_instance
    _validator_instance = validator

# Global validator instance
_validator_instance: Optional[NLMValidator] = None

# Convenience functions
if __name__ == "__main__":
    example = ErrorHandlingExample()
    example.run_all_demonstrations()