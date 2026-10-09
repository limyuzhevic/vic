# NLM Phase 2 Advanced CLI Implementation

## Overview
This document describes the comprehensive advanced command-line interface (CLI) enhancements for NLM (熙然) Phase 2: Real Neural Computation.

## Implemented Features

### 1. Enhanced Help/Usage System
- **--help, -h**: Displays comprehensive usage information
- **--version, -v**: Shows version information and system details
- **--usage**: Provides detailed usage examples and common workflows
- **Context-sensitive help**: Specific help for individual options

### 2. Simulation Control
- **--steps=N, -s N**: Specify number of simulation steps to run
- **--duration=T, -d T**: Specify simulation duration in seconds
- **--save-interval=N**: Set checkpoint save interval (in steps)
- **--load-checkpoint=F**: Load specific checkpoint file
- **--checkpoint-dir=D**: Specify checkpoint directory

### 3. Output and Logging Control
- **--verbose, -v**: Enable verbose output and debugging information
- **--quiet, -q**: Suppress output for quiet operation
- **--log-level=L**: Set logging level (Debug, Info, Warning, Error, Critical)
- **--log-file=F**: Specify file for log output
- **--output-format=F**: Set output format (JSON, CSV, Human-readable)

### 4. Advanced Configuration Options
- **--export-config=F**: Export current configuration to JSON file
- **--validate-config=F**: Validate configuration file against schema
- **--show-config**: Display current configuration settings
- **--apply-profile=P**: Apply predefined configuration profiles (basic, advanced, production, development)

### 5. Debug and Testing Options
- **--debug**: Enable debug mode with additional diagnostic output
- **--test**: Run validation tests and system checks
- **--benchmark**: Run performance benchmarking
- **--profile**: Enable profiling information collection
- **--replay**: Replay recorded sequences for testing

### 6. Checkpoint Management
- **--create-checkpoint=N**: Create checkpoint with specific name
- **--list-checkpoints**: List all available checkpoints
- **--delete-checkpoint=N**: Delete specific checkpoint
- **--purge-checkpoints**: Delete all old checkpoints

### 7. Neuromodulation and Learning Control
- **--enable-reward-modulation**: Enable reward-based learning
- **--enable-structural-plasticity**: Enable structural plasticity
- **--enable-development**: Enable developmental processes
- **--enable-curiosity**: Enable curiosity-driven exploration
- **--curiosity-intensity=I**: Set curiosity intensity (0.0-1.0)

### 8. Memory and Persistence Options
- **--memory-capacity=C**: Set memory capacity
- **--episodic-memory-size=S**: Set episodic memory size
- **--working-memory-size=W**: Set working memory size
- **--no-memory**: Disable all memory systems

### 9. Development and Stage Control
- **--developmental-stage=S**: Set developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)
- **--critical-period-duration=T**: Set critical period duration
- **--plasticity-rate=R**: Set overall plasticity rate

### 10. Performance and Resource Control
- **--neuron-count=N**: Set neuron count
- **--region-count=R**: Set number of brain regions
- **--connection-probability=P**: Set connection probability
- **--max-memory=M**: Set maximum memory usage
- **--thread-count=T**: Set number of processing threads

### 11. Specialized Modes
- **--sleep-mode**: Enter sleep/rest cycle
- **--replay-mode=F**: Replay specific episode
- **--ablation-mode**: Disable specific brain regions
- **--experiment-mode=M**: Run specific experiment

### 12. Output and Export Options
- **--export-brain-state=F**: Export brain state to file
- **--export-behavior-log=F**: Export behavioral logs to file
- **--export-neural-activity=F**: Export neural activity data to file
- **--export-simulation-data=F**: Export simulation data to file

## Key Implementation Details

### Enhanced Argument Parsing
- Modern C++ parsing with comprehensive type conversion
- Support for both `--key=value` and `-key value` formats
- Automatic argument validation and type checking
- Backward compatibility with existing `--config` option

### Configuration Integration
- Seamless integration with existing Config class
- Command-line overrides of configuration values
- Profile-based configuration application
- Export/import capabilities with JSON format

### Usage Information
- Comprehensive usage documentation
- Contextual help for different option categories
- Example commands for common use cases
- Option categorization for easier navigation

### Error Handling
- Clear error messages with suggested corrections
- Graceful handling of malformed arguments
- Detailed validation feedback
- Robust exception handling

### Help System
- Context-sensitive help generation
- Option categorization
- Example commands
- Interactive help via `--help` flag

## Files Modified

### src/main.cpp
- Replaced simple argument parsing with comprehensive CLI
- Added help/usage generation functions
- Implemented argument validation
- Added configuration integration
- Extended simulation control options
- Added export/import capabilities

### src/core/Config/Config.cpp
- Added export/import methods for configuration
- Added validation methods
- Added profile support
- Enhanced JSON conversion utilities

### src/brain/Brain.hpp
- Added CLI-related methods for configuration control
- Added checkpoint management methods
- Added neuromodulation control methods
- Added memory management methods
- Added specialized mode controls

## Usage Examples

### Basic Simulation
```bash
# Run default simulation (3 built-in tests)
nlm

# Run with custom number of steps
nlm --steps=1000

# Load configuration and run verbose
nlm --config=myconfig.json --verbose --steps=500
```

### Configuration Management
```bash
# Export current configuration
nlm --export-config=exported.json

# Validate configuration file
nlm --validate-config=myconfig.json

# Apply predefined profile
nlm --apply-profile=advanced
```

### Research Experiments
```bash
# Run benchmark with larger network
nlm --benchmark --neuron-count=2000 --region-count=4

# Create and save checkpoints
nlm --steps=10000 --save-interval=500 --create-checkpoint="experiment1"

# Enable curiosity-driven exploration
nlm --enable-curiosity --curiosity-intensity=0.8 --steps=5000
```

### Developmental Studies
```bash
# Run during critical period
nlm --developmental-stage=CriticalPeriod --critical-period-duration=100

# Export brain state
nlm --export-brain-state=brain_state.json --steps=1000
```

## Testing

The implementation includes comprehensive tests for:
- Help and version commands
- Configuration export/import
- Profile application
- Checkpoint management
- Neuromodulation control
- Memory management
- Development options
- Performance control
- Export capabilities
- Debug and testing options

## Benefits

1. **Backward Compatibility**: Maintains existing `--config` option
2. **User-Friendly**: Professional, user-friendly interface for all skill levels
3. **Flexible**: Supports various use cases from beginners to advanced users
4. **Extensible**: Modular design for easy future enhancements
5. **Well-Documented**: Comprehensive help and usage information
6. **Robust**: Comprehensive error handling and validation
7. **Testable**: Extensive test coverage for all features

## Conclusion

The advanced CLI transforms NLM from a basic neural simulator into a sophisticated simulation framework that can be easily controlled and customized for various research and experimentation needs. It provides users with powerful tools to configure, run, and analyze neural simulations with unprecedented flexibility and ease of use.