# Enhanced Configuration System - NLM

This document describes the enhanced configuration system for the Neural Learning Machine (NLM) project.

## Overview

The configuration system now supports multiple formats (JSON, YAML, and key=value) with enhanced features including validation, schema support, and environment-specific configurations.

## Configuration Formats

### 1. JSON Configuration (`.json`)

JSON configuration uses a hierarchical structure with nested objects for organization.

**Example:** `configs/default_enhanced.json`

```json
{
  "nlm": {
    "description": "Neural Learning Machine Configuration",
    "version": "2.0",
    "environment": "development",
    "brain": {
      "neuron_count": 500,
      "region_count": 1,
      "connection_probability": 0.15
    }
  }
}
```

### 2. YAML Configuration (`.yaml`)

YAML configuration uses a clean, human-readable format with indentation for structure.

**Example:** `configs/default_enhanced.yaml`

```yaml
# NLM Enhanced Configuration
# Supports neural network simulation parameters

nlm:
  description: "Neural Learning Machine Configuration"
  version: "2.0"
  environment: "development"
  
  brain:
    neuron_count: 500
    region_count: 1
    connection_probability: 0.15
    inhibitory_ratio: 0.2
```

### 3. Key-Value Configuration (`.cfg`)

Traditional key=value format for backward compatibility.

**Example:** `configs/default.cfg`

```
# NLM Configuration File
# Phase 1 skeleton

random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
```

## Configuration Loading Order

1. **Command Line Arguments** (highest priority)
2. **Environment Variables** (NLM_* prefix)
3. **Configuration File** (based on file extension)
4. **Default Values** (lowest priority)

## Command Line Arguments

### Basic Format
```bash
./nlm --config=file.json --neurons=500 --plasticity=true
```

### Shorthand Format
```bash
./nlm -c file.json -n 500 -p true
```

### Boolean Flags
```bash
./nlm --verbose --enable-stdp --enable-plasticity
```

### List Values
```bash
./nlm --neurons=500,600,700 --regions=1,2,3
```

### File Input
```bash
./nlm --config-file=config.json
```

### Environment Variables
```bash
export NLM_ENVIRONMENT=production
export NLM_NEURONS=1000
export NLM_CONNECTION_PROBABILITY=0.2
./nlm
```

## Configuration Groups

The enhanced configuration system supports grouping configuration options:

### Available Groups
- **brain** - Brain and neural network parameters
- **plasticity** - Plasticity and learning parameters
- **development** - Development and maturation parameters
- **neuromodulation** - Neuromodulator parameters
- **memory** - Memory system parameters
- **cognition** - Cognitive and planning parameters
- **environment** - Environment and simulation parameters
- **logging** - Logging and output parameters
- **experiments** - Experiment configuration
- **performance** - Performance optimization parameters
- **validation** - Configuration validation parameters
- **output** - Output configuration

### Example Group Usage
```bash
./nlm --brain.neuron_count=1000 --brain.region_count=2
./nlm --plasticity.stdp_ltp_weight=0.05 --plasticity.hebbian_learning_rate=0.02
./nlm --environment.simulation_timestep=0.001 --environment.max_simulation_steps=5000
```

## Configuration Validation

The enhanced configuration system includes comprehensive validation:

### Type Validation
- Numeric ranges validation
- String enumeration validation
- List validation
- Structure validation

### Schema Validation
```bash
./nlm --validate-schema=production-schema.json
```

### Environment-Specific Validation
```bash
./nlm --environment=production --validate
```

## Configuration Management

### Loading Presets
```bash
./nlm --load-preset=high-performance
./nlm --load-preset=memory-efficient
./nlm --save-preset=custom-config
```

### Configuration Comparison
```bash
./nlm --compare config1.json config2.json
```

### Configuration Merging
```bash
./nlm --merge config1.json config2.json merged.json
```

## Configuration Output

### Save to Different Formats
```bash
./nlm --output=config.json --format=json
./nlm --output=config.yaml --format=yaml
./nlm --output=config.cfg --format=keyvalue
```

### Configuration Documentation
```bash
./nlm --documentation=groups
./nlm --documentation=all
```

