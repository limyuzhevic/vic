# NLM CLI and Configuration Management Improvements

This document describes the improvements made to the CLI and configuration management system in NLM Phase 2.

## Changes Overview

### 1. Enhanced CLI (`CLI.hpp`, `CLI.cpp`)

#### New Features:
- **Argument Validation**: All CLI arguments are now validated against type, range, and allowed values
- **Type Safety**: Arguments are properly typed (string, int, double, flag, file)
- **Better Error Messages**: User-friendly error messages with suggestions for corrections
- **Extended Configuration Options**: Added `output_format`, `debug_level`, `quiet` options
- **Graceful Unknown Argument Handling**: Unknown arguments trigger informative error messages with suggestions

#### New CLI Options:
- `--output-format=<format>`: Output format (text, json)
- `--debug-level=<level>`: Debug level (0-3)
- `--quiet, -q`: Suppress non-error output
- Enhanced `--log-level` with more granular control

### 2. Configuration Management (`ConfigManager.cpp`, `ConfigSchema.hpp`, `ConfigSchema.cpp`)

#### New Features:
- **Centralized Configuration Schema**: All configuration parameters defined in one place with validation rules
- **Configuration Validation**: Validates all configuration values against schema rules
- **Better Defaults Management**: Uses schema to apply defaults consistently
- **Configuration Template Support**: `configs/nlm_config_template.cfg` provides comprehensive template
- **Improved Error Handling**: Clear error messages with context for configuration issues
- **Source Tracking**: Tracks where configuration values come from (default, file, command line, runtime)

#### Key Improvements:
- Centralized default values in `ConfigSchema.cpp` instead of hardcoded in `ConfigManager.cpp`
- Consistent validation across all configuration parameters
- Better separation between CLI options and config file values
- More robust file handling with error checking

### 3. Code Quality Improvements

#### Enhanced Error Handling:
- Custom exception classes for CLI and configuration errors
- User-friendly error messages with actionable suggestions
- Graceful fallbacks for non-critical errors

#### Better Separation of Concerns:
- CLI argument parsing separated from configuration loading
- Configuration schema separated from runtime configuration
- Clearer boundaries between different levels of configuration sources

#### Constants and Magic Numbers:
- Configuration parameters defined as constants in the schema
- Better named constants for commonly used values

## Usage Examples

### Basic Usage with New Options:
```bash
# Basic usage
./nlm

# With configuration file and logging
./nlm --config=myconfig.cfg --log-level=info --log-file=nlm.log

# Verbose output with debug level
./nlm --verbose --debug-level=2

# Quiet mode (only errors shown)
./nlm --quiet --log-level=warning

# JSON output format
./nlm --output-format=json --verbose

# Help and information
./nlm --help
./nlm --version
```

### Configuration File Examples:

#### Standard Configuration (`configs/default.cfg`):
```bash
./nlm --config=configs/default.cfg
```

#### Custom Configuration (based on template):
```bash
./nlm --config=custom_nlm_config.cfg
```

#### Using Configuration Template:
```bash
# Copy template and edit
cp configs/nlm_config_template.cfg my_nlm_config.cfg
# Edit my_nlm_config.cfg with your settings
./nlm --config=my_nlm_config.cfg
```

### Error Handling Examples:

#### Invalid Arguments:
```bash
# Shows suggestion for correction
./nlm --log-level=invalud
# Error: Invalid argument '--log-level=invalud'. Expected: log-level
# Did you mean '--log-level=debug'?
```

#### Missing Values:
```bash
# Clear error message
./nlm --output-format
# Error: Missing value for argument '--output-format'. Use '--arg value' or '--arg=value'
```

#### Invalid Values:
```bash
# Configuration validation error
./nlm --config=invalid_config.cfg
# Error: Invalid value for parameter 'neuron_count': Value must be an integer
# Configuration key: neuron_count
# Invalid value: 'invalid'
```

## Configuration Schema

The configuration system now uses a centralized schema defined in `src/ConfigSchema.hpp`. All configuration parameters include:

- Name and description
- Default values
- Type information (int, double, string, bool, file)
- Allowed values (for string parameters)
- Value ranges (for numeric parameters)
- Whether the parameter is required

### Configuration Parameters:

