# Enhanced NLM Phase 6 Demo

## Overview

This is an enhanced version of the NLM (Neural Learning Machine) Phase 6 integration demo that provides comprehensive functionality with advanced features including:

- **Advanced CLI interface** with comprehensive argument parsing
- **Configuration file support** (.json, .cfg formats)
- **Interactive modes** and help systems
- **Real-time progress indicators** and feedback
- **Detailed statistics** and metrics visualization
- **Colorized output** and enhanced user experience
- **Logging integration** with file output
- **Results persistence** with multiple export formats
- **Educational features** with tutorial modes
- **Power user features** with batch processing and parameter tuning
- **Advanced error handling** with graceful recovery
- **Visualization integration** (GraphViz/Plotly support)
- **Activity monitoring** and state snapshots
- **Plugin architecture** for extensibility

## Key Features

### 1. Advanced CLI Interface
- Comprehensive argument parsing with short and long options
- Built-in help system with usage examples
- Version information
- Interactive mode support
- Export/import capabilities

### 2. Configuration Management
- Support for .cfg and .json configuration files
- Command-line argument override
- Configuration validation
- Backup and restore functionality
- Persistent state management

### 3. Enhanced User Experience
- Real-time progress indicators with visual feedback
- Colorized terminal output
- Detailed statistics and metrics
- Logging integration with file output
- Results persistence across runs

### 4. Visualization Integration
- Network visualization support
- Activity monitoring dashboards
- State snapshots for debugging
- Data export for external tools

### 5. Educational Features
- Tutorial modes for step-by-step guidance
- Concept explanations and interactive learning
- Progress tracking and achievement systems
- Built-in documentation and examples

### 6. Power User Features
- Batch processing capabilities
- Parameter tuning and optimization
- Comparative experiments
- Plugin architecture
- API integration for external tools

### 7. Advanced Error Handling
- Graceful error recovery
- Detailed error messages
- Input validation
- Safe defaults
- Comprehensive logging

## File Structure

```
src/experiments/
├── Phase6Demo.hpp          # Enhanced Phase 6 demo header
├── Phase6Demo.cpp          # Enhanced Phase 6 demo implementation
├── EnhancedPhase6Demo.hpp   # Advanced demo class
├── EnhancedPhase6Demo.cpp   # Advanced demo implementation
├── CLIParser.hpp           # Command-line argument parser
├── CLIParser.cpp           # Command-line parser implementation
├── Phase6Demo Original/
│   ├── Phase6Demo.cpp       # Original Phase 6 demo
│   └── Phase6Demo.hpp       # Original Phase 6 demo header
└── configs/
    └── default.cfg        # Default configuration file
```

## Building

### Prerequisites
- C++20 compiler (GCC 11+ or Clang 14+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings, if needed)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Run tests (if built)
ctest --output-on-failure
```

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Running

### Basic Usage

```bash
# Run enhanced Phase 6 demo
./nlm_phase6_demo_enhanced

# Show help information
./nlm_phase6_demo_enhanced --help

# Show version information
./nlm_phase6_demo_enhanced --version
```

### Configuration Examples

```bash
# Run with custom configuration file
./nlm_phase6_demo_enhanced --config myconfig.cfg

# Export results to file
./nlm_phase6_demo_enhanced --output results.json

# Enable educational mode
./nlm_phase6_demo_enhanced --education integration

# Enable power user mode
./nlm_phase6_demo_enhanced --power batch --max-episodes 20

# Enable verbose output
./nlm_phase6_demo_enhanced --verbose

# Suppress informational messages
./nlm_phase6_demo_enhanced --quiet
```

### Configuration File Examples

**basic.cfg**
```
# Basic configuration
neuron_count = 500
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
enable_checkpointing = true
enable_replay = true
enable_development = true
```

**educational.cfg**
```
# Configuration optimized for educational use
neuron_count = 200
region_count = 1
connection_probability = 0.05
simulation_timestep = 0.001
stdp_ltp_weight = 0.005
stdp_ltd_weight = 0.006
enable_checkpointing = true
enable_replay = true
enable_development = true
enable_visualization = true
```

**poweruser.json**
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.025,
    "stdp_time_constant": 25.0,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true,
    "max_steps": 5000,
    "max_episodes": 10,
    "enable_visualization": true,
    "visualization_update_rate": 30.0
}
```

