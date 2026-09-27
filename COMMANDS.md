// COMMANDS.md - Project Command Line Interface Documentation

# NLM Command Line Interface

## Overview
NLM (熙然) provides a comprehensive command-line interface for interacting with the neural learning machine. The CLI supports multiple phases of operation, configuration management, and experimental control.

## Available Commands

### Basic Operations

#### `nlm`
Primary executable that runs Phase 2 basic neural computation demo.

**Arguments:**
- `--config <file>`: Load configuration from file (default: configs/default.cfg)
- `--neuron-count <n>`: Override neuron count
- `--random-seed <n>`: Set random seed
- `--help`: Show help information

**Example:**
```bash
./nlm --config my_config.cfg --neuron-count 1000 --random-seed 42
```

#### `nlm_phase3_demo`
Phase 3 demonstration showing world interaction, action selection, and learning.

**Arguments:**
- `num_episodes`: Number of episodes to run (default: 5)
- `steps_per_episode`: Steps per episode (default: 200)

**Example:**
```bash
./nlm_phase3_demo 10 500
```

#### `nlm_phase4_demo`
Phase 4 demonstration of emerging cognition and integrated experiments.

**Arguments:**
- `num_trials`: Number of trials for each experiment (default: 50)

**Example:**
```bash
./nlm_phase4_demo 100
```

#### `nlm_phase6_demo`
Phase 6 integration test demonstrating all brain systems working together.

**Arguments:**
- None (uses default configuration)

**Example:**
```bash
./nlm_phase6_demo
```

### Configuration Management

#### `nlm-config --help`
Shows configuration options and file locations.

#### `nlm-config --list`
Lists all available configuration options with current values.

#### `nlm-config --validate <file>`
Validates a configuration file syntax and structure.

#### `nlm-config --export <format>`
Exports current configuration to specified format (json, yaml, ini).

### Development & Debugging

#### `nlm-test --help`
Shows all available tests and test options.

#### `nlm-test --run <test_name>`
Runs a specific test.

#### `nlm-test --run-all`
Runs all available tests.

#### `nlm-test --benchmark`
Runs performance benchmarks on core neural systems.

#### `nlm-debug --start`
Starts an interactive debug session with live visualization.

### Experiment Management

#### `nlm-experiment --help`
Shows experiment management options.

#### `nlm-experiment --create <name>`
Creates a new experiment configuration.

#### `nlm-experiment --run <name> <config>`
Runs a named experiment with specified configuration.

#### `nlm-experiment --list`
Lists all available experiments.

#### `nlm-experiment --results <name>`
Shows results from a completed experiment.

### Performance & Monitoring

#### `nlm-monitor --start`
Starts real-time monitoring of neural activity.

#### `nlm-monitor --stats`
Shows current system statistics.

#### `nlm-monitor --plot <metric>`
Generates plots of specified metric.

#### `nlm-profile --help`
Shows profiling options.

#### `nlm-profile --cpu`
Profile CPU usage of neural simulation.

#### `nlm-profile --memory`
Profile memory usage of neural systems.

#### `nlm-profile --time`
Profile execution time of components.

### Administration

#### `nlm-admin --help`
Shows administrative commands.

#### `nlm-admin --backup <path>`
Creates a backup of current brain state.

#### `nlm-admin --restore <path>`
Restores brain state from backup.

#### `nlm-admin --checkpoint <path>`
Manually creates a checkpoint at specified path.

#### `nlm-admin --status`
Shows system status and resource usage.

#### `nlm-admin --logs --view [--lines N]`
View recent log entries.

### Advanced Features

#### `nlm-advanced --help`
Shows advanced command options.

#### `nlm-advanced --plugin <name>`
Loads and runs a plugin.

#### `nlm-advanced --script <file>`
Runs a custom script with extended functionality.

#### `nlm-advanced --batch <file>`
Runs a batch of commands from file.

#### `nlm-advanced --export-model <path>`
Exports neural model to external format.

#### `nlm-advanced --import-model <path>`
Imports neural model from external format.

## Configuration File Format

### Basic Configuration (INI format)
```ini
[Neural]
neuron_count = 1000
region_count = 1
connection_probability = 0.1

simulation_timestep = 0.001
random_seed = 42

[Plasticity]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[Memory]
working_memory_capacity = 1000
episodic_memory_capacity = 1000

[Neuromodulation]
dopamine_sensitivity = 1.0
curiosity_threshold = 0.5
novelty_sensitivity = 1.0
```

### Command Line Override Priority (highest to lowest)
1. Command line arguments
2. Configuration file
3. Default values

## Scripting & Automation

### Batch Scripts
NLM supports batch execution via script files:

```bash
#!/bin/bash
# experiment.sh
./nlm --config config1.cfg
./nlm_phase3_demo 5 100
./nlm_phase4_demo 50
./nlm_phase6_demo
```

### Environment Variables
```bash
export NLM_CONFIG_PATH="configs/custom.cfg"
export NLM_LOG_LEVEL="DEBUG"
export NLM_PERFORMANCE_MODE="OPTIMIZED"
```

## Examples

### Running a Complete Experiment
```bash
#!/bin/bash
# run_complete_experiment.sh

echo "=== Starting NLM Complete Experiment ==="

# Phase 2: Basic neural computation
./nlm --config configs/experiment.cfg

# Phase 3: World interaction
./nlm_phase3_demo 10 500

# Phase 4: Cognition demo
./nlm_phase4_demo 100

# Phase 6: Final integration
./nlm_phase6_demo

echo "=== Experiment Complete ==="
```

### Automated Testing
```bash
#!/bin/bash
# run_tests.sh

echo "Running NLM unit tests..."
nlm-test --run-all

echo "Running integration tests..."
./nlm_phase6_demo

echo "Running performance benchmarks..."
nlm-test --benchmark
```

### Configuration Management
```bash
#!/bin/bash
# manage_config.sh

# Validate configuration
nlm-config --validate configs/production.cfg

# Export current config
nlm-config --export json > config_export.json

# Load and test new config
nlm --config configs/production.cfg

# Backup current state
nlm-admin --backup backup_$(date +%Y%m%d_%H%M%S).bin
```

## Help & Support

- `--help` flag available for all commands
- Detailed man pages available at docs/
- Online documentation at https://nlm.ai/docs
- Support: support@nlm.ai
- GitHub repository: https://github.com/nicholas-lemarchand/nlm

## Notes

- All file paths are relative to current working directory
- Configuration files in configs/ directory are automatically discovered
- Logs are written to logs/ directory with timestamped filenames
- Checkpoints can be saved to any accessible directory