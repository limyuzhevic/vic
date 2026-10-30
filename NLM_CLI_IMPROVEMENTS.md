# NLM Phase 6: Advanced Command Line Interface Improvements

## Overview

This document describes the comprehensive improvements made to the NLM (Neural Learning Machine) Phase 6: Final Integration command-line interface (CLI) to provide advanced functionality for power users and researchers.

## Table of Contents

1. Introduction
2. Configuration System Improvements
3. CLI Command Enhancements
4. Advanced Features
5. Integration with Existing Codebase
6. Usage Examples
7. Performance Considerations
8. Future Enhancements

## 1. Introduction

The NLM project has undergone significant improvements in Phase 6, focusing on integrating all existing systems into a coherent artificial brain. This document describes the CLI enhancements that make this advanced system more accessible and powerful for researchers and developers.

## 2. Configuration System Improvements

### 2.1 Enhanced Configuration Class

The `Config` class in `src/core/Config/Config.hpp` and `src/core/Config/Config.cpp` has been significantly enhanced:

#### 2.1.1 JSON Support
- **Automatic Format Detection**: The configuration loader now automatically detects whether a configuration file is in JSON format or the legacy key=value format
- **Full JSON Type Support**: Supports all JSON types including arrays, objects, and nested structures
- **Backward Compatibility**: Maintains support for the original simple key=value format for legacy configurations

#### 2.1.2 Improved File I/O
- **Smart Value Serialization**: The `saveToFile()` method now properly serializes complex configuration values including vectors and nested structures
- **Enhanced Error Handling**: Better error handling and recovery mechanisms for file operations
- **Configuration Validation**: Automatic validation of critical configuration parameters

#### 2.1.3 Advanced Features
- **Multiple Source Support**: Configuration can be loaded from files, command-line arguments, and runtime settings
- **Type Safety**: Strong typing with proper type checking and error handling
- **Extensible Architecture**: Easy to add new configuration types and validation rules

### 2.2 Configuration File Format

#### 2.2.1 JSON Format (Recommended)

The new configuration system supports JSON format, which provides better structure and type safety:

```json
{
  "version": "Phase 6.0",
  "description": "Complete NLM Phase 6 configuration",
  
  # Core Configuration
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 5000,
  "region_count": 4,
  
  # Neural Dynamics
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "plasticity_learning_rate": 0.01,
  
  # Development Systems
  "development_critical_period_start": 0.5,
  "development_synaptogenesis_rate": 0.001,
  
  # Neuromodulation
  "dopamine_baseline": 0.05,
  "curiosity_strength": 0.2,
  "novelty_sensitivity": 0.15,
  
  # Memory Systems
  "working_memory_capacity": 500,
  "episodic_memory_max_episodes": 10000,
  
  # Cognition Systems
  "planning_depth": 5,
  "attention_inhibition_strength": 0.5,
  
  # Environment
  "environment_name": "GridWorld",
  "environment_width": 20,
  "environment_height": 20,
  
  # Simulation Control
  "max_simulation_steps": 100000,
  "checkpoint_interval": 1000,
  
  # Feature Flags
  "enable_structural_plasticity": true,
  "enable_development": true,
  "enable_checkpointing": true
}
```

#### 2.2.2 Legacy Key-Value Format

For backward compatibility, the system still supports the original key=value format:

```
# NLM Configuration File
# Phase 6: Final Integration Configuration

random_seed = 42
simulation_timestep = 0.001
neuron_count = 5000
region_count = 4
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
environment_width = 20
environment_height = 20
log_level = INFO
max_simulation_steps = 100000
enable_development = true
enable_checkpointing = true
```

### 2.3 Default Configuration

The `configs/default.cfg` file has been completely updated to Phase 6 standards, including:

- **Phase 6 Scale**: Increased neuron count (5000 neurons, 4 regions)
- **Advanced Settings**: Added developmental stages, neuromodulation parameters
- **Memory Systems**: Configured working memory (500 capacity), episodic memory (10,000 episodes)
- **Cognition Systems**: Set planning depth, attention parameters, concept formation
- **Development**: Added critical period and development-specific rates
- **Neuromodulation**: Configured dopamine, curiosity, and novelty systems
- **Performance**: Added SIMD vectorization, memory pools, and event queues
- **Checkpointing**: Configured checkpoint intervals and compression

