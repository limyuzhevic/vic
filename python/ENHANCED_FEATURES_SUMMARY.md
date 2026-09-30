# NLM Python Bindings - Comprehensive Validation and Error Handling

This document provides detailed information about the **36 new validation and error handling functions** added to the NLM Python bindings.

## Overview

The NLM Python bindings now include comprehensive validation and error handling capabilities to ensure robust neural simulation execution. All validation functions support different validation levels (Basic, Thorough, Strict) and include comprehensive error messages, logging options, and exception handling.

## Available Validation Categories

### 1. Enhanced Parameter Validation (7 functions)

| Function | Description | Level Support |
|----------|-------------|---------------|
| `validate_parameters()` | Comprehensive parameter validation for all functions | Basic, Thorough, Strict |
| `validate_brain_config()` | Validate brain configuration structure | Basic, Thorough, Strict |
| `validate_world_config()` | Validate world configuration parameters | Basic, Thorough, Strict |
| `validate_simulation_step()` | Validate simulation step parameters | Basic, Thorough, Strict |
| `validate_neuron_ids()` | Validate neuron ID ranges and formats | Basic, Thorough, Strict |
| `validate_synapse_ids()` | Validate synapse ID ranges and formats | Basic, Thorough, Strict |
| `validate_region_ids()` | Validate region ID ranges and formats | Basic, Thorough, Strict |

### 2. Error Handling and Recovery (6 functions)

| Function | Description | Level Support |
|----------|-------------|---------------|
| `safe_step()` | Safe simulation step with error recovery | Basic, Thorough, Strict |
| `safe_brain_operation()` | Safe brain operations with validation | Basic, Thorough, Strict |
| `validate_and_safe_step()` | Combined validation and safe operation | Basic, Thorough, Strict |
| `validate_and_safe_brain_operation()` | Combined validation and safe brain operation | Basic, Thorough, Strict |
| `try_brain_operation()` | Try-catch wrapper for brain operations | Basic, Thorough, Strict |
| `validate_and_safe_agent_operation()` | Safe agent operations with validation | Basic, Thorough, Strict |

### 3. Robust Configuration Validation (5 functions)

| Function | Description | Level Support |
|----------|-------------|---------------|
| `validate_config_completeness()` | Check if all required config values are present | Basic, Thorough, Strict |
| `validate_config_consistency()` | Check for consistency between config values | Basic, Thorough, Strict |
| `validate_config_values()` | Validate individual config values against constraints | Basic, Thorough, Strict |
| `fix_config_issues()` | Attempt to automatically fix common config issues | Basic, Thorough |
| `validate_world_completeness()` | Check world configuration completeness | Basic, Thorough, Strict |

### 4. Data Validation and Integrity (5 functions)

| Function | Description | Level Support |
|----------|-------------|---------------|
| `validate_sensory_data()` | Validate sensory input data | Basic, Thorough, Strict |
| `validate_motor_command()` | Validate motor command data | Basic, Thorough, Strict |
| `validate_action()` | Validate action data structure | Basic, Thorough, Strict |
| `validate_object_data()` | Validate world object data | Basic, Thorough, Strict |
| `validate_memory_data()` | Validate memory data integrity | Basic, Thorough, Strict |

### 5. State Validation (4 functions)

| Function | Description | Level Support |
|----------|-------------|---------------|
| `validate_brain_state()` | Validate brain internal state | Basic, Thorough, Strict |
| `validate_agent_state()` | Validate agent state consistency | Basic, Thorough, Strict |
| `validate_world_state()` | Validate world state | Basic, Thorough, Strict |
| `validate_development_state()` | Validate developmental stage consistency | Basic, Thorough, Strict |

### 6. Performance Validation (4 functions)

| Function | Description | Level Support |
|----------|-------------|---------------|
| `validate_performance_parameters()` | Validate performance-related parameters | Basic, Thorough, Strict |
| `validate_simulation_parameters()` | Validate simulation parameters | Basic, Thorough, Strict |
| `validate_plasticity_parameters()` | Validate plasticity rule parameters | Basic, Thorough, Strict |
| `validate_neuromodulation_parameters()` | Validate neuromodulation parameters | Basic, Thorough, Strict |

### 7. Safety and Security (5 functions)

| Function | Description | Level Support |
|----------|-------------|---------------|
| `check_resource_usage()` | Monitor resource usage | Basic, Thorough |
| `validate_memory_bounds()` | Check memory usage against limits | Basic, Thorough, Strict |
| `check_safety_constraints()` | Validate safety constraints | Basic, Thorough, Strict |
| `validate_neural_connectivity()` | Validate neural network connectivity | Basic, Thorough, Strict |
| `validate_plasticity_constraints()` | Validate plasticity rule constraints | Basic, Thorough, Strict |

## ValidationResult Class

All validation functions return a `ValidationResult` object with the following properties:

- `valid`: Boolean indicating if validation passed
- `message`: Human-readable message describing the validation result
- `warnings`: Array of warning messages
- `errors`: Array of error messages
- `isValid()`: Helper method to check if validation passed
- `hasErrors()`: Helper method to check if there are errors
- `hasWarnings()`: Helper method to check if there are warnings
- `getFullMessage()`: Returns combined message with all warnings and errors

## ValidationLevel Enum

```python
ValidationLevel {
    Basic,      # Only basic checks
    Thorough,   # Additional comprehensive checks
    Strict      # Strict validation with all checks
}
```

## Exception Classes

### ValidationException
Raised when validation fails and `throw_on_error=True` is set in the validation configuration.

### ValidationWarning
Raised when validation produces warnings and `throw_on_warning=True` is set in the validation configuration.

## Error Handling Configuration

