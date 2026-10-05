# NLM Configuration Enhancement - README

## Overview

This document describes the enhanced configuration system for NLM (Neural Learning Machine). The configuration system has been significantly enhanced to provide comprehensive configuration management capabilities for creating, validating, optimizing, and managing NLM configurations.

## Key Features

### 1. Advanced Configuration Management

The enhanced configuration system provides:

- **AdvancedConfigManager**: Main class for advanced configuration management
- **ConfigSchema**: Schema definition for validation rules
- **ConfigTemplate**: Template system for common use cases
- **ConfigPreset**: Preset system for predefined configurations
- **ValidationError**: Detailed validation error reporting

### 2. Configuration Templates and Presets

#### Predefined Presets

- **training**: Optimized for neural network training and learning
- **simulation**: Optimized for stable simulation runs
- **research**: Optimized for research and experimentation
- **development**: Optimized for development and debugging

#### Configuration Templates

- **advanced**: Comprehensive template with all NLM features
- **custom**: User-defined custom templates

### 3. Configuration Validation

#### Validation Rules

The system supports comprehensive validation including:
- Type validation (int, double, bool, string, arrays)
- Range validation (min/max values)
- Enum validation (allowed values)
- Size validation for arrays
- Required field validation
- Custom validation rules

#### Validation Reports

Detailed validation reports include:
- Validation status (PASSED/FAILED)
- Total, valid, and invalid entry counts
- Specific validation error details with error codes
- Configuration summary information

### 4. Configuration Comparison

#### Comparison Features

- **Similarity Calculation**: Compare two configurations with similarity scoring
- **Change Tracking**: Track modifications between configuration versions
- **Difference Analysis**: Create difference configurations
- **Subset Creation**: Extract specific parameter subsets

### 5. Configuration Optimization

#### Optimization Tools

- **Automatic Optimization**: Optimize parameters for specific use cases
- **Performance Analysis**: Analyze configuration performance
- **Improvement Suggestions**: Generate optimization suggestions
- **Optimized Preset Generation**: Create optimized presets based on analysis

### 6. Configuration Export/Import

#### Supported Formats

- **JSON**: Full configuration export/import with pretty printing
- **YAML**: Alias to JSON export/import
- **INI/CFG**: File format import (for backward compatibility)

#### Export Options

- Include/exclude default values
- Pretty printing for human-readable output
- Comprehensive metadata inclusion

### 7. Configuration Analysis and Insights

#### Analysis Features

- **Pattern Detection**: Detect parameter patterns and correlations
- **Performance Estimation**: Estimate configuration performance for different use cases
- **Parameter Statistics**: Calculate statistical properties of parameters
- **Recommendation System**: Get configuration recommendations for specific tasks

### 8. Configuration Backup and Restore

#### Backup Features

- **Automated Backup**: Create timestamped backups
- **Restore Functionality**: Restore from backup files
- **Configuration Hashing**: Create unique configuration hashes for integrity verification

### 9. Dynamic Configuration Management

#### Dynamic Features

- **Auto-reload**: Automatically reload configuration files at specified intervals
- **Update Callbacks**: Register callbacks for configuration changes
- **Real-time Validation**: Validate configuration on changes
- **Change Tracking**: Track all configuration modifications

## Python Integration

The enhanced configuration system is fully exposed through Python bindings via `pybind11`. Python users can access all C++ configuration capabilities through:

### Python API Examples

```python
from python.nlm_utils import ConfigManager, ConfigBuilder, ConfigValidator

# Create default configuration
config = ConfigBuilder.create_default_config()

# Apply preset
config.apply_preset("training")

# Validate configuration
errors = ConfigValidator.validate_advanced_config(config)
if errors:
    print("Validation errors:", errors)
else:
    print("Configuration is valid")

# Create brain with advanced configuration
brain = nlm_py.createBrain(config)
brain.initialize()
```

### Python Examples Directory

The `python/examples/` directory contains comprehensive examples:

- **config_builder.py**: Demonstrates configuration building and optimization
- **advanced_config_demo.py**: Shows all advanced configuration features

## Usage

### Building the Enhanced Configuration System

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Tests

```bash
cd tests
./nlm_test
```

Or using Python tests:

```bash
python -m pytest python/tests/ -v
```

### Running Examples

Python examples can be run directly:

