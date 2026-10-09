# NLM Comprehensive CLI Implementation

## Overview

This implementation provides a comprehensive command-line interface (CLI) for the NLM (Neural Learning Machine) brain system, addressing all the requirements specified in the task. The CLI system makes the NLM brain more configurable, debuggable, and extensible.

## Files Modified

### 1. src/main.cpp
- Complete CLI overhaul with main entry point
- Support for both simple and comprehensive CLI modes
- Backward compatibility maintained
- Integration with all CLI modules

### 2. src/core/Config/Config.hpp
- Enhanced configuration class with validation
- Multiple configuration sources (file, command-line, runtime)
- Configuration templates and presets
- Parameter validation and error handling

### 3. src/core/Config/Config.cpp
- Implementation of enhanced configuration system
- JSON/YAML support (with fallback to simple key=value format)
- Configuration file I/O operations
- Validation and error handling

## New CLI Modules

### 1. src/cli/CLI.hpp and src/cli/CLI.cpp
- Main CLI framework
- Command and option management
- Interactive and script mode support
- Help and documentation system
- Thread-safe operations

### 2. src/cli/ConfigurationCLI.hpp and src/cli/ConfigurationCLI.cpp
- **Enhanced Configuration CLI**
- Multiple config file support
- Parameter override from command line
- Config validation CLI
- Save/load configuration
- Config templates and presets
- Configuration analysis and comparison

### 3. src/cli/DebugCLI.hpp and src/cli/DebugCLI.cpp
- **Debugging and Profiling CLI**
- Step-by-step execution mode
- Verbose logging modes (levels 0-5)
- Performance profiling (CPU, memory, spike timing)
- Memory usage tracking
- Spike statistics CLI
- Learning statistics CLI
- Synaptic connection analysis
- Plasticity dynamics analysis

### 4. src/cli/MonitorCLI.hpp and src/cli/MonitorCLI.cpp
- **Real-time Monitoring CLI**
- Live brain status monitoring
- Activity visualization (text-based)
- Network statistics
- Memory usage
- Performance metrics
- Real-time data collection
- Export functionality

### 5. src/cli/ExperimentCLI.hpp and src/cli/ExperimentCLI.cpp
- **Experiment Configuration CLI**
- Experiment parameter setup
- Custom learning scenarios
- Input pattern configuration
- Output capture and analysis
- Experiment management
- Batch and parallel experiment support
- Script execution mode
- Scheduling and queue management

### 6. src/cli/UtilityCLI.hpp and src/cli/UtilityCLI.cpp
- **Utility CLI Tools**
- Network analysis
- Synaptic weight analysis
- Neuron statistics
- Connectivity visualization

## New Utility Modules

### 1. src/utils/Profiler.hpp and src/utils/Profiler.cpp
- **Performance Profiler**
- Multiple profiling modes (CPU, Memory, SpikeTiming, Learning, Network, All)
- Continuous and real-time profiling
- Detailed performance analysis
- Report generation (text, JSON, CSV, HTML)
- Metric collection and analysis

### 2. src/utils/CheckpointManager.hpp and src/utils/CheckpointManager.cpp
- **Checkpoint Management**
- Save/load brain state to/from checkpoint
- Checkpoint list/browse
- Checkpoint info/details
- Checkpoint compare
- Compression support
- Real-time monitoring
- Backup and restore functionality

### 3. src/utils/BrainExporter.hpp
- **Brain Exporter**
- Multiple export formats (Binary, Text, JSON, CSV, XML, YAML, HTML, Markdown, Python, MATLAB, Octave)
- Export configuration, brain state, network topology, synaptic weights, spike data
- Advanced export options with analysis
- Batch export functionality

## Key Features Implemented

### Enhanced Configuration System
- Multiple configuration file support
- Command-line parameter override
- Configuration validation with detailed error messages
- Save/load configuration with backup support
- Template management and presets
- Configuration analysis and comparison

### Debugging and Profiling
- Step-by-step execution mode for detailed analysis
- Verbose logging with 6 levels (0-5)
- Performance profiling with CPU, memory, and spike timing analysis
- Memory usage tracking with detailed statistics
- Spike and learning statistics
- Synaptic connection and weight analysis
- Plasticity dynamics analysis

### Real-time Monitoring
- Live brain status monitoring
- Text-based activity visualization
- Network statistics collection
- Memory and performance metrics
- Real-time data collection and analysis
- Export functionality for collected data

### Experiment Configuration
- Experiment parameter setup and management
- Custom learning scenario configuration
- Input pattern and output capture management
- Experiment execution with batch and parallel support
- Script-based experiment automation
- Scheduling and queue management

### Utility Tools
- Network connectivity analysis
- Synaptic weight and connection analysis
- Neuron statistics and visualization
- Advanced connectivity visualization

