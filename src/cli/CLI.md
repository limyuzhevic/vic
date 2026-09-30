# NLM CLI Tool Documentation

The NLM CLI tool is an advanced command-line interface for the Neural Learning Machine (NLM) project. It provides comprehensive options for neural network simulations, configuration management, benchmarking, and data export.

## Overview

The CLI tool extends the existing NLM functionality by providing:

- **Advanced argument parsing** with subcommands and complex option handling
- **Multiple simulation modes** (Basic, Connectivity, Plasticity, STDP, Hebbian, Structural, Neuromodulation)
- **Configuration file management** with JSON, YAML, and key-value formats
- **Multiple output formats** (JSON, YAML, CSV, XML, text)
- **Progress reporting** with visual progress bars
- **Performance benchmarking** capabilities
- **Data export** in various formats
- **Debug and profiling** options

## Installation

To build the CLI tool, ensure you have CMake 3.16 or higher installed, then run:

```bash
mkdir build
cd build
cmake ..
make
```

The CLI tool will be installed as `nlm_cli` in the build directory.

## Usage

### Basic Usage

```bash
# Show help information
nlm_cli --help

# Show command list
nlm_cli help

# Run a simulation
nlm_cli simulate --mode basic --steps 1000 --neuron-count 500 --output results.json

# Manage configuration
nlm_cli config --load configs/my_config.yaml
nlm_cli config --get simulation_timestep
nlm_cli config --set debug true
nlm_cli config --save configs/my_config.yaml

# Run benchmarks
nlm_cli benchmark --duration 60 --neuron-count 1000 --iterations 10

# Show system information
nlm_cli info --summary

# Export data
nlm_cli export --format json --output brain_data.json --neurons
nlm_cli export --format csv --output spike_data.csv --spikes
```

### Command Reference

#### `simulate` Command

Runs neural simulations with various modes and configurations.

**Usage:**
```bash
nlm_cli simulate [OPTIONS] [MODE] [OUTPUT_FILE]
```

**Options:**
- `--mode MODE` - Simulation mode (basic, connectivity, plasticity, stdp, hebbian, structural, neuromodulation)
- `--steps N` - Number of simulation steps (default: 1000)
- `--timestep T` - Simulation timestep in seconds (default: 0.001)
- `--neuron-count N` - Number of neurons (default: 500)
- `--region-count N` - Number of neural regions (default: 1)
- `--plasticity` - Enable plasticity rules
- `--stdp` - Enable STDP plasticity
- `--hebbian` - Enable Hebbian plasticity
- `--structural` - Enable structural plasticity
- `--save FILE` - Save simulation results to file
- `--checkpoint N` - Save checkpoint every N steps
- `--batch-size N` - Number of parallel simulations (default: 1)
- `--parallel` - Enable parallel processing

**Examples:**
```bash
# Basic simulation
nlm_cli simulate --mode basic --steps 1000 --output basic_sim.json

# Plasticity learning simulation
nlm_cli simulate --mode plasticity --steps 5000 --plasticity --output learning_results.json

# STDP verification
nlm_cli simulate --mode stdp --steps 2000 --stdp --output stdp_results.json
```

#### `config` Command

Manages NLM configuration files and settings.

**Usage:**
```bash
nlm_cli config [OPTIONS] [KEY] [VALUE]
```

**Options:**
- `--load FILE` - Load configuration from file
- `--save FILE` - Save current configuration to file
- `--get KEY` - Get configuration value for key
- `--set KEY VALUE` - Set configuration value for key
- `--remove KEY` - Remove configuration key
- `--list` - List all configuration keys and values
- `--schema` - Show configuration schema
- `--validate` - Validate current configuration
- `--generate MODE` - Generate configuration for mode (basic, advanced)
- `--format FORMAT` - Configuration file format (json, yaml, keyvalue)

**Examples:**
```bash
# Load configuration
nlm_cli config --load configs/experiment.yaml

# Set a configuration value
nlm_cli config --set debug true
nlm_cli config --set neuron_count 1000
nlm_cli config --set connection_probability 0.2

# Get configuration values
nlm_cli config --get debug
nlm_cli config --get neuron_count

# List configuration
nlm_cli config --list

# Generate configuration
nlm_cli config --generate basic --save configs/basic_config.yaml
nlm_cli config --generate advanced --save configs/advanced_config.yaml
```

#### `benchmark` Command

Runs performance benchmarks for different components of the NLM system.

**Usage:**
```bash
nlm_cli benchmark [OPTIONS] [BENCHMARK_TYPE]
```

**Options:**
- `--duration S` - Benchmark duration in seconds (default: 60)
- `--iterations N` - Number of iterations (default: 10)
- `--neuron-count N` - Number of neurons to use (default: 1000)
- `--parallel` - Enable parallel benchmarking
- `--output FILE` - Output benchmark results to file
- `--format FORMAT` - Output format (json, yaml, text)
- `--detailed` - Show detailed benchmark information

**Benchmark Types:**
- `neuron` - Neuron performance benchmark
- `synapse` - Synapse transmission benchmark
- `plasticity` - Plasticity rule performance benchmark
- `simulation` - Full simulation performance benchmark
- `memory` - Memory usage benchmark
- `all` - Run all benchmarks (default)

**Examples:**
```bash
# Run all benchmarks
nlm_cli benchmark --duration 120 --neuron-count 500 --output benchmark_results.json

# Detailed neuron benchmark
nlm_cli benchmark neuron --duration 30 --detailed

# Parallel benchmark
nlm_cli benchmark --parallel --iterations 20
```

#### `info` Command

Shows system and configuration information.

