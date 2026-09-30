# Error Handling Examples

These examples demonstrate enhanced error handling and validation in NLM Python bindings.

## Overview

The error handling examples cover:

1. **Enhanced Exception Types** - Custom exception classes with detailed recovery information
2. **Type Validation** - Strict type checking with helpful error messages
3. **Parameter Validation** - Range checking, null pointer detection, bounds checking
4. **State Validation** - State checking and error recovery
5. **Error Recovery Patterns** - Graceful error handling and recovery
6. **Error Logging** - Structured error logging and reporting

## Key Features Demonstrated

- **Specific Exception Types**: ValidationError, ConfigError, ResourceError, StateError
- **Type Safety**: Strict type checking with detailed error messages
- **Parameter Validation**: Range checking, null pointer detection, bounds checking
- **Recovery Suggestions**: Each error includes recovery guidance
- **Structured Error Handling**: Try-catch with specific exception handling
- **Error Logging**: Comprehensive error reporting with context

## How to Use

```bash
# Navigate to this directory
cd error_handling

# Run error handling examples
python example1_enhanced_exceptions.py
python example2_type_validation.py
python example3_parameter_validation.py
python example4_state_validation.py
python example5_error_recovery.py
python example6_error_logging.py
```

## Dependencies

```python
import pynlm
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    ValidationError, ConfigError, ResourceError, StateError
)
```

All examples work with:
- The original `pynlm` module
- The enhanced `enhanced_bindings` module
- Both modules simultaneously for comparison

## Error Handling Categories

### Enhanced Exception Types
- `example1_enhanced_exceptions.py` - Custom exception classes
- Features ValidationError, ConfigError, ResourceError, StateError
- Demonstrates exception inheritance and specific error handling

### Type Validation
- `example2_type_validation.py` - Strict type checking
- Demonstrates type validation with recovery suggestions
- Shows getBoolRequired, getIntRequired, etc. methods

### Parameter Validation
- `example3_parameter_validation.py` - Parameter validation
- Range checking, null pointer detection, bounds checking
- Demonstrates validateRange, validateParameter, validatePointer functions

### State Validation
- `example4_state_validation.py` - State checking
- Initial vs. initialized state validation
- Operation state checking and error recovery

### Error Recovery
- `example5_error_recovery.py` - Graceful error handling
- Try-catch blocks with specific exception handling
- Recovery patterns and error continuation

### Error Logging
- `example6_error_logging.py` - Structured error logging
- Comprehensive error reporting with context
- Error classification and recovery guidance
