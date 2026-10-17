# NLM - Neural Learning Machine
# Enhanced Documentation and User Guides

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) project, covering all enhancements made to improve the codebase, user experience, and development workflow.

## Table of Contents

1. [Introduction](#introduction)
2. [Enhanced Configuration System](#enhanced-configuration-system)
3. [Enhanced Command Line Interface](#enhanced-command-line-interface)
4. [Phase 3 Demo - Advanced Features](#phase-3-demo---advanced-features)
5. [Phase 4 Demo - Enhanced Visualization and Monitoring](#phase-4-demo---enhanced-visualization-and-monitoring)
6. [Main Application - Enhanced Core Functionality](#main-application---enhanced-core-functionality)
7. [Testing and Validation](#testing-and-validation)
8. [Performance Optimization](#performance-optimization)
9. [Development Guidelines](#development-guidelines)
10. [Troubleshooting](#troubleshooting)
11. [API Reference](#api-reference)
12. [Contributing](#contributing)

## Introduction

NLM (熙然) is an experimental artificial developmental brain project that implements real spiking neural computation with biologically plausible mechanisms. This project has been enhanced with comprehensive improvements across configuration management, user interface, testing, and documentation.

### Key Improvements

- **Enhanced Configuration System**: Advanced configuration parsing with JSON/YAML support, validation, inheritance, and environment variable override
- **Advanced Command Line Interface**: Comprehensive argument parsing with enhanced options and validation
- **Improved User Experience**: Better progress monitoring, visualization, and error handling
- **Enhanced Testing**: Comprehensive test coverage with statistical analysis
- **Performance Optimizations**: Advanced profiling and optimization capabilities
- **Multi-platform Support**: Cross-platform build system and deployment capabilities

## Enhanced Configuration System

### Configuration File Formats

The enhanced configuration system supports multiple formats:

#### Simple Format (key = value)
```
# Basic configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
```

#### JSON Format
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012
}
```

#### YAML Format
```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1
plasticity_learning_rate: 0.01
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
```

### Configuration Features

#### 1. Configuration Inheritance

Configuration files can inherit from parent configurations:

```
# parent.cfg
random_seed = 42
simulation_timestep = 0.001
base_config = true

# child.cfg
random_seed = 100
neuron_count = 1000
# Inherits from parent.cfg
```

#### 2. Environment Variable Override

Environment variables can override configuration file values:

```bash
export NLM_RANDOM_SEED=123
export NLM_NEURON_COUNT=2000
export NLM_SIMULATION_TIMESTEP=0.0001
```

#### 3. Configuration Validation

Configuration values are validated against predefined rules:

```python
def validate_random_seed(value):
    if not isinstance(value, int):
        return False, "Random seed must be an integer"
    if value < 0 or value > 2**31 - 1:
        return False, "Random seed must be between 0 and 2^31-1"
    return True, ""

def validate_neuron_count(value):
    if not isinstance(value, int):
        return False, "Neuron count must be an integer"
    if value < 10 or value > 100000:
        return False, "Neuron count must be between 10 and 100000"
    return True, ""
```

#### 4. Configuration Templates

Preset configurations for common scenarios:

```python
# Phase 2: Real Neural Computation
PHASE2_PRESET = {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_time_constant": 20.0
}

# Phase 3: World Interaction
PHASE3_PRESET = {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "environment_name": "GridWorld",
    "environment_width": 20,
    "environment_height": 20,
    "vision_width": 16,
    "vision_height": 16
}

# Phase 4: Emerging Cognition
PHASE4_PRESET = {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "log_level": "info",
    "log_to_file": False,
    "enable_visualization": False
}
```

### Configuration Command Line Options

```bash
# Configuration file loading
--config <file>                    # Configuration file path (default: configs/default.cfg)
--config-format <format>            # Configuration format (simple, json, yaml, auto)
--inherit-config <file>             # Parent configuration file for inheritance

# Configuration validation
--validate-config                   # Validate configuration file
--compare-config <file>             # Compare with configuration file

# Configuration management
--export-config <file>              # Export configuration to file
--import-config <file>              # Import configuration from file
--backup-config                     # Create configuration backup
--restore-config <name>             # Restore configuration from backup

# Environment variable override
--env-override                      # Enable environment variable override
--env-prefix <prefix>              # Environment variable prefix (default: NLM_)

# Configuration templates
--config-template <name>           # Apply preset configuration template

# Configuration history
--config-history                    # Show configuration history
--config-version <name>             # Show configuration version
```

## Enhanced Command Line Interface

### Core Options

```bash
# Basic options
--experiment <name>                # Experiment name (basic, phase2, phase3, phase4)
--steps <N>                        # Number of simulation steps
--episodes <N>                     # Number of episodes for world interaction
--output <file>                    # Output file path

# Configuration options
--config <file>                    # Configuration file path
--config-format <format>            # Configuration file format
--log-level <level>                # Log level (debug, info, warning, error)
--log-file <file>                  # Log file path
--seed <N>                         # Random seed

# Advanced options
--benchmark                        # Enable benchmarking mode
--validate                         # Validate configuration
--help                             # Show help information
--version                          # Show version information
```

### Advanced Options

```bash
# Experiment modes
--parallel                         # Run experiments in parallel
--compare                          # Compare experiment results
--visualize                        # Enable visualization
--adaptive                         # Enable adaptive parameters
--statistical                      # Enable statistical analysis
--detailed                         # Enable detailed output

# Configuration management
--env-override                     # Enable environment variable override
--backup                           # Create backup before running
--restore                          # Restore from backup after running
--config-history                   # Show configuration history
--config-version <name>            # Show configuration version

# Positional arguments
experiment                         # Experiment name
output                            # Output file path
```

### Usage Examples

#### Basic Usage
```bash
# Run basic experiment with default configuration
./nlm --experiment basic --steps 1000 --output results.txt

# Run Phase 3 experiment with custom configuration
./nlm --experiment phase3 --config my_config.cfg --steps 5000 --episodes 10
```

#### Advanced Usage
```bash
# Run multiple experiments in parallel
./nlm --experiment phase4 --parallel --steps 2000 --episodes 5

# Compare experiment results with statistical analysis
./nlm --experiment phase3 --compare --statistical --steps 3000

# Run with validation and backup
./nlm --experiment phase2 --validate --backup --steps 1000

# Export configuration to JSON format
./nlm --experiment phase4 --export-config config.json --steps 500
```

## Phase 3 Demo - Advanced Features

### Enhanced User Interface

The Phase 3 demo now features an advanced user interface with:

- **Real-time Progress Monitoring**: Progress bars with percentage completion and ETA calculations
- **Color-coded Output**: Color-coded console output for better readability
- **Interactive Status Updates**: Real-time status updates for experiment progress
- **Comprehensive Metrics**: Detailed metrics collection and analysis

### Advanced Features

#### 1. Multiple Exploration Strategies

- **Epsilon-Greedy**: Balances exploration and exploitation
- **UCB1**: Upper Confidence Bound for exploration
- **Thompson Sampling**: Probabilistic exploration

#### 2. Learning Metrics

- **Performance Metrics**: Reward, success rate, learning curves
- **Statistical Analysis**: Confidence intervals, p-values, significance testing
- **Behavioral Analysis**: Action frequencies, exploration rates, adaptation patterns

#### 3. Adaptive Parameters

- **Dynamic Learning Rate**: Adaptive learning rate based on performance
- **Exploration-Exploitation Balance**: Automatic adjustment of exploration factors
- **Self-tuning Configuration**: Automatic parameter optimization

### Usage Examples

```bash
# Run Phase 3 demo with advanced features
./nlm_phase3_demo --episodes 20 --steps 200 --strategy epsilon_greedy --learning-rate 0.01 --exploration-factor 0.1

# Run with statistical analysis and visualization
./nlm_phase3_demo --episodes 30 --steps 300 --statistical --visualize --output phase3_results.csv

# Compare different exploration strategies
./nlm_phase3_demo --episodes 15 --steps 150 --compare --strategies epsilon_greedy,ucb1,thompson
```

## Phase 4 Demo - Enhanced Visualization and Monitoring

### Enhanced Visualization

The Phase 4 demo features advanced visualization capabilities:

- **Multiple Output Formats**: Console, JSON, CSV, and HTML output
- **Real-time Charts**: Interactive charts and graphs for data visualization
- **Performance Dashboards**: Comprehensive dashboards with metrics monitoring
- **Historical Trend Analysis**: Time-series analysis and trend identification

### Advanced Monitoring

#### 1. Comprehensive Metrics Collection

- **Temporal Prediction Learning**: Prediction accuracy, error rates, learning curves
- **Working Memory**: Retention rates, capacity measurements, interference effects
- **Episodic Memory**: Recall accuracy, storage capacity, retrieval performance
- **Concept Formation**: Pattern discovery, category stability, generalization ability
- **Neural Attention**: Selectivity, distraction resistance, attention dynamics
- **Multi-step Planning**: Planning accuracy, confidence levels, decision quality
- **Self-Model**: Self-prediction accuracy, body awareness, self-other distinction
- **Social Learning**: Imitation accuracy, observation effectiveness, social interaction
- **Continual Learning**: Behavior improvement, transfer performance, adaptation rates

#### 2. Statistical Analysis

- **Statistical Significance Testing**: T-tests, ANOVA, effect size calculations
- **Comparative Analysis**: Statistical comparison between experimental conditions
- **Confidence Intervals**: Estimation of parameter uncertainties
- **Performance Benchmarking**: Comparison with baseline and control conditions

### Usage Examples

```bash
# Run Phase 4 demo with enhanced visualization
./nlm_phase4_demo 50 --visualize --statistical --detailed --output phase4_results.json

# Run with custom trial count and detailed analysis
./nlm_phase4_demo 100 --trials 100 --statistical --detailed --confidence 0.95 --alpha 0.05

# Run with parallel execution and comparison
./nlm_phase4_demo 25 --parallel --compare --visualize --output phase4_comparison.html
```

## Main Application - Enhanced Core Functionality

### Enhanced Configuration Management

The main application now features enhanced configuration management:

#### 1. Configuration Loading

```cpp
// Load configuration with inheritance and validation
bool loadConfig(const std::string& filepath, 
                const std::string& format = "auto",
                bool inherit = true,
                bool validate = true,
                bool overrideWithEnv = true);
```

#### 2. Configuration Validation

```cpp
// Validate configuration with detailed error reporting
bool validateConfig();
std::vector<std::string> getValidationErrors();
```

#### 3. Configuration Management

```cpp
// Create, restore, and manage configuration backups
bool createConfigBackup(const std::string& name = "");
bool restoreConfigBackup(const std::string& name = "latest");
std::vector<std::string> listConfigBackups();
```

### Enhanced Argument Parsing

```cpp
// Advanced argument parsing with validation
struct Argument {
    std::string longName;
    char shortName;
    std::string description;
    enum Type { FLAG, VALUE, POSITIONAL } type;
    bool required;
    std::string defaultValue;
    std::string value;
    bool specified;
    std::function<bool(const std::string&)> validator;
};

// Add argument with validation
parser.addOption("--experiment", 'e', "Experiment name", 
                Argument::Type::VALUE, false, "basic",
                [](const std::string& val) { return val == "basic" || val == "phase2" || ...; });
```

### Enhanced Error Handling

```cpp
// Comprehensive error handling with recovery
class ErrorHandler {
public:
    enum ErrorCode {
        SUCCESS,
        CONFIG_FILE_NOT_FOUND,
        CONFIG_FILE_FORMAT_ERROR,
        CONFIG_VALIDATION_ERROR,
        ARGUMENT_PARSE_ERROR,
        APPLICATION_RUN_ERROR
    };
    
    bool handleError(ErrorCode code, const std::string& message = "");
    std::string getErrorMessage(ErrorCode code) const;
};
```

### Performance Optimization

```cpp
// Performance monitoring and optimization
class PerformanceMonitor {
public:
    void startTiming(const std::string& operation);
    void stopTiming(const std::string& operation);
    double getElapsedTime(const std::string& operation) const;
    void printPerformanceReport() const;
};
```

## Testing and Validation

### Comprehensive Test Coverage

The enhanced testing system includes:

#### 1. Unit Tests

- **Configuration Tests**: Configuration parsing, validation, and management
- **Argument Parser Tests**: Command-line argument parsing and validation
- **Experiment Tests**: Experiment execution and validation
- **Performance Tests**: Performance benchmarking and optimization

#### 2. Integration Tests

- **System Tests**: End-to-end system testing
- **Regression Tests**: Regression testing for existing functionality
- **Compatibility Tests**: Cross-platform compatibility testing

#### 3. Statistical Analysis

- **Performance Analysis**: Statistical analysis of performance metrics
- **Significance Testing**: Statistical significance testing for experimental results
- **Comparative Analysis**: Comparative analysis between different experimental conditions

### Test Framework

```cpp
// Example test case
class EnhancedConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<EnhancedConfig>();
        config_->load("test_config.cfg");
    }
    
    void TearDown() override {
        config_.reset();
    }
    
    std::shared_ptr<EnhancedConfig> config_;
};

TEST_F(EnhancedConfigTest, LoadConfigFromFile) {
    EXPECT_TRUE(config_->load("test_config.cfg"));
    EXPECT_EQ(config_->getEntry("random_seed").value(), ConfigValue(int64_t(42)));
}

TEST_F(EnhancedConfigTest, ValidateConfig) {
    EXPECT_TRUE(config_->validate());
    std::vector<std::string> errors = config_->getValidationErrors();
    EXPECT_TRUE(errors.empty());
}
```

## Performance Optimization

### Profiling and Optimization

The enhanced performance optimization system includes:

#### 1. Performance Monitoring

- **Timing Measurements**: Accurate timing measurements for critical operations
- **Memory Profiling**: Memory usage monitoring and optimization
- **CPU Profiling**: CPU usage analysis and optimization

#### 2. Optimization Strategies

- **Caching**: Intelligent caching for frequently accessed data
- **Lazy Loading**: Deferred loading of large data structures
- **Parallel Processing**: Parallel execution for CPU-intensive tasks

#### 3. Performance Benchmarking

```cpp
// Performance benchmarking
class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string operation;
        double elapsedTime;
        size_t memoryUsage;
        double cpuUsage;
        std::string timestamp;
    };
    
    void startBenchmark(const std::string& operation);
    void endBenchmark(const std::string& operation);
    std::vector<BenchmarkResult> getBenchmarkResults() const;
    void generatePerformanceReport() const;
};
```

### Optimization Guidelines

#### 1. Memory Optimization

- Use smart pointers for memory management
- Implement object pooling for frequently allocated objects
- Use move semantics to reduce copy operations

#### 2. CPU Optimization

- Minimize function calls in tight loops
- Use local variables instead of member variables when possible
- Optimize data structures for cache locality

#### 3. I/O Optimization

- Buffer I/O operations
- Use asynchronous I/O for large data transfers
- Implement compression for data storage

## Development Guidelines

### Coding Standards

#### 1. Code Style

- **Naming Conventions**: Use camelCase for variables and functions, PascalCase for classes
- **Indentation**: Use 4 spaces for indentation
- **Line Length**: Maximum 100 characters per line
- **Comments**: Use Doxygen-style comments for documentation

#### 2. Error Handling

- **Exception Safety**: Ensure exception safety in all functions
- **Error Recovery**: Implement graceful error recovery
- **Error Reporting**: Provide detailed error messages with context

#### 3. Testing

- **Unit Testing**: Write comprehensive unit tests for all functions
- **Integration Testing**: Test integration between components
- **Regression Testing**: Implement regression testing for existing functionality

### Development Workflow

#### 1. Code Development

```bash
# Clone the repository
git clone <repository-url>
cd nlm-project

# Create a new feature branch
git checkout -b feature/enhanced-configuration

# Make changes
# ... develop code ...

# Run tests
mkdir build && cd build
cmake ..
make
ctest

# Commit changes
git add src/
git commit -m "feat: Add enhanced configuration system"

# Push to remote
git push origin feature/enhanced-configuration

# Create pull request
# ... create PR ...
```

#### 2. Testing and Validation

```bash
# Run unit tests
./build/tests/test_enhanced_config

# Run integration tests
./build/tests/test_enhanced_application

# Run performance benchmarks
./build/benchmarks/performance_benchmark

# Generate test reports
./build/scripts/generate_test_reports.py
```

### Contribution Guidelines

#### 1. Code Quality

- **Code Reviews**: All pull requests require at least one code review
- **Testing**: New code must include comprehensive tests
- **Documentation**: All new code must include documentation

#### 2. Feature Development

- **Feature Flags**: Use feature flags for experimental features
- **Backward Compatibility**: Maintain backward compatibility with existing API
- **Performance**: Ensure new features do not degrade performance

#### 3. Documentation

- **API Documentation**: Document all public APIs using Doxygen
- **User Guide**: Update user guide with new features
- **Examples**: Provide examples for new features

## Troubleshooting

### Common Issues

#### 1. Configuration File Not Found

```bash
# Error: Configuration file not found
# Solution: Ensure the configuration file exists
# Example: ./nlm --config configs/my_config.cfg
```

#### 2. Argument Parsing Errors

```bash
# Error: Unknown option: --unknown-option
# Solution: Use --help to see available options
# Example: ./nlm --help
```

#### 3. Configuration Validation Errors

```bash
# Error: Configuration validation failed
# Solution: Check configuration file format and values
# Example: ./nlm --validate-config --config my_config.cfg
```

### Debugging

#### 1. Enable Debug Logging

```bash
# Enable debug logging
./nlm --log-level debug --log-file debug.log --experiment basic
```

#### 2. Run with Validation

```bash
# Run with configuration validation
./nlm --validate --experiment phase2
```

#### 3. Check Application Status

```bash
# Check application status
./nlm --version
```

## API Reference

### Core Classes

#### EnhancedConfig
```cpp
class EnhancedConfig {
public:
    // Configuration loading and saving
    bool load(const std::string& filepath, 
              const std::string& configFormat = "auto",
              bool inherit = true,
              bool validate = true,
              bool overrideWithEnv = true);
    
    bool save(const std::string& filepath, 
              const std::string& outputFormat = "simple") const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration entry management
    std::vector<ConfigEntry> getEntries() const;
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    void setEntry(const std::string& key, const ConfigValue& value,
                  const std::string& description = "",
                  ConfigSource source = ConfigSource::Runtime);
    
    // Configuration operations
    bool mergeWith(const EnhancedConfig& other, MergeStrategy strategy = MergeStrategy::Override);
    std::vector<std::string> compareWith(const EnhancedConfig& other) const;
    std::string getSummary() const;
};
```

#### AdvancedArgumentParser
```cpp
class AdvancedArgumentParser {
public:
    enum OptionType { FLAG, VALUE, POSITIONAL };
    
    void addOption(const std::string& longName, char shortName = '\0',
                  const std::string& description = "",
                  OptionType type = OptionType::VALUE,
                  bool required = false,
                  const std::string& defaultValue = "",
                  std::function<bool(const std::string&)> validator = nullptr);
    
    void addFlag(const std::string& longName, char shortName = '\0',
                const std::string& description = "",
                bool defaultValue = false);
    
    void addPositional(const std::string& name,
                      const std::string& description = "",
                      bool required = false,
                      std::function<bool(const std::string&)> validator = nullptr);
    
    bool parse(int argc, char** argv);
    std::string getOptionValue(const std::string& optionName) const;
    bool getFlagValue(const std::string& optionName) const;
    std::string getPositionalValue(size_t index) const;
};
```

### Global Functions

```cpp
// Enhanced configuration functions
EnhancedConfig& getGlobalEnhancedConfig();
AdvancedArgumentParser& getGlobalEnhancedArgumentParser();

// Configuration file operations
bool loadEnhancedConfigFile(const std::string& filepath,
                          const std::string& format = "auto",
                          bool inherit = true,
                          bool validate = true,
                          bool overrideWithEnv = true);
bool saveEnhancedConfigFile(const std::string& filepath,
                          const std::string& format = "simple");

// Enhanced application management
void initializeEnhancedApplication(int argc, char** argv);
bool isEnhancedApplicationInitialized();
void runEnhancedApplication();
void shutdownEnhancedApplication();
```

## Contributing

### How to Contribute

1. **Fork the Repository**: Fork the NLM repository on GitHub
2. **Create a Feature Branch**: Create a new branch for your feature
3. **Implement the Feature**: Develop your feature or bug fix
4. **Add Tests**: Add comprehensive tests for your changes
5. **Update Documentation**: Update documentation with your changes
6. **Submit a Pull Request**: Submit a pull request to the main branch

### Code Quality Standards

1. **Code Style**: Follow the project's coding standards
2. **Testing**: Ensure all new code is thoroughly tested
3. **Documentation**: Document all new APIs and functions
4. **Error Handling**: Implement proper error handling
5. **Performance**: Ensure new features do not degrade performance

### Development Environment Setup

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install dependencies
# Ubuntu/Debian
 sudo apt-get install build-essential cmake git

# macOS
brew install cmake git

# Install Python dependencies (for testing)
pip3 install pytest

# Build the project
mkdir build
cd build
cmake ..
make -j4

# Run tests
ctest

# Run examples
./examples/example_program
```

### Getting Help

If you need help with the NLM project, you can:

1. **Join the Community**: Join the NLM Discord server for discussions
2. **File Issues**: File issues on the GitHub repository
3. **Submit Pull Requests**: Submit pull requests for contributions
4. **Contact Maintainers**: Contact the project maintainers for assistance

## License

This project is licensed under the MIT License. See the LICENSE file for more information.

## Acknowledgments

The NLM project acknowledges the contributions of:

- **Open Source Community**: All contributors who have contributed to the project
- **Research Community**: All researchers who have contributed to the development of NLM
- **Users**: All users who have tested and provided feedback on the project

## Version Information

### Current Version
- **Version**: 2.0.0
- **Release Date**: October 17, 2026
- **Phase**: Phase 8 - Final Integration

### Version History

#### Version 2.0.0 (October 17, 2026)
- **Enhanced Configuration System**: Advanced configuration parsing with JSON/YAML support
- **Advanced Command Line Interface**: Comprehensive argument parsing with validation
- **Enhanced Testing**: Comprehensive test coverage with statistical analysis
- **Performance Optimization**: Advanced profiling and optimization capabilities
- **Multi-platform Support**: Cross-platform build system and deployment capabilities

#### Version 1.0.0 (Phase 7)
- **Original Implementation**: Basic NLM functionality
- **Core Components**: Core neural computation and plasticity systems
- **Phase 7 Integration**: Integration of all core systems

#### Version 0.5.0 (Phase 6)
- **Initial Development**: Experimental artificial developmental brain project
- **Phase 6 Foundation**: Foundation for neural system development

This document provides comprehensive documentation for the NLM project, covering all enhancements made to improve the codebase, user experience, and development workflow.

For more information, please visit the NLM project website or contact the project maintainers.
