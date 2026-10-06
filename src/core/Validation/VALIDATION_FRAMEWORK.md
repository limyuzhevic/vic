# NLM Configuration Validation Framework

This document describes the comprehensive configuration validation framework implemented for the NLM (Neural Learning Machine) system.

## Overview

The NLM Configuration Validation Framework provides robust validation capabilities for configuration files, ensuring configuration integrity and providing helpful error messages to users. The framework supports multiple validation types including:

- **Type validation** (int, float, string, bool, arrays)
- **Range validation** (min/max values)
- **Dependency validation** (key dependencies)
- **Consistency validation** (conflicting values)
- **Schema validation** (JSON schema-like definitions)
- **Nested schema support** for complex configurations

## Architecture

The validation framework is organized into several key components:

### 1. Core Validation Classes

#### ConfigValidator (`src/core/Validation/ConfigValidator.hpp/.cpp`)

**Purpose**: Main validator interface for comprehensive configuration validation

**Features**:
- Validates all configuration types in a single operation
- Supports type, range, dependency, and consistency validation
- Provides detailed error aggregation and reporting
- Integrates with the existing Config class

**Key Methods**:
- `validateAll()`: Runs all validation checks
- `validateTypes()`: Validates configuration types
- `validateRanges()`: Validates range constraints
- `validateDependencies()`: Validates key dependencies
- `validateConsistency()`: Validates consistency rules

#### SchemaValidator (`src/core/Validation/SchemaValidator.hpp/.cpp`)

**Purpose**: JSON schema-like validator for configuration schemas

**Features**:
- Supports JSON Schema-like definitions
- Validates required fields, types, and constraints
- Provides detailed schema validation error messages
- Supports nested schemas for complex configurations

**Key Features**:
- Type validation with support for primitive types
- Range validation (minimum, maximum values)
- Enum validation (allowed values)
- Pattern validation (regular expressions)
- Format validation (email, URI, IPv4 formats)

#### ConstraintValidator (`src/core/Validation/ConstraintValidator.hpp/.cpp`)

**Purpose**: Validator for type and range constraints

**Features**:
- Type validation for all supported ConfigValue types
- Range validation with min/max constraints
- Format validation for strings
- Enum validation for value enumeration
- Custom validation support

#### DependencyValidator (`src/core/Validation/DependencyValidator.hpp/.cpp`)

**Purpose**: Validator for key dependencies and relationships

**Features**:
- Required key dependency validation
- Conditional dependencies based on key values
- Mutual exclusivity checking
- Conditional requirement validation

#### ConsistencyValidator (`src/core/Validation/ConsistencyValidator.cpp`)

**Purpose**: Validator for consistency and conflict checking

**Features**:
- Conflicting value combination detection
- Logical consistency checking
- Business rule validation
- Value relationship validation

### 2. Exception Handling

#### ValidationException (`src/core/Validation/ValidationException.hpp`)

**Purpose**: Exception class for validation failures

**Features**:
- Categorized errors (Type, Range, Dependency, Consistency, Schema, Custom)
- Detailed error context (key, context information)
- Standardized error reporting format
- Integration with existing error handling system

#### ValidationResult

**Purpose**: Structure holding validation operation results

**Features**:
- Boolean validity flag
- Aggregated errors, warnings, and info messages
- Error categorization and count tracking
- Comprehensive validation reporting

## Integration Points

### 1. Config Class (`src/core/Config/Config.hpp/.cpp`)

**Enhanced Methods**:
- `validate(const nlohmann::json& schema)`: Validates against JSON schema
- `validate()`: Validates using built-in validator
- `getValidationResult()`: Returns detailed validation result
- `validateBeforeSave()`: Validates before saving to file
- `isValidKey()`: Checks if key is valid
- `getValidKeys()`: Returns list of valid keys

**Integration Features**:
- Automatic validation in `loadFromFile()` and `saveToFile()`
- Schema-based validation support
- Comprehensive error reporting
- Validation-as-you-go feedback

### 2. Python Bindings (`python/bindings.cpp`)

**Enhanced Python Interface**:
- `config.validate(schema)`: Python bindings for schema validation
- `config.validate()`: Python bindings for built-in validation
- `config.get_validation_result()`: Returns validation result as JSON
- `config.is_valid(schema)`: Python bindings for validation checking
- `config.validate_before_save()`: Python bindings for pre-save validation

