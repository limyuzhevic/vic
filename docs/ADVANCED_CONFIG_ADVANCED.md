# ConfigAdvanced Documentation

## Overview

This document provides comprehensive documentation for the ConfigAdvanced class, which offers advanced Python APIs for the Config class, including schema validation, analysis, and management APIs.

## Introduction

The ConfigAdvanced class provides enhanced access to Config functionality through Python-friendly APIs, including comprehensive schema validation, parameter analysis, and configuration management capabilities.

## Key Features

### Schema Validation
- **Schema Definition**: Get and validate configuration schemas
- **Validation Rules**: Define and apply validation rules for parameters
- **Schema Examples**: Access examples for common configurations
- **Schema Documentation**: Get detailed schema documentation

### Parameter Management
- **Parameter Loading**: Load configurations from files
- **Parameter Saving**: Save configurations to files
- **Parameter Access**: Get and set configuration values
- **Parameter Clearing**: Clear all configuration entries

### Configuration Analysis
- **Configuration Statistics**: Get comprehensive configuration statistics
- **Parameter Optimization**: Get optimization hints for parameters
- **Parameter Dependencies**: Analyze parameter dependencies
- **Parameter Conflicts**: Identify parameter conflicts
- **Configuration Reports**: Generate comprehensive configuration reports

### Validation Management
- **Configuration Validation**: Validate configuration parameters
- **Error Management**: Get and manage validation errors
- **Warning Management**: Get and manage validation warnings
- **Automatic Fixing**: Automatically fix configuration issues

## API Reference

### Constructor
```python
def __init__(self, config):
    """Initialize ConfigAdvanced with a Config object
    
    Args:
        config: The Config object to wrap
    """
```

### Basic Configuration Operations

#### Core Operations

**`loadFromFile(filepath)`**
```python
def loadFromFile(self, filepath):
    """Load configuration from a JSON file
    
    Args:
        filepath (str): Path to the configuration file
    
    Returns:
        bool: True if loading successful, False otherwise
    """
```

**`saveToFile(filepath)`**
```python
def saveToFile(self, filepath):
    """Save configuration to a JSON file
    
    Args:
        filepath (str): Path to save the configuration file
    
    Returns:
        bool: True if saving successful, False otherwise
    """
```

**`has(key)`**
```python
def has(self, key):
    """Check if a configuration key exists
    
    Args:
        key (str): Configuration key to check
    
    Returns:
        bool: True if key exists, False otherwise
    """
```

**`getKeys()`**
```python
def getKeys(self):
    """Get all configuration keys
    
    Returns:
        list: List of all configuration keys
    """
```

**`get(key, default_value=None)`**
```python
def get(self, key, default_value=None):
    """Get configuration value with optional default
    
    Args:
        key (str): Configuration key
        default_value (any, optional): Default value if key doesn't exist
    
    Returns:
        any: Configuration value or default value
    """
```

**`set(key, value, source="Runtime")`**
```python
def set(self, key, value, source="Runtime"):
    """Set configuration value
    
    Args:
        key (str): Configuration key
        value (any): Configuration value
        source (str): Source of the configuration (Default, File, CommandLine, Runtime)
    """
```

**`clear()`**
```python
def clear(self):
    """Clear all configuration entries
    
    Returns:
        bool: True if clearing successful, False otherwise
    """
```

**`summary()`**
```python
def summary(self):
    """Get configuration summary
    
    Returns:
        str: Configuration summary string
    """
```

### Schema APIs

#### Schema Definition and Validation

**`getSchema()`**
```python
def getSchema(self):
    """Get configuration schema
    
    Returns:
        dict: Configuration schema containing parameters, validation rules, and defaults
    """
```

**`getSchemaDocumentation()`**
```python
def getSchemaDocumentation(self):
    """Get schema documentation
    
    Returns:
        dict: Schema documentation with title, description, version, author, date, and parameters
    """
```

**`getSchemaExamples()`**
```python
def getSchemaExamples(self):
    """Get schema examples
    
    Returns:
        dict: Schema examples containing minimal, standard, performance, and learning configurations
    """
```

**`getSchemaValidationRules()`**
```python
def getSchemaValidationRules(self):
    """Get schema validation rules
    
    Returns:
        dict: Schema validation rules for type validation, range validation, and dependency validation
    """
```

**`validateSchema(data)`**
```python
def validateSchema(self, data):
    """Validate data against schema
    
    Args:
        data (dict): Data to validate against schema
    
    Returns:
        dict: Validation result with valid, errors, and warnings
    """
```