## 3. CLI Command Enhancements

### 3.1 Available Commands

The NLM CLI now provides comprehensive command-line interface functionality:

#### 3.1.1 Basic Commands

```bash
# Show help information
nlm --help
nlm -h

# Show version information
nlm --version
nlm -v

# List all configuration options
nlm --list-configs
nlm -l
```

#### 3.1.2 Configuration Commands

```bash
# Load configuration from file
nlm --config my_config.json
nlm --config=FILE

# Generate complete configuration
nlm --generate-config complete_config.json

# Validate configuration file
nlm --validate-config my_config.json

# Save current configuration to file
nlm --save-config backup.json

# Get configuration value
nlm --get neuron_count

# Set configuration key-value pair
nlm --set neuron_count=10000
nlm --set random_seed=123
```

#### 3.1.3 Simulation Commands

```bash
# Run with custom simulation mode
nlm --run-mode quick
nlm --run-mode thorough
nlm --run-mode benchmark

# Set number of simulation steps
nlm --steps 5000

# Set simulation duration
nlm --duration 60.0

# Dry run (show configuration without running)
nlm --dry-run

# Run comprehensive test suite
nlm --test-all
```

#### 3.1.4 Advanced Commands

```bash
# Export brain state
nlm --export-brain current_state.bin --steps 10000

# Import brain state
nlm --import-brain saved_state.bin

# Show checkpoint information
nlm --checkpoint-info path/to/checkpoint

# Run performance benchmarks
nlm --benchmark 5

# Enable performance profiling
nlm --profile

# Enable debug logging
nlm --debug
```

### 3.2 Command-Line Argument Parsing

The enhanced argument parser supports:

- **Long format**: `--option value`
- **Short format**: `-o value`
- **Equal format**: `--option=value`
- **Multiple values**: `--option1 value1 --option2 value2`
- **Flag options**: `--enable-feature`

### 3.3 Configuration Override System

Command-line arguments can override configuration file settings:

```bash
# Override multiple settings
nlm --config base_config.json \
    --neuron_count=10000 \
    --environment_width=50 \
    --environment_height=50 \
    --enable_development=true
```

## 4. Advanced Features

### 4.1 Performance Monitoring

The CLI includes comprehensive performance monitoring capabilities:

- **Timing Information**: Detailed timing for all operations
- **Memory Usage**: Real-time memory usage monitoring
- **Performance Benchmarks**: Comparative performance testing
- **Resource Utilization**: CPU and memory usage tracking

### 4.2 Advanced Configuration Management

- **Configuration Diffing**: Compare two configuration files
- **Configuration Merging**: Merge multiple configuration files
- **Configuration Archiving**: Archive configuration versions
- **Configuration Rollback**: Roll back to previous configurations

### 4.3 Development and Debugging

- **Build Analysis**: Analyze build configuration and dependencies
- **Dependency Checking**: Verify all required dependencies are available
- **System Information**: Display system capabilities and limitations
- **Debug Mode**: Enhanced debugging output and error reporting

### 4.4 Script Integration

The CLI supports scripting and automation:

- **Batch Processing**: Process multiple configuration files
- **Pipeline Integration**: Integrate with other tools via pipes
- **Logging**: Comprehensive logging for automated systems
- **Error Handling**: Graceful error handling for script integration

## 5. Integration with Existing Codebase

### 5.1 Main Executable Enhancements

The `src/main.cpp` file has been completely rewritten to leverage the new configuration system:

- **Modern C++ Features**: Uses C++17 features for better performance
- **Configuration Validation**: Automatic validation of critical parameters
- **Error Handling**: Comprehensive error handling and recovery
- **Phase 6 Integration**: Full integration test suite

### 5.2 Backward Compatibility

All improvements maintain backward compatibility:

- **Legacy Configuration Support**: Original key=value format still works
- **Legacy CLI Interface**: Original command-line interface preserved
- **Existing Scripts**: Scripts using the old interface continue to work
- **API Compatibility**: All public APIs remain unchanged

### 5.3 New Integration Features