You can configure global error handling settings using the `error_handling.set_validation_config()` function:

```python
from pynlm.validation import ValidationLevel

# Configure validation with strict settings
error_handling.set_validation_config(
    default_level=ValidationLevel.Strict,
    enable_logging=True,
    throw_on_error=True,
    throw_on_warning=False
)
```

## Usage Examples

### Example 1: Basic Parameter Validation

```python
import pynlm as nlm

# Create a brain and world
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()

# Validate parameters
parameters = [0.1, 0.5, 0.9, 0.3]
result = nlm.validation.validate_parameters(parameters, "neural network initialization")

if not result.isValid():
    print("Validation failed:", result.getFullMessage())
else:
    print("Validation passed:", result.message)
if result.hasWarnings():
    print("Warnings:", result.warnings)
```

### Example 2: Configuration Validation

```python
import pynlm as nlm

config = nlm.createDefaultConfig()

# Validate brain configuration
brain_result = nlm.validation.validate_brain_config(config, nlm.ValidationLevel.Thorough)
print("Brain config valid:", brain_result.isValid())

# Validate world configuration
world_result = nlm.validation.validate_world_config(config, nlm.ValidationLevel.Strict)
print("World config valid:", world_result.isValid())
```

### Example 3: Safe Brain Operation

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()

# Safe step operation
def neural_step():
    brain.step(1)

success = nlm.validation.safe_brain_operation(brain, neural_step, nlm.ValidationLevel.Thorough)
if success:
    print("Brain operation completed successfully")
else:
    print("Brain operation failed with validation errors")
```

### Example 4: Error Recovery with Validation

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()

# Validate and safe step in one operation
result = nlm.validation.validate_and_safe_step(brain, 1, 0.1, nlm.ValidationLevel.Thorough)

if result.isValid():
    print("Step completed successfully")
else:
    print("Step validation failed:", result.getFullMessage())
    # Try to fix configuration issues
    fixed = nlm.validation.fix_config_issues(config)
    if fixed:
        print("Configuration issues fixed")
```

### Example 5: Agent Operation Validation

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
agent.initialize(world)

# Process sensory input with validation
def process_sensory():
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)

success = nlm.validation.validate_and_safe_agent_operation(agent, process_sensory)
if success:
    print("Agent sensory processing completed successfully")
```

## Error Logging

To enable error logging:

```python
# Enable logging to file
nlm.error_handling.set_validation_config(
    default_level=nlm.ValidationLevel.Thorough,
    enable_logging=True,
    log_file="/path/to/validation.log"
)

# Manually log validation messages
nlm.validation.log_validation_message("Custom validation message", is_error=False)
```

The log file will contain timestamps and error/warning levels for all validation messages.

## Performance Considerations

1. **Basic Level**: Fastest validation, minimal overhead
2. **Thorough Level**: Moderate validation, good balance of safety and performance
3. **Strict Level**: Most comprehensive validation, highest overhead

For production environments, use **Thorough** level unless you need maximum safety.

## Integration with Existing Code

The validation functions are fully integrated into the existing NLM Python bindings. You can use them alongside existing functions without any changes to your existing code.

For example, existing step operations can be replaced with validated versions:

```python
# Old way
brain.step(step_number)

# New way with validation
result = nlm.validation.validate_and_safe_step(brain, step_number)
if result.isValid():
    # Step succeeded
else:
    # Handle validation errors
    print("Step validation failed:", result.getFullMessage())
```

## Testing and Validation

You can use the validation functions to test your neural simulations:

```python
import pynlm as nlm

# Create a complete test scenario
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

# Validate entire system
brain_result = nlm.validation.validate_brain_state(brain, nlm.ValidationLevel.Thorough)
world_result = nlm.validation.validate_world_state(world, nlm.ValidationLevel.Thorough)
agent_result = nlm.validation.validate_agent_state(agent, nlm.ValidationLevel.Thorough)

if brain_result.isValid() and world_result.isValid() and agent_result.isValid():
    print("All system components validated successfully")
else:
    print("System validation failed - check individual results")
```

## Migration Guide

### From: Basic brain operations

```python
# Old: No validation
brain.step(step)
brain.receiveSensoryInput(input)
brain.produceAction()

# New: With validation
result = nlm.validation.validate_and_safe_step(brain, step)
if result.isValid():
    # Step succeeded
else:
    # Handle errors
    print(result.getFullMessage())
```

### From: Manual error checking

```python
# Old: Manual error checking
if config.has("neuronCount") and config.get("neuronCount") > 1000:
    print("Warning: High neuron count")

# New: Automatic validation with logging
result = nlm.validation.validate_config_values(config)
for warning in result.warnings:
    print("Warning:", warning)
for error in result.errors:
    print("Error:", error)
```

## Troubleshooting

### Validation Fails but Code Should Work

1. Check validation level - try `Basic` instead of `Strict`
2. Disable logging if it interferes with performance
3. Check validation configuration settings

### Validation Passes but Errors Occur

1. Use `safe_operation` or `validate_and_safe_operation` functions
2. Ensure `throw_on_error=False` in validation config
3. Implement proper error handling in your code

### Validation Configuration Issues

1. Ensure you're using the correct Python bindings version
2. Check that the validation module is properly imported
3. Verify that your NLM C++ library includes the validation support

## Dependencies

The validation and error handling functions require:
- The NLM C++ library with validation support
- Python 3.8+
- pybind11 2.11+

## Further Reading

For more detailed information about specific validation functions, refer to their individual documentation in the `pynlm.validation` module.

For information about the NLM framework and neural simulation, see the main NLM documentation.

## Version Information

This documentation covers version 0.1.0 of the enhanced NLM Python bindings with comprehensive validation and error handling support.