### Validation APIs

#### Configuration Validation

**`validate()`**
```python
def validate(self):
    """Validate configuration
    
    Returns:
        bool: True if configuration is valid, False otherwise
    """
```

**`validateAll()`**
```python
def validateAll(self):
    """Validate all parameters
    
    Returns:
        bool: True if all parameters are valid, False otherwise
    """
```

**`getValidationErrors()`**
```python
def getValidationErrors(self):
    """Get validation errors
    
    Returns:
        list: List of validation errors
    """
```

**`getValidationWarnings()`**
```python
def getValidationWarnings(self):
    """Get validation warnings
    
    Returns:
        list: List of validation warnings
    """
```

#### Validation Rule Management

**`addValidation(key, validator)`**
```python
def addValidation(self, key, validator):
    """Add validation for a key
    
    Args:
        key (str): Configuration key
        validator (callable): Validator function
    """
```

**`removeValidation(key)`**
```python
def removeValidation(self, key):
    """Remove validation for a key
    
    Args:
        key (str): Configuration key
    """
```

**`fixConfiguration()`**
```python
def fixConfiguration(self):
    """Fix configuration automatically
    
    Returns:
        bool: True if configuration was fixed, False otherwise
    """
```

#### Parameter Range Validation

**`addRangeValidation(key, min, max, description="")`**
```python
def addRangeValidation(self, key, min, max, description=""):
    """Add range validation for a parameter
    
    Args:
        key (str): Configuration key
        min (float): Minimum value
        max (float): Maximum value
        description (str): Description of the validation
    """
```

**`addIntegerRangeValidation(key, min, max, description="")`**
```python
def addIntegerRangeValidation(self, key, min, max, description=""):
    """Add integer range validation for a parameter
    
    Args:
        key (str): Configuration key
        min (int): Minimum integer value
        max (int): Maximum integer value
        description (str): Description of the validation
    """
```

**`addPositiveValidation(key, description="")`**
```python
def addPositiveValidation(self, key, description=""):
    """Add positive validation for a parameter
    
    Args:
        key (str): Configuration key
        description (str): Description of the validation
    """
```

**`addNonNegativeValidation(key, description="")`**
```python
def addNonNegativeValidation(self, key, description=""):
    """Add non-negative validation for a parameter
    
    Args:
        key (str): Configuration key
        description (str): Description of the validation
    """
```

#### Neural Operation Validation

**`addNeuronCountValidation(max_neurons=1000000)`**
```python
def addNeuronCountValidation(self, max_neurons=1000000):
    """Add neuron count validation
    
    Args:
        max_neurons (int): Maximum number of neurons
    """
```

**`addConnectionProbabilityValidation(min=0.0, max=1.0)`**
```python
def addConnectionProbabilityValidation(self, min=0.0, max=1.0):
    """Add connection probability validation
    
    Args:
        min (float): Minimum connection probability
        max (float): Maximum connection probability
    """
```

#### Memory Validation

**`addMemoryCapacityValidation(max_working_memory=10000)`**
```python
def addMemoryCapacityValidation(self, max_working_memory=10000):
    """Add memory capacity validation
    
    Args:
        max_working_memory (int): Maximum working memory capacity
    """
```

**`addTimeStepValidation(min=0.0, max=1.0)`**
```python
def addTimeStepValidation(self, min=0.0, max=1.0):
    """Add time step validation
    
    Args:
        min (float): Minimum time step
        max (float): Maximum time step
    """
```

#### Integration Validation

**`addRegionValidation(max_regions=100)`**
```python
def addRegionValidation(self, max_regions=100):
    """Add region validation
    
    Args:
        max_regions (int): Maximum number of regions
    """
```

### Configuration Analysis APIs

#### Parameter Analysis

**`getConfigurationStats()`**
```python
def getConfigurationStats(self):
    """Get configuration statistics
    
    Returns:
        dict: Dictionary with configuration statistics:
            - total_parameters (int): Total number of parameters
            - parameter_types (dict): Count of parameters by type
            - value_ranges (dict): Value ranges for parameters
            - sources (dict): Count of parameters by source
    """
```

**`getParameterOptimizationHints()`**
```python
def getParameterOptimizationHints(self):
    """Get optimization hints for parameters
    
    Returns:
        list: List of optimization hints
    """
```

**`getParameterDependencies()`**
```python
def getParameterDependencies(self):
    """Get parameter dependencies
    
    Returns:
        dict: Dictionary with parameter dependencies
    """
```