**Usage:**
```bash
nlm_cli info [OPTIONS]
```

**Options:**
- `--full` - Show full system information
- `--summary` - Show summary information only

**Examples:**
```bash
# Show summary information
nlm_cli info --summary

# Show full system information
nlm_cli info --full
```

#### `export` Command

Exports data in various formats.

**Usage:**
```bash
nlm_cli export [OPTIONS] [FORMAT] [OUTPUT_FILE]
```

**Options:**
- `--format FORMAT` - Output format (json, yaml, csv, xml)
- `--output FILE` - Output file (console if omitted)
- `--neurons` - Export neuron data
- `--synapses` - Export synapse data
- `--spikes` - Export spike data
- `--activity` - Export neural activity data
- `--weights` - Export synaptic weights

**Examples:**
```bash
# Export neuron data as JSON
nlm_cli export --format json --output neurons.json --neurons

# Export spike data as CSV
nlm_cli export --format csv --output spikes.csv --spikes

# Export all data
nlm_cli export --format yaml --output full_data.yaml --neurons --synapses --spikes --activity --weights
```

### General Options

- `--help, -h` - Show help message
- `--version, -v` - Show version information
- `--config FILE, -c FILE` - Load configuration from file
- `--output FILE, -o FILE` - Output results to file
- `--format FORMAT` - Output format (text, json, yaml, csv)
- `--verbose, -V` - Enable verbose logging
- `--quiet, -q` - Suppress output

## Configuration Files

The CLI tool supports three configuration file formats:

### JSON Configuration

```json
{
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "debug": true,
    "plasticity": {
        "stdp_enabled": true,
        "hebbian_enabled": true,
        "structural_enabled": true
    }
}
```

### YAML Configuration

```yaml
# NLM Configuration
simulation_timestep: 0.001
neuron_count: 500
region_count: 1
connection_probability: 0.15
debug: true
plasticity:
    stdp_enabled: true
    hebbian_enabled: true
    structural_enabled: true
```

### Key-Value Configuration

```
# NLM Configuration
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
debug = true
plasticity.stdp_enabled = true
plasticity.hebbian_enabled = true
plasticity.structural_enabled = true
```

## Output Formats

The CLI tool supports the following output formats:

### Text Format

Human-readable output with clear formatting:

```
Simulation Results:
  Duration: 1.234 seconds
  Steps: 1000
  Total Spikes: 567
  Neurons: 500
  Synapses: 12500
  Final Weight Mean: 0.452
  Final Weight StdDev: 0.123
  Most Active Neurons: [1, 5, 10, 15, 20]
```

### JSON Format

Machine-readable format:

```json
{
  "simulation_time": 1.234,
  "total_steps": 1000,
  "total_spikes": 567,
  "total_neurons": 500,
  "total_synapses": 12500,
  "final_weight_mean": 0.452,
  "final_weight_stddev": 0.123,
  "most_active_neurons": [1, 5, 10, 15, 20]
}
```

### YAML Format

Structured data format:

```yaml
duration: 1.234
simulation_time: 1.234
total_steps: 1000
total_spikes: 567
total_neurons: 500
total_synapses: 12500
final_weight_mean: 0.452
final_weight_stddev: 0.123
most_active_neurons:
  - 1
  - 5
  - 10
  - 15
  - 20
```

### CSV Format

Tabular data format:

```csv
key,value
duration,1.234
total_steps,1000
total_spikes,567
total_neurons,500
total_synapses,12500
final_weight_mean,0.452
final_weight_stddev,0.123
```

## Advanced Features

### Progress Reporting

The CLI tool includes visual progress reporting for long-running operations:

```
[===========>                       ] 42.5% 420/1000 ETA: 45.2s Processing simulation data...
```

### Parallel Processing

Enable parallel processing for multiple simulations:

```bash
nlm_cli simulate --mode basic --steps 1000 --neuron-count 500 --parallel --batch-size 10
```

### Checkpointing

Save simulation checkpoints at regular intervals:

```bash
nlm_cli simulate --mode plasticity --steps 5000 --checkpoint 100 --save checkpoint.json
```

### Configuration Validation

Validate configurations before running simulations:

```bash
nlm_cli config --load invalid_config.yaml --validate
```

## Integration with Existing NLM

The CLI tool maintains full compatibility with the existing NLM implementation:

- All existing functionality from `src/main.cpp` is preserved
- Configuration files are compatible with the original Config class
- Simulation modes use the same underlying neural network components
- Output formats provide comprehensive data for analysis

To run the original NLM simulation (without CLI), simply execute:

```bash
./nlm
```

## Performance Tips

1. **Use configuration files** for complex simulations to avoid command-line argument limits
2. **Enable parallel processing** for large-scale simulations
3. **Use checkpointing** for long-running simulations to prevent data loss
4. **Monitor progress** with the `--verbose` flag for real-time feedback
5. **Use efficient output formats** (JSON, YAML) for programmatic analysis

## Troubleshooting

### Common Issues

1. **File not found**: Ensure configuration and output files exist
2. **Permission denied**: Check file permissions
3. **Invalid format**: Use the correct format specification for `--format` option
4. **Memory issues**: Reduce simulation complexity or increase system resources

### Getting Help

- `nlm_cli --help` - Show general help
- `nlm_cli simulate --help` - Show simulate command help
- `nlm_cli config --help` - Show config command help

## Version Information

- **Version**: 2.0.0
- **Build Date**: $(date)
- **Compiler**: $(CXX)
- **Libraries**: NLM Core, Boost (if used)

## License

This software is licensed under the NLM License. See the LICENSE file for details.