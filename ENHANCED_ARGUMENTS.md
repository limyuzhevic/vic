# Enhanced Command-Line Argument Handling for NLM Phase 2

## Summary of Improvements

This document summarizes the comprehensive improvements made to the command-line argument handling in the NLM (Neural Learning Machine) Phase 2 project.

## 1. Better Argument Parsing

### 1.1 Support for Both --key=value and -k value Formats
The new ArgumentParser class supports both formats:
- `--key=value` (key=value syntax)
- `-k value` (short option with value)

### 1.2 Enhanced Option Parsing
```cpp
// Supported formats:
--config=configs/default.cfg
-c configs/default.cfg
--test=connectivity,plasticity,stdp
-t connectivity,plasticity,stdp
--timestep=0.001
--seed=42
--plasticity_learning_rate=0.01
```

## 2. Configuration File Loading

### 2.1 Robust Configuration File Path Handling
- Automatic fallback to `configs/default.cfg` when no config file specified
- Validation of configuration file existence
- Support for both absolute and relative paths

### 2.2 Configuration Loading Validation
```cpp
// Configuration loading with validation:
if (config->loadFromFile(configFile)) {
    NLM_LOG_INFO("Loaded configuration from: " + configFile);
} else {
    NLM_LOG_INFO("Using default configuration.");
}
```

## 3. Test Selection with Advanced Options

### 3.1 Test Mode Support
Four distinct test modes with different performance and logging characteristics:
- **Fast**: Quick execution with minimal logging
- **Normal**: Standard execution with balanced logging
- **Detailed**: Detailed execution with extensive logging
- **Debug**: Debug execution with maximum logging and validation

### 3.2 Enhanced Test Selection
```cpp
// Test mode configuration:
switch (testMode) {
    case TestMode::Fast:
        testConfig.logDetailedResults = false;
        testConfig.enableProgressReporting = false;
        testConfig.timestep = 0.01;
        break;
    // ... other modes
}
```

## 4. Parameter Validation

### 4.1 Comprehensive Parameter Validation
- Type validation for all parameters
- Range validation for numeric parameters
- Format validation for strings
- Custom validation functions for complex parameters

### 4.2 Validation Examples
```cpp
// Parameter validation:
if (args.timestep <= 0.0 || args.timestep >= 1.0) {
    args.isValid = false;
    args.errorMessage = "Timestep must be between 0 and 1";
}

if (args.seed == 0) {
    args.isValid = false;
    args.errorMessage = "Seed must be non-zero";
}
```

## 5. Enhanced Help System

### 5.1 Detailed Help Documentation
```cpp
void printHelp(const std::string& programName, const ArgumentParser& parser) {
    std::cout << parser.getHelpText(programName) << std::endl;
}
```

### 5.2 Comprehensive Help Information
- Usage information
- All available options with descriptions
- Test mode explanations
- Parameter specifications
- Examples

## 6. Configuration Schema Validation

### 6.1 Configuration Schema Validator
```cpp
class ConfigValidator {
public:
    static bool validateConfig(const Config& config) {
        bool valid = true;
        
        // Validate required keys
        if (!config.has("random_seed")) {
            std::cerr << "Configuration validation error: Missing required key: random_seed" << std::endl;
            valid = false;
        }
        
        // Validate types and ranges
        if (config.has("simulation_timestep")) {
            auto timestepOpt = config.get<double>("simulation_timestep");
            if (!timestepOpt || *timestepOpt <= 0.0 || *timestepOpt >= 1.0) {
                std::cerr << "Configuration validation error: simulation_timestep must be between 0 and 1" << std::endl;
                valid = false;
            }
        }
        
        return valid;
    }
};
```

## 7. Enhanced Error Handling

### 7.1 Improved Error Messages
```cpp
if (!args.isValid) {
    std::cerr << "Error: " << args.errorMessage << std::endl;
    printHelp(argv[0], parser);
    return 1;
}
```

### 7.2 Error Recovery Options
- Graceful handling of invalid arguments
- Detailed error messages with suggestions
- Option to continue with defaults when possible

## 8. Logging Configuration

### 8.1 Flexible Logging System
```cpp
void setupLogging(const ArgumentParser::ParsedArgs& args) {
    auto logger = std::make_shared<Logger>();
    
    // Console logger
    LogLevel level = LogLevel::Info;
    if (args.verbose) {
        level = LogLevel::Debug;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    logger->addLogger(consoleLogger);
    
    // File logger
    if (!args.logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(args.logFile, level);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
}
```

## 9. Performance Tuning

### 9.1 Simulation Parameters
```cpp
// Performance tuning options:
--timestep=<value>          Simulation timestep (default: 0.001)
--max_simulation_steps=<value> Maximum simulation steps
--plasticity_learning_rate=<value> Plasticity learning rate
--connection_probability=<value> Connection probability
--simulation_time_limit=<value> Simulation time limit
```

### 9.2 Test Mode Performance
Each test mode optimizes parameters for different use cases:
- **Fast**: Larger timesteps, fewer steps
- **Normal**: Balanced configuration
- **Detailed**: Smaller timesteps, more steps
- **Debug**: Smallest timesteps, most steps

## 10. Persistence Options

### 10.1 Save/Load Configuration State
```cpp
void saveCurrentConfiguration(const Config& config, const std::string& filepath) {
    std::cout << "Saving configuration to: " << filepath << std::endl;
    if (config.saveToFile(filepath)) {
        std::cout << "Configuration saved successfully!" << std::endl;
    } else {
        std::cerr << "Error: Failed to save configuration" << std::endl;
    }
}
```

## Key Features Summary

### 1. Backward Compatibility
- All existing command-line arguments continue to work
- Legacy compatibility layer maintained
- Existing configuration files supported

### 2. New Features
- Enhanced argument parsing (key=value, -k value)
- Multiple test modes (fast, normal, detailed, debug)
- Configuration schema validation
- Flexible logging configuration
- Persistent state management
- Advanced parameter validation

### 3. Performance Considerations
- Optimized test modes for different use cases
- Configurable simulation parameters
- Efficient validation mechanisms
- Minimal overhead for enhanced features

### 4. Error Handling
- Comprehensive error messages
- Graceful error recovery
- Detailed validation feedback
- Informative help system

## Usage Examples

### Basic Usage
```bash
./nlm --test=connectivity,plasticity --verbose
./nlm -t connectivity -c custom.cfg
```

### Advanced Usage
```bash
./nlm --test-mode=debug --timestep=0.0001 --seed=123
./nlm --log-level=debug --log-file=nlm.log --save-config
custom_config.cfg
```

### Configuration File Usage
```bash
./nlm --config=experiments/config.cfg
```

## Implementation Files

1. **src/core/ArgumentParser.hpp** - Enhanced argument parser class
2. **src/core/ArgumentParser.cpp** - Argument parser implementation
3. **src/core/ConfigValidator.hpp** - Configuration schema validator
4. **src/core/ConfigValidator.cpp** - Validator implementation
5. **src/main.cpp** - Updated main function with enhanced features

## Testing

The implementation includes:
- Unit tests for argument parsing
- Configuration validation tests
- Error handling tests
- Integration tests with existing test framework

## Conclusion

The enhanced command-line argument handling system provides:
- **Robust parsing** with multiple formats
- **Comprehensive validation** of all parameters
- **Flexible configuration** loading and saving
- **Performance tuning** options for different use cases
- **Enhanced logging** and debugging capabilities
- **Persistent state** management
- **Full backward compatibility** with existing code

This implementation significantly improves the usability and flexibility of the NLM Phase 2 project while maintaining all existing functionality.