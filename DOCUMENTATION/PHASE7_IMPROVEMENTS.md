# Configuration for NLM Phase 7: Enhanced Framework

## Overview

This document outlines the comprehensive improvements made to the NLM (Neural Learning Machine) codebase during Phase 7, focusing on enhancing usability, maintainability, and extensibility.

## Improvements Made

### 1. Unified Configuration System

#### Key Features:
- **Type-safe configuration parameters**: Supports int64_t, double, float, bool, and string values
- **Configuration validation**: Built-in validation for critical parameters (neuron_count, connection_probability, etc.)
- **Multi-source configuration**: Support for configuration from files, command-line arguments, and runtime
- **Parameter tracking**: Tracks the source of each configuration parameter (default, command_line, file, etc.)

#### Benefits:
- Prevents configuration errors through validation
- Maintains configuration history for debugging
- Provides flexible configuration management

#### Files Modified:
- `src/core/Config/UnifiedConfig.hpp` - New unified configuration header
- `src/core/Config/UnifiedConfig.cpp` - Implementation of unified configuration

### 2. Unified CLI Framework

#### Key Features:
- **Common CLI interface**: Standardized command-line interface for all demos
- **Configuration integration**: Seamless integration with configuration system
- **Help system**: Comprehensive help and usage documentation
- **Parameter validation**: Command-line argument validation
- **Version information**: Program metadata display

#### Benefits:
- Consistent user experience across all demos
- Reduced code duplication
- Better user guidance through help system
- Easier parameter management

#### Files Modified:
- `src/core/CLI/UnifiedCLI.hpp` - CLI framework header
- `src/core/CLI/UnifiedCLI.cpp` - CLI framework implementation
- `src/experiments/Phase3Demo.cpp` - Updated to use unified CLI

### 3. Performance Monitoring and Logging

#### Key Features:
- **Advanced performance monitoring**: Tracks task execution times and accumulated performance metrics
- **Multi-level logging**: Debug, Info, Warning, Error, and Critical log levels
- **Console and file output**: Flexible logging destinations
- **Performance reporting**: Detailed performance reports with statistics
- **Log statistics**: Built-in log analysis and reporting

#### Benefits:
- Better performance analysis and optimization
- Improved debugging capabilities
- Comprehensive logging for production use
- Performance tracking for experiment reproducibility

#### Files Modified:
- `src/core/Logging/AdvancedLogger.hpp` - New advanced logging system
- `src/core/Logging/AdvancedLogger.cpp` - Implementation

### 4. Results Export Functionality

#### Key Features:
- **Multiple export formats**: NLM format (key=value), JSON, YAML
- **Structured output**: Organized export of configuration and results
- **Timestamp tracking**: Export of experiment metadata
- **Episode-wise export**: Separate export for each experiment episode

#### Benefits:
- Better experiment reproducibility
- Easier data sharing and analysis
- Integration with external tools and systems
- Comprehensive experiment documentation

#### Files Modified:
- `src/experiments/Phase3Demo.cpp` - Added results export functionality

### 5. Configuration Scripting

#### Key Features:
- **Multiple configuration formats**: JSON, YAML, NLM (key=value)
- **Configuration templates**: Reusable experiment configurations
- **Configuration builder**: Fluent API for building configurations
- **Configuration manager**: Automatic format detection and parsing

#### Benefits:
- Flexible configuration management
- Reusable experiment setups
- Easy configuration sharing
- Programmatic configuration generation

#### Files Modified:
- `src/core/Scripting/ConfigScripting.hpp` - Configuration scripting system
- `src/core/Scripting/ConfigScripting.cpp` - Implementation

### 6. Comprehensive Test Suite

#### Key Features:
- **Unit tests**: Tests for individual components
- **Integration tests**: Tests for system interactions
- **Performance tests**: Performance monitoring and benchmarking
- **Logging tests**: Logging functionality validation
- **Configuration tests**: Configuration system validation

#### Benefits:
- Improved code quality and reliability
- Early bug detection
- Better maintainability
- Documentation of expected behavior

#### Files Modified:
- `tests/TestUnifiedConfig.cpp` - Comprehensive test suite

### 7. Enhanced Phase 3 Demo

#### Key Improvements:
- **Unified CLI integration**: Command-line interface for the demo
- **Configuration file support**: Load configuration from files
- **Results export**: Export experiment results
- **Performance monitoring**: Track and report performance metrics
- **Advanced logging**: Comprehensive logging with different levels
- **Benchmark mode**: Performance benchmarking capabilities

#### Benefits:
- Better user experience
- Improved experiment reproducibility
- Performance analysis capabilities
- Comprehensive logging for debugging

## Migration Guide

### For Existing Users

#### Upgrading from Previous Versions:
1. **Command-line interface**: Use the new unified CLI interface
   - `--help` or `-h`: Show help information
   - `--version` or `-v`: Show version information
   - `--config=FILE`: Load configuration from file
   - `--episodes=N`: Set number of episodes
   - `--steps=N`: Set number of steps per episode

