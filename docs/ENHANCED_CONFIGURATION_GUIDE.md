# Enhanced Configuration and Command Line Interface

## Overview

The NLM project now includes enhanced configuration and command-line capabilities that significantly improve user experience, debugging, and system flexibility. These enhancements build upon Phase 6 integration while maintaining backward compatibility.

## Enhanced Configuration System

### CommandLineParser Class

The `CommandLineParser` class provides comprehensive command-line argument parsing with help text, validation, and mode-specific options.

#### Key Features:

1. **Flexible Argument Parsing**
   - Support for both `--flag` and `--flag=value` syntax
   - Boolean flags, numeric parameters, and string values
   - Automatic help and version display

2. **Phase 6 Specific Options**
   - Agent mode selection (`--agent-mode`)
   - Learning mode configuration (`--learning-mode`)  
   - Neuromodulation control (`--neuromodulation-mode`)
   - Memory system selection (`--memory-mode`)
   - Curiosity and novelty thresholds (`--curiosity-threshold`, `--novelty-threshold`)
   - Plasticity rate adjustment (`--plasticity-rate`)
   - Development profile selection (`--development-profile`)

3. **Enhanced Defaults**
   - Comprehensive default configuration for Phase 6
   - Improved validation and error handling
   - Detailed logging and reporting

### EnhancedConfig Class

The `EnhancedConfig` class extends the basic `Config` class with Phase 6-specific enhancements:

#### Key Features:

1. **Automatic Validation**
   - Configuration validation based on current mode
   - Detailed error reporting with specific validation failures
   - Automatic fallback to defaults for invalid configurations

2. **Application to Brain Systems**
   - Automatic application to `Brain` instances
   - Configuration for `AgentBrain` instances with feature toggles
   - Support for `SimpleWorld` configuration

3. **Comprehensive Reporting**
   - Detailed configuration summaries
   - Color-coded validation results
   - Mode-specific configuration guidance

### PluginManager Class

The `PluginManager` provides extensibility for NLM:

#### Key Features:

1. **Dynamic Plugin Loading**
   - Load plugins from specified directories
   - Plugin dependency management
   - Enable/disable plugins at runtime

2. **Plugin Information System**
   - Plugin metadata (name, version, description)
   - Dependency tracking
   - Mode-specific plugin availability

3. **Plugin Execution**
   - Function call support for plugins
   - Result passing and error handling
   - Safe plugin lifecycle management

### AdvancedExperimentRunner Class

The `AdvancedExperimentRunner` provides enhanced experiment control:

#### Key Features:

1. **Flexible Experiment Configuration**
   - Multi-phase experiment support
   - Custom configuration for each phase
   - Continuous and discrete experiment modes

2. **Advanced Metrics and Reporting**
   - Comprehensive experiment statistics
   - Performance metrics collection
   - Configurable output formats (JSON, CSV, text)

3. **Monitoring and Profiling**
   - System monitoring capabilities
   - Performance profiling support
   - Real-time monitoring integration

### MonitoringSystem Class

The `MonitoringSystem` provides real-time monitoring and metrics collection:

#### Key Features:

1. **Metrics Collection**
   - Time-series metrics storage
   - Metric tagging for filtering
   - Thread-safe concurrent access

2. **System Health Monitoring**
   - CPU and memory usage tracking
   - Neural system statistics
   - Active neuron and synapse counts

3. **Integration Support**
   - Brain and world system monitoring
   - Custom metric definitions
   - Alert system support

## Command Line Interface Usage

### Basic Commands

```bash
# Show help information
./nlm --help

# Show version information
./nlm --version

# Run basic neural computation test
./nlm basic

# Run learning experiment
./nlm learning

# Run Phase 6 integration test
./nlm phase6

# Run demo with world interaction
./nlm demo

# List available plugins
./nlm --list-plugins
```

### Configuration Options

```bash
# Specify configuration file
./nlm --config myconfig.cfg basic

# Override specific settings
./nlm --neuron_count 1000 --connection_probability 0.05 learning

# Enable debugging
./nlm --debug --verbose phase6

# Set simulation speed
./nlm --sim-speed 2.0 --max-steps 5000 phase6
```

### Phase 6 Specific Options

```bash
# Configure agent behavior
./nlm --agent-mode advanced --learning-mode episodic phase6

# Configure neuromodulation
./nlm --neuromodulation-mode dopamine --curiosity-threshold 0.5 phase6

# Configure memory systems
./nlm --memory-mode episodic --enable-replay phase6

# Configure development
./nlm --development-profile aggressive --enable-development phase6
```