#### Core Simulation Parameters:
- `random_seed`: Random seed for reproducible experiments
- `simulation_timestep`: Simulation timestep in seconds
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Connection probability for random connectivity

#### Plasticity Parameters:
- `stdp_ltp_weight`, `stdp_ltd_weight`, `stdp_tau`
- `synaptogenesis_rate`, `pruning_rate`

#### Logging Configuration:
- `log_level`, `log_file`, `verbose`, `quiet`, `debug_level`

#### Output Configuration:
- `output_format`

#### Simulation Control:
- `max_simulation_steps`, `simulation_time_limit`

#### Environment Configuration:
- `environment_name`, `environment_width`, `environment_height`

#### Neuromodulation Configuration:
- `dopamine_baseline`, `reward_discount_factor`

#### Memory Configuration:
- Working memory, episodic memory settings

#### Development Configuration:
- Developmental stages

#### Checkpoint Configuration:
- Checkpoint saving and loading

#### Visualization Configuration:
- Visualization settings

## Testing

The existing test suite has been updated to test the new functionality:

### New Tests Added:
- CLI argument validation tests
- Configuration schema validation tests
- Error handling tests
- Configuration loading tests

### Running Tests:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_test
```

Or using the provided test script:
```bash
# Run from project root
./run_tests.sh
```

## Migration Guide

### From Old Version:

If upgrading from a previous version, note these changes:

1. **New CLI Options**: Some new options have been added (`--output-format`, `--debug-level`, `--quiet`)
2. **Enhanced Validation**: All configuration values are now validated
3. **Template File**: A new configuration template is available: `configs/nlm_config_template.cfg`
4. **Error Messages**: Error messages are more detailed and include suggestions
5. **Configuration Loading**: Configuration is now loaded from a centralized schema

### Backward Compatibility:

The changes maintain backward compatibility:
- Existing configuration files (like `configs/default.cfg`) continue to work
- Existing CLI usage patterns are still supported
- All existing functionality is preserved
- New options are optional

## File Structure Changes

### New Files:
- `src/ConfigSchema.hpp` - Configuration schema definition
- `src/ConfigSchema.cpp` - Configuration schema implementation
- `configs/nlm_config_template.cfg` - Configuration template
- `docs/` - Updated documentation (to be added)

### Modified Files:
- `src/CLI.hpp` - Enhanced CLI structure
- `src/CLI.cpp` - Enhanced CLI implementation
- `src/ConfigManager.hpp` - Updated ConfigManager interface
- `src/ConfigManager.cpp` - Enhanced ConfigManager implementation
- `src/main.cpp` - Enhanced main application with error handling
- `src/core/Config/Config.hpp` - Added getSource and getDescription methods
- `src/core/Config/Config.cpp` - Added getSource and getDescription implementations

## Best Practices

### Configuration Management:
1. Always specify configuration values in the template first
2. Use the template as a starting point for customization
3. Validate configuration before starting the simulation
4. Use `--verbose` flag for debugging configuration issues
5. Keep configuration files in version control for reproducibility

### CLI Usage:
1. Use `--help` to see all available options
2. Use `--verbose` for detailed output during development
3. Use `--quiet` in production to reduce noise
4. Validate configuration with `--debug-level=2` when troubleshooting
5. Always specify `--config=your_config.cfg` for custom configurations

### Error Handling:
1. Configuration validation happens before simulation starts
2. CLI errors provide actionable suggestions
3. Graceful degradation for non-critical errors
4. Detailed error messages with context
5. Always check exit codes to detect failures

## Performance Considerations

The enhanced configuration system has minimal performance impact:
- Schema initialization happens once at startup
- Validation adds minimal overhead
- Configuration loading is streamlined
- Error handling paths are optimized

## Future Enhancements

The improved configuration system lays the foundation for:
1. JSON/YAML configuration file support
2. Configuration inheritance and overrides
3. Configuration validation plugins
4. Advanced configuration scenarios
5. Integration with configuration management tools

## Conclusion

The enhanced CLI and configuration management system provides:
- More robust and user-friendly argument parsing
- Centralized configuration with validation
- Better error handling and user experience
- Improved maintainability and extensibility
- Backward compatibility with existing configurations

These improvements make NLM Phase 2 more accessible to new users while providing advanced users with greater control and flexibility.