## Environment Support

### Available Environments
- **development** - For development and testing
- **testing** - For automated testing
- **staging** - For pre-production testing
- **production** - For production deployment

### Environment-Specific Configuration
```bash
./nlm --environment=production
./nlm --environment=staging
./nlm --environment=testing
./nlm --environment=development
```

## Configuration Examples

### Complete Example with All Features
```bash
./nlm \
  --config=production.json \
  --verbose \
  --enable-stdp \
  --enable-plasticity \
  --brain.neuron_count=2000 \
  --brain.region_count=4 \
  --plasticity.stdp_ltp_weight=0.03 \
  --plasticity.hebbian_learning_rate=0.02 \
  --environment.simulation_timestep=0.0005 \
  --environment.max_simulation_steps=20000 \
  --logging.log_level=DEBUG \
  --logging.log_to_file=true \
  --experiments.default_preset=production \
  --validate \
  --output=results/production.json
```

## Configuration File Formats

### JSON Schema Support
The configuration system supports JSON Schema for validation:

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "properties": {
    "nlm": {
      "type": "object",
      "properties": {
        "brain": {
          "type": "object",
          "properties": {
            "neuron_count": {
              "type": "integer",
              "minimum": 1,
              "maximum": 10000
            }
          }
        }
      }
    }
  }
}
```

### YAML Structure

```yaml
# YAML configuration with anchors and aliases
nlm: &nlm_defaults
  description: "Neural Learning Machine Configuration"
  version: "2.0"

brain: &brain_defaults
  neuron_count: 500
  region_count: 1

production:
  <<: *nlm_defaults
  <<: *brain_defaults
  environment: production
  plasticity:
    stdp_ltp_weight: 0.02
    hebbian_learning_rate: 0.01
```

## Best Practices

### Configuration Organization
1. Use groups to organize related configuration options
2. Use nested structures for complex configurations
3. Document all configuration options with clear descriptions
4. Provide sensible defaults for all critical parameters
5. Use environment-specific configurations for different deployment targets

### Configuration Validation
1. Validate configuration ranges and types
2. Use schemas for production configurations
3. Implement comprehensive error messages
4. Provide configuration examples in documentation
5. Test configuration loading with various formats

### Configuration Management
1. Use version control for configuration files
2. Separate development and production configurations
3. Implement configuration validation in CI/CD pipelines
4. Document configuration changes
5. Use configuration templates for common setups

## Migration Guide

### From Key-Value to JSON/YAML
1. Convert existing `configs/default.cfg` to `configs/default.json` or `configs/default.yaml`
2. Update configuration loading code to detect file format automatically
3. Update command-line parsing to support both formats
4. Test all configuration loading scenarios

### From Old to Enhanced Configuration
1. Add groups to organize configuration options
2. Add validation for critical parameters
3. Add schema validation for production configurations
4. Update documentation
5. Test all functionality

## Performance Considerations

### Large Configuration Files
- Use JSON for large configurations (better performance)
- Use YAML for human-readable configurations
- Use key-value for simple configurations

### Memory Usage
- Monitor memory usage for large configuration files
- Use streaming for very large configuration files
- Implement configuration caching for frequently accessed configurations

### Configuration Loading
- Cache configuration files in memory
- Implement lazy loading for large configurations
- Use format detection to avoid unnecessary parsing

## Troubleshooting

### Common Issues
1. **File not found**: Ensure the configuration file exists
2. **Invalid format**: Check the file extension and content
3. **Validation errors**: Check configuration values against schema
4. **Missing required values**: Add required configuration options
5. **Type mismatches**: Ensure configuration values match expected types

### Debug Commands
```bash
./nlm --config=problematic.json --debug
./nlm --validate-schema=schema.json --verbose
./nlm --environment=production --show-errors
```

## Conclusion

The enhanced configuration system provides a flexible, powerful, and user-friendly way to configure the NLM project. It supports multiple formats, comprehensive validation, and advanced features while maintaining backward compatibility with the existing configuration system.

This configuration system is suitable for both beginners and advanced users, providing a solid foundation for configuring neural network simulations with enhanced flexibility and power.