**`getParameterConflicts()`**
```python
def getParameterConflicts(self):
    """Get parameter conflicts
    
    Returns:
        list: List of parameter conflicts
    """
```

**`getParameterRelationships()`**
```python
def getParameterRelationships(self):
    """Get parameter relationships
    
    Returns:
        dict: Dictionary with parameter relationships
    """
```

**`getConfigurationReport()`**
```python
def getConfigurationReport(self):
    """Get comprehensive configuration report
    
    Returns:
        dict: Dictionary with comprehensive configuration report
    """
```

### Python Integration APIs

#### Version and Information APIs

**`getPythonBindingsVersion()`**
```python
def getPythonBindingsVersion(self):
    """Get Python bindings version
    
    Returns:
        str: Python bindings version
    """
```

**`getPythonBindingsInfo()`**
```python
def getPythonBindingsInfo(self):
    """Get Python bindings information
    
    Returns:
        dict: Python bindings information including version, platform, python version, and features
    """
```

### Resource Monitoring APIs

#### Memory Usage APIs

**`getMemoryUsage()`**
```python
def getMemoryUsage(self):
    """Get memory usage
    
    Returns:
        int: Memory usage in bytes
    """
```

### Export APIs

#### Configuration Export

**`exportConfig(filepath)`**
```python
def exportConfig(self, filepath):
    """Export configuration to file
    
    Args:
        filepath (str): Path to export file
    
    Returns:
        str: Export result message
    """
```

**`importConfig(filepath)`**
```python
def importConfig(self, filepath):
    """Import configuration from file
    
    Args:
        filepath (str): Path to import file
    
    Returns:
        str: Import result message
    """
```

**`getConfigAsDict()`**
```python
def getConfigAsDict(self):
    """Get configuration as dictionary
    
    Returns:
        dict: Configuration as dictionary
    """
```

**`fromDict(config_dict)`**
```python
def fromDict(self, config_dict):
    """Create configuration from dictionary
    
    Args:
        config_dict (dict): Configuration dictionary
    
    Returns:
        ConfigAdvanced: Self for method chaining
    """
```

### Comparison APIs

#### Configuration Comparison

**`equals(other_config)`**
```python
def equals(self, other_config):
    """Check if configuration equals another
    
    Args:
        other_config (ConfigAdvanced): Other configuration to compare
    
    Returns:
        bool: True if configurations are equal, False otherwise
    """
```

**`copy()`**
```python
def copy(self):
    """Copy configuration
    
    Returns:
        ConfigAdvanced: Copy of the configuration
    """
```

## Usage Examples

### Basic Configuration Usage

```python
from nlm import ConfigAdvanced

# Create and initialize configuration
config = ConfigAdvanced(config_object)

# Load configuration from file
config.loadFromFile("config.json")

# Save configuration to file
config.saveToFile("config.json")

# Get configuration statistics
stats = config.getConfigurationStats()
print(f"Total Parameters: {stats['total_parameters']}")
print(f"Parameter Types: {stats['parameter_types']}")
```

### Schema Validation Usage

```python
from nlm import ConfigAdvanced

config = ConfigAdvanced(config_object)

# Get configuration schema
schema = config.getSchema()
print(f"Schema Version: {schema['version']}")
print(f"Schema Description: {schema['description']}")

# Validate data against schema
validation_data = {
    'brain.neuronCount': 10000,
    'simulation.timeStep': 0.01
}
validation_result = config.validateSchema(validation_data)
print(f"Validation Result:")
print(f"  Valid: {validation_result['valid']}")
print(f"  Errors: {validation_result['errors']}")
print(f"  Warnings: {validation_result['warnings']}")
```

### Parameter Validation Usage

```python
config = ConfigAdvanced(config_object)

# Add range validation
config.addRangeValidation('brain.neuronCount', 100, 100000, 'Neuron count must be within range')
config.addPositiveValidation('simulation.timeStep', 'Time step must be positive')

# Validate configuration
if config.validate():
    print("Configuration is valid.")
else:
    print("Configuration validation errors:")
    for error in config.getValidationErrors():
        print(f"  - {error}")
```

### Configuration Analysis Usage