## Command Line Options

### Basic Options
- `--help, -h` - Show help message
- `--version, -v` - Show version information
- `--verbose, -V` - Enable verbose output
- `--quiet, -q` - Suppress informational messages
- `--interactive, -i` - Enable interactive mode

### Configuration Options
- `--config, -c <file>` - Load configuration from file
- `--output, -o <file>` - Save results to file
- `--log <file>` - Log output to file

### Display Options
- `--enable-colors, --color` - Enable colored output (default)
- `--no-colors, --nocolor` - Disable colored output
- `--enable-progress, --progress` - Enable progress indicators (default)
- `--no-progress, --noprogress` - Disable progress indicators
- `--enable-logging, --logging` - Enable logging to file (default)
- `--no-logging, --nologging` - Disable logging

### Simulation Control
- `--max-steps <n>` - Maximum number of simulation steps (default: 2000)
- `--max-episodes <n>` - Maximum number of episodes (default: 5)
- `--timeout <s>` - Timeout in seconds (default: 0 = unlimited)

### Educational Mode
- `--education <topic>` - Enable educational mode with step-by-step guidance
  - Topics: basic, integration, memory, neuromodulation, prediction, development, replay, checkpointing

### Power User Mode
- `--power <mode>` - Enable power user mode with advanced features
  - Modes: batch, tuning, comparison, plugin, api

### Export/Import
- `--export-results` - Export results to file (default: JSON)
- `--export-format <fmt>` - Export format (json, csv, xml)
- `--import-results` - Import results from file
- `--import-format <fmt>` - Import format (json, csv, xml)

## Educational Features

### Tutorial Modes
The enhanced demo includes several tutorial modes to help users understand the NLM architecture:

1. **Basic Tutorial** - Introduction to NLM and Phase 6
2. **Integration Tutorial** - Step-by-step integration testing
3. **Memory Tutorial** - Working and episodic memory systems
4. **Neuromodulation Tutorial** - Dopamine, curiosity, and novelty systems
5. **Prediction Tutorial** - Temporal and causal prediction systems
6. **Development Tutorial** - Age-dependent plasticity
7. **Replay Tutorial** - Memory consolidation and replay
8. **Checkpoint Tutorial** - State persistence and recovery

Each tutorial provides:
- Detailed explanations of concepts
- Step-by-step guidance
- Interactive feedback
- Progress tracking
- Assessment of understanding

### Progress Tracking
The educational system tracks user progress and provides:
- Achievement badges
- Progress reports
- Personalized learning paths
- Performance metrics

## Power User Features

### Batch Processing
- Run multiple experiments with different parameters
- Automated parameter tuning
- Comparative analysis of different configurations
- Statistical analysis of results

### Plugin Architecture
- Extensible plugin system
- Custom experiment definitions
- Visualization plugins
- Data export plugins

### API Integration
- REST API for external tools
- Programmatic access to demo functionality
- Integration with visualization tools
- Data export for analysis

## Results Export

The enhanced demo can export results in multiple formats:

### JSON Format (Default)
```json
{
    "demo": "NLM Phase 6 Enhanced",
    "version": "2.0.0-Enhanced",
    "timestamp": "2026-10-04 05:22:00",
    "totalReward": 150.5,
    "avgFiringRate": 8.3,
    "avgSynapticWeight": 0.45,
    "memoryEpisodesStored": 25,
    "integrationStatus": {
        "memoryWorkingMemoryIntegrated": true,
        "memoryEpisodicMemoryIntegrated": true,
        "neuromodulationIntegrated": true,
        "predictionIntegrated": true,
        "developmentIntegrated": true,
        "checkpointingWorks": true,
        "replayWorks": true
    }
}
```

### CSV Format
```csv
demo,version,timestamp,totalReward,avgFiringRate,memoryEpisodesStored,memoryWorkingMemoryIntegrated,memoryEpisodicMemoryIntegrated,neuromodulationIntegrated,predictionIntegrated,developmentIntegrated,checkpointingWorks,replayWorks
NLM Phase 6 Enhanced,2.0.0-Enhanced,2026-10-04 05:22:00,150.5,8.3,25,true,true,true,true,true,true,true
```

