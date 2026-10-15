# NLM (熙然) - Quick Start Guide

## Welcome to NLM - Neural Learning Machine

NLM is an experimental artificial developmental brain project that implements a closed-loop cognitive architecture with neurons, synapses, and emerging intelligence capabilities.

## What's New in Version 1.0

### 🚀 Major Improvements

1. **Configuration Validation System** - New robust validation framework for configuration files
2. **Agent Execution CLI** - User-friendly command-line interface for running experiments
3. **Bug Fixes** - Fixed critical bugs in configuration handling and file I/O
4. **Enhanced Error Handling** - Better error messages and recovery mechanisms
5. **Code Quality** - Improved documentation and code structure

## Quick Start Examples

### 1. Run Basic Neural Computation

```bash
# Run the original Phase 2 demo (with automatic validation)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm
```

### 2. Run Enhanced Agent Execution

```bash
# Build the new agent execution tool
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 6 integration experiment with agent system
./nlm_agent --experiment=phase6 --world=simple --steps=5000

# Run with brain persistence (useful for long experiments)
./nlm_agent --experiment=phase6 --save=brain_state.nlm --output=experiment_report.txt --verbose

# Run with custom configuration
./nlm_agent --config=my_config.cfg --world=simple --time=300
```

### 3. Configuration Validation

Create a configuration file:

```ini
# configs/my_config.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
reward_discount_factor = 0.99
environment_width = 10
environment_height = 10
visualization_update_rate = 30.0
```

The configuration will be automatically validated before execution.

## Key Features

### 🔧 Configuration Validation

- **Automatic validation**: Configs validated before execution
- **Type safety**: Proper type checking and conversion
- **Range checking**: Validates parameter ranges (e.g., neuron_count > 0)
- **Detailed error messages**: Clear feedback on validation failures
- **Custom validators**: Support for custom validation rules

### 🤖 Enhanced Agent Execution

- **Flexible experiment types**: phase3, phase4, phase6, custom
- **Multiple world types**: simple, complex environments
- **Progress monitoring**: Real-time logging and progress tracking
- **Brain persistence**: Save/load brain states for reproducibility
- **Comprehensive reporting**: Detailed experiment reports
- **Command-line options**: Extensive configuration options

### 🐛 Bug Fixes & Improvements

- **Fixed placeholder values**: Config files now save actual values
- **Fixed duplicate entries**: Clean configuration files
- **Better error handling**: Comprehensive error recovery
- **Improved documentation**: Detailed docstrings and examples
- **Enhanced logging**: Structured logging with progress tracking

## Command-Line Reference

### Basic Commands

```bash
# Show help information
./nlm_agent --help

# Run with verbose output
./nlm_agent --experiment=phase6 --verbose

# Run with custom configuration
./nlm_agent --config=experiment.cfg --world=simple

# Run with time limit and save brain
./nlm_agent --experiment=phase6 --time=60 --save=checkpoint.nlm
```

### Configuration Validation

```bash
# Validate a configuration file without running
./nlm_agent --config=test.cfg --no-validate
# (Note: --no-validate skips validation, use --help for validation options)
```

## Configuration Parameters

### Required Parameters

- `random_seed`: Random seed for reproducible experiments (integer)
- `simulation_timestep`: Simulation time step in seconds (double)
- `neuron_count`: Number of neurons in the brain (positive integer)
- `region_count`: Number of brain regions (1-10, integer)
- `connection_probability`: Connection probability for random connectivity (0.0-1.0, float)

### Optional Parameters

- `stdp_ltp_weight`: STDP long-term potentiation weight (float)
- `stdp_ltd_weight`: STDP long-term depression weight (float)
- `stdp_tau`: STDP time constant (float)
- `synaptogenesis_rate`: Synaptogenesis rate (float)
- `pruning_rate`: Pruning rate (float)
- `reward_discount_factor`: Reward discount factor (0.0-1.0, float)
- `environment_width`: Environment width (1-100, integer)
- `environment_height`: Environment height (1-100, integer)
- `visualization_update_rate`: Visualization update rate (0.0-1000.0, float)

## Error Handling

### Configuration Errors

If configuration validation fails, you will see:

```
✗ Configuration validation failed: Invalid value for configuration key 'neuron_count': -5
Continuing with default configuration due to validation errors...
```

The system will continue with default values for invalid parameters.

### File I/O Errors

For file-related errors:

```
✗ Failed to save brain to: brain_state.nlm
✗ Configuration file not found: configs/default.cfg
```

Check file permissions and paths, then retry.

## Development and Testing

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

### Running Tests

```bash
# Run configuration validation tests
./test_config_advanced

# Run existing test suite
make test
```

### Getting Help

For detailed help:

```bash
./nlm_agent --help
```

For API documentation, see the header files in `src/` directory.

## Troubleshooting

### Common Issues

1. **Configuration file not found**
   - Ensure the config file exists at the specified path
   - Use relative or absolute paths
   - Check file permissions

2. **Validation fails for valid configuration**
   - Check for duplicate configuration keys
   - Ensure parameter types match expected types
   - Verify parameter values are within valid ranges

3. **Brain save/load fails**
   - Check disk space availability
   - Verify write permissions for the target directory
   - Ensure the brain object is properly initialized

4. **No output or progress**
   - Check verbose flag: `--verbose`
   - Enable debug logging: `--debug`
   - Verify the experiment type is supported

### Advanced Usage

For advanced usage and custom configurations:

1. **Create custom experiment configurations**
2. **Modify validation rules** in the ConfigValidator class
3. **Extend the AgentBrain class** with custom behavior
4. **Add new world types** by extending the Environment class

## Further Reading

- **IMPROVEMENTS_SUMMARY.md**: Detailed summary of all improvements
- **README.md**: Original project documentation
- **docs/**: Scientific documentation and architecture notes
- **Phase 6 Final Audit**: Complete audit of system integration status

## License

MIT License. See LICENSE file for details.

---

**NLM (熙然) - Neural Learning Machine**
**Phase 6: Final Integration & System Improvements**
**Version 1.0**