```python
config = ConfigAdvanced(config_object)

# Get configuration statistics
stats = config.getConfigurationStats()
print(f"Configuration Statistics:")
print(f"  Total Parameters: {stats['total_parameters']}")
print(f"  Parameter Types: {stats['parameter_types']}")

# Get optimization hints
hints = config.getParameterOptimizationHints()
print(f"\nOptimization Hints:")
for hint in hints:
    print(f"  - {hint}")

# Get parameter dependencies
dependencies = config.getParameterDependencies()
print(f"\nParameter Dependencies:")
for param, deps in dependencies.items():
    print(f"  {param}: {deps}")

# Get parameter conflicts
conflicts = config.getParameterConflicts()
print(f"\nParameter Conflicts:")
for conflict in conflicts:
    print(f"  - {conflict}")

# Generate configuration report
report = config.getConfigurationReport()
print(f"\nConfiguration Report:")
print(f"  Summary: {report['summary']}")
print(f"  Validation Status: {report['validation_status']}")
print(f"  Recommendation: {report['recommendation']}")
```

### Configuration Export/Import Usage

```python
config = ConfigAdvanced(config_object)

# Export configuration to file
config.exportConfig("config.json")
print("Configuration exported to config.json")

# Import configuration from file
config.importConfig("config.json")
print("Configuration imported from config.json")

# Get configuration as dictionary
config_dict = config.getConfigAsDict()
print(f"Configuration as dictionary: {len(config_dict)} parameters")

# Create configuration from dictionary
new_config = ConfigAdvanced(config_object)
new_config.fromDict(config_dict)
print("Configuration created from dictionary")
```

### Configuration Comparison Usage

```python
config1 = ConfigAdvanced(config_object)
config2 = ConfigAdvanced(config_object)

# Add parameters to configurations
config1.set('brain.neuronCount', 10000)
config1.set('simulation.timeStep', 0.01)

config2.set('brain.neuronCount', 10000)
config2.set('simulation.timeStep', 0.01)

# Compare configurations
if config1.equals(config2):
    print("Configurations are equal")
else:
    print("Configurations are different")

# Copy configuration
config_copy = config1.copy()
print(f"Configuration copied with {len(config_copy.getKeys())} parameters")
```

## Performance Tips

### 1. Schema Validation Optimization

```python
config = ConfigAdvanced(config_object)

# Add validation rules for critical parameters
config.addRangeValidation('brain.neuronCount', 100, 100000, 'Neuron count must be within range')
config.addPositiveValidation('simulation.timeStep', 'Time step must be positive')

# Validate configuration once
if not config.validate():
    # Fix configuration automatically
    config.fixConfiguration()
    print("Configuration automatically fixed and validated.")
```

### 2. Parameter Analysis Optimization

```python
config = ConfigAdvanced(config_object)

# Get configuration statistics for optimization
stats = config.getConfigurationStats()
print(f"Total Parameters: {stats['total_parameters']}")

# Get optimization hints for performance improvement
hints = config.getParameterOptimizationHints()
for hint in hints[:3]:  # Top 3 optimization hints
    print(f"Optimization: {hint}")
```

### 3. Configuration Export/Import Optimization

```python
config = ConfigAdvanced(config_object)

# Export configuration in efficient format
config.exportConfig("config.json")
print("Configuration exported efficiently")

# Import configuration in efficient format
config.importConfig("config.json")
print("Configuration imported efficiently")
```

## Best Practices

### 1. Schema Validation Best Practices

```python
config = ConfigAdvanced(config_object)

# Always validate configuration before use
if not config.validate():
    print("Configuration validation failed!")
    print("Errors:", config.getValidationErrors())
    print("Warnings:", config.getValidationWarnings())
    
    # Try to fix configuration automatically
    if config.fixConfiguration():
        print("Configuration automatically fixed.")
        
        # Validate again
        if config.validate():
            print("Configuration successfully validated.")
        else:
            print("Configuration could not be fixed automatically.")
    else:
        print("Configuration could not be fixed automatically.")
```

### 2. Parameter Management Best Practices

```python
config = ConfigAdvanced(config_object)

# Always validate before using configuration
if config.validate():
    # Use configuration
    neuron_count = config.get('brain.neuronCount')
    print(f"Neuron count: {neuron_count}")
    
    # Set configuration
    config.set('simulation.timeStep', 0.01)
    
    # Get all configuration keys
    keys = config.getKeys()
    print(f"Configuration keys: {keys}")
else:
    print("Cannot use configuration until validation passes.")
```

### 3. Schema Management Best Practices