### Monitoring Options

```bash
# Enable system monitoring
./nlm --enable-monitoring --monitoring-port 8080 phase6

# Enable GPU acceleration
./nlm --enable-gpu phase6

# Set plugin directory
./nlm --plugin-path ./myplugins phase6
```

## Configuration File Format

The enhanced configuration system supports JSON format with improved defaults:

### Example Configuration File

```json
{
    "nlm_version": "6.0",
    "simulation": {
        "timestep": 0.001,
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.1
    },
    "learning": {
        "agent_mode": "advanced",
        "learning_mode": "episodic",
        "memory_mode": "working",
        "neuromodulation_mode": "dopamine"
    },
    "neuromodulation": {
        "curiosity_threshold": 0.3,
        "novelty_threshold": 0.5,
        "plasticity_rate": 0.01
    },
    "memory": {
        "working_memory_size": 1000,
        "episodic_memory_capacity": 100,
        "semantic_memory_capacity": 1000
    },
    "development": {
        "enabled": true,
        "profile": "standard"
    },
    "experiment": {
        "max_steps": 2000,
        "enable_checkpointing": true,
        "enable_replay": true,
        "enable_development": true
    }
}
```

## Enhanced API Documentation

### Core Classes and Methods

#### CommandLineParser

**Static Methods:**
- `parse(int argc, char** argv)` - Parse command-line arguments
- `printHelp(const std::string& programName)` - Display help information
- `printVersion()` - Display version information
- `validateOptions(const Options& options)` - Validate parsed options

**Options Structure:**
- `configFile` - Configuration file path
- `verbose`, `debug`, `quiet` - Logging control
- `experimentModes` - List of experiment modes to run
- `simulationModes` - List of simulation modes
- `simulationSpeed` - Simulation speed factor
- `maxSteps` - Maximum simulation steps
- `enableGPU` - Enable GPU acceleration
- `enableMonitoring` - Enable monitoring system
- Various NLM-specific options for agent configuration

#### EnhancedConfig

**Public Methods:**
- `EnhancedConfig()` - Constructor with Phase 6 defaults
- `loadWithValidation(const std::string& filepath)` - Load and validate configuration
- `applyToBrain(std::shared_ptr<Brain> brain)` - Apply configuration to brain
- `applyToAgentBrain(std::shared_ptr<AgentBrain> agentBrain)` - Apply configuration to agent
- `applyToWorld(std::shared_ptr<SimpleWorld> world)` - Apply configuration to world
- `getConfigurationReport()` - Get detailed configuration report
- `validateConfiguration(const std::string& mode)` - Validate configuration for specific mode

#### PluginManager

**Public Methods:**
- `PluginManager()` - Constructor
- `loadPluginsFromDirectory(const std::string& directory)` - Load plugins from directory
- `getPlugin(const std::string& name)` - Get plugin by name
- `enablePlugin(const std::string& name, bool enable)` - Enable/disable plugin
- `listPlugins()` - List all available plugins
- `getPluginsForMode(const std::string& mode)` - Get plugins suitable for specific mode

#### AdvancedExperimentRunner

**Public Methods:**
- `AdvancedExperimentRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)` - Constructor
- `runExperiment(const ExperimentOptions& options)` - Run experiment with advanced options
- `getExperimentStats()` - Get experiment statistics
- `saveExperimentData(const std::string& filepath, const ExperimentStats& stats)` - Save experiment data
- `loadExperimentConfig(const std::string& filepath)` - Load experiment configuration

#### MonitoringSystem

**Public Methods:**
- `MonitoringSystem()` - Constructor
- `recordMetric(const std::string& name, double value, const std::unordered_map<std::string, std::string>& tags)` - Record metric
- `getMetrics(const std::string& name, double startTime, double endTime)` - Get metrics for time range
- `getLatestMetric(const std::string& name)` - Get latest metric value
- `startMonitoring(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)` - Start monitoring
- `stopMonitoring()` - Stop monitoring
- `getHealthStatus()` - Get system health status

### Utility Functions

#### Utils Namespace