### XML Format
```xml
<?xml version="1.0" encoding="UTF-8"?>
<demoResult>
    <demo>NLM Phase 6 Enhanced</demo>
    <version>2.0.0-Enhanced</version>
    <timestamp>2026-10-04 05:22:00</timestamp>
    <totalReward>150.5</totalReward>
    <avgFiringRate>8.3</avgFiringRate>
    <memoryEpisodesStored>25</memoryEpisodesStored>
    <integrationStatus>
        <memoryWorkingMemoryIntegrated>true</memoryWorkingMemoryIntegrated>
        <memoryEpisodicMemoryIntegrated>true</memoryEpisodicMemoryIntegrated>
        <neuromodulationIntegrated>true</neuromodulationIntegrated>
        <predictionIntegrated>true</predictionIntegrated>
        <developmentIntegrated>true</developmentIntegrated>
        <checkpointingWorks>true</checkpointingWorks>
        <replayWorks>true</replayWorks>
    </integrationStatus>
</demoResult>
```

## Configuration Management

### Configuration File Support
The enhanced demo supports two configuration file formats:

#### .cfg Format (Simple Key-Value)
```
# Simple configuration file
neuron_count = 500
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
enable_checkpointing = true
enable_replay = true
enable_development = true
```

#### .json Format (Structured)
```json
{
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true
}
```

### Configuration Management Features
- **Validation**: Configuration validation with detailed error messages
- **Backup**: Automatic backup of configuration files
- **Import/Export**: Import and export configuration between runs
- **Override**: Command-line arguments override configuration files
- **Merge**: Merge configuration files and command-line arguments

### Configuration Keys
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of connection between neurons |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds |
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight |
| `stdp_time_constant` | float | 20.0 | STDP time constant |
| `enable_checkpointing` | bool | true | Enable checkpointing |
| `enable_replay` | bool | true | Enable replay |
| `enable_development` | bool | true | Enable development |

## Error Handling

### Error Types
1. **Configuration Errors**: Invalid configuration files
2. **File I/O Errors**: Read/write failures
3. **Memory Errors**: Out of memory or allocation failures
4. **Simulation Errors**: Numerical instabilities or crashes
5. **Plugin Errors**: Plugin loading or execution failures

### Error Recovery
- **Graceful Degradation**: Continue with default values if configuration is invalid
- **Retry Logic**: Retry failed operations with exponential backoff
- **Fallback Mechanisms**: Use built-in defaults if custom settings fail
- **Detailed Reporting**: Comprehensive error messages with context

### Error Messages
All error messages follow a consistent format:
```
[ERROR] <timestamp>: <error message>
[CAUSE] <root cause>: <description>
[SOLUTION] <suggested action>: <steps to fix>
```

## Logging

### Log Levels
- **DEBUG**: Detailed debugging information
- **INFO**: General information messages
- **WARNING**: Potentially problematic situations
- **ERROR**: Error conditions
- **CRITICAL**: Critical errors requiring attention

### Log Output
- **Console**: Real-time output to terminal with colors
- **File**: Persistent log files for debugging
- **Format**: Structured log entries with timestamps

### Log Examples
```
[INFO] 2026-10-04 05:22:00: Enhanced Phase 6 Demo started
[INFO] 2026-10-04 05:22:01: Configuration loaded from: demo.cfg
[INFO] 2026-10-04 05:22:02: Running integration verification...
[INFO] 2026-10-04 05:22:03: Integration verification passed!
[INFO] 2026-10-04 05:22:04: Demo results saved to: results.json
[DEBUG] 2026-10-04 05:22:05: Neural population: 500 neurons, 1 regions, 50 synapses
[WARNING] 2026-10-04 05:22:06: Simulation timestep very small (0.0001), may affect performance
[ERROR] 2026-10-04 05:22:07: Failed to load plugin: visualization_v2
[CRITICAL] 2026-10-04 05:22:08: Simulation crashed due to numerical instability
```

## Performance Monitoring