```python
config = ConfigAdvanced(config_object)

# Always validate data against schema
validation_data = {
    'brain.neuronCount': 10000,
    'simulation.timeStep': 0.01
}
validation_result = config.validateSchema(validation_data)

if validation_result['valid']:
    print("Data is valid according to schema.")
else:
    print("Data validation failed:")
    for error in validation_result['errors']:
        print(f"  - {error}")
    for warning in validation_result['warnings']:
        print(f"  Warning: {warning}")
```

### 4. Configuration Analysis Best Practices

```python
config = ConfigAdvanced(config_object)

# Analyze configuration for optimization
stats = config.getConfigurationStats()
print(f"Configuration Analysis:")
print(f"  Total Parameters: {stats['total_parameters']}")
print(f"  Parameter Types: {stats['parameter_types']}")

# Get parameter dependencies
parameters = config.getConfigurationStats()
print(f"Parameter Dependencies: {parameters.get('dependencies', 'N/A')}")

# Get parameter conflicts
conflicts = config.getParameterConflicts()
if conflicts:
    print(f"Parameter Conflicts: {len(conflicts)} conflicts found")
    for conflict in conflicts:
        print(f"  - {conflict}")
```

## Troubleshooting

### Common Issues and Solutions

1. **Configuration Validation Errors**
   - **Issue**: Configuration validation fails
   - **Solution**: Use `config.fixConfiguration()` to automatically fix common issues

2. **Schema Validation Errors**
   - **Issue**: Data validation against schema fails
   - **Solution**: Use `config.getSchemaValidationRules()` to check validation rules

3. **Parameter Validation Errors**
   - **Issue**: Parameter validation fails
   - **Solution**: Use `config.addRangeValidation()` and related methods to add validation rules

4. **Configuration Export/Import Issues**
   - **Issue**: Configuration export/import fails
   - **Solution**: Use `config.getConfigAsDict()` and `config.fromDict()` for efficient export/import

5. **Performance Issues**
   - **Issue**: Configuration analysis is slow
   - **Solution**: Use `config.getConfigurationStats()` to get quick statistics

### Debug Commands

```python
config = ConfigAdvanced(config_object)

# Debug configuration
print(f"Configuration Parameters: {config.getKeys()}")
print(f"Configuration Statistics: {config.getConfigurationStats()}")

# Debug schema validation
print(f"Schema Validation Rules: {config.getSchemaValidationRules()}")

# Debug validation
print(f"Validation Status: {config.validate()}")
print(f"Validation Errors: {config.getValidationErrors()}")
print(f"Validation Warnings: {config.getValidationWarnings()}")
```

## Migration Guide

### From Basic to Advanced API

If you're migrating from basic NLM Python bindings to advanced bindings, follow these steps:

1. **Upgrade Config Usage**
   ```python
   # Basic
   config = Config(config_file)
   
   # Advanced
   config = ConfigAdvanced(config_object)
   
   # Add advanced features
   schema = config.getSchema()
   config.validate()
   ```

2. **Upgrade Parameter Usage**
   ```python
   # Basic
   neuron_count = config.get('brain.neuronCount')
   
   # Advanced
   config = ConfigAdvanced(config_object)
   neuron_count = config.get('brain.neuronCount')
   config.set('brain.neuronCount', 10000)
   ```

3. **Upgrade Schema Usage**
   ```python
   # Basic
   config = Config(config_file)
   
   # Advanced
   config = ConfigAdvanced(config_object)
   
   # Add advanced features
   schema = config.getSchema()
   validation_result = config.validateSchema(data)
   ```

## Conclusion

The ConfigAdvanced class provides comprehensive access to all configuration management systems through Python-friendly APIs. This documentation covers all major features including schema validation, parameter management, configuration analysis, validation management, Python integration, resource monitoring, export/import, and comparison APIs.

By following the best practices outlined in this documentation, you can effectively use the ConfigAdvanced class to manage NLM configurations with high performance and reliability.

## See Also

- [NLM Python Bindings Documentation](https://nlm.org/docs/python-bindings) - General Python bindings documentation
- [BrainAdvanced Documentation](https://nlm.org/docs/brain-advanced) - BrainAdvanced documentation
- [AgentBrainAdvanced Documentation](https://nlm.org/docs/agent-advanced) - AgentBrainAdvanced documentation
- [NLM Examples](https://nlm.org/examples) - Code examples and tutorials
- [NLM Community](https://nlm.org/community) - Community support and forums

---

*Last updated: 2025-01-01*  
*Version: 0.1.0-advanced*  
*Copyright: NLM Development Team*