**Python Features**:
- Pythonic validation method names
- JSON-based schema validation
- Comprehensive error reporting in Python
- Integration with existing Python helpers

## Schema Format

The validation framework supports a JSON schema-like format with the following structure:

```json
{
    "properties": {
        "key1": {
            "type": "int",
            "min": 0,
            "max": 100,
            "required": true,
            "enum": ["value1", "value2"],
            "pattern": "^[a-zA-Z0-9_]+$",
            "format": "email"
        },
        "key2": {
            "type": "object",
            "properties": {
                "nested_key": {
                    "type": "float",
                    "min": 0.0,
                    "max": 1.0
                }
            }
        }
    },
    "required": ["key1", "key2.nested_key"]
}
```

## Error Categories

The validation framework categorizes errors for better error handling:

1. **Type Error**: Invalid value types (e.g., string instead of integer)
2. **Range Error**: Values outside allowed ranges (e.g., port < 0 or > 65535)
3. **Dependency Error**: Missing required dependencies
4. **Consistency Error**: Conflicting or inconsistent values
5. **Schema Error**: Invalid schema definitions
6. **Custom Error**: User-defined validation failures

## Validation Process

### Basic Validation Steps

1. **Type Validation**: Ensure all values match expected types
2. **Range Validation**: Check values against min/max constraints
3. **Dependency Validation**: Verify required keys are present
4. **Consistency Validation**: Check for conflicting values
5. **Schema Validation**: Validate against schema if provided

### Detailed Validation Workflow

1. **Configuration Load**: Validate configuration when loaded from file
2. **Schema Validation**: Validate against schema if schema is provided
3. **Built-in Validation**: Validate against built-in rules and constraints
4. **Error Aggregation**: Collect all validation errors and warnings
5. **Result Reporting**: Return comprehensive validation result

## Usage Examples

### C++ Usage

```cpp
#include "nlm/Config.hpp"
#include "nlohmann/json.hpp"

int main() {
    nlm::Config config;
    
    // Load configuration
    config.loadFromFile("config.json");
    
    // Define schema
    nlohmann::json schema = {
        {"properties", {
            {"neuron_count", {{"type", "int"}, {"min", 10}, {"max", 10000}}},
            {"connection_probability", {{"type", "float"}, {"min", 0.0}, {"max", 1.0}}}
        }},
        {"required", {"neuron_count", "connection_probability"}}
    };
    
    // Validate against schema
    try {
        if (config.validate(schema)) {
            std::cout << "Configuration is valid!" << std::endl;
        } else {
            auto result = config.getValidationResult();
            std::cout << "Validation errors:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  - " << error << std::endl;
            }
        }
    } catch (const nlm::ValidationException& e) {
        std::cout << "Schema validation error: " << e.what() << std::endl;
    }
    
    // Validate before saving
    try {
        config.validateBeforeSave();
        config.saveToFile("config_validated.json");
    } catch (const nlm::ValidationException& e) {
        std::cout << "Pre-save validation failed: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### Python Usage

```python
from pynlm import Config
import json

def example_validation():
    # Create configuration
    config = Config()
    config["neuron_count"] = 1000
    config["connection_probability"] = 0.5
    
    # Define schema
    schema = {
        "neuron_count": {"type": "int", "min": 10, "max": 10000},
        "connection_probability": {"type": "float", "min": 0.0, "max": 1.0},
        "learning_rate": {"type": "float", "required": True, "min": 0.0, "max": 1.0}
    }
    
    # Validate against schema
    try:
        if config.validate(schema):
            print("✓ Configuration is valid!")
        else:
            result = config.get_validation_result()
            print("✗ Validation errors:")
            for error in result["errors"]:
                print(f"  - {error}")
    except Exception as e:
        print(f"✗ Schema validation error: {e}")
    
    # Validate using built-in validator
    if config.validate():
        print("✓ Configuration passes built-in validation!")
    else:
        result = config.get_validation_result()
        print("✗ Built-in validation errors:")
        for error in result["errors"]:
            print(f"  - {error}")
    
    # Validate before saving
    try:
        config.validate_before_save()
        config.save_to_file("validated_config.json")
        print("✓ Configuration validated and saved successfully!")
    except Exception as e:
        print(f"✗ Pre-save validation failed: {e}")

if __name__ == "__main__":
    example_validation()
