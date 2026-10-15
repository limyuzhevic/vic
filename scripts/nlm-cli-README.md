# NLM Command Line Interface

The NLM Command Line Interface (CLI) provides a powerful and user-friendly way to interact with the Neural Learning Machine. It supports subcommands for brain simulation, configuration management, experiments, benchmarking, checkpoint management, and utilities.

## Quick Start

### Installation

1. Ensure NLM is installed:
   ```bash
   cd /workspace/app
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. Install the CLI script:
   ```bash
   chmod +x /workspace/app/scripts/nlm-cli.sh
   ```

3. Add NLM to PATH (optional):
   ```bash
   echo 'export PATH="/workspace/app/scripts:$PATH"' >> ~/.bashrc
   source ~/.bashrc
   ```

### Basic Usage

```bash
# Show help
nlm help

# Run basic simulation
nlm run --neurons 1000 --steps 1000

# Create a new configuration
nlm config create --name myconfig

# Run an experiment
nlm experiment run --type learning --steps 1000

# Run benchmarks
nlm benchmark --type performance --duration 60

# Save checkpoint
nlm checkpoint save --name current

# Export brain state
nlm export --format json --output brain_state.json
```

## Commands

### Run Command

Run the brain simulation with customizable parameters.

#### Options:

- `--neurons N` - Number of neurons (default: 1000)
- `--regions R` - Number of regions (default: 1)
- `--steps S` - Number of simulation steps (default: 1000)
- `--config FILE` - Configuration file (default: configs/default.cfg)
- `--output FILE` - Output file for results
- `--verbose` - Enable verbose output
- `--quiet` - Suppress non-essential output
- `--benchmark` - Run benchmark mode
- `--checkpoint` - Enable checkpointing

#### Examples:

```bash
# Basic simulation
nlm run --neurons 2000 --steps 5000 --verbose

# With custom configuration
nlm run --config myconfig.cfg --output results.txt

# Benchmark mode
nlm run --benchmark --duration 60
```

### Config Command

Manage NLM configurations.

#### Subcommands:

- `create` - Create a new configuration
- `list` - List all configurations
- `show` - Show configuration details
- `edit` - Edit configuration
- `delete` - Delete configuration
- `export` - Export configuration
- `import` - Import configuration

#### Examples:

```bash
# Create a new configuration
nlm config create --name myconfig

# List all configurations
nlm config list

# Show configuration details
nlm config show --name myconfig

# Export configuration as JSON
nlm config export --name myconfig --format json
```

### Experiment Command

Run and manage experiments.

#### Subcommands:

- `run` - Run a specific experiment
- `list` - List available experiments
- `create` - Create a new experiment
- `analyze` - Analyze experiment results

#### Examples:

```bash
# Run a learning experiment
nlm experiment run --type learning --steps 1000

# Run a plasticity experiment
nlm experiment run --type plasticity --neurons 500

# List available experiments
nlm experiment list
```

### Benchmark Command

Run performance benchmarks.

#### Options:

- `--type TYPE` - Benchmark type (performance, memory, spike, plasticity)
- `--duration SEC` - Duration in seconds (default: 60)
- `--steps S` - Number of steps (default: 10000)
- `--output FILE` - Output file for benchmark results
- `--verbose` - Enable detailed output

#### Examples:

```bash
# Performance benchmark
nlm benchmark --type performance --duration 120

# Spike rate benchmark
nlm benchmark --type spike --steps 5000 --output results.csv
```

### Checkpoint Command

Manage brain state checkpoints.

#### Subcommands:

- `save` - Save current state as checkpoint
- `load` - Load from checkpoint
- `list` - List available checkpoints
- `delete` - Delete checkpoints
- `prune` - Remove old checkpoints

#### Examples:

```bash
# Save current state
nlm checkpoint save --name current

# Load from checkpoint
nlm checkpoint load --name backup1

# List checkpoints
nlm checkpoint list
```

### Export/Import Command

Export or import brain state in various formats.

#### Options:

- `--format FORMAT` - Export format (json, yaml, txt, binary)
- `--output FILE` - Output file
- `--neurons` - Include neuron states
- `--synapses` - Include synapse states
- `--connections` - Include connection information
- `--config` - Include configuration

#### Examples:

```bash
# Export as JSON
nlm export --format json --output brain_state.json

# Export with all components
nlm export --format binary --output brain_state.bin --neurons --synapses --connections --config
```

### Utils Command

Utility functions for analysis and management.

#### Subcommands:

- `analyze` - Analyze brain state and performance
- `stats` - Get statistics about the current state
- `visualize` - Generate visualizations
- `profile` - Profile performance
- `validate` - Validate configuration

#### Examples:

```bash
# Analyze results
nlm utils analyze --file results.txt

# Get statistics in JSON format
nlm utils stats --format json

