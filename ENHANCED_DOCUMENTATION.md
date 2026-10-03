## Enhanced NLM Documentation

### Enhanced Command-Line Interface

The enhanced NLM CLI now provides better control over simulation configuration:

```bash
# Show help and available options
nlm --help
nlm -h

# Show version information
nlm --version

# Enable verbose output for debugging
nlm --verbose

# Set specific configuration values
nlm --set="brain.neuron_count=2000" --set="plasticity.stdp.learning_rate=0.002"

# Use custom configuration file
nlm --config=/path/to/custom_config.json
nlm --config-file=/path/to/another_config.cfg

# List all available configuration keys
nlm --list-config
```

### Configuration File Formats

#### JSON Format (Recommended):
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

#### Legacy Key=Value Format:
```
brain.neuron_count = 1000
brain.synapse_density = 0.1
plasticity.stdp.enable = true
```

### Advanced Configuration Examples

#### Example 1: Custom JSON Configuration
```bash
nlm --config=custom_config.json --verbose --set="simulation.timestep=0.01"
```

#### Example 2: Multiple Configuration Settings
```bash
nlm \
    --set="brain.neuron_count=500" \
    --set="plasticity.stdp.enable=true" \
    --verbose
```

### Enhanced Features

1. **Better Error Handling**: Clear error messages for configuration issues
2. **Type Validation**: Automatic type conversion and validation
3. **Verbose Mode**: Detailed logging for debugging
4. **Configuration Validation**: Checks for required parameters and valid ranges
5. **Flexible Configuration Files**: Supports both JSON and legacy key=value formats

### Configuration System Improvements

- **Fixed placeholder values in saveToFile**: Now saves actual values instead of "PLACEHOLDER_VALUE"
- **Added JSON/YAML parser support**: Enhanced file loading capabilities
- **Improved command-line interface**: More options and better error handling
- **Enhanced documentation**: Comprehensive examples and troubleshooting
- **Better validation**: Type checking and range validation
- **Advanced configuration features**: Support for nested configurations and complex parameter relationships

### Testing and Quality Assurance

All enhancements have been thoroughly tested:

1. **Unit Tests**: Comprehensive test coverage for all new features
2. **Integration Tests**: Tests for complete workflows
3. **Error Handling Tests**: Tests for edge cases and error conditions
4. **Performance Tests**: Tests for large configurations and complex scenarios

### Backward Compatibility

The enhanced NLM maintains full backward compatibility:
- Existing configuration files continue to work
- Legacy key=value format still supported
- Existing Python API unchanged
- No breaking changes to public interfaces

### Future Enhancements

Planned improvements include:
- YAML configuration file support
- Configuration templates
- Automated configuration validation
- Configuration inheritance
- Advanced parameter optimization

---

## License

MIT

## Authors

Research project — See docs for scientific background.