```

## Files Added/Modified

### New Files Created:

1. `src/core/Validation/ConfigValidator.hpp`
2. `src/core/Validation/ConfigValidator.cpp`
3. `src/core/Validation/SchemaValidator.hpp`
4. `src/core/Validation/SchemaValidator.cpp`
5. `src/core/Validation/ConstraintValidator.hpp`
6. `src/core/Validation/ConstraintValidator.cpp`
7. `src/core/Validation/DependencyValidator.hpp`
8. `src/core/Validation/DependencyValidator.cpp`
9. `src/core/Validation/ConsistencyValidator.cpp`
10. `src/core/Validation/ValidationException.hpp`

### Modified Files:

1. `src/core/Config/Config.hpp` - Added validation methods
2. `src/core/Config/Config.cpp` - Implemented validation integration
3. `python/bindings.cpp` - Added Python bindings for validation

## Testing

### C++ Tests

The existing test infrastructure includes:
- `tests/test_config.cpp` - Basic Config functionality tests
- New validation tests will be added to this file

### Python Tests

The existing test infrastructure includes:
- `python/tests/test_config.py` - Python Config tests
- Enhanced validation tests in this file

## Performance Considerations

### Validation Performance

- **Type validation**: O(1) per key
- **Range validation**: O(1) per numeric key
- **Dependency validation**: O(n) where n is number of dependencies
- **Consistency validation**: O(n²) in worst case
- **Schema validation**: O(n) where n is number of schema properties

### Optimization Strategies

1. **Caching**: Cache validation results for frequently validated configurations
2. **Lazy validation**: Validate only when needed (e.g., before save)
3. **Parallel validation**: Validate independent keys in parallel
4. **Early termination**: Stop validation on first critical error (strict mode)

## Configuration Examples

### Example Configuration Schema

```yaml
# config_schema.yaml
"""
NLM Configuration Schema

This schema defines the structure and validation rules for NLM configurations.
"""

neuron_count:
  type: int
  min: 10
  max: 100000
  required: true
  description: Number of neurons in the network

region_count:
  type: int
  min: 1
  max: 1000
  required: true
  description: Number of neural regions

simulation_timestep:
  type: float
  min: 0.0001
  max: 1.0
  required: true
  description: Time step for simulation

random_seed:
  type: int
  min: 0
  max: 2147483647
  required: false
  description: Random seed for reproducibility

learning_rate:
  type: float
  min: 0.0
  max: 1.0
  required: false
  description: Learning rate for plasticity rules

connection_probability:
  type: float
  min: 0.0
  max: 1.0
  required: false
  description: Probability of synaptic connections

enable_structural_plasticity:
  type: bool
  required: false
  description: Enable structural plasticity

enable_development:
  type: bool
  required: false
  description: Enable neural development

stdp_ltp_weight:
  type: float
  min: 0.0
  max: 1.0
  required: false
  description: STDP LTP weight parameter

stdp_ltd_weight:
  type: float
  min: 0.0
  max: 1.0
  required: false
  description: STDP LTD weight parameter

synaptogenesis_rate:
  type: float
  min: 0.0
  max: 1.0
  required: false
  description: Rate of new synapse formation

pruning_rate:
  type: float
  min: 0.0
  max: 1.0
  required: false
  description: Rate of synaptic pruning
```

## Migration Guide

### Upgrading from Previous Version

1. **Existing Configurations**: All existing configurations will continue to work
2. **New Validation Rules**: New validation rules are applied by default
3. **Schema Validation**: Add schema files for more detailed validation
4. **Error Messages**: Updated error messages are more descriptive
5. **Performance**: Validation adds minimal overhead to configuration loading

### Migration Steps

1. Run validation on existing configurations
2. Update schema files as needed
3. Add validation to build process
4. Update documentation and examples
5. Test with sample configurations

## Conclusion

The NLM Configuration Validation Framework provides a comprehensive solution for configuration validation with the following key benefits:

- **Enhanced Reliability**: Validates configuration integrity before use
- **Better Error Messages**: Detailed, categorized error messages
- **Schema Support**: JSON schema-like validation for complex configurations
- **Type Safety**: Comprehensive type checking
- **Dependency Management**: Validates key relationships
- **Consistency Checking**: Detects conflicting values
- **Performance**: Optimized for both speed and accuracy
- **Python Integration**: Full Python bindings for validation

The framework is designed to be extensible, allowing users to add custom validation rules and constraints as needed. It maintains backward compatibility while providing powerful new validation capabilities.
