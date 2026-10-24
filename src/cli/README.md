# CLI and Argument Parsing Framework

## Overview
This framework provides a unified command-line interface for NLM (Neural Learning Machine) with support for:
- Subcommands (run, config, experiment, utils)
- Configuration file support (--config)
- Parameter overrides (--key value or --key=value)
- Help and version flags (--help, --version)
- Validation and error handling
- Advanced features for power users

## Main CLI Entry Point

File: `src/cli/CLI.cpp`

This is the main entry point that provides a unified interface for all NLM operations.

## Features

### 1. Subcommand System
- `run` - Run simulations and experiments
- `config` - Configuration file management
- `experiment` - Experiment management and creation
- `utils` - Utility functions

### 2. Configuration Management
- Load configurations from multiple formats (JSON, YAML, custom formats)
- Save configurations with validation
- Profile and preset support

### 3. Experiment Control
- Run specific experiments with custom parameters
- Load/save experiment configurations
- Parameter optimization

### 4. Advanced Features
- Real-time monitoring integration
- Performance profiling
- Debugging and development tools

## Usage Examples

### Basic Usage
```bash
# Run a simulation with default configuration
./nlm run --config configs/default.cfg

# Run with custom parameters
./nlm run --config configs/test.cfg --neuron_count=2000 --stdp_ltp_weight=0.025

# Run specific experiment
./nlm run --experiment=connectivity_test --steps=5000

# Create new experiment configuration
./nlm config create --name=my_experiment --from-template=plasticity

# Load experiment from file
./nlm experiment load my_experiment.cfg

# Utility functions
./nlm utils benchmark --duration=60
./nlm utils validate --file=configs/checkpoint.cfg
```

### Advanced Usage
```bash
# Run with profiling
./nlm run --profile=full --profile_output=perf.prof --config=my_config.cfg

# Run multiple simulations in batch
./nlm batch run --config-list=experiments.txt --output-dir=batch_results

# Advanced parameter tuning
./nlm optimize --experiment=learning --parameter-space="neuron_count=[100,1000],stdp_ltp_weight=[0.01,0.05]" --iterations=100

# Real-time monitoring
./nlm monitor --web-port=8080 --log-level=debug --config=live.cfg

# Compare experiment results
./nlm compare --experiment=exp1.cfg --experiment=exp2.cfg --output=comparison.json

# Export results
./nlm export --experiment=exp1.cfg --format=json --output=exp1_results.json

# Import custom brain architecture
./nlm import --brain=custom_brain.nml --format=brainml --config=custom.cfg
```

## File Structure

```
src/cli/
├── CLI.hpp                    # Main CLI interface
├── CLI.cpp                    # CLI implementation
├── SubcommandBase.hpp          # Base class for subcommands
├── RunCommand.hpp/.cpp         # Run simulation command
├── ConfigCommand.hpp/.cpp      # Configuration management command
├── ExperimentCommand.hpp/.cpp  # Experiment management command
├── UtilsCommand.hpp/.cpp       # Utility commands
├── Common.hpp                 # Common CLI utilities and types
└── ArgumentParser.hpp          # Advanced argument parsing

include/nlm/cli/
├── CLI.hpp                    # Public API
├── Command.hpp                # Command interface
├── Subcommand.hpp             # Subcommand base class
└── Result.hpp                 # Command execution results

src/utils/
├── ConfigValidator.hpp        # Configuration validation
├── ProfileManager.hpp         # Profile management
└── ParameterOptimizer.hpp     # Parameter optimization

include/nlm/utils/
├── ConfigValidator.hpp
├── ProfileManager.hpp
└── ParameterOptimizer.hpp
```

## Key Features

### 1. Unified Interface
- Consistent interface across all subcommands
- Tab completion support
- Comprehensive help system

### 2. Advanced Configuration
- Multiple configuration formats
- Configuration inheritance and merging
- Profile and preset support

### 3. Experiment Management
- Experiment templates
- Custom experiment creation
- Experiment comparison and analysis

### 4. Development Tools
- Real-time monitoring
- Performance profiling
- Debugging utilities

### 5. Extensibility
- Plugin architecture for commands
- Custom command registration
- Extension point system

## Integration with Existing Code

The CLI framework integrates with existing NLM components:
- Configuration system (src/core/Config/)
- Brain system (src/brain/)
- Experiment system (src/experiments/)
- Utility systems (src/utils/)

## Benefits

### For Users
- **Easy to use**: Simple commands for common tasks
- **Powerful**: Advanced features for power users
- **Flexible**: Configurable behavior through parameters
- **Extensible**: Plugin architecture for custom commands
- **Well documented**: Comprehensive help and examples

### For Developers
- **Modular**: Easy to add new features
- **Testable**: Unit tests for all CLI components
- **Maintainable**: Clean separation of concerns
- **Extensible**: Plugin architecture for customization
- **Documented**: Doxygen documentation

## Build Instructions

### Basic Build
```bash
cd /workspace/app
mkdir build && cd build
cmake ..
make -j4
```

### Build with CLI Features
```bash
cd /workspace/app
mkdir build && cd build
cmake -DNLM_ENABLE_CLI=ON ..
make -j4
```

### Testing
```bash
./nlm test --cli --verbose
cd build && ctest --output-on-failure
```

## Performance Considerations

- **Startup time**: CLI initialization is optimized for fast startup
- **Memory usage**: Command objects are created lazily
- **File I/O**: Configuration files are cached
- **Validation**: Configuration validation is performed at runtime
- **Error handling**: Robust error recovery with informative messages

## Error Handling

The CLI framework provides comprehensive error handling:
- **Input validation**: Invalid arguments trigger helpful error messages
- **File I/O errors**: Clear error messages with file details
- **Configuration errors**: Detailed validation error messages
- **Runtime errors**: Graceful error handling with recovery options
- **Help system**: Automatic help generation for invalid commands

## Future Enhancements

1. **Web Interface**: RESTful API for remote control
2. **Docker Integration**: Dockerized CLI for containerized deployment
3. **GUI Interface**: Graphical user interface for visual configuration
4. **Distributed Computing**: Support for distributed simulation execution
5. **Cloud Integration**: Integration with cloud computing platforms

## Conclusion

This CLI framework transforms NLM from a prototype into a production-ready neural simulation platform. It provides:
- **Unified interface** for all operations
- **Advanced features** for power users
- **Extensible architecture** for future enhancements
- **Comprehensive documentation** and examples
- **Robust error handling** and validation

The framework is designed to be both easy to use for beginners and powerful enough for advanced research and development work.