**Functions:**
- `createConfiguration(int argc, char** argv)` - Create configuration from defaults and command line
- `createAgentBrain(std::shared_ptr<Brain> brain, const std::string& mode)` - Create agent brain with optimal settings
- `createWorld(const std::string& mode)` - Create world with specific settings
- `runPhase1Tests(std::shared_ptr<Brain> brain)` - Run Phase 1 tests
- `runPhase2Tests(std::shared_ptr<Brain> brain)` - Run Phase 2 tests
- `runPhase3Tests(std::shared_ptr<Brain> brain)` - Run Phase 3 tests
- `runPhase4Tests(std::shared_ptr<Brain> brain)` - Run Phase 4 tests
- `runPhase6Tests(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)` - Run Phase 6 tests
- `saveBrainWithMetadata(std::shared_ptr<Brain> brain, const std::string& filepath, const std::string& metadata)` - Save brain state with metadata
- `loadBrainWithValidation(const std::string& filepath)` - Load brain state with validation
- `generateConfigTemplate(const std::string& outputFile)` - Generate configuration template
- `validateConfigFile(const std::string& filepath)` - Validate configuration file
- `getSystemInfo()` - Get system information

## Integration with Existing Systems

### Configuration File Structure

The enhanced configuration system integrates seamlessly with the existing NLM configuration system:

1. **Backward Compatibility**
   - All existing configuration keys continue to work
   - New keys are added as needed for Phase 6 features
   - Graceful fallback to defaults for unknown keys

2. **Enhanced Defaults**
   - Phase 6-specific defaults are applied automatically
   - Configuration validation ensures sensible defaults
   - Mode-specific configuration guidance provided

3. **Application to Components**
   - Configuration applied to brain components
   - AgentBrain configuration with feature toggles
   - World system configuration support
   - Experiment configuration

### Command Line Integration

The command-line interface integrates with all existing NLM executables:

1. **Existing Executables**
   - `nlm` - Main neural computation executable
   - `nlm_phase3_demo` - Phase 3 demonstration
   - `nlm_phase4_demo` - Phase 4 demonstration
   - `nlm_phase6_demo` - Phase 6 integration demonstration

2. **Enhanced Features**
   - Command-line argument parsing for all executables
   - Configuration file support
   - Debugging and verbose modes
   - Plugin system support
   - Monitoring integration

## Performance and Scalability

### Optimized Configuration Loading

The enhanced configuration system includes performance optimizations:

1. **Lazy Loading**
   - Configuration values loaded on demand
   - Caching of frequently accessed values
   - Efficient parsing of configuration files

2. **Memory Efficiency**
   - Minimal memory overhead for configuration storage
   - Efficient string handling
   - Support for configuration templates

3. **Validation Optimization**
   - Early validation failures
   - Mode-specific validation
   - Performance-focused validation logic

### Monitoring and Debugging

The enhanced monitoring system provides comprehensive debugging capabilities:

1. **Real-time Monitoring**
   - System resource tracking
   - Neural activity monitoring
   - Performance profiling

2. **Debugging Support**
   - Detailed logging
   - Configuration validation reports
   - System health monitoring

3. **Performance Analysis**
   - Time-series metrics collection
   - Experiment statistics
   - Benchmark integration

## Future Enhancements

### Planned Features

1. **Plugin Architecture**
   - Dynamic plugin loading
   - Plugin marketplace
   - Community plugin support

2. **Advanced Configuration**
   - Configuration templates
   - Configuration inheritance
   - Configuration validation schemas

3. **Enhanced Monitoring**
   - Visual monitoring dashboards
   - Alert system integration
   - Performance benchmarking

4. **Experimental Features**
   - Dynamic configuration generation
   - Reinforcement learning integration
   - Advanced visualization

## Migration Guide

### Upgrading from Previous Versions

1. **Configuration File**
   - Old format automatically detected
   - Graceful migration to new format
   - Backward compatibility maintained

2. **Command Line Interface**
   - Existing commands continue to work
   - New options added for Phase 6 features
   - Enhanced help documentation

3. **Plugin System**
   - New plugin architecture
   - Plugin lifecycle management
   - Plugin dependency system

## Conclusion

The enhanced configuration and command-line interface system represents a significant improvement to the NLM project. It provides:

1. **Improved User Experience**
   - Comprehensive command-line interface
   - Flexible configuration management
   - Detailed help and documentation

2. **Enhanced Debugging**
   - Configuration validation
   - Detailed logging
   - System monitoring

3. **Performance Optimizations**
   - Efficient configuration loading
   - Memory-efficient storage
   - Validation optimizations

4. **Future Extensibility**
   - Plugin architecture
   - Configuration templates
   - Advanced monitoring

This system enables researchers and developers to more easily configure, run, and debug NLM experiments while maintaining full backward compatibility with existing code.