```bash
python python/examples/config_builder.py
python python/examples/advanced_config_demo.py
```

### Command Line Interface

The enhanced configuration system provides command line tools:

```bash
# Build configuration templates
nlm generate-template --output=config_template.cfg

# Run with advanced configuration
nlm run --config=advanced_config.cfg

# Validate configuration
nlm validate --config=my_config.cfg

# Create optimized configuration
nlm optimize --use-case=training --output=optimized_config.json
```

## Configuration File Formats

### JSON Configuration

```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 1000,
  "plasticity_learning_rate": 0.01,
  "log_level": "INFO"
}
```

### Template Configuration File (.cfg)

```
# NLM Configuration File
# Advanced Configuration Template

# Basic Settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Plasticity Settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Development Settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation Settings
dopamine_baseline = 0.0

# Environment Settings
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

## API Reference

### AdvancedConfigManager Class

| Method | Description |
|--------|-------------|
| `addSchema(schema)` | Add configuration schema |
| `removeSchema(key)` | Remove configuration schema |
| `loadTemplate(name)` | Load configuration template |
| `saveTemplate(name, description)` | Save configuration template |
| `applyPreset(name)` | Apply configuration preset |
| `validateAll()` | Validate all configuration |
| `isValid()` | Check if configuration is valid |
| `exportToJSON(filepath, includeDefaults, prettyPrint)` | Export to JSON |
| `importFromJSON(filepath)` | Import from JSON |
| `optimize(useCase)` | Optimize configuration |
| `generateDocumentation(format)` | Generate documentation |
| `calculateSimilarity(other)` | Calculate similarity with another config |

### ConfigSchema Class

| Property | Type | Description |
|----------|------|-------------|
| key | string | Configuration key |
| type | string | Value type (int, double, bool, string, vector) |
| description | string | Human-readable description |
| defaultValue | string | Default value |
| required | bool | Whether field is required |
| allowedValues | array | Allowed values |
| minValue/maxValue | double | Range constraints |
| minSize/maxSize | int | Array size constraints |

## Migration Guide

### From Basic Config to AdvancedConfigManager

For users who were using the basic Config class, migration is straightforward:

```cpp
// Old way
nlm::Config config;
config.set("param", 42);

// New way (backward compatible)
nlm::AdvancedConfigManager config;
config.set("param", 42);

// Additional advanced features
config.addSchema(schema);  // Add validation
config.loadTemplate("advanced");  // Load preset
config.optimize("training");  // Get optimization suggestions
```

## Testing

### Unit Tests

The enhanced configuration system includes comprehensive unit tests:

- **test_advanced_config.cpp**: Tests all advanced configuration features
- **test_config.cpp**: Tests basic configuration functionality
- **test_validation.cpp**: Tests validation features

Run tests with:

```bash
cd tests
./nlm_test
```

### Performance Testing

Performance benchmarks are available for configuration operations:

- Configuration validation performance
- Export/import performance
- Optimization performance
- Memory usage analysis

## Troubleshooting

### Common Issues

1. **Validation Errors**
   - Check schema definitions for syntax errors
   - Verify parameter types match schema
   - Ensure required parameters are provided

2. **Import/Export Issues**
   - Verify file paths are correct
   - Check JSON syntax for malformed files
   - Ensure proper permissions for file operations

3. **Optimization Problems**
   - Check use case strings against supported options
   - Verify configuration has all required schemas
   - Ensure parameter values are within valid ranges

### Support

For issues with the enhanced configuration system:

1. Check the generated validation reports for specific error details
2. Review the configuration documentation for schema requirements
3. Consult the example files for usage patterns
4. Check the test logs for detailed error information

## Future Enhancements

The enhanced configuration system is designed to be extensible. Future enhancements may include:

- **Machine Learning Optimization**: ML-based configuration optimization
- **Configuration Versioning**: Configuration history and versioning
- **Collaboration Support**: Shared configuration workspaces
- **Web Interface**: Web-based configuration management
- **Docker Integration**: Docker-based configuration management

## Conclusion

The enhanced configuration system provides comprehensive configuration management capabilities for NLM, making it easier to create, validate, optimize, and manage NLM configurations. The system is backward compatible with the basic Config class while providing advanced features for production use.

## License

MIT License - See LICENSE file for details.

## Authors

NLM Project Team

## Version

Enhanced Configuration System v2.0