### Metrics Collected
- **Simulation Time**: Real-time tracking of simulation progress
- **Memory Usage**: RAM consumption during execution
- **CPU Usage**: Processor utilization
- **Network I/O**: File read/write operations
- **Plugin Performance**: Loading and execution times

### Performance Reports
- **Benchmark Results**: Performance comparison across configurations
- **Resource Usage**: Detailed resource consumption analysis
- **Bottleneck Analysis**: Identification of performance constraints
- **Optimization Recommendations**: Suggestions for performance improvements

## Backward Compatibility

The enhanced Phase 6 demo maintains full backward compatibility with the original implementation:

### Original CLI Usage
```bash
# Original usage continues to work
./nlm_phase6_demo 5 200  # 5 episodes, 200 steps each
./nlm_phase6_demo --neuron_count=500 --connection_probability=0.1
```

### Configuration File Format
- Original .cfg format is supported
- Original configuration keys are maintained
- No breaking changes to existing scripts

### Output Format
- Original output format is preserved
- Additional output provided for enhanced features
- Progress indicators are optional and can be disabled

### API Compatibility
- All original function signatures remain unchanged
- New functionality is added as extensions
- No changes to core NLM functionality

## Migration Guide

### For New Users
If you are new to the NLM project:
1. **Start with Basic Demo**: Use `./nlm_phase6_demo_enhanced` for basic usage
2. **Explore Configuration**: Try `./nlm_phase6_demo_enhanced --config demo.cfg`
3. **Try Educational Mode**: Use `./nlm_phase6_demo_enhanced --education basic`
4. **Explore Power User Features**: Try `./nlm_phase6_demo_enhanced --power batch`

### For Existing Users
If you are already using the original Phase 6 demo:
1. **Upgrade**: Replace `./nlm_phase6_demo` with `./nlm_phase6_demo_enhanced`
2. **Keep Existing Scripts**: All existing CLI arguments continue to work
3. **New Features**: Explore new configuration file options
4. **Gradual Adoption**: Gradually adopt new features at your own pace

## Troubleshooting

### Common Issues

#### Issue: "Configuration file not found"
**Solution**: Ensure the configuration file exists and is readable
```bash
# Check if file exists
ls -la config.cfg

# Try with absolute path
./nlm_phase6_demo_enhanced --config /path/to/config.cfg
```

#### Issue: "Permission denied"
**Solution**: Check file permissions
```bash
# Check file permissions
ls -la output.json

# Ensure write permissions
chmod 644 output.json
```

#### Issue: "Memory allocation failed"
**Solution**: Reduce configuration values
```bash
# Reduce neuron count
./nlm_phase6_demo_enhanced --neuron_count=100

# Increase available memory if possible
```

#### Issue: "Simulation timestep too small"
**Solution**: Increase simulation timestep
```bash
# Use larger timestep
./nlm_phase6_demo_enhanced --simulation_timestep=0.01
```

### Getting Help

#### Online Resources
- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Issues**: https://github.com/nlm-project/nlm/issues

#### Local Documentation
- **README.md**: Basic usage and building instructions
- **HOW_TO_USE.md**: Detailed documentation
- **ENHANCEMENT_SUMMARY.md**: Summary of enhancements
- **FINAL_STATUS_REPORT.md**: Final status and achievements

#### Interactive Help
```bash
# Show all options
./nlm_phase6_demo_enhanced --help

# Show version information
./nlm_phase6_demo_enhanced --version

# Show educational topics
./nlm_phase6_demo_enhanced --education help
```

## Conclusion

The Enhanced NLM Phase 6 Demo provides a comprehensive solution that:

1. **Maintains Backward Compatibility**: All existing functionality continues to work
2. **Adds Advanced Features**: New features are available as enhancements
3. **Improves User Experience**: Better CLI, configuration, and visualization
4. **Supports Education**: Tutorial modes and learning paths
5. **Enables Power User Features**: Advanced capabilities for experienced users
6. **Ensures Reliability**: Comprehensive error handling and logging
7. **Facilitates Research**: Export capabilities and plugin architecture

This enhanced demo represents a significant improvement over the original Phase 6 demo while maintaining the simplicity and effectiveness that made it successful in the first place.

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
