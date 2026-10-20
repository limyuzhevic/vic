# NLM CLI - Command Examples

This document provides detailed examples of common workflows using the NLM Advanced CLI.

## Basic Brain Operations

### Create and Simulate a Brain

```bash
# Create a brain with 1000 neurons and default timestep
nlm brain create --neuron-count 1000 nlm brain simulate --steps 100

# Save output to JSON file
nlm brain create --neuron-count 1000 nlm brain simulate --steps 100 \
  --output-format json --output-file brain_simulation.json
```

### Create Brain with Custom Configuration

```bash
# Create brain with custom config file
nlm --config-file my_config.cfg brain create --neuron-count 2000 nlm brain simulate

# Inspect brain properties
nlm brain create --neuron-count 1000 nlm brain inspect
```

## World Operations

### Create and Configure a World

```bash
# Create world with custom dimensions
nlm world create --width 200 --height 200 --vision-width 64 --vision-height 64

# Configure world parameters
nlm world configure --max-energy 200.0 --energy-decay 0.05

# Run world simulation
nlm world simulate --steps 500
```

### Export World Data

```bash
# Export world data for external analysis
nlm world simulate nlm world export --format json --output world_data.json
```

## Agent Operations

### Train and Evaluate an Agent

```bash
# Create agent and train it
nlm agent create nlm agent train --episodes 1000 --learning-rate 0.01 nlm agent evaluate --test-episodes 50

# Run exploration with curiosity
nlm agent explore --exploration-steps 500
```

### Analyze Agent Behavior

```bash
# Analyze agent learning metrics
nlm agent create --training-episodes 200 nlm agent analyze

# Get performance statistics
nlm agent create --training-episodes 200 nlm agent analyze \
  --output-format csv --output-file agent_stats.csv
```

## Configuration Operations

### Configuration Management

```bash
# Create a new configuration
nlm config create --name "production" --neuron-count 5000

# Load configuration
nlm config load --file production.cfg

# Validate configuration
nlm config validate --file test.cfg

# Compare two configurations
nlm config compare --base base.cfg --new new.cfg

# Migrate configuration between versions
nlm config migrate --from 0.1 --to 0.2 --file old.cfg
```

## Experiment Operations

### Run Experiments

```bash
# Run a simple experiment
nlm experiment run --name "basic_test" --description "Test basic brain functionality" nlm brain simulate

# List available experiments
nlm experiment list

# Create new experiment
nlm experiment create --name "plasticity_test" --description "Test synaptic plasticity"

# Analyze experiment results
nlm experiment analyze --output-format json --output-file experiment_results.json
```

## Profile Operations

### Performance Profiling

```bash
# Start performance profiling
nlm profile start

# Run operations with profiling
nlm brain simulate --steps 1000 nlm agent train --episodes 200

# Stop profiling and analyze
nlm profile stop nlm profile analyze

# Generate performance report
nlm profile report --output-format html --output-file performance_report.html
```

## Advanced Options

### Output Formats

```bash
# Output results in different formats
nlm brain simulate --steps 100 --output-format json --output-file results.json
nlm brain simulate --steps 100 --output-format table
nlm brain simulate --steps 100 --output-format csv --output-file results.csv
```

### Dry Run Mode

```bash
# Preview operations without executing
nlm brain create --neuron-count 1000 --dry-run

# Dry run with output file preview
nlm brain create --neuron-count 1000 --output-file test.json --dry-run
```

### Force Overwrite

```bash
# Force overwrite existing output files
nlm brain simulate --steps 100 --output-file existing.json --force
```

## Interactive Mode

### Using the REPL

```bash
# Start interactive CLI with full functionality
nlm

# In interactive mode, you can:
# - Run commands directly: brain create --neuron-count 1000
# - Use command history with up/down arrows
# - Get auto-completion with Tab
# - Use !prefix to run shell commands
# - Exit with exit, quit, or Ctrl+D
```

### Script Mode

```bash
# Run a sequence of commands from a script
#!/bin/bash
nlm brain create --neuron-count 1000 nlm brain simulate --steps 100 \
  --output-format json > simulation_output.json

# Or run commands from a file
nlm --script operations.txt

# Script file example (operations.txt):
# brain create --neuron-count 500
# world create --width 100 --height 100
# agent create --training-episodes 100
# experiment run --name "batch_test"
```

## Integration Examples

### Remote Execution

```bash
# Run on remote server (requires SSH access)
nlm remote --host nlm-server "brain create --neuron-count 1000"

# Submit job to queue system
nlm submit --job-file brain_job.json
```

### API Server

```bash
# Start API server
nlm api-server --host 0.0.0.0 --port 8080

# Query via HTTP
curl -X POST http://localhost:8080/api/brain/create \
  -H "Content-Type: application/json" \
  -d '{"neuron_count": 1000}'
```

### Web Dashboard Integration

```bash
# Start web dashboard with live monitoring
nlm dashboard --port 8080 --dashboard-url http://localhost:3000

# Connect to dashboard for real-time visualization
nlm dashboard connect --id brain_simulation_123
```

## Troubleshooting

### Common Issues

1. **pynlm bindings not available**
   ```bash
   # Install dependencies
   pip install -e .
   ```

2. **Configuration file not found**
   ```bash
   # Use absolute path or copy to current directory
   cp /path/to/config.cfg ./my_config.cfg
   nlm --config-file my_config.cfg brain create --neuron-count 1000
   ```

3. **Permission denied**
   ```bash
   # Ensure write permissions
   chmod 755 ./output.json
   ```

4. **Memory issues**
   ```bash
   # Reduce neuron count or simulation steps
   nlm brain create --neuron-count 500 nlm brain simulate --steps 50
   ```

### Getting Help

```bash
# Show help for specific commands
nlm brain --help
nlm brain create --help
nlm brain simulate --help

# Show all available commands
nlm --help

# Get usage examples for specific commands
nlm brain create --examples
nlm agent train --examples
```

## Configuration File Examples

### Basic Configuration (basic.cfg)

```ini
[general]
random_seed = 42
simulation_timestep = 0.001

[neuron]
neuron_count = 1000
region_count = 2
connection_probability = 0.1

[plasticity]
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

[environment]
environment_name = GridWorld
environment_width = 10
environment_height = 10

[logging]
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### Advanced Configuration (advanced.cfg)

```json
{
    "general": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "max_simulation_steps": 10000,
        "simulation_time_limit": 0.0
    },
    "neuron": {
        "neuron_count": 10000,
        "region_count": 3,
        "connection_probability": 0.15,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": true
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.5
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "development": {
        "initial_plasticity": 1.0,
        "plasticity_decay_rate": 0.95
    },
    "memory": {
        "working_capacity": 100,
        "episodic_capacity": 1000
    },
    "environment": {
        "name": "ComplexWorld",
        "width": 50,
        "height": 50,
        "vision_width": 16,
        "vision_height": 16,
        "max_energy": 100.0,
        "energy_decay_rate": 0.1
    },
    "logging": {
        "level": "INFO",
        "to_file": true,
        "filename": "nlm_advanced.log"
    }
}
```