# Profile performance
nlm utils profile --duration 30
```

## Configuration Files

### Default Configuration

The default configuration file (`configs/default.cfg`) contains settings for running the NLM brain simulation:

```bash
# NLM Configuration File
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
random_seed = 42
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
attention_inhibition_strength = 0.5
attention_excitation_strength = 1.5
replay_interval = 100
consolidation_interval = 1000
checkpoint_dir = ./checkpoints
checkpoint_interval = 10000
checkpoint_enabled = true
```

### Creating Custom Configurations

1. Create a new config file:
   ```bash
   nlm config create --name myconfig --neurons 2000 --regions 3 --timestep 0.0005
   ```

2. Edit an existing config:
   ```bash
   nlm config edit --name myconfig --set neuron_count=3000
   ```

3. Export a config:
   ```bash
   nlm config export --name myconfig --format json > myconfig.json
   ```

4. Import a config:
   ```bash
   nlm config import --format json --input myconfig.json
   ```

## Advanced Usage

### Running with Specific Configuration

```bash
nlm run --config myconfig.cfg --output results.txt --verbose
```

### Running with Environment Variables

```bash
export NLM_CONFIG_PATH="~/myconfigs"
export NLM_LOG_LEVEL="debug"
nlm run --benchmark
```

### Batch Processing

```bash
#!/bin/bash

# Process multiple configurations
for config in configs/*.cfg; do
    echo "Processing $config"
    nlm run --config "$config" --steps 1000 --output "results/$(basename $config .cfg).txt"
done
```

### Scripting

```bash
# Create a simple script for running experiments
#!/usr/bin/env bash

experiment_id=$1
neurons=$2
steps=$3

nlm experiment run --type learning --steps "$steps" --neurons "$neurons" \
    --output "experiments/$experiment_id/results.json"

# Analyze results
nlm utils analyze --file "experiments/$experiment_id/results.json"
```

## Troubleshooting

### Common Issues

1. **Command not found**: Ensure the CLI script is in your PATH
   ```bash
   which nlm
   ```

2. **Configuration not found**: Use absolute paths or ensure config directory exists
   ```bash
   nlm run --config /path/to/config.cfg
   ```

3. **Permission denied**: Make the CLI script executable
   ```bash
   chmod +x /workspace/app/scripts/nlm-cli.sh
   ```

4. **Missing dependencies**: Ensure all NLM dependencies are installed
   ```bash
   ./build/nlm --help
   ```

### Getting Help

- For general help: `nlm help`
- For command-specific help: `nlm help <command>`
- For option-specific help: `nlm run --help` (if implemented)

### Logging

The CLI supports different log levels:

```bash
export NLM_LOG_LEVEL=debug
nlm run --verbose --benchmark
```

## Configuration Schema

The NLM configuration system supports the following configuration options:

- `neuron_count` (integer): Number of neurons
- `region_count` (integer): Number of brain regions
- `simulation_timestep` (double): Simulation timestep in seconds
- `random_seed` (integer): Random seed for reproducibility
- `connection_probability` (float): Probability of connection between neurons
- `stdp_ltp_weight` (float): STDP LTP weight
- `stdp_ltd_weight` (float): STDP LTD weight
- `stdp_tau` (float): STDP time constant
- `synaptogenesis_rate` (float): Rate of new synapse formation
- `pruning_rate` (float): Rate of synapse pruning
- `working_memory_capacity` (integer): Working memory capacity
- `episodic_memory_max_episodes` (integer): Maximum episodic memory episodes
- `attention_inhibition_strength` (float): Attention inhibition strength
- `attention_excitation_strength` (float): Attention excitation strength
- `replay_interval` (integer): Memory replay interval
- `consolidation_interval` (integer): Memory consolidation interval
- `checkpoint_dir` (string): Checkpoint directory
- `checkpoint_interval` (integer): Checkpoint interval
- `checkpoint_enabled` (boolean): Whether checkpoints are enabled

## Development

### Adding New Commands

To add a new command, follow this pattern:

1. Add a new case in the main command dispatcher
2. Implement the command function
3. Add help documentation
4. Test the command

### Extending Configuration

To add new configuration options:

1. Update the configuration schema in the documentation
2. Add support in the configuration parser
3. Update the default configuration file
4. Add configuration validation

### Plugin System

The NLM CLI supports a plugin system for extending functionality:

1. Create plugin scripts in `~/.nlm/plugins/`
2. Add plugin configuration in `~/.nlm/config.yaml`
3. Plugins can add new commands and extend existing functionality

## License

This software is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

- NLM Project Team
- Contributors to the Neural Learning Machine project
- Users who provide feedback and report issues

## Contact

For questions, issues, or feature requests:
- GitHub: https://github.com/nlm-project/nlm
- Email: contact@nlm-project.org
- Website: https://nlm-project.org

## Version

Version: 0.1.0
Build: $(date +%Y-%m-%d)
Compiler: $(g++ --version | head -1)