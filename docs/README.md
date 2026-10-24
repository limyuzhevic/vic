# NLM Neural Learning Machine - Documentation System
# Comprehensive documentation for NLM CLI and advanced features

This documentation provides comprehensive information about the NLM (Neural Learning Machine) project, including:

## Table of Contents

- [Introduction](#introduction)
- [Command Line Interface](#command-line-interface)
- [Advanced Features](#advanced-features)
- [Configuration Management](#configuration-management)
- [Brain Operations](#brain-operations)
- [Experiment Control](#experiment-control)
- [Monitoring and Analysis](#monitoring-and-analysis)
- [Export and Import](#export-and-import)
- [Script Execution](#script-execution)
- [System Administration](#system-administration)
- [Performance Optimization](#performance-optimization)
- [Troubleshooting](#troubleshooting)
- [API Reference](#api-reference)
- [Migration Guide](#migration-guide)
- [Contributing](#contributing)

## Introduction

The NLM Neural Learning Machine is an advanced artificial intelligence platform that implements real spiking neural computation with event-driven dynamics, synaptic plasticity, and integrated memory systems.

### Key Features

- **Real Neural Computation**: Leaky Integrate-and-Fire (LIF) neuron dynamics
- **Event-Driven Processing**: Spike-driven synaptic transmission with delays
- **Advanced Plasticity**: STDP and Hebbian plasticity rules
- **Structural Plasticity**: Synaptogenesis and pruning
- **Integrated Memory**: Working, episodic, and associative memory systems
- **Prediction Systems**: Forward and inverse models
- **Cognitive Functions**: Planning, attention, and concept formation
- **Neuromodulation**: Dopamine, curiosity, and reward systems
- **Developmental Processes**: Critical periods and maturation

## Command Line Interface

The NLM project provides a comprehensive command-line interface with multiple executables:

### Main Executables

1. **nlm** - Main NLM application (demonstrates neural computation)
2. **nlm_cli** - Enhanced command-line interface with advanced features
3. **nlm_cli_simple** - Simplified CLI interface
4. **nlm_phase3_demo** - Phase 3 demonstration
5. **nlm_phase4_demo** - Phase 4 demonstration

### CLI Usage Examples

```bash
# Run main NLM application
./nlm

# Use enhanced CLI with help
./nlm_cli --help

# Run specific brain operation
./nlm_cli brain create -n mybrain -N 500 -R 2

# Run experiment
./nlm_cli experiment create -n myexp -t learning -S 1000

# Show system configuration
./nlm_cli config list

# Monitor brain activity
./nlm_cli monitor stats

# Export brain state
./nlm_cli export -f brain.checkpoint --neurons --synapses
```

## Advanced Features

### Power User Commands

The enhanced CLI provides additional commands for power users:

#### System Administration

- `nlm_cli system` - System administration commands
- `nlm_cli system perf` - Performance monitoring
- `nlm_cli system benchmark` - Performance benchmarking
- `nlm_cli system backup` - Create system backups
- `nlm_cli system restore` - Restore from backup

#### Advanced Brain Operations

- `nlm_cli brain optimize` - Optimize brain with specified method
- `nlm_cli brain evolve` - Evolve brain for specified generations
- `nlm_cli brain transfer` - Transfer brain state between files
- `nlm_cli brain diagnose` - Diagnose brain with specified check
- `nlm_cli brain analyze` - Analyze brain performance
- `nlm_cli brain profile` - Profile brain performance

#### Advanced Experiment Control

- `nlm_cli experiment parallel` - Run multiple experiments in parallel
- `nlm_cli experiment hyperparameter-tune` - Tune hyperparameters
- `nlm_cli experiment compare` - Compare results from multiple files
- `nlm_cli experiment analyze` - Analyze experimental data

#### Advanced Monitoring

- `nlm_cli monitor live` - Enable live monitoring
- `nlm_cli monitor predict` - Make predictions
- `nlm_cli monitor anomalies` - Detect anomalies
- `nlm_cli monitor performance` - Monitor performance metrics

#### Advanced Export/Import

- `nlm_cli io batch-export` - Export multiple files
- `nlm_cli io batch-import` - Import multiple files
- `nlm_cli io transform` - Transform data format
- `nlm_cli io validate` - Validate file against schema

#### Script Execution

- `nlm_cli script run` - Execute scripts
- `nlm_cli script list` - List available scripts
- `nlm_cli script upload` - Upload script files
- `nlm_cli script schedule` - Schedule script execution

## Configuration Management

### Configuration File Format

NLM uses configuration files in a simple key=value format:

```ini
# NLM Configuration File

# Random seed
random_seed = 42

# Neural parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Logging
log_level = info
log_file = nlm.log
```

### Configuration Commands

```bash
# List all configuration values
./nlm_cli config list

# Set configuration value
./nlm_cli config set --key neuron_count --value 2000

# Load configuration from file
./nlm_cli config load --file myconfig.cfg

# Save configuration to file
./nlm_cli config save --file myconfig.cfg
```

## Brain Operations

### Brain Creation

Create a new brain with specified parameters:

```bash
./nlm_cli brain create -n mybrain -N 1000 -R 2 --plasticity --stdp --hebbian --structural
```

Options:
- `-n, --name <name>`: Brain name
- `-N, --neurons <count>`: Number of neurons (default: 1000)
- `-R, --regions <count>`: Number of regions (default: 1)
- `--plasticity`: Enable synaptic plasticity
- `--stdp`: Enable STDP plasticity
- `--hebbian`: Enable Hebbian plasticity
- `--structural`: Enable structural plasticity
- `-s, --seed <seed>`: Random seed (default: 42)

### Brain Exploration

Explore existing brain state:

```bash
./nlm_cli brain explore -n mybrain
```

### Brain Optimization

Optimize brain with specified method:

```bash
./nlm_cli brain optimize --method stdp
./nlm_cli brain optimize --method hebbian
./nlm_cli brain optimize --method reward
```

### Brain Evolution

Evolve brain for specified generations:

```bash
./nlm_cli brain evolve --generations 100
```

### Brain Transfer

Transfer brain state between files:

```bash
./nlm_cli brain transfer --source brain1.nvm --target brain2.nvm
```

### Brain Diagnosis

Diagnose brain with specified check:

```bash
./nlm_cli brain diagnose --check plasticity
./nlm_cli brain diagnose --check connectivity
./nlm_cli brain diagnose --check memory
```

### Brain Analysis

Analyze brain performance and provide recommendations:

```bash
./nlm_cli brain analyze
```

### Brain Profiling

Profile brain performance:

```bash
./nlm_cli brain profile
```

## Experiment Control

### Experiment Creation

Create a new experiment:

```bash
./nlm_cli experiment create -n myexp -t learning -S 1000 --detailed
```

Options:
- `-n, --name <name>`: Experiment name
- `-t, --type <type>`: Experiment type (connectivity, plasticity, stdp, learning)
- `-S, --steps <steps>`: Maximum simulation steps (default: 1000)
- `--detailed`: Generate detailed results
- `--results-dir <dir>`: Directory for experiment results

### Parallel Experiment Execution

Run multiple experiments in parallel:

```bash
./nlm_cli experiment parallel --runs 10 --method stdp
```

### Hyperparameter Tuning

Tune hyperparameters:

```bash
./nlm_cli experiment hyperparameter-tune --param learning_rate
```

### Experiment Comparison

Compare results from multiple files:

```bash
./nlm_cli experiment compare --brains brain1.nvm brain2.nvm
```

### Experiment Analysis

Analyze experimental data:

```bash
./nlm_cli experiment analyze --data results.csv
```

## Monitoring and Analysis

### Basic Monitoring

Show brain statistics:

```bash
./nlm_cli monitor stats
```

Enable live monitoring:

```bash
./nlm_cli monitor stats --live --interval 1s
```

### Advanced Monitoring

Make predictions:

```bash
./nlm_cli monitor predict --horizon 10s
```

Detect anomalies:

```bash
./nlm_cli monitor anomalies --threshold 2.0
```

Monitor performance metrics:

```bash
./nlm_cli monitor performance --metric spike_rate
```

### Monitoring Options

- `--live`: Enable live monitoring
- `--format <format>`: Output format (json, csv, xml)
- `--interval <time>`: Monitoring interval
- `--horizon <time>`: Prediction horizon
- `--threshold <value>`: Anomaly detection threshold
- `--metric <name>`: Performance metric to monitor

## Export and Import

### Basic Export

Export brain state:

```bash
./nlm_cli export -f brain.checkpoint --neurons --synapses --plasticity
```

### Batch Export

Export multiple files matching pattern:

```bash
./nlm_cli io batch-export --pattern *.nvm --format json
```

### Basic Import

Import brain state:

```bash
./nlm_cli import -f brain.checkpoint --neurons --synapses
```

### Batch Import

Import multiple files from directory:

```bash
./nlm_cli io batch-import --directory ./exports
```

### Data Transformation

Transform data format:

```bash
./nlm_cli io transform --format json --output binary
```

### Data Validation

Validate file against schema:

```bash
./nlm_cli io validate --file brain.nvm --schema standard
```

## Script Execution

### Script Execution

Execute scripts:

```bash
./nlm_cli script run --file experiment.py
```

List available scripts:

```bash
./nlm_cli script list --path ./scripts
```

Upload script files:

```bash
./nlm_cli script upload --file script.sh
```

Schedule script execution:

```bash
./nlm_cli script schedule --file daily_run.sh --time 02:00
```

## System Administration

### System Configuration

Show system configuration:

```bash
./nlm_cli system config --show
```

Set system configuration:

```bash
./nlm_cli system config --set random_seed=123
```

### Performance Monitoring

Start performance monitoring:

```bash
./nlm_cli system perf --monitor
```

### Performance Benchmarking

Run performance benchmark:

```bash
./nlm_cli system benchmark --name neural_simulation
```

### Backup and Recovery

Create system backup:

```bash
./nlm_cli system backup --create --path ./backup
```

Restore from backup:

```bash
./nlm_cli system restore --path ./backup
```

### Remote Monitoring

Start remote monitoring:

```bash
./nlm_cli system monitor --start
```

Stop remote monitoring:

```bash
./nlm_cli system monitor --stop
```

## Performance Optimization

### Thread Pool Configuration

Configure thread pool size:

```bash
./nlm_cli system config --set max_threads=8
```

### Memory Management

Configure memory usage:

```bash
./nlm_cli system config --set max_memory_usage=2048000000
```

### Profiling

Enable profiling:

```bash
./nlm_cli system perf --profile --output profiling.json
```

### Benchmarking

Run benchmarks:

```bash
./nlm_cli system benchmark --name comprehensive --output benchmark.json
```

## Troubleshooting

### Common Issues

1. **Brain initialization fails**
   - Check configuration file syntax
   - Verify available memory
   - Ensure required libraries are installed

2. **Experiment execution fails**
   - Check experiment parameters
   - Verify input files exist
   - Ensure output directory is writable

3. **Export/import fails**
   - Check file permissions
   - Verify file format
   - Ensure disk space is available

4. **Monitoring fails**
   - Check network connectivity
   - Verify monitoring configuration
   - Ensure monitoring tools are installed

### Error Messages

- `[ERROR] Configuration file not found`
- `[ERROR] Failed to initialize brain`
- `[ERROR] Experiment execution failed`
- `[ERROR] Export/import failed`
- `[ERROR] Monitoring connection failed`

### Diagnostic Commands

Run diagnostic commands:

```bash
./nlm_cli system status --check all
./nlm_cli system perf --diagnose
./nlm_cli brain diagnose --check connectivity
./nlm_cli experiment analyze --diagnose
```

## API Reference

### Core Classes

- `nlm::Brain` - Main brain class
- `nlm::Neuron` - Single neuron class
- `nlm::Synapse` - Synapse class
- `nlm::NeuralRegion` - Neural region class
- `nlm::NeuralPopulation` - Neural population class

### Configuration System

- `nlm::Config` - Configuration management
- `nlm::ConfigEntry` - Configuration entry
- `nlm::ConfigSource` - Configuration source

### Memory Systems

- `nlm::WorkingMemory` - Working memory
- `nlm::EpisodicMemory` - Episodic memory
- `nlm::AssociativeMemory` - Associative memory

### Prediction Systems

- `nlm::PredictionSystem` - Prediction system
- `nlm::NeuralPrediction` - Neural prediction

### Cognitive Functions

- `nlm::NeuralPlanner` - Neural planner
- `nlm::ConceptFormation` - Concept formation
- `nlm::AttentionalSelection` - Attentional selection

### Neuromodulation

- `nlm::Dopamine` - Dopamine system
- `nlm::Curiosity` - Curiosity system
- `nlm::PredictionError` - Prediction error signal

### Development

- `nlm::DevelopmentSystem` - Development system
- `nlm::Synaptogenesis` - Synaptogenesis
- `nlm::Pruning` - Pruning
- `nlm::Maturation` - Maturation

## Migration Guide

### From NLM v0.1 to v0.2

1. **CLI Changes**
   - New advanced CLI commands available
   - Configuration format changes
   - Additional options for existing commands

2. **Brain API Changes**
   - New plasticity methods
   - Enhanced memory integration
   - Improved developmental processes

3. **Configuration Changes**
   - New configuration options
   - Improved configuration file format
   - Better configuration validation

4. **Experiment Changes**
   - New experiment types
   - Enhanced experiment control
   - Improved experiment analysis

## Contributing

### Code Style

- Use C++20 features
- Follow existing code conventions
- Write comprehensive comments
- Add unit tests
- Follow KISS principle

### Testing

Run tests:

```bash
cmake --build build --target test
```

Run specific tests:

```bash
ctest -R <test_name>
```

### Documentation

Update documentation:

```bash
./nlm_cli help --update-docs
```

Generate documentation:

```bash
make doc
```

### Bug Reports

Report bugs:

1. Use issue tracker
2. Provide reproduction steps
3. Include error messages
4. Attach logs if available

### Feature Requests

Request features:

1. Use feature request tracker
2. Provide detailed description
3. Include use case
4. Suggest implementation approach

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

We acknowledge the following contributors and projects:

- CLI20 library for CLI functionality
- nlohmann/json library for JSON parsing
- CMake for build system
- Various open-source libraries and frameworks

## Contact

For questions and support, please contact:

- Email: support@nlm.org
- GitHub: https://github.com/nlm-project/nlm
- Discord: https://discord.gg/nlm

---

This documentation is continuously updated. For the latest version, visit the project repository.