2. **Configuration files**: Use the new unified configuration format
   ```
   # NLM Configuration File
   random_seed = 42
   simulation_timestep = 0.001
   neuron_count = 1000
   region_count = 2
   connection_probability = 0.1
   ```

3. **Experiment configuration**: Use the new configuration builder
   ```cpp
   ExperimentConfigBuilder builder;
   builder.setInt("neuron_count", 1000);
   builder.setFloat("learning_rate", 0.01f);
   builder.setBool("enable_plasticity", true);
   auto config = builder.getConfig();
   ```

### For Developers

#### Adding New Demos:
1. **Include unified CLI headers**: Include `UnifiedCLI.hpp` and `DemoApp` class
2. **Implement configuration**: Use the configuration system for parameter management
3. **Add CLI options**: Use the CLI framework to add command-line options
4. **Implement results export**: Add results export functionality if needed

#### Extending Configuration System:
1. **Add new parameter types**: Extend the `ConfigValue` variant
2. **Add validation**: Implement validation logic for new parameters
3. **Add file parsers**: Implement new parsers for additional formats
4. **Add templates**: Create configuration templates for common setups

#### Enhancing Logging:
1. **Add log levels**: Extend the logging system with new levels
2. **Add output destinations**: Add new logging destinations
3. **Add log formats**: Customize log message formats
4. **Add log analysis**: Implement log analysis tools

## Example Usage

### Basic Usage (Command Line)
```bash
# Run with default settings
./nlm_phase3_demo

# Run with custom settings
./nlm_phase3_demo --episodes 10 --steps 500 --config my_config.cfg

# Run with results export
./nlm_phase3_demo --results-output results.json --benchmark

# Show help information
./nlm_phase3_demo --help
```

### Configuration File Example
```
# config/my_experiment.cfg
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.15

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Logging settings
log_level = INFO
log_to_file = true
log_filename = my_experiment.log
```

### Programmatic Configuration
```cpp
#include "core/Scripting/ConfigScripting.hpp"
#include "core/CLI/UnifiedCLI.hpp"

int main(int argc, char* argv[]) {
    // Create CLI framework
    CLI cli("my_demo", "1.0.0", "My custom demo");
    
    // Add CLI options
    cli.addOption(CLIOption("episodes", 'e', "Number of episodes", true, "5", "int"));
    cli.addOption(CLIOption("steps", 's', "Steps per episode", true, "200", "int"));
    
    // Parse arguments
    cli.parseArgs(argc, argv);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    cli.loadConfig(config);
    
    // Override with command line
    if (cli.hasArg("episodes")) {
        config->setFromString("episodes", cli.getArg("episodes"), "command_line");
    }
    
    // Create experiment
    EnhancedDemo demo;
    demo.run(config);
    
    return 0;
}
```

### Results Export
```cpp
#include "core/CLI/UnifiedCLI.hpp"

int main(int argc, char* argv[]) {
    EnhancedDemo demo("my_demo", "1.0.0", "Demo with results export");
    
    // Parse arguments
    demo.parseArgs(argc, argv);
    
    // Run demo
    int result = demo.execute();
    
    // Export results
    std::string resultsFile = demo.getCLI()->getArg("results-output");
    if (!resultsFile.empty()) {
        demo.exportResults(resultsFile);
    }
    
    return result;
}
```

## Performance Considerations

### Configuration Loading:
- Configuration is loaded once and cached for the duration of the experiment
- File I/O is performed only when necessary
- Configuration validation is performed on first use

### Performance Monitoring:
- Performance monitoring is enabled by default but can be disabled
- Minimal overhead when disabled
- Detailed reporting when enabled

### Logging:
- Log levels filter out messages above the threshold
- File output is buffered for performance
- Console output is line-buffered

## Future Enhancements

### Phase 8: Advanced Features
1. **Distributed Computing**: Support for distributed simulation execution
2. **Visualization Integration**: Real-time visualization of brain activity
3. **Web Interface**: Web-based control and monitoring
4. **Plugin System**: Plugin architecture for extending functionality
5. **Machine Learning Integration**: Integration with modern machine learning frameworks

### Recommended Next Steps:
1. Implement the Phase 8 advanced features
2. Add more configuration templates for common use cases
3. Extend the test suite to cover all new functionality
4. Improve documentation with examples and tutorials
5. Add performance benchmarks for new features

## References

1. **NLM Documentation**: https://nlm.readthedocs.io
2. **GitHub Repository**: https://github.com/nlm-project/nlm
3. **Configuration Format**: NLM configuration specification
4. **Command-line Interface**: NLM CLI specification

## Copyright and License

Copyright (c) 2026 NLM Project

This code is licensed under the MIT License. See LICENSE file for details.

---

For more information about NLM and the Neural Learning Machine project, visit the official website or repository.