- **Phase 6 Integration Test**: Comprehensive test of all integrated systems
- **Checkpoint Testing**: Verify checkpoint save/load functionality
- **Memory System Testing**: Validate all memory systems integration
- **Neuromodulation Testing**: Verify neuromodulation system functionality

## 6. Usage Examples

### 6.1 Basic Usage

```bash
# Run with default configuration (Phase 6 optimized)
nlm

# View all available configuration options
nlm --list-configs

# Show version information
nlm --version
```

### 6.2 Configuration Management

```bash
# Generate a complete configuration file
nlm --generate-config my_complete_config.json

# Validate a configuration file
nlm --validate-config my_config.json

# Create a backup of current configuration
nlm --save-config backup_config.json

# Compare two configuration files
nlm --compare-configs default.json optimized.json
```

### 6.3 Advanced Usage

```bash
# Run NLM with custom configuration and settings
nlm --config my_config.json \
    --neuron_count=15000 \
    --environment_width=50 \
    --environment_height=50 \
    --enable_development=true \
    --enable_checkpointing=true \
    --checkpoint_path=./my_checkpoints

# Run performance benchmark
nlm --benchmark 3 --steps 20000

# Run quick test for development
nlm --run-mode quick --steps 1000

# Export brain state for research
nlm --export-brain research_brain.bin --steps 50000

# Import and resume from saved state
nlm --import-brain research_brain.bin
```

### 6.4 Scripting and Automation

```bash
#!/bin/bash
# NLM Automation Script

# Generate configuration
./nlm --generate-config config_$(date +%Y%m%d).json

# Validate configuration
./nlm --validate-config config_$(date +%Y%m%d).json

# Run experiment with custom parameters
./nlm --config config_$(date +%Y%m%d).json \
      --steps 10000 \
      --environment_width=30 \
      --environment_height=30

# Backup brain state
cp brain_state.bin brain_state_backup_$(date +%Y%m%d).bin

# Export analysis results
./nlm --export-brain final_state.bin
```

## 7. Performance Considerations

### 7.1 Performance Optimization

The CLI includes several performance optimizations:

- **Lazy Loading**: Configuration values are loaded only when needed
- **Caching**: Frequently used values are cached for faster access
- **Parallel Processing**: Some operations can be executed in parallel
- **Memory Efficiency**: Memory usage is optimized for large configurations

### 7.2 System Requirements

Minimum system requirements for Phase 6:

- **CPU**: Multi-core processor (2+ cores recommended)
- **Memory**: 4GB RAM (8GB+ recommended for large configurations)
- **Storage**: 1GB free space for configuration and checkpoint files
- **Operating System**: Linux, macOS, or Windows 10+

### 7.3 Scalability

The system is designed to scale with configuration complexity:

- **Large Configurations**: Supports up to 100,000+ neurons
- **Memory-Intensive Tasks**: Optimized for large memory allocations
- **Multi-Processor**: Can utilize multiple CPU cores
- **Distributed Computing**: Foundation for distributed computing setups

## 8. Future Enhancements

### 8.1 Planned Features

The following enhancements are planned for future releases:

- **Web Interface**: Web-based configuration and control
- **REST API**: REST API for programmatic access
- **Machine Learning Integration**: Integration with ML frameworks
- **Visualization Tools**: Advanced visualization and analysis tools
- **Cloud Integration**: Cloud-based deployment and management
- **Container Support**: Docker and container orchestration

### 8.2 Research Integration

The CLI is designed to support research needs:

- **Reproducibility**: Enhanced support for reproducible research
- **Experiment Tracking**: Automatic experiment documentation and tracking
- **Data Analysis**: Built-in data analysis and visualization tools
- **Collaboration**: Support for collaborative research projects

## Conclusion

The NLM Phase 6 command-line interface represents a significant advancement in usability and functionality. The enhanced configuration system, comprehensive CLI commands, and advanced features make this experimental artificial developmental brain more accessible and powerful for researchers, developers, and educators.

The improvements maintain full backward compatibility while introducing powerful new capabilities that leverage the full potential of Phase 6's integrated artificial brain system.

For more information and support, visit the NLM documentation and support channels.
