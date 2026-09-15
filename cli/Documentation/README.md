# NLM Brain CLI - Command Line Interface Documentation

## Overview

The NLM Brain CLI provides a comprehensive command-line interface for managing and simulating artificial brains. It transforms the NLM from a research prototype into a production-ready brain simulation platform.

## Quick Start

### Installation

```bash
mkdir build
cd build
cmake ..
make
```

### Basic Usage

```bash
# Interactive mode
nlm

# Batch mode (run commands from script)
nlm --batch <script.txt>

# Help
nlm help
nlm help <command>

# Create and initialize a brain
nlm create mybrain
nlm initialize mybrain.bin
nlm step mybrain.bin 100
nlm status mybrain.bin
```

## Core Brain Operations

### Create Brain

```bash
nlm create <brain_name> [config_file]

Creates a new brain with specified name and optional configuration.

Arguments:
  <brain_name>     Name for the new brain
  [config_file]    Configuration file (default: configs/default.cfg)

Example:
nlm create mybrain configs/optimized.cfg
```

### Initialize Brain

```bash
nlm initialize [brain_file]

Initializes a brain for simulation.

Arguments:
  [brain_file]     Saved brain file (default: brain.bin)

Example:
nlm initialize mybrain.bin
```

### Step Brain

```bash
nlm step [steps] [timestep] [brain_file]

Advances brain simulation by specified steps.

Arguments:
  [steps]          Number of steps to simulate (default: 10)
  [timestep]       Time step in seconds (default: 0.001)
  [brain_file]     Brain file (default: brain.bin)

Example:
nlm step 100 0.001 brain.bin
```

### Save/Load Brain

```bash
nlm save <filename>
nlm load <filename>

Saves or loads brain state to/from file.

Arguments:
  <filename>       File name for brain state

Example:
nlm save mybrain_state.bin
nlm load mybrain_state.bin
```

### Brain Status

```bash
nlm status [brain_file]

Displays detailed brain statistics and status.

Arguments:
  [brain_file]     Brain file to check (default: brain.bin)

Example:
nlm status brain.bin
```

## Advanced Brain Management

### Clone Brain

```bash
nlm clone <source_brain> [target_name] [mutation_rate]

Clones a brain with optional mutations.

Arguments:
  <source_brain>   Source brain file
  [target_name]    Target brain name (default: cloned_brain)
  [mutation_rate]  Mutation rate (0.0-1.0, default: 0.1)

Example:
nlm clone brain1.bin evolved_brain 0.05
```

### Compare Brains

```bash
nlm compare <brain1> <brain2>

Compares two brain configurations.

Arguments:
  <brain1>         First brain file
  <brain2>         Second brain file

Example:
nlm compare brain_v1.bin brain_v2.bin
```

### Batch Operations

```bash
nlm batch <operation> <brain_files...>

Performs operations on multiple brains.

Arguments:
  <operation>      Operation type (average, compare)
  <brain_files...> Brain files to process

Example:
nlm batch average brain1.bin brain2.bin brain3.bin
nlm batch compare *.bin
```

## Visualization and Export

### Visualize Neural Network

```bash
nlm visualize <type> [output_file]

Creates neural network visualizations.

Arguments:
  <type>           Visualization type (network, activity, connectivity, statistics)
  [output_file]    Output file (default: brain_visualization.png)

Example:
nlm visualize network brain_network.png
```

### Export Data

```bash
nlm export <format> [output_file]

Exports brain data in various formats.

Arguments:
  <format>         Export format (json, csv, neural_network)
  [output_file]    Output file

Example:
nlm export json brain_data.json
nlm export csv brain_data.csv
```

### Progress Monitor

```bash
nlm monitor [interval] [brain_file]

Monitors brain simulation progress.

Arguments:
  [interval]       Monitoring interval in milliseconds (default: 1000)
  [brain_file]     Brain file to monitor (default: brain.bin)

Example:
nlm monitor 500 brain.bin
```

## Advanced Analysis

### Pattern Recognition

```bash
nlm pattern <brain_file> [pattern_type]

Performs pattern recognition on brain data.

Arguments:
  <brain_file>     Brain file to analyze
  [pattern_type]    Pattern type (auto, modular, hub, small_world, default: auto)

Example:
nlm pattern brain.bin modular
```

### Learning Trajectory Analysis

```bash
nlm trajectory <brain_file1> <brain_file2>

Analyzes learning trajectory between brain states.

Arguments:
  <brain_file1>    Starting brain state
  <brain_file2>    Ending brain state

Example:
nlm trajectory brain_initial.bin brain_final.bin
```

### Performance Benchmark

```bash
nlm benchmark <brain_file> [iterations]

Runs performance benchmarks.

Arguments:
  <brain_file>     Brain file to benchmark
  [iterations]     Number of iterations (default: 10)

Example:
nlm benchmark brain.bin 50
```

### Statistical Analysis

```bash
nlm stats <brain_file> [metrics]

Performs statistical analysis.

Arguments:
  <brain_file>     Brain file to analyze
  [metrics]        Comma-separated metrics (mean,std,range, etc., default: all)

Example:
nlm stats brain.bin mean,std,range
```

## Collaboration and Sharing

### Share Brain

```bash
nlm share <brain_file> [community_id]

Shares a brain state with the community.

Arguments:
  <brain_file>     Brain file to share
  [community_id]    Community identifier (default: public)

Example:
nlm share brain.bin public
```

### Configuration Import/Export

```bash
nlm config <operation> <filename> [options]

Imports or exports configuration.

Arguments:
  <operation>       Operation (export, import)
  <filename>        Configuration file
  [options]         Additional options (format, etc.)

Example:
nlm config export brain_config.cfg
nlm config import brain_config.cfg
```

### Community Marketplace

```bash
nlm marketplace

Browse community-shared brain configurations.

Example:
nlm marketplace
```

## Development Tools

### Brain Editor

```bash
nlm edit <brain_file> [tool]

Opens brain editor with GUI-like capabilities.

Arguments:
  <brain_file>     Brain file to edit
  [tool]            Editor tool (structure, connections, properties, default: structure)

Example:
nlm edit brain.bin connections
```

### Debug Tools

```bash
nlm debug <brain_file> [debug_type]

Debug brain simulation.

Arguments:
  <brain_file>     Brain file to debug
  [debug_type]      Debug type (neuromodulation, plasticity, memory, all)

Example:
nlm debug brain.bin plasticity
```

### Experiment Design

```bash
nlm experiment <brain_file> <experiment_type>

Designs and runs experiments.

Arguments:
  <brain_file>     Brain file for experiment
  <experiment_type> Type of experiment (learning, adaptation, evolution, default: learning)

Example:
nlm experiment brain.bin evolution
```

### Testing and Validation

```bash
nlm test <brain_file> [validation_type]

Tests and validates brain.

Arguments:
  <brain_file>     Brain file to test
  [validation_type] Validation type (plasticity, learning, stability, all)

Example:
nlm test brain.bin stability
```

## Performance and Resource Management

### Resource Monitoring

```bash
nlm monitor [options]

Monitors system resources.

Options:
  --memory         Memory usage
  --cpu            CPU usage
  --network        Network usage
  --all            All resources

Example:
nlm monitor --all
```

### Performance Profiling

```bash
nlm profile <brain_file> [profile_type]

Profiles brain performance.

Arguments:
  <brain_file>     Brain file to profile
  [profile_type]    Profile type (spiking, plasticity, memory, all)

Example:
nlm profile brain.bin spiking
```

### Memory Management

```bash
nlm memory <operation> [options]

Manages brain memory.

Arguments:
  <operation>       Operation (stats, optimize, cleanup)
  [options]         Additional options

Example:
nlm memory stats brain.bin
```

### Parallel Processing

```bash
nlm parallel <brain_file> [threads]

Enables parallel processing.

Arguments:
  <brain_file>     Brain file
  [threads]         Number of threads (default: CPU cores)

Example:
nlm parallel brain.bin 8
```

## Plugin Architecture

### Plugin System

```bash
nlm plugins

Lists all loaded plugins.

Example:
nlm plugins
```

### Load Plugin

```bash
nlm loadplugin <plugin_path>

Loads a plugin from specified path.

Arguments:
  <plugin_path>     Path to plugin file

Example:
nlm loadplugin ./plugins/brain_analyzer.so
```

### Plugin Marketplace

```bash
nlm pluginmarket

Browse plugin marketplace.

Example:
nlm pluginmarket
```

## Configuration Management

### Configuration File

The CLI uses configuration files for persistent settings. Default location: `configs/default.cfg`

Configuration file format:
```
# Setting = Value
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
max_memory = 0  # Unlimited
```

### Configuration Commands

```bash
nlm config --show                    # Show current configuration
nlm config --set <key> <value>       # Set configuration value
nlm config --load <file>             # Load configuration from file
nlm config --save <file>             # Save configuration to file
```

## Configuration Files

### Basic Configuration

`configs/default.cfg` - Default configuration file

### Template Configuration

`configs/template.cfg` - Template for creating new configurations

### Custom Configuration

`configs/custom.cfg` - User-defined configuration

## Examples and Tutorials

### Example 1: Basic Brain Creation and Simulation

```bash
#!/bin/bash
# Create a brain
nlm create mybrain configs/optimized.cfg

# Initialize it
nlm initialize mybrain.bin

# Run simulation
nlm step mybrain.bin 1000 0.001

# Save results
nlm save mybrain_final.bin
```

### Example 2: Batch Processing

```bash
# Create a batch script
cat > brain_batch.sh << EOF
# Batch processing script for brain analysis
nlm create brain1
nlm initialize brain1.bin
nlm step brain1.bin 100
nlm visualize network brain1_network.png
nlm export json brain1_data.json

nlm create brain2
nlm initialize brain2.bin
nlm step brain2.bin 100
nlm visualize activity brain2_activity.png
nlm export csv brain2_data.csv

nlm compare brain1.bin brain2.bin
EOF

# Run batch script
bash brain_batch.sh
```

### Example 3: Advanced Analysis Workflow

```bash
# Create a sophisticated analysis workflow
cat > analyze_brain.sh << EOF
#!/bin/bash

# Configuration
BRAIN_FILE="mybrain.bin"
OUTPUT_DIR="analysis_results"

# Create output directory
mkdir -p $OUTPUT_DIR

# Run simulation
nlm step $BRAIN_FILE 1000 0.001

# Generate visualizations
nlm visualize network $OUTPUT_DIR/network.png
nlm visualize activity $OUTPUT_DIR/activity.png
nlm visualize connectivity $OUTPUT_DIR/connectivity.png

# Export data
nlm export json $OUTPUT_DIR/data.json
cat $OUTPUT_DIR/data.json | jq .

# Perform analysis
nlm pattern $BRAIN_FILE modular
nlm trajectory brain_initial.bin brain_final.bin
nlm benchmark $BRAIN_FILE 20

# Generate statistics
nlm stats $BRAIN_FILE mean,std,range,skewness

# Create report
{ echo "=== Brain Analysis Report ==="
  echo "Brain: $BRAIN_FILE"
  echo "Date: $(date)"
  echo "Generated visualizations in $OUTPUT_DIR"
  echo "Export file: $OUTPUT_DIR/data.json"
} > $OUTPUT_DIR/report.txt

echo "Analysis completed. Results in $OUTPUT_DIR"
EOF

# Make executable and run
chmod +x analyze_brain.sh
./analyze_brain.sh
```

## Configuration Management

### Setting Configuration Values

```bash
# Set configuration value
nlm config --set simulation.timestep 0.001

# Load configuration from file
nlm config --load configs/optimized.cfg

# Save current configuration
nlm config --save myconfig.cfg

# Show current configuration
nlm config --show
```

### Configuration Templates

Create new configuration files using templates:

```bash
# Create from template
nlm config --template configs/template.cfg configs/myconfig.cfg

# View available templates
nlm config --templates
```

## Error Handling

### Error Codes

- `0` - Success
- `1` - General error
- `2` - Invalid command
- `3` - Configuration error
- `4` - File not found
- `5` - Permission denied
- `6` - Out of memory
- `7` - Invalid arguments

### Error Handling Options

```bash
# Strict mode (fail on first error)
nlm --strict command

# Verbose mode (show detailed error information)
nlm --verbose command

# Quiet mode (suppress non-error output)
nlm --quiet command
```

## Cross-Platform Compatibility

### Linux

```bash
# Build and install
./build.sh

# Run
./bin/nlm
```

### macOS

```bash
# Build with Homebrew dependencies (if available)
brew install cmake

# Build and run
./build.sh
./bin/nlm
```

### Windows

```bash
# Build with CMake
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release

# Run
Release\nlm.exe
```

## Troubleshooting

### Common Issues

**Issue:** Brain simulation doesn't start
**Solution:** Check if brain file exists and configuration is valid
```bash
nlm status mybrain.bin
```

**Issue:** Memory usage too high
**Solution:** Reduce brain size or increase memory limits
```bash
nlm config --set max_memory 2048000000  # 2GB
```

**Issue:** Plugin not loading
**Solution:** Check plugin file and permissions
```bash
chmod +x myplugin.so
nlm loadplugin ./myplugin.so
```

**Issue:** Configuration file not found
**Solution:** Use default configuration or create custom file
```bash
nlm create mybrain  # Uses default config
nlm config --load configs/custom.cfg
```

## Advanced Features

### Scripting

The CLI supports scripting through batch files and shell scripts. All commands can be chained and redirected.

### Automation

Use cron or scheduled tasks for automated brain processing:

```bash
# Add to crontab for daily analysis
0 2 * * * /path/to/bin/nlm --batch /path/to/nightly_analysis.sh
```

### Integration

The CLI can be integrated with other tools:

- **Python:** Use subprocess to call CLI commands
- **JavaScript:** Use child_process to execute commands
- **Docker:** Run CLI in containerized environments

## References

- [NLM Documentation](https://docs.nlm.org)
- [NLM GitHub Repository](https://github.com/NLM-project/nlm)
- [Plugin Development Guide](https://docs.nlm.org/plugins)

## License

MIT License

---

*Documentation generated by NLM Brain CLI*