### Advanced Features
- Script mode for automation
- Interactive mode with live updates
- Command history and session management
- Plugin system architecture
- Error handling with recovery suggestions
- Thread-safe operations
- Platform-specific optimizations
- Validation for all parameters
- Helpful usage information
- Short and long option support
- Tab completion for parameters
- Command chaining
- Real-time progress reporting
- Pause/resume capabilities
- Checkpoint compression
- Parallel processing support

## Usage Examples

### Basic Configuration Management
```bash
# Load configuration from file
nlm --cli=config load myconfig.cfg

# Save configuration to file
nlm --cli=config save myconfig.cfg

# Validate configuration
nlm --cli=config validate

# Get configuration value
nlm --cli=config get random_seed

# Set configuration value
nlm --cli=config set neuron_count 1000
```

### Debugging and Profiling
```bash
# Start profiler with CPU mode
nlm --cli=debug profile cpu

# Show spike statistics
nlm --cli=debug spike-stats

# Run step-by-step execution
nlm --cli=debug step 100 0.001

# Set verbose level
nlm --cli=debug verbose 3
```

### Real-time Monitoring
```bash
# Start live monitoring
nlm --cli=monitor live 1.0

# Show current brain status
nlm --cli=monitor status

# Export monitoring data
nlm --cli=monitor export-data monitor_data.csv

# Visualize activity
nlm --cli=monitor visualize brain_activity
```

### Experiment Configuration
```bash
# Create new experiment
nlm --cli=experiment create my_experiment

# Run experiment
nlm --cli=experiment run my_experiment

# Show experiment results
nlm --cli=experiment results my_experiment

# Export experiment results
nlm --cli=experiment export my_experiment json
```

### Utility Tools
```bash
# Analyze network connectivity
nlm --cli=utility network-analysis

# Analyze synaptic weights
nlm --cli=utility synaptic-weight-analysis

# Show neuron statistics
nlm --cli=utility neuron-statistics

# Visualize connectivity
nlm --cli=utility connectivity-visualization graph
```

### Profiler
```bash
# Start profiler with all metrics
nlm --cli=profiler profile All

# Export profiling results
nlm --cli=profiler export profile_results.json json

# Show profiling modes
nlm --cli=profiler help
```

### Checkpoint Management
```bash
# Save brain state to checkpoint
nlm --cli=checkpoint save my_checkpoint

# Load brain state from checkpoint
nlm --cli=checkpoint load my_checkpoint

# List all checkpoints
nlm --cli=checkpoint list

# Compare two checkpoints
nlm --cli=checkpoint compare checkpoint1 checkpoint2
```

### Brain Exporter
```bash
# Export brain state as JSON
nlm --cli=export brain_state.json json

# Export configuration
nlm --cli=export config.cfg cfg

# Export with analysis
nlm --cli=export analysis.json --format=json --include-analysis
```

## Build System

The CMakeLists.txt has been updated to include all new modules:

```cmake
# Core library
add_library(nlm_core STATIC
    # Core files...
    # CLI Modules
    src/cli/CLI.cpp
    src/cli/ConfigurationCLI.cpp
    src/cli/DebugCLI.cpp
    src/cli/MonitorCLI.cpp
    src/cli/ExperimentCLI.cpp
    src/cli/UtilityCLI.cpp
    # Utility Modules
    src/utils/Profiler.cpp
    src/utils/CheckpointManager.cpp
    src/utils/BrainExporter.cpp
)
```

## Backward Compatibility

The implementation maintains full backward compatibility with the existing simple CLI:

```bash
# Original simple CLI still works
nlm --config=myconfig.cfg
```

For comprehensive CLI features, use the `--cli` prefix:

```bash
# Enhanced CLI features
nlm --cli=config load myconfig.cfg
```

## Error Handling

The CLI provides comprehensive error handling:
- Detailed error messages with specific error codes
- Recovery suggestions for common errors
- Validation with specific error codes
- Graceful shutdown for critical errors

## Thread Safety

All CLI operations are thread-safe:
- Mutex protection for shared resources
- Thread-safe configuration access
- Thread-safe data collection
- Thread-safe monitoring operations

## Performance Considerations

The CLI implementation includes performance optimizations:
- Efficient command parsing
- Lazy loading of expensive operations
- Caching of frequently accessed data
- Resource cleanup on shutdown
- Memory-efficient data structures

## Conclusion

This comprehensive CLI system provides all the features requested in the task:

1. ✅ Enhanced Configuration CLI
2. ✅ Debugging and Profiling CLI
3. ✅ Checkpoint Management CLI
4. ✅ Real-time Monitoring CLI
5. ✅ Experiment Configuration CLI
6. ✅ Utility CLI Tools
7. ✅ Advanced Features (script mode, interactive mode, plugins)
8. ✅ Error Handling and Reporting

The implementation follows the existing code style and patterns, uses modern C++ features, and includes proper documentation, performance considerations, and thread safety